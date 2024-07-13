#pragma once

#include <cassert>
#include <play/robust/base/logger.hpp>

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