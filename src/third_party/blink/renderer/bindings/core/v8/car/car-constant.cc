#include <map>
#include <nan.h>
#include "car-constantoid.h"

using namespace std;
using namespace Nan;
using namespace v8;

_ELASTOS_NAMESPACE_USING
CAR_BRIDGE_NAMESPACE_BEGIN

static map<AutoPtr<IConstantInfo >, CopyablePersistent<NumberObject>> _mapConstantInfoToCARConstant;

Local<NumberObject> CARConstant(IConstantInfo *constantInfo)
{
    Local<NumberObject> constant;
    auto &_constant = _mapConstantInfoToCARConstant[constantInfo];
    if (!_constant.IsEmpty())
        return New(_constant);

    constant = CARConstantoid(constantInfo, "CARConstant");
    _constant.Reset(constant);
    return constant;
}

CAR_BRIDGE_NAMESPACE_END
