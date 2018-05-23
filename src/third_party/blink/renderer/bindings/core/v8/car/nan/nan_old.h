/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2018 NAN contributors:
 *   - Rod Vagg <https://github.com/rvagg>
 *   - Benjamin Byholm <https://github.com/kkoopa>
 *   - Trevor Norris <https://github.com/trevnorris>
 *   - Nathan Rajlich <https://github.com/TooTallNate>
 *   - Brett Lawson <https://github.com/brett19>
 *   - Ben Noordhuis <https://github.com/bnoordhuis>
 *   - David Siegel <https://github.com/agnat>
 *   - Michael Ira Krufky <https://github.com/mkrufky>
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 *
 * Version 2.10.0: current Node 9.8.0, Node 12: 0.12.18, Node 10: 0.10.48, iojs: 3.3.1
 *
 * See https://github.com/nodejs/nan for the latest update to this file
 **********************************************************************************/

#ifndef NAN_H_
#define NAN_H_

//#include <node_version.h>

#define NODE_0_10_MODULE_VERSION 11
#define NODE_0_12_MODULE_VERSION 14
#define ATOM_0_21_MODULE_VERSION 41
#define IOJS_1_0_MODULE_VERSION  42
#define IOJS_1_1_MODULE_VERSION  43
#define IOJS_2_0_MODULE_VERSION  44
#define IOJS_3_0_MODULE_VERSION  45
#define NODE_4_0_MODULE_VERSION  46
#define NODE_5_0_MODULE_VERSION  47
#define NODE_6_0_MODULE_VERSION  48
#define NODE_7_0_MODULE_VERSION  51
#define NODE_8_0_MODULE_VERSION  57
#define NODE_9_0_MODULE_VERSION  59

#ifdef _MSC_VER
# define NAN_HAS_CPLUSPLUS_11 (_MSC_VER >= 1800)
#else
# define NAN_HAS_CPLUSPLUS_11 (__cplusplus >= 201103L)
#endif

#if NODE_MODULE_VERSION >= IOJS_3_0_MODULE_VERSION && !NAN_HAS_CPLUSPLUS_11
# error This version of node/NAN/v8 requires a C++11 compiler
#endif

#include <uv.h>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>
#include <algorithm>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <utility>
#if defined(_MSC_VER)
# pragma warning( push )
# pragma warning( disable : 4530 )
# include <queue>
# include <string>
# include <vector>
# pragma warning( pop )
#else
# include <queue>
# include <string>
# include <vector>
#endif

// uv helpers
#ifdef UV_VERSION_MAJOR
# ifndef UV_VERSION_PATCH
#  define UV_VERSION_PATCH 0
# endif
# define NAUV_UVVERSION ((UV_VERSION_MAJOR << 16) | \
                         (UV_VERSION_MINOR <<  8) | \
                         (UV_VERSION_PATCH))
#else
# define NAUV_UVVERSION 0x000b00
#endif

#if NAUV_UVVERSION < 0x000b0b
# ifdef WIN32
#  include <windows.h>
# else
#  include <pthread.h>
# endif
#endif

namespace Nan {

#define NAN_INLINE inline  // TODO(bnoordhuis) Remove in v3.0.0.

#if defined(__GNUC__) && \
    !(defined(V8_DISABLE_DEPRECATIONS) && V8_DISABLE_DEPRECATIONS)
# define NAN_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && \
    !(defined(V8_DISABLE_DEPRECATIONS) && V8_DISABLE_DEPRECATIONS)
# define NAN_DEPRECATED __declspec(deprecated)
#else
# define NAN_DEPRECATED
#endif

#if NAN_HAS_CPLUSPLUS_11
# define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&) = delete;
# define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&) = delete;
# define NAN_DISALLOW_MOVE(CLASS)                                              \
    CLASS(CLASS&&) = delete;  /* NOLINT(build/c++11) */                        \
    void operator=(CLASS&&) = delete;
#else
# define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&);
# define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&);
# define NAN_DISALLOW_MOVE(CLASS)
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


# include "nan_maybe_43_inl.h"  // NOLINT(build/include)


#include "nan_converters.h"  // NOLINT(build/include)
#include "nan_new.h"  // NOLINT(build/include)


#if NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
template<typename T>
v8::Local<T> New(v8::Handle<T>);
#endif

typedef v8::WeakCallbackType WeakCallbackType;


template<typename P> class WeakCallbackInfo;

# include "nan_persistent_12_inl.h"  // NOLINT(build/include)

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


//============ =================================================================

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

#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION >= 3))
  inline bool IdleNotification(int idle_time_in_ms) {
    return v8::Isolate::GetCurrent()->IdleNotificationDeadline(
        idle_time_in_ms * 0.001);
  }
# else
  inline bool IdleNotification(int idle_time_in_ms) {
    return v8::Isolate::GetCurrent()->IdleNotification(idle_time_in_ms);
  }
#endif

