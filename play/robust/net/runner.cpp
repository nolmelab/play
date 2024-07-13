#include <play/robust/net/runner.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace play { namespace robust { namespace net {

runner::runner(std::string_view name) : ioc_{}, name_{name} {}

void runner::sleep(int ms)
{
#ifdef _WIN32
  ::Sleep(ms);
#else
  ::usleep(ms * 1000);
#endif
}

}}}  // namespace play::robust::net
