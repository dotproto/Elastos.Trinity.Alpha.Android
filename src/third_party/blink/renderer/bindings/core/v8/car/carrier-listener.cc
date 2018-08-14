#include "carrier-listener.h"
#include <Elastos.CoreLibrary.h>
#include "logging.h"
#include <nan.h>
#include <string.h>
#include <stdio.h>

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
#if 0
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
#endif
    base::AutoLock lock(msg_buf_lock_);
    memset(event_, MAX_EVENT_LEN, 0);
    sprintf(event_, "{\"callback\":\"OnConnectionChanged\", \"para1\":\"%d\" }", online);
    Debug_LOG("==== OnConnectionChanged end");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnReady()
{
    Debug_LOG("==== OnReady");
    memset(event_, MAX_EVENT_LEN, 0);
    sprintf(event_, "{\"callback\":\"OnReady\" }");
    Debug_LOG("==== OnReady end");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnFriendRequest(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ const _ELASTOS String& hello)
{
    Debug_LOG("==== OnFriendRequest: %s", uid.string());

    base::AutoLock lock(msg_buf_lock_);
    memset(event_, MAX_EVENT_LEN, 0);
    sprintf(event_, "{\"callback\":\"OnFriendRequest\", \"para1\":\"%s\", \"para2\":\"%s\" }", uid.string(), hello.string());

    Debug_LOG("==== OnFriendRequest end");
    return NOERROR;
}

_ELASTOS ECode CCarrierListener::OnFriendConnetionChanged(
    /* [in] */ const _ELASTOS String& uid,
    /* [in] */ _ELASTOS Boolean online)
{
    Debug_LOG("==== OnFriendConnetionChanged: %s", uid.string());
    memset(event_, MAX_EVENT_LEN, 0);
    sprintf(event_, "{\"callback\":\"OnFriendConnetionChanged\", \"para1\":\"%s\", \"para2\":\"%d\" }", uid.string(), online);
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
    memset(event_, MAX_EVENT_LEN, 0);
    Elastos::String msg(message);

    sprintf(event_, "{\"callback\":\"OnMessageReceived\", \"para1\":\"%s\", \"para2\":\"%s\" }", uid.string(), msg.string());
    Debug_LOG("==== OnMessageReceived end");
    return NOERROR;
}

unsigned CCarrierListener::GetEvent(char** event)
{
    base::AutoLock lock(msg_buf_lock_);
    if(NULL == event) {
        Debug_LOG("invalid arguments.");
        return 0;
    }

    unsigned len = strlen(event_);
    memcpy(*event, event_, len);
    memset(event_, 0, MAX_EVENT_LEN);
    return len;
}