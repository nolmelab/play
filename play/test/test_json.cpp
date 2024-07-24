#include <doctest/doctest.h>
#include <nlohmann/json.hpp>
#include <play/base/json_reader.hpp>

using nlohmann::json;
using namespace play::base;

TEST_CASE("json")
{
  SUBCASE("read 1")
  {
    json j;
    j["hello"] = 3;

    SUBCASE("non-existent key")
    {
      auto v = json_reader::read<int>(j, "a", 0);
      CHECK(v == 0);
    }

    SUBCASE("key match")
    {
      auto v = json_reader::read<int>(j, "hello", 0);
      CHECK(v == 3);
    }
  }

  SUBCASE("read 2")
  {
    json j2 = {{"pi", 3.141},
               {"happy", true},
               {"name", "Niels"},
               {"nothing", nullptr},
               {"answer", {{"everything", 42}},
               {"list", {1, 0, 2}},
               {"object", {{"currency", "USD"}, {"value", 42.99}}}}};

    SUBCASE("nested key")
    {
      auto v = json_reader::read(j2, "answer.everything", 0);
      CHECK(v == 42);
    }

    SUBCASE("object")
    {
      auto v = json_reader::read(j2, "answer", json{});
      CHECK(v == j2["answer"]);
    }
  }
}

TEST_CASE("learn")
{
  SUBCASE("reading json.hpp")
  {
    // - stl이나 boost 스타일의 generic programming을 사용
    // - basic_json, json_value 두 가지가 핵심
    // - 트리 구조를 지원
    // - accessor들
    //   - 구현을 위한 포인터 얻기 등
  }

  SUBCASE("github tutorial")
  {
    SUBCASE("1st")
    {
      json j;
      j["hello"] = 3.14;
      // basic_json constructor called with val == 3.14
      // JSONSerializer<U>::to_json(*this, ...);

      j["object"] = json{};

      auto jv = j["hello"];
      // json : m_data, m_type, m_value [ object | array | string | binary | number_integer | ...]

      auto s1 = j.dump();
      CHECK(s1.length() > 0);
    }
  }

  SUBCASE("2nd")
  {
    json j2 = {{"pi", 3.141},
               {"happy", true},
               {"name", "Niels"},
               {"nothing", nullptr},
               {"answer", {{"everything", 42}},
               {"list", {1, 0, 2}},
               {"object", {{"currency", "USD"}, {"value", 42.99}}}}};

    // what's the magic behind?
    // initializer list를 받아서 basic_json을 만든다.
    // 타잎만 알면 비밀은 없다.
  }
}