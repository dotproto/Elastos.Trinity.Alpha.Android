#ifndef NAN_H_
#define NAN_H_

#include <algorithm>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <utility>

#include <queue>
#include <string>
#include <vector>

#include <v8.h>
#include <android/log.h>
#define NAN_LOG(FORMAT, ...) \
    __android_log_print(ANDROID_LOG_INFO, "chromium", \
                        "%s:%d " FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__)

namespace Nan {

#define NAN_INLINE inline  // TODO(bnoordhuis) Remove in v3.0.0.

#if defined(__GNUC__) && \
    !(defined(V8_DISABLE_DEPRECATIONS) && V8_DISABLE_DEPRECATIONS)
#define NAN_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && \
    !(defined(V8_DISABLE_DEPRECATIONS) && V8_DISABLE_DEPRECATIONS)
#define NAN_DEPRECATED __declspec(deprecated)
#else
#define NAN_DEPRECATED
#endif

#if NAN_HAS_CPLUSPLUS_11
#define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&) = delete;
#define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&) = delete;
#define NAN_DISALLOW_MOVE(CLASS)                                              \
    CLASS(CLASS&&) = delete;  /* NOLINT(build/c++11) */                        \
    void operator=(CLASS&&) = delete;
#else
#define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&);
#define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&);
#define NAN_DISALLOW_MOVE(CLASS)
#endif

#define NAN_DISALLOW_ASSIGN_COPY(CLASS)                                        \
    NAN_DISALLOW_ASSIGN(CLASS)                                                 \
    NAN_DISALLOW_COPY(CLASS)

#define NAN_DISALLOW_ASSIGN_MOVE(CLASS)                                        \
    NAN_DISALLOW_ASSIGN(CLASS)                                                 \
    NAN_DISALLOW_MOVE(CLASS)

#define NAN_DISALLOW_COPY_MOVE(CLASS)                                          \
    NAN_DISALLOW_COPY(CLASS)                                                   \
    NAN_DISALLOW_MOVE(CLASS)

#define NAN_DISALLOW_ASSIGN_COPY_MOVE(CLASS)                                   \
    NAN_DISALLOW_ASSIGN(CLASS)                                                 \
    NAN_DISALLOW_COPY(CLASS)                                                   \
    NAN_DISALLOW_MOVE(CLASS)

#define TYPE_CHECK(T, S)                                                       \
    while (false) {                                                            \
      *(static_cast<T *volatile *>(0)) = static_cast<S*>(0);                   \
    }

//=== RegistrationFunction =====================================================

typedef v8::Local<v8::Object> ADDON_REGISTER_FUNCTION_ARGS_TYPE;

#define NAN_MODULE_INIT(name)                                                  \
    void name(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target)

//=== CallbackInfo =============================================================

#include "nan_callbacks.h"  // NOLINT(build/include)

//==============================================================================
typedef v8::UnboundScript      UnboundScript;
typedef v8::Script             BoundScript;

typedef v8::String::ExternalOneByteStringResource
    ExternalOneByteStringResource;

template<typename T>
class NonCopyablePersistentTraits :
    public v8::NonCopyablePersistentTraits<T> {};
template<typename T>
class CopyablePersistentTraits :
    public v8::CopyablePersistentTraits<T> {};

template<typename T>
class PersistentBase :
    public v8::PersistentBase<T> {};

template<typename T, typename M = v8::NonCopyablePersistentTraits<T> >
class Persistent;

#include "nan_maybe_43_inl.h"  // NOLINT(build/include)
#include "nan_converters.h"  // NOLINT(build/include)
#include "nan_new.h"  // NOLINT(build/include)

template<typename T>
v8::Local<T> New(v8::Handle<T>);


typedef v8::WeakCallbackType WeakCallbackType;

template<typename P> class WeakCallbackInfo;


#include "nan_persistent_12_inl.h"  // NOLINT(build/include)

namespace imp {
  static const size_t kMaxLength = 0x3fffffff;
  // v8::String::REPLACE_INVALID_UTF8 was introduced
  // in node.js v0.10.29 and v0.8.27.
  static const unsigned kReplaceInvalidUtf8 = v8::String::REPLACE_INVALID_UTF8;

}  // end of namespace imp

//=== HandleScope ==============================================================

class HandleScope {
  v8::HandleScope scope;

 public:
  inline HandleScope() : scope(v8::Isolate::GetCurrent()) {}
  inline static int NumberOfHandles() {
    return v8::HandleScope::NumberOfHandles(v8::Isolate::GetCurrent());
  }

 private:
  // Make it hard to create heap-allocated or illegal handle scopes by
  // disallowing certain operations.
  HandleScope(const HandleScope &);
  void operator=(const HandleScope &);
  void *operator new(size_t size);
  void operator delete(void *, size_t) {
    abort();
  }
};

class EscapableHandleScope {
 public:
  inline EscapableHandleScope() : scope(v8::Isolate::GetCurrent()) {}

  inline static int NumberOfHandles() {
    return v8::EscapableHandleScope::NumberOfHandles(v8::Isolate::GetCurrent());
  }

  template<typename T>
  inline v8::Local<T> Escape(v8::Local<T> value) {
    return scope.Escape(value);
  }

 private:
  v8::EscapableHandleScope scope;


