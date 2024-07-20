#include <doctest/doctest.h>
#include <flatbuffers/flatbuffers.h>
#include "learn_fb_generated.h"

using namespace app::sample;

TEST_CASE("learn flatbuffers")
{
  SUBCASE("compile")
  {
    // [1] install flatbuffers-compiler
    // [2] install flatubffers vcpkg
    // [3] include flatbuffers.h
    // [4] make a sample fbs
    // - install vscode fbs extension
    // [5] flatc --cpp file.fbs

    // version mismatch: VerifyField is not found
    // - vcpkg version: 24.3.25
    // - vcpkg의 buildtrees 아래의 flatc를 사용하도록 수정
    // - 다시 fbs 컴파일 OK
    //
  }

  SUBCASE("using generated schema")
  {
    flatbuffers::FlatBufferBuilder builder(1024);

    SUBCASE("CreateWeapon") {}
  }

  SUBCASE("object api") {}

  SUBCASE("FlatBufferBuilder") {}

  SUBCASE("verification") {}

  SUBCASE("how it works?") {}

  SUBCASE("how to use in an app?")
  {
    //
  }
}
