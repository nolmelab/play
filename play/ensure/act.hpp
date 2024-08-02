#pragma once

#include <nlohmann/json.hpp>
#include <play/app/act.hpp>
#include <play/base/stop_watch.hpp>
#include <string>

namespace ensure {

// flow를 구성하는 act.
/**
 * actor의 컴포넌트 클래스이지만 여기서는 act_composite인
 * act_serial, act_parallel에 포함하여 사용. 
 * 
 * actor의 컴포넌트로도 사용할 수 있음 
 */
class act : public play::act
{
public:
  using ptr = std::shared_ptr<act>;  // hide app::act::ptr

public:
  struct path
  {
    // /로 시작하는 절대 경로인지 확인
    static bool is_absolute_path(const std::string& path);

    // / 없이 시작하는 상대 경로인지 확인. (.과 ..은 지원하지 않음)
    static bool is_relative_path(const std::string& path);

    // 마지막 act 이름 얻기
    static std::string get_last_act(const std::string& path);

    // path 절대 경로가 self_path 절대 경로의 자식인지 확인
    static bool is_child_of(const std::string& self_path, const std::string& path);

    // 절대 경로에서 path의 self_path의 자식 경로를 얻음
    static std::string get_child_path(const std::string& self_path, const std::string& path);

    // 상대 경로에서 path의 자식 경로를 얻음. 최상위가 현재 액트
    static std::string get_child_path(const std::string& path);

    // 상대 경로 또는 절대 경로에서 첫 act를 얻음
    static std::string get_first_act(const std::string& path);

    // 제일 앞의 경로를 제거
    static std::string pop_head_act(const std::string& path);

    // 전체 경로를 받아서 설정
    void setup(const std::string& spath);

    std::string full_path_;
    std::vector<std::string> parts_;
    std::string act_name_;
  };

public:
  act(play::actor& owner, act::ptr parent, const nlohmann::json& json, const std::string& name)
      : play::act(owner),
        parent_{parent},
        json_{json},
        name_{name},
        active_{false}
  {
    build_path();
    load_json();
  }

  bool activate();

  void update();

  void deactivate();

  // jump to the act at path.
  virtual void jump(const std::string& path);

  // move to the next act (act_serial)
  virtual void next();

  // exit by stop bot
  virtual void exit();

  ptr find(const std::string& path);

  const path& get_path() const
  {
    return path_;
  }

  const std::string& get_name() const
  {
    return name_;
  }

  ptr get_parent() const
  {
    return parent_;
  }

  bool has_parent() const
  {
    return !!parent_;
  }

  bool is_active() const
  {
    return active_;
  }

  size_t get_active_count() const
  {
    return active_count_;
  }

  play::stop_watch& get_stop_watch()
  {
    return stop_watch_;
  }

protected:
  // 하위 클래스의 활성 로직 구현
  virtual bool on_activate();

  virtual void on_update();

  // 하위 클래스의 비활성 로직 구현
  virtual void on_deactivate();

  // activate()에서 on_activate() 호출 전 실행
  virtual void on_load_json();

  // 지정된 명령에 따라 부모에 요청
  void signal(std::string_view sig, std::string_view message);

  void succed(std::string_view message)
  {
    signal("success", message);
  }

  void fail(std::string_view message)
  {
    signal("fail", message);
  }

  const nlohmann::json& get_json() const
  {
    return json_;
  }

  ptr self()
  {
    return std::static_pointer_cast<act>(this->shared_from_this());
  }

  bool is_self(const std::string& path) const;

  ptr find_up(const std::string& path);

  virtual ptr find_child(const std::string& path);

private:
  void build_path();

  void load_json();

private:
  play::stop_watch stop_watch_;
  std::string name_;
  ptr parent_;
  path path_;
  nlohmann::json json_;
  bool active_{false};
  size_t active_count_{0};
};

}  // namespace ensure
