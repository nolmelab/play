#pragma once

#include <boost/uuid/uuid.hpp>
#include <string>

namespace play {

class uuid
{
public:
  uuid();

  const boost::uuids::uuid& get() const
  {
    return uuid_;
  }

  std::string to_string() const;

private:
  boost::uuids::uuid uuid_;
};

}  // namespace play
