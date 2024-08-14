#include <play/app/actor_container.hpp>

namespace play {

template <typename Actor, bool HasName, bool HasSession, bool HasUuid>
inline bool actor_container<Actor, HasName, HasSession, HasUuid>::add(ptr actor)
{
  auto pv = actors_.insert(std::pair{actor->get_id(), actor});
  if (pv.second)
  {
    if constexpr (HasName)
    {
      index_name::link(actor->get_name(), actor->get_id());
    }

    if constexpr (HasSession)
    {
      index_session::link(actor->get_session_key(), actor->get_id());
    }

    if constexpr (HasUuid)
    {
      index_uuid::link(actor->get_uuid(), actor->get_id());
    }
  }
  return pv.second;
}

template <typename Actor, bool HasName, bool HasSession, bool HasUuid>
template <typename Key, bool IsSessionKey>
inline void actor_container<Actor, HasName, HasSession, HasUuid>::del(const Key& key)
{
  typename container_type::iterator iter;

  if constexpr (IsSessionKey)
  {
    static_assert(std::is_same_v<Key, session_key_type>);
    static_assert(HasSession);
    iter = index_session::find(key);
  }
  else
  {
    if constexpr (std::is_same_v<Key, id_type>)
    {
      iter = actors_.find(key);
    }
    else if constexpr (std::is_same_v<Key, name_type>)
    {
      static_assert(HasName);
      iter = index_name::find(key);
    }
    else
    {
      static_assert(HasUuid);
      static_assert(std::is_same_v<Key, uuid>);
      iter = index_uuid::find(key);
    }
  }

  if (iter != actors_.end())
  {
    actors_.erase(iter);

    if constexpr (HasName)
    {
      index_name::unlink(iter->second->get_name());
    }

    if constexpr (HasSession)
    {
      index_session::unlink(iter->second->get_session_key());
    }

    if constexpr (HasUuid)
    {
      index_uuid::unlink(iter->second->get_uuid());
    }
  }
}

template <typename Actor, bool HasName, bool HasSession, bool HasUuid>
template <typename Key, bool IsSessionKey>
inline std::optional<typename actor_container<Actor, HasName, HasSession, HasUuid>::ptr>
actor_container<Actor, HasName, HasSession, HasUuid>::find(const Key& key)
{
  typename container_type::iterator iter;

  if constexpr (IsSessionKey)
  {
    static_assert(std::is_same_v<Key, session_key_type>);
    static_assert(HasSession);
    iter = index_session::find(key);
  }
  else
  {
    if constexpr (std::is_same_v<Key, id_type>)
    {
      iter = actors_.find(key);
    }
    else if constexpr (std::is_same_v<Key, name_type>)
    {
      static_assert(HasName);
      iter = index_name::find(key);
    }
    else
    {
      static_assert(HasUuid);
      static_assert(std::is_same_v<Key, uuid>);
      iter = index_uuid::find(key);
    }
  }

  if (iter == actors_.end())
  {
    return {};
  }
  return {iter->second};
}

}  // namespace play
