#include <play/net/pulse_fb.hpp>

namespace play {

template <typename Protocol>
template <typename TopicInput, typename FlatBufferObj, typename Obj>
inline bool pulse_fb<Protocol>::send(session_ptr se, TopicInput topic_in, Obj& obj, bool encrypt)
{
  auto pic = static_cast<topic>(topic_in);

  static thread_local flatbuffers::FlatBufferBuilder fbb(64 * 1024);
  fbb.Reset();
  fbb.Finish(FlatBufferObj::Pack(fbb, &obj));

  auto buf = fbb.GetBufferPointer();
  auto size = fbb.GetSize();

  return se->send(pic, buf, size, encrypt);
}

template <typename Protocol>
inline void pulse_fb<Protocol>::add_unpacker(topic pic, unpacker fn)
{
  auto result = unpackers_.insert(std::pair(pic, fn));
  if (!result.second)
  {
    LOG()->info("unpacker exists. topic: {}", pic);
  }
}

template <typename Protocol>
template <typename FbObjType, typename ObjType>
inline typename pulse_fb<Protocol>::frame_ptr pulse_fb<Protocol>::unpack_fn(const uint8_t* data,
                                                                            size_t len)
{
  auto obj = std::make_shared<ObjType>();
  auto fb_obj = flatbuffers::GetRoot<FbObjType>(data);

  flatbuffers::Verifier verifier(data, len, flatbuffers::Verifier::Options{});
  auto result = fb_obj->Verify(verifier);

  if (!result)
  {
    LOG()->warn("flatbuffer verification error");
    return {};
  }

  fb_obj->UnPackTo(obj.get());
  return obj;
}

template <typename Protocol>
typename pulse_fb<Protocol>::frame_ptr pulse_fb<Protocol>::unpack(topic pic, const uint8_t* data,
                                                                  size_t len)
{
  auto iter = unpackers_.find(pic);
  if (iter != unpackers_.end())
  {
    auto& unpack_fn = iter->second;
    return unpack_fn(reinterpret_cast<const uint8_t*>(data), len);
  }

  return {};
}

}  // namespace play