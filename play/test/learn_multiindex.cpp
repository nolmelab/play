#include <doctest/doctest.h>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <streambuf>

namespace bmi = boost::multi_index;

namespace {

struct User
{
  std::string name;
  uintptr_t session_key;
  // lobby_user::ptr user;
};

struct index_name
{
};

struct index_session
{
};

using UserContainer = bmi::multi_index_container<
    User,
    bmi::indexed_by<
        // The "primary key"
        bmi::ordered_unique<bmi::tag<index_name>, bmi::member<User, std::string, &User::name>>,
        bmi::ordered_unique<bmi::tag<index_session>,
                            bmi::member<User, uintptr_t, &User::session_key>>>>;
}  // namespace

TEST_CASE("learn multi_index_container")
{
  SUBCASE("user container")
  {
    // session과 user_id로 검색 가능하게 함
    UserContainer users;
    users.insert({"hello", 3});
    users.insert({"hello_2", 5});

    // 인덱스를 얻은 후에 찾는 구조
    auto& name_index = users.get<index_name>();
    auto iter = name_index.find("hello");
    CHECK(iter->name == std::string{"hello"});

    // member 접근이 필요한 경우가 많고 public이어야 가능하므로 인덱스를 위한
    // 필드는 노출하여 처리해야 한다.
    // BOOST_MULTI_INDEX_CONST_MEM_FUN 으로 멤버 함수에 접근하여 인덱스 처리를 할 수 있다.
    // 내부에 포함하고 접근 가능하게 숨기는 것이 좋아 보인다.
  }
}

namespace {

class lobby_user
{
public:
  lobby_user(const std::string& name, uintptr_t session_key)
      : name_{name},
        session_key_{session_key}
  {
  }

  const std::string& get_name() const
  {
    return name_;
  }

  uintptr_t get_session_key() const
  {
    return session_key_;
  }

private:
  std::string name_;
  uintptr_t session_key_;
};

class user_container
{
public:
  void add(lobby_user&& user)
  {
    users_.insert(std::move(user));
  }

private:
  using multi_index_container = bmi::multi_index_container<
      lobby_user,
      bmi::indexed_by<
          // The "primary key"
          bmi::ordered_unique<bmi::tag<index_name>, BOOST_MULTI_INDEX_CONST_MEM_FUN(
                                                        lobby_user, const std::string&, get_name)>,
          bmi::ordered_unique<bmi::tag<index_session>,
                              BOOST_MULTI_INDEX_CONST_MEM_FUN(lobby_user, uintptr_t,
                                                              get_session_key)>>>;

  multi_index_container users_;
};

}  // namespace

TEST_CASE("user container design")
{
  SUBCASE("usage")
  {
    user_container users;

    users.add({std::string{"name"}, 3});
  }
}