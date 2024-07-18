#include <play/robust/base/macros.hpp>
#include <play/robust/base/serializer.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic>
inline asio::const_buffer secure_protocol<Topic>::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!cipher_handshake_);
  PLAY_CHECK(!cipher_codec_);

  accepted_ = true;
  closed_ = false;

  cipher_handshake_ = std::make_unique<sodium_handshake>(handle_, true);
  cipher_codec_ = std::make_unique<sodium_cipher>(handle_, *cipher_handshake_.get());

  // pub 키 전송 요청
  return encode_handshake(cipher_handshake_->prepare());
}

template <typename Topic>
inline asio::const_buffer secure_protocol<Topic>::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!cipher_handshake_);
  PLAY_CHECK(!cipher_codec_);

  connected_ = true;
  closed_ = false;

  cipher_handshake_ = std::make_unique<sodium_handshake>(handle_, false);
  cipher_codec_ = std::make_unique<sodium_cipher>(handle_, *cipher_handshake_.get());

  return encode_handshake(cipher_handshake_->prepare());
}

template <typename Topic>
inline void secure_protocol<Topic>::closed()
{
  PLAY_RETURN_IF(closed_);
  closed_ = true;
}

template <typename Topic>
inline size_t secure_protocol<Topic>::encode(Topic pic, const asio::const_buffer& src,
                                             asio::streambuf& dst, bool encrypt)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("encode called on closed session. handle: {}", handle_);
    return 0;
  }

  if (!is_established())
  {
    LOG()->warn("encode called on non-established session. handle: {}", handle_);
    return 0;
  }

  auto header_len = length_field_size + sizeof(Topic) + 1;
  auto total_len = src.size() + header_len;
  auto wdst = dst.prepare(total_len);

  auto pdst = reinterpret_cast<uint8_t*>(wdst.data());
  base::serializer::serialize(pdst, sizeof(pic), pic);  // prepare()로 공간이 충분
  pdst += sizeof(Topic);
  base::serializer::serialize(pdst, 1, encrypt);
  pdst += 1;
  base::serializer::serialize(pdst, length_field_size, static_cast<uint32_t>(src.size()));

  if (encrypt)
  {
    // src를 암호화 하여 dst에 prepare를 한 후 쓰고 commit 한다.
    auto enc_len = cipher_codec_->encode(src, dst, header_len);
    PLAY_CHECK(enc_len > 0);
  }
  else
  {
    auto pbody = pdst + length_field_size;
    std::memcpy(pbody, src.data(), src.size());
    dst.commit(src.size() + header_len);
  }

  return total_len;
}

template <typename Topic>
inline std::tuple<size_t, asio::const_buffer, Topic> secure_protocol<Topic>::decode(
    const asio::const_buffer& src)
{
  if (src.size() <= get_min_size())
  {
    return {0, {}, 0};
  }

  auto psrc = reinterpret_cast<const uint8_t*>(src.data());
  Topic pic{};
  bool encrypt{false};
  uint32_t len{0};

  base::serializer::deserialize(psrc, sizeof(pic), pic);
  psrc += sizeof(Topic);
  base::serializer::deserialize(psrc, 1, encrypt);
  psrc += 1;
  base::serializer::deserialize(psrc, length_field_size, len);

  auto total_len = len + length_field_size + sizeof(Topic) + 1;
  if (src.size() < total_len)
  {
    return {0, {}, Topic{}};
  }

  auto pframe = psrc + 4;

  if (encrypt)
  {
    auto [used_len, dec_buf] = cipher_codec_->decode({pframe, len});
    return {total_len, dec_buf, pic};  // secure frame
  }
  // else
  return {total_len, {pframe, len}, pic};  // plainframe
}

template <typename Topic>
std::pair<size_t, asio::const_buffer> secure_protocol<Topic>::handshake(
    const asio::const_buffer& src)
{
  recv_buf_.prepare(src.size());
  recv_buf_.sputn(reinterpret_cast<const char*>(src.data()), src.size());

  auto frame = handshake_codec_.decode(recv_buf_.data());
  if (frame.size() > 0)
  {
    auto [used_len, resp] = cipher_handshake_->on_handshake(frame);
    PLAY_CHECK(used_len == frame.size());
    recv_buf_.consume(used_len + handshake_codec_.length_field_size);

    if (resp.size() > 0)
    {
      auto send = encode_handshake(resp);
      return {src.size(), send};
    }
    // else - handshake completed
    PLAY_CHECK(cipher_handshake_->is_established());
    return {src.size(), {}};
  }
  // else - needs to recv more data
  return {src.size(), {}};
}

template <typename Topic>
inline asio::const_buffer secure_protocol<Topic>::encode_handshake(const asio::const_buffer& src)
{
  auto size = handshake_codec_.encode(src, send_buf_);
  PLAY_CHECK(size == src.size() + handshake_codec_.length_field_size);
  auto send = send_buf_.data();
  send_buf_.consume(size);
  return send;
}

}}}  // namespace play::robust::net