#include <doctest/doctest.h>
#include <boost/uuid/uuid_io.hpp>
#include <play/app/actor.hpp>
#include <play/app/actor_container.hpp>

namespace {

class user : public play::actor
{
public:
  using ptr = std::shared_ptr<user>;

public:
  user(const std::string& name, uintptr_t skey = 0)
      : name_{name},
        uuid_{boost::uuids::random_generator()()},
        skey_{skey}
  {
  }

  const std::string& get_name() const
  {
    return name_;
  }

  const boost::uuids::uuid& get_uuid() const
  {
    return uuid_;
  }

  uintptr_t get_session_key() const
  {
    return skey_;
  }

  bool start() final
  {
    return true;
  }

  void stop() final {}

private:
  std::string name_;
  boost::uuids::uuid uuid_;
  uintptr_t skey_;
};

}  // namespace

TEST_CASE("actor container")
{
  SUBCASE("basic usage")
  {
    play::actor_container<user, true, false, true> users;

    users.add(std::make_shared<user>("hello"));
    CHECK(users.find(std::string{"hello"}).has_value());
    CHECK(users.find(std::string{"hello"}).value().get()->get_name() == std::string{"hello"});

    // static_assert fail. uintptr_t와 size_t가 같아서 태그가 필요
    // users.find<uintptr_t, true>(3);

    auto rv = users.find(boost::uuids::random_generator()());
    CHECK(!rv.has_value());

    auto user = users.find(std::string{"hello"});
    LOG()->info("uuid: {}, size: {}", boost::uuids::to_string(user.value().get()->get_uuid()),
                sizeof(boost::uuids::uuid));

    auto user2 = users.find(user.value().get()->get_uuid());
    CHECK(user2.has_value());

    users.del(user2.value().get()->get_uuid());

    auto user3 = users.find(user.value().get()->get_uuid());
    CHECK(!user3.has_value());
  }

  SUBCASE("id")
  {
    play::actor_container<user, false, false, false> users;

    auto u1 = std::make_shared<user>("hello");
    users.add(u1);
    CHECK(users.find(u1->get_id()));
    CHECK(users.find(u1->get_id()).value().get()->get_name() == std::string{"hello"});

    users.del(u1->get_id());
    CHECK(!users.find(u1->get_id()));
  }

  SUBCASE("session key")
  {
    play::actor_container<user, false, true, false> users;

    auto u1 = std::make_shared<user>("hello", 5);
    users.add(u1);
    CHECK(users.find_by_session(u1->get_session_key()));
    CHECK(users.find_by_session(u1->get_session_key()).value().get()->get_name() ==
          std::string{"hello"});

    users.del<uintptr_t, true>(u1->get_session_key());
    CHECK(!users.find_by_session(u1->get_session_key()));
  }

  SUBCASE("name")
  {
    play::actor_container<user, true, false, false> users;

    auto u1 = std::make_shared<user>("hello");
    users.add(u1);
    CHECK(users.find(u1->get_name()));
    CHECK(users.find(u1->get_name()).value().get()->get_name() == std::string{"hello"});

    users.del(u1->get_name());
    CHECK(!users.find(u1->get_name()));
  }
}