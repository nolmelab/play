#pragma once

#include <boost/asio.hpp>

// globally applied alias
namespace asio = boost::asio;

namespace play {

// non-boost 버전의 adapter
using error_code = boost::system::error_code;

}  // namespace play
