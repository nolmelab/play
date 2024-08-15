#pragma once

#include <memory>
#include <play/net/pulse_fb.hpp>

namespace play {

template <typename Pulse>
class session_container
{
public:
  using session_ptr = typename Pulse::session_ptr;

public:
  void bind(Pulse* pulse)
  {
    PLAY_CHECK(pulse != nullptr);
    pulse_ = pulse;
  }

  bool add(session_ptr se)
  {
    auto key = reinterpret_cast<uintptr_t>(se.get());
    return sessions_.insert(std::pair{key, se}).second;
  }

  void del(session_ptr se)
  {
    auto key = reinterpret_cast<uintptr_t>(se.get());
    sessions_.erase(key);
  }

  template <typename FlatBufferObj, typename TopicInput, typename Obj>
  bool send(TopicInput topic_in, Obj& obj, bool encrypt = false)
  {
    bool result = true;
    for (auto& kv : sessions_)
    {
      auto session = kv.second;
      // read!
      // https://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-keywords
      result = result && pulse_->template send<FlatBufferObj>(session, topic_in, obj, encrypt);
    }
    return result;
  }

private:
  Pulse* pulse_;
  std::map<uintptr_t, session_ptr> sessions_;
};

}  // namespace play
