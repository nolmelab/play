#include <play/robust/base/macros.hpp>
#include <play/robust/net/session.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
void session<Protocol>::send(const void* data, size_t len)
{
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
std::string session<Protocol>::get_endpoint() const
{
  if (endpoint_.empty())
  {
    tcp::endpoint ep = socket_.remote_endpoint();

    auto addr = ep.address().to_string();
    auto port = ep.port();

    endpoint_ = fmt::format("{}:{}", addr, port);
  }
  return endpoint_;
}

template <typename Protocol>
void session<Protocol>::start_send()
{
  // locked
  PLAY_CHECK(!sending_);
  sending_ = true;

  auto& send_buf = send_bufs_[acc_buf_index_];  // acc buffer becomes send buffer
  acc_buf_index_ = 1 - acc_buf_index_;          // switch send accumulation buffer

  auto self(this->shared_from_this());
  auto buf = send_buf.data();
  boost::asio::async_write(socket_, boost::asio::buffer(buf.data(), buf.size()),
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
  boost::asio::async_read(socket_, boost::asio::buffer(buf.data(), buf.size()),
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
    auto index = 1 - acc_buf_index_;
    auto& send_buf = send_bufs_[index];
    send_buf.consume(len);
    sending_ = false;
    start_send();
  }
  else
  {
    protocol_->closed();
    handler_.on_closed(this->shared_from_this(), ec);
  }
}

template <typename Protocol>
void session<Protocol>::handle_recv(boost::system::error_code ec, size_t len)
{
  // TODO: some errors need to be ignored
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
    protocol_->closed();
    handler_.on_closed(this->shared_from_this(), ec);
  }
}

}}}  // namespace play::robust::net

// note:
// - session<Protocol> dependent-name lookup. hence it requires to use this->
//   or fully qualified function name for shared_from_this()