  inline void LowMemoryNotification() {
    v8::Isolate::GetCurrent()->LowMemoryNotification();
  }

  inline void ContextDisposedNotification() {
    v8::Isolate::GetCurrent()->ContextDisposedNotification();
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

# define NAN_GC_CALLBACK(name)                                                 \
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

# define X(NAME)                                                               \
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
    inline void Throw ## NAME(const char *msg) {                               \
      HandleScope scope;                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(New(msg).ToLocalChecked()));                     \
    }                                                                          \
                                                                               \
    inline void Throw ## NAME(v8::Local<v8::String> msg) {                     \
      HandleScope scope;                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(msg));                                           \
    }

  X(Error)
  X(RangeError)
  X(ReferenceError)
  X(SyntaxError)
  X(TypeError)

# undef X

  inline void ThrowError(v8::Local<v8::Value> error) {
    v8::Isolate::GetCurrent()->ThrowException(error);
  }

 
  NAN_DEPRECATED inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::Function> func
    , int argc
    , v8::Local<v8::Value>* argv) {
#if NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
    EscapableHandleScope scope;
    return scope.Escape(New(node::MakeCallback(
        v8::Isolate::GetCurrent(), target, func, argc, argv)));
#else
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource res("nan:makeCallback");
    return res.runInAsyncScope(target, func, argc, argv)
        .FromMaybe(v8::Local<v8::Value>());
# else
    return node::MakeCallback(
        v8::Isolate::GetCurrent(), target, func, argc, argv);
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#endif  // NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::String> symbol
    , int argc
    , v8::Local<v8::Value>* argv) {
#if NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
    EscapableHandleScope scope;
    return scope.Escape(New(node::MakeCallback(
        v8::Isolate::GetCurrent(), target, symbol, argc, argv)));
#else
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource res("nan:makeCallback");
    return res.runInAsyncScope(target, symbol, argc, argv)
        .FromMaybe(v8::Local<v8::Value>());
# else
    return node::MakeCallback(
        v8::Isolate::GetCurrent(), target, symbol, argc, argv);
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#endif  // NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , const char* method
    , int argc
    , v8::Local<v8::Value>* argv) {
#if NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
    EscapableHandleScope scope;
    return scope.Escape(New(node::MakeCallback(
        v8::Isolate::GetCurrent(), target, method, argc, argv)));
#else
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource res("nan:makeCallback");
    return res.runInAsyncScope(target, method, argc, argv)
        .FromMaybe(v8::Local<v8::Value>());
# else
    return node::MakeCallback(
        v8::Isolate::GetCurrent(), target, method, argc, argv);
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#endif  // NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
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

  NAN_DEPRECATED inline v8::Local<v8::Value> NanErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
    return ErrnoException(errorno, syscall, message, path);
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


namespace imp {
inline void
widenString(std::vector<uint16_t> *ws, const uint8_t *s, int l) {
  size_t len = static_cast<size_t>(l);
  if (l < 0) {
    len = strlen(reinterpret_cast<const char*>(s));
  }
  assert(len <= INT_MAX && "string too long");
  ws->resize(len);
  std::copy(s, s + len, ws->begin());  // NOLINT(build/include_what_you_use)
}
}  // end of namespace imp

  inline MaybeLocal<v8::String>
  NewOneByteString(const uint8_t * value, int length = -1) {
    std::vector<uint16_t> wideString;  // NOLINT(build/include_what_you_use)
    imp::widenString(&wideString, value, length);
    return v8::String::New(wideString.data(),
                           static_cast<int>(wideString.size()));
  }

  inline MaybeLocal<BoundScript> CompileScript(
      v8::Local<v8::String> s
    , const v8::ScriptOrigin& origin
  ) {
    return v8::Script::Compile(s, const_cast<v8::ScriptOrigin *>(&origin));
  }

  inline MaybeLocal<BoundScript> CompileScript(
    v8::Local<v8::String> s
  ) {
    return v8::Script::Compile(s);
  }

  inline
  MaybeLocal<v8::Value> RunScript(v8::Local<v8::Script> script) {
    return script->Run();
  }

  inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::Function> func
    , int argc
    , v8::Local<v8::Value>* argv) {
    v8::HandleScope scope;
    return scope.Close(New(node::MakeCallback(target, func, argc, argv)));
  }

  inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::String> symbol
    , int argc
    , v8::Local<v8::Value>* argv) {
    v8::HandleScope scope;
    return scope.Close(New(node::MakeCallback(target, symbol, argc, argv)));
  }

  inline v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , const char* method
    , int argc
    , v8::Local<v8::Value>* argv) {
    v8::HandleScope scope;
    return scope.Close(New(node::MakeCallback(target, method, argc, argv)));
  }

  inline void FatalException(const TryCatch& try_catch) {
    node::FatalException(const_cast<v8::TryCatch &>(try_catch.try_catch_));
  }

  inline v8::Local<v8::Value> ErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
    return node::ErrnoException(errorno, syscall, message, path);
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> NanErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
    return ErrnoException(errorno, syscall, message, path);
  }


  template<typename T>
  inline void SetIsolateData(
      v8::Isolate *isolate
    , T *data
  ) {
      isolate->SetData(data);
  }

  template<typename T>
  inline T *GetIsolateData(
      v8::Isolate *isolate
  ) {
      return static_cast<T*>(isolate->GetData());
  }

