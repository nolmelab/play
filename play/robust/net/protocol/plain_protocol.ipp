#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/plain_protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic>
inline void plain_protocol<Topic>::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);

  accepted_ = true;
  closed_ = false;

  length_codec_ = std::make_unique<length_delimited>(handle_);
}

template <typename Topic>
inline void plain_protocol<Topic>::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);
  PLAY_CHECK(!length_codec_);

  connected_ = true;
  closed_ = false;

  length_codec_ = std::make_unique<length_delimited>(handle_);

  get_adapter().established(handle_);
}

template <typename Topic>
inline void plain_protocol<Topic>::closed()
{
  PLAY_RETURN_IF(closed_);
  closed_ = true;
}

template <typename Topic>
inline void plain_protocol<Topic>::send(Topic topic, const char* data, size_t len)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("send called on closed session. handle: {}", handle_);
    return;
  }

  PLAY_CHECK(is_established());

  auto m_1 = send_buf_.prepare(sizeof(Topic));
  this->serialize(m_1, topic);
  send_buf_.commit(sizeof(Topic));

  auto payload_buf = asio::const_buffer{data, len};

  auto result = length_codec_->encode(payload_buf, send_buf_);
  PLAY_CHECK(result);  // 여기서 실패하는 경우는 없다

  auto send_data = send_buf_.data();
  get_adapter().send(send_data.data(), send_data.size());
  send_buf_.consume(send_data.size());
}

template <typename Topic>
inline void plain_protocol<Topic>::receive(const char* data, size_t len)
{
  const char* payload = reinterpret_cast<const char*>(data);
  auto mut_buf = recv_buf_.prepare(len);
  recv_buf_.sputn(payload, len);
  // commit() 불필요

  Topic topic{0};

  auto next_frame = [&topic, this]() -> asio::const_buffer
  {
    auto d_1 = recv_buf_.data();
    if (d_1.size() <= get_min_size())
    {
      return {};
    }
    this->deserialize(d_1, topic);
    recv_buf_.consume(sizeof(topic));

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
    get_adapter().forward(topic, length_buf.data(), length_buf.size());

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
