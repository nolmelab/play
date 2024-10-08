// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TOPIC_ALPHA_H_
#define FLATBUFFERS_GENERATED_TOPIC_ALPHA_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

namespace alpha {

enum class topic : uint16_t {
  auth_req_login = 201,
  auth_req_login_f2b = 202,
  auth_syn_login_b2f = 203,
  auth_res_login = 204,
  auth_req_logout = 205,
  auth_req_logout_f2b = 206,
  auth_syn_logout_b2f = 207,
  auth_res_logout_b2f = 208,
  auth_res_logout = 209,
  room_syn_runner_up = 301,
  room_req_create = 302,
  room_req_create_f2b = 303,
  room_req_create_b2f = 304,
  room_res_create_f2b = 305,
  room_res_create_b2f = 306,
  room_res_create = 307,
  room_req_page = 308,
  room_req_page_f2b = 309,
  room_res_page_b2f = 310,
  room_res_page = 311,
  room_req_reserve = 312,
  room_req_reserve_f2b = 313,
  room_req_reserve_b2f = 314,
  room_res_reserve_f2b = 315,
  room_res_reserve_b2f = 316,
  room_res_reserve = 317,
  room_req_checkin = 318,
  room_res_checkin = 319,
  room_req_chat = 320,
  room_res_chat = 321,
  room_syn_chat = 322,
  room_req_leave = 323,
  room_syn_leave_f2b = 324,
  room_res_leave = 325,
  MIN = auth_req_login,
  MAX = room_res_leave
};

inline const topic (&EnumValuestopic())[34] {
  static const topic values[] = {
    topic::auth_req_login,
    topic::auth_req_login_f2b,
    topic::auth_syn_login_b2f,
    topic::auth_res_login,
    topic::auth_req_logout,
    topic::auth_req_logout_f2b,
    topic::auth_syn_logout_b2f,
    topic::auth_res_logout_b2f,
    topic::auth_res_logout,
    topic::room_syn_runner_up,
    topic::room_req_create,
    topic::room_req_create_f2b,
    topic::room_req_create_b2f,
    topic::room_res_create_f2b,
    topic::room_res_create_b2f,
    topic::room_res_create,
    topic::room_req_page,
    topic::room_req_page_f2b,
    topic::room_res_page_b2f,
    topic::room_res_page,
    topic::room_req_reserve,
    topic::room_req_reserve_f2b,
    topic::room_req_reserve_b2f,
    topic::room_res_reserve_f2b,
    topic::room_res_reserve_b2f,
    topic::room_res_reserve,
    topic::room_req_checkin,
    topic::room_res_checkin,
    topic::room_req_chat,
    topic::room_res_chat,
    topic::room_syn_chat,
    topic::room_req_leave,
    topic::room_syn_leave_f2b,
    topic::room_res_leave
  };
  return values;
}

inline const char * const *EnumNamestopic() {
  static const char * const names[126] = {
    "auth_req_login",
    "auth_req_login_f2b",
    "auth_syn_login_b2f",
    "auth_res_login",
    "auth_req_logout",
    "auth_req_logout_f2b",
    "auth_syn_logout_b2f",
    "auth_res_logout_b2f",
    "auth_res_logout",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "room_syn_runner_up",
    "room_req_create",
    "room_req_create_f2b",
    "room_req_create_b2f",
    "room_res_create_f2b",
    "room_res_create_b2f",
    "room_res_create",
    "room_req_page",
    "room_req_page_f2b",
    "room_res_page_b2f",
    "room_res_page",
    "room_req_reserve",
    "room_req_reserve_f2b",
    "room_req_reserve_b2f",
    "room_res_reserve_f2b",
    "room_res_reserve_b2f",
    "room_res_reserve",
    "room_req_checkin",
    "room_res_checkin",
    "room_req_chat",
    "room_res_chat",
    "room_syn_chat",
    "room_req_leave",
    "room_syn_leave_f2b",
    "room_res_leave",
    nullptr
  };
  return names;
}

inline const char *EnumNametopic(topic e) {
  if (::flatbuffers::IsOutRange(e, topic::auth_req_login, topic::room_res_leave)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(topic::auth_req_login);
  return EnumNamestopic()[index];
}

}  // namespace alpha

#endif  // FLATBUFFERS_GENERATED_TOPIC_ALPHA_H_