class Utf8String {
 public:
  inline explicit Utf8String(v8::Local<v8::Value> from) :
      length_(0), str_(str_st_) {
    v8::HandleScope scope;
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

#endif  // NODE_MODULE_VERSION

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
# define NAN_INDEX_GETTER(name)                                                \
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
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource async("nan:Callback:operator()");
    return Call_(isolate, target, argc, argv, &async)
        .FromMaybe(v8::Local<v8::Value>());
# else
    return Call_(isolate, target, argc, argv);
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#else
    return Call_(target, argc, argv);
#endif  //  NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
  }

  NAN_DEPRECATED inline v8::Local<v8::Value> operator()(
      int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource async("nan:Callback:operator()");
    return scope.Escape(Call_(isolate, isolate->GetCurrentContext()->Global(),
                              argc, argv, &async)
                            .FromMaybe(v8::Local<v8::Value>()));
# else
    return scope.Escape(
        Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv));
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#else
    v8::HandleScope scope;
    return scope.Close(Call_(v8::Context::GetCurrent()->Global(), argc, argv));
#endif  //  NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
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
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource async("nan:Callback:Call");
    return Call_(isolate, target, argc, argv, &async)
        .FromMaybe(v8::Local<v8::Value>());
# else
    return Call_(isolate, target, argc, argv);
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#else
    return Call_(target, argc, argv);
#endif
  }

  // Deprecated: For async callbacks Use the versions that accept an
  // AsyncResource. If this callback does not correspond to an async resource,
  // that is, it is a synchronous function call on a non-empty JS stack, you
  // should Nan::Call instead.
  NAN_DEPRECATED inline v8::Local<v8::Value>
  Call(int argc, v8::Local<v8::Value> argv[]) const {
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
# if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    AsyncResource async("nan:Callback:Call");
    return Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv,
                 &async)
        .FromMaybe(v8::Local<v8::Value>());
# else
    return scope.Escape(
        Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv));
# endif  // NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#else
    v8::HandleScope scope;
    return scope.Close(Call_(v8::Context::GetCurrent()->Global(), argc, argv));
#endif
  }

  inline MaybeLocal<v8::Value>
  Call(v8::Local<v8::Object> target
     , int argc
     , v8::Local<v8::Value> argv[]
     , AsyncResource* resource) const {
#if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    return Call_(isolate, target, argc, argv, resource);
#elif NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    return Call_(isolate, target, argc, argv);
#else
    return Call_(target, argc, argv);
#endif
  }

  inline MaybeLocal<v8::Value>
  Call(int argc, v8::Local<v8::Value> argv[], AsyncResource* resource) const {
#if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    return Call(isolate->GetCurrentContext()->Global(), argc, argv, resource);
#elif NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(
        Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv));
#else
    v8::HandleScope scope;
    return scope.Close(Call_(v8::Context::GetCurrent()->Global(), argc, argv));
