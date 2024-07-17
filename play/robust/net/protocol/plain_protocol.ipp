#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/plain_protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic>
inline asio::const_buffer plain_protocol<Topic>::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);

  accepted_ = true;
  closed_ = false;

  length_codec_ = std::make_unique<length_delimited>(handle_);
  return {};
}

template <typename Topic>
inline asio::const_buffer plain_protocol<Topic>::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);

  connected_ = true;
  closed_ = false;

  length_codec_ = std::make_unique<length_delimited>(handle_);
  return {};
}

template <typename Topic>
inline void plain_protocol<Topic>::closed()
{
  PLAY_RETURN_IF(closed_);
  closed_ = true;
}

template <typename Topic>
inline size_t plain_protocol<Topic>::encode(topic pic, const asio::const_buffer& src,
                                            asio::streambuf& dst)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("plain_protocol::encode called on closed session. handle: {}", handle_);
    return 0;
  }

  PLAY_CHECK(is_established());

  auto total_len = length_codec_->length_field_size + src.size() + sizeof(topic);
  auto dst_buf = dst.prepare(total_len);

  auto wbuf = reinterpret_cast<char*>(dst_buf.data());
  this->serialize(reinterpret_cast<uint8_t*>(wbuf), src.size(), pic);

  auto mbuf = asio::mutable_buffer{reinterpret_cast<void*>(wbuf + sizeof(topic)),
                                   total_len - sizeof(topic)};
  auto sbuf = length_codec_->encode(src, mbuf);
  dst.commit(total_len);
  return total_len;
}

template <typename Topic>
inline std::tuple<size_t, asio::const_buffer, Topic> plain_protocol<Topic>::decode(
    const asio::const_buffer& src)
{
  if (src.size() <= get_min_size())
  {
    return {0, {}, {}};  // not enough data to begin decode
  }

  auto rbuf = reinterpret_cast<const char*>(src.data());
  Topic pic{};
  this->deserialize(reinterpret_cast<const uint8_t*>(rbuf), src.size(), pic);

  auto cbuf = asio::const_buffer{rbuf + sizeof(Topic), src.size() - sizeof(Topic)};
  auto sbuf = length_codec_->decode(cbuf);

  if (sbuf.size() > 0)
  {
    auto consumed_len = sizeof(Topic) + length_codec_->length_field_size + sbuf.size();
    return {consumed_len, sbuf, pic};
  }

  return {0, {}, {}};  // a full frame is not received, yet.
}

template <typename Topic>
inline std::pair<size_t, asio::const_buffer> plain_protocol<Topic>::handshake(
    const asio::const_buffer& /* data */)
{
  return {0, {}};  // no handshake required
}

}}}  // namespace play::robust::net
