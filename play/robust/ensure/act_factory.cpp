#include <play/robust/ensure/act_factory.hpp>
#include <play/robust/base/macros.hpp>

namespace play { namespace robust { namespace ensure {

act_factory& act_factory::get()
{
    static act_factory inst_;
    return inst_;
}

act::ptr act_factory::create(std::string_view name)
{
  auto iter = creators_.find(std::string{name});
  if ( iter == creators_.end())
  {
    return {};
  }
  return iter->second();
}

void act_factory::reg(std::string_view name, creator fn)
{
  auto result = creators_.insert(std::pair{name, fn});
  PLAY_CHECK(result.second);
}

}}} // 