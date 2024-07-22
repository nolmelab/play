#include <play/robust/base/macros.hpp>
#include <play/robust/net/session.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
session<Protocol>::session(session_handler& handler, asio::io_context& ioc, bool accepted)
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

  // get address
  {
    tcp::endpoint ep = socket_.remote_endpoint();

    auto addr = ep.address().to_string();
    auto port = ep.port();

    remote_addr_ = fmt::format("{}:{}", addr, port);
  }

  // 통지보다 먼저 와야 함
  this->start_recv();

  if (protocol_->is_established())
  {
    handler_.on_established(this->shared_from_this());
  }
}

template <typename Protocol>
void session<Protocol>::send(const void* data, size_t len)
{
  //PLAY_CHECK(len > 0);

  if (!is_open())
  {
    LOG()->warn("send called on closed session. handle: {}, remote: {}", get_handle(),
                get_remote_addr());
    return;
  }

  std::lock_guard guard(mutex_);
  auto& acc_buf = send_bufs_[acc_buf_index_];  // write to the accumulation buffer
  auto p = acc_buf.prepare(len);
  acc_buf.sputn(reinterpret_cast<const char*>(data), len);  // auto commit

  if (!sending_)
  {
    start_send();
  }
}

template <typename Protocol>
bool session<Protocol>::send(topic pic, const void* data, size_t len, bool encrypt)
{
  if (!is_open())
  {
    LOG()->warn("send w/ topic called on closed session. handle: {}, remote: {}", get_handle(),
                get_remote_addr());
    return false;
  }

  if (len >= Protocol::max_send_size)
  {
    LOG()->warn("frame size is bigger than the limit. limit: {}, handle: {}",
                Protocol::max_send_size, get_handle());
    return false;
  }

  PLAY_CHECK(len > 0);
  PLAY_CHECK(data != nullptr);
  if (len == 0 || data == nullptr)
  {
    // NOTE: fmt :#x가 버전별 지원이 다르다.
    LOG()->warn("session::send. len: {}, data: {}", len, data);
    return false;
  }

  std::lock_guard guard(mutex_);
  auto& acc_buf = send_bufs_[acc_buf_index_];  // write to the accumulation buffer
  // protocol::encode() prepare, encode, and commit
  auto cbuf = asio::const_buffer{data, len};

  try 
  {
    auto total_len = protocol_->encode(pic, cbuf, acc_buf, encrypt);
  }
  catch ( std::exception& ex)
  {
    LOG()->error("protocol encode error. handle: {}", handle_);
    close();
    return false;
  }

  if (!sending_)
  {
    start_send();
  }


  return false;
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
template <typename UserData>
void session<Protocol>::bind_data(std::shared_ptr<UserData> data)
{
  auto sp = std::static_pointer_cast<user_data>(data);
  sp->handle_ = handle_;
  bind_data_ = data;
}

template <typename Protocol>
template <typename UserData>
std::shared_ptr<UserData> session<Protocol>::get_data() const
{
  return bind_data_;
}

template <typename Protocol>
std::string session<Protocol>::get_remote_addr() const
{
  return remote_addr_;
}

template <typename Protocol>
void session<Protocol>::start_send()
{
  // locked
  PLAY_CHECK(!sending_);

  auto& send_buf = send_bufs_[acc_buf_index_];  // acc buffer becomes send buffer
  auto buf = send_buf.data();
  if (buf.size() == 0)
  {
    return;  // nothing to send in accumulation buffer
  }

  acc_buf_index_ = acc_buf_index_ ^ 1;  // switch send accumulation buffer
  sending_ = true;

  auto self(this->shared_from_this());
  socket_.async_send(boost::asio::buffer(buf.data(), buf.size()),
                     [this, self](error_code ec, std::size_t len)
                     {
                       this->handle_send(ec, len);
                     });
}

template <typename Protocol>
void session<Protocol>::start_recv()
{
  auto self(this->shared_from_this());
  auto buf = recv_buf_.prepare(recv_size);
  socket_.async_receive(boost::asio::buffer(buf.data(), buf.size()),
                        [this, self](error_code ec, std::size_t len)
                        {
                          this->handle_recv(ec, len);
                        });
}

template <typename Protocol>
void session<Protocol>::handle_send(error_code ec, size_t len)
{
  std::lock_guard guard(mutex_);
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
    handle_close(ec);
  }
}

template <typename Protocol>
void session<Protocol>::handle_recv(error_code ec, size_t len)
{
  if (!ec)
  {
    recv_buf_.commit(len);

    if (protocol_->is_established())
    {
      recv_frames();
    }
    else
    {
      auto recv_data = recv_buf_.data();

      auto cbuf = asio::const_buffer{recv_data.data(), recv_data.size()};
      auto [consumed_len, buf] = protocol_->handshake(cbuf);
      recv_buf_.consume(consumed_len);  // used in protocol
      this->send_handshake(buf);

      if (protocol_->is_established())
      {
        handler_.on_established(this->shared_from_this());
        recv_frames();
      }
    }

    this->start_recv();
  }
  else
  {
    handle_close(ec);
  }
}

template <typename Protocol>
void session<Protocol>::handle_close(error_code ec)
{
  if (is_open())
  {
    close();  // ensure socket is closed
    protocol_->closed();
    handler_.on_closed(this->shared_from_this(), ec);
  }
}

template <typename Protocol>
void session<Protocol>::send_handshake(asio::const_buffer buf)
{
  if (buf.size() > 0)
  {
    send(buf.data(), buf.size());
  }
}

template <typename Protocol>
size_t session<Protocol>::recv_frames()
{
  size_t frame_count = 0;

  auto recv_data = recv_buf_.data();

  while (recv_data.size() > 0)
  {
    try
    {
      auto cbuf = asio::const_buffer{recv_data.data(), recv_data.size()};
      auto [consumed_len, frame, topic] = protocol_->decode(cbuf);
      if (consumed_len > 0)
      {
        PLAY_CHECK(frame.size() > 0);
        handler_.on_receive(this->shared_from_this(), topic, frame.data(), frame.size());
        recv_buf_.consume(consumed_len);
      }
      else
      {
        break;
      }
    }
    catch (std::exception& ex)
    {
      LOG()->error("exception in recv_frames. exception: {}", ex.what());
      close();  // start_recv()에서 시작하여 에러 통지
      return frame_count;
    }

    ++frame_count;
    recv_data = recv_buf_.data();
  }

  return frame_count;
}

}}}  // namespace play::robust::net
