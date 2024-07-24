#pragma once

#include <nlohmann/json.hpp>
#include <play/app/act.hpp>
#include <play/base/stop_watch.hpp>
#include <string>

namespace play { namespace ensure {

class act : public app::act
{
public:
  struct path;

public:
  act(app::actor& owner, act::ptr parent, const nlohmann::json& json, const std::string& name)
      : app::act(owner),
        parent_{parent},
        json_{json},
        name_{name},
        active_{false}
  {
    build_path();
  }

  bool activate();

  void deactivate();

  const std::string& get_path() const
  {
    return path_;
  }

  const std::string& get_name() const
  {
    return name_;
  }

  std::shared_ptr<act> get_parent() const
  {
    return std::static_pointer_cast<act>(parent_);
  }

  base::stop_watch& get_stop_watch()
  {
    return stop_watch_;
  }

protected:
  virtual bool on_activate();

  virtual void on_deactivate();

  virtual void on_load_json();

  virtual void jump(const std::string& path);

  virtual void next();

  virtual void exit();

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

private:
  void build_path();

private:
  base::stop_watch stop_watch_;
  std::string name_;
  act::ptr parent_;
  std::string path_;
  nlohmann::json json_;
  bool active_;
};

struct act::path
{
  // /로 시작하는 절대 경로인지 확인
  static bool is_absolute_path(const std::string& path);

  // / 없이 시작하는 상대 경로인지 확인. (.과 ..은 지원하지 않음)
  static bool is_relative_path(const std::string& path);

  // 마지막 act 이름 얻기
  static std::string get_last_act_name(const std::string& path);

  // path 절대 경로가 self_path 절대 경로의 자식인지 확인
  static std::string is_chlid_of(const std::string& self_path, const std::string& path);

  // 절대 경로에서 path의 self_path의 자식 경로를 얻음
  static std::string get_child_path(const std::string& self_path, const std::string& path);
};

}}  // namespace play::ensure