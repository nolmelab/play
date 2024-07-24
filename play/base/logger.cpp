#include <play/base/logger.hpp>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sstream>

namespace play { namespace base {

void logger::setup()
{
  get_instance().initialized_ = true;

  spdlog::init_thread_pool(32 * 1024, 1);
  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
      "logs/system.log", 0, 0);
  std::vector<spdlog::sink_ptr> sinks{stdout_sink, daily_sink};

  auto logger = std::make_shared<spdlog::async_logger>(
      "system", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::register_logger(logger);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%L%$][%t] %v");
}

std::shared_ptr<spdlog::logger> logger::get()
{
  auto& logger = get_instance();
  if (!logger.initialized_)
  {
    setup();
  }

  return spdlog::get("system");
}

logger& logger::get_instance()
{
  static logger instance_;
  return instance_;
}

void logger::dump_hex(spdlog::level::level_enum lvl, std::string_view m,
                      const void* data, size_t len)
{
  dump_hex(lvl, m, reinterpret_cast<const char*>(data), len);
}

void logger::dump_hex(spdlog::level::level_enum lvl, std::string_view m,
                      const char* data, size_t len)
{
  std::ostringstream oss;
  for (size_t i = 0; i < len; ++i)
  {
    oss << std::setw(2) << std::hex
        << (static_cast<unsigned int>(data[i]) & 0xFF) << "|";
  }
  LOG()->log(lvl, "{}. dump: {}", m, oss.str());
}

}}  // namespace play::base