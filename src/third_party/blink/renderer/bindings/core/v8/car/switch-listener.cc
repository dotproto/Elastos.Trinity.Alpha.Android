
#include "switch-listener.h"

CAR_INTERFACE_IMPL(CSwitchListener, Object, ISwitchListener)

_ELASTOS ECode CSwitchListener::OnSwitch1Changed(
        /* [in] */ _ELASTOS Boolean on)
{
    v8::Local<v8::Context> context = mIsolate->GetCurrentContext();

    v8::Local<Function> func = v8::Local<Function>::Cast(context->Global()->Get(Nan::New("switch1").ToLocalChecked()));
    v8::Local<Value> argv[1] = {Nan::New(on)};
    func->Call(context->Global(), 1, argv);

    return NOERROR;
}

_ELASTOS ECode CSwitchListener::OnSwitch2Changed(
        /* [in] */ _ELASTOS Boolean on)
{
    v8::Local<v8::Context> context = mIsolate->GetCurrentContext();

    v8::Local<Function> func = v8::Local<Function>::Cast(context->Global()->Get(Nan::New("switch2").ToLocalChecked()));
    v8::Local<Value> argv[1] = {Nan::New(on)};
    func->Call(context->Global(), 1, argv);

    return NOERROR;
}

