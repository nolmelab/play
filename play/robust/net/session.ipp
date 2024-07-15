#include <play/robust/base/macros.hpp>
#include <play/robust/net/session.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
session<Protocol>::session(session_handler<Protocol>& handler, asio::io_context& ioc, bool accepted)
    : handler_{handler},
      socket_{ioc},
      handle_{0},
      accepted_{accepted}
{
}

template <typename Protocol>
session<Protocol>::~session()
{
  close();
}

template <typename Protocol>
void session<Protocol>::start()
{
  PLAY_CHECK(!protocol_);

  handle_ = socket_.native_handle();
  PLAY_CHECK(handle_ > 0);

  adapter_ = std::make_unique<protocol_adapter>(this->shared_from_this());
  protocol_ = std::make_unique<Protocol>(handle_, *adapter_);

  if (accepted_)
    protocol_->accepted();
  else
    protocol_->connected();

  start_recv();
}

template <typename Protocol>
void session<Protocol>::send(const void* data, size_t len)
{
  if (!is_open())
  {
    LOG()->warn("send called on closed session. handle: {}, remote: {}", get_handle(),
                get_remote_addr());
    return;
  }

  std::lock_guard<std::recursive_mutex> guard(mutex_);
  auto& acc_buf = send_bufs_[acc_buf_index_];  // write to the accumulation buffer
  auto p = acc_buf.prepare(len);
  acc_buf.sputn(reinterpret_cast<const char*>(data), len);  // auto commit

  if (!sending_)
  {
    start_send();
  }
}

template <typename Protocol>
void session<Protocol>::close()
{
  if (is_open())
  {
    socket_.close();  // handle_recv() 또는 handle_send()에서 에러 처리
  }
}

template <typename Protocol>
std::string session<Protocol>::get_remote_addr() const
{
  if (remote_addr_.empty())
  {
    tcp::endpoint ep = socket_.remote_endpoint();

    auto addr = ep.address().to_string();
    auto port = ep.port();

    remote_addr_ = fmt::format("{}:{}", addr, port);
  }
  return remote_addr_;
}

template <typename Protocol>
void session<Protocol>::start_send()
{
  // locked
  PLAY_CHECK(!sending_);

  auto& send_buf = send_bufs_[acc_buf_index_];  // acc buffer becomes send buffer
  if (send_buf.size() == 0)
  {
    return;  // nothing to send in accumulation buffer
  }

  sending_ = true;
  acc_buf_index_ = acc_buf_index_ ^ 1;  // switch send accumulation buffer

  auto self(this->shared_from_this());
  auto buf = send_buf.data();
  socket_.async_send(boost::asio::buffer(buf.data(), buf.size()),
                     [this, self](boost::system::error_code ec, std::size_t len)
                     {
                       handle_send(ec, len);
                     });
}

template <typename Protocol>
void session<Protocol>::start_recv()
{
  auto self(this->shared_from_this());
  auto buf = recv_buf_.prepare(recv_size);
  socket_.async_receive(boost::asio::buffer(buf.data(), buf.size()),
                        [this, self](boost::system::error_code ec, std::size_t len)
                        {
                          handle_recv(ec, len);
                        });
}

template <typename Protocol>
void session<Protocol>::handle_send(boost::system::error_code ec, size_t len)
{
  std::lock_guard<std::recursive_mutex> guard(mutex_);
  PLAY_CHECK(sending_);

  if (!ec)
  {
    auto index = acc_buf_index_ ^ 1;
    auto& send_buf = send_bufs_[index];
    send_buf.consume(len);
    sending_ = false;
    start_send();
  }
  else
  {
    close();  // ensure socket is closed
    protocol_->closed();
    handler_.on_closed(this->shared_from_this(), ec);
  }
}

template <typename Protocol>
void session<Protocol>::handle_recv(boost::system::error_code ec, size_t len)
{
  if (!ec)
  {
    recv_buf_.commit(len);
    auto payload_buf = recv_buf_.data();
    protocol_->receive(reinterpret_cast<const char*>(payload_buf.data()), payload_buf.size());
    recv_buf_.consume(len);  // used in protocol

    start_recv();
  }
  else
  {
    close();  // ensure socket is closed
    protocol_->closed();
    handler_.on_closed(this->shared_from_this(), ec);
  }
}

}}}  // namespace play::robust::net

// note:
// - session<Protocol> dependent-name lookup. hence it requires to use this->
//   or fully qualified function name for shared_from_this()
// - streambuf의 commit의 pptr()을 앞으로 이동하고, consume()은 gptr()을 앞으로 이동
//   - prepare에서 버퍼 이동해서 공간을 확보
//   - 매번 헷갈려서 여러 곳에 적어 둔다.
