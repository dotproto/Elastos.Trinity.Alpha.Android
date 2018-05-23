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


# define NAN_HAS_CPLUSPLUS_11 (__cplusplus >= 201103L)

#include <algorithm>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <utility>

# include <queue>
# include <string>
# include <vector>

#  include <pthread.h>

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

  # define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&) = delete;
  # define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&) = delete;
  # define NAN_DISALLOW_MOVE(CLASS)                                              \
      CLASS(CLASS&&) = delete;  /* NOLINT(build/c++11) */                        \
      void operator=(CLASS&&) = delete;


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

}  // end of namespace Nan

#endif  // NAN_H_
