#include <play/app/index.hpp>

namespace play {

template <typename Container, typename IndexKey>
inline index<Container, IndexKey>::index(container_type& container)
    : container_{container}
{
}

template <typename Container, typename IndexKey>
inline void index<Container, IndexKey>::link(const index_key_type& index, const key_type& key)
{
  index_.insert(std::pair{index, key});
}

template <typename Container, typename IndexKey>
inline void index<Container, IndexKey>::unlink(const index_key_type& index)
{
  index_.erase(index);
}

template <typename Container, typename IndexKey>
inline typename index<Container, IndexKey>::iterator_type index<Container, IndexKey>::find(
    const index_key_type& key)
{
  auto iter = index_.find(key);
  if (iter == index_.end())
  {
    return container_.end();
  }
  auto target_iter = container_.find(iter->second);
  return target_iter;
}

template <typename Container, typename IndexKey>
inline typename index<Container, IndexKey>::const_iterator_type index<Container, IndexKey>::find(
    const index_key_type& key) const
{
  auto iter = index_.find(key);
  if (iter == index_.end())
  {
    return container_.cend();
  }
  auto target_iter = container_.find(iter->second);
  return target_iter;
}

}  // namespace play