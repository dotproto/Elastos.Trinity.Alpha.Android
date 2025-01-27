/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2018 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#ifndef NAN_WEAK_H_
#define NAN_WEAK_H_

static const int kInternalFieldsInWeakCallback = 2;
static const int kNoInternalFieldIndex = -1;

#define NAN_WEAK_PARAMETER_CALLBACK_DATA_TYPE_ \
    v8::WeakCallbackInfo<WeakCallbackInfo<T> > const&
#define NAN_WEAK_TWOFIELD_CALLBACK_DATA_TYPE_ \
    NAN_WEAK_PARAMETER_CALLBACK_DATA_TYPE_
#define NAN_WEAK_PARAMETER_CALLBACK_SIG_ NAN_WEAK_PARAMETER_CALLBACK_DATA_TYPE_
#define NAN_WEAK_TWOFIELD_CALLBACK_SIG_ NAN_WEAK_TWOFIELD_CALLBACK_DATA_TYPE_

template<typename T>
class WeakCallbackInfo {
 public:
  typedef void (*Callback)(const WeakCallbackInfo<T>& data);
  WeakCallbackInfo(
      Persistent<v8::Value> *persistent
    , Callback callback
    , void *parameter
    , void *field1 = 0
    , void *field2 = 0) :
        callback_(callback), isolate_(0), parameter_(parameter) {
    std::memcpy(&persistent_, persistent, sizeof (v8::Persistent<v8::Value>));
    internal_fields_[0] = field1;
    internal_fields_[1] = field2;
  }
  inline v8::Isolate *GetIsolate() const { return isolate_; }
  inline T *GetParameter() const { return static_cast<T*>(parameter_); }
  inline void *GetInternalField(int index) const {
    assert((index == 0 || index == 1) && "internal field index out of bounds");
    if (index == 0) {
      return internal_fields_[0];
    } else {
      return internal_fields_[1];
    }
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(WeakCallbackInfo)
  Callback callback_;
  v8::Isolate *isolate_;
  void *parameter_;
  void *internal_fields_[kInternalFieldsInWeakCallback];
  v8::Persistent<v8::Value> persistent_;
  template<typename S, typename M> friend class Persistent;
  template<typename S> friend class PersistentBase;

  template<bool isFirstPass>
  static void invokeparameter(NAN_WEAK_PARAMETER_CALLBACK_SIG_ data);
  template<bool isFirstPass>
  static void invoketwofield(NAN_WEAK_TWOFIELD_CALLBACK_SIG_ data);
  static WeakCallbackInfo *unwrapparameter(
	  NAN_WEAK_PARAMETER_CALLBACK_DATA_TYPE_ data);
  static WeakCallbackInfo *unwraptwofield(
	  NAN_WEAK_TWOFIELD_CALLBACK_DATA_TYPE_ data);

};


template<typename T>
template<bool isFirstPass>
void
WeakCallbackInfo<T>::invokeparameter(NAN_WEAK_PARAMETER_CALLBACK_SIG_ data) {
  WeakCallbackInfo<T> *cbinfo = unwrapparameter(data);
  if (isFirstPass) {
    cbinfo->persistent_.Reset();
    data.SetSecondPassCallback(invokeparameter<false>);
  } else {
    cbinfo->callback_(*cbinfo);
    delete cbinfo;
  }
}

template<typename T>
template<bool isFirstPass>
void
WeakCallbackInfo<T>::invoketwofield(NAN_WEAK_TWOFIELD_CALLBACK_SIG_ data) {
  WeakCallbackInfo<T> *cbinfo = unwraptwofield(data);
  if (isFirstPass) {
    cbinfo->persistent_.Reset();
    data.SetSecondPassCallback(invoketwofield<false>);
  } else {
    cbinfo->callback_(*cbinfo);
    delete cbinfo;
  }
}

template<typename T>
WeakCallbackInfo<T> *WeakCallbackInfo<T>::unwrapparameter(
    NAN_WEAK_PARAMETER_CALLBACK_DATA_TYPE_ data) {
  WeakCallbackInfo<T> *cbinfo =
      static_cast<WeakCallbackInfo<T>*>(data.GetParameter());
  cbinfo->isolate_ = data.GetIsolate();
  return cbinfo;
}

template<typename T>
WeakCallbackInfo<T> *WeakCallbackInfo<T>::unwraptwofield(
    NAN_WEAK_TWOFIELD_CALLBACK_DATA_TYPE_ data) {
  WeakCallbackInfo<T> *cbinfo =
      static_cast<WeakCallbackInfo<T>*>(data.GetInternalField(0));
  cbinfo->isolate_ = data.GetIsolate();
  return cbinfo;
}

#undef NAN_WEAK_PARAMETER_CALLBACK_SIG_
#undef NAN_WEAK_TWOFIELD_CALLBACK_SIG_
#undef NAN_WEAK_PARAMETER_CALLBACK_DATA_TYPE_
#undef NAN_WEAK_TWOFIELD_CALLBACK_DATA_TYPE_


template<typename T, typename M>
template<typename P>
inline void Persistent<T, M>::SetWeak(
    P *parameter
  , typename WeakCallbackInfo<P>::Callback callback
  , WeakCallbackType type) {
  WeakCallbackInfo<P> *wcbd;
  if (type == WeakCallbackType::kParameter) {
    wcbd = new WeakCallbackInfo<P>(
        reinterpret_cast<Persistent<v8::Value>*>(this)
      , callback
      , parameter);
    v8::PersistentBase<T>::SetWeak(
        wcbd
      , WeakCallbackInfo<P>::template invokeparameter<true>
      , type);
  } else {
#if 0 //?jw
    v8::Local<T>* self = reinterpret_cast<v8::Local<T>*>(this);
    assert((*self)->IsObject());

    int count = (*self)->InternalFieldCount();
#else
    int count = 0;
#endif
    void *internal_fields[kInternalFieldsInWeakCallback] = {0, 0};
    for (int i = 0; i < count && i < kInternalFieldsInWeakCallback; i++) {
#if 0 //?jw
      internal_fields[i] = (*self)->GetAlignedPointerFromInternalField(i);
#endif
    }
    wcbd = new WeakCallbackInfo<P>(
        reinterpret_cast<Persistent<v8::Value>*>(this)
      , callback
      , 0
      , internal_fields[0]
      , internal_fields[1]);
#if 0//?jw
    (*self)->SetAlignedPointerInInternalField(0, wcbd);
#endif
    v8::PersistentBase<T>::SetWeak(
        static_cast<WeakCallbackInfo<P>*>(0)
      , WeakCallbackInfo<P>::template invoketwofield<true>
      , type);
  }
}


#endif  // NAN_WEAK_H_
