#ifndef __CAR_BRIDGE_CAR_ARGUMENTS_CC_H
#define __CAR_BRIDGE_CAR_ARGUMENTS_CC_H

#include <memory>
#include <new>
#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "error.h"
#include "weak-external-base.h"

CAR_BRIDGE_NAMESPACE_BEGIN

template<class _CalleeAllocCARArray>
_CalleeAllocCARArray *CalleeAllocCARArray_(ICarArrayInfo const *carArrayInfo, Elastos::CarQuintet **carQuintet)
{
    ::std::unique_ptr<_CalleeAllocCARArray, typename _CalleeAllocCARArray::Deleter> carArray(
        new(::std::nothrow) _CalleeAllocCARArray
    );
    if (carArray == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    carArray->carArrayInfo = const_cast<ICarArrayInfo *>(carArrayInfo);
    carArray->carQuintet = carQuintet;
    return carArray.release();
}

template<class _CalleeAllocStruct>
_CalleeAllocStruct *CalleeAllocStruct_(IStructInfo *structInfo, void **struct_)
{
    ::std::unique_ptr<_CalleeAllocStruct, typename _CalleeAllocStruct::Deleter> _struct(new(::std::nothrow) _CalleeAllocStruct);
    if (_struct == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _struct->structInfo = structInfo;
    _struct->struct_ = struct_;
    return _struct.release();
}

template<class _CallerAllocInt16>
_CallerAllocInt16 *CallerAllocInt16_(Elastos::Int16 *i16)
{
    ::std::unique_ptr<_CallerAllocInt16, typename _CallerAllocInt16::Deleter> _i16(new(::std::nothrow) _CallerAllocInt16);
    if (_i16 == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _i16->i16 = i16;
    return _i16.release();
}

template<class _CallerAllocInt32>
_CallerAllocInt32 *CallerAllocInt32_(Elastos::Int32 *i32)
{
    ::std::unique_ptr<_CallerAllocInt32, typename _CallerAllocInt32::Deleter> _i32(new(::std::nothrow) _CallerAllocInt32);
    if (_i32 == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _i32->i32 = i32;
    return _i32.release();
}

template<class _CallerAllocInt64>
_CallerAllocInt64 *CallerAllocInt64_(Elastos::Int64 *i64)
{
    ::std::unique_ptr<_CallerAllocInt64, typename _CallerAllocInt64::Deleter> _i64(new(::std::nothrow) _CallerAllocInt64);
    if (_i64 == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _i64->i64 = i64;
    return _i64.release();
}

template<class _CallerAllocByte>
_CallerAllocByte *CallerAllocByte_(Elastos::Byte *byte)
{
    ::std::unique_ptr<_CallerAllocByte, typename _CallerAllocByte::Deleter> _byte(new(::std::nothrow) _CallerAllocByte);
    if (_byte == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _byte->byte = byte;
    return _byte.release();
}

template<class _CallerAllocFloat>
_CallerAllocFloat *CallerAllocFloat_(Elastos::Float *f)
{
    ::std::unique_ptr<_CallerAllocFloat, typename _CallerAllocFloat::Deleter> _f(new(::std::nothrow) _CallerAllocFloat);
    if (_f == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _f->f = f;
    return _f.release();
}

template<class _CallerAllocDouble>
_CallerAllocDouble *CallerAllocDouble_(Elastos::Double *d)
{
    ::std::unique_ptr<_CallerAllocDouble, typename _CallerAllocDouble::Deleter> _d(new(::std::nothrow) _CallerAllocDouble);
    if (_d == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _d->d = d;
    return _d.release();
}

template<class _CallerAllocChar32>
_CallerAllocChar32 *CallerAllocChar32_(Elastos::Char32 *c32)
{
    ::std::unique_ptr<_CallerAllocChar32, typename _CallerAllocChar32::Deleter> _c32(new(::std::nothrow) _CallerAllocChar32);
    if (_c32 == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _c32->c32 = c32;
    return _c32.release();
}

template<class _CallerAllocString>
_CallerAllocString *CallerAllocString_(Elastos::String *s)
{
    ::std::unique_ptr<_CallerAllocString, typename _CallerAllocString::Deleter> _s(new(::std::nothrow) _CallerAllocString);
    if (_s == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _s->s = s;
    return _s.release();
}

template<class _CallerAllocBoolean>
_CallerAllocBoolean *CallerAllocBoolean_(Elastos::Boolean *b)
{
    ::std::unique_ptr<_CallerAllocBoolean, typename _CallerAllocBoolean::Deleter> _b(new(::std::nothrow) _CallerAllocBoolean);
    if (_b == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _b->b = b;
    return _b.release();
}

template<class _CallerAllocEMuid>
_CallerAllocEMuid *CallerAllocEMuid_(Elastos::EMuid *id)
{
    ::std::unique_ptr<_CallerAllocEMuid, typename _CallerAllocEMuid::Deleter> _id(new(::std::nothrow) _CallerAllocEMuid);
    if (_id == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _id->id = id;
    return _id.release();
}

template<class _CallerAllocEGuid>
_CallerAllocEGuid *CallerAllocEGuid_(Elastos::EGuid *id)
{
    ::std::unique_ptr<_CallerAllocEGuid, typename _CallerAllocEGuid::Deleter> _id(new(::std::nothrow) _CallerAllocEGuid);
    if (_id == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _id->id = id;
    return _id.release();
}

template<class _CallerAllocECode>
_CallerAllocECode *CallerAllocECode_(Elastos::ECode *ecode)
{
    ::std::unique_ptr<_CallerAllocECode, typename _CallerAllocECode::Deleter> _ecode(new(::std::nothrow) _CallerAllocECode);
    if (_ecode == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _ecode->ecode = ecode;
    return _ecode.release();
}

template<class _CallerAllocLocalPtr>
_CallerAllocLocalPtr *CallerAllocLocalPtr_(void **localPtr)
{
    ::std::unique_ptr<_CallerAllocLocalPtr, typename _CallerAllocLocalPtr::Deleter> _localPtr(
        new(::std::nothrow) _CallerAllocLocalPtr
    );
    if (_localPtr == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _localPtr->localPtr = localPtr;
    return _localPtr.release();
}

template<class _CallerAllocLocalType>
_CallerAllocLocalType *CallerAllocLocalType_(IDataTypeInfo const *dataTypeInfo, void *localTypeObject)
{
    ::std::unique_ptr<_CallerAllocLocalType, typename _CallerAllocLocalType::Deleter> _localTypeObject(
        new(::std::nothrow) _CallerAllocLocalType
    );
    if (_localTypeObject == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _localTypeObject->dataTypeInfo = dataTypeInfo;
    _localTypeObject->localTypeObject = localTypeObject;
    return _localTypeObject.release();
}

template<class _CallerAllocEnum>
_CallerAllocEnum *CallerAllocEnum_(Elastos::Int32 *enum_)
{
    ::std::unique_ptr<_CallerAllocEnum, typename _CallerAllocEnum::Deleter> _enum(new(::std::nothrow) _CallerAllocEnum);
    if (_enum == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _enum->enum_ = enum_;
    return _enum.release();
}

template<class _CallerAllocCARArray>
_CallerAllocCARArray *CallerAllocCARArray_(ICarArrayInfo       *carArrayInfo, IVariableOfCarArray *variableOfCARArray)
{
    ::std::unique_ptr<_CallerAllocCARArray, typename _CallerAllocCARArray::Deleter> carArray(
        new(::std::nothrow) _CallerAllocCARArray
    );
    if (carArray == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    carArray->carArrayInfo = carArrayInfo;
    carArray->variableOfCARArray = variableOfCARArray;
    return carArray.release();
}

template<class _CallerAllocStruct>
_CallerAllocStruct *CallerAllocStruct_(IStructInfo        *structInfo, IVariableOfStruct *variableOfStruct)
{
    ::std::unique_ptr<_CallerAllocStruct, typename _CallerAllocStruct::Deleter> struct_(new(::std::nothrow) _CallerAllocStruct);
    if (struct_ == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    struct_->structInfo = structInfo;
    struct_->variableOfStruct = variableOfStruct;
    return struct_.release();
}

template<class _CallerAllocInterface>
_CallerAllocInterface *CallerAllocInterface_(IInterfaceInfo *interfaceInfo, IInterface **interface_)
{
    ::std::unique_ptr<_CallerAllocInterface, typename _CallerAllocInterface::Deleter> _interface(
        new(::std::nothrow) _CallerAllocInterface
    );
    if (_interface == nullptr)
        Throw_LOG(Error::NO_MEMORY, 0);
    _interface->interface_ = interface_;
    return _interface.release();
}

CAR_BRIDGE_NAMESPACE_END

#endif
