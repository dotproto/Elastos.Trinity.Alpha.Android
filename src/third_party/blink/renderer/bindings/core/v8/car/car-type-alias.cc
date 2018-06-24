#include <map>
#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "nan-ext.h"
#include "car-data-type.h"
#include "error.h"
#include "js-2-car.h"

using namespace std;
using namespace Nan;
using namespace v8;

_ELASTOS_NAMESPACE_USING
CAR_BRIDGE_NAMESPACE_BEGIN

static map<AutoPtr<IFunctionInfo>, CopyablePersistent<Object>> _mapFunctionInfoToCARFunction;
static map<AutoPtr<ITypeAliasInfo>, CopyablePersistent<Object>> _mapTypeAliasInfoToCARTypeAlias;

Local<Object> CARTypeAlias(ITypeAliasInfo const *ptypeAliasInfo)
{
    Nan::EscapableHandleScope scope;
    Local<Object> typeAlias;
    ECode ec;
    Elastos::String name;
    Elastos::Boolean isDummy;
    AutoPtr<IDataTypeInfo> typeInfo;
    IDataTypeInfo *_typeInfo;

    ITypeAliasInfo *typeAliasInfo = const_cast<ITypeAliasInfo *>(ptypeAliasInfo);

    auto &_typeAlias = _mapTypeAliasInfoToCARTypeAlias[typeAliasInfo];
    if (!_typeAlias.IsEmpty())
        return scope.Escape(New(_typeAlias));

    typeAlias = New<Object>();
    DefineOwnProperty(typeAlias,
                      New("$what").ToLocalChecked(),
                      New("CARTypeAlias").ToLocalChecked(),
                      static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = typeAliasInfo->GetName(&name);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    DefineOwnProperty(typeAlias,
                      New("$name").ToLocalChecked(),
                      ToValue(name),
                      static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));
    ec = typeAliasInfo->IsDummy(&isDummy);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    DefineOwnProperty(typeAlias,
                      New("isDummy").ToLocalChecked(),
                      ToValueFromBoolean(isDummy),
                      static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    ec = typeAliasInfo->GetTypeInfo(&_typeInfo);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    typeInfo = _typeInfo, _typeInfo->Release();
    DefineOwnProperty(typeAlias,
                      New("realType").ToLocalChecked(),
                      CARDataType(typeInfo),
                      static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    _typeAlias.Reset(typeAlias);

    return scope.Escape(typeAlias);
}

CAR_BRIDGE_NAMESPACE_END
