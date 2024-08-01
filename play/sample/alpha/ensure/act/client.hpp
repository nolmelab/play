#pragma once

#include <play/net/client.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/protocol/secure_protocol.hpp>

namespace alpha {

class client : public play::client<play::secure_protocol<uint16_t>, flatbuffers::NativeTable>
{
public:
  using client_base = play::client<play::secure_protocol<uint16_t>, flatbuffers::NativeTable>;
  using protocol = play::secure_protocol<uint16_t>;
  using frame = flatbuffers::NativeTable;
  using session = typename client_base::session;
  using frame_handler = play::flatbuffer_handler<session>;

  client(frame_handler& handler)
  : client_base{handler}
  {}
};

}  // namespace alpha