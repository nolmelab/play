#include <play/net/pulse.hpp>
#include <play/net/util/address.hpp>

namespace play {

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>::~pulse()
{
  stop();
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_server(uint16_t port)
{
  mode_ = mode::server;
  port_ = port;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_client(const std::string& addr)
{
  mode_ = mode::client;
  addr_ = addr;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_child(pulse* parent)
{
  PLAY_CHECK(parent != nullptr);
  mode_ = mode::child;
  parent_ = parent;
  parent_->bind_child(this);
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_independent()
{
  mode_ = mode::independent;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::with_runner(runner* runner)
{
  PLAY_CHECK(runner != nullptr);
  runner_ = runner;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::with_strand(size_t key)
{
  strand_key_ = key;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::with_session(session_ptr ss)
{
  session_ = ss;
  return *this;
}

template <typename Protocol, typename Frame>
bool pulse<Protocol, Frame>::start()
{
  PLAY_CHECK(mode_ != mode::none);

  switch (mode_)
  {
    case mode::server:
    {
      if (runner_ == nullptr)
      {
        return false;
      }
      server_ = std::make_unique<server>(*runner_);
      server_->bind_pulse(this);
      return server_->start(port_);
    }
    break;
    case mode::client:
    {
      if (runner_ == nullptr)
      {
        return false;
      }
      client_ = std::make_unique<client>(*runner_);
      client_->bind_pulse(this);
      auto pv = address::parse(addr_);
      return client_->connect(pv.first, pv.second);
    }
    break;
    case mode::child:
    {
      return parent_ != nullptr;
    }
    break;
    case mode::independent:
    {
      return true;
    }
    break;
    case mode::none:
    {
      return false;
    }
    break;
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::stop()
{
  if (stop_)
  {
    return;
  }

  stop_ = true;

  switch (mode_)
  {
    case mode::server:
      server_->stop();
      break;
    case mode::client:
      client_->close();
      break;
    case mode::child:
      parent_->unbind_child(this);
      break;
    case mode::independent:
      break;
    case mode::none:
      LOG()->error("invalid mode while stopping pulse");
      break;
  }
}

template <typename Protocol, typename Frame>
template <typename CompletionToken>
void pulse<Protocol, Frame>::post(CompletionToken&& handler)
{
  auto runner = get_runner();
  PLAY_CHECK(runner);

  if (strand_key_ > 0)
  {
    return runner->post(strand_key_, std::forward<CompletionToken>(handler));
  }
  return runner->post(std::forward<CompletionToken>(handler));
}

template <typename Protocol, typename Frame>
template <typename CompletionToken>
timer::ref pulse<Protocol, Frame>::once(asio::chrono::milliseconds ms, CompletionToken&& handler)
{
  auto runner = get_runner();
  PLAY_CHECK(runner);

  if (strand_key_ > 0)
  {
    return runner->once(strand_key_, ms, std::forward<CompletionToken>(handler));
  }
  return runner->once(ms, std::forward<CompletionToken>(handler));
}

template <typename Protocol, typename Frame>
template <typename CompletionToken>
timer::ref pulse<Protocol, Frame>::repeat(asio::chrono::milliseconds ms, CompletionToken&& handler)
{
  auto runner = get_runner();
  PLAY_CHECK(runner);

  if (strand_key_ > 0)
  {
    return runner->repeat(strand_key_, ms, std::forward<CompletionToken>(handler));
  }
  return runner->repeat(ms, std::forward<CompletionToken>(handler));
}

template <typename Protocol, typename Frame>
template <typename TopicInput>
inline pulse<Protocol, Frame>& pulse<Protocol, Frame>::subscribe(TopicInput topic_in, receiver cb)
{
  PLAY_CHECK(mode_ != mode::none);
  auto pic = static_cast<topic>(topic_in);
  auto session_key = reinterpret_cast<uintptr_t>(session_.get());

  // add
  {
    std::unique_lock guard(sub_mutex_);
    auto iter = subscriptions_.find(pic);
    if (iter == subscriptions_.end())
    {
      std::vector<subscription> subs{};
      auto result = subscriptions_.insert(std::pair{pic, subs});
      PLAY_CHECK(result.second);
      iter = result.first;
    }
    iter->second.push_back({session_key, cb});
  }

  if (!is_root())
  {
    auto skey = reinterpret_cast<uintptr_t>(session_.get());
    get_root()->show_interest(this, skey, pic);
  }

  return *this;
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::publish(session_ptr se, topic pic, frame_ptr frame)
{
  dispatch(se, pic, frame);
}

template <typename Protocol, typename Frame>
inline void pulse<Protocol, Frame>::on_receive(session_ptr se, topic pic, const void* data,
                                               size_t len)
{
  PLAY_CHECK(pic != topic_estalished);
  PLAY_CHECK(pic != topic_closed);

  auto frame_ptr = unpack(pic, reinterpret_cast<const uint8_t*>(data), len);
  if (!frame_ptr)
  {
    LOG()->error("frame unpack error. session: {} topic: {}", se->get_handle(), pic);
    return;
  }

  dispatch(se, pic, frame_ptr);
  call_receive(se, pic);
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::on_established(session_ptr se)
{
  dispatch(se, topic_estalished, frame_ptr{});
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::on_closed(session_ptr se, error_code ec)
{
  dispatch(se, topic_closed, frame_ptr{});

  // TODO: process call
  call_closed(se);
}

template <typename Protocol, typename Frame>
template <typename TopicInput>
void pulse<Protocol, Frame>::call(session_ptr se, TopicInput request, TopicInput response,
                                  call_receiver cb)
{
  auto skey = reinterpret_cast<uintptr_t>(se.get());

  // caller까지 추가
  {
    std::unique_lock guard(call_mutex_);

    auto iter = calls_.find(skey);
    if (iter == calls_.end())
    {
      // 없을 경우 추가
      session_calls scalls;
      iter = calls_.insert(std::pair{skey, scalls}).first;
    }
    auto& scalls = iter->second;

    auto req = static_cast<topic>(request);
    auto res = static_cast<topic>(response);
    auto topic_iter = scalls.find(req);
    if (topic_iter == scalls.end())
    {
      // 없을 경우 추가. 0으로 인덱스 시작
      topic_iter = scalls.insert(std::pair{req, res, 0, 0, {}}).first;
    }
    auto& tcalls = topic_iter->second;
    tcalls.call_index++;
    tcalls.insert(std::pair{tcalls.call_index, {tcalls.call_index, 0, cb}});

    // 페어 등록
    call_pairs_[res] = req;
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::call_closed(session_ptr se)
{
  auto skey = reinterpret_cast<uintptr_t>(se.get());

  std::vector<std::pair<size_t, call_receiver>> callbacks;

  // collect
  {
    std::unique_lock guard(call_mutex_);
    auto iter == calls_.find(skey);
    if (iter != calls_end())
    {
      auto& topic_calls = iter->second.calls;
      for (auto& kv : topic_calls)
      {
        auto& callers = kv.second.calls;
        for (auto& caller : callers)
        {
          if (strand_key_ == 0)
          {
            callbacks.push_back({0, caller.cb});
          }
          else
          {
            callbacks.push_back({strand_key_, caller.cb});
          }
        }
      }
    }
  }

  for (auto& cb : callbacks)
  {
    if (cb.first == 0)
    {
      cb.second();
    }
    else
    {
      auto cb = cb.second;
      this->get_runner()->post(strand_key_,
                               [cb]()
                               {
                                 cb();
                               });
    }
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::call_receive(session_ptr se, topic pic)
{
  auto skey = reinterpret_cast<uintptr_t>(se.get());

  std::shared_lock guard(call_mutex_);
  auto iter = calls_.find(skey);
  if (iter != calls_.end())
  {
    auto cp_iter = call_pairs_.find(pic);
    if (cp_iter != call_pairs_.end())
    {
      auto req = cp_iter->second;
      auto& scalls = iter->second;
      auto tcall_iter = scalls.calls.find(req);
      if (tcall_iter != scalls.calls.end())
      {
        auto& tcalls = tcall_iter->second;
        tcalls.recv_index++;

        auto caller_iter = tcalls.calls.find(tcalls.recv_index);
        PLAY_CHECK(caller_iter != tcalls.calls.end());
        PLAY_CHECK(caller_iter->second.call_id == tcalls.recv_index);

        tcalls.calls.erase(tcalls.recv_index);  // 여기서 지우면 됨
      }
    }
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::bind_child(pulse* child)
{
  PLAY_CHECK(child != nullptr);
  auto key = reinterpret_cast<uintptr_t>(child);

  std::unique_lock guard(sub_mutex_);
  childs_.insert(std::pair{key, child});
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::unbind_child(pulse* child)
{
  PLAY_CHECK(child != nullptr);
  auto key = reinterpret_cast<uintptr_t>(child);

  // lose interests
  {
    std::unique_lock guard(sub_mutex_);
    for (auto& kv : interests_)
    {
      auto& cmap = kv.second;
      cmap.erase(key);
    }
  }

  // unbind
  {
    std::unique_lock guard(sub_mutex_);
    childs_.erase(key);
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::show_interest(pulse* child, uintptr_t skey, topic pic)
{
  PLAY_CHECK(is_root());

  // assert: chld는 자식 중에 포함되어야 함
  auto key = interest_key{skey, pic};

  std::unique_lock guard(sub_mutex_);
  auto iter = interests_.find(key);
  if (iter == interests_.end())
  {
    child_map cmap;
    iter = interests_.insert(std::pair{key, cmap}).first;
  }
  auto ckey = reinterpret_cast<uintptr_t>(child);
  iter->second.insert(std::pair{ckey, child});
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::lose_interest(pulse* child, uintptr_t skey, topic pic)
{
  PLAY_CHECK(is_root());

  auto key = interest_key{skey, pic};
  auto ckey = reinterpret_cast<uintptr_t>(child);

  std::unique_lock guard(sub_mutex_);
  auto iter = interests_.find(key);
  if (iter != interests_.end())
  {
    auto& cmap = iter->second;
    cmap.erase(ckey);
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::dispatch(session_ptr se, topic pic, frame_ptr frame, bool from_root)
{
  bool handled = false;
  auto session_key = reinterpret_cast<uintptr_t>(se.get());

  // 구독자들에게 전달
  {
    std::shared_lock guard(sub_mutex_);
    auto siter = subscriptions_.find(pic);
    if (siter != subscriptions_.end())
    {
      handled = true;
      for (auto& sub : siter->second)
      {
        if (strand_key_ == 0)
        {
          if (is_target(sub.session_key, session_key, from_root))
          {
            sub.cb(se, frame);
          }
        }
        else
        {
          if (is_target(sub.session_key, session_key, from_root))
          {
            auto cb = sub.cb;
            this->get_runner()->post(strand_key_,
                                     [cb, se, frame]()
                                     {
                                       cb(se, frame);
                                     });
          }
        }
      }
    }
  }

  // 루트일 경우만 자식들에게 전달
  if (is_root())
  {
    std::shared_ptr<session> null_session;

    std::shared_lock guard(sub_mutex_);
    // topic only interest group
    {
      auto key = interest_key{0, pic};
      auto iter = interests_.find(key);
      if (iter != interests_.end())
      {
        handled = true;
        auto& cmap = iter->second;
        for (auto& kv : cmap)
        {
          kv.second->dispatch(null_session, pic, frame, true);
        }
      }
    }

    // <session, topic> interest group
    {
      if (se)  // se가 유효한 포인터일 경우. 0인 경우는 위에서 처리됨
      {
        auto skey = reinterpret_cast<uintptr_t>(se.get());
        auto key = interest_key{skey, pic};
        auto iter = interests_.find(key);
        if (iter != interests_.end())
        {
          handled = true;
          auto& cmap = iter->second;
          for (auto& kv : cmap)
          {
            kv.second->dispatch(se, pic, frame, true);
          }
        }
      }
    }
  }

  if (!handled)
  {
    LOG()->debug("topic: {} not handled", pic);
  }
}

template <typename Protocol, typename Frame>
bool pulse<Protocol, Frame>::is_target(uintptr_t sub_key, uintptr_t key, bool from_root) const
{
  if (from_root)
  {
    // 루트에서 호출할 경우 키가 같은 경우만 타겟
    return sub_key == key;
  }
  else
  {
    // 직접 호출할 경우 키가 0이거나 같은 경우에 타겟
    return sub_key == 0 || (sub_key == key);
  }
}

template <typename Protocol, typename Frame>
runner* pulse<Protocol, Frame>::get_runner()
{
  if (runner_ == nullptr)
  {
    if (parent_ != nullptr)
    {
      return parent_->get_runner();
    }
  }

  return runner_;
}

template <typename Protocol, typename Frame>
bool pulse<Protocol, Frame>::is_root() const
{
  return parent_ == nullptr;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>* pulse<Protocol, Frame>::get_root()
{
  if (is_root())
    return this;

  return parent_->get_root();
}

}  // namespace play