 private:
  // Make it hard to create heap-allocated or illegal handle scopes by
  // disallowing certain operations.
  EscapableHandleScope(const EscapableHandleScope &);
  void operator=(const EscapableHandleScope &);
  void *operator new(size_t size);
  void operator delete(void *, size_t) {
    abort();
  }
};

//=== TryCatch =================================================================

class TryCatch {
  v8::TryCatch try_catch_;
  friend void FatalException(const TryCatch&);

 public:
  inline bool HasCaught() const { return try_catch_.HasCaught(); }

  inline bool CanContinue() const { return try_catch_.CanContinue(); }

  inline v8::Local<v8::Value> ReThrow() {
    return New(try_catch_.ReThrow());
  }

  inline v8::Local<v8::Value> Exception() const {
    return try_catch_.Exception();
  }


  inline v8::MaybeLocal<v8::Value> StackTrace() const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(try_catch_.StackTrace(isolate->GetCurrentContext())
                            .FromMaybe(v8::Local<v8::Value>()));
  }

  inline v8::Local<v8::Message> Message() const {
    return try_catch_.Message();
  }

  inline void Reset() { try_catch_.Reset(); }

  inline void SetVerbose(bool value) { try_catch_.SetVerbose(value); }

  inline void SetCaptureMessage(bool value) {
    try_catch_.SetCaptureMessage(value);
  }
};

v8::Local<v8::Value> MakeCallback(v8::Local<v8::Object> target,
                                  v8::Local<v8::Function> func,
                                  int argc,
                                  v8::Local<v8::Value>* argv);
v8::Local<v8::Value> MakeCallback(v8::Local<v8::Object> target,
                                  v8::Local<v8::String> symbol,
                                  int argc,
                                  v8::Local<v8::Value>* argv);
v8::Local<v8::Value> MakeCallback(v8::Local<v8::Object> target,
                                  const char* method,
                                  int argc,
                                  v8::Local<v8::Value>* argv);

// === AsyncResource ===========================================================

class AsyncResource {
 public:
  AsyncResource(
      v8::Local<v8::String> name
    , v8::Local<v8::Object> resource = New<v8::Object>()) {
  }

  AsyncResource(
      const char* name
    , v8::Local<v8::Object> resource = New<v8::Object>()) {
  }

  ~AsyncResource() {
  }

  inline MaybeLocal<v8::Value> runInAsyncScope(
      v8::Local<v8::Object> target
    , v8::Local<v8::Function> func
    , int argc
    , v8::Local<v8::Value>* argv) {
    return MakeCallback(target, func, argc, argv);
  }

  inline MaybeLocal<v8::Value> runInAsyncScope(
      v8::Local<v8::Object> target
    , v8::Local<v8::String> symbol
    , int argc
    , v8::Local<v8::Value>* argv) {
    return MakeCallback(target, symbol, argc, argv);
  }

  inline MaybeLocal<v8::Value> runInAsyncScope(
      v8::Local<v8::Object> target
    , const char* method
    , int argc
    , v8::Local<v8::Value>* argv) {
    return MakeCallback(target, method, argc, argv);
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(AsyncResource)
};

//============ =================================================================

/* node 0.12  */
  inline
  void SetCounterFunction(v8::CounterLookupCallback cb) {
    v8::Isolate::GetCurrent()->SetCounterFunction(cb);
  }

  inline
  void SetCreateHistogramFunction(v8::CreateHistogramCallback cb) {
    v8::Isolate::GetCurrent()->SetCreateHistogramFunction(cb);
  }

  inline
  void SetAddHistogramSampleFunction(v8::AddHistogramSampleCallback cb) {
    v8::Isolate::GetCurrent()->SetAddHistogramSampleFunction(cb);
  }

  inline bool IdleNotification(int idle_time_in_ms) {
    return v8::Isolate::GetCurrent()->IdleNotificationDeadline(
        idle_time_in_ms * 0.001);
  }

  inline void LowMemoryNotification() {
    v8::Isolate::GetCurrent()->LowMemoryNotification();
  }

  inline void ContextDisposedNotification() {
    v8::Isolate::GetCurrent()->ContextDisposedNotification();
  }

  inline v8::Local<v8::Primitive> Undefined() {
    EscapableHandleScope scope;
    return scope.Escape(New(v8::Undefined(v8::Isolate::GetCurrent())));
  }

  inline v8::Local<v8::Primitive> Null() {
    EscapableHandleScope scope;
    return scope.Escape(New(v8::Null(v8::Isolate::GetCurrent())));
  }

  inline v8::Local<v8::Boolean> True() {
    EscapableHandleScope scope;
    return scope.Escape(New(v8::True(v8::Isolate::GetCurrent())));
  }

  inline v8::Local<v8::Boolean> False() {
    EscapableHandleScope scope;
    return scope.Escape(New(v8::False(v8::Isolate::GetCurrent())));
  }

  inline v8::Local<v8::String> EmptyString() {
    return v8::String::Empty(v8::Isolate::GetCurrent());
  }

  inline int AdjustExternalMemory(int bc) {
    return static_cast<int>(
        v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(bc));
  }

  inline void SetTemplate(
      v8::Local<v8::Template> templ
    , const char *name
    , v8::Local<v8::Data> value) {
    templ->Set(v8::Isolate::GetCurrent(), name, value);
  }

