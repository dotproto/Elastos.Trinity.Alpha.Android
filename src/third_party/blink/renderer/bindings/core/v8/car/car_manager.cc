//#include "ElastosCore.h"

#include "car_manager.h"
#include "args_converter.h"
#include "base/logging.h"

#include <stdlib.h>
#include <string.h>

#include <map>
#include <string>

using std::map;
using std::pair;
using std::string;

using v8::Context;
using v8::External;
using v8::FunctionTemplate;
using v8::Global;
using v8::HandleScope;
using v8::Local;
using v8::Object;
using v8::ObjectTemplate;
using v8::Script;
using v8::String;
using v8::NewStringType;
using v8::TryCatch;
using v8::Value;
using v8::Persistent;
using v8::internal::GlobalHandles;

static Local<Value> Throw(v8::Isolate *isolate, const char *message)
{
    return isolate->ThrowException(
               v8::String::NewFromUtf8(isolate, message, NewStringType::kNormal)
               .ToLocalChecked());
}

static void MethodInvoke(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();
    HandleScope handle_scope(isolate);

    IMethodInfo *pMethodInfo
        = reinterpret_cast<IMethodInfo*>(args.Data().As<v8::External>()->Value());

    if (args.Holder()->InternalFieldCount() != 1) {
        Throw(isolate, "this is not a cobject");
        return;
    }

    // 获取对象
    Local<External> pObject = Local<External>::Cast(args.Holder()->GetInternalField(0));
    if (!pObject->IsExternal()) {
        Throw(isolate, "Worker is defunct because main thread is terminating");
        return;
    }
    IInterface* thisObj = static_cast<IInterface *>(pObject->Value());
    // for debug
    Elastos::String methodName;
    pMethodInfo->GetName(&methodName);
    LOG(INFO) << "MethodInvoke: " << methodName << " " << thisObj;

    // 将js参数类型转换为对应car类型
    AutoPtr<IArgumentList> pArgList;
    ECode ec = pMethodInfo->CreateArgumentList((IArgumentList**)&pArgList);
    if (FAILED(ec)) {
        Throw(isolate, "isolate, CreateArgumentList failed");
        return;
    }

    Elastos::String output;
    ec = pArgList->SetOutputArgumentOfStringPtr(0, &output);
    if (FAILED(ec)) {
        LOG(INFO) << "SetOutputArgumentOfStringPtr failed.";
        return;
    }
    ArgsConverter argsConverter(args);
    ec = argsConverter.Normalize(pMethodInfo, pArgList);

    // 执行方法
    pMethodInfo->Invoke(thisObj, pArgList);

    // 设置返回值
    args.GetReturnValue().Set(
        v8::String::NewFromUtf8(isolate, output.string(),
                      NewStringType::kNormal,
                      output.GetByteLength()).ToLocalChecked());
}

static void Destructor(const v8::WeakCallbackInfo<v8::Persistent<v8::Object> >& data)
{
    v8::Persistent<v8::Object>* handle = data.GetParameter();
    handle->Reset();
    LOG(INFO) << "Destructor: handle [" << handle << "]";
    IInterface* object = reinterpret_cast<IInterface*>(data.GetInternalField(0));
    LOG(INFO) << "Destructor: object [" << object << "]";
    object->Release();
}

// 创建对应的Car对象，并加入到js对象的扩展空间
static void Constructor(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    HandleScope handle_scope(isolate);

    // 1. Get Car class info.
    IClassInfo* pClassInfo = reinterpret_cast<IClassInfo*>(args.Data().As<v8::External>()->Value());
    if (!args.IsConstructCall()) {
        Throw(isolate, "class must be constructed with new");
        return;
    }

    // 2. Get the param of constructor.
    //if (args.Length() < 1 || !args[0]->IsNumber()) {
    //    Throw(isolate, "new class invalid argument");
    //    return;
    //}

    //String::Utf8Value str(isolate, args[0]);
    //int count = args[0]->Int32Value(isolate->GetCurrentContext()).FromJust();;

    // 3. Create a c++ object and set the c++ object as the beginning of the hidden
    // field of the js object, which is equivalent to this pointer.
    IInterface* pObject;
    ECode ec = pClassInfo->CreateObject((IInterface**)&pObject);
    if (FAILED(ec)) {
        LOG(INFO) << "CreateObject failed";
        return;
    }

    // 4. Make object handle weak so we can delete the data format once GC kicks in.
    v8::Persistent<v8::Object> handle;
    {
        v8::HandleScope scope(isolate);
        v8::Local<v8::ObjectTemplate> object_template = v8::ObjectTemplate::New(isolate);
        object_template->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = object_template->NewInstance(context).ToLocalChecked();

        handle.Reset(isolate, obj);
        obj->SetAlignedPointerInInternalField(0, pObject);
    }
    handle.SetWeak(&handle, Destructor, v8::WeakCallbackType::kInternalFields);

    // == for debug ==
    Elastos::String className;
    pClassInfo->GetName(&className);
    LOG(INFO) << "Constructor: " << className << " [" << pClassInfo << "]" << " this: [" << pObject << "]";
    args.Holder()->SetInternalField(0, External::New(isolate, pObject));
}

