#include "ModuleManager.h"
#include <stdlib.h>
#include <map>
#include <string>
#include <dlfcn.h>

#include "elastos.h"
#include "ElastosCore.h"

#include "V8CHello.h"
#include "base/logging.h"

using namespace std;
using namespace v8;

namespace ela {

static v8::Isolate* s_isolate = 0;

static bool installModule(const char* name);

static void logCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() < 1)
        return;

    Local<Value> arg = args[0];
    v8::String::Utf8Value value(args.GetIsolate(), arg);
    //LOG(INFO) << "[js] " << *value;
}

static void requireCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Local<Value> arg = args[0];
    v8::String::Utf8Value moduleName(args.GetIsolate(), arg);
    //LOG(INFO) << "require : " << *moduleName << "module";
    installModule(*moduleName);
}

bool ModuleManager::initialize(v8::Isolate* isolate) {
    s_isolate = isolate;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    //LOG(INFO) << "ela initialize.";

    v8::Local<v8::FunctionTemplate> logTemplate = v8::FunctionTemplate::New(
          isolate, logCallback);
    logTemplate->InstanceTemplate()->SetInternalFieldCount(1);

    context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "log", NewStringType::kNormal)
                  .ToLocalChecked(),
              logTemplate->GetFunction());

    v8::Local<v8::FunctionTemplate> requireTemplate = v8::FunctionTemplate::New(
          isolate, requireCallback);
    logTemplate->InstanceTemplate()->SetInternalFieldCount(1);
    context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "require", NewStringType::kNormal)
                  .ToLocalChecked(),
              requireTemplate->GetFunction());

    return 1;
}

bool installModule(const char* name)
{
    LOG(INFO) << "installModule " << name;

    IModuleInfo *pMdlInfo;
    CReflector::AcquireModuleInfo(Elastos::String(name), &pMdlInfo);
    if(NULL == pMdlInfo) {
        LOG(INFO) << "AcquireModuleInfo " << name << "error!";
        return false;
    }
    LOG(INFO) << " debug ";
    IClassInfo *pClsInfos;
    pMdlInfo->GetClassInfo(Elastos::String("CHello"), &pClsInfos);
    if(NULL == pClsInfos) {
        LOG(INFO) << "GetClassInfo CHello error!";
        return false;
    }
    LOG(INFO) << " debug " ;
    PInterface pInterface;
    pClsInfos->CreateObject(&pInterface);
    if(NULL == pInterface) {
        LOG(INFO) << "GetClassInfo CHello error!";
        return false;
    }
    LOG(INFO) << " debug ";
    // call constructor
    Elastos::Int32 nMtdCnt;
    pClsInfos->GetMethodCount(&nMtdCnt);
    LOG(INFO) << "method count " << nMtdCnt;

    ArrayOf<IMethodInfo*> *pMtdInfo = ArrayOf<IMethodInfo*>::Alloc(nMtdCnt);
    pClsInfos->GetAllMethodInfos(pMtdInfo);
    IMethodInfo *pMethod = (*pMtdInfo)[0];
    if(NULL == pMethod) {
        LOG(INFO) << "error: pMtdInfo 0 is null.";
        return false;
    } 
    IArgumentList *al;
    pMethod->CreateArgumentList(&al);

    LOG(INFO) << "invoke method.";
    // transform parameters 
    pMethod->Invoke(pInterface, al);

#if 0
    //LOG(INFO) << "installModule start.";
    void* helloModule = NULL;
    if(NULL == s_isolate) {
        //LOG(INFO) << "module manager not initialize!";
        return 0;
    }

    //LOG(INFO) << "load " << name <<" module.";
    helloModule = dlopen(name, RTLD_LAZY);
    if (!helloModule) {
        //LOG(INFO) << name <<" dlopen faild; " << dlerror();
        helloModule = dlopen("/data/app/org.chromium.chrome-1/lib/arm/libchello.so", RTLD_LAZY);
        if (!helloModule) {
            //LOG(INFO) << "libchello.so dlopen faild; " << dlerror();
            return 1;
        }
    }

    ela::V8CHello::installV8CHelloTemplate(s_isolate, helloModule);
    //LOG(INFO) << name <<" dlopen success!";
#endif
    return 0;
}

} //namespace ela