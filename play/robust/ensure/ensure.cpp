#include <play/robust/ensure/ensure.hpp>

namespace play { namespace robust { namespace ensure {

ensure::ensure()
{
  //
}

bool ensure::start(std::string_view jconf)
{

  base::start(jconf);
}

void ensure::stop()
{
  on_stop();
}

}}}  // namespace play::robust::ensure
