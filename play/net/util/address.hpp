#pragma once

#include <string>
#include <tuple>

namespace play {

class address
{
public:
  static std::pair<std::string, uint16_t> parse(const std::string& ip_addr);
};

}  // namespace play