#endif
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(Callback)
  Persistent<v8::Function> handle_;

#if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
  MaybeLocal<v8::Value> Call_(v8::Isolate *isolate
                            , v8::Local<v8::Object> target
                            , int argc
                            , v8::Local<v8::Value> argv[]
                            , AsyncResource* resource) const {
    EscapableHandleScope scope;
    v8::Local<v8::Function> func = New(handle_);
    auto maybe = resource->runInAsyncScope(target, func, argc, argv);
    v8::Local<v8::Value> local;
    if (!maybe.ToLocal(&local)) return MaybeLocal<v8::Value>();
    return scope.Escape(local);
  }
#elif NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
  v8::Local<v8::Value> Call_(v8::Isolate *isolate
                           , v8::Local<v8::Object> target
                           , int argc
                           , v8::Local<v8::Value> argv[]) const {
    EscapableHandleScope scope;

    v8::Local<v8::Function> callback = New(handle_);
# if NODE_MODULE_VERSION < IOJS_3_0_MODULE_VERSION
    return scope.Escape(New(node::MakeCallback(
        isolate
      , target
      , callback
      , argc
      , argv
    )));
# else
    return scope.Escape(node::MakeCallback(
        isolate
      , target
      , callback
      , argc
      , argv
    ));
# endif
  }
#else
  v8::Local<v8::Value> Call_(v8::Local<v8::Object> target
                           , int argc
                           , v8::Local<v8::Value> argv[]) const {
    EscapableHandleScope scope;

    v8::Local<v8::Function> callback = New(handle_);
    return scope.Escape(New(node::MakeCallback(
        target
      , callback
      , argc
      , argv
    )));
  }
#endif
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
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);
  return scope.Escape(
      Call(*callback, isolate->GetCurrentContext()->Global(), argc, argv)
          .FromMaybe(v8::Local<v8::Value>()));
#else
  EscapableHandleScope scope;
  return scope.Escape(
      Call(*callback, v8::Context::GetCurrent()->Global(), argc, argv)
          .FromMaybe(v8::Local<v8::Value>()));
#endif
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

