#include <nan.h>

#include "macros.h"
#include "nan-ext.h"

#include "car-interface-adapter.h"
#include "car-object.h"
#include "error.h"
#include "parse-uri.h"
#include "require.h"

#include "car-bridge.h"

#include "logging.h"

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

using namespace Nan;
using namespace v8;

CAR_BRIDGE_NAMESPACE_USING

static NAN_METHOD(Require)
{
    Nan::EscapableHandleScope scope;
    Local<Value> arg0;
    Local<Value> arg1;

    if (info.Length() < 2)
        Throw_LOG(Error::INVALID_ARGUMENT, 0);

    arg0 = info[0];
    arg1 = info[1];
    if (!arg0->IsString() || !arg1->IsString())
        Throw_LOG(Error::INVALID_ARGUMENT, 0);

    Local<Value> result = Require(*Utf8String(arg0), *Utf8String(arg1));
    Debug_LOG("Require finished.");

    NAN_METHOD_RETURN_VALUE(scope.Escape(result));
}

void Carbridge_Initialize(v8::Local<v8::Object> target)
{
    CARObject::Initialize();
    CARInterfaceAdapter::Initialize();

    Export(target, "version", New("0.0.0").ToLocalChecked());
    Export(target, "require", Require);
}

void Carbridge_throw(const char* func, const char* file, int line, char const * info, int const ecode)
{
    Debug_LOG("%s %d at [%s %s:%d]", info, ecode, func, file, line);

    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(
        v8::String::NewFromUtf8(isolate, "", NewStringType::kNormal)
        .ToLocalChecked());

    //abort();
}

bool ela::CarManager::initialize(v8::Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    Debug_LOG("CarManager initialize.");

    Carbridge_Initialize(context->Global());
    Carrier::initialize(isolate, context);

    return 1;
}

