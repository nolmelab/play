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

    auto weapon_one_name = builder.CreateString("Sword");
    short weapon_one_damage = 3;

    auto weapon_two_name = builder.CreateString("Axe");
    short weapon_two_damage = 5;

    // Use the `CreateWeapon` shortcut to create Weapons with all the fields set.
    auto sword = Createweapon(builder, weapon_one_name, weapon_one_damage);
    auto axe = Createweapon(builder, weapon_two_name, weapon_two_damage);

    // Serialize a name for our monster, called "Orc".
    auto name = builder.CreateString("Orc");

    // Create a `vector` representing the inventory of the Orc. Each number
    // could correspond to an item that can be claimed after he is slain.
    unsigned char treasure[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto inventory = builder.CreateVector(treasure, 10);
    // struct와 table의 구분은 중요하고, table이 핵심이다.
    // builder 내에 table의 내용이 다른 table을 포함하여 플랫하게 트리 내용들이 들어간다.
    // 이 트리의 내용은 depth first, pre-order로 저장한다.
    std::vector<flatbuffers::Offset<weapon>> weapons_vector;
    weapons_vector.push_back(sword);
    weapons_vector.push_back(axe);
    auto weapons = builder.CreateVector(weapons_vector);

    vec3 points[] = {vec3(1.0f, 2.0f, 3.0f), vec3(4.0f, 5.0f, 6.0f)};
    auto path = builder.CreateVectorOfStructs(points, 2);

    // Create the position struct
    auto position = vec3(1.0f, 2.0f, 3.0f);

    // Set his hit points to 300 and his mana to 150.
    int hp = 300;
    int mana = 150;

    // Finally, create the monster using the `CreateMonster` helper function
    // to set all fields.
    auto orc = Createmonster(builder, &position, mana, hp, name, inventory, color_red, weapons,
                             equipment_weapon, axe.Union(), path);

    builder.Finish(orc);

    auto buffer = builder.GetBufferPointer();
    auto size = builder.GetSize();

    auto orc_2 = flatbuffers::GetRoot<monster>(buffer);
    CHECK(orc_2->hp() == hp);

    SUBCASE("notes on schema")
    {
      // [1] naming convention을 snake로 하면 잘 맞지 않지만 object-api를 사용하면
      // 특별히 문제될 부분은 없다. 해당 코드를 볼 일이 거의 없기 때문이다.

      // [2] builder의 평면 버퍼에 트리가 저장되는 과정은 reverse vector를 사용하여 진행된다.
    }
  }

  SUBCASE("object api")
  {
    flatbuffers::FlatBufferBuilder builder(1024);
    monsterT m;
    m.hp = 300;
    builder.Finish(monster::Pack(builder, &m));

    auto buffer = builder.GetBufferPointer();

    // 읽어올 오브젝트
    monsterT m2;
    // GetRoot<T>로 루트 테이블에 대한 포인터를 얻는다.
    auto p_m2 = flatbuffers::GetRoot<monster>(buffer);
    // Verifier를 옵션을 갖고 만든다.
    flatbuffers::Verifier verifier(buffer, builder.GetSize(), flatbuffers::Verifier::Options{});
    // Vierify를 한다.
    auto result = p_m2->Verify(verifier);
    CHECK(result);
    // 타겟 오브젝트에 쓴다.
    p_m2->UnPackTo(&m2);

    CHECK(m2.hp == m.hp);
  }
}
