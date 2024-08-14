// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_AUTH_ALPHA_AUTH_H_
#define FLATBUFFERS_GENERATED_AUTH_ALPHA_AUTH_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

#include "error_code_generated.h"

namespace alpha {
namespace auth {

struct req_login;
struct req_loginBuilder;
struct req_loginT;

struct req_login_f2b;
struct req_login_f2bBuilder;
struct req_login_f2bT;

struct syn_login_b2f;
struct syn_login_b2fBuilder;
struct syn_login_b2fT;

struct res_login;
struct res_loginBuilder;
struct res_loginT;

struct syn_kickout;
struct syn_kickoutBuilder;
struct syn_kickoutT;

struct req_logout;
struct req_logoutBuilder;
struct req_logoutT;

struct req_logout_f2b;
struct req_logout_f2bBuilder;
struct req_logout_f2bT;

struct syn_logout_b2f;
struct syn_logout_b2fBuilder;
struct syn_logout_b2fT;

struct res_logout_b2f;
struct res_logout_b2fBuilder;
struct res_logout_b2fT;

struct req_loginT : public ::flatbuffers::NativeTable {
  typedef req_login TableType;
  std::string name{};
  std::string password{};
};

struct req_login FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef req_loginT NativeTableType;
  typedef req_loginBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_PASSWORD = 6
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *password() const {
    return GetPointer<const ::flatbuffers::String *>(VT_PASSWORD);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_PASSWORD) &&
           verifier.VerifyString(password()) &&
           verifier.EndTable();
  }
  req_loginT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(req_loginT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<req_login> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_loginT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct req_loginBuilder {
  typedef req_login Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(req_login::VT_NAME, name);
  }
  void add_password(::flatbuffers::Offset<::flatbuffers::String> password) {
    fbb_.AddOffset(req_login::VT_PASSWORD, password);
  }
  explicit req_loginBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<req_login> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<req_login>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<req_login> Createreq_login(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> password = 0) {
  req_loginBuilder builder_(_fbb);
  builder_.add_password(password);
  builder_.add_name(name);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<req_login> Createreq_loginDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    const char *password = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto password__ = password ? _fbb.CreateString(password) : 0;
  return alpha::auth::Createreq_login(
      _fbb,
      name__,
      password__);
}

::flatbuffers::Offset<req_login> Createreq_login(::flatbuffers::FlatBufferBuilder &_fbb, const req_loginT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct req_login_f2bT : public ::flatbuffers::NativeTable {
  typedef req_login_f2b TableType;
  std::string name{};
  std::string password{};
};

struct req_login_f2b FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef req_login_f2bT NativeTableType;
  typedef req_login_f2bBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_PASSWORD = 6
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *password() const {
    return GetPointer<const ::flatbuffers::String *>(VT_PASSWORD);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_PASSWORD) &&
           verifier.VerifyString(password()) &&
           verifier.EndTable();
  }
  req_login_f2bT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(req_login_f2bT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<req_login_f2b> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_login_f2bT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct req_login_f2bBuilder {
  typedef req_login_f2b Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(req_login_f2b::VT_NAME, name);
  }
  void add_password(::flatbuffers::Offset<::flatbuffers::String> password) {
    fbb_.AddOffset(req_login_f2b::VT_PASSWORD, password);
  }
  explicit req_login_f2bBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<req_login_f2b> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<req_login_f2b>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<req_login_f2b> Createreq_login_f2b(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> password = 0) {
  req_login_f2bBuilder builder_(_fbb);
  builder_.add_password(password);
  builder_.add_name(name);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<req_login_f2b> Createreq_login_f2bDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    const char *password = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto password__ = password ? _fbb.CreateString(password) : 0;
  return alpha::auth::Createreq_login_f2b(
      _fbb,
      name__,
      password__);
}

::flatbuffers::Offset<req_login_f2b> Createreq_login_f2b(::flatbuffers::FlatBufferBuilder &_fbb, const req_login_f2bT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct syn_login_b2fT : public ::flatbuffers::NativeTable {
  typedef syn_login_b2f TableType;
  alpha::error_code ec = alpha::error_code::success;
  std::string name{};
  std::string reason{};
};

struct syn_login_b2f FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef syn_login_b2fT NativeTableType;
  typedef syn_login_b2fBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_EC = 4,
    VT_NAME = 6,
    VT_REASON = 8
  };
  alpha::error_code ec() const {
    return static_cast<alpha::error_code>(GetField<uint16_t>(VT_EC, 0));
  }
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *reason() const {
    return GetPointer<const ::flatbuffers::String *>(VT_REASON);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_EC, 2) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_REASON) &&
           verifier.VerifyString(reason()) &&
           verifier.EndTable();
  }
  syn_login_b2fT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(syn_login_b2fT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<syn_login_b2f> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const syn_login_b2fT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct syn_login_b2fBuilder {
  typedef syn_login_b2f Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_ec(alpha::error_code ec) {
    fbb_.AddElement<uint16_t>(syn_login_b2f::VT_EC, static_cast<uint16_t>(ec), 0);
  }
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(syn_login_b2f::VT_NAME, name);
  }
  void add_reason(::flatbuffers::Offset<::flatbuffers::String> reason) {
    fbb_.AddOffset(syn_login_b2f::VT_REASON, reason);
  }
  explicit syn_login_b2fBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<syn_login_b2f> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<syn_login_b2f>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<syn_login_b2f> Createsyn_login_b2f(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    alpha::error_code ec = alpha::error_code::success,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> reason = 0) {
  syn_login_b2fBuilder builder_(_fbb);
  builder_.add_reason(reason);
  builder_.add_name(name);
  builder_.add_ec(ec);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<syn_login_b2f> Createsyn_login_b2fDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    alpha::error_code ec = alpha::error_code::success,
    const char *name = nullptr,
    const char *reason = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto reason__ = reason ? _fbb.CreateString(reason) : 0;
  return alpha::auth::Createsyn_login_b2f(
      _fbb,
      ec,
      name__,
      reason__);
}

::flatbuffers::Offset<syn_login_b2f> Createsyn_login_b2f(::flatbuffers::FlatBufferBuilder &_fbb, const syn_login_b2fT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct res_loginT : public ::flatbuffers::NativeTable {
  typedef res_login TableType;
  alpha::error_code ec = alpha::error_code::success;
};

struct res_login FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef res_loginT NativeTableType;
  typedef res_loginBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_EC = 4
  };
  alpha::error_code ec() const {
    return static_cast<alpha::error_code>(GetField<uint16_t>(VT_EC, 0));
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_EC, 2) &&
           verifier.EndTable();
  }
  res_loginT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(res_loginT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<res_login> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const res_loginT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct res_loginBuilder {
  typedef res_login Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_ec(alpha::error_code ec) {
    fbb_.AddElement<uint16_t>(res_login::VT_EC, static_cast<uint16_t>(ec), 0);
  }
  explicit res_loginBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<res_login> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<res_login>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<res_login> Createres_login(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    alpha::error_code ec = alpha::error_code::success) {
  res_loginBuilder builder_(_fbb);
  builder_.add_ec(ec);
  return builder_.Finish();
}

::flatbuffers::Offset<res_login> Createres_login(::flatbuffers::FlatBufferBuilder &_fbb, const res_loginT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct syn_kickoutT : public ::flatbuffers::NativeTable {
  typedef syn_kickout TableType;
  alpha::error_code ec = alpha::error_code::success;
};

struct syn_kickout FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef syn_kickoutT NativeTableType;
  typedef syn_kickoutBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_EC = 4
  };
  alpha::error_code ec() const {
    return static_cast<alpha::error_code>(GetField<uint16_t>(VT_EC, 0));
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_EC, 2) &&
           verifier.EndTable();
  }
  syn_kickoutT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(syn_kickoutT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<syn_kickout> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const syn_kickoutT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct syn_kickoutBuilder {
  typedef syn_kickout Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_ec(alpha::error_code ec) {
    fbb_.AddElement<uint16_t>(syn_kickout::VT_EC, static_cast<uint16_t>(ec), 0);
  }
  explicit syn_kickoutBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<syn_kickout> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<syn_kickout>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<syn_kickout> Createsyn_kickout(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    alpha::error_code ec = alpha::error_code::success) {
  syn_kickoutBuilder builder_(_fbb);
  builder_.add_ec(ec);
  return builder_.Finish();
}

::flatbuffers::Offset<syn_kickout> Createsyn_kickout(::flatbuffers::FlatBufferBuilder &_fbb, const syn_kickoutT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct req_logoutT : public ::flatbuffers::NativeTable {
  typedef req_logout TableType;
};

struct req_logout FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef req_logoutT NativeTableType;
  typedef req_logoutBuilder Builder;
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
  req_logoutT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(req_logoutT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<req_logout> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_logoutT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct req_logoutBuilder {
  typedef req_logout Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  explicit req_logoutBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<req_logout> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<req_logout>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<req_logout> Createreq_logout(
    ::flatbuffers::FlatBufferBuilder &_fbb) {
  req_logoutBuilder builder_(_fbb);
  return builder_.Finish();
}

::flatbuffers::Offset<req_logout> Createreq_logout(::flatbuffers::FlatBufferBuilder &_fbb, const req_logoutT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct req_logout_f2bT : public ::flatbuffers::NativeTable {
  typedef req_logout_f2b TableType;
  std::string name{};
};

struct req_logout_f2b FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef req_logout_f2bT NativeTableType;
  typedef req_logout_f2bBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
  req_logout_f2bT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(req_logout_f2bT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<req_logout_f2b> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_logout_f2bT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct req_logout_f2bBuilder {
  typedef req_logout_f2b Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(req_logout_f2b::VT_NAME, name);
  }
  explicit req_logout_f2bBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<req_logout_f2b> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<req_logout_f2b>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<req_logout_f2b> Createreq_logout_f2b(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0) {
  req_logout_f2bBuilder builder_(_fbb);
  builder_.add_name(name);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<req_logout_f2b> Createreq_logout_f2bDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return alpha::auth::Createreq_logout_f2b(
      _fbb,
      name__);
}

::flatbuffers::Offset<req_logout_f2b> Createreq_logout_f2b(::flatbuffers::FlatBufferBuilder &_fbb, const req_logout_f2bT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct syn_logout_b2fT : public ::flatbuffers::NativeTable {
  typedef syn_logout_b2f TableType;
  std::string name{};
};

struct syn_logout_b2f FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef syn_logout_b2fT NativeTableType;
  typedef syn_logout_b2fBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
  syn_logout_b2fT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(syn_logout_b2fT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<syn_logout_b2f> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const syn_logout_b2fT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct syn_logout_b2fBuilder {
  typedef syn_logout_b2f Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(syn_logout_b2f::VT_NAME, name);
  }
  explicit syn_logout_b2fBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<syn_logout_b2f> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<syn_logout_b2f>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<syn_logout_b2f> Createsyn_logout_b2f(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0) {
  syn_logout_b2fBuilder builder_(_fbb);
  builder_.add_name(name);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<syn_logout_b2f> Createsyn_logout_b2fDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return alpha::auth::Createsyn_logout_b2f(
      _fbb,
      name__);
}

::flatbuffers::Offset<syn_logout_b2f> Createsyn_logout_b2f(::flatbuffers::FlatBufferBuilder &_fbb, const syn_logout_b2fT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct res_logout_b2fT : public ::flatbuffers::NativeTable {
  typedef res_logout_b2f TableType;
};

struct res_logout_b2f FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef res_logout_b2fT NativeTableType;
  typedef res_logout_b2fBuilder Builder;
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
  res_logout_b2fT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(res_logout_b2fT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<res_logout_b2f> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const res_logout_b2fT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct res_logout_b2fBuilder {
  typedef res_logout_b2f Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  explicit res_logout_b2fBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<res_logout_b2f> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<res_logout_b2f>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<res_logout_b2f> Createres_logout_b2f(
    ::flatbuffers::FlatBufferBuilder &_fbb) {
  res_logout_b2fBuilder builder_(_fbb);
  return builder_.Finish();
}

::flatbuffers::Offset<res_logout_b2f> Createres_logout_b2f(::flatbuffers::FlatBufferBuilder &_fbb, const res_logout_b2fT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline req_loginT *req_login::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<req_loginT>(new req_loginT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void req_login::UnPackTo(req_loginT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); }
  { auto _e = password(); if (_e) _o->password = _e->str(); }
}

inline ::flatbuffers::Offset<req_login> req_login::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_loginT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createreq_login(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<req_login> Createreq_login(::flatbuffers::FlatBufferBuilder &_fbb, const req_loginT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const req_loginT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  auto _password = _o->password.empty() ? 0 : _fbb.CreateString(_o->password);
  return alpha::auth::Createreq_login(
      _fbb,
      _name,
      _password);
}

inline req_login_f2bT *req_login_f2b::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<req_login_f2bT>(new req_login_f2bT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void req_login_f2b::UnPackTo(req_login_f2bT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); }
  { auto _e = password(); if (_e) _o->password = _e->str(); }
}

inline ::flatbuffers::Offset<req_login_f2b> req_login_f2b::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_login_f2bT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createreq_login_f2b(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<req_login_f2b> Createreq_login_f2b(::flatbuffers::FlatBufferBuilder &_fbb, const req_login_f2bT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const req_login_f2bT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  auto _password = _o->password.empty() ? 0 : _fbb.CreateString(_o->password);
  return alpha::auth::Createreq_login_f2b(
      _fbb,
      _name,
      _password);
}

inline syn_login_b2fT *syn_login_b2f::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<syn_login_b2fT>(new syn_login_b2fT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void syn_login_b2f::UnPackTo(syn_login_b2fT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = ec(); _o->ec = _e; }
  { auto _e = name(); if (_e) _o->name = _e->str(); }
  { auto _e = reason(); if (_e) _o->reason = _e->str(); }
}

inline ::flatbuffers::Offset<syn_login_b2f> syn_login_b2f::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const syn_login_b2fT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createsyn_login_b2f(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<syn_login_b2f> Createsyn_login_b2f(::flatbuffers::FlatBufferBuilder &_fbb, const syn_login_b2fT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const syn_login_b2fT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _ec = _o->ec;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  auto _reason = _o->reason.empty() ? 0 : _fbb.CreateString(_o->reason);
  return alpha::auth::Createsyn_login_b2f(
      _fbb,
      _ec,
      _name,
      _reason);
}

inline res_loginT *res_login::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<res_loginT>(new res_loginT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void res_login::UnPackTo(res_loginT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = ec(); _o->ec = _e; }
}

inline ::flatbuffers::Offset<res_login> res_login::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const res_loginT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createres_login(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<res_login> Createres_login(::flatbuffers::FlatBufferBuilder &_fbb, const res_loginT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const res_loginT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _ec = _o->ec;
  return alpha::auth::Createres_login(
      _fbb,
      _ec);
}

inline syn_kickoutT *syn_kickout::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<syn_kickoutT>(new syn_kickoutT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void syn_kickout::UnPackTo(syn_kickoutT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = ec(); _o->ec = _e; }
}

inline ::flatbuffers::Offset<syn_kickout> syn_kickout::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const syn_kickoutT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createsyn_kickout(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<syn_kickout> Createsyn_kickout(::flatbuffers::FlatBufferBuilder &_fbb, const syn_kickoutT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const syn_kickoutT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _ec = _o->ec;
  return alpha::auth::Createsyn_kickout(
      _fbb,
      _ec);
}

inline req_logoutT *req_logout::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<req_logoutT>(new req_logoutT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void req_logout::UnPackTo(req_logoutT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
}

inline ::flatbuffers::Offset<req_logout> req_logout::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_logoutT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createreq_logout(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<req_logout> Createreq_logout(::flatbuffers::FlatBufferBuilder &_fbb, const req_logoutT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const req_logoutT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  return alpha::auth::Createreq_logout(
      _fbb);
}

inline req_logout_f2bT *req_logout_f2b::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<req_logout_f2bT>(new req_logout_f2bT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void req_logout_f2b::UnPackTo(req_logout_f2bT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); }
}

inline ::flatbuffers::Offset<req_logout_f2b> req_logout_f2b::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const req_logout_f2bT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createreq_logout_f2b(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<req_logout_f2b> Createreq_logout_f2b(::flatbuffers::FlatBufferBuilder &_fbb, const req_logout_f2bT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const req_logout_f2bT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  return alpha::auth::Createreq_logout_f2b(
      _fbb,
      _name);
}

inline syn_logout_b2fT *syn_logout_b2f::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<syn_logout_b2fT>(new syn_logout_b2fT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void syn_logout_b2f::UnPackTo(syn_logout_b2fT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); }
}

inline ::flatbuffers::Offset<syn_logout_b2f> syn_logout_b2f::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const syn_logout_b2fT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createsyn_logout_b2f(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<syn_logout_b2f> Createsyn_logout_b2f(::flatbuffers::FlatBufferBuilder &_fbb, const syn_logout_b2fT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const syn_logout_b2fT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  return alpha::auth::Createsyn_logout_b2f(
      _fbb,
      _name);
}

inline res_logout_b2fT *res_logout_b2f::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<res_logout_b2fT>(new res_logout_b2fT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void res_logout_b2f::UnPackTo(res_logout_b2fT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
}

inline ::flatbuffers::Offset<res_logout_b2f> res_logout_b2f::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const res_logout_b2fT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return Createres_logout_b2f(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<res_logout_b2f> Createres_logout_b2f(::flatbuffers::FlatBufferBuilder &_fbb, const res_logout_b2fT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const res_logout_b2fT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  return alpha::auth::Createres_logout_b2f(
      _fbb);
}

}  // namespace auth
}  // namespace alpha

#endif  // FLATBUFFERS_GENERATED_AUTH_ALPHA_AUTH_H_
