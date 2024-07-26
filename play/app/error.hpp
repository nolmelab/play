#pragma once

#include <fmt/format.h>
#include <exception>
#include <play/base/logger.hpp>
#include <string>

namespace play { namespace app {

struct error : public std::exception
{
  error(const std::string& what)
      : what_{what}
  {
  }

  const char* what() const noexcept override
  {
    return what_.c_str();
  }

  std::string what_;

  // throw error::create(...);
  template <typename... Args>
  inline static error create(const std::string& fmt, Args&&... args)
  {
    LOG()->error(fmt, std::forward<Args>(args)...);
    return error(fmt::format(fmt, std::forward<Args>(args)...));
  }
};

}}  // namespace play::app