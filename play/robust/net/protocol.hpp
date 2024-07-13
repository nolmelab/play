#pragma once

#include <functional>
#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

// type used to mark empty topic (topic not used)
struct empty_topic
{
};

template <typename Topic>
class protocol
{
public:
  using topic = Topic;

  // 프로토콜을 위한 함수들 제공
  struct adapter
  {
    // send function provided by session
    using send_fn = std::function<void(const void* data, size_t len)>;
    // forward function provided by session to pass to session_handler
    using forward_fn = std::function<void(Topic topic, const void* data, size_t len)>;
    // state listener
    using accepted_fn = std::function<void(size_t handle)>;
    using connected_fn = std::function<void(size_t handle)>;
    using closed_fn = std::function<void(size_t handle)>;
    using established_fn = std::function<void(size_t handle)>;

    send_fn send_;
    forward_fn forward_;
    accepted_fn accepted_;
    connected_fn connected_;
    closed_fn closed_;
    established_fn established_;
  };

public:
  protocol(adapter adapter) : adapter_{adapter} {}

  // 비트 연산을 제공하는 값 타잎들에 대해 직렬화
  template <typename Value>
  void serialize(asio::mutable_buffer& mbuf, Value v);

  void serialize(asio::mutable_buffer& mbuf, bool v);

  // 비트 연산을 제공하는 값 타잎들에 대해 역직렬화
  template <typename Value>
  void deserialize(const asio::const_buffer& cbuf, Value& v);

  void deserialize(const asio::const_buffer& cbuf, bool& v);

protected:
  adapter& get_adapter() const { return adapter_; }

private:
  adapter adapter_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/protocol.ipp>