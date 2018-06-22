#ifndef __CAR_BRIDGE_CAR_DATA_TYPE_H
#define __CAR_BRIDGE_CAR_DATA_TYPE_H

#include <nan.h>
#include <elastos.h>
#include "macros.h"

CAR_BRIDGE_NAMESPACE_BEGIN

extern v8::Local<v8::Object> CARLocalPtr(ILocalPtrInfo *localPtrInfo);
extern v8::Local<v8::Object> CARLocalType(IDataTypeInfo *dataTypeInfo);
extern v8::Local<v8::Object> CAREnum(IEnumInfo *enumInfo);
extern v8::Local<v8::Object> CARArray(ICarArrayInfo *carArrayInfo);
extern v8::Local<v8::Object> CARCPPVector(ICppVectorInfo *cppVectorInfo);
extern v8::Local<v8::Object> CARStruct(IStructInfo *structInfo);
extern v8::Local<v8::Object> CARInterface(IInterfaceInfo *interfaceInfo);

typedef IInterfaceInfo ICallbackInterfaceInfo;
extern v8::Local<v8::Object> CARCallbackInterface(ICallbackInterfaceInfo *callbackInterfaceInfo);
extern v8::Local<v8::Object> CARDataType(IDataTypeInfo *dataTypeInfo);

CAR_BRIDGE_NAMESPACE_END

#endif
