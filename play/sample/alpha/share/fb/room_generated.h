// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ROOM_ALPHA_ROOM_H_
#define FLATBUFFERS_GENERATED_ROOM_ALPHA_ROOM_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

namespace alpha {
namespace room {

struct req_create;
struct req_createBuilder;
struct req_createT;

struct req_createT : public ::flatbuffers::NativeTable {
  typedef req_create TableType;
  std::string name{};
  uint16_t size = 0;
};

struct req_create FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef req_createT NativeTableType;
  typedef req_createBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_SIZE = 6
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  uint16_t size() const {
    return GetField<uint16_t>(VT_SIZE, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyField<uint16_t>(verifier, VT_SIZE, 2) &&
           verifier.EndTable();
  }
  req_createT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(req_createT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<req_create> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_createT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct req_createBuilder {
  typedef req_create Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(req_create::VT_NAME, name);
  }
  void add_size(uint16_t size) {
    fbb_.AddElement<uint16_t>(req_create::VT_SIZE, size, 0);
  }
  explicit req_createBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<req_create> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<req_create>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<req_create> Createreq_create(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    uint16_t size = 0) {
  req_createBuilder builder_(_fbb);
  builder_.add_name(name);
  builder_.add_size(size);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<req_create> Createreq_createDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    uint16_t size = 0) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return alpha::room::Createreq_create(
      _fbb,
      name__,
      size);
}

::flatbuffers::Offset<req_create> Createreq_create(::flatbuffers::FlatBufferBuilder &_fbb, const req_createT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline req_createT *req_create::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<req_createT>(new req_createT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void req_create::UnPackTo(req_createT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); }
  { auto _e = size(); _o->size = _e; }
}

inline ::flatbuffers::Offset<req_create> req_create::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_createT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createreq_create(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<req_create> Createreq_create(::flatbuffers::FlatBufferBuilder &_fbb, const req_createT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const req_createT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  auto _size = _o->size;
  return alpha::room::Createreq_create(
      _fbb,
      _name,
      _size);
}

}  // namespace room
}  // namespace alpha

#endif  // FLATBUFFERS_GENERATED_ROOM_ALPHA_ROOM_H_
