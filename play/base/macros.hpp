#pragma once

#include <cassert>
#include <play/base/logger.hpp>

#define PLAY_RETURN_IF(cond) \
  if ((cond))                \
    return;

#define PLAY_RETURN_VALUE_IF(cond, val) \
  if ((cond))                           \
    return (val);

#define PLAY_CHECK(cond)                                            \
  if (!(cond))                                                      \
  {                                                                 \
    LOG()->warn("check fail. {} {}@{}", #cond, __FILE__, __LINE__); \
    assert(cond);                                                   \
  }

#define PLAY_UNUSED(expr) \
  do                      \
  {                       \
    (void)(expr);         \
  } while (0)
