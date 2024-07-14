#pragma once

#include <functional>
#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

// type used to mark empty topic (topic not used)
struct empty_topic
{
};

// 프로토콜을 위한 함수들 제공
/**
 * duck typing을 사용하므로 동일한 함수 호출만 제공 가능하면 adapter가 됨
 * std::function, 함수 포인터, 멤버 함수로 제공 가능.
 */
template <typename Topic>
struct protocol_default_adapter
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

  send_fn send;
  forward_fn forward;
  accepted_fn accepted;
  connected_fn connected;
  closed_fn closed;
  established_fn established;
};

template <typename Topic, typename Adapter = protocol_default_adapter<Topic>>
class protocol
{
public:
  using topic = Topic;
  using adapter = Adapter;

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