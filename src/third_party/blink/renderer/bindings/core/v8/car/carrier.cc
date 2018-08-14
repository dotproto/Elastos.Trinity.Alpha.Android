#include <string>
#include "elastos.h"
#include "elapi.h"
#include "ElastosCore.h"
#include "carrier-listener.h"
#include "elastos/utility/etl/HashMap.h"
//#include <new>
//#include <sstream>
//#include <string>
//#include <vector>
#include "car-bridge.h"
#include "elastos-ext.h"
#include "can-down-delete.h"
#include "car-arguments.h"
#include "car-data-type.h"
#include "car-function.h"
#include "error.h"
#include "js-2-car.h"
#include "weak-external-base.h"
#include "logging.h"

using namespace Nan;
using namespace v8;

CAR_BRIDGE_NAMESPACE_USING

ICarrier*         gCarrier = NULL;
CCarrierListener* gListener = NULL;

static v8::Local<v8::Value> Throw(const char *message)
{
    return v8::Isolate::GetCurrent()->ThrowException(Nan::New(message).ToLocalChecked());
}

static void Start(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::String> _path;
    Elastos::String path;
    Debug_LOG("");
    ECode ec = (ECode) _CCarrier_GetInstance(&gCarrier);
    if (FAILED(ec)) {
        Debug_LOG("carrier start error [%x].", ec);
        return;
    }

    Debug_LOG("");
    gListener = new CCarrierListener(info.GetIsolate(), info.GetIsolate()->GetCurrentContext());
    if(NULL == gListener) {
        Debug_LOG("no memeory");
        return;
    }
    gCarrier->AddCarrierNodeListener(ICarrierListener::Probe(gListener));

    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }

    _path =v8::Local<v8::String>::Cast(info[0]);

    ToString(path, _path);
    gCarrier->Start(path);
    Debug_LOG("start at %s", path.string());
}

static void Stop(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    Debug_LOG("");
    gCarrier->Stop();
    gCarrier->Release();
    gCarrier = NULL;
}

static void AddFriend(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");

    v8::Local<v8::String> _friendAddress;
    Elastos::String friendAddress;
    Elastos::String friendId;

    Debug_LOG("");
    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }

    _friendAddress =v8::Local<v8::String>::Cast(info[0]);
    ToString(friendAddress, _friendAddress);

    gCarrier->AddFriend(friendAddress, Elastos::String("hello"), &friendId);
    
    info.GetReturnValue().Set(ToValue(friendId));
    Debug_LOG("add friend %s", friendId.string());
}

static void RemoveFriend(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");
    Debug_LOG("");

    v8::Local<v8::String> _uid;
    Elastos::String uid;

    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }
    _uid =v8::Local<v8::String>::Cast(info[0]);
    ToString(uid, _uid);

    IFriend* pFriend;
    ECode ec = gCarrier->GetFriend(uid, &pFriend);
    if (FAILED(ec)) {
        Debug_LOG("GetFriend faild[%x].", ec);
    }

    gCarrier->RemoveFriend(pFriend);
    pFriend->Release();
}

static void GetFriends(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");

    Local<Array> array = v8::Array::New(info.GetIsolate());
    ArrayOf<IFriend*>* friends = NULL;
 
    Debug_LOG("");
    ECode ec = gCarrier->GetFriends(&friends);
    if (FAILED(ec) || friends == NULL) {
        Debug_LOG("get frined list from carrier error [%x].", ec);
        info.GetReturnValue().Set(array);
        return;
    }

    Local<v8::Object> element;
    Debug_LOG("have %d friends.", friends->GetLength());
    for (int i = 0; i < friends->GetLength(); i++) {
        Elastos::String  uid;
        Elastos::String  name;
        Elastos::String  label;
        Elastos::Boolean online;
 
        IFriend* iFriend = (*friends)[i];
        iFriend->GetUid(&uid);
        iFriend->GetName(&name);
        iFriend->GetLabel(&label);
        iFriend->IsOnline(&online);

        DefineOwnProperty(element,
                          New("uid").ToLocalChecked(),
                          New(uid).ToLocalChecked(),
                         static_cast<enum PropertyAttribute>(DontDelete | DontEnum));
        DefineOwnProperty(element,
                          New("online").ToLocalChecked(),
                          New(online),
                         static_cast<enum PropertyAttribute>(DontDelete | DontEnum));
        if(label.GetLength() > 0) {
            DefineOwnProperty(element,
                              New("label").ToLocalChecked(),
                              New(label).ToLocalChecked(),
                              static_cast<enum PropertyAttribute>(DontDelete | DontEnum));
        }
        if(name.GetLength() > 0) {
            DefineOwnProperty(element,
                              New("name").ToLocalChecked(),
                              New(name).ToLocalChecked(),
                              static_cast<enum PropertyAttribute>(DontDelete | DontEnum));
        }

        Debug_LOG("friend: %s, label:%s", uid.string(), label.string());

        array->Set(i, element);
    }
 
    friends->Release();
    info.GetReturnValue().Set(array);
    Debug_LOG("");
}

