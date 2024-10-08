#include <play/base/macros.hpp>
#include <play/net/protocol/secure_protocol.hpp>
#include <play/net/util/serializer.hpp>

namespace play {

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

  auto header_len = length_field_size + sizeof(Topic) + enc_field_size;
  auto total_len = src.size() + header_len;
  auto wdst = dst.prepare(total_len);

  auto pdst = reinterpret_cast<uint8_t*>(wdst.data());
  serializer::serialize(pdst, sizeof(pic), pic);  // prepare()로 공간이 충분
  pdst += sizeof(Topic);
  serializer::serialize(pdst, enc_field_size, encrypt);
  pdst += enc_field_size;
  serializer::serialize(pdst, length_field_size, static_cast<uint32_t>(src.size()));

  auto pbody = pdst + length_field_size;

  if (encrypt)
  {
    asio::mutable_buffer mbuf{pbody, src.size()};
    auto enc_len = cipher_codec_->encode(src, mbuf);
    PLAY_CHECK(enc_len == src.size());
    dst.commit(total_len);
  }
  else
  {
    std::memcpy(pbody, src.data(), src.size());
    dst.commit(total_len);
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

  serializer::deserialize(psrc, sizeof(pic), pic);
  psrc += sizeof(Topic);
  serializer::deserialize(psrc, enc_field_size, encrypt);
  psrc += enc_field_size;
  serializer::deserialize(psrc, length_field_size, len);

  auto total_len = len + length_field_size + sizeof(Topic) + enc_field_size;
  if (src.size() < total_len)
  {
    return {0, {}, Topic{}};
  }

  // TODO: 최대 수신 길이 처리

  auto ppayload = psrc + length_field_size;

  if (encrypt)
  {
    auto [used_len, dec_buf] = cipher_codec_->decode({ppayload, len});
    PLAY_CHECK(used_len == len);
    return {total_len, dec_buf, pic};  // secure frame
  }
  // else
  return {total_len, {ppayload, len}, pic};  // plainframe
}

template <typename Topic>
std::pair<size_t, asio::const_buffer> secure_protocol<Topic>::handshake(
    const asio::const_buffer& src)
{
  LOG()->debug("handshake: {} bytes", src.size());
  auto payload = hs_length_codec_.decode(src);
  if (payload.size() > 0)
  {
    auto [used_len, resp] = cipher_handshake_->on_handshake(payload);
    PLAY_CHECK(used_len == payload.size());
    // 세션에 알려서 수신 읽기 위치를 조절하는 길이
    auto consumed_len = payload.size() + hs_length_codec_.length_field_size;

    if (resp.size() > 0)
    {
      auto send = encode_handshake(resp);
      return {consumed_len, send};
    }
    // else - handshake completed
    PLAY_CHECK(cipher_handshake_->is_established());
    return {consumed_len, {}};
  }
  // else - not used any bytes
  return {0, {}};
}

template <typename Topic>
inline asio::const_buffer secure_protocol<Topic>::encode_handshake(const asio::const_buffer& src)
{
  static thread_local asio::streambuf hs_send_buf;  // 협상 전송용 버퍼

  auto size = hs_length_codec_.encode(src, hs_send_buf);
  PLAY_CHECK(size == src.size() + hs_length_codec_.length_field_size);
  auto send = hs_send_buf.data();
  hs_send_buf.consume(size);

  LOG()->info("encode_handshake: {} bytes", size);
  return send;
}

}  // namespace play