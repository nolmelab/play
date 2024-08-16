#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <functional>
#include <optional>
#include <play/app/index.hpp>
#include <type_traits>

namespace play {

// Actor들을 아이디, 이름, 세션 키로 보관하고 검색
/**
 * Actor::get_id(), Actor::get_name(), Actor::get_session_key() 함수를 
 * 템플릿 설정에 따라 constexpr 조건 내에서 호출.
 * 
 * Actor::ptr을 재정의해야 actor::ptr보다 하위 클래스로 보관됨
 */
template <typename Actor, bool HasName = true, bool HasSession = true, bool HasUuid = false>
class actor_container : private index<std::map<size_t, typename Actor::ptr>, std::string>,
                        private index<std::map<size_t, typename Actor::ptr>, uintptr_t>,
                        private index<std::map<size_t, typename Actor::ptr>, boost::uuids::uuid>
{
public:
  using id_type = size_t;
  using name_type = std::string;
  using session_key_type = uintptr_t;
  using ptr = typename Actor::ptr;
  using index_name = index<std::map<id_type, ptr>, std::string>;
  using index_session = index<std::map<id_type, ptr>, uintptr_t>;
  using uuid = boost::uuids::uuid;
  using index_uuid = index<std::map<id_type, ptr>, uuid>;
  using container_type = std::map<id_type, ptr>;

public:
  actor_container()
      : index_name{actors_},
        index_session{actors_},
        index_uuid{actors_}
  {
  }

  // actor를 추가하고 인덱스를 연결
  bool add(ptr actor);

  // 액터와 연관된 인덱스 연결 해제
  template <typename Key, bool IsSessionKey = false>
  void del(const Key& key);

  void del_by_session(const uintptr_t& key)
  {
    del<uintptr_t, true>(key);
  }

  // key 타잎에 따라 인덱스를 사용하고, 액터를 찾아 돌려줌.
  template <typename Key, bool IsSessionKey = false>
  std::optional<ptr> find(const Key& key);

  std::optional<ptr> find_by_session(const uintptr_t& key)
  {
    return find<uintptr_t, true>(key);
  }

  template <typename Fn>
  void for_each(Fn fn)
  {
    for (auto& kv : actors_)
    {
      fn(kv.second);
    }
  }

  size_t size() const
  {
    return actors_.size();
  }

private:
  container_type actors_;
};

}  // namespace play

#include <play/app/actor_container.ipp>