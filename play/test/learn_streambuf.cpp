#include <doctest/doctest.h>
#include <boost/asio/streambuf.hpp>
#include <streambuf>

using streambuf = boost::asio::streambuf;

/**
 * streambuf 사용의 핵심은 다음과 같다. 
 * - gptr()이 읽기(get) 포인터, pptr()이 쓰기(put) 포인터이다. 
 * - consume()은 읽기 포인터를 앞으로 읽고 이동하는데 쓴다. 
 * - commit()은 쓰기 완료시 다음 쓸 위치를 앞으로 옮긴다. 
 * - prepare()에서 빈 곳으로 이동하여 공간을 추가로 확보한다. 
 */
TEST_CASE("learn streambuf")
{
  SUBCASE("basics")
  {
    streambuf buf;  // constructor creates 128 bytes buffer
    // streambuf controls input and ouput to a character sequence.
    // input sequence (get area), output sequence (put area)
    // source for input, sink for output
    // baisc_istram, basic_ostream, ofstream, stringstream are streambuf

    // threee pointers:
    // - the beginnning pointer
    // - the next pointer
    // - the end pointer, points one past the end of the buffer

    // if the next pointer is greater than the beginning pointer in a get area,
    // a putback position is available, and the next pointer can be decremented,
    // dereferenced, and assignec to, in order to put a character back into
    // the get area.

    // get area (input buffer)
    // - eback(), gptr(), egptr()
    // - sputback(), ggetc()
    // - in_avail() : egptr() - gptr()

    // put area (output buffer, write buffer)
    // - pbase(), pptr(), epptr()
    // - sputc()
    //

    SUBCASE("socket recv")
    {
      streambuf buf;

      // boost::asio::streambuf::mutable_buffers_type
      auto mbuf = buf.prepare(1024);
      // then, socket writes to the mbuf when recv from the socket

      // somehow put bytes into buffer. this emulates write and commit
      buf.sputn("payload", 7);
      CHECK(buf.size() == 7);

      buf.consume(7);  // consume forwards the read pointer
      CHECK(buf.size() == 0);
    }

    SUBCASE("socket send")
    {
      streambuf buf;
      buf.sputn("payload", 7);
      // send(buf.data(), buf.size());
      buf.consume(7);
    }
  }

  SUBCASE("working with codec")
  {
    SUBCASE("encode")
    {
      streambuf send_buf;

      boost::asio::const_buffer in_buf("payload", 7);

      // cipher codec first
      // - provide a buf to save the result
      // - then, pass to the length_delimited codec using send_buf
      // - since we know the length, we can prepare

      // encode(const_buffer&, mutable_buffer&)
    }

    SUBCASE("decode")
    {
      streambuf recv_buf;

      boost::asio::const_buffer cbuf = recv_buf.data();
      // vector<const_buffer> decode(cbuf);
      // consume required
      // cipher codec keeps internal buffer and use it to provide const_buffers
      // almost done? maybe.
    }
  }
}