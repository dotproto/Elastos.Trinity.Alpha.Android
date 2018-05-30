#ifndef __VAR_CONVERTER_H__
#define __VAR_CONVERTER_H__

#include <v8.h>
#include <elastos.h>

using v8::Local;
using v8::Value;
#define MAX_SYMBOL_LEN               128
//=================assist function ========================================//
void ReportNanNumberError();
double jsVal2DoubleAssit(Local<Value> jsv);
unsigned short double2Uint16Assit(double d);

//=================JS 2 CAR ========================================//
ECode jsVal2Int16(Local<Value> jsv, Int16 *val);
ECode jsVal2Int32(Local<Value> jsv, Int32 *val);
ECode jsVal2Int64(Local<Value> jsv, Int64 *val);
ECode jsVal2Byte(Local<Value> jsv, Byte *val);
ECode jsVal2Float(Local<Value> jsv, Float *val);
ECode jsVal2Double(Local<Value> jsv, Double *val);
#if 0
ECode jsVal2AChar(Local<Value> jsv, AChar *val);
ECode jsVal2WChar(Local<Value> jsv, WChar *val);
ECode jsVal2AString(Local<Value> jsv, char **as);
ECode jsVal2WString(Local<Value> jsv, wchar_t **ws);
ECode jsVal2Boolean(Local<Value> jsv, Boolean *val);
ECode jsVal2ECode(Local<Value> jsv, ECode *val);

ECode jsVal2EMuid(Local<Value>  jsv, EMuid *val);
ECode jsVal2EGuid(Local<Value>  jsv, EGuid *val);
ECode jsVal2Array(Local<Value>  jsv, ICarArrayInfo *pIArrayInfo,  IVariableOfCarArray **variable);
ECode jsVal2Struct(Local<Value>  jsv, IStructInfo *pIStructInfo, IVariableOfStruct **variable);
ECode jsVal2Interface(Local<Value>  jsv, IInterfaceInfo *pInterfaceInfo, PInterface *object);
//ECode jsVal2Variant(Local<Value>  jsv, Variant * value);

//=================CAR 2 js ========================================//
ECode Array2JsVal(ICarArrayInfo *pIArrayInfo, IVariableOfCarArray *variable, Local<Value> jsv);
ECode Array2JsVal(IDataTypeInfo *typeInfo, PCarQuintet value, Local<Value> jsv);
ECode Struct2JsVal(IStructInfo *pIStructInfo, IVariableOfStruct *variable, Local<Value> jsv);
ECode Struct2JsVal(IDataTypeInfo *typeInfo, PVoid value, Local<Value> jsv);
ECode Interface2JsVal(PInterface object, Local<Value> jsv);
ECode EMuid2JsVal(EMuid *val, Local<Value> jsv);
ECode EGuid2JsVal(EGuid *val, Local<Value> jsv);
ECode AString2JsVal(AString str, Local<Value> jsv);
ECode AStringN2JsVal(AString str, Int32 n, Local<Value> jsv);
//ECode Variant2jsVal(const Variant& value, Local<Value> jsv);
#endif // 0
#endif //__VAR_CONVERTER_H__