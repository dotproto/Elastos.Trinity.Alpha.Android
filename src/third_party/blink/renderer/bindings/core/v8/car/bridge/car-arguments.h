#ifndef __CAR_BRIDGE_CAR_ARGUMENTS_H
#define __CAR_BRIDGE_CAR_ARGUMENTS_H

#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "weak-external-base.h"

CAR_BRIDGE_NAMESPACE_BEGIN

struct CARArgumentBase : WeakExternalBase
{
    Nan::Persistent<v8::Value> value;
protected:
    ~CARArgumentBase() override
    {
        value.Reset();
    }
};
struct CalleeAllocCARArray: CARArgumentBase
{
    Elastos::AutoPtr<ICarArrayInfo> carArrayInfo;
    Elastos::CarQuintet **carQuintet = nullptr;
protected:
    ~CalleeAllocCARArray() override = default;
};

template<class _CalleeAllocCARArray = struct CalleeAllocCARArray>
_CalleeAllocCARArray * CalleeAllocCARArray_(ICarArrayInfo const *carArrayInfo, Elastos::CarQuintet **carQuintet);
struct CalleeAllocStruct: CARArgumentBase
{
    Elastos::AutoPtr<IStructInfo> structInfo;
    void **struct_ = nullptr;
protected:
    ~CalleeAllocStruct() override = default;
};

template<class _CalleeAllocStruct = struct CalleeAllocStruct>
_CalleeAllocStruct * CalleeAllocStruct_(IStructInfo *structInfo, void **struct_);
struct CallerAllocInt16: CARArgumentBase
{
    Elastos::Int16 *i16 = nullptr;
protected:
    ~CallerAllocInt16() override = default;
};

template<class _CallerAllocInt16 = struct CallerAllocInt16>
_CallerAllocInt16 * CallerAllocInt16_(Elastos::Int16 *i16);
struct CallerAllocInt32: CARArgumentBase
{
    Elastos::Int32 *i32 = nullptr;
protected:
    ~CallerAllocInt32() override = default;
};

template<class _CallerAllocInt32 = struct CallerAllocInt32>
_CallerAllocInt32 * CallerAllocInt32_(Elastos::Int32 *i32);
struct CallerAllocInt64: CARArgumentBase
{
    Elastos::Int64 *i64 = nullptr;
protected:
    ~CallerAllocInt64() override = default;
};

template<class _CallerAllocInt64 = struct CallerAllocInt64>
_CallerAllocInt64 * CallerAllocInt64_(Elastos::Int64 *i64);
struct CallerAllocByte: CARArgumentBase
{
    Elastos::Byte *byte = nullptr;
protected:
    ~CallerAllocByte() override = default;
};

template<class _CallerAllocByte = struct CallerAllocByte>
_CallerAllocByte * CallerAllocByte_(Elastos::Byte *byte);
struct CallerAllocFloat: CARArgumentBase
{
    Elastos::Float *f = nullptr;
protected:
    ~CallerAllocFloat() override = default;
};

template<class _CallerAllocFloat = struct CallerAllocFloat>
_CallerAllocFloat * CallerAllocFloat_(Elastos::Float *f);
struct CallerAllocDouble: CARArgumentBase
{
    Elastos::Double *d = nullptr;
protected:
    ~CallerAllocDouble() override = default;
};

template<class _CallerAllocDouble = struct CallerAllocDouble>
_CallerAllocDouble * CallerAllocDouble_(Elastos::Double *d);
struct CallerAllocChar32: CARArgumentBase
{
    Elastos::Char32 *c32 = nullptr;
protected:
    ~CallerAllocChar32() override = default;
};

template<class _CallerAllocChar32 = struct CallerAllocChar32>
_CallerAllocChar32 * CallerAllocChar32_(Elastos::Char32 *c32);
struct CallerAllocString: CARArgumentBase
{
    Elastos::String *s = nullptr;
protected:
    ~CallerAllocString() override = default;
};

