#include <nan.h>

#include "bridge/macros.h"
#include "bridge/nan-ext.h"

#include "bridge/car-interface-adapter.h"
#include "bridge/car-object.h"
#include "bridge/error.h"
#include "bridge/parse-uri.h"
#include "bridge/require.h"

#include "car_manager.h"
#include "args_converter.h"

#include "base/logging.h"
#include <android/log.h>

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
    Nan::HandleScope scope;

    Local<Value> arg0;

    if (info.Length() < 1)
        Throw_LOG(CAR_BRIDGE::Error::INVALID_ARGUMENT, 0);

    arg0 = info[0];
    if (!arg0->IsString())
        Throw_LOG(CAR_BRIDGE::Error::INVALID_ARGUMENT, 0);

    ParseURI uri(*Utf8String(arg0));
	LOG(INFO) << "car uri: " << *Utf8String(arg0);

    NAN_METHOD_RETURN_VALUE(
            Require(*Utf8String(arg0)/*uri.ecoPath()*/,
                uri.major(), uri.minor(), uri.build(), uri.revision(),
                uri.nEntryIds(), uri.entryIds())
            );
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
    __android_log_print(ANDROID_LOG_INFO, "chromium", \
                        "[%s at %s:%d] %s %d" , func, file, line, info, ecode);

    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(
        v8::String::NewFromUtf8(isolate, "", NewStringType::kNormal)
        .ToLocalChecked());

    //abort();
}

bool ela::CarManager::initialize(v8::Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    LOG(INFO) << "CarManager initialize.";

	Carbridge_Initialize(context->Global());

    return 1;
}

