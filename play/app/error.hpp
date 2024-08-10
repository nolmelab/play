#pragma once

#include <fmt/format.h>
#include <exception>
#include <play/base/logger.hpp>
#include <string>

namespace play {

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
  inline static error create(std::string_view fmt, Args&&... args)
  {
    LOG()->error(fmt, std::forward<Args>(args)...);
    return error(fmt::format(fmt, std::forward<Args>(args)...));
  }
};

}  // namespace play