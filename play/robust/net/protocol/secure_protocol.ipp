#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic>
inline asio::const_buffer secure_protocol<Topic>::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);
  PLAY_CHECK(!cipher_handshake_);
  PLAY_CHECK(!cipher_codec_);

  accepted_ = true;
  closed_ = false;

  cipher_handshake_ = std::make_unique<sodium_handshake>(handle_, true);
  cipher_codec_ = std::make_unique<sodium_cipher>(handle_, *cipher_handshake_.get());

  // pub 키 전송 요청
  return cipher_handshake_->prepare();
}

template <typename Topic>
inline asio::const_buffer secure_protocol<Topic>::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);
  PLAY_CHECK(!cipher_handshake_);
  PLAY_CHECK(!cipher_codec_);

  connected_ = true;
  closed_ = false;

  cipher_handshake_ = std::make_unique<sodium_handshake>(handle_, false);
  cipher_codec_ = std::make_unique<sodium_cipher>(handle_, *cipher_handshake_.get());

  return cipher_handshake_->prepare();
}

template <typename Topic>
inline void secure_protocol<Topic>::closed()
{
  PLAY_RETURN_IF(closed_);
  closed_ = true;
}

template <typename Topic>
inline size_t secure_protocol<Topic>::encode(Topic pic, const asio::const_buffer& src,
                                             asio::streambuf& dst, bool encrypt = false)
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

  auto total_len = len + length_codec_->length_field_size + sizeof(Topic) + 1;
  auto wdst = dst.prepare(total_len);

  auto pdst = reinterpret_cast<uint8_t*>(wdst.data());
  this->serialize(pdst, pic);
  pdst += sizeof(Topic);
  this->serialize(pdst, encrypt);
  pdst += 1;
  this->serialize(pdst + 1, src.size());
  dst.commit(sizeof(Topic) + 1);  // commit topic and encrypt

  // src를 암호화 하여 dst에 prepare를 한 후 쓰고 commit 한다.
  auto enc_len = cipher_codec_->encode(src, dst);
  PLAY_CHECK(enc_len > 0);

  return total_len;
}

template <typename Topic>
inline std::pair<size_t, asio::const_buffer> secure_protocol<Topic>::decode(
    const asio::const_buffer& src)
{
  if (src.size() <= get_min_size())
  {
    return {0, {}};
  }

  auto psrc = reinterpret_cast<const uint8_t*>(src.data());
  Topic pic{};
  bool encrypt{false};
  uint32_t len{0};

  this->deserialize(psrc, pic);
  psrc += sizeof(Topic);
  this->deserialize(psrc, encrypt);
  psrc += 1;
  this->deserialize(psrc, len);

  auto total_len = len + length_codec_->length_field_size + sizeof(Topic) + 1;
  if (src.size() < total_len)
  {
    return {0, {}};
  }

  auto pframe = psrc + 4;

  if (encrypt)
  {
    auto [used_len, dec_buf] = cipher_codec_->decode({pframe, len});
    return {total_len, dec_buf};  // found one frame
  }
  // else
  return {total_len, {pframe, len}};
}

template <typename Topic>
std::pair<size_t, asio::const_buffer> secure_protocol<Topic>::handshake(
    const asio::const_buffer& src)
{
  recv_buf_.prepare(src.size());
  recv_buf_.sputn(src.data(), len);
}

}}}  // namespace play::robust::net