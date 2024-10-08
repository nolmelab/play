#pragma once

#include <flatbuffers/flatbuffers.h>
#include <play/net/pulse.hpp>

namespace play {

template <typename Protocol>
class pulse_fb : public pulse<Protocol, flatbuffers::NativeTable>
{
public:
  using base = pulse<Protocol, flatbuffers::NativeTable>;
  using protocol = Protocol;
  using session = session<Protocol>;
  using frame = flatbuffers::NativeTable;
  using session_ptr = std::shared_ptr<session>;
  using frame_ptr = std::shared_ptr<frame>;
  using topic = typename Protocol::topic;
  using receiver = std::function<void(session_ptr, frame_ptr)>;
  using call_receiver = std::function<void()>;
  using unpacker = std::function<frame_ptr(const uint8_t*, size_t)>;

public:
  // flatbuffer_handler를 알고 있는 곳에서만 사용 가능한 함수
  /**
   * send()는 flatbuffers 전용의 전송 편의 함수
   */
  template <typename FlatBufferObj, typename TopicInput, typename Obj>
  static bool send(session_ptr se, TopicInput topic_in, Obj& obj, bool encrypt = false);

  // 정적으로 한번 등록. 락 사용하지 않음. 한번만 등록. 중복되면 경고 로그
  static void add_unpacker(topic pic, unpacker fn);

  // add_unpacker()에 flatbuffers object로 unpack할 함수로 사용하는 템플릿 함수
  /**
   * add_unpacker(1, &flatbuffer_handler<server::session_ptr>::unpack<req_move, req_moveT>)와 같이 등록
   */
  template <typename FbObjType, typename ObjType>
  static frame_ptr unpack_fn(const uint8_t* data, size_t len);

public:
  pulse_fb() = default;

  // 세션이 있을 경우 내부 세션을 사용하여 전송
  template <typename FlatBufferObj, typename TopicInput, typename Obj>
  bool send(TopicInput topic_in, Obj& obj, bool encrypt = false)
  {
    PLAY_CHECK(this->get_session());
    return this->send<FlatBufferObj>(this->get_session(), topic_in, obj, encrypt);
  }

  // 세션에 대해 call() 요청을 보내고, 세션 단선시 call_receiver를 호출
  template <typename FlatBufferObj, typename TopicInput, typename Obj>
  void call(TopicInput req, TopicInput res, Obj& obj, call_receiver cb, bool encrypt = false);

  // 단선 상태만 체크하여 에러 처리하는 함수
  template <typename FlatBufferObj, typename TopicInput, typename Obj>
  void call(TopicInput req, Obj& obj, call_receiver cb, bool encrypt = false);

private:
  // 플랫버퍼 프레임인 NatvieTable을 등록된 unpacker들에서 생성
  frame_ptr unpack(topic pic, const uint8_t* data, size_t len) final;

private:
  using unpacker_map = std::map<topic, unpacker>;

private:
  inline static unpacker_map unpackers_;
};

}  // namespace play

#include <play/net/pulse_fb.ipp>