  inline void SetTemplate(
      v8::Local<v8::Template> templ
    , v8::Local<v8::String> name
    , v8::Local<v8::Data> value
    , v8::PropertyAttribute attributes) {
    templ->Set(name, value, attributes);
  }

  inline v8::Local<v8::Context> GetCurrentContext() {
    return v8::Isolate::GetCurrent()->GetCurrentContext();
  }

  inline void* GetInternalFieldPointer(
      v8::Local<v8::Object> object
    , int index) {
    return object->GetAlignedPointerFromInternalField(index);
  }

  inline void SetInternalFieldPointer(
      v8::Local<v8::Object> object
    , int index
    , void* value) {
    object->SetAlignedPointerInInternalField(index, value);
  }

#define NAN_GC_CALLBACK(name)                                                 \
    void name(v8::Isolate *isolate, v8::GCType type, v8::GCCallbackFlags flags)

  typedef v8::Isolate::GCCallback GCEpilogueCallback;
  typedef v8::Isolate::GCCallback GCPrologueCallback;

  inline void AddGCEpilogueCallback(
      GCEpilogueCallback callback
    , v8::GCType gc_type_filter = v8::kGCTypeAll) {
    v8::Isolate::GetCurrent()->AddGCEpilogueCallback(callback, gc_type_filter);
  }

  inline void RemoveGCEpilogueCallback(
      GCEpilogueCallback callback) {
    v8::Isolate::GetCurrent()->RemoveGCEpilogueCallback(callback);
  }

  inline void AddGCPrologueCallback(
      GCPrologueCallback callback
    , v8::GCType gc_type_filter = v8::kGCTypeAll) {
    v8::Isolate::GetCurrent()->AddGCPrologueCallback(callback, gc_type_filter);
  }

  inline void RemoveGCPrologueCallback(
      GCPrologueCallback callback) {
    v8::Isolate::GetCurrent()->RemoveGCPrologueCallback(callback);
  }

  inline void GetHeapStatistics(
      v8::HeapStatistics *heap_statistics) {
    v8::Isolate::GetCurrent()->GetHeapStatistics(heap_statistics);
  }

#define X(NAME)                                                               \
    inline v8::Local<v8::Value> NAME(const char *msg) {                        \
      EscapableHandleScope scope;                                              \
      return scope.Escape(v8::Exception::NAME(New(msg).ToLocalChecked()));     \
    }                                                                          \
                                                                               \
    inline                                                                     \
    v8::Local<v8::Value> NAME(v8::Local<v8::String> msg) {                     \
      return v8::Exception::NAME(msg);                                         \
    }                                                                          \
                                                                               \
    inline void Throw ##NAME(const char *msg) {                               \
      HandleScope scope;                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(New(msg).ToLocalChecked()));                     \
    }                                                                          \
                                                                               \
    inline void Throw ##NAME(v8::Local<v8::String> msg) {                     \
      HandleScope scope;                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(msg));                                           \
    }

  X(Error)
  X(RangeError)
  X(ReferenceError)
  X(SyntaxError)
  X(TypeError)

#undef X

  inline void ThrowError(v8::Local<v8::Value> error) {
    v8::Isolate::GetCurrent()->ThrowException(error);
  }

#if 0//?jw
  inline MaybeLocal<v8::Object> NewBuffer(
      char *data
    , size_t length
    , node::smalloc::FreeCallback callback
    , void *hint
  ) {
    // arbitrary buffer lengths requires
    // 14 >= 45
    assert(length <= imp::kMaxLength && "too large buffer");
    return node::Buffer::New(v8::Isolate::GetCurrent(), data, length, callback,
                             hint);
  }

  inline MaybeLocal<v8::Object> CopyBuffer(
      const char *data
    , uint32_t size
  ) {
    // arbitrary buffer lengths requires
    // 14 >= 45
    assert(size <= imp::kMaxLength && "too large buffer");
    return node::Buffer::New(v8::Isolate::GetCurrent(), data, size);
  }

  inline MaybeLocal<v8::Object> NewBuffer(uint32_t size) {
    // arbitrary buffer lengths requires
    // 14 >= 45
    assert(size <= imp::kMaxLength && "too large buffer");
    return node::Buffer::New(v8::Isolate::GetCurrent(), size);
  }

  inline MaybeLocal<v8::Object> NewBuffer(
      char* data
    , uint32_t size
  ) {
    // arbitrary buffer lengths requires
    // 14 >= 45
    assert(size <= imp::kMaxLength && "too large buffer");
    return node::Buffer::Use(v8::Isolate::GetCurrent(), data, size);
  }
#endif //?jw
  inline MaybeLocal<v8::String>
  NewOneByteString(const uint8_t * value, int length = -1) {
    return v8::String::NewFromOneByte(v8::Isolate::GetCurrent(), value,
          v8::NewStringType::kNormal, length);
  }

  inline MaybeLocal<BoundScript> CompileScript(
      v8::Local<v8::String> s
    , const v8::ScriptOrigin& origin
  ) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    v8::ScriptCompiler::Source source(s, origin);
    return scope.Escape(
        v8::ScriptCompiler::Compile(isolate->GetCurrentContext(), &source)
            .FromMaybe(v8::Local<BoundScript>()));
  }

  inline MaybeLocal<BoundScript> CompileScript(
      v8::Local<v8::String> s
  ) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    v8::ScriptCompiler::Source source(s);
    return scope.Escape(
        v8::ScriptCompiler::Compile(isolate->GetCurrentContext(), &source)
            .FromMaybe(v8::Local<BoundScript>()));
  }

  inline MaybeLocal<v8::Value> RunScript(
      v8::Local<UnboundScript> script
  ) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(script->BindToCurrentContext()
                            ->Run(isolate->GetCurrentContext())
                            .FromMaybe(v8::Local<v8::Value>()));
  }

  inline MaybeLocal<v8::Value> RunScript(
      v8::Local<BoundScript> script
  ) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(script->Run(isolate->GetCurrentContext())
                            .FromMaybe(v8::Local<v8::Value>()));
  }

