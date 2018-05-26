
#include <map>
#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "nan-ext.h"
#include "car-constantoid.h"

using namespace std;
using namespace Nan;
using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

static map<AutoPtr<IConstantInfo >, CopyablePersistent<NumberObject>> _mapConstantInfoToCARConstant;

#pragma clang diagnostic pop

Local<NumberObject> CARant(IConstantInfo   * antInfo)
{
    Local<NumberObject> ant;

    auto &_ant = _mapConstantInfoToCARConstant[antInfo];
    if (!_ant.IsEmpty())
        return New(_ant);

    ant = CARConstantoid(antInfo, "CARant");

    _ant.Reset(ant);

    return ant;
}

CAR_BRIDGE_NAMESPACE_END

