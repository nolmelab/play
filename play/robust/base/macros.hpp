#pragma once

#include <play/robust/base/logger.hpp>

#define PLAY_RETURN_IF(v) if((v)) return;
#define PLAY_RETURN_VALUE_IF(v, val) if((v)) return (val);
#define PLAY_CHECK(cond) if (!(v)) LOG()->warn("check fail. {} {}@{}", #cond, __FILE__, __LINE__)