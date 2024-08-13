#include <play/net/pulse.hpp>
#include <play/net/util/address.hpp>

namespace play {

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>::~pulse()
{
  stop();
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_server(runner* runner, uint16_t port)
{
  PLAY_CHECK(runner != nullptr);
  runner_ = runner;
  mode_ = mode::server;
  port_ = port;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_client(runner* runner, const std::string& addr)
{
  PLAY_CHECK(runner != nullptr);
  runner_ = runner;
  mode_ = mode::client;
  addr_ = addr;
  return *this;
}

template <typename Protocol, typename Frame>
pulse<Protocol, Frame>& pulse<Protocol, Frame>::as_independent(runner* runner)
{
  PLAY_CHECK(runner != nullptr);
  runner_ = runner;
  mode_ = mode::independent;
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
  PLAY_CHECK(get_runner() != nullptr);

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
template <typename TopicInput>
bool pulse<Protocol, Frame>::has_subscription(TopicInput topic_in) const
{
  auto pic = static_cast<topic>(topic_in);

  std::shared_lock guard(sub_mutex_);
  auto iter = subscriptions_.find(pic);
  return iter != subscriptions_.end();
}

template <typename Protocol, typename Frame>
inline void pulse<Protocol, Frame>::publish(session_ptr se, topic pic, frame_ptr frame)
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
}

template <typename Protocol, typename Frame>
bool pulse<Protocol, Frame>::connect(std::string_view addr, uint16_t port)
{
  PLAY_CHECK(mode_ == mode::client);
  client_->connect(addr, port);
}

template <typename Protocol, typename Frame>
template <typename TopicInput>
void pulse<Protocol, Frame>::call(session_ptr se, TopicInput request, TopicInput response,
                                  call_receiver cb)
{
  PLAY_CHECK(mode_ == mode::child);
  PLAY_CHECK(!!session_);
  PLAY_CHECK(!is_root());

  auto res = static_cast<topic>(response);

  call_subscribe_closed();
  call_subscribe_reply(res);

  auto skey = reinterpret_cast<uintptr_t>(se.get());

  // caller까지 추가
  {
    std::unique_lock guard(call_mutex_);

    auto req = static_cast<topic>(request);
    auto res = static_cast<topic>(response);
    auto topic_call_iter = calls_.find(req);
    if (topic_call_iter == calls_.end())
    {
      topic_calls tcall;
      // 없을 경우 추가. 0으로 인덱스 시작
      topic_call_iter = calls_.insert(std::pair{req, tcall}).first;
    }
    auto& tcalls = topic_call_iter->second;
    tcalls.call_index++;
    tcalls.callers.push_back({tcalls.call_index, cb});

    // 페어 등록
    call_pairs_[res] = req;
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
void pulse<Protocol, Frame>::dispatch(session_ptr se, topic pic, frame_ptr frame, bool from_root)
{
  bool handled = false;
  auto session_key = reinterpret_cast<uintptr_t>(se.get());

  // 구독자들에게 전달. 모으고 보내는 게 가장 단순
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
            auto cb = sub.cb;
            this->get_runner()->post(
                [cb, se, frame]()
                {
                  cb(se, frame);
                });
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
inline void pulse<Protocol, Frame>::call_subscribe_reply(topic pic)
{
  PLAY_CHECK(!is_root());
  PLAY_CHECK(!!session_);

  if (!has_subscription(pic))
  {
    subscribe(pic,
              [this, pic](session_ptr se, frame_ptr)
              {
                call_receive(se, pic);
              });
  }
}

template <typename Protocol, typename Frame>
inline void pulse<Protocol, Frame>::call_subscribe_closed()
{
  PLAY_CHECK(!is_root());
  PLAY_CHECK(!!session_);
  if (!has_subscription(topic_closed))
  {
    // interest는 하나의 키(session, topic)에 대해 pulse 하나만 등록 가능
    // 구독은 여러 함수를 등록 가능하므로 모든 topic_closed 구독자들이 호출됨
    subscribe(topic_closed,
              [this](session_ptr se, frame_ptr)
              {
                call_closed(se);
              });
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::call_closed(session_ptr se)
{
  PLAY_CHECK(mode_ == mode::child);
  PLAY_CHECK(!is_root());
  PLAY_CHECK(!!session_);

  if (se != session_)
  {
    return;
  }

  // 서버간 단선은 재앙에 해당하므로 약간 무겁지만 괜찮다.

  auto skey = reinterpret_cast<uintptr_t>(se.get());

  std::vector<std::pair<size_t, call_receiver>> callbacks;

  // collect
  {
    std::unique_lock guard(call_mutex_);

    for (auto& kv : calls_)
    {
      auto& callers = kv.second.callers;
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
    calls_.clear();
  }

  for (auto& callback : callbacks)
  {
    if (callback.first == 0)
    {
      auto cb = callback.second;
      this->get_runner()->post(
          [cb]()
          {
            cb();
          });
    }
    else
    {
      auto cb = callback.second;
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
  PLAY_CHECK(mode_ == mode::child);
  PLAY_CHECK(!is_root());
  PLAY_CHECK(!!session_);

  auto skey = reinterpret_cast<uintptr_t>(se.get());

  std::unique_lock guard(call_mutex_);

  auto cp_iter = call_pairs_.find(pic);
  if (cp_iter != call_pairs_.end())
  {
    auto req = cp_iter->second;
    auto topic_call_iter = calls_.find(req);
    if (topic_call_iter != calls_.end())
    {
      auto& tcalls = topic_call_iter->second;
      tcalls.reply_index++;

      PLAY_CHECK(!tcalls.callers.empty());
      auto& matching_call = tcalls.callers.front();
      PLAY_CHECK(matching_call.call_id == tcalls.reply_index);
      tcalls.callers.pop_front();
    }
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
    PLAY_CHECK(parent_ != nullptr);
    return parent_->get_runner();
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