/* abstract */ class AsyncWorker {
 public:
  explicit AsyncWorker(Callback *callback_,
                       const char* resource_name = "nan:AsyncWorker")
      : callback(callback_), errmsg_(NULL) {
    request.data = this;

    HandleScope scope;
    v8::Local<v8::Object> obj = New<v8::Object>();
    persistentHandle.Reset(obj);
    async_resource = new AsyncResource(resource_name, obj);
  }

  virtual ~AsyncWorker() {
    HandleScope scope;

    if (!persistentHandle.IsEmpty())
      persistentHandle.Reset();
    delete callback;
    delete[] errmsg_;
    delete async_resource;
  }

  virtual void WorkComplete() {
    HandleScope scope;

    if (errmsg_ == NULL)
      HandleOKCallback();
    else
      HandleErrorCallback();
    delete callback;
    callback = NULL;
  }

  inline void SaveToPersistent(
      const char *key, const v8::Local<v8::Value> &value) {
    HandleScope scope;
    New(persistentHandle)->Set(New(key).ToLocalChecked(), value);
  }

  inline void SaveToPersistent(
      const v8::Local<v8::String> &key, const v8::Local<v8::Value> &value) {
    HandleScope scope;
    New(persistentHandle)->Set(key, value);
  }

  inline void SaveToPersistent(
      uint32_t index, const v8::Local<v8::Value> &value) {
    HandleScope scope;
    New(persistentHandle)->Set(index, value);
  }

  inline v8::Local<v8::Value> GetFromPersistent(const char *key) const {
    EscapableHandleScope scope;
    return scope.Escape(
        New(persistentHandle)->Get(New(key).ToLocalChecked()));
  }

  inline v8::Local<v8::Value>
  GetFromPersistent(const v8::Local<v8::String> &key) const {
    EscapableHandleScope scope;
    return scope.Escape(New(persistentHandle)->Get(key));
  }

  inline v8::Local<v8::Value> GetFromPersistent(uint32_t index) const {
    EscapableHandleScope scope;
    return scope.Escape(New(persistentHandle)->Get(index));
  }

  virtual void Execute() = 0;

  uv_work_t request;

  virtual void Destroy() {
      delete this;
  }

 protected:
  Persistent<v8::Object> persistentHandle;
  Callback *callback;
  AsyncResource *async_resource;

  virtual void HandleOKCallback() {
    HandleScope scope;

    callback->Call(0, NULL, async_resource);
  }

  virtual void HandleErrorCallback() {
    HandleScope scope;

    v8::Local<v8::Value> argv[] = {
      v8::Exception::Error(New<v8::String>(ErrorMessage()).ToLocalChecked())
    };
    callback->Call(1, argv, async_resource);
  }

  void SetErrorMessage(const char *msg) {
    delete[] errmsg_;

    size_t size = strlen(msg) + 1;
    errmsg_ = new char[size];
    memcpy(errmsg_, msg, size);
  }

  const char* ErrorMessage() const {
    return errmsg_;
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(AsyncWorker)
  char *errmsg_;
};

/* abstract */ class AsyncBareProgressWorkerBase : public AsyncWorker {
 public:
  explicit AsyncBareProgressWorkerBase(
      Callback *callback_,
      const char* resource_name = "nan:AsyncBareProgressWorkerBase")
      : AsyncWorker(callback_, resource_name) {
    uv_async_init(
        uv_default_loop()
      , &async
      , AsyncProgress_
    );
    async.data = this;
  }

  virtual ~AsyncBareProgressWorkerBase() {
  }

  virtual void WorkProgress() = 0;

  virtual void Destroy() {
      uv_close(reinterpret_cast<uv_handle_t*>(&async), AsyncClose_);
  }

 private:
  inline static NAUV_WORK_CB(AsyncProgress_) {
    AsyncBareProgressWorkerBase *worker =
            static_cast<AsyncBareProgressWorkerBase*>(async->data);
    worker->WorkProgress();
  }

  inline static void AsyncClose_(uv_handle_t* handle) {
    AsyncBareProgressWorkerBase *worker =
            static_cast<AsyncBareProgressWorkerBase*>(handle->data);
    delete worker;
  }

 protected:
  uv_async_t async;
};

template<class T>
/* abstract */
class AsyncBareProgressWorker : public AsyncBareProgressWorkerBase {
 public:
  explicit AsyncBareProgressWorker(
      Callback *callback_,
      const char* resource_name = "nan:AsyncBareProgressWorker")
      : AsyncBareProgressWorkerBase(callback_, resource_name) {
  }

  virtual ~AsyncBareProgressWorker() {
  }

  class ExecutionProgress {
    friend class AsyncBareProgressWorker;
   public:
    void Signal() const {
      uv_async_send(&that_->async);
    }

    void Send(const T* data, size_t count) const {
      that_->SendProgress_(data, count);
    }

   private:
    explicit ExecutionProgress(AsyncBareProgressWorker *that) : that_(that) {}
    NAN_DISALLOW_ASSIGN_COPY_MOVE(ExecutionProgress)
    AsyncBareProgressWorker* const that_;
  };

  virtual void Execute(const ExecutionProgress& progress) = 0;
  virtual void HandleProgressCallback(const T *data, size_t size) = 0;

 private:
  void Execute() /*final override*/ {
    ExecutionProgress progress(this);
    Execute(progress);
  }

  virtual void SendProgress_(const T *data, size_t count) = 0;
};

template<class T>
/* abstract */
class AsyncProgressWorkerBase : public AsyncBareProgressWorker<T> {
 public:
  explicit AsyncProgressWorkerBase(
      Callback *callback_,
      const char* resource_name = "nan:AsyncProgressWorkerBase")
      : AsyncBareProgressWorker<T>(callback_, resource_name), asyncdata_(NULL),
        asyncsize_(0) {
    uv_mutex_init(&async_lock);
  }

  virtual ~AsyncProgressWorkerBase() {
    uv_mutex_destroy(&async_lock);

    delete[] asyncdata_;
  }

  void WorkProgress() {
    uv_mutex_lock(&async_lock);
    T *data = asyncdata_;
    size_t size = asyncsize_;
    asyncdata_ = NULL;
    uv_mutex_unlock(&async_lock);

    // Don't send progress events after we've already completed.
    if (this->callback) {
        this->HandleProgressCallback(data, size);
    }
    delete[] data;
  }

 private:
  void SendProgress_(const T *data, size_t count) {
    T *new_data = new T[count];
    {
      T *it = new_data;
      std::copy(data, data + count, it);
    }

    uv_mutex_lock(&async_lock);
    T *old_data = asyncdata_;
    asyncdata_ = new_data;
    asyncsize_ = count;
    uv_mutex_unlock(&async_lock);

    delete[] old_data;
    uv_async_send(&this->async);
  }

  uv_mutex_t async_lock;
  T *asyncdata_;
  size_t asyncsize_;
};

// This ensures compatibility to the previous un-templated AsyncProgressWorker
// class definition.
typedef AsyncProgressWorkerBase<char> AsyncProgressWorker;

template<class T>
/* abstract */
class AsyncBareProgressQueueWorker : public AsyncBareProgressWorkerBase {
 public:
  explicit AsyncBareProgressQueueWorker(
      Callback *callback_,
      const char* resource_name = "nan:AsyncBareProgressQueueWorker")
      : AsyncBareProgressWorkerBase(callback_, resource_name) {
  }

  virtual ~AsyncBareProgressQueueWorker() {
  }

  class ExecutionProgress {
    friend class AsyncBareProgressQueueWorker;
   public:
    void Send(const T* data, size_t count) const {
      that_->SendProgress_(data, count);
    }

   private:
    explicit ExecutionProgress(AsyncBareProgressQueueWorker *that)
        : that_(that) {}
    NAN_DISALLOW_ASSIGN_COPY_MOVE(ExecutionProgress)
    AsyncBareProgressQueueWorker* const that_;
  };

  virtual void Execute(const ExecutionProgress& progress) = 0;
  virtual void HandleProgressCallback(const T *data, size_t size) = 0;

 private:
  void Execute() /*final override*/ {
    ExecutionProgress progress(this);
    Execute(progress);
  }

  virtual void SendProgress_(const T *data, size_t count) = 0;
};

template<class T>
/* abstract */
class AsyncProgressQueueWorker : public AsyncBareProgressQueueWorker<T> {
 public:
  explicit AsyncProgressQueueWorker(
      Callback *callback_,
      const char* resource_name = "nan:AsyncProgressQueueWorker")
      : AsyncBareProgressQueueWorker<T>(callback_) {
    uv_mutex_init(&async_lock);
  }

  virtual ~AsyncProgressQueueWorker() {
    uv_mutex_lock(&async_lock);

    while (!asyncdata_.empty()) {
      std::pair<T*, size_t> &datapair = asyncdata_.front();
      T *data = datapair.first;

      asyncdata_.pop();

      delete[] data;
    }

    uv_mutex_unlock(&async_lock);
    uv_mutex_destroy(&async_lock);
  }

  void WorkComplete() {
    WorkProgress();
    AsyncWorker::WorkComplete();
  }

  void WorkProgress() {
    uv_mutex_lock(&async_lock);

    while (!asyncdata_.empty()) {
      std::pair<T*, size_t> &datapair = asyncdata_.front();

      T *data = datapair.first;
      size_t size = datapair.second;

      asyncdata_.pop();
      uv_mutex_unlock(&async_lock);

      // Don't send progress events after we've already completed.
      if (this->callback) {
          this->HandleProgressCallback(data, size);
      }

      delete[] data;

      uv_mutex_lock(&async_lock);
    }

    uv_mutex_unlock(&async_lock);
  }

 private:
  void SendProgress_(const T *data, size_t count) {
    T *new_data = new T[count];
    {
      T *it = new_data;
      std::copy(data, data + count, it);
    }

    uv_mutex_lock(&async_lock);
    asyncdata_.push(std::pair<T*, size_t>(new_data, count));
    uv_mutex_unlock(&async_lock);

    uv_async_send(&this->async);
  }

  uv_mutex_t async_lock;
  std::queue<std::pair<T*, size_t> > asyncdata_;
};

inline void AsyncExecute (uv_work_t* req) {
  AsyncWorker *worker = static_cast<AsyncWorker*>(req->data);
  worker->Execute();
}

inline void AsyncExecuteComplete (uv_work_t* req) {
  AsyncWorker* worker = static_cast<AsyncWorker*>(req->data);
  worker->WorkComplete();
  worker->Destroy();
}

inline void AsyncQueueWorker (AsyncWorker* worker) {
  uv_queue_work(
      uv_default_loop()
    , &worker->request
    , AsyncExecute
    , reinterpret_cast<uv_after_work_cb>(AsyncExecuteComplete)
  );
}

namespace imp {

inline
ExternalOneByteStringResource const*
GetExternalResource(v8::Local<v8::String> str) {
#if NODE_MODULE_VERSION < ATOM_0_21_MODULE_VERSION
    return str->GetExternalAsciiStringResource();
#else
    return str->GetExternalOneByteStringResource();
#endif
}

inline
bool
IsExternal(v8::Local<v8::String> str) {
#if NODE_MODULE_VERSION < ATOM_0_21_MODULE_VERSION
    return str->IsExternalAscii();
#else
    return str->IsExternalOneByte();
#endif
}

}  // end of namespace imp

enum Encoding {ASCII, UTF8, BASE64, UCS2, BINARY, HEX, BUFFER};

#if NODE_MODULE_VERSION < NODE_0_10_MODULE_VERSION
# include "nan_string_bytes.h"  // NOLINT(build/include)
#endif

inline v8::Local<v8::Value> Encode(
    const void *buf, size_t len, enum Encoding encoding = BINARY) {
#if (NODE_MODULE_VERSION >= ATOM_0_21_MODULE_VERSION)
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  node::encoding node_enc = static_cast<node::encoding>(encoding);

  if (encoding == UCS2) {
    return node::Encode(
        isolate
      , reinterpret_cast<const uint16_t *>(buf)
      , len / 2);
  } else {
    return node::Encode(
        isolate
      , reinterpret_cast<const char *>(buf)
      , len
      , node_enc);
  }
#elif (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
  return node::Encode(
      v8::Isolate::GetCurrent()
    , buf, len
    , static_cast<node::encoding>(encoding));
#else
# if NODE_MODULE_VERSION >= NODE_0_10_MODULE_VERSION
  return node::Encode(buf, len, static_cast<node::encoding>(encoding));
# else
  return imp::Encode(reinterpret_cast<const char*>(buf), len, encoding);
# endif
#endif
}

inline ssize_t DecodeBytes(
    v8::Local<v8::Value> val, enum Encoding encoding = BINARY) {
#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
  return node::DecodeBytes(
      v8::Isolate::GetCurrent()
    , val
    , static_cast<node::encoding>(encoding));
#else
# if (NODE_MODULE_VERSION < NODE_0_10_MODULE_VERSION)
  if (encoding == BUFFER) {
    return node::DecodeBytes(val, node::BINARY);
  }
# endif
  return node::DecodeBytes(val, static_cast<node::encoding>(encoding));
#endif
}

inline ssize_t DecodeWrite(
    char *buf
  , size_t len
  , v8::Local<v8::Value> val
  , enum Encoding encoding = BINARY) {
#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
  return node::DecodeWrite(
      v8::Isolate::GetCurrent()
    , buf
    , len
    , val
    , static_cast<node::encoding>(encoding));
#else
# if (NODE_MODULE_VERSION < NODE_0_10_MODULE_VERSION)
  if (encoding == BUFFER) {
    return node::DecodeWrite(buf, len, val, node::BINARY);
  }
# endif
  return node::DecodeWrite(
      buf
    , len
    , val
    , static_cast<node::encoding>(encoding));
#endif
}

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

#if (NODE_MODULE_VERSION >= NODE_6_0_MODULE_VERSION)
  return obj->SetAccessor(
      GetCurrentContext()
    , name
    , getter_
    , setter_
    , dataobj
    , settings
    , attribute).FromMaybe(false);
#else
  return obj->SetAccessor(
      name
    , getter_
    , setter_
    , dataobj
    , settings
    , attribute);
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

#if NODE_MODULE_VERSION > NODE_0_12_MODULE_VERSION
  tpl->SetHandler(v8::NamedPropertyHandlerConfiguration(
      getter_, setter_, query_, deleter_, enumerator_, obj));
#else
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

#if NODE_MODULE_VERSION > NODE_0_12_MODULE_VERSION
  tpl->SetHandler(v8::IndexedPropertyHandlerConfiguration(
      getter_, setter_, query_, deleter_, enumerator_, obj));
#else
  tpl->SetIndexedPropertyHandler(
      getter_
    , setter_
    , query_
    , deleter_
    , enumerator_
    , obj);
#endif
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



#define NAN_STRINGIZE2(x) #x
#define NAN_STRINGIZE(x) NAN_STRINGIZE2(x)
#define NAN_TEST_EXPRESSION(expression) \
  ( expression ), __FILE__ ":" NAN_STRINGIZE(__LINE__) ": " #expression


}  // end of namespace Nan

#endif  // NAN_H_
