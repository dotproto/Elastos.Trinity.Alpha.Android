/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2018 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#ifndef NAN_CONVERTERS_H_
#define NAN_CONVERTERS_H_

namespace imp {
template<typename T> struct ToFactoryBase {
  typedef MaybeLocal<T> return_t;
};
template<typename T> struct ValueFactoryBase { typedef Maybe<T> return_t; };

template<typename T> struct ToFactory;

template<>
struct ToFactory<v8::Function> : ToFactoryBase<v8::Function> {
  static inline return_t convert(v8::Local<v8::Value> val) {
    if (val.IsEmpty() || !val->IsFunction()) return MaybeLocal<v8::Function>();
    return MaybeLocal<v8::Function>(val.As<v8::Function>());
  }
};

#define X(TYPE)                                                                \
    template<>                                                                 \
    struct ToFactory<v8::TYPE> : ToFactoryBase<v8::TYPE> {                     \
      static inline return_t convert(v8::Local<v8::Value> val);                \
    };

X(Boolean)
X(Number)
X(String)
X(Object)
X(Integer)
X(Uint32)
X(Int32)

#undef X

#define X(TYPE)                                                                \
    template<>                                                                 \
    struct ToFactory<TYPE> : ValueFactoryBase<TYPE> {                          \
      static inline return_t convert(v8::Local<v8::Value> val);                \
    };

X(bool)
X(double)
X(int64_t)
X(uint32_t)
X(int32_t)

#undef X
}  // end of namespace imp

template<typename T>
inline
typename imp::ToFactory<T>::return_t To(v8::Local<v8::Value> val) {
  return imp::ToFactory<T>::convert(val);
}

# include "nan_converters_43_inl.h"

#endif  // NAN_CONVERTERS_H_
