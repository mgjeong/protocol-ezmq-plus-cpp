// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_no_generic_services.proto

#ifndef PROTOBUF_google_2fprotobuf_2funittest_5fno_5fgeneric_5fservices_2eproto__INCLUDED
#define PROTOBUF_google_2fprotobuf_2funittest_5fno_5fgeneric_5fservices_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace google {
namespace protobuf {
namespace no_generic_services_test {
class TestMessage;
class TestMessageDefaultTypeInternal;
extern TestMessageDefaultTypeInternal _TestMessage_default_instance_;
}  // namespace no_generic_services_test
}  // namespace protobuf
}  // namespace google

namespace google {
namespace protobuf {
namespace no_generic_services_test {

namespace protobuf_google_2fprotobuf_2funittest_5fno_5fgeneric_5fservices_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static void InitDefaultsImpl();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_google_2fprotobuf_2funittest_5fno_5fgeneric_5fservices_2eproto

enum TestEnum {
  FOO = 1
};
bool TestEnum_IsValid(int value);
const TestEnum TestEnum_MIN = FOO;
const TestEnum TestEnum_MAX = FOO;
const int TestEnum_ARRAYSIZE = TestEnum_MAX + 1;

const ::google::protobuf::EnumDescriptor* TestEnum_descriptor();
inline const ::std::string& TestEnum_Name(TestEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    TestEnum_descriptor(), value);
}
inline bool TestEnum_Parse(
    const ::std::string& name, TestEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<TestEnum>(
    TestEnum_descriptor(), name, value);
}
// ===================================================================

class TestMessage : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:google.protobuf.no_generic_services_test.TestMessage) */ {
 public:
  TestMessage();
  virtual ~TestMessage();

  TestMessage(const TestMessage& from);

  inline TestMessage& operator=(const TestMessage& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  TestMessage(TestMessage&& from) noexcept
    : TestMessage() {
    *this = ::std::move(from);
  }

  inline TestMessage& operator=(TestMessage&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const TestMessage& default_instance();

  static inline const TestMessage* internal_default_instance() {
    return reinterpret_cast<const TestMessage*>(
               &_TestMessage_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(TestMessage* other);
  friend void swap(TestMessage& a, TestMessage& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline TestMessage* New() const PROTOBUF_FINAL { return New(NULL); }

  TestMessage* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const TestMessage& from);
  void MergeFrom(const TestMessage& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(TestMessage* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 a = 1;
  bool has_a() const;
  void clear_a();
  static const int kAFieldNumber = 1;
  ::google::protobuf::int32 a() const;
  void set_a(::google::protobuf::int32 value);

  GOOGLE_PROTOBUF_EXTENSION_ACCESSORS(TestMessage)
  // @@protoc_insertion_point(class_scope:google.protobuf.no_generic_services_test.TestMessage)
 private:
  void set_has_a();
  void clear_has_a();

  ::google::protobuf::internal::ExtensionSet _extensions_;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::int32 a_;
  friend struct protobuf_google_2fprotobuf_2funittest_5fno_5fgeneric_5fservices_2eproto::TableStruct;
};
// ===================================================================

static const int kTestExtensionFieldNumber = 1000;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::no_generic_services_test::TestMessage,
    ::google::protobuf::internal::PrimitiveTypeTraits< ::google::protobuf::int32 >, 5, false >
  test_extension;

// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// TestMessage

// optional int32 a = 1;
inline bool TestMessage::has_a() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TestMessage::set_has_a() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TestMessage::clear_has_a() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TestMessage::clear_a() {
  a_ = 0;
  clear_has_a();
}
inline ::google::protobuf::int32 TestMessage::a() const {
  // @@protoc_insertion_point(field_get:google.protobuf.no_generic_services_test.TestMessage.a)
  return a_;
}
inline void TestMessage::set_a(::google::protobuf::int32 value) {
  set_has_a();
  a_ = value;
  // @@protoc_insertion_point(field_set:google.protobuf.no_generic_services_test.TestMessage.a)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


}  // namespace no_generic_services_test
}  // namespace protobuf
}  // namespace google

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::google::protobuf::no_generic_services_test::TestEnum> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::google::protobuf::no_generic_services_test::TestEnum>() {
  return ::google::protobuf::no_generic_services_test::TestEnum_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_google_2fprotobuf_2funittest_5fno_5fgeneric_5fservices_2eproto__INCLUDED