// ------------------------- initialize ---------------------------------------------
static void Require(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    int len = args.Length();
    if (len < 1) {
        Throw(isolate, "Require: car name Required");
        return;
    }
    if (!args[0]->IsString()) {
      Throw(isolate, "Require: type error, car name must be a string");
      return;
    }

    v8::String::Utf8Value carName(isolate, args[0]);
    LOG(INFO) << "Require : " << *carName;

    AutoPtr<IModuleInfo> pMdlInfo;
    ECode ec = CReflector::AcquireModuleInfo(Elastos::String(*carName), (IModuleInfo**)&pMdlInfo);
    if (FAILED(ec)) {
        LOG(INFO) << "Require: AcquireModuleInfo failed";
        return;
    }

    Int32 nClassCnt;
    ec = pMdlInfo->GetClassCount(&nClassCnt);
    if (FAILED(ec)) {
        LOG(INFO) << "Require: GetClassCount failed";
        return;
    }
    ArrayOf<IClassInfo*>* pClassInfoArray = ArrayOf<IClassInfo*>::Alloc(nClassCnt);
    pMdlInfo->GetAllClassInfos(pClassInfoArray);
    if (FAILED(ec)) {
        LOG(INFO) << "Require: GetAllClassInfos failed";
        return;
    }

    // 注册所有的类
    //LOG(INFO) << "nClassCnt:" << nClassCnt;
    for(int i = 0 ; i < nClassCnt ; i++)  {
        IClassInfo* pClassInfo = (*pClassInfoArray)[i];
        Local<FunctionTemplate> car_class_template = 
            FunctionTemplate::New(isolate, Constructor, v8::External::New(isolate, (void *)pClassInfo));
        Local<v8::Signature> car_class_signature = v8::Signature::New(isolate, car_class_template);

        Elastos::String className;
        pClassInfo->GetName(&className);
        LOG(INFO) << "Require: Set Class: " << className.string() << " [" << pClassInfo << "]";
        car_class_template->SetClassName(
            v8::String::NewFromUtf8(isolate, className.string(), NewStringType::kNormal)
            .ToLocalChecked());
        car_class_template->ReadOnlyPrototype();
        car_class_template->InstanceTemplate()->SetInternalFieldCount(1);

        // 注册所有的方法
        Int32 nMtdCnt;
        ec = pClassInfo->GetMethodCount(&nMtdCnt);
        if (FAILED(ec)) {
            LOG(INFO) << "Require: GetMethodCount failed";
            return;
        }

        ArrayOf<IMethodInfo*>* pMethodInfoArray = ArrayOf<IMethodInfo*>::Alloc(nMtdCnt);
        ec = pClassInfo->GetAllMethodInfos(pMethodInfoArray);
        if (FAILED(ec)) {
            LOG(INFO) << "Require: GetAllMethodInfo failed";
            return;
        }
        for(int j = 0 ; j < nMtdCnt ; j++)  {
            IMethodInfo* pMethodInfo = (*pMethodInfoArray)[j];
            Elastos::String szMtdName;
            ec = pMethodInfo->GetName(&szMtdName);
            if (FAILED(ec)) {
                LOG(INFO) << "Require: pMethodInfo->GetName failed";
                return;
            }
            LOG(INFO) << "Require: Set method: " << szMtdName.string() << " [" << pMethodInfo << "] for " << className.string();
            car_class_template->PrototypeTemplate()->Set(
                v8::String::NewFromUtf8(isolate, szMtdName.string(), NewStringType::kNormal)
                    .ToLocalChecked(),
                FunctionTemplate::New(isolate, MethodInvoke, v8::External::New(isolate, (void *)(pMethodInfo)), car_class_signature));
        }

        context->Global()->Set(
            v8::String::NewFromUtf8(isolate, className.string(), NewStringType::kNormal)
            .ToLocalChecked(),
            car_class_template->GetFunction());
    }
}

bool ela::CarManager::initialize(v8::Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    LOG(INFO) << "CarManager initialize.";

    v8::Local<v8::FunctionTemplate> RequireTemplate = v8::FunctionTemplate::New(isolate, Require);
    context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "require", NewStringType::kNormal)
                  .ToLocalChecked(),
              RequireTemplate->GetFunction());

    return 1;
}