static void AccpetFriendRequest(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");
    Debug_LOG("");
    v8::Local<v8::String> _uid;
    Elastos::String uid;

    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }
    _uid = v8::Local<v8::String>::Cast(info[0]);
    ToString(uid, _uid);

    Debug_LOG("accept friend %s", uid.string());
    gCarrier->AccpetFriendRequest(uid);
    Debug_LOG("");
}

static void GetAddress(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");
    Debug_LOG("");
    Elastos::String address;
    gCarrier->GetAddress(&address);

    info.GetReturnValue().Set(ToValue(address));
    Debug_LOG("address:%s", address.string());
}

static void GetUserid(const v8::FunctionCallbackInfo<v8::Value> &info)
{
     if (gCarrier == NULL) Throw("carrier uninitialized");
     Debug_LOG("");

    Elastos::String uid;
    gCarrier->GetUserid(&uid);

    info.GetReturnValue().Set(ToValue(uid));
    Debug_LOG("user id:%s", uid.string());
}

static void SendMessage(const v8::FunctionCallbackInfo<v8::Value> &info)
 {
    if (gCarrier == NULL) Throw("carrier uninitialized");

    Debug_LOG("");
    v8::Local<v8::String> _uid;
    Elastos::String uid;

    v8::Local<v8::String> _msg;
    Elastos::String msg;
    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
        return;
    }
    if (!info[1]->IsString()) {
        Debug_LOG("arg1 not String");
        return;
    }

    _uid = v8::Local<v8::String>::Cast(info[0]);
    ToString(uid, _uid);

    _msg = v8::Local<v8::String>::Cast(info[1]);
    ToString(msg, _msg);

    Debug_LOG("SendMessage %s to %s", msg.string(), uid.string());

    gCarrier->SendMessage(uid, *msg.GetBytes().Get());
    Debug_LOG("");
}

static void SetFriendLabel(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");

    Debug_LOG("");
    v8::Local<v8::String> _uid;
    Elastos::String uid;

    v8::Local<v8::String> _label;
    Elastos::String label;
    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
        return;
    }
    if (!info[1]->IsString()) {
        Debug_LOG("arg1 not String");
        return;
    }

    _uid = v8::Local<v8::String>::Cast(info[0]);
    ToString(uid, _uid);

    _label = v8::Local<v8::String>::Cast(info[1]);
    ToString(label, _label);

    Debug_LOG("SetFriendLabel %s to %s", label.string(), uid.string());

    IFriend* pFriend;
    ECode ec = gCarrier->GetFriend(uid, &pFriend);
    if (FAILED(ec)) {
        Debug_LOG("GetFriend faild[%x].", ec);
    }

    pFriend->SetLabel(label);
    pFriend->Release();
    Debug_LOG("");
}


static void New(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    Debug_LOG("new Carrier, call from js.");
}

static void recevMessage(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    char* event = (char*)malloc(MAX_EVENT_LEN+1);
    memset(event, 0, MAX_EVENT_LEN);
    unsigned len = gListener->GetEvent(&event);
    if(len > 0) {
        Debug_LOG("%s", event);
        info.GetReturnValue().Set(Nan::New(event).ToLocalChecked());
    }

    free(event);
}

bool ela::Carrier::initialize(v8::Isolate* isolate, v8::Local<v8::Context> context)
 {
     Debug_LOG("");
 
     Nan::EscapableHandleScope scope;
     v8::Local<FunctionTemplate> classTemplate;
     classTemplate = FunctionTemplate::New(isolate, New);
 
     classTemplate->SetClassName(Nan::New("Carrier").ToLocalChecked());

     classTemplate->PrototypeTemplate()->Set(
         Nan::New("Start").ToLocalChecked(), FunctionTemplate::New(isolate, Start));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("Stop").ToLocalChecked(), FunctionTemplate::New(isolate, Stop));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("AddFriend").ToLocalChecked(), FunctionTemplate::New(isolate, AddFriend));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("RemoveFriend").ToLocalChecked(), FunctionTemplate::New(isolate, RemoveFriend));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("GetFriends").ToLocalChecked(), FunctionTemplate::New(isolate, GetFriends));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("AccpetFriendRequest").ToLocalChecked(), FunctionTemplate::New(isolate, AccpetFriendRequest));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("GetAddress").ToLocalChecked(), FunctionTemplate::New(isolate, GetAddress));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("GetUserid").ToLocalChecked(), FunctionTemplate::New(isolate, GetUserid));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("SendMessage").ToLocalChecked(), FunctionTemplate::New(isolate, SendMessage));
     classTemplate->PrototypeTemplate()->Set(
         Nan::New("recevMessage").ToLocalChecked(), FunctionTemplate::New(isolate, recevMessage));
    classTemplate->PrototypeTemplate()->Set(
         Nan::New("SetFriendLabel").ToLocalChecked(), FunctionTemplate::New(isolate, SetFriendLabel));

     context->Global()->Set(
         Nan::New("Carrier").ToLocalChecked(), classTemplate->GetFunction());

     Debug_LOG("");
     return 1;
 }
