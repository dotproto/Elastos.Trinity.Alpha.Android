#include "V8CHello.h"

#include "bindings/core/v8/BindingSecurity.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/WindowProxy.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8DOMConfiguration.h"
#include "bindings/core/v8/V8DOMStringList.h"
#include "core/dom/Document.h"
#include "core/frame/UseCounter.h"
#include <dlfcn.h>
#include "base/logging.h"

using namespace v8;

namespace ela {

void*   s_module = NULL;
IHello* s_phello = NULL;

static create_t* Hello_Create = NULL;

static void CHelloCreate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if(NULL != Hello_Create) {
        s_phello = Hello_Create();
        LOG(INFO) << "CHelloCreate";
    }
    else {
        LOG(ERROR) << "CHello is not loaded!";
    }
}

static void CHello_hello(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() < 1)
        return;

    v8::Local<Value> arg = args[0];
    v8::String::Utf8Value value(args.GetIsolate(), arg);
    LOG(INFO) << "CHello_hello " << *value;
    const char* result = s_phello->hello(*value);
    args.GetReturnValue().Set(
      v8::String::NewFromUtf8(args.GetIsolate(), result,
                          NewStringType::kNormal,
                          strlen(result)).ToLocalChecked());
}

void V8CHello::installV8CHelloTemplate(v8::Isolate* isolate, void* module)
{
    LOG(INFO) << "V8CHello::installV8CHelloTemplate";
    s_module = module;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    Local<FunctionTemplate> CHello_fun_template =
      FunctionTemplate::New(isolate, CHelloCreate);
    Local<Signature> CHello_signature =
      Signature::New(isolate, CHello_fun_template);
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

   // load CHello from so
    dlerror();
    Hello_Create = (create_t*)dlsym(s_module, "Create");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        dlclose(s_module);
        LOG(ERROR) << "laod CHello new symbol error!";
    }
    LOG(INFO) << "V8CHello::installV8CHelloTemplate success.";
}

} //namespace ela