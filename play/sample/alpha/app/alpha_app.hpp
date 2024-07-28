#include <play/app/app_base.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/protocol/secure_protocol.hpp>

namespace alpha {

class alpha_app : public play::app_base<play::secure_protocol<uint16_t>, flatbuffers::NativeTable>
{
public:
  using app_base = play::app_base<play::secure_protocol<uint16_t>, flatbuffers::NativeTable>;
  using protocol = play::secure_protocol<uint16_t>;
  using frame = flatbuffers::NativeTable;
  using server = play::server<protocol, frame>;
  using client = play::client<protocol, frame>;
  using session = typename server::session;
  using frame_handler = play::flatbuffer_handler<session>;

public:
  alpha_app() = default;

  // 서버 시작. 구성 로딩.
  bool start(const std::string& config_file);

  // json 구성에서 서버 시작
  bool start(const nlohmann::json& jconf);

  void wait();

  // signal로 stop
  void stop();

  play::runner& get_runner()
  {
    return *runner_;
  }

  frame_handler& get_handler()
  {
    return *handler_;
  }

private:
  bool start_services();

  void stop_services();

private:
  std::string config_file_;
  nlohmann::json jconf_;
  std::unique_ptr<play::thread_runner> runner_;
  std::unique_ptr<server> server_;
  std::unique_ptr<frame_handler> handler_;
  std::string role_;
  bool stop_{false};
};

}  // namespace alpha