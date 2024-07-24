#include <play/robust/base/json_reader.hpp>
#include <play/robust/base/logger.hpp>
#include <play/robust/base/macros.hpp>
#include <play/robust/ensure/act_composite.hpp>
#include <play/robust/ensure/act_factory.hpp>

namespace play { namespace robust { namespace ensure {

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

    auto ap = act_factory::get().create(type, get_owner(), shared(), jact, name);
    PLAY_CHECK(ap);
    if (ap)
    {
      acts_.push_back(ap);
    }
    else
    {
      LOG()->error("act creator not found for type: {}, name: {}", type, name);
      return false;  // better to stop than suffering from the problems
    }
  }

  return true;
}

}}}  // namespace play::robust::ensure