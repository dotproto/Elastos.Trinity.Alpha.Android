#include "V8CHello.h"

#include "elastos.h"
#include "ElastosCore.h"

#include "base/logging.h"

using namespace v8;

namespace ela {

static void CHelloCreate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   LOG(INFO) << "CHelloCreate";
}

static void CHello_hello(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() < 1)
        return;

    const char* libname = "/data/app/org.chromium.chrome-1/lib/arm/libElastos.HelloCarDemo.so";
    const char* className = "CHelloCar";

    LOG(INFO) << "ReflectionDemo Start reflction";

    AutoPtr<IModuleInfo> pMdlInfo;
    ECode ec = CReflector::AcquireModuleInfo(Elastos::String(libname), (IModuleInfo**)&pMdlInfo);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo AcquireModuleInfo failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo GetClassInfo";
    AutoPtr<IClassInfo> pClsInfo;
    ec = pMdlInfo->GetClassInfo(Elastos::String(className), (IClassInfo**)&pClsInfo);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo GetClassInfo failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo CreateObject";
    AutoPtr<IInterface> pObject;
    ec = pClsInfo->CreateObject((IInterface**)&pObject);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo CreateObject failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo GetMethodCount";
    Elastos::Int32 nMtdCnt;
    ec = pClsInfo->GetMethodCount(&nMtdCnt);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo GetMethodCount failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo GetAllMethodInfos";
    ArrayOf<IMethodInfo*>* pMtdInfoArray = ArrayOf<IMethodInfo*>::Alloc(nMtdCnt);
    ec = pClsInfo->GetAllMethodInfos(pMtdInfoArray);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo GetAllMethodInfo failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo MtdInfo GetName";
    IMethodInfo *pMtdInfo = (*pMtdInfoArray)[0]; //Hello()
    Elastos::String szMtdName;
    ec = pMtdInfo->GetName(&szMtdName);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo pMtdInfo->GetName failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo CreateArgumentList";
    AutoPtr<IArgumentList> pArgList;
    ec = pMtdInfo->CreateArgumentList((IArgumentList**)&pArgList);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo CreateArgumentList failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo SetOutputArgumentOfElastos::StringPtr";
    Elastos::String output;
    ec = pArgList->SetOutputArgumentOfStringPtr(0, &output);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo SetOutputArgumentOfElastos::StringPtr failed";
        return ;
    }

    LOG(INFO) << "ReflectionDemo Invoke method.";
    ec = pMtdInfo->Invoke(pObject.Get(), pArgList);
    if (FAILED(ec)) {
        LOG(ERROR) << "ReflectionDemo Invoke failed" ;
        return ;
    } else {
        LOG(INFO) << "ReflectionDemo Hello " << output.string() ;

        args.GetReturnValue().Set(
            v8::String::NewFromUtf8(args.GetIsolate(), output.string(),
                          NewStringType::kNormal,
                          output.GetByteLength()).ToLocalChecked());
    }
    LOG(INFO) << "ReflectionDemo Done" ;
}

void V8CHello::installV8CHelloTemplate(v8::Isolate* isolate)
{
    LOG(INFO) << "V8CHello::installV8CHelloTemplate";
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    Local<FunctionTemplate> CHello_fun_template = FunctionTemplate::New(isolate, CHelloCreate);
    Local<Signature> CHello_signature = Signature::New(isolate, CHello_fun_template);
    CHello_fun_template->SetClassName(
      v8::String::NewFromUtf8(isolate, "CHello", NewStringType::kNormal)
          .ToLocalChecked());
    CHello_fun_template->ReadOnlyPrototype();
    CHello_fun_template->PrototypeTemplate()->Set(
      v8::String::NewFromUtf8(isolate, "hello", NewStringType::kNormal)
          .ToLocalChecked(),
      FunctionTemplate::New(isolate, CHello_hello, Local<Value>(),
                            CHello_signature));

    context->Global()->Set(
      v8::String::NewFromUtf8(isolate, "CHello", NewStringType::kNormal)
          .ToLocalChecked(),
      CHello_fun_template->GetFunction());

    LOG(INFO) << "V8CHello::installV8CHelloTemplate success.";
}

} //namespace ela