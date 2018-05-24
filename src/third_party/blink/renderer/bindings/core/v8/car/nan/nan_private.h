/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2018 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#ifndef NAN_PRIVATE_H_
#define NAN_PRIVATE_H_

inline Maybe<bool>
HasPrivate(v8::Local<v8::Object> object, v8::Local<v8::String> key) {
  HandleScope scope;
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Private> private_key = v8::Private::ForApi(isolate, key);
  return object->HasPrivate(context, private_key);
}

inline MaybeLocal<v8::Value>
GetPrivate(v8::Local<v8::Object> object, v8::Local<v8::String> key) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Private> private_key = v8::Private::ForApi(isolate, key);
  v8::MaybeLocal<v8::Value> v = object->GetPrivate(context, private_key);
  return scope.Escape(v.ToLocalChecked());
}

inline Maybe<bool> SetPrivate(
    v8::Local<v8::Object> object,
    v8::Local<v8::String> key,
    v8::Local<v8::Value> value) {
  HandleScope scope;
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Private> private_key = v8::Private::ForApi(isolate, key);
  return object->SetPrivate(context, private_key, value);
}

inline Maybe<bool> DeletePrivate(
    v8::Local<v8::Object> object,
    v8::Local<v8::String> key) {
  HandleScope scope;
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Private> private_key = v8::Private::ForApi(isolate, key);
  return object->DeletePrivate(isolate->GetCurrentContext(), private_key);
}

#endif  // NAN_PRIVATE_H_

