#include <play/base/string_util.hpp>
#include <play/net/util/address.hpp>

namespace play {

std::pair<std::string, uint16_t> address::parse(const std::string& ip_addr)
{
  auto vs = string_util::split(ip_addr, ":");
  if (vs.size() < 2)
  {
    return {{}, 0};
  }

  return {vs[0], std::atoi(vs[1].c_str())};
}

}  // namespace play