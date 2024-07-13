#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic>
inline void secure_protocol<Topic>::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);
  PLAY_CHECK(!cipher_handshake_);
  PLAY_CHECK(!cipher_codec_);

  accepted_ = true;
  closed_ = false;

  length_codec_ = std::make_unique<length_delimited>(handle_);
  cipher_handshake_ = std::make_unique<sodium_handshake>(handle_, true, get_adapter().send_);
  cipher_codec_ = std::make_unique<sodium_cipher>(handle_, *cipher_handshake_.get());

  cipher_handshake_->prepare();
  cipher_handshake_->sync_key();

  get_adapter().accepted_(handle_);
}

template <typename Topic>
inline void secure_protocol<Topic>::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);
  PLAY_CHECK(!cipher_handshake_);
  PLAY_CHECK(!cipher_codec_);

  connected_ = true;
  closed_ = false;

  length_codec_ = std::make_unique<length_delimited>(handle_);
  cipher_handshake_ = std::make_unique<sodium_handshake>(handle_, false, get_adapter().send_);
  cipher_codec_ = std::make_unique<sodium_cipher>(handle_, *cipher_handshake_.get());

  cipher_handshake_->prepare();
  cipher_handshake_->sync_key();

  get_adapter().connected_(handle_);
}

template <typename Topic>
inline void secure_protocol<Topic>::closed()
{
  PLAY_CHECK(!closed_);
  closed_ = true;

  get_adapter().closed_(handle_);
}

template <typename Topic>
inline void secure_protocol<Topic>::send(Topic topic, const char* data, size_t len, bool encrypt)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("send called on closed session. handle: {}", handle_);
    return;
  }

  auto m_1 = send_buf_.prepare(sizeof(Topic));
  serialize(m_1, topic);
  send_buf_.commit(sizeof(Topic));

  auto m_2 = send_buf_.prepare(1);
  protocol<Topic>::serialize(m_2, encrypt);
  send_buf_.commit(1);

  auto payload_buf = asio::const_buffer{data, len};

  auto enc_len = cipher_codec_->encode(payload_buf, enc_buf_);
  if (enc_len > 0)
  {
    auto enc_buf = enc_buf_.data();
    auto result = length_codec_->encode(enc_buf, send_buf_);
    PLAY_CHECK(result);  // 여기서 실패하는 경우는 없다

    auto send_data = send_buf_.data();
    get_adapter().send_(send_data.data(), send_data.size());
    send_buf_.consume(send_data.size());

    // 사용한 만큼 읽기 포인터를 앞으로 이동해야 함
    enc_buf_.consume(enc_buf.size());
  }
}

template <typename Topic>
inline void secure_protocol<Topic>::receive(const char* data, size_t len)
{
  const char* payload = reinterpret_cast<const char*>(data);
  auto mut_buf = recv_buf_.prepare(len);
  recv_buf_.sputn(payload, len);
  // commit() 불필요

  if (!cipher_handshake_->is_established())
  {
    auto hs_buf = recv_buf_.data();
    cipher_handshake_->on_receive(hs_buf);

    if (cipher_handshake_->is_established())
    {
      get_adapter().established_(handle_);
    }
    else
    {
      if (cipher_handshake_->is_key_received())
      {
        cipher_handshake_->sync_nonce();
      }
    }
    // 읽은 만큼 앞으로 이동
    recv_buf_.consume(hs_buf.size());
    return;
  }

  PLAY_CHECK(cipher_handshake_->is_established());

  Topic topic{0};
  bool encrypt = false;

  auto next_frame = [&topic, &encrypt, this]() -> asio::const_buffer
  {
    auto d_1 = recv_buf_.data();
    if (d_1.size() < get_min_size())
    {
      return {};
    }
    deserialize(d_1, topic);
    recv_buf_.consume(sizeof(topic));

    auto d_2 = recv_buf_.data();
    this->deserialize(d_2, encrypt);  // XXX: this->가 없으면 clang에서 못 찾음
    recv_buf_.consume(1);

    return recv_buf_.data();
  };

  auto data_buf = next_frame();
  if (data_buf.size() == 0)
  {
    return;
  }

  PLAY_CHECK(data_buf.size() > get_min_size());
  PLAY_CHECK(topic != 0);

  // 프레임이 있으면 계속 읽음
  while (auto result = length_codec_->decode(data_buf))
  {
    auto length_buf = result.value();
    if (encrypt)
    {
      auto plain_result = cipher_codec_->decode(length_buf);

      if (plain_result)
      {
        auto frame = plain_result.value();
        get_adapter().forward_(topic, frame.data(), frame.size());
      }
    }
    else
    {
      get_adapter().forward_(topic, length_buf.data(), length_buf.size());
    }

    // 다음 사용 가능한 데이터로 이동
    recv_buf_.consume(length_buf.size() + length_codec_->length_field_size);

    data_buf = next_frame();
    if (data_buf.size() == 0)
    {
      break;
    }
    PLAY_CHECK(data_buf.size() > get_min_size());
    PLAY_CHECK(topic != 0);
  }
}
}}}  // namespace play::robust::net