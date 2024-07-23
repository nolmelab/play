#pragma once

#include <string>
#include <vector>

namespace play { namespace robust { namespace base {

class string_util
{
public:
  // 문자열을 구분자로 자르는 함수. 구분자 중복이나 시작, 끝일 경우 공백 문자열 추가 없음
  static std::vector<std::string> split(const std::string& in, const std::string& delimiter = " ")
  {
    if (in.empty())
    {
      return {};
    }

    std::vector<std::string> result;

    size_t ppos = 0;
    size_t cpos = in.find_first_of(delimiter);
    while (cpos != std::string::npos)
    {
      if (cpos > ppos)
      {
        result.push_back(in.substr(ppos, (cpos - ppos)));
      }
      ppos = cpos + delimiter.length();
      cpos = in.find_first_of(delimiter, ppos);
    }

    if (ppos < in.length())
    {
      result.push_back(in.substr(ppos));
    }

    return result;
  }
};

}}}  // namespace play::robust::base