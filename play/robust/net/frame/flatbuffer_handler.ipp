#include <play/robust/base/logger.hpp>
#include <play/robust/net/frame/flatbuffer_handler.hpp>

namespace play { namespace robust { namespace net {

template <typename Session>
template <typename FlatBufferObj, typename Obj>
inline bool flatbuffer_handler<Session>::send(session_ptr se, topic pic, Obj& obj, bool encrypt)
{
  static thread_local flatbuffers::FlatBufferBuilder fbb(4096);
  fbb.Reset();
  fbb.Finish(FlatBufferObj::Pack(fbb, &obj));

  auto buf = fbb.GetBufferPointer();
  auto size = fbb.GetSize();

  return se->send(pic, buf, size, encrypt);
}

template <typename Session>
inline void flatbuffer_handler<Session>::recv(session_ptr se, topic pic, const void* data,
                                              size_t len)
{
  auto iter = unpackers_.find(pic);
  if (iter != unpackers_.end())
  {
    auto& unpack_fn = iter->second;
    auto frame_ptr = unpack_fn(reinterpret_cast<const uint8_t*>(data), len);
    // dispatch
    {
      std::shared_lock guard(subs_lock_);
      auto siter = receivers_.find(pic);
      if (siter != receivers_.end())
      {
        for (auto& sub : siter->second)
        {
          sub.cb(se, frame_ptr);
        }
      }
      else
      {
        LOG()->debug("sub for topic: {} not found", pic);
      }
    }
  }
  else
  {
    LOG()->debug("unpacker for topic: {} not found", pic);
  }
}

template <typename Session>
inline size_t flatbuffer_handler<Session>::sub(topic pic, receiver fn)
{
  std::unique_lock guard(subs_lock_);
  auto iter = receivers_.find(pic);
  if (iter == receivers_.end())
  {
    std::vector<sub_entry> subs{};
    auto result = receivers_.insert(std::pair(pic, subs));
    PLAY_CHECK(result.second);
    iter = result.first;
  }
  auto id = current_sub_id_++;
  iter->second.push_back({id, fn});
  return id;
}

template <typename Session>
inline void flatbuffer_handler<Session>::reg(topic pic, unpacker fn)
{
  auto result = unpackers_.insert(std::pair(pic, fn));
  if (!result.second)
  {
    LOG()->info("unpacker exists. topic: {}", pic);
  }
}

template <typename Session>
template <typename FbObjType, typename ObjType>
inline typename flatbuffer_handler<Session>::frame_ptr flatbuffer_handler<Session>::unpack(
    const uint8_t* data, size_t len)
{
  auto obj = std::make_shared<ObjType>();
  auto fb_obj = flatbuffers::GetRoot<FbObjType>(data);

  flatbuffers::Verifier verifier(data, len, flatbuffers::Verifier::Options{});
  auto result = fb_obj->Verify(verifier);

  if (!result)
  {
    return {};
  }

  fb_obj->UnPackTo(obj.get());
  return obj;
}

}}}  // namespace play::robust::net