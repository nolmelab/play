#pragma once

#include <play/app/app_base.hpp>
#include <play/ensure/bot.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/protocol/secure_protocol.hpp>

namespace play { namespace ensure {

// ensure app. flow를 시작. 그러면 전체 테스트가 동작
/**
 * act_factory에 <type, creator>로 생성 함수를 등록 
 * start() 호출로 구성 파일에서 bot들 생성하여 flow 내에서 act 실행 시작
 * 
 * NOTE: ensure_app의 서버로서 기본 프로토콜은 json으로 변경될 예정 
 */
class ensure : public app::app_base<net::secure_protocol<uint16_t>, flatbuffers::NativeTable>
{
public:
  using app_base = app::app_base<net::secure_protocol<uint16_t>, flatbuffers::NativeTable>;
  using protocol = net::secure_protocol<uint16_t>;
  using frame = flatbuffers::NativeTable;
  using server = net::server<protocol, frame>;
  using client = net::client<protocol, frame>;
  using session = typename server::session;

  // TODO: json_handler to report in json
  using frame_handler = net::flatbuffer_handler<session>;

public:
  ensure(const std::string& config_file);

  // 서버 시작. 구성 로딩. 봇 생성. on_start() 호출 후 각 봇 시작
  bool start();

  void stop();

  net::runner& get_runner()
  {
    return *runner_.get();
  }

private:
  bool start_bots();

  void stop_bots();

  void register_default_acts();

private:
  std::string config_file_;
  nlohmann::json jconf_;
  size_t bot_count_;
  size_t bot_start_index_;
  std::string bot_prefix_;

  std::unique_ptr<net::thread_runner> runner_;
  std::unique_ptr<server> server_;
  frame_handler handler_;
  std::vector<bot::ptr> bots_;
};

}}  // namespace play::ensure