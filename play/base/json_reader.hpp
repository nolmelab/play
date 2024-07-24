#pragma once

#include <nlohmann/json.hpp>
#include <play/base/string_util.hpp>

namespace play { namespace base {

class json_reader
{
public:
  inline static const nlohmann::json& read(const nlohmann::json& json, const std::string& path)
  {
    static nlohmann::json null;
    auto vpath = string_util::split(path, ".");
    auto* jv = &json;

    for (size_t i = 0; i < vpath.size() - 1; ++i)
    {
      if (jv->contains(vpath[i]))
      {
        jv = &(jv->at(vpath[i]));
        continue;
      }

      return null;
    }

    if (jv->contains(vpath.back()))
    {
      jv = &(jv->at(vpath.back()));
      return *jv;
    }

    return null;  //
  }

  // json에서 경로 형식으로 읽고, 못 찾으면 기본 값을 돌려준다.
  template <typename Value>
  inline static Value read(const nlohmann::json& json, const std::string& path,
                           Value default_value = Value{})
  {
    auto jr = read(json, path);

    if (jr.is_null())
    {
      return default_value;
    }

    return jr.get<Value>();  //
  }
};

}}  // namespace play::base