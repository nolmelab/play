#pragma once

#include <play/app/app_base.hpp>
#include <play/ensure/bot.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/protocol/secure_protocol.hpp>

namespace ensure {

// ensure app. flow를 시작. 그러면 전체 테스트가 동작
/**
 * act_factory에 <type, creator>로 생성 함수를 등록 
 * start() 호출로 구성 파일에서 bot들 생성하여 flow 내에서 act 실행 시작
 */
class ensure_app : public play::app_base<play::secure_protocol<uint16_t>, flatbuffers::NativeTable>
{
public:
  using app_base = play::app_base<play::secure_protocol<uint16_t>, flatbuffers::NativeTable>;
  using protocol = play::secure_protocol<uint16_t>;
  using frame = flatbuffers::NativeTable;
  using server = play::server<protocol, frame>;
  using client = play::client<protocol, frame>;
  using session = typename server::session;
  using session_ptr = std::shared_ptr<session>;
  using frame_ptr = std::shared_ptr<frame>;
  using frame_handler = play::flatbuffer_handler<session>;

public:
  ensure_app() = default;

  // 서버 시작. 구성 로딩. 봇 생성. on_start() 호출 후 각 봇 시작
  bool start(const std::string& config_file);

  // json 구성에서 서버 시작
  bool start(const nlohmann::json& jconf);

  void wait();

  // signal or service calls stop
  void stop();

  play::runner& get_runner()
  {
    return *runner_;
  }

  frame_handler& get_handler()
  {
    return *handler_;
  }

  size_t get_bot_count() const
  {
    return bots_.size();
  }

  bot::ptr get_bot(size_t index)
  {
    PLAY_CHECK(index >= 0 && index < bots_.size());
    return bots_[index];
  }

private:
  bool start_bots();

  void stop_bots();

  void register_default_acts();

private:
  std::string config_file_;
  nlohmann::json jconf_;
  std::unique_ptr<play::thread_runner> runner_;
  std::unique_ptr<server> server_;
  std::unique_ptr<frame_handler> handler_;
  bool stop_{false};

  size_t bot_count_;
  size_t bot_start_index_;
  std::string bot_prefix_;
  std::vector<bot::ptr> bots_;
};

}  // namespace ensure