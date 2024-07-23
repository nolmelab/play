#pragma once

#include <nlohmann/json.hpp>

namespace play { namespace robust { namespace base {

class json_reader
{
public:
  // json에서 경로 형식으로 읽고, 못 찾으면 기본 값을 돌려준다.
  template <typename Value>
  static Value read(const nlohmann::json& json, const std::string& path, Value default_value = Value{})
  {

  }
}

}}}  // namespace play::robust::base