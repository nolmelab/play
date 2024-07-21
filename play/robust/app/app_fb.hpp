#pragma once

#include <play/robust/app/app.hpp>
#include <play/robust/net/frame/flatbuffer_handler.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>

namespace play { namespace robust { namespace app {

// secure_protocol로 16비트 토픽을 갖는 플랫버퍼 앱
/**
 * server를 실행한다.  
 */
class app_fb : public app<net::secure_protocol<uint16_t>, flatbuffers::NativeTable>
{
public:
  using protocol = net::secure_protocol<uint16_t>;
  using frame = flatbuffers::NativeTable;
  using server = net::server<protocol, frame>;
  using client = net::client<protocol, frame>;
  using session = typename server::session;
  using frame_handler = net::flatbuffer_handler<session>;

public:
  static app_fb& get(); 

public:
  app_fb(const std::string& jconf)
      : app(),
        server_{runner_, jconf, handler_}
  {
  }

  bool start() override;

  void stop() override;

  frame_handler& get_handler()
  {
    return handler_;
  }

  net::thread_runner get_runner()
  {
    return runner_;
  }

protected:
  virtual bool on_start() {}

  virtual void on_stop() {}

private:
  server_type server_;
  frame_handler handler_;
  net::thread_runner runner_;
};

}}}  // namespace play::robust::app