#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <play/base/uuid.hpp>

namespace play {

uuid::uuid()
{
  uuid_ = boost::uuids::random_generator()();
}

std::string uuid::to_string() const
{
  return boost::uuids::to_string(uuid_);
}

}  // namespace play