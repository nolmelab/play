#include <play/app/app_base.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/protocol/secure_protocol.hpp>

class alpha
    : public play::app::app_base<play::net::secure_protocol<uint16_t>, flatbuffers::NativeTable>
{
public:
  using app_base =
      play::app::app_base<play::net::secure_protocol<uint16_t>, flatbuffers::NativeTable>;
  using protocol = play::net::secure_protocol<uint16_t>;
  using frame = flatbuffers::NativeTable;
  using server = play::net::server<protocol, frame>;
  using client = play::net::client<protocol, frame>;
  using session = typename server::session;

  // TODO: json_handler to report in json
  using frame_handler = play::net::flatbuffer_handler<session>;
};

}  // namespace app