///////////////////////////////////////////////////////////////////////
#if 0
  NAN_DEPRECATED inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::Function> func
    , int argc
    , v8::Local<v8::Value>* argv) {
    EscapableHandleScope scope;
    return scope.Escape(New(node::MakeCallback(
        v8::Isolate::GetCurrent(), target, func, argc, argv)));
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::String> symbol
    , int argc
    , v8::Local<v8::Value>* argv) {
    EscapableHandleScope scope;
    return scope.Escape(New(node::MakeCallback(
        v8::Isolate::GetCurrent(), target, symbol, argc, argv)));
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , const char* method
    , int argc
    , v8::Local<v8::Value>* argv) {
    EscapableHandleScope scope;
    return scope.Escape(New(node::MakeCallback(
        v8::Isolate::GetCurrent(), target, method, argc, argv)));
  }

  inline void FatalException(const TryCatch& try_catch) {
    node::FatalException(v8::Isolate::GetCurrent(), try_catch.try_catch_);
  }

  inline v8::Local<v8::Value> ErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
    return node::ErrnoException(v8::Isolate::GetCurrent(), errorno, syscall,
            message, path);
  }
#endif
////////////////////////////////////////////////////////////////////////////
  NAN_DEPRECATED inline v8::Local<v8::Value> NanErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
#if 0//?jw
    return ErrnoException(errorno, syscall, message, path);
#else
	v8::Local<v8::Value> temp;
    return temp;
#endif
  }

  template<typename T>
  inline void SetIsolateData(
      v8::Isolate *isolate
    , T *data
  ) {
      isolate->SetData(0, data);
  }

  template<typename T>
  inline T *GetIsolateData(
      v8::Isolate *isolate
  ) {
      return static_cast<T*>(isolate->GetData(0));
  }

class Utf8String {
 public:
  inline explicit Utf8String(v8::Local<v8::Value> from) :
      length_(0), str_(str_st_) {
    HandleScope scope;
    if (!from.IsEmpty()) {
      v8::Local<v8::String> string = from->ToString();
      if (!string.IsEmpty()) {
        size_t len = 3 * string->Length() + 1;
        assert(len <= INT_MAX);
        if (len > sizeof (str_st_)) {
          str_ = static_cast<char*>(malloc(len));
          assert(str_ != 0);
        }
        const int flags =
            v8::String::NO_NULL_TERMINATION | imp::kReplaceInvalidUtf8;
        length_ = string->WriteUtf8(str_, static_cast<int>(len), 0, flags);
        str_[length_] = '\0';
      }
    }
  }

  inline int length() const {
    return length_;
  }

  inline char* operator*() { return str_; }
  inline const char* operator*() const { return str_; }