template<class _CallerAllocString = struct CallerAllocString>
_CallerAllocString * CallerAllocString_(Elastos::String *s);
struct CallerAllocBoolean: CARArgumentBase
{
    Elastos::Boolean *b = nullptr;
protected:
    ~CallerAllocBoolean() override = default;
};

template<class _CallerAllocBoolean = struct CallerAllocBoolean>
_CallerAllocBoolean * CallerAllocBoolean_(Elastos::Boolean *b);
struct CallerAllocEMuid: CARArgumentBase
{
    Elastos::EMuid *id = nullptr;
protected:
    ~CallerAllocEMuid() override = default;
};

template<class _CallerAllocEMuid = struct CallerAllocEMuid>
_CallerAllocEMuid * CallerAllocEMuid_(Elastos::EMuid *id);
struct CallerAllocEGuid: CARArgumentBase
{
    Elastos::EGuid *id = nullptr;
protected:
    ~CallerAllocEGuid()           override = default;
};

template<class _CallerAllocEGuid = struct CallerAllocEGuid>
_CallerAllocEGuid * CallerAllocEGuid_(Elastos::EGuid *id);
struct CallerAllocECode: CARArgumentBase
{
    Elastos::ECode *ecode = nullptr;
protected:
    ~CallerAllocECode() override = default;
};

template<class _CallerAllocECode = struct CallerAllocECode>
_CallerAllocECode * CallerAllocECode_(Elastos::ECode *ecode);
struct CallerAllocLocalPtr: CARArgumentBase
{
    void **localPtr = nullptr;
protected:
    ~CallerAllocLocalPtr() override = default;
};

template<class _CallerAllocLocalPtr = struct CallerAllocLocalPtr>
_CallerAllocLocalPtr * CallerAllocLocalPtr_(void **localPtr);
struct CallerAllocLocalType: CARArgumentBase
{
    Elastos::AutoPtr<IDataTypeInfo> dataTypeInfo;
    void *localTypeObject = nullptr;
protected:
    ~CallerAllocLocalType() override = default;
};

template<class _CallerAllocLocalType = struct CallerAllocLocalType>
_CallerAllocLocalType * CallerAllocLocalType_(IDataTypeInfo *dataTypeInfo, void *localTypeObject);
struct CallerAllocEnum: CARArgumentBase
{
    Elastos::Int32 *enum_ = nullptr;
protected:
    ~CallerAllocEnum() override = default;
};

template<class _CallerAllocEnum = struct CallerAllocEnum>
_CallerAllocEnum * CallerAllocEnum_(Elastos::Int32 *enum_);
struct CallerAllocCARArray: CARArgumentBase
{
    Elastos::AutoPtr<ICarArrayInfo > carArrayInfo;
    Elastos::AutoPtr<IVariableOfCarArray> variableOfCARArray;
protected:
    ~CallerAllocCARArray() override = default;
};

template<class _CallerAllocCARArray = struct CallerAllocCARArray>
_CallerAllocCARArray * CallerAllocCARArray_(ICarArrayInfo *carArrayInfo, IVariableOfCarArray *variableOfCARArray);
struct CallerAllocStruct: CARArgumentBase
{
    Elastos::AutoPtr<IStructInfo> structInfo;
    Elastos::AutoPtr<IVariableOfStruct> variableOfStruct;
protected:
    ~CallerAllocStruct() override = default;
};

template<class _CallerAllocStruct = struct CallerAllocStruct>
_CallerAllocStruct * CallerAllocStruct_(IStructInfo *structInfo, IVariableOfStruct *variableOfStruct);
struct CallerAllocInterface: CARArgumentBase
{
    Elastos::AutoPtr<IInterfaceInfo> interfaceInfo;
    IInterface **interface_ = nullptr;
protected:
    ~CallerAllocInterface() override = default;
};

template<class _CallerAllocInterface = struct CallerAllocInterface>
_CallerAllocInterface * CallerAllocInterface_(IInterfaceInfo *interfaceInfo, IInterface **interface_);

CAR_BRIDGE_NAMESPACE_END

#include "car-arguments-cc.h"

#endif
