#include <doctest/doctest.h>
#include <play/test/test_fb_generated.h>
#include <play/base/stop_watch.hpp>
#include <play/net/client.hpp>
#include <play/net/protocol/secure_protocol.hpp>
#include <play/net/pulse_fb.hpp>
#include <play/net/runner/poll_runner.hpp>
#include <play/net/server.hpp>

using namespace play;

namespace {

// 1, 2는 시스템에서 사용
const uint16_t topic_message = 11;

struct test_config
{
  inline static int test_frame_count = 1024;
  inline static int payload_loops = 100;
  inline static std::string payload_data = "hello";
  inline static bool client_encrypt = true;
  inline static bool server_encrypt = false;
  inline static int echo_count = 5;
};

}  // namespace

// test_flatbuffers와 동일한 테스트를 pulse로 진행한다.
/**
 * pulse_fb 내에 server와 client를 실행한다. 
 * pulse에 구독한 핸들러 람다에서 에코를 처리한다. 
 * topic_established에서 전송을 시작한다. 
 */
TEST_CASE("pulse")
{
  using pulse_t = pulse_fb<secure_protocol<uint16_t>>;

  size_t recv_count = 0;
  play::stop_watch watch;

  poll_runner runner{"pulse runner"};
  pulse_t pulse_server;
  pulse_t pulse_client;

  pulse_t::add_unpacker(topic_message, &pulse_t::unpack_fn<fb::req_move, fb::req_moveT>);

  pulse_server.as_server(7000).with_runner(&runner).subscribe(
      topic_message,
      [](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
      {
        auto req_move = std::static_pointer_cast<fb::req_moveT>(fr);
        auto v = req_move->pos->x();
        pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), false);
      });

  pulse_client.as_client("127.0.0.1:7000")
      .with_runner(&runner)
      .subscribe(pulse_t::topic_estalished,
                 [](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
                 {
                   fb::req_moveT move;
                   move.pos = std::make_unique<fb::vec3>(1, 1, 1);
                   move.name = "hello";

                   pulse_t::send<fb::req_move>(se, topic_message, move, false);
                 })
      .subscribe(topic_message,
                 [&recv_count](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
                 {
                   recv_count++;
                   auto req_move = std::static_pointer_cast<fb::req_moveT>(fr);
                   auto v = req_move->pos->x();
                   pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), true);
                 });

  CHECK(pulse_server.start());
  CHECK(pulse_client.start());

  runner.poll_one();

  bool end = false;

  while (recv_count < test_config::test_frame_count)
  {
    if (runner.poll_one() == 0)
    {
      runner.sleep(1);
    }
  }

  pulse_server.stop();
  pulse_client.stop();

  auto elapsed = watch.stop();

  LOG()->info("pulse elapsed: {}, frames; {}", elapsed, test_config::test_frame_count);
}
