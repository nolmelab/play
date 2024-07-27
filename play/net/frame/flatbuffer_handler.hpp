#pragma once

#include <flatbuffers/flatbuffers.h>
#include <play/net/frame_handler.hpp>
#include <play/net/runner.hpp>
#include <shared_mutex>

namespace play {

// 프레임 바이트를 받아서 Frame 포인터를 만들어 전달하는 클래스
/**
 * - subscribe handler with topic 
 * - regiser creator prototype functions for each topic 
 * 
 * server.get_handler().send<app::ack_move>(session, pic, obj);
 */
template <typename Session>
class flatbuffer_handler final : public frame_handler<Session, uint16_t, flatbuffers::NativeTable>
{
public:
  enum class session_state
  {
    established,
    closed
  };

public:
  using frame_handler = frame_handler<Session, uint16_t, flatbuffers::NativeTable>;
  using frame_ptr = typename frame_handler::frame_ptr;
  using topic = typename frame_handler::topic;
  using session_ptr = std::shared_ptr<Session>;
  using unpacker = std::function<frame_ptr(const uint8_t*, size_t)>;
  using receiver = std::function<void(session_ptr, frame_ptr)>;
  using session_cb = std::function<void(session_ptr, session_state)>;

public:
  flatbuffer_handler(runner& runner);

  // flatbuffer_handler를 알고 있는 곳에서만 사용 가능한 함수
  /**
   * send()는 flatbuffers 전용의 전송 편의 함수
   */
  template <typename FlatBufferObj, typename Obj>
  bool send(session_ptr se, topic pic, Obj& obj, bool encrypt = false);

  // 등록된 생성기에서 만든 후 구독한 함수(들)에 전달
  /**
   * recv는 handler들의 공통 인터페이스로 반드시 구현이 필요
   */
  void recv(session_ptr se, topic pic, const void* data, size_t len) final;

  // 연결 프로토콜 협상이 완료
  void established(session_ptr se) final;

  // 단선
  void closed(session_ptr se, error_code ec) final;

  // 처리할 최상위 핸들러 함수들을 토픽에 대해 등록
  /**
   * @return sub 아이디를 돌려준다. uint64_t로 증가하기만 한다. 이를 갖고 취소할 수 있다.
   */
  size_t sub_topic(topic pic, receiver fn);

  // strand_key 0이 아니면 get_runner()에게 strand_key로 핸들러 호출 post
  size_t sub_topic_strand(topic pic, receiver fn, size_t strand_key);

  // 특정 토픽 전체에 대해 통지를 받지 않음
  void unsub_topic(topic pic);

  // 특정 토픽의 특정 핸들러에 대한 구독 중지
  void unsub(topic pic, size_t id);

  // 세션 상태 통지를 구독. 구독 아이디를 돌려줌
  /**
   * 주의: 모든 세션의 연결과 단선에 대해 통지를 받으므로 서비스 수준에서만 구독
   */
  size_t sub_session(session_cb cb);

  // strand_key 0이 아니면 get_runner()에게 strand_key로 핸들러 호출 post
  size_t sub_session_strand(session_cb cb, size_t strand_key);

  // 세션 통지 구독 중지
  void unsub_session(size_t id);

  // 구독 개수를 얻는다.
  size_t get_subs_size(topic pic) const;

  // 정적으로 한번 등록. 락 사용하지 않음. 한번만 등록. 중복되면 경고 로그
  void reg(topic pic, unpacker fn);

public:
  // reg()에 flatbuffers object로 unpack할 함수로 사용하는 템플릿 함수
  /**
   * reg(1, &flatbuffer_handler<server::session_ptr>::unpack<req_move, req_moveT>)와 같이 등록
   */
  template <typename FbObjType, typename ObjType>
  static frame_ptr unpack(const uint8_t* data, size_t len);

private:
  struct sub_entry
  {
    size_t id;
    receiver cb;
    size_t strand_key;
  };

  struct sub_session_entry
  {
    size_t id;
    session_cb cb;
    size_t strand_key;
  };

  using unpacker_map = std::map<topic, unpacker>;
  using receiver_map = std::map<topic, std::vector<sub_entry>>;
  using shared_mutex = std::shared_timed_mutex;
  using session_sub_map = std::map<size_t, sub_session_entry>;

private:
  mutable shared_mutex subs_lock_;
  unpacker_map unpackers_;
  receiver_map receivers_;
  session_sub_map session_subs_;
  size_t current_sub_id_{1};
};

}  // namespace play

#include <play/net/frame/flatbuffer_handler.ipp>