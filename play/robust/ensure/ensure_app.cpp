#include <play/robust/ensure/ensure_app.hpp>

namespace play { namespace robust { namespace ensure {

ensure_app::ensure_app() {}

bool ensure_app::start(std::string_view jconf)
{
  // TODO: server는 nlohmann::json을 입력으로 받아야 한다.
  // 그래야 다른 곳에서 자유롭게 지정할 수 있다.

  app::app::start(jconf);
}

void ensure_app::stop() {}

bool ensure_app::on_start() {}

void ensure_app::on_stop() {}

}}}  // namespace play::robust::ensure
