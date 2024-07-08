#pragma once 

#include <atomic>
#include <memory>
#include <spdlog/spdlog.h>

namespace play { namespace robust { namespace base {

/// @brief system logger
/**
 * 
 */
class logger
{
public:
  static std::shared_ptr<spdlog::logger> get(); 

private:
  static logger& get_instance();
  static void setup();

private:
  std::atomic<bool> initialized_{false};
};

} } } // play::robust::base

#define LOG() play::robust::base::logger::get()