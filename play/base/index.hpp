#pragma once

namespace play {

template <typename Container, typename IndexKey>
class index
{
public:
  using container_type = Container;
  using iterator_type = typename Container::iterator;
  using const_iterator_type = typename Container::const_iterator;
  using key_type = typename Container::key_type;
  using index_key_type = IndexKey;

public:
  index(container_type& container);

  void link(const index_key_type& index, const key_type& key);

  void unlink(const index_key_type& index);

  iterator_type find(const index_key_type& key);

  const_iterator_type find(const index_key_type& key) const;

private:
  container_type& container_;
  std::map<index_key_type, key_type> index_;
};

}  // namespace play

#include <play/base/index.ipp>