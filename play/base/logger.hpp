#pragma once

#include <spdlog/spdlog.h>
#include <atomic>
#include <memory>
#include <string_view>

namespace play {

/// @brief system logger
/**
 * 
 */
class logger
{
public:
  inline static std::string name = "system";

public:
  static std::shared_ptr<spdlog::logger> get();

  static void dump_hex(spdlog::level::level_enum lvl, std::string_view m, const void* data,
                       size_t len);

  static void dump_hex(spdlog::level::level_enum lvl, std::string_view m, const char* data,
                       size_t len);

private:
  static logger& get_instance();
  static void setup();

private:
  std::atomic<bool> initialized_{false};
};

}  // namespace play

#define LOG() play::logger::get()