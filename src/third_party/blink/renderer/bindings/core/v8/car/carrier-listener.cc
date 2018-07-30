#include "carrier-listener.h"
#include <Elastos.CoreLibrary.h>
#include "logging.h"
#include <nan.h>

using namespace v8;

CAR_INTERFACE_IMPL(CCarrierListener, Object, ICarrierListener)

_ELASTOS ECode CCarrierListener::OnIdle()
{
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnConnectionChanged(
    /* [in] */ _ELASTOS Boolean online)
{
    Debug_LOG("==== OnConnectionChanged: %d", online);
    // v8::Locker lock(isolate_);
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Value> value = context->Global()->Get(Nan::New("OnConnectionChanged").ToLocalChecked());
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);

    v8::Local<Value> argv[1] = {Nan::New(online)};
    func->Call(context->Global(), 1, argv);
    // isolate_->Dispose();
    Debug_LOG("==== OnConnectionChanged end");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnReady()
{
    Debug_LOG("==== OnReady");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnFriendRequest(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ const _ELASTOS String& hello)
{
    Debug_LOG("==== OnFriendRequest: %s", uid.string());

    v8::Local<v8::Context> context = v8::Isolate::GetCurrent()->GetCurrentContext();

    v8::Local<Function> func = v8::Local<Function>::Cast(context->Global()->Get(Nan::New("OnFriendRequest").ToLocalChecked()));
    v8::Local<Value> argv[2] = {Nan::New(uid.string()).ToLocalChecked(), Nan::New(hello.string()).ToLocalChecked()};
    func->Call(context->Global(), 1, argv);

    Debug_LOG("==== OnFriendRequest end");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnFriendConnetionChanged(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ _ELASTOS Boolean online)
{
    Debug_LOG("==== OnFriendConnetionChanged: %s", uid.string());
    v8::Local<v8::Context> context = isolate_->GetCurrentContext();

    v8::Local<Function> func = v8::Local<Function>::Cast(context->Global()->Get(Nan::New("OnFriendConnetionChanged").ToLocalChecked()));
    v8::Local<Value> argv[2] = {Nan::New(uid.string()).ToLocalChecked(), Nan::New(online)};
    func->Call(context->Global(), 1, argv);

    Debug_LOG("==== OnFriendConnetionChanged end");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnPortForwardingRequest(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ const _ELASTOS String& servicePort,
    /* [out] */ _ELASTOS Boolean * accept)
{
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnPortForwardingResult(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ const _ELASTOS String& localPort,
    /* [in] */ const _ELASTOS String& remotePort,
    /* [in] */ _ELASTOS ECode code)
{
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnMessageReceived(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & message)
{
    return NOERROR;
}

