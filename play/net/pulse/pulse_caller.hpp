#pragma once

#include <play/net/pulse_listener.hpp>
#include <play/net/session.hpp>

namespace play {

template <typename Protocol, typename Frame>
class pulse_caller
{
public:
  using protocol = Protocol;
  using session = session<Protocol>;
  using frame = Frame;
  using session_ptr = std::shared_ptr<session>;
  using frame_ptr = std::shared_ptr<frame>;
  using topic = typename Protocol::topic;
  using call_receiver = std::function<void(session_ptr, topic)>;

public:
  pulse_caller() = default;

  ~pulse_caller();

  void closed(session_ptr se);

  void call(session_ptr se, topic req, topic res, call_receiver cb);

private:
  struct caller
  {
    call_receiver cb;
  };

  struct topic_calls
  {
    topic req;
    topic res;
    size_t call_count{0};
    size_t recv_count{0};
    std::vector<caller> calls;
  };

  struct session_calls
  {
  };

  using subscriber_map = std::map<topic, std::vector<subscription>>;
  using child_map = std::map<uintptr_t, pulse*>;
  using shared_mutex = std::shared_timed_mutex;
  using interest_key = std::pair<uintptr_t, topic>;
  using interest_map = std::map<interest_key, child_map>;

private:
  // 자식 펄스를 연결
  void bind_child(pulse* child);

  // 자식 펄스 연결 해제
  void unbind_child(pulse* child);

  // 루트에 관심을 알림
  void show_interest(pulse* child, uintptr_t skey, topic pic);

  // 루트에 관심이 없음을 알림
  void lose_interest(pulse* child, uintptr_t skey, topic pic);

  // 구독한 함수들에 전달. 스트랜드 여부 반영. 자식들 dispatch() 호출
  void dispatch(session_ptr se, topic pic, frame_ptr frame, bool from_root = false);

  // 루트에 등록한 관심 그룹 처리를 분리하여 배포 여부를 결정
  bool is_target(uintptr_t sub_key, uintptr_t key, bool from_root) const;

  // 나로 부터 시작하여 부모에서 runner를 얻음
  runner* get_runner();

  // 부모가 없는지 확인
  bool is_root() const;

  // 루트 부모를 얻음
  pulse* get_root();

private:
  mode mode_{mode::none};
  mutable shared_mutex mutex_;
  subscriber_map subscriptions_;
  interest_map interests_;
  runner* runner_{nullptr};
  bool stop_{false};

  std::unique_ptr<server> server_;
  std::unique_ptr<client> client_;
  pulse* parent_{nullptr};
  child_map childs_;

  uint16_t port_{0};
  std::string addr_;
  session_ptr session_;
  size_t strand_key_{0};
};

}  // namespace play

#include <play/net/pulse.ipp>