  inline ~Utf8String() {
    if (str_ != str_st_) {
      free(str_);
    }
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(Utf8String)

  int length_;
  char *str_;
  char str_st_[1024];
};

typedef void (*FreeCallback)(char *data, void *hint);

typedef const FunctionCallbackInfo<v8::Value>& NAN_METHOD_ARGS_TYPE;
typedef void NAN_METHOD_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>& NAN_GETTER_ARGS_TYPE;
typedef void NAN_GETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<void>& NAN_SETTER_ARGS_TYPE;
typedef void NAN_SETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>&
    NAN_PROPERTY_GETTER_ARGS_TYPE;
typedef void NAN_PROPERTY_GETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>&
    NAN_PROPERTY_SETTER_ARGS_TYPE;
typedef void NAN_PROPERTY_SETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Array>&
    NAN_PROPERTY_ENUMERATOR_ARGS_TYPE;
typedef void NAN_PROPERTY_ENUMERATOR_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Boolean>&
    NAN_PROPERTY_DELETER_ARGS_TYPE;
typedef void NAN_PROPERTY_DELETER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Integer>&
    NAN_PROPERTY_QUERY_ARGS_TYPE;
typedef void NAN_PROPERTY_QUERY_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>& NAN_INDEX_GETTER_ARGS_TYPE;
typedef void NAN_INDEX_GETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>& NAN_INDEX_SETTER_ARGS_TYPE;
typedef void NAN_INDEX_SETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Array>&
    NAN_INDEX_ENUMERATOR_ARGS_TYPE;
typedef void NAN_INDEX_ENUMERATOR_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Boolean>&
    NAN_INDEX_DELETER_ARGS_TYPE;
typedef void NAN_INDEX_DELETER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Integer>&
    NAN_INDEX_QUERY_ARGS_TYPE;
typedef void NAN_INDEX_QUERY_RETURN_TYPE;

#define NAN_METHOD(name)                                                       \
    Nan::NAN_METHOD_RETURN_TYPE name(Nan::NAN_METHOD_ARGS_TYPE info)
#define NAN_GETTER(name)                                                       \
    Nan::NAN_GETTER_RETURN_TYPE name(                                          \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_GETTER_ARGS_TYPE info)
#define NAN_SETTER(name)                                                       \
    Nan::NAN_SETTER_RETURN_TYPE name(                                          \
        v8::Local<v8::String> property                                         \
      , v8::Local<v8::Value> value                                             \
      , Nan::NAN_SETTER_ARGS_TYPE info)
#define NAN_PROPERTY_GETTER(name)                                              \
    Nan::NAN_PROPERTY_GETTER_RETURN_TYPE name(                                 \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_PROPERTY_GETTER_ARGS_TYPE info)
#define NAN_PROPERTY_SETTER(name)                                              \
    Nan::NAN_PROPERTY_SETTER_RETURN_TYPE name(                                 \
        v8::Local<v8::String> property                                         \
      , v8::Local<v8::Value> value                                             \
      , Nan::NAN_PROPERTY_SETTER_ARGS_TYPE info)
#define NAN_PROPERTY_ENUMERATOR(name)                                          \
    Nan::NAN_PROPERTY_ENUMERATOR_RETURN_TYPE name(                             \
        Nan::NAN_PROPERTY_ENUMERATOR_ARGS_TYPE info)
#define NAN_PROPERTY_DELETER(name)                                             \
    Nan::NAN_PROPERTY_DELETER_RETURN_TYPE name(                                \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_PROPERTY_DELETER_ARGS_TYPE info)
#define NAN_PROPERTY_QUERY(name)                                               \
    Nan::NAN_PROPERTY_QUERY_RETURN_TYPE name(                                  \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_PROPERTY_QUERY_ARGS_TYPE info)
#define NAN_INDEX_GETTER(name)                                                \
    Nan::NAN_INDEX_GETTER_RETURN_TYPE name(                                    \
        uint32_t index                                                         \
      , Nan::NAN_INDEX_GETTER_ARGS_TYPE info)
#define NAN_INDEX_SETTER(name)                                                 \
    Nan::NAN_INDEX_SETTER_RETURN_TYPE name(                                    \
        uint32_t index                                                         \
      , v8::Local<v8::Value> value                                             \
      , Nan::NAN_INDEX_SETTER_ARGS_TYPE info)
#define NAN_INDEX_ENUMERATOR(name)                                             \
    Nan::NAN_INDEX_ENUMERATOR_RETURN_TYPE                                      \
    name(Nan::NAN_INDEX_ENUMERATOR_ARGS_TYPE info)
#define NAN_INDEX_DELETER(name)                                                \
    Nan::NAN_INDEX_DELETER_RETURN_TYPE name(                                   \
        uint32_t index                                                         \
      , Nan::NAN_INDEX_DELETER_ARGS_TYPE info)
#define NAN_INDEX_QUERY(name)                                                  \
    Nan::NAN_INDEX_QUERY_RETURN_TYPE name(                                     \
        uint32_t index                                                         \
      , Nan::NAN_INDEX_QUERY_ARGS_TYPE info)

class Callback {
 public:
  Callback() {}

  explicit Callback(const v8::Local<v8::Function> &fn) : handle_(fn) {}

  ~Callback() {
    handle_.Reset();
  }

  bool operator==(const Callback &other) const {
    return handle_ == other.handle_;
  }

  bool operator!=(const Callback &other) const {
    return !operator==(other);
  }

  inline
  v8::Local<v8::Function> operator*() const { return GetFunction(); }

  NAN_DEPRECATED inline v8::Local<v8::Value> operator()(
      v8::Local<v8::Object> target
    , int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
	return Call_(isolate, target, argc, argv);
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> operator()(
      int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(
	  Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv));
  }

  inline MaybeLocal<v8::Value> operator()(
      AsyncResource* resource
    , int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
    return this->Call(argc, argv, resource);
  }

  inline MaybeLocal<v8::Value> operator()(
      AsyncResource* resource
    , v8::Local<v8::Object> target
    , int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
    return this->Call(target, argc, argv, resource);
  }

  // TODO(kkoopa): remove
  inline void SetFunction(const v8::Local<v8::Function> &fn) {
    Reset(fn);
  }

  inline void Reset(const v8::Local<v8::Function> &fn) {
    handle_.Reset(fn);
  }

  inline void Reset() {
    handle_.Reset();
  }

  inline v8::Local<v8::Function> GetFunction() const {
    return New(handle_);
  }

  inline bool IsEmpty() const {
    return handle_.IsEmpty();
  }

  // Deprecated: For async callbacks Use the versions that accept an
  // AsyncResource. If this callback does not correspond to an async resource,
  // that is, it is a synchronous function call on a non-empty JS stack, you
  // should Nan::Call instead.
  NAN_DEPRECATED inline v8::Local<v8::Value>
  Call(v8::Local<v8::Object> target
     , int argc
     , v8::Local<v8::Value> argv[]) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
	return Call_(isolate, target, argc, argv);
  }

