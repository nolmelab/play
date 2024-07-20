#pragma once

#include <mutex>
#include <play/robust/net/protocol.hpp>

using tcp = asio::ip::tcp;

namespace play { namespace robust { namespace net {

// tcp 세션의 바이트 송수신을 처리한다.
/**
 * 1 recv, 1 send로 구현한다. 1 send이기에 누적 버퍼 (accumulation) 용도와 
 * 전송 버퍼를 바꾸어 보내는 중에는 모으고 전송 완료되면 한번에 전송한다.
 * 
 * session은 바이트 송수신만을 담당하고, 프레임 변환은 Protocol에서 처리한다.
 */
template <typename Protocol, typename Handler>
class session : public std::enable_shared_from_this<session<Protocol, Handler>>
{
public:
  using ptr = std::shared_ptr<session<Protocol, Handler>>;
  using handle = size_t;
  using topic = typename Protocol::topic;

public:
  // 프로토콜 생성. 프로토콜에 알림. 수신 시작
  session(Handler& handler, asio::io_context& ioc, bool accepted);

  ~session();

  // kick off protocol and communication
  void start();

  // 바이트를 쓴다.
  void send(const void* data, size_t len);

  // 토픽을 포함하여 전송하는 프로토콜 전송용 함수
  bool send(topic pic, const void* data, size_t len, bool encrypt = false);

  // 소켓을 끊음. 통지는 따로 0~2까지 발생 (recv, send)
  void close();

  tcp::socket& get_socket()
  {
    return socket_;
  }

  std::string get_remote_addr() const;

  size_t get_handle()
  {
    return handle_;
  }

  bool is_open() const
  {
    return socket_.is_open();
  }

  Protocol& get_protocol()
  {
    return *protocol_.get();
  }

  bool is_established() const
  {
    return protocol_ && protocol_->is_established();
  }

private:
  static constexpr size_t recv_size = 1024 * 8;

  void start_send();

  void start_recv();

  void handle_send(error_code ec, size_t len);

  void handle_recv(error_code ec, size_t len);

  void handle_close(error_code ec);

  size_t recv_frames();

  void send_handshake(asio::const_buffer hs);

private:
  Handler& handler_;
  tcp::socket socket_;
  std::unique_ptr<Protocol> protocol_;
  bool accepted_;
  size_t handle_;
  mutable std::string remote_addr_;

  std::recursive_mutex mutex_;
  asio::streambuf recv_buf_;
  bool sending_{false};
  int acc_buf_index_{0};
  asio::streambuf send_bufs_[2];
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/session.ipp>