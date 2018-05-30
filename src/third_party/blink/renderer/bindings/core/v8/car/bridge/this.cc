#include <nan.h>
#include "macros.h"
#include "nan-ext.h"
#include "car-interface-adapter.h"
#include "car-object.h"
#include "error.h"
#include "parse-uri.h"
#include "require.h"
using namespace Nan;
using namespace v8;

CAR_BRIDGE_NAMESPACE_BEGIN

static NAN_METHOD(Require)
{
#if 0 //?try
    try
    {
#endif
        Nan::HandleScope scope;
        Local<Value> arg0;
        if (info.Length() < 1)
            Throw_LOG(Error::INVALID_ARGUMENT, 0);
        arg0 = info[0];
        if (!arg0->IsString())
            Throw_LOG(Error::INVALID_ARGUMENT, 0);
        ParseURI uri(*Utf8String(arg0));
        NAN_METHOD_RETURN_VALUE(
            Require(uri.ecoPath(),
                    uri.major(), uri.minor(), uri.build(), uri.revision(),
                    uri.nEntryIds(), uri.entryIds())
        );
#if 0
    }
    catch (Error const &error)
    {
        Nan::HandleScope scope;
        ThrowError(ToValue(error));
    }
    catch (...)
    {
        Nan::HandleScope scope;
        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
#endif
}

void Car_Initialize(v8::Local<v8::Object> target)
{
    CARObject::Initialize();
    CARInterfaceAdapter::Initialize();
    Export(target, "version", New("0.0.0").ToLocalChecked());
    Export(target, "require", Require);
}

CAR_BRIDGE_NAMESPACE_END
