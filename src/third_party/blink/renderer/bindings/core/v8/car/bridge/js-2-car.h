#ifndef __CAR_BRIDGE_JS_2_CAR_H
#define __CAR_BRIDGE_JS_2_CAR_H

#include <nan.h>
#include <elastos.h>
#include "macros.h"

CAR_BRIDGE_NAMESPACE_BEGIN

extern bool IsInt16(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInt16(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int16 ToInt16(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Int16 i16);

extern bool IsInt32(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInt32(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int32 ToInt32(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Int32 i32);

extern bool IsInt64(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInt64(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int64 ToInt64(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Int64 i64);

extern bool IsByte(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsByte(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Byte ToByte(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Byte byte);

extern bool IsFloat(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsFloat(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Float ToFloat(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Float f);

extern bool IsDouble(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsDouble(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Double ToDouble(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Double d);

extern bool IsChar32(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsChar32(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Char32 ToChar32(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Char32 c32);

extern bool IsString(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsString(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToString(_ELASTOS String &s, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS String const &s);

extern bool IsBoolean(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsBoolean(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Boolean ToBoolean(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValueFromBoolean(_ELASTOS Boolean b);

extern bool IsEMuid(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsEMuid(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToEMuid(_ELASTOS EMuid *id, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS EMuid *id);

extern bool IsEGuid(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsEGuid(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToEGuid(_ELASTOS EGuid *id, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS EGuid *id);

extern bool IsECode(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsECode(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS ECode ToECode(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValueFromECode(_ELASTOS ECode ecode);

extern bool IsLocalPtr(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsLocalPtr(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void *ToLocalPtr(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(void *localPtr);

# if 0
extern bool IsLocalType(IDataTypeInfo  *dataTypeInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsLocalType(IDataTypeInfo  *dataTypeInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern void ToLocalType(IDataTypeInfo  *dataTypeInfo, void *localTypeObject, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(IDataTypeInfo  *dataTypeInfo, void  *localTypeObject);

# endif
extern bool IsEnum(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsEnum(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int32 ToEnum(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValueFromEnum(_ELASTOS Int32 enum_);

extern bool IsCARArray(ICarArrayInfo *carArrayInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsCARArray(ICarArrayInfo *carArrayInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern void ToCARArray(ICarArrayInfo *carArrayInfo,
        ICarArraySetter *carArraySetter,
        ::v8::Local<::v8::Value> value);

extern void ToCARArray(ICarArrayInfo *carArrayInfo,
        _ELASTOS CarQuintet *carQuintet,
        ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(ICarArrayInfo *carArrayInfo, ICarArrayGetter *carArrayGetter);

extern ::v8::Local<::v8::Value> ToValue(ICarArrayInfo *carArrayInfo, _ELASTOS CarQuintet *carQuintet);

extern bool IsCPPVector(ICppVectorInfo *cppVectorInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsCPPVector(ICppVectorInfo *cppVectorInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern void ToCPPVector(ICppVectorInfo *cppVectorInfo,
        ICppVectorSetter *cppVectorSetter,
        ::v8::Local<::v8::Value> value);

# if 0
extern void ToCPPVector(ICppVectorInfo  *cppVectorInfo, void *cppVector, ::v8::Local<::v8::Value> value);

# endif
extern ::v8::Local<::v8::Value> ToValue(ICppVectorInfo  *cppVectorInfo, ICppVectorGetter  *cppVectorGetter);

# if 0
extern ::v8::Local<::v8::Value> ToValue(ICppVectorInfo  *cppVectorInfo, void  *cppVector);

# endif
extern bool IsStruct(IStructInfo  *structInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsStruct(IStructInfo *structInfo, ::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToStruct(IStructInfo *structInfo, IStructSetter *structSetter, ::v8::Local<::v8::Value> value);

extern void ToStruct(IStructInfo *structInfo, void *struct_, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(IStructInfo  *structInfo, IStructGetter  *structGetter);

extern ::v8::Local<::v8::Value> ToValue(IStructInfo  *structInfo, void  *struct_);

extern bool IsInterface(IInterfaceInfo  *interfaceInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInterface(IInterfaceInfo  *interfaceInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern _ELASTOS AutoPtr<IInterface> ToInterface(IInterfaceInfo  *interfaceInfo, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(IInterface *interface_);

extern bool Is(IDataTypeInfo  *dataTypeInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAs(IDataTypeInfo  *dataTypeInfo, ::v8::Local<::v8::Value> value, int *priority = nullptr);

extern bool IsCARObject(::v8::Local<::v8::Value> value);

extern IInterface *AsCARObject(::v8::Local<::v8::Value> value);

extern bool IsGeneric(::v8::Local<::v8::Value> value);

extern bool IsRegime(::v8::Local<::v8::Value> value);

extern IRegime *AsRegime(::v8::Local<::v8::Value> value);

extern bool IsAspect(::v8::Local<::v8::Value> value);

extern IAspect *AsAspect(::v8::Local<::v8::Value> value);

CAR_BRIDGE_NAMESPACE_END

#endif
