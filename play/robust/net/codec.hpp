#pragma 

#include <boost/asio/streambuf.hpp>

namespace play { namespace robust { namespace net {

// codec translates buffer with encode and decode
class codec
{
public:
  using streambuf = boost::asio::streambuf;
  using const_buffer = boost::asio::const_buffer;
  using mutable_buffer = boost::asio::mutable_buffer;
};

}}} // play::robust::net