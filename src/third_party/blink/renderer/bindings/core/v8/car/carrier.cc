#include <string>
#include "elastos.h"
#include "elapi.h"
#include "ElastosCore.h"
#include "carrier-listener.h"
//#include "switch-listener.h"
#include "elastos/utility/etl/HashMap.h"
//#include "Switch.h"

#include <climits>
#include <cstdlib>
#include <cmath>
#include <new>
#include <sstream>
#include <string>
#include <vector>
#include "car-bridge.h"
#include "libc++-ext.h"
#include "elastos-ext.h"
#include "car-object.h"
#include "can-down-delete.h"
#include "car-arguments.h"
#include "car-data-type.h"
#include "car-function.h"
#include "error.h"
#include "js-2-car.h"
#include "util.h"
#include "weak-external-base.h"
#include "logging.h"

using namespace Nan;
using namespace v8;

CAR_BRIDGE_NAMESPACE_USING

ICarrier* gCarrier = NULL;
IServiceManager* gServiceManager = NULL;

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
    if (FAILED(ec)) return;

    CCarrierListener* listener = new CCarrierListener(info.GetIsolate(), info.GetIsolate()->GetCurrentContext());
    gCarrier->AddCarrierNodeListener(ICarrierListener::Probe(listener));

    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }

    _path =v8::Local<v8::String>::Cast(info[0]);

    ToString(path, _path);
    gCarrier->Start(path);
}

static void Stop(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    Debug_LOG("");
    gCarrier->Stop();
    gCarrier->Release();
    gCarrier = NULL;
}

static void addFriend(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");

    v8::Local<v8::String> _friendAddress;
    Elastos::String friendAddress;
    Elastos::String friendId;

    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }

    _friendAddress =v8::Local<v8::String>::Cast(info[0]);
    ToString(friendAddress, _friendAddress);

    gCarrier->AddFriend(friendAddress, Elastos::String("hello"), &friendId);
    
    info.GetReturnValue().Set(ToValue(friendId));
}

static void deleteFriend(const v8::FunctionCallbackInfo<v8::Value> &info)
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
        Debug_LOG("GetFriend faild");
    }

    gCarrier->RemoveFriend(pFriend);
    pFriend->Release();
}

static void getFriendList(const v8::FunctionCallbackInfo<v8::Value> &info)
{
#if 0
    if (gCarrier == NULL) Throw("carrier uninitialized");

    ArrayOf<IFriend*>* friends = NULL;
    ECode ec = gCarrier->GetFriends(&friends);
    if (FAILED(ec) || friends == NULL) return NULL;

    jclass cls_ArrayList = env->FindClass("java/util/ArrayList");
    jmethodID construct = env->GetMethodID(cls_ArrayList, "<init>", "()V");
    jobject obj_ArrayList = env->NewObject(cls_ArrayList, construct,"");
    jmethodID arrayList_add = env->GetMethodID(cls_ArrayList, "add", "(Ljava/lang/Object;)Z");

    jclass cls_friend = env->FindClass("Friend");
    //none argument construct function
    jmethodID construct_user = env->GetMethodID(cls_friend, "<init>", "(Ljava/lang/String;Z)V");

    for (int i = 0; i < friends->GetLength(); i++) {
        IFriend* iFriend = (*friends)[i];

        String uid;
        iFriend->GetUid(&uid);
        Boolean online;
        iFriend->IsOnline(&online);
        jobject obj_friend = env->NewObject(cls_friend, construct_user, env->NewStringUTF(uid.string()), online);
        env->CallObjectMethod(obj_ArrayList, arrayList_add, obj_friend);
    }

    friends->Release();

#endif
}

static void acceptFriend(const v8::FunctionCallbackInfo<v8::Value> &info)
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
}

static void getAddress(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");
    Debug_LOG("");
    Elastos::String address;
    gCarrier->GetAddress(&address);

    info.GetReturnValue().Set(ToValue(address));
}

static void startServiceManager(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gCarrier == NULL) Throw("carrier uninitialized");
    if (gServiceManager != NULL) {
        Debug_LOG("service manager already started");
    }
    Debug_LOG("");
    _CServiceManager_AcquireInstance(&gServiceManager);
}

static void stopServiceManager(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    Debug_LOG("");
    if (gServiceManager) {
        gServiceManager->Release();
        gServiceManager = NULL;
    }
}

static void addService(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    if (gServiceManager == NULL) {
        Throw("service manager not init.");
    }
#if 0
    ISwitch* pSwitch;
    ECode ec = CSwitch::New(&pSwitch);
    if (FAILED(ec)) return ec;

    v8::Local<v8::String> _name;
    Elastos::String name;

    if (!info[0]->IsString()) {
        Debug_LOG("arg0 not String");
    }
    _name = v8::Local<v8::String>::Cast(info[0]);
    ToString(name, _name);

    CSwitchListener* listener = new CSwitchListener(info.GetIsolate());
    pSwitch->SetSwitchListener(ISwitchListener::Probe(listener));

    Debug_LOG("add service %s", name.string());
    ec = gServiceManager->AddService(name, pSwitch);

    pSwitch->Release();
#endif
}

static void New(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    Debug_LOG("new Carrier, call from js.");
}

bool ela::Carrier::initialize(v8::Isolate* isolate)
{
    Debug_LOG("");

    Nan::EscapableHandleScope scope;
    v8::Local<FunctionTemplate> classTemplate;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    classTemplate = FunctionTemplate::New(isolate, New);

    classTemplate->SetClassName(Nan::New("Carrier").ToLocalChecked());

    classTemplate->PrototypeTemplate()->Set(
        Nan::New("Start").ToLocalChecked(), FunctionTemplate::New(isolate, Start));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("Stop").ToLocalChecked(), FunctionTemplate::New(isolate, Stop));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("addFriend").ToLocalChecked(), FunctionTemplate::New(isolate, addFriend));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("deleteFriend").ToLocalChecked(), FunctionTemplate::New(isolate, deleteFriend));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("getFriendList").ToLocalChecked(), FunctionTemplate::New(isolate, getFriendList));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("acceptFriend").ToLocalChecked(), FunctionTemplate::New(isolate, acceptFriend));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("getAddress").ToLocalChecked(), FunctionTemplate::New(isolate, getAddress));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("startServiceManager").ToLocalChecked(), FunctionTemplate::New(isolate, startServiceManager));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("stopServiceManager").ToLocalChecked(), FunctionTemplate::New(isolate, stopServiceManager));
    classTemplate->PrototypeTemplate()->Set(
        Nan::New("addService").ToLocalChecked(), FunctionTemplate::New(isolate, addService));

    context->Global()->Set(
        Nan::New("Carrier").ToLocalChecked(), classTemplate->GetFunction());

    return 1;
}