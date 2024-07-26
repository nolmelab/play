#include <play/net/runner.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace play {

runner::runner(std::string_view name, size_t strand_count)
    : ioc_{},
      name_{name},
      timer_service_{ioc_}
{
  make_strands(strand_count);
}

void runner::sleep(int ms)
{
#ifdef _WIN32
  ::Sleep(ms);
#else
  ::usleep(ms * 1000);
#endif
}

void runner::make_strands(size_t strand_count)
{
  for (size_t i = 0; i < strand_count; ++i)
  {
    strands_.push_back(strand_type(get_ioc()));
  }
}

}  // namespace play::net
