#include <play/robust/base/macros.hpp>
#include <play/robust/net/session.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol, typename Handler>
session<Protocol, Handler>::session(Handler& handler, asio::io_context& ioc, bool accepted)
    : handler_{handler},
      socket_{ioc},
      handle_{0},
      accepted_{accepted}
{
}

template <typename Protocol, typename Handler>
session<Protocol, Handler>::~session()
{
  close();
}

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::start()
{
  PLAY_CHECK(!protocol_);

  handle_ = socket_.native_handle();
  PLAY_CHECK(handle_ > 0);

  protocol_ = std::make_unique<Protocol>(handle_);

  if (accepted_)
  {
    auto buf = protocol_->accepted();
    this->send_handshake(buf);
  }
  else
  {
    auto buf = protocol_->connected();
    this->send_handshake(buf);
  }

  if (protocol_->is_established())
  {
    handler_.on_established(this->shared_from_this());
  }

  this->start_recv();
}

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::send(const void* data, size_t len)
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

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::send(topic pic, const void* data, size_t len)
{
  if (!is_open())
  {
    LOG()->warn("send w/ topic called on closed session. handle: {}, remote: {}", get_handle(),
                get_remote_addr());
    return;
  }

  std::lock_guard<std::recursive_mutex> guard(mutex_);
  auto& acc_buf = send_bufs_[acc_buf_index_];  // write to the accumulation buffer
  // protocol::encode() prepare, encode, and commit
  auto total_len = protocol_->encode(pic, data, len, acc_buf);

  if (!sending_)
  {
    start_send();
  }
}

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::close()
{
  if (is_open())
  {
    socket_.close();  // handle_recv() 또는 handle_send()에서 에러 처리
  }
}

template <typename Protocol, typename Handler>
std::string session<Protocol, Handler>::get_remote_addr() const
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

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::start_send()
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
                       this->handle_send(ec, len);
                     });
}

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::start_recv()
{
  auto self(this->shared_from_this());
  auto buf = recv_buf_.prepare(recv_size);
  socket_.async_receive(boost::asio::buffer(buf.data(), buf.size()),
                        [this, self](boost::system::error_code ec, std::size_t len)
                        {
                          this->handle_recv(ec, len);
                        });
}

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::handle_send(boost::system::error_code ec, size_t len)
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

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::handle_recv(boost::system::error_code ec, size_t len)
{
  if (!ec)
  {
    recv_buf_.commit(len);
    auto payload_buf = recv_buf_.data();

    if (protocol_->is_established())
    {
      for (;;)
      {
        auto [consumed_len, frame, topic] = protocol_->decode(
            reinterpret_cast<const char*>(payload_buf.data()), payload_buf.size());
        handler_.on_receive(this->shared_from_this(), topic, frame.data(), frame.size());

        recv_buf_.consume(consumed_len);
        if (consumed_len == 0)
        {
          break;
        }

        payload_buf = recv_buf_.data();
      }
    }
    else
    {
      auto [consumed_len, buf] = protocol_->handshake(
          reinterpret_cast<const char*>(payload_buf.data()), payload_buf.size());
      recv_buf_.consume(consumed_len);  // used in protocol
      this->send_handshake(buf);

      if (protocol_->is_established())
      {
        handler_.on_established(this->shared_from_this());
      }
    }

    this->start_recv();
  }
  else
  {
    close();  // ensure socket is closed
    protocol_->closed();
    handler_.on_closed(this->shared_from_this(), ec);
  }
}

template <typename Protocol, typename Handler>
void session<Protocol, Handler>::send_handshake(asio::const_buffer buf)
{
  if (buf.size() > 0)
  {
    send(buf.data(), buf.size());
  }
}

}}}  // namespace play::robust::net

// note:
// - session<Protocol> dependent-name lookup. hence it requires to use this->
//   or fully qualified function name for shared_from_this()
// - streambuf의 commit의 pptr()을 앞으로 이동하고, consume()은 gptr()을 앞으로 이동
//   - prepare에서 버퍼 이동해서 공간을 확보
//   - 매번 헷갈려서 여러 곳에 적어 둔다.
