#include <play/base/json_reader.hpp>
#include <play/base/logger.hpp>
#include <play/base/macros.hpp>
#include <play/ensure/act_composite.hpp>
#include <play/ensure/act_factory.hpp>

namespace play { namespace ensure {

bool act_composite::load_acts()
{
  // 복합 액트는 항상 flow를 찾을 수 있다.
  auto jflow = get_json()["flow"];

  if (jflow.is_null())
  {
    LOG()->error("act_composite: {} cannot find flow entry", get_name());
    return false;
  }

  for (auto& jact : jflow)
  {
    auto type = base::json_reader::read(jact, "type", std::string{});
    auto name = base::json_reader::read(jact, "name", std::string{});

    auto ap = act_factory::get().create(type, get_owner(), self(), jact, name);
    if (ap)
    {
      acts_.push_back(ap);
    }
    else
    {
      LOG()->error("type: {} not found in act_factory for name: {}", type, name);
      return false;  // better to stop than suffering from the problems
    }
  }

  return true;
}

}}  // namespace play::ensure