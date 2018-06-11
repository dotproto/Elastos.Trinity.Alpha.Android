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
    Nan::HandleScope scope;
    Local<Value> arg0;
    Local<Value> arg1;

    if (info.Length() < 2)
        Throw_LOG(Error::INVALID_ARGUMENT, 0);

    arg0 = info[0];
    arg1 = info[1];
    if (!arg0->IsString() || !arg1->IsString())
        Throw_LOG(Error::INVALID_ARGUMENT, 0);

    NAN_METHOD_RETURN_VALUE(
        Require(*Utf8String(arg0), *Utf8String(arg1))
    );
}

void Car_Initialize(v8::Local<v8::Object> target)
{
    CARObject::Initialize();
    CARInterfaceAdapter::Initialize();
    Export(target, "version", New("0.0.1").ToLocalChecked());
    Export(target, "require", Require);
}

CAR_BRIDGE_NAMESPACE_END