  // Deprecated: For async callbacks Use the versions that accept an
  // AsyncResource. If this callback does not correspond to an async resource,
  // that is, it is a synchronous function call on a non-empty JS stack, you
  // should Nan::Call instead.
  NAN_DEPRECATED inline v8::Local<v8::Value>
  Call(int argc, v8::Local<v8::Value> argv[]) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(
        Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv));
  }

  inline MaybeLocal<v8::Value>
  Call(v8::Local<v8::Object> target
     , int argc
     , v8::Local<v8::Value> argv[]
     , AsyncResource* resource) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    return Call_(isolate, target, argc, argv);
  }

  inline MaybeLocal<v8::Value>
  Call(int argc, v8::Local<v8::Value> argv[], AsyncResource* resource) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(
        Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv));
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(Callback)
  Persistent<v8::Function> handle_;
  v8::Local<v8::Value> Call_(v8::Isolate *isolate
                           , v8::Local<v8::Object> target
                           , int argc
                           , v8::Local<v8::Value> argv[]) const {
#if 0 //?jw
    EscapableHandleScope scope;
    v8::Local<v8::Function> callback = New(handle_);
		return scope.Escape(New(node::MakeCallback(
			target
		  , callback
		  , argc
		  , argv
		)));
#else
		return target;
#endif

    }
};

inline MaybeLocal<v8::Value> Call(
    const Nan::Callback& callback
  , v8::Local<v8::Object> recv
  , int argc
  , v8::Local<v8::Value> argv[]) {
  return Call(*callback, recv, argc, argv);
}

inline MaybeLocal<v8::Value> Call(
    const Nan::Callback& callback
  , int argc
  , v8::Local<v8::Value> argv[]) {

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);
  return scope.Escape(
      Call(*callback, isolate->GetCurrentContext()->Global(), argc, argv)
          .FromMaybe(v8::Local<v8::Value>()));
}

inline MaybeLocal<v8::Value> Call(
    v8::Local<v8::String> symbol
  , v8::Local<v8::Object> recv
  , int argc
  , v8::Local<v8::Value> argv[]) {
  EscapableHandleScope scope;
  v8::Local<v8::Value> fn_v =
      Get(recv, symbol).FromMaybe(v8::Local<v8::Value>());
  if (fn_v.IsEmpty() || !fn_v->IsFunction()) return v8::Local<v8::Value>();
  v8::Local<v8::Function> fn = fn_v.As<v8::Function>();
  return scope.Escape(
      Call(fn, recv, argc, argv).FromMaybe(v8::Local<v8::Value>()));
}

inline MaybeLocal<v8::Value> Call(
    const char* method
  , v8::Local<v8::Object> recv
  , int argc
  , v8::Local<v8::Value> argv[]) {
  EscapableHandleScope scope;
  v8::Local<v8::String> method_string =
      New<v8::String>(method).ToLocalChecked();
  return scope.Escape(
      Call(method_string, recv, argc, argv).FromMaybe(v8::Local<v8::Value>()));
}


namespace imp {

inline
ExternalOneByteStringResource const*
GetExternalResource(v8::Local<v8::String> str) {
    return str->GetExternalOneByteStringResource();
}

inline
bool
IsExternal(v8::Local<v8::String> str) {
    return str->IsExternalOneByte();
}

}  // end of namespace imp
#if 0//?
enum Encoding {ASCII, UTF8, BASE64, UCS2, BINARY, HEX, BUFFER};

inline v8::Local<v8::Value> Encode(
    const void *buf, size_t len, enum Encoding encoding = BINARY) {
  return node::Encode(
      v8::Isolate::GetCurrent()
    , buf, len
    , static_cast<node::encoding>(encoding));
}

inline ssize_t DecodeBytes(
    v8::Local<v8::Value> val, enum Encoding encoding = BINARY) {
  return node::DecodeBytes(
      v8::Isolate::GetCurrent()
    , val
    , static_cast<node::encoding>(encoding));
}

inline ssize_t DecodeWrite(
    char *buf
  , size_t len
  , v8::Local<v8::Value> val
  , enum Encoding encoding = BINARY) {
  return node::DecodeWrite(
      v8::Isolate::GetCurrent()
    , buf
    , len
    , val
    , static_cast<node::encoding>(encoding));
}
#endif //?jw
inline void SetPrototypeTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , const char *name
  , v8::Local<v8::Data> value
) {
  HandleScope scope;
  SetTemplate(templ->PrototypeTemplate(), name, value);
}

inline void SetPrototypeTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , v8::Local<v8::String> name
  , v8::Local<v8::Data> value
  , v8::PropertyAttribute attributes
) {
  HandleScope scope;
  SetTemplate(templ->PrototypeTemplate(), name, value, attributes);
}

inline void SetInstanceTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , const char *name
  , v8::Local<v8::Data> value
) {
  HandleScope scope;
  SetTemplate(templ->InstanceTemplate(), name, value);
}

inline void SetInstanceTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , v8::Local<v8::String> name
  , v8::Local<v8::Data> value
  , v8::PropertyAttribute attributes
) {
  HandleScope scope;
  SetTemplate(templ->InstanceTemplate(), name, value, attributes);
}

namespace imp {

// Note(@agnat): Helper to distinguish different receiver types. The first
// version deals with receivers derived from v8::Template. The second version
// handles everything else. The final argument only serves as discriminator and
// is unused.
template <typename T>
inline
void
SetMethodAux(T recv,
             v8::Local<v8::String> name,
             v8::Local<v8::FunctionTemplate> tpl,
             v8::Template *) {
  recv->Set(name, tpl);
}

template <typename T>
inline
void
SetMethodAux(T recv,
             v8::Local<v8::String> name,
             v8::Local<v8::FunctionTemplate> tpl,
             ...) {
  recv->Set(name, GetFunction(tpl).ToLocalChecked());
}

}  // end of namespace imp

