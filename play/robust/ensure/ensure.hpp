#pragma once

#include <play/robust/app/app_base.hpp>
#include <play/robust/ensure/bot.hpp>

namespace play { namespace robust { namespace ensure {

// ensure app. flow를 시작. 그러면 전체 테스트가 동작
/**
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

private:
  bool start_bots();

  void stop_bots();

private:
  std::string config_file_;
  nlohmann::json jconf_;
  size_t bot_count_;
  size_t bot_start_index_;
  std::string bot_prefix_;
  std::vector<bot::ptr> bots_;
};

}}}  // namespace play::robust::ensure