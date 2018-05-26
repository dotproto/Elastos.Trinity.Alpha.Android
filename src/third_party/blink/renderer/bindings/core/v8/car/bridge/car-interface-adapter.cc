
#include <cassert>
#include <cstdarg>
#include <cstddef>

#include <new>

#include <nan.h>

#include <elastos.h>
#include "macros.h"
#include "elastos-ext.h"
#include "car-interface-adapter.h"
#include "car-function-adapter.h"
#include "error.h"
#include "js-2-car.h"

using namespace std;
using namespace Nan;
using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

static int const _MAX_NUM_OF_METHODS = 0XE4;

typedef void (*VMethod)(void);

static VMethod _vtable[_MAX_NUM_OF_METHODS];

template<size_t end>
void CARInterfaceAdapter::InitializeVTableFrom4To(void) noexcept
{
    InitializeVTableFrom4To<end - 1>();

    _vtable[end - 1] = reinterpret_cast<VMethod>(&CallOtherMethodIndexed<end - 5>);
}

template<>
void CARInterfaceAdapter::InitializeVTableFrom4To<5>(void) noexcept
{
    _vtable[4] = reinterpret_cast<VMethod>(&CallOtherMethodIndexed<0>);
}

template<>
void CARInterfaceAdapter::InitializeVTableFrom4To<4>(void) noexcept
{}

template<>
void CARInterfaceAdapter::InitializeVTableFrom4To<3>(void) noexcept
{}

template<>
void CARInterfaceAdapter::InitializeVTableFrom4To<2>(void) noexcept
{}

template<>
void CARInterfaceAdapter::InitializeVTableFrom4To<1>(void) noexcept
{}

template<>
void CARInterfaceAdapter::InitializeVTableFrom4To<0>(void) noexcept
{}

void CARInterfaceAdapter::Initialize(void) noexcept
{
    _vtable[0] = reinterpret_cast<VMethod>(&CARInterfaceAdapter::Probe);
    _vtable[1] = reinterpret_cast<VMethod>(&CARInterfaceAdapter::AddRef);
    _vtable[2] = reinterpret_cast<VMethod>(&CARInterfaceAdapter::Release);
    _vtable[3] = reinterpret_cast<VMethod>(&CARInterfaceAdapter::GetInterfaceID);

    InitializeVTableFrom4To<_MAX_NUM_OF_METHODS>();
}

CARAPI CARInterfaceAdapter::New(
        IInterface **interface_,
        IInterfaceInfo const *interfaceInfo, Local<::v8::Object> object) noexcept
{
#if 0//?jw
    try {
#endif
        CARInterfaceAdapter *carInterfaceAdapter;

        AutoPtr<IInterface> _interface;

        carInterfaceAdapter = new(nothrow) CARInterfaceAdapter(const_cast<IInterfaceInfo*>(interfaceInfo), object);
        if (carInterfaceAdapter == nullptr)
            LOG(Error::NO_MEMORY, 0);

        _interface = reinterpret_cast<IInterface *>(carInterfaceAdapter);

        if (interface_ != nullptr)
            _interface->AddRef(), *interface_ = _interface;
#if 0//?jw
    } catch (Error const &error) {
        return ToECode(error);
    } catch (...) {
        return E_FAILED;
    }
#endif
    return NO_ERROR;
}

CARInterfaceAdapter::CARInterfaceAdapter(IInterfaceInfo* pinterfaceInfo, Local<::v8::Object> object)
{

    ECode ec;

    _ELASTOS Int32 nMethods;

    AutoPtr<ArrayOf<IMethodInfo *> > methodInfos;

    _vtptr = _vtable;

    _referenceCount = 0;
	_interfaceInfo = pinterfaceInfo;
#if 0//?jw
    //_object = object;
#endif
    IInterfaceInfo *interfaceInfo = const_cast<IInterfaceInfo *>(pinterfaceInfo);
    ec = interfaceInfo->GetId(&_interfaceId);

    if (FAILED(ec))
        LOG(Error::TYPE_ELASTOS, ec);

    ec = interfaceInfo->GetMethodCount(&nMethods);
    if (FAILED(ec))
        LOG(Error::TYPE_ELASTOS, ec);

    methodInfos = ArrayOf<IMethodInfo *>::Alloc(nMethods);
    if (methodInfos == 0)
        LOG(Error::NO_MEMORY, 0);

    ec = interfaceInfo->GetAllMethodInfos(reinterpret_cast<ArrayOf<IMethodInfo *> *>(methodInfos.Get()));
    if (FAILED(ec))
        LOG(Error::TYPE_ELASTOS, ec);

    _methodInfos = methodInfos;
}

CARInterfaceAdapter::~CARInterfaceAdapter()
{
    _object.Reset();
}

CARAPI_(IInterface *) CARInterfaceAdapter::Probe(CARInterfaceAdapter *self, REIID riid)
{
    if (riid == EIID_IInterface)
        return reinterpret_cast<IInterface *>(self);
    else if (riid == self->_interfaceId)
        return reinterpret_cast<IInterface *>(self);
    else
        return 0;
}

CARAPI_(UInt32) CARInterfaceAdapter::AddRef(CARInterfaceAdapter *self)
{
    return atomic_inc(&self->_referenceCount);
}

CARAPI_(UInt32) CARInterfaceAdapter::Release(CARInterfaceAdapter *self)
{
    Int32 referenceCount;

    referenceCount = atomic_dec(&self->_referenceCount);
    assert(referenceCount >= 0);
    if (referenceCount == 0)
        delete self;

    return referenceCount;
}

CARAPI CARInterfaceAdapter::GetInterfaceID(CARInterfaceAdapter *self,
        IInterface *object,
        InterfaceID *interfaceId) noexcept
{
    if (object == 0)
        return E_INVALID_ARGUMENT;

    if (interfaceId == 0)
        return E_INVALID_ARGUMENT;

    if (object == reinterpret_cast<IInterface *>(self))
        *interfaceId = self->_interfaceId;
    else
        return E_INVALID_ARGUMENT;

    return NO_ERROR;
}

template<size_t index>
CARAPI CARInterfaceAdapter::CallOtherMethodIndexed(CARInterfaceAdapter *self, ...) noexcept
{
    va_list ap;

    ECode ec;

    va_start(ap, self);

    ec = CallOtherMethodIndexed(index, self, ap);

    va_end(ap);

    return ec;
}

CARAPI CARInterfaceAdapter::CallOtherMethodIndexed(size_t index, CARInterfaceAdapter *self, va_list ap) noexcept
{
    ::Nan::HandleScope scope;

    IMethodInfo *methodInfo;

    ECode ec;

    _ELASTOS String methodName;
    Local<::v8::String> _methodName;

    Local<Value> value;

    methodInfo = const_cast<IMethodInfo *>((*self->_methodInfos)[index]);

    ec = methodInfo->GetName(&methodName);
    if (FAILED(ec))
        return ec;

    _methodName = ToValue(methodName).As<::v8::String>();

    if (!Has(::Nan::New(self->_object), _methodName).FromJust())
        return E_NOT_IMPLEMENTED;

    value = Get(::Nan::New(self->_object), _methodName).ToLocalChecked();
    if (!value->IsFunction())
        return E_NOT_IMPLEMENTED;

    return CARFunctionAdapter::Call(value.As<Function>(), ::Nan::New(self->_object), methodInfo, ap);
}

CAR_BRIDGE_NAMESPACE_END
