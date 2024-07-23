#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/app/act.hpp>
#include <play/robust/base/stop_watch.hpp>
#include <string>

namespace play { namespace robust { namespace ensure {

class act : public app::act
{
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

  // sig를 부모에게 전달
  /**
   * flow, act_composite와 같은 자식을 갖는 곳에서 override 가능
   */
  virtual void signal(std::string_view sig, std::string_view message);

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

}}}  // namespace play::robust::ensure