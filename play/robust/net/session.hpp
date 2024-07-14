#pragma once

#include <mutex>
#include <play/robust/net/protocol.hpp>
#include <play/robust/net/session_handler.hpp>

using tcp = asio::ip::tcp;

namespace play { namespace robust { namespace net {

// tcp 세션의 바이트 송수신을 처리한다.
/**
 * 1 recv, 1 send로 구현한다. 1 send이기에 누적 버퍼 (accumulation) 용도와 
 * 전송 버퍼를 바꾸어 보내는 중에는 모으고 전송 완료되면 한번에 전송한다.
 * 
 * session은 바이트 송수신만을 담당하고, 프레임 변환은 Protocol에서 처리한다.
 */
template <typename Protocol>
class session : public std::enable_shared_from_this<session<Protocol>>
{
public:
  using ptr = std::shared_ptr<session<Protocol>>;
  using handle = size_t;
  using topic = typename Protocol::topic;

public:
  // session protocol adapter
  /**
   * app에서 이를 기본 어댑터로 사용하거나 상속하여 구현하여 처리할 수 있음. 
   * Protocol::adapter 타잎의 생성자에 항상 session::ptr을 넘겨줌
   */
  struct protocol_adapter : public protocol_adapter_base<topic>
  {
    protocol_adapter(ptr session) : session_{session} {}

    void send(const void* data, size_t len) override { session_->send(data, len); }

    void forward(topic topic, const void* data, size_t len) override
    {
      session_->handler_.on_receive(session_, topic, data, len);
    }

    void established(size_t handle) override { session_->handler_.on_established(session_); }

    ptr session_;
  };

public:
  session(session_handler<Protocol>& handler, tcp::socket&& socket, bool accepted)
      : handler_{handler},
        socket_{std::move(socket)},
        accepted_{accepted},
        adapter_{this->shared_from_this()}
  {
    handle_ = socket_.native_handle();
    protocol_ = std::make_unique<Protocol>(handle_, adapter_);

    if (accepted_)
      protocol_->accepted();
    else
      protocol_->connected();

    start_recv();
  }

  // 바이트를 쓴다. 앱에서는 protocol_.send()를 사용
  void send(const void* data, size_t len);

  void close();

  std::string get_endpoint() const;

  size_t get_handle() { return handle_; }

  bool is_open() const { return socket_.is_open(); }

  Protocol& get_protocol() { return protocol_; }

private:
  static constexpr size_t recv_size = 1024 * 8;

  void start_send();

  void start_recv();

  void handle_send(boost::system::error_code ec, size_t len);

  void handle_recv(boost::system::error_code ec, size_t len);

private:
  session_handler<Protocol>& handler_;
  tcp::socket socket_;
  protocol_adapter adapter_;
  std::unique_ptr<Protocol> protocol_;
  bool accepted_;
  size_t handle_;
  mutable std::string endpoint_;

  std::recursive_mutex mutex_;
  asio::streambuf recv_buf_;
  bool sending_{false};
  int acc_buf_index_{0};
  asio::streambuf send_bufs_[2];
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/session.ipp>