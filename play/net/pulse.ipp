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
  }
}

template <typename Protocol, typename Frame>
template <typename CompletionToken>
void pulse<Protocol, Frame>::post(CompletionToken&& handler)
{
  auto runner = get_runner();
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
  auto pic = static_cast<topic>(topic_in);

  // add
  {
    std::unique_lock guard(mutex_);
    auto iter = subscriptions_.find(pic);
    if (iter == subscriptions_.end())
    {
      std::vector<subscription> subs{};
      auto result = subscriptions_.insert(std::pair{pic, subs});
      PLAY_CHECK(result.second);
      iter = result.first;
    }
    iter->second.push_back({0, cb});
  }

  if (!is_root())
  {
    auto skey = reinterpret_cast<uintptr_t>(session_.get());
    get_root()->show_interest(this, skey, pic);
  }

  return *this;
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
void pulse<Protocol, Frame>::bind_child(pulse* child)
{
  PLAY_CHECK(child != nullptr);
  auto key = reinterpret_cast<uintptr_t>(child);

  std::unique_lock guard(mutex_);
  childs_.insert(std::pair{key, child});
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::unbind_child(pulse* child)
{
  PLAY_CHECK(child != nullptr);
  auto key = reinterpret_cast<uintptr_t>(child);

  // lose interests
  {
    std::unique_lock guard(mutex_);
    for (auto& kv : interests_)
    {
      auto& vec = kv.second;
      vec.erase(std::remove_if(vec.begin(), vec.end(),
                               [child](pulse* p)
                               {
                                 return p == child;
                               }),
                vec.end());
    }
  }

  // unbind
  {
    std::unique_lock guard(mutex_);
    childs_.erase(key);
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::show_interest(pulse* child, uintptr_t skey, topic pic)
{
  PLAY_CHECK(is_root());

  // assert: chld는 자식 중에 포함되어야 함
  auto key = interest_key{skey, pic};

  std::unique_lock guard(mutex_);
  auto iter = interests_.find(key);
  if (iter == interests_.end())
  {
    std::vector<pulse*> vec;
    iter = interests_.insert(std::pair{key, vec}).first;
  }
  // assert: iter->second에 동일한 child가 없어야 함
  iter->second.push_back(child);
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::lose_interest(pulse* child, uintptr_t skey, topic pic)
{
  PLAY_CHECK(is_root());

  auto key = interest_key{skey, pic};

  std::unique_lock guard(mutex_);
  auto iter = interests_.find(key);
  if (iter != interests_.end())
  {
    auto& vec = iter->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [child](pulse* p)
                             {
                               return p == child;
                             }),
              vec.end());
  }
}

template <typename Protocol, typename Frame>
void pulse<Protocol, Frame>::dispatch(session_ptr se, topic pic, frame_ptr frame)
{
  // 구독자들에게 전달
  {
    std::shared_lock guard(mutex_);
    auto siter = subscriptions_.find(pic);
    if (siter != subscriptions_.end())
    {
      for (auto& sub : siter->second)
      {
        if (strand_key_ == 0)
        {
          sub.cb(se, frame);
        }
        else
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
    else
    {
      if (childs_.empty())
      {
        LOG()->debug("sub for topic: {} not found", pic);
      }
    }
  }

  // 루트일 경우만 자식들에게 전달
  if (is_root())
  {
    std::shared_lock guard(mutex_);
    // topic only interest group
    {
      auto key = interest_key{0, pic};
      auto iter = interests_.find(key);
      if (iter != interests_.end())
      {
        auto& vec = iter->second;
        for (auto& child : vec)
        {
          child->dispatch(se, pic, frame);
        }
      }
    }

    // <session, topic> interest group
    {
      auto skey = reinterpret_cast<uintptr_t>(se.get());
      auto key = interest_key{skey, pic};
      auto iter = interests_.find(key);
      if (iter != interests_.end())
      {
        auto& vec = iter->second;
        for (auto& child : vec)
        {
          child->dispatch(se, pic, frame);
        }
      }
    }
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