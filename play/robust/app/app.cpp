#include <play/robust/app/app.hpp>

namespace play { namespace robust { namespace app {

app& app::get()
{
  static app inst_;
  return inst_;
}

bool app::start(std::string_view jconf)
{
  auto rc1 = server_.start(jconf);
  PLAY_RETURN_VALUE_IF(!rc1, false);

  return on_start();
}

void app::stop()
{
  runner_.stop(); // XXX: needs stop runner_ first?
  server_.stop();

  on_stop();
}

}}}  // namespace play::robust::app