template <typename T, template <typename> class HandleType>
inline void SetMethod(
    HandleType<T> recv
  , const char *name
  , FunctionCallback callback) {
  HandleScope scope;
  v8::Local<v8::FunctionTemplate> t = New<v8::FunctionTemplate>(callback);
  v8::Local<v8::String> fn_name = New(name).ToLocalChecked();
  t->SetClassName(fn_name);
  // Note(@agnat): Pass an empty T* as discriminator. See note on
  // SetMethodAux(...) above
  imp::SetMethodAux(recv, fn_name, t, static_cast<T*>(0));
}

inline void SetPrototypeMethod(
    v8::Local<v8::FunctionTemplate> recv
  , const char* name, FunctionCallback callback) {

  HandleScope scope;
  v8::Local<v8::FunctionTemplate> t = New<v8::FunctionTemplate>(
      callback
    , v8::Local<v8::Value>()
    , New<v8::Signature>(recv));

  v8::Local<v8::String> fn_name = New(name).ToLocalChecked();
  recv->PrototypeTemplate()->Set(fn_name, t);
  t->SetClassName(fn_name);
}

//=== Accessors and Such =======================================================

inline void SetAccessor(
    v8::Local<v8::ObjectTemplate> tpl
  , v8::Local<v8::String> name
  , GetterCallback getter
  , SetterCallback setter = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()
  , v8::AccessControl settings = v8::DEFAULT
  , v8::PropertyAttribute attribute = v8::None
  , imp::Sig signature = imp::Sig()) {
  HandleScope scope;

  imp::NativeGetter getter_ =
      imp::GetterCallbackWrapper;
  imp::NativeSetter setter_ =
      setter ? imp::SetterCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kAccessorFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();

  obj->SetInternalField(
      imp::kGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (setter != 0) {
    obj->SetInternalField(
        imp::kSetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetAccessor(
      name
    , getter_
    , setter_
    , obj
    , settings
    , attribute
    , signature);
}

inline bool SetAccessor(
    v8::Local<v8::Object> obj
  , v8::Local<v8::String> name
  , GetterCallback getter
  , SetterCallback setter = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()
  , v8::AccessControl settings = v8::DEFAULT
  , v8::PropertyAttribute attribute = v8::None) {
  HandleScope scope;
#if 0//?jw
  imp::NativeGetter getter_ =
      imp::GetterCallbackWrapper;
  imp::NativeSetter setter_ =
      setter ? imp::SetterCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kAccessorFieldCount);
  v8::Local<v8::Object> dataobj = NewInstance(otpl).ToLocalChecked();

  dataobj->SetInternalField(
      imp::kGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (!data.IsEmpty()) {
    dataobj->SetInternalField(imp::kDataIndex, data);
  }

  if (setter) {
    dataobj->SetInternalField(
        imp::kSetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  return obj->SetAccessor(
      name
    , getter_
    , setter_
    , dataobj
    , settings
    , attribute);
#else
    return false;
#endif
}

inline void SetNamedPropertyHandler(
    v8::Local<v8::ObjectTemplate> tpl
  , PropertyGetterCallback getter
  , PropertySetterCallback setter = 0
  , PropertyQueryCallback query = 0
  , PropertyDeleterCallback deleter = 0
  , PropertyEnumeratorCallback enumerator = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;
#if 0//?jw
  imp::NativePropertyGetter getter_ =
      imp::PropertyGetterCallbackWrapper;
  imp::NativePropertySetter setter_ =
      setter ? imp::PropertySetterCallbackWrapper : 0;
  imp::NativePropertyQuery query_ =
      query ? imp::PropertyQueryCallbackWrapper : 0;
  imp::NativePropertyDeleter *deleter_ =
      deleter ? imp::PropertyDeleterCallbackWrapper : 0;
  imp::NativePropertyEnumerator enumerator_ =
      enumerator ? imp::PropertyEnumeratorCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kPropertyFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();
  obj->SetInternalField(
      imp::kPropertyGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (setter) {
    obj->SetInternalField(
        imp::kPropertySetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  if (query) {
    obj->SetInternalField(
        imp::kPropertyQueryIndex
      , New<v8::External>(reinterpret_cast<void *>(query)));
  }

  if (deleter) {
    obj->SetInternalField(
        imp::kPropertyDeleterIndex
      , New<v8::External>(reinterpret_cast<void *>(deleter)));
  }

  if (enumerator) {
    obj->SetInternalField(
        imp::kPropertyEnumeratorIndex
      , New<v8::External>(reinterpret_cast<void *>(enumerator)));
  }

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetNamedPropertyHandler(
      getter_
    , setter_
    , query_
    , deleter_
    , enumerator_
    , obj);
#endif
}

inline void SetIndexedPropertyHandler(
    v8::Local<v8::ObjectTemplate> tpl
  , IndexGetterCallback getter
  , IndexSetterCallback setter = 0
  , IndexQueryCallback query = 0
  , IndexDeleterCallback deleter = 0
  , IndexEnumeratorCallback enumerator = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  imp::NativeIndexGetter getter_ =
      imp::IndexGetterCallbackWrapper;
  imp::NativeIndexSetter setter_ =
      setter ? imp::IndexSetterCallbackWrapper : 0;
  imp::NativeIndexQuery query_ =
      query ? imp::IndexQueryCallbackWrapper : 0;
  imp::NativeIndexDeleter deleter_ =
      deleter ? imp::IndexDeleterCallbackWrapper : 0;
  imp::NativeIndexEnumerator enumerator_ =
      enumerator ? imp::IndexEnumeratorCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kIndexPropertyFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();
  obj->SetInternalField(
      imp::kIndexPropertyGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (setter) {
    obj->SetInternalField(
        imp::kIndexPropertySetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  if (query) {
    obj->SetInternalField(
        imp::kIndexPropertyQueryIndex
      , New<v8::External>(reinterpret_cast<void *>(query)));
  }

  if (deleter) {
    obj->SetInternalField(
        imp::kIndexPropertyDeleterIndex
      , New<v8::External>(reinterpret_cast<void *>(deleter)));
  }

  if (enumerator) {
    obj->SetInternalField(
        imp::kIndexPropertyEnumeratorIndex
      , New<v8::External>(reinterpret_cast<void *>(enumerator)));
  }

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetIndexedPropertyHandler(
      getter_
    , setter_
    , query_
    , deleter_
    , enumerator_
    , obj);
}

inline void SetCallHandler(
    v8::Local<v8::FunctionTemplate> tpl
  , FunctionCallback callback
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kFunctionFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();

  obj->SetInternalField(
      imp::kFunctionIndex
    , New<v8::External>(reinterpret_cast<void *>(callback)));

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetCallHandler(imp::FunctionCallbackWrapper, obj);
}


inline void SetCallAsFunctionHandler(
    v8::Local<v8::ObjectTemplate> tpl,
    FunctionCallback callback,
    v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kFunctionFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();

  obj->SetInternalField(
      imp::kFunctionIndex
    , New<v8::External>(reinterpret_cast<void *>(callback)));

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetCallAsFunctionHandler(imp::FunctionCallbackWrapper, obj);
}

//=== Weak Persistent Handling =================================================

#include "nan_weak.h"  // NOLINT(build/include)

//=== ObjectWrap ===============================================================

#include "nan_object_wrap.h"  // NOLINT(build/include)

//=== HiddenValue/Private ======================================================

#include "nan_private.h"  // NOLINT(build/include)

//=== Export ==================================================================

inline
void
Export(ADDON_REGISTER_FUNCTION_ARGS_TYPE target, const char *name,
    FunctionCallback f) {
  HandleScope scope;

  Set(target, New<v8::String>(name).ToLocalChecked(),
      GetFunction(New<v8::FunctionTemplate>(f)).ToLocalChecked());
}

//=== Tap Reverse Binding =====================================================

struct Tap {
  explicit Tap(v8::Local<v8::Value> t) : t_() {
    HandleScope scope;

    t_.Reset(To<v8::Object>(t).ToLocalChecked());
  }

  ~Tap() { t_.Reset(); }  // not sure if neccessary

  inline void plan(int i) {
    HandleScope scope;
    v8::Local<v8::Value> arg = New(i);
    Call("plan", New(t_), 1, &arg);
  }

  inline void ok(bool isOk, const char *msg = NULL) {
    HandleScope scope;
    v8::Local<v8::Value> args[2];
    args[0] = New(isOk);
    if (msg) args[1] = New(msg).ToLocalChecked();
    Call("ok", New(t_), msg ? 2 : 1, args);
  }

  inline void pass(const char * msg = NULL) {
    HandleScope scope;
    v8::Local<v8::Value> hmsg;
    if (msg) hmsg = New(msg).ToLocalChecked();
    Call("pass", New(t_), msg ? 1 : 0, &hmsg);
  }

  inline void end() {
    HandleScope scope;
    Call("end", New(t_), 0, NULL);
  }

 private:
  Persistent<v8::Object> t_;
};

#define NAN_STRINGIZE2(x) #x
#define NAN_STRINGIZE(x) NAN_STRINGIZE2(x)
#define NAN_TEST_EXPRESSION(expression) \
  ( expression ), __FILE__ ":" NAN_STRINGIZE(__LINE__) ": " #expression

#define NAN_EXPORT(target, function) Export(target, #function, function)

#undef TYPE_CHECK

//=== Generic Maybefication ===================================================

namespace imp {

template <typename T> struct Maybefier;

template <typename T> struct Maybefier<v8::Local<T> > {
  inline static MaybeLocal<T> convert(v8::Local<T> v) {
    return v;
  }
};

template <typename T> struct Maybefier<MaybeLocal<T> > {
  inline static MaybeLocal<T> convert(MaybeLocal<T> v) {
    return v;
  }
};

}  // end of namespace imp

template <typename T, template <typename> class MaybeMaybe>
inline MaybeLocal<T>
MakeMaybe(MaybeMaybe<T> v) {
  return imp::Maybefier<MaybeMaybe<T> >::convert(v);
}

//=== TypedArrayContents =======================================================

#include "nan_typedarray_contents.h"  // NOLINT(build/include)

}  // end of namespace Nan

#endif  // NAN_H_
