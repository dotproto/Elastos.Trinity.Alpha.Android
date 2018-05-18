#include <elastos.h>

#include "var_converter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
#define PUSH_ARRAY_ELEMENT(type) do { /*TypeArray CppVertor*/\
    for (i = 0; i < arrayLen; i++) { \
        type val; \
        char strProperty[5] = {0};\
        JSStringRef strPropertyName;\
        _itoa(i,strProperty,10);\
        strPropertyName = JSStringCreateWithUTF8CString(strProperty);\
        jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName,NULL);\
        ec = jsVal2##type(cx, jsvalue, &val); \
        if (FAILED(ec)) goto ERR; \
        ec = setter->Set##type##Element(i, val); \
        if (FAILED(ec)) goto ERR; \
    }\
}while(0)

#define PUSH_STRUCT_FIELD(type) do { \
    type val; \
    ec = jsVal2##type(cx, jsvalue, &val); \
    if (FAILED(ec)) goto ERR; \
    setter->Set##type##Field(name, val); \
    if (FAILED(ec)) goto ERR; \
}while(0)
#endif

#if 0
static ECode _cppVector2jsVal(ICppVectorInfo *pICppVectorInfo,
                              ICppVectorGetter *getter, Local<Value> jsv);
static ECode _jsVal2CppVector(Local<Value> jsv,
                              ICppVectorInfo *pICppVectorInfo, ICppVectorSetter *setter);

static ECode _struct2jsVal(IStructInfo *pIStructInfo,
                           IStructGetter *getter, Local<Value> jsv);
static ECode _jsVal2Struct(Local<Value> jsv,
                           IStructInfo *pIStructInfo, IStructSetter *setter);
#endif

//===================================assit function ==================//
void ReportNanNumberError()
{
    printf("The param convert Failed,the jsvalue is not a number type!\n");
}

double jsVal2DoubleAssit(Local<Value> jsv)
{
    if(!jsv->IsNumber()) {
        ReportNanNumberError();
        return E_INVALID_ARGUMENT;
    }

    double d = jsv->NumberValue();
    return d;
}

unsigned short double2Uint16Assit(double d)
{
    unsigned short v;
    unsigned long i, m;
    int neg;
    if(d == 0) { //||!JSDOUBLE_IS_FINITE(d))
        v = 0;
        return v;
    }
    i = (unsigned long)d;
    if ((double)i == d) {
        v = (unsigned short)i;
        return v;
    }
    neg = (d < 0);
    d = floor(neg ? -d : d);
    d = neg ? -d : d;
    m = ((unsigned long)1 << (16));
    d = fmod(d, (double)m);
    if (d < 0)
        d += m;
    v = (unsigned short) d;
    return v;
}


//=================javascript 2 CAR ========================================//
ECode jsVal2Int16(Local<Value> jsv, Int16 *val)
{
    if (!val) return E_INVALID_ARGUMENT;

    double d = jsVal2DoubleAssit(jsv);
    unsigned short v = double2Uint16Assit(d);
    *val = v;
    return NOERROR;
}

ECode jsVal2Int32(Local<Value> jsv, Int32 *val)
{
    if (!val) return E_INVALID_ARGUMENT;

    double d = jsVal2DoubleAssit(jsv);
    if(d <= -2147483648.0 || 2147483647.0 < d) { //depend on the range of Int32 in CAR
        return E_INVALID_ARGUMENT;
    }
    *val = (Int32)floor(d + 0.5);
    return NOERROR;
}

ECode jsVal2Int64(Local<Value> jsv, Int64 *val)
{
    if (!val) return E_INVALID_ARGUMENT;

    double d = jsVal2DoubleAssit(jsv);
    *val = (Int64)d;
    return NOERROR;
}

ECode jsVal2Byte(Local<Value> jsv, Byte *val)
{
    if (!val) return E_INVALID_ARGUMENT;

    double d = jsVal2DoubleAssit(jsv);
    unsigned short v = double2Uint16Assit(d);
    if(v > 255)
        return E_INVALID_ARGUMENT;
    *val = (Byte)v;
    return NOERROR;
}

ECode jsVal2Float(Local<Value> jsv, Float *val)
{
    if (!val) return E_INVALID_ARGUMENT;

    double d = jsVal2DoubleAssit(jsv);
    *val = (Float)d;
    return NOERROR;
}

ECode jsVal2Double(Local<Value> jsv, Double *val)
{
    if (!val) return E_INVALID_ARGUMENT;

    *val = (Double)jsVal2DoubleAssit(jsv);
    return NOERROR;
}
#if 0
ECode jsVal2AChar(Local<Value> jsv, AChar *val)
{
    if (!val || (!JSValueIsString(jsv) && !JSValueIsNumber(jsv)))
        return E_INVALID_ARGUMENT;

    if(JSValueIsString(jsv)) {
        Local<Value> *exception = NULL;
        JSStringRef jsStr = JSValueToStringCopy(jsv, exception);
        if(exception != NULL)
            return E_INVALID_ARGUMENT;
        if(JSStringGetLength(jsStr) == 1) {
            //const char* p = (const char*)JSStringGetCharactersPtr(jsStr);
            char buf[4];
            memset(buf, 0, 2);
            JSStringGetUTF8CString(jsStr, buf, 4);
            *val = (AChar) * buf;
            return NOERROR;
        }
    }
    if(JSValueIsNumber(jsv)) {
        double d = jsVal2DoubleAssit(jsv);
        unsigned short v = double2Uint16Assit(d);
        *val = (AChar)v;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode jsVal2WChar(Local<Value> jsv, WChar *val)
{
    if (!val || (!JSValueIsString(jsv) && !JSValueIsNumber(jsv)))
        return E_INVALID_ARGUMENT;

    if(JSValueIsString(jsv)) {
        Local<Value> *exception = NULL;
        JSStringRef jsStr = JSValueToStringCopy(jsv, exception);
        if(exception != NULL)
            return E_INVALID_ARGUMENT;

        Int32 len = (Int32)JSStringGetLength(jsStr);
        char buf[8];
        memset(buf, 0, 8);
        JSStringGetUTF8CString(jsStr, buf, 8);
        //Int32 ret = mbtowc((wchar_t*)val,buf,len);//has some problem?
        //if(len == ret) //has some problem?
        return NOERROR;
    }

    if(JSValueIsNumber(jsv)) {
        double d = jsVal2DoubleAssit(jsv);
        unsigned short v = double2Uint16Assit(d);
        *val = (WChar)v;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode jsVal2AString(Local<Value> jsv, char **as)
{
    if (!as || (!JSValueIsNull(jsv) && !JSValueIsString(jsv)))
        return E_INVALID_ARGUMENT;

    if(JSValueIsNull(jsv)) {
        *as = NULL;
        return NOERROR;
    }

    //JSvalueIsObject
    /*
    char * p = (char *)JS_GetInstancePrivate(cx, JSVAL_TO_OBJECT(jsv),
    &LocaleIDItem_Class, NULL);
    if (p) {
    int len = strlen(p);
    len += 3;
    char * str = (char *)malloc(len * sizeof(char));
    if (!str) {
    *as = NULL;
    return E_OUT_OF_MEMORY;
    }
    strcpy(str, p);
    str[len - 1] = 0;
    str[len - 2] = 0;
    *as = str;
    return NOERROR;
    }
    */
    if(JSValueIsString(jsv)) {
        Local<Value> *exception = NULL;
        JSStringRef jsStr = JSValueToStringCopy(jsv, exception);
        if(exception != NULL)
            return E_INVALID_ARGUMENT;

        int len = (int)JSStringGetLength(jsStr);
        *as = (char *)malloc((len + 1) * sizeof(char));
        if(*as == NULL) E_OUT_OF_MEMORY;

        char buf[128];
        memset(buf, 0, 128);
        JSStringGetUTF8CString(jsStr, buf, 128);
        strncpy((char *)*as, buf, len + 1);
        return NOERROR;
    }

    return E_FAIL;
}

ECode jsVal2WString(Local<Value> jsv, wchar_t **ws)
{
    if (!ws || (!JSValueIsNull(jsv) && !JSValueIsString(jsv)))
        return E_INVALID_ARGUMENT;

    if (JSValueIsNull(jsv)) {
        *ws = NULL;
        return NOERROR;
    }
    /*
    char * p = (char *)JS_GetInstancePrivate(cx, JSVAL_TO_OBJECT(jsv),
    &LocaleIDItem_Class, NULL);
    if (p) {
    int len = strlen(p);
    len += 3;
    char * str = (char *)malloc(len * sizeof(char));
    if (!str) {
    *ws = NULL;
    return E_OUT_OF_MEMORY;
    }
    strcpy(str, p);
    str[len - 1] = 0;
    str[len - 2] = 0;
    *ws = (wchar_t *)str;
    return NOERROR;
    }
    */
    if(JSValueIsString(jsv)) {
        Local<Value> *exception = NULL;
        JSStringRef jsStr = JSValueToStringCopy(jsv, exception);
        if(exception != NULL)
            return E_INVALID_ARGUMENT;

        int len = (int)JSStringGetLength(jsStr);
        *ws = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
        if(*ws == NULL) E_OUT_OF_MEMORY;

        char buf[1024];
        memset(buf, 0, 1024);
        JSStringGetUTF8CString(jsStr, buf, 1024);

        //    mbstowcs((wchar_t*)*ws, buf, len + 1);
        WStringBuf_<1024> temp;
        temp.Copy(buf, Encoding_UTF8);
        wcscpy(*ws, (wchar_t *)temp);

        return NOERROR;
    }

    return E_FAIL;
}



ECode jsVal2EMuid(Local<Value> jsv, EMuid *val)
{
    if (!cx || JSValueIsNull(cx, jsv) || !val) return E_INVALID_ARGUMENT;

    JSObjectRef pJSObj, pJSObj2;
    Local<Value> jsvalue;
    JSStringRef strPropertyName;

    if (NULL == (pJSObj = JSValueToObject(cx, jsv, NULL))) {
        return E_INVALID_ARGUMENT;
    }

    char propertyName[6];
    memcpy(propertyName, "Data0", sizeof("Data0"));
    propertyName[5] = 0;

    Int32 i;
    Int16 *Data = (Int16 *)&val->Data2;
    ECode ec;
    for (i = 0; i < 3; i++) {//Data 1 - 3
        propertyName[4] = propertyName[4] + 1;
        strPropertyName = JSStringCreateWithUTF8CString(propertyName);
        if (NULL == (jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL))
                || NULL == (pJSObj2 = JSValueToObject(cx, jsvalue, NULL))) {
            return E_INVALID_ARGUMENT;
        }
        if (0 == i) {
            ec = jsVal2Int32(cx, jsvalue, (Int32 *)&val->Data1);
        } else {
            ec = jsVal2Int16(cx, jsvalue, &Data[i - 1]);
        }
        if (FAILED(ec)) return ec;
    }
    strPropertyName = JSStringCreateWithUTF8CString("Data4");
    if (NULL == (jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL))
            || NULL == (pJSObj2 = JSValueToObject(cx, jsvalue, NULL))) {
        return E_INVALID_ARGUMENT;
    }

    UInt8 *Data4 = val->Data4;
    Int16 v;
    char strProperty[5] = {0};
    for (i = 0; i < 8; i++) {
        _itoa(i, strProperty, 10);
        strPropertyName = JSStringCreateWithUTF8CString(strProperty);
        if (NULL == (jsvalue = JSObjectGetProperty(cx, pJSObj2, strPropertyName, NULL))) {
            return E_INVALID_ARGUMENT;
        }
        ec = jsVal2Int16(cx, jsvalue, &v);
        if (FAILED(ec)) return ec;
        Data4[i] = (UInt8)v;
    }

    return NOERROR;
}

ECode jsVal2EGuid(Local<Value> jsv, EGuid *val)
{
    if (!cx || (NULL == jsv) || !val) return E_INVALID_ARGUMENT;

    JSObjectRef pJSObj;
    Local<Value> jsvalue;
    JSStringRef strPropertyName;

    if (NULL == (pJSObj = JSValueToObject(cx, jsvalue, NULL))) {
        return E_INVALID_ARGUMENT;
    }

    strPropertyName = JSStringCreateWithUTF8CString("clsid");
    if (NULL == (jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL))) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = jsVal2EMuid(cx, jsvalue, &val->clsid);
    if (FAILED(ec)) return ec;

    strPropertyName = JSStringCreateWithUTF8CString("pUunm");
    if (NULL == (jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL))) {
        return E_INVALID_ARGUMENT;
    }
    ec = jsVal2WString(cx, jsvalue, (wchar_t **)&val->pUunm);
    if (FAILED(ec)) return ec;

    return NOERROR;
}


ECode jsVal2Boolean(Local<Value> jsv, Boolean *val)
{

    if (!val || (!JSValueIsBoolean(jsv) && !JSValueIsNumber(jsv)))
        return E_INVALID_ARGUMENT;

    bool b = JSValueToBoolean(jsv);
    *val = (Boolean)b;
    return NOERROR;
}

ECode jsVal2ECode(Local<Value> jsv, ECode *val)
{
    Int32 value;
    ECode ec = jsVal2Int32(jsv, &value);
    if (FAILED(ec)) return ec;

    *val = (ECode)value;
    return NOERROR;
}

//extern JSClass LocaleIDItem_Class;

ECode jsVal2Array(Local<Value> jsv, ICarArrayInfo *pIArrayInfo,
                  IVariableOfCarArray **variable)
{
    if (!cx || (NULL == jsv) || !pIArrayInfo || !variable) {
        return E_INVALID_ARGUMENT;
    }

    JSObjectRef  pJSObj = NULL;
    if (NULL == (pJSObj = JSValueToObject(cx, jsv, NULL))) {
        return E_INVALID_ARGUMENT;
    }

    IDataTypeInfo *elementTypeInfo;
    CarDataType dataType;

    pIArrayInfo->GetElementTypeInfo(&elementTypeInfo);
    elementTypeInfo->GetDataType(&dataType);

    if (dataType == CarDataType_Byte
            || dataType == CarDataType_AChar
            || dataType == CarDataType_WChar) {
        elementTypeInfo->Release();
        return E_NOT_IMPLEMENTED;
    }

    Int32 i;
    Local<Value> jsvalue;
    Int32 arrayLen;
    ICarArraySetter *setter = NULL;

    JSStringRef   string = JSStringCreateWithUTF8CString("length");
    Local<Value>    jsLen = JSObjectGetProperty(cx, pJSObj, string, NULL);
    arrayLen = 0;// (unsigned long *)JSValueToNumber(cx,jsLen,NULL);//?

    ECode ec = pIArrayInfo->CreateVariable(arrayLen, variable);
    if (FAILED(ec)) {
        elementTypeInfo->Release();
        return ec;
    }
    ec = (*variable)->GetSetter(&setter);
    if (FAILED(ec)) goto ERR;

    switch(dataType) {
    case CarDataType_Int16:
        PUSH_ARRAY_ELEMENT(Int16);
        break;
    case CarDataType_Int32:
        PUSH_ARRAY_ELEMENT(Int32);
        break;
    case CarDataType_Int64:
        PUSH_ARRAY_ELEMENT(Int64);
        break;
    case CarDataType_Float:
        PUSH_ARRAY_ELEMENT(Float);
        break;
    case CarDataType_Double:
        PUSH_ARRAY_ELEMENT(Double);
        break;
    case CarDataType_AString:
        for (i = 0; i < arrayLen; i++) {
            char *val;
            char strProperty[5] = {0};
            JSStringRef strPropertyName;
            _itoa(i, strProperty, 10);
            strPropertyName = JSStringCreateWithUTF8CString(strProperty);
            jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL);
            ec = jsVal2AString(cx, jsvalue, &val);
            if (FAILED(ec)) goto ERR;
            ec = setter->SetAStringElement(i, val);
            if (FAILED(ec)) goto ERR;
        }
        break;
    case CarDataType_WString:
        for (i = 0; i < arrayLen; i++) {
            wchar_t *val;
            char strProperty[5] = {0};
            JSStringRef strPropertyName;
            _itoa(i, strProperty, 10);
            strPropertyName = JSStringCreateWithUTF8CString(strProperty);
            jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL);
            ec = jsVal2WString(cx, jsvalue, &val);
            if (FAILED(ec)) goto ERR;
            ec = setter->SetWStringElement(i, val);
            if (FAILED(ec)) goto ERR;
        }
        break;
    case CarDataType_Boolean:
        PUSH_ARRAY_ELEMENT(Boolean);
        break;
    case CarDataType_EMuid:
    case CarDataType_EGuid:
    case CarDataType_ECode:
        assert(0 && "not implement");
        break;
        //        case CarDataType_Enum: {
        //            for (i = 0; i < arrayLen; i++) {
        //                Int32 val;
        //                jsvalue = JSObjectGetProperty(cx, pJSObj,JSValueMakeNumber(cx,i));
        //                ec = jsVal2Int32(cx, jsvalue, &val);
        //                if (FAILED(ec)) goto ERR;
        //                setter->SetEnumElement(i, val);
        //                if (FAILED(ec)) goto ERR;
        //            }
        //            break;
        //        }
    case CarDataType_Struct: {
        for (i = 0; i < arrayLen; i++) {
            IStructSetter *InnerSetter;
            IStructInfo *piStructInfo;
            piStructInfo = IStructInfo::Probe(elementTypeInfo);
            if (!piStructInfo) ec = E_NO_INTERFACE;
            else ec = NOERROR;
            if (FAILED(ec)) goto ERR;

            char strProperty[5] = {0};
            JSStringRef strPropertyName;
            _itoa(i, strProperty, 10);
            strPropertyName = JSStringCreateWithUTF8CString(strProperty);
            jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL);

            ec = setter->GetStructElementSetter(i, &InnerSetter);
            if (FAILED(ec)) goto ERR;
            ec = _jsVal2Struct(cx, jsvalue, piStructInfo, InnerSetter);
            InnerSetter->Release();
            if (FAILED(ec)) goto ERR;
        }
        break;
    }
    case CarDataType_Interface: {
        for (i = 0; i < arrayLen; i++) {
            PInterface val;
            IInterfaceInfo *piInterfaceInfo;
            piInterfaceInfo = IInterfaceInfo::Probe(elementTypeInfo);
            if (!piInterfaceInfo) ec = E_NO_INTERFACE;
            else ec = NOERROR;
            if (FAILED(ec)) goto ERR;

            char strProperty[5] = {0};
            JSStringRef strPropertyName;
            _itoa(i, strProperty, 10);
            strPropertyName = JSStringCreateWithUTF8CString(strProperty);
            jsvalue = JSObjectGetProperty(cx, pJSObj, strPropertyName, NULL);
            ec = jsVal2Interface(cx, jsvalue, piInterfaceInfo, &val);
            if (FAILED(ec)) goto ERR;
            ec = setter->SetObjectPtrElement(i, val);
            if (FAILED(ec)) goto ERR;
        }
        break;
    }
    default:
        assert(0 && "not implement");
        break;
    }

    setter->SetUsed(arrayLen);
    elementTypeInfo->Release();
    setter->Release();

    return NOERROR;

ERR:
    elementTypeInfo->Release();
    (*variable)->Release();
    (*variable) = NULL;
    if (setter) setter->Release();
    return ec;
}

ECode jsVal2Struct(Local<Value> jsv, IStructInfo *pIStructInfo,
                   IVariableOfStruct **variable)
{
    if (!cx || (NULL == jsv) || !pIStructInfo || !variable) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = pIStructInfo->CreateVariable(variable);
    if (FAILED(ec)) return ec;
    IStructSetter *setter;

    if (1) {// for VARIANT
        AStringBuf_<256>  name;
        pIStructInfo->GetName(&name);
        if (!strcmp("VARIANT", name)) {
            //Variant * var;
            //ec = (*variable)->GetPayload((PVoid*)&var);
            //if (FAILED(ec)) return ec;

            return E_INVALID_ARGUMENT;//暂未实现

            //ec = jsVal2Variant(cx, jsv, var);
            //if (FAILED(ec)) goto ERR;
            //return NOERROR;
        }
    }

    ec = (*variable)->GetSetter(&setter);
    if (FAILED(ec)) goto ERR;
    ec = _jsVal2Struct(cx, jsv, pIStructInfo, setter);
    setter->Release();
    if (FAILED(ec)) goto ERR;
    return NOERROR;

ERR:
    (*variable)->Release();
    *variable = NULL;
    return ec;
}

ECode jsVal2Interface(Local<Value> jsv,
                      IInterfaceInfo *pInterfaceInfo, PInterface *object)
{
    if (!cx || !pInterfaceInfo || !object) {
        return E_INVALID_ARGUMENT;
    }

    //if ((NULL == jsv)) {
    //    *object = NULL;
    //    return NOERROR;
    //}

    //IInterface* piObj = GetInnerCarObject(cx, jsv);
    //if (NULL == piObj) {
    //    return E_INVALID_ARGUMENT;
    //}

    //InterfaceId iid;
    //ECode ec = NOERROR;

    //pInterfaceInfo->GetId(&iid);
    //*object = piObj->Probe(iid);
    //if (*object) (* object)->AddRef();
    //else ec = E_NO_INTERFACE;
    //piObj->Release();

    //return ec;
    return E_INVALID_ARGUMENT;//暂未实现
}

//ECode jsVal2Variant(Local<Value> jsv, Variant* value)
//{
//    if (!cx) return E_INVALID_ARGUMENT;
//
//    if ((NULL == jsv)) {
//        return NOERROR;
//    }
//    else if (JSVAL_IS_NUMBER(jsv)) {
//        jsdouble dp;
//        if (JS_TRUE == JS_ValueToNumber(cx, jsv, &dp)) {
//            (*value) = (Double)dp;
//            return NOERROR;
//        }
//        return E_FAIL;
//    }
//    else if (JSVAL_IS_STRING(jsv)) {
//        char *str = NULL;
//        ECode ec = jsVal2AString(cx, jsv, &str);
//        if (FAILED(ec)) return E_FAIL;
//        AString s = str;
//        (*value) = s;
//        return NOERROR;
//    }
//    else if (JSVAL_IS_BOOLEAN(jsv)) {
//        JSBool bp;
//        JS_ValueToBoolean(cx, jsv, &bp);
//        (*value) = (bp ? true : false);
//        return NOERROR;
//    }
//    else if (IsCarObject(cx, jsv)) {
//        (*value) = GetInnerCarObject(cx, jsv);
//        return NOERROR;
//    }
//
//    return NOERROR;
//}


//=============================CAR 2 javascript========================//

ECode Array2JsVal(IDataTypeInfo *typeInfo,
                  PCarQuintet value, Local<Value> jsv)
{
    if (!cx || !typeInfo || !value || !jsv) {
        return E_INVALID_ARGUMENT;
    }

    ICarArrayInfo *pIArrayInfo;
    ECode ec = NOERROR;
    pIArrayInfo = ICarArrayInfo::Probe(typeInfo);
    if (!pIArrayInfo) ec = E_NO_INTERFACE;
    else ec = NOERROR;
    if (FAILED(ec)) return ec;

    IVariableOfCarArray *variable;
    ec = pIArrayInfo->CreateVariableBox(value, &variable);
    if (FAILED(ec)) return ec;

    ec = Array2JsVal(cx, pIArrayInfo, variable, jsv);
    variable->Release();

    return ec;
}

ECode Array2JsVal(ICarArrayInfo *pIArrayInfo,
                  IVariableOfCarArray *variable, Local<Value> jsv)
{
    if (!cx || !pIArrayInfo || !variable || !jsv) {
        return E_INVALID_ARGUMENT;
    }

    JSObjectRef  pJSObj = JSObjectMakeArray(cx, 0, NULL, NULL);
    if (NULL == pJSObj) return E_OUT_OF_MEMORY;

    Int32 i, used;
    Local<Value>  jsvalue;
    CarDataType dataType;
    ICarArrayGetter *getter;
    IDataTypeInfo *elementTypeInfo;

    ECode ec = variable->GetGetter(&getter);
    if (FAILED(ec)) return ec;
    ec = getter->GetUsed(&used);
    if (FAILED(ec)) goto EXIT1;

    ec = pIArrayInfo->GetElementTypeInfo(&elementTypeInfo);
    if (FAILED(ec)) goto EXIT1;
    elementTypeInfo->GetDataType(&dataType);

    switch(dataType) {
    case CarDataType_Byte: {
        Byte *buff = (Byte *)_alloca(sizeof(Byte) * used);
        if (buff == NULL) {
            ec = E_OUT_OF_MEMORY;
            goto EXIT2;
        }

        for (i = 0; i < used; i++) {
            ec = getter->GetByteElement(i, &buff[i]);
            if (FAILED(ec)) goto EXIT2;
        }
        ec = AStringN2JsVal(cx, (char *)buff, used, jsv);
        if (FAILED(ec)) goto EXIT2;
        elementTypeInfo->Release();
        getter->Release();
        return NOERROR;
    }
    case CarDataType_AChar: {
        AChar *buff = (AChar *)_alloca(sizeof(AChar) * used);
        if (buff == NULL) {
            ec = E_OUT_OF_MEMORY;
            goto EXIT2;
        }

        for (i = 0; i < used; i++) {
            ec = getter->GetACharElement(i, &buff[i]);
            if (FAILED(ec)) goto EXIT2;
        }
        ec = AStringN2JsVal(cx, (AString)buff, used, jsv);
        if (FAILED(ec)) goto EXIT2;
        elementTypeInfo->Release();
        getter->Release();
        return NOERROR;
    }
    case CarDataType_WChar: {
        AChar *buff = (AChar *)_alloca(MB_LEN_MAX * used);
        if (buff == NULL) {
            ec = E_OUT_OF_MEMORY;
            goto EXIT2;
        }
        WChar wch;
        Int32 len = 0, ret;
        for (i = 0; i < used; i++) {
            ec = getter->GetWCharElement(i, &wch);
            if (FAILED(ec)) goto EXIT2;
            ret = wctomb(buff + len, wch);
            if (ret == -1) {
                ec = -1;
                goto EXIT2;
            }
            len += ret;
        }
        ec = AStringN2JsVal(cx, (AString)buff, len, jsv);
        if (FAILED(ec)) goto EXIT2;
        elementTypeInfo->Release();
        getter->Release();
        return NOERROR;
    }
    case CarDataType_Int16: {
        Int16 val;
        for (i = 0; i < used; i++) {
            ec = getter->GetInt16Element(i, &val);
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeNumber(cx, val), NULL);
        }
        break;
    }
    case CarDataType_Int32: {
        Int32 val;
        for (i = 0; i < used; i++) {
            ec = getter->GetInt32Element(i, &val);
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeNumber(cx, val), NULL);
        }
        break;
    }
    case CarDataType_Int64: {
        Int64 val;
        for (i = 0; i < used; i++) {
            ec = getter->GetInt64Element(i, &val);
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeNumber(cx, val), NULL);
        }
        break;
    }
    case CarDataType_Float: {
        Float val;
        for (i = 0; i < used; i++) {
            ec = getter->GetFloatElement(i, &val);
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeNumber(cx, val), NULL);
        }
        break;
    }
    case CarDataType_Double: {
        Double val;
        for (i = 0; i < used; i++) {
            ec = getter->GetDoubleElement(i, &val);
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeNumber(cx, val), NULL);
        }
        break;
    }
    case CarDataType_AString: {
        AString val;
        JSStringRef jsStr;
        for (i = 0; i < used; i++) {
            ec = getter->GetAStringElement(i, &val);
            if (FAILED(ec)) goto EXIT2;
            jsStr = JSStringCreateWithUTF8CString(val);
            if (NULL == jsStr) {
                ec = E_OUT_OF_MEMORY;
                goto EXIT2;
            }
            jsStr = JSStringCreateWithUTF8CString(val);
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeString(cx, jsStr), NULL);
        }
        break;
    }
    case CarDataType_WString: {
        WString val;
        JSStringRef jsStr;
        for (i = 0; i < used; i++) {
            ec = getter->GetWStringElement(i, &val);
            if (FAILED(ec)) goto EXIT2;
            char tempStr[256] = {0};
            mbtowc((wchar_t *)(const wchar_t *)val, tempStr, 0);
            jsStr = JSStringCreateWithUTF8CString(tempStr);
            if (NULL == jsStr) {
                ec = E_OUT_OF_MEMORY;
                goto EXIT2;
            }
            JSObjectSetPropertyAtIndex(cx, pJSObj, i,  JSValueMakeString(cx, jsStr), NULL);
        }
        break;
    }
    case CarDataType_Boolean: {
        Boolean val;
        for (i = 0; i < used; i++) {
            ec = getter->GetBooleanElement(i, &val);
            if (FAILED(ec)) goto EXIT2;
            if (val) {
                jsvalue = JSValueMakeBoolean(cx, true);
            } else {
                jsvalue = JSValueMakeBoolean(cx, false);
            }
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, jsvalue, NULL);
        }
        break;
    }
    case CarDataType_Enum : {
        Int32 val;
        for (i = 0; i < used; i++) {
            //ec = getter->GetEnumElement(i, &val);
            //if (FAILED(ec)) goto EXIT2;
            //JSObjectSetPropertyAtIndex(cx, pJSObj, i, JSValueMakeNumber(cx, val), NULL);
        }
        break;
    }
    case CarDataType_Struct: {
        for (i = 0; i < used; i++) {
            IStructGetter *innerGetter;
            IStructInfo *piStructInfo;
            piStructInfo = IStructInfo::Probe(elementTypeInfo);
            if (!piStructInfo) ec = E_NO_INTERFACE;
            else ec = NOERROR;
            if (FAILED(ec)) goto EXIT2;

            ec = getter->GetStructElementGetter(i, &innerGetter);
            if (FAILED(ec)) goto EXIT2;
            ec = _struct2jsVal(cx, piStructInfo, innerGetter, jsvalue);
            innerGetter->Release();
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, jsvalue, NULL);
        }
        break;
    }
    case CarDataType_Interface: {
        for (i = 0; i < used; i++) {
            PInterface val;
            ec = getter->GetObjectPtrElement(i, &val);
            if (FAILED(ec)) goto EXIT2;
            ec = Interface2JsVal(cx, val, jsvalue);
            if (FAILED(ec)) goto EXIT2;
            JSObjectSetPropertyAtIndex(cx, pJSObj, i, jsvalue, NULL);
        }
        break;
    }
    default:
        assert(0 && "not implement");
        break;
    }

    jsv = pJSObj;
    ec = NOERROR;
EXIT2:
    elementTypeInfo->Release();
EXIT1:
    getter->Release();
    return ec;
}

ECode Struct2JsVal(IDataTypeInfo *typeInfo,
                   PVoid value, Local<Value> jsv)
{
    if (!cx || !typeInfo || !value || !jsv) {
        return E_INVALID_ARGUMENT;
    }

    IStructInfo *pIStructInfo;
    pIStructInfo = IStructInfo::Probe(typeInfo);
    if (!(pIStructInfo)) return E_NO_INTERFACE;
    pIStructInfo->AddRef();
    typeInfo->Release();

    IVariableOfStruct *variable;
    ECode ec = pIStructInfo->CreateVariableBox(value, &variable);
    if (FAILED(ec)) return ec;

    ec = Struct2JsVal(cx, pIStructInfo, variable, jsv);
    variable->Release();
    return ec;
}

ECode Struct2JsVal(IStructInfo *pIStructInfo,
                   IVariableOfStruct *variable, Local<Value> jsv)
{
    if (!cx || !pIStructInfo || !variable || !jsv) {
        return E_INVALID_ARGUMENT;
    }

    if (1) {// for VARIANT
        AStringBuf_<256>  name;
        pIStructInfo->GetName(&name);
        if (!strcmp("VARIANT", name)) {
            struct VARIANT *var;
            ECode ec = variable->GetPayload((PVoid *)&var);
            if (FAILED(ec)) return ec;
            return E_INVALID_ARGUMENT;
            //return Variant2jsVal(cx, *var, jsv);
        }
    }

    IStructGetter *getter;
    ECode ec = variable->GetGetter(&getter);
    if (FAILED(ec)) return ec;

    ec = _struct2jsVal(cx, pIStructInfo, getter, jsv);
    getter->Release();
    return ec;
}

ECode Interface2JsVal(PInterface object, Local<Value> jsv)
{
    //return NewJSObject(cx, object, jsv);
    return E_NOT_IMPLEMENTED;//暂未实现
}

ECode EMuid2JsVal(EMuid *val, Local<Value> jsv)
{
    if (!cx || !jsv) {
        return E_INVALID_ARGUMENT;
    }
    if (NULL == val) {
        jsv = NULL;
        return NOERROR;
    }
    JSStringRef objceIString;

    //JSObjectRef pRetObj = JSObjectMake(cx, &js_ObjectClass, NULL);
    JSObjectRef pRetObj = JSObjectMake(cx, JSClassCreate(&kJSClassDefinitionEmpty), NULL);
    if (NULL == pRetObj) return E_OUT_OF_MEMORY;
    JSObjectRef  pJSArray = JSObjectMakeArray(cx, 0, NULL, NULL);
    if (NULL == pJSArray) return E_OUT_OF_MEMORY;

    Local<Value> rval;

    rval = JSValueMakeNumber(cx, val->Data1);
    objceIString = JSStringCreateWithUTF8CString("Data1");
    JSObjectSetProperty(cx, pRetObj, objceIString, rval, kJSPropertyAttributeNone, NULL);

    rval = JSValueMakeNumber(cx, val->Data2);
    objceIString = JSStringCreateWithUTF8CString("Data2");
    JSObjectSetProperty(cx, pRetObj, objceIString, rval, kJSPropertyAttributeNone, NULL);

    rval = JSValueMakeNumber(cx, val->Data3);
    objceIString = JSStringCreateWithUTF8CString("Data3");
    JSObjectSetProperty(cx, pRetObj, objceIString, rval, kJSPropertyAttributeNone, NULL);

    for (Int32 i = 0; i < 8; i++) {
        rval = JSValueMakeNumber(cx, val->Data4[i]);
        JSObjectSetPropertyAtIndex(cx, pRetObj, i, rval, NULL);
    }

    rval = pJSArray;
    objceIString = JSStringCreateWithUTF8CString("Data4");
    JSObjectSetProperty(cx, pRetObj, objceIString, rval, kJSPropertyAttributeNone, NULL);

    jsv = pRetObj;
    return NOERROR;
}

ECode EGuid2JsVal(EGuid *val, Local<Value> jsv)
{
    //if (!cx || !jsv) {
    //    return E_INVALID_ARGUMENT;
    //}
    //if (NULL == val) {
    //    jsv = NULL;
    //    return NOERROR;
    //}
    //JSStringRef objceIString;

    ////JSObjectRef pRetObj = JSObjectMake(cx, &js_ObjectClass, NULL);
    ////if (NULL == pRetObj) return E_OUT_OF_MEMORY;

    //Local<Value> rval;

    //ECode ec = EMuid2JsVal(cx, &val->clsid, &rval);
    //if (FAILED(ec)) return ec;
    //objceIString = JSStringCreateWithUTF8CString("clsid");
    //JSObjectSetProperty(cx, pRetObj, objceIString, rval, kJSPropertyAttributeNone, NULL);

    //ec = AString2JsVal(cx, (char *)CW2A(val->pUunm), &rval);
    //if (FAILED(ec)) return ec;
    //objceIString = JSStringCreateWithUTF8CString("pUunm");
    //JSObjectSetProperty(cx, pRetObj, objceIString, rval, kJSPropertyAttributeNone, NULL);

    //jsv = pRetObj;
    //return NOERROR;
    return E_NOT_IMPLEMENTED;
}

ECode AString2JsVal(AString str, Local<Value> jsv)
{
    if (!cx || !jsv) {
        return E_INVALID_ARGUMENT;
    }

    if (str.IsNull()) {
        jsv = JSValueMakeNull(cx);
        return NOERROR;
    }
    JSStringRef objceIString = JSStringCreateWithUTF8CString(str);
    Local<Value> jsStr = JSValueMakeString(cx, objceIString);
    if (NULL == jsStr) {
        return E_OUT_OF_MEMORY;
    }

    jsv = jsStr;
    return NOERROR;
}

ECode AStringN2JsVal(AString str, Int32 n, Local<Value> jsv)
{
    if (!cx || !jsv) {
        return E_INVALID_ARGUMENT;
    }

    if (str.IsNull() || !n) {
        jsv = NULL;
        return NOERROR;
    }

    JSStringRef jsStr = JSStringCreateWithUTF8CString(str);
    if (NULL == jsStr) {
        return E_OUT_OF_MEMORY;
    }

    jsv = JSValueMakeString(cx, jsStr);
    return NOERROR;
}

static ECode _jsVal2CppVector(Local<Value> jsv,
                              ICppVectorInfo *pICppVectorInfo, ICppVectorSetter *setter)
{
    return E_NOT_IMPLEMENTED;
    /*assert(cx && pICppVectorInfo && setter);

    JSObjectRef  pJSObj = NULL;
    if (NULL == (pJSObj = JSValueToObject(cx, jsv, NULL))) {
        if (JSVAL_IS_STRING(jsv)) {
            JSStringRef jsStr = JSValueToStringCopy(cx, jsv,NULL);
            int len = JSStringGetLength(jsStr);
            Int32 arrayLen;
            CarDataType dataType;
            IDataTypeInfo * elementTypeInfo;
            ECode ec = pICppVectorInfo->GetLength(&arrayLen);
            if (FAILED(ec)) return ec;

            ec = pICppVectorInfo->GetElementTypeInfo(&elementTypeInfo);
            if (FAILED(ec)) return ec;
            ec = elementTypeInfo->GetDataType(&dataType);
            elementTypeInfo->Release();
            if (FAILED(ec)) return ec;
            setter->ZeroAllElements();
            switch(dataType) {
                case CarDataType_Byte:
                case CarDataType_AChar:
                    if (len != arrayLen) return E_INVALID_ARGUMENT;
                    setter->SetAllElements((PVoid)JS_GetStringBytes(jsStr),
                        len * sizeof(char));
                    break;
                case CarDataType_WChar: {
                    const char * str = JS_GetStringBytes(jsStr);
                    Int32 i, pos = 0;
                    WChar wch;
                    for (i = 0; i < arrayLen; i++) {
                        if (pos >= len) return E_INVALID_ARGUMENT;
                        pos += mbtowc((wchar_t*)&wch, str + pos, len - pos);
                        setter->SetWCharElement(i, wch);
                    }
                    if (pos != len) return E_INVALID_ARGUMENT;
                    break;
                                        }
                default:
                    return E_INVALID_ARGUMENT;
                    break;
            }
            return NOERROR;
        }
        return E_INVALID_ARGUMENT;
    }

    jsuint jsArrayLen;
    Local<Value>  jsvalue;
    JS_GetArrayLength(cx, pJSObj, &jsArrayLen);

    Int32 arrayLen;
    ECode ec = pICppVectorInfo->GetLength(&arrayLen);
    if (FAILED(ec)) return ec;
    if (arrayLen != (Int32)jsArrayLen) {
        return E_INVALID_ARGUMENT;
    }

    CarDataType dataType;
    IDataTypeInfo * elementTypeInfo;
    ec = pICppVectorInfo->GetElementTypeInfo(&elementTypeInfo);
    if (FAILED(ec)) return ec;
    ec = elementTypeInfo->GetDataType(&dataType);
    if (FAILED(ec)) {
        elementTypeInfo->Release();
        return ec;
    }

    Int32 i;
    setter->ZeroAllElements();
    switch(dataType) {
        case CarDataType_Int16:
            PUSH_ARRAY_ELEMENT(Int16);
            break;
        case CarDataType_Int32:
            PUSH_ARRAY_ELEMENT(Int32);
            break;
        case CarDataType_Int64:
            PUSH_ARRAY_ELEMENT(Int64);
            break;
        case CarDataType_Float:
            PUSH_ARRAY_ELEMENT(Float);
            break;
        case CarDataType_Double:
            PUSH_ARRAY_ELEMENT(Double);
            break;
        case CarDataType_Boolean:
            PUSH_ARRAY_ELEMENT(Boolean);
            break;
        case CarDataType_Byte:
            PUSH_ARRAY_ELEMENT(Byte);
            break;
        case CarDataType_AChar:
            PUSH_ARRAY_ELEMENT(AChar);
            break;
        case CarDataType_WChar:
            PUSH_ARRAY_ELEMENT(WChar);
            break;
        case CarDataType_Enum: {
            for (i = 0; i < arrayLen; i++) {
                Int32 val;
                jsvalue = JSObjectGetProperty(cx, pJSObj,JSValueMakeNumber(cx,i),NULL);
                ec = jsVal2Int32(cx, jsvalue, &val);
                if (FAILED(ec)) goto ERR;
                setter->SetEnumElement(i, val);
                if (FAILED(ec)) goto ERR;
            }
            break;
                               }
        case CarDataType_Struct: {
            for (i = 0; i < arrayLen; i++) {
                IStructSetter *innerSetter;
                IStructInfo * piStructInfo;
                piStructInfo = IStructInfo::Probe(elementTypeInfo);
                if (!piStructInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;

                jsvalue = JSObjectGetProperty(cx, pJSObj,JSValueMakeNumber(cx,i),NULL);
                ec = setter->GetStructElementSetter(i, &innerSetter);
                if (FAILED(ec)) goto ERR;
                ec = _jsVal2Struct(cx, jsvalue, piStructInfo, innerSetter);
                innerSetter->Release();
                if (FAILED(ec)) goto ERR;
            }
            break;
                                 }
        case CarDataType_CppVector: {
            for (i = 0; i < arrayLen; i++) {
                ICppVectorSetter *innerSetter;
                ICppVectorInfo * piCppVectorInfo;
                piCppVectorInfo = ICppVectorInfo::Probe(elementTypeInfo);
                if (!piCppVectorInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;

                jsvalue = JSObjectGetProperty(cx, pJSObj,JSValueMakeNumber(cx,i),NULL);
                ec = setter->GetCppVectorElementSetter(i, &innerSetter);
                if (FAILED(ec)) goto ERR;
                ec = _jsVal2CppVector(cx, jsvalue, piCppVectorInfo, innerSetter);
                innerSetter->Release();
                if (FAILED(ec)) goto ERR;
            }
            break;
                                    }
        case CarDataType_ECode:
            PUSH_ARRAY_ELEMENT(ECode);
            break;
        case CarDataType_EMuid: {
            for (i = 0; i < arrayLen; i++) {
                EMuid val;
                jsvalue = JSObjectGetProperty(cx, pJSObj,JSValueMakeNumber(cx,i),NULL);
                ec = jsVal2EMuid(cx, jsvalue, &val);
                if (FAILED(ec)) goto ERR;
                setter->SetEMuidElement(i, &val);
                if (FAILED(ec)) goto ERR;
            }
            break;
                                }
        case CarDataType_EGuid: {
            assert(0 && "cppvector EGuid");
            for (i = 0; i < arrayLen; i++) {
                EGuid val;
                jsvalue = JSObjectGetProperty(cx, pJSObj,JSValueMakeNumber(cx,i),NULL);
                ec = jsVal2EGuid(cx, jsvalue, &val);
                if (FAILED(ec)) goto ERR;
                setter->SetEGuidElement(i, &val);
                if (FAILED(ec)) goto ERR;
            }
            break;
                                }
        default:
            assert(0 && "not implement");
            break;
    }
    elementTypeInfo->Release();
    return NOERROR;
    ERR:
    elementTypeInfo->Release();
    return ec;*/
}

static ECode _cppVector2jsVal(JSContextRef cx,
                              ICppVectorInfo *pICppVectorInfo, ICppVectorGetter *getter, Local<Value> jsv)
{
    return E_NOT_IMPLEMENTED;
    //    assert(cx && pICppVectorInfo && getter && jsv);
    //
    //    JSObjectRef  pJSObj = JSObjectMakeArray(cx,0,NULL,NULL);
    //    if (NULL == pJSObj) {
    //        return E_OUT_OF_MEMORY;
    //    }
    //
    //    Int32 arrayLen;
    //    ECode ec = pICppVectorInfo->GetLength(&arrayLen);
    //    if (FAILED(ec)) return ec;
    //
    //    CarDataType dataType;
    //    IDataTypeInfo * elementTypeInfo;
    //    ec = pICppVectorInfo->GetElementTypeInfo(&elementTypeInfo);
    //    if (FAILED(ec)) return ec;
    //    ec = elementTypeInfo->GetDataType(&dataType);
    //    if (FAILED(ec)) {
    //        elementTypeInfo->Release();
    //        return ec;
    //    }
    //
    //    Int32 i;
    //    Local<Value> jsvalue;
    //    switch(dataType) {
    //        case CarDataType_Int16: {
    //            Int16 val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetInt16Element(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewNumberValue(cx, val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                }
    //        case CarDataType_Int32: {
    //            Int32 val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetInt32Element(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewNumberValue(cx, val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                }
    //        case CarDataType_Int64: {
    //            Int64 val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetInt64Element(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewDoubleValue(cx, (jsdouble)val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                }
    //        case CarDataType_Float: {
    //            Float val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetFloatElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewDoubleValue(cx, (jsdouble)val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                }
    //        case CarDataType_Double: {
    //            Double val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetDoubleElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewDoubleValue(cx, (jsdouble)val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                 }
    //        case CarDataType_Byte: {
    //            Byte *buff = (Byte*)_alloca(sizeof(Byte) * arrayLen);
    //            if (buff == NULL) {
    //                ec = E_OUT_OF_MEMORY;
    //                goto ERR;
    //            }
    //
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetByteElement(i, &buff[i]);
    //                if (FAILED(ec)) goto ERR;
    //            }
    //            ec = AStringN2JsVal(cx, (char *)buff, arrayLen, jsv);
    //            if (FAILED(ec)) goto ERR;
    //            elementTypeInfo->Release();
    //            return NOERROR;
    //                               }
    //        case CarDataType_AChar: {
    //            AChar *buff = (AChar*)_alloca(sizeof(AChar) * arrayLen);
    //            if (buff == NULL) {
    //                ec = E_OUT_OF_MEMORY;
    //                goto ERR;
    //            }
    //
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetACharElement(i, &buff[i]);
    //                if (FAILED(ec)) goto ERR;
    //            }
    //            ec = AStringN2JsVal(cx, (AString)buff, arrayLen, jsv);
    //            if (FAILED(ec)) goto ERR;
    //            elementTypeInfo->Release();
    //            return NOERROR;
    //                                }
    //        case CarDataType_WChar: {
    //            AChar *buff = (AChar*)_alloca(MB_LEN_MAX * arrayLen);
    //            if (buff == NULL) {
    //                ec = E_OUT_OF_MEMORY;
    //                goto ERR;
    //            }
    //            WChar wch;
    //            Int32 len = 0, ret;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetWCharElement(i, &wch);
    //                if (FAILED(ec)) goto ERR;
    //                ret = wctomb(buff + len, wch);
    //                if (ret == -1) {
    //                    ec = -1;
    //                    goto ERR;
    //                }
    //                len += ret;
    //            }
    //            ec = AStringN2JsVal(cx, (AString)buff, len, jsv);
    //            if (FAILED(ec)) goto ERR;
    //            elementTypeInfo->Release();
    //            return NOERROR;
    //                                }
    //        case CarDataType_Boolean: {
    //            Boolean val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetBooleanElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                if (val) {
    //                    jsvalue = BOOLEAN_TO_JSVAL(true);
    //                }
    //                else {
    //                    jsvalue = BOOLEAN_TO_JSVAL(false);
    //                }
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                  }
    //        case CarDataType_Enum :{
    //            Int32 val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetEnumElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewNumberValue(cx, val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                               }
    //        case CarDataType_Struct: {
    //            for (i = 0; i < arrayLen; i++) {
    //                IStructGetter *innerGetter;
    //                IStructInfo * piStructInfo;
    //                piStructInfo = IStructInfo::Probe(elementTypeInfo);
    //                if (!piStructInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
    //                if (FAILED(ec)) goto ERR;
    //
    //                ec = getter->GetStructElementGetter(i, &innerGetter);
    //                if (FAILED(ec)) goto ERR;
    //                ec = _struct2jsVal(cx, piStructInfo,
    //                    innerGetter, &jsvalue);
    //                innerGetter->Release();
    //                if (FAILED(ec)) goto ERR;
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                 }
    //        case CarDataType_CppVector: {
    //            for (i = 0; i < arrayLen; i++) {
    //                ICppVectorGetter *innerGetter;
    //                ICppVectorInfo * piCppVectorInfo;
    //                piCppVectorInfo = ICppVectorInfo::Probe(elementTypeInfo);
    //                if (!piCppVectorInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
    //                if (FAILED(ec)) goto ERR;
    //
    //                ec = getter->GetCppVectorElementGetter(i, &innerGetter);
    //                if (FAILED(ec)) goto ERR;
    //                ec = _cppVector2jsVal(cx, piCppVectorInfo, innerGetter, &jsvalue);
    //                innerGetter->Release();
    //                if (FAILED(ec)) goto ERR;
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                    }
    //        case CarDataType_ECode: {
    //            ECode val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetECodeElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                JS_NewNumberValue(cx, val, &jsvalue);
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                }
    //        case CarDataType_EMuid:{
    //            EMuid val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetEMuidElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                ec = EMuid2JsVal(cx, &val, &jsvalue);
    //                if (FAILED(ec)) goto ERR;
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                               }
    //        case CarDataType_EGuid: {
    //            assert(0 && "cppvector EGuid");
    //            EGuid val;
    //            for (i = 0; i < arrayLen; i++) {
    //                ec = getter->GetEGuidElement(i, &val);
    //                if (FAILED(ec)) goto ERR;
    //                ec = EGuid2JsVal(cx, &val, &jsvalue);
    //                //Need free EGuid->pUunm ????
    //                if (FAILED(ec)) goto ERR;
    //                JSObjectSetProperty(cx,JSValueMakeNumber(cx,i),jsvalue,pJSObj);
    //            }
    //            break;
    //                                }
    //        default:
    //            assert(0 && "not implement");
    //            break;
    //    }
    //
    //    elementTypeInfo->Release();
    //    *jsv = OBJECT_TO_JSVAL(pJSObj);
    //    return NOERROR;
    //
    //ERR:
    //    elementTypeInfo->Release();
    //    return ec;
}

static ECode _jsVal2Struct(Local<Value> jsv,
                           IStructInfo *pIStructInfo, IStructSetter *setter)
{
    assert (cx  && pIStructInfo && setter);

    JSObjectRef  pJSObj;
    if ( NULL == (pJSObj = JSValueToObject(cx, jsv, NULL))) {
        return E_INVALID_ARGUMENT;
    }
    Int32 nFieldCount;
    ECode ec = pIStructInfo->GetFieldCount(&nFieldCount);
    if (FAILED(ec)) return ec;
    assert(nFieldCount && "StructFieldCount <= 0");
    BufferOf<IFieldInfo *> *pfieldInfos = BufferOf<IFieldInfo *>::Alloc(nFieldCount);
    if (!pfieldInfos) return E_OUT_OF_MEMORY;

    ec = pIStructInfo->GetAllFieldInfos(pfieldInfos);
    if (FAILED(ec)) {
        BufferOf<IFieldInfo *>::Free(pfieldInfos);
        return ec;
    }
    Int32 i = 0;
    IFieldInfo *pIFieldInfo;
    AStringBuf_<MAX_SYMBOL_LEN> name;
    IDataTypeInfo *typeInfo;
    CarDataType dataType;
    Local<Value> jsvalue;
    setter->ZeroAllFields();
    for (; i < nFieldCount; i++) {
        pIFieldInfo = (*pfieldInfos)[i];
        if (!pIFieldInfo) {
            ec = E_NO_INTERFACE;
            goto ERR;
        }

        pIFieldInfo->GetName(&name);
        if (NULL == (jsvalue = JSObjectGetProperty(cx, pJSObj, JSStringCreateWithUTF8CString(name), NULL))) {
            goto ERR;
        }

        pIFieldInfo->GetTypeInfo(&typeInfo);
        typeInfo->GetDataType(&dataType);

        switch(dataType) {
        case CarDataType_Int16:
            PUSH_STRUCT_FIELD(Int16);
            break;
        case CarDataType_Int32:
            PUSH_STRUCT_FIELD(Int32);
            break;
        case CarDataType_Int64:
            PUSH_STRUCT_FIELD(Int64);
            break;
        case CarDataType_Byte:
            PUSH_STRUCT_FIELD(Byte);
            break;
        case CarDataType_Float:
            PUSH_STRUCT_FIELD(Float);
            break;
        case CarDataType_Double:
            PUSH_STRUCT_FIELD(Double);
            break;
        case CarDataType_AChar:
            PUSH_STRUCT_FIELD(AChar);
            break;
        case CarDataType_WChar:
            PUSH_STRUCT_FIELD(WChar);
            break;
        case CarDataType_Boolean:
            PUSH_STRUCT_FIELD(Boolean);
            break;
        case CarDataType_Enum: {
            Int32 val;
            ec = jsVal2Int32(cx, jsvalue, &val);
            if (FAILED(ec)) goto ERR;
            setter->SetEnumField(name, val);
            if (FAILED(ec)) goto ERR;
            break;
        }
        case CarDataType_CppVector: {
            ICppVectorSetter *innerSetter;
            ICppVectorInfo *piCppVectorInfo;
            piCppVectorInfo = ICppVectorInfo::Probe(typeInfo);
            if (!piCppVectorInfo) ec = E_NO_INTERFACE;
            else ec = NOERROR;
            if (FAILED(ec)) goto ERR;

            ec = setter->GetCppVectorFieldSetter(name, &innerSetter);
            if (FAILED(ec)) goto ERR;
            ec = _jsVal2CppVector(cx, jsvalue, piCppVectorInfo, innerSetter);
            innerSetter->Release();
            if (FAILED(ec)) goto ERR;
            break;
        }
        case CarDataType_Struct: {
            IStructSetter *innerSetter;
            IStructInfo *piStructInfo;
            piStructInfo = IStructInfo::Probe(typeInfo);
            if (!piStructInfo) ec = E_NO_INTERFACE;
            else ec = NOERROR;
            if (FAILED(ec)) goto ERR;

            ec = setter->GetStructFieldSetter(name, &innerSetter);
            if (FAILED(ec)) goto ERR;
            ec = _jsVal2Struct(cx, jsvalue, piStructInfo, innerSetter);
            innerSetter->Release();
            if (FAILED(ec)) goto ERR;
            break;
        }
        default:
            assert(0 && "not implemented");
            break;
        }
        typeInfo->Release();
        pIFieldInfo->Release();
    }

    BufferOf<IFieldInfo *>::Free(pfieldInfos);
    return NOERROR;

ERR:
    for (; i < nFieldCount; i++) {
        if ((*pfieldInfos)[i]) (*pfieldInfos)[i]->Release();
    }
    BufferOf<IFieldInfo *>::Free(pfieldInfos);
    return ec;
}
static ECode _struct2jsVal(IStructInfo *pIStructInfo,
                           IStructGetter *getter, Local<Value> jsv)
{
    return E_NOT_IMPLEMENTED;
    /*assert(cx && pIStructInfo && getter && jsv);
    JSObjectRef pRetObj = JS_NewObject(cx, &js_ObjectClass, NULL, NULL);
    if (NULL == pRetObj) return E_OUT_OF_MEMORY;

    Int32 i, nFieldCount;
    IFieldInfo * piFieldInfo;
    IDataTypeInfo *typeInfo;
    CarDataType dataType;
    Local<Value> val;

    AStringBuf_<MAX_SYMBOL_LEN> name;
    ECode ec = pIStructInfo->GetFieldCount(&nFieldCount);
    if (FAILED(ec)) return ec;
    BufferOf<IFieldInfo *>* pfields = BufferOf<IFieldInfo *>::Alloc(nFieldCount);
    if (NULL == pfields) return E_OUT_OF_MEMORY;
    ec = pIStructInfo->GetAllFieldInfos(pfields);
    if (FAILED(ec)) {
        BufferOf<IFieldInfo *>::Free(pfields);
        return ec;
    }

    for (i = 0; i < nFieldCount; i++) {
        piFieldInfo = IFieldInfo::Probe((*pfields)[i]);
        if (!piFieldInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
        if (FAILED(ec)) goto ERR;

        piFieldInfo->GetName(&name);
        piFieldInfo->GetTypeInfo(&typeInfo);
        typeInfo->GetDataType(&dataType);
        switch(dataType) {
            case CarDataType_Int16: {
                Int16 filedVal;
                ec = getter->GetInt16Field(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                JS_NewNumberValue(cx, filedVal, &val);
                break;
                                    }
            case CarDataType_Int32: {
                Int32 filedVal;
                ec = getter->GetInt32Field(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                JS_NewNumberValue(cx, filedVal, &val);
                break;
                                    }
            case CarDataType_Int64: {
                Int64 filedVal;
                ec = getter->GetInt64Field(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                JS_NewDoubleValue(cx, (jsdouble)filedVal, &val);
                break;
                                    }
            case CarDataType_Float: {
                Float filedVal;
                ec = getter->GetFloatField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                JS_NewDoubleValue(cx, (jsdouble)filedVal, &val);
                break;
                                    }
            case CarDataType_Double: {
                Double filedVal;
                ec = getter->GetDoubleField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                JS_NewDoubleValue(cx, (jsdouble)filedVal, &val);
                break;
                                     }
            case CarDataType_Byte: {
                Byte filedVal;
                ec = getter->GetByteField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                val = INT_TO_JSVAL(((Int32)filedVal));
                break;
                                   }
            case CarDataType_AChar:{
                AChar filedVal;
                ec = getter->GetACharField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                val = INT_TO_JSVAL(((Int32)filedVal));
                break;
                                   }
            case CarDataType_WChar: {
                WChar filedVal;
                ec = getter->GetWCharField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                val = INT_TO_JSVAL(((Int32)filedVal));
                break;
                                    }
            case CarDataType_Boolean: {
                Boolean filedVal;
                ec = getter->GetBooleanField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                if (filedVal) {
                    val = BOOLEAN_TO_JSVAL(true);
                }
                else {
                    val = BOOLEAN_TO_JSVAL(false);
                }
                break;
                                      }
            case CarDataType_Enum: {
                Int32 filedVal;
                ec = getter->GetEnumField(name, &filedVal);
                if (FAILED(ec)) goto ERR;
                JS_NewNumberValue(cx, filedVal, &val);
                break;
                                   }
            case CarDataType_CppVector: {
                ICppVectorGetter * innerGetter;
                ICppVectorInfo * piCppVectorInfo;
                piCppVectorInfo = ICppVectorInfo::Probe(typeInfo);
                if (!piCppVectorInfo) ec = E_NO_INTERFACE;
                if (FAILED(ec)) goto ERR;

                ec = getter->GetCppVectorFieldGetter(name, &innerGetter);
                if (FAILED(ec)) goto ERR;
                ec = _cppVector2jsVal(cx, piCppVectorInfo, innerGetter, &val);
                innerGetter->Release();
                if (FAILED(ec)) goto ERR;
                break;
                                        }
            case CarDataType_Struct: {
                IStructGetter * innerGetter;
                IStructInfo * piStructInfo;
                piStructInfo = IStructInfo::Probe(typeInfo);
                if (!piStructInfo) ec = E_NO_INTERFACE;
                if (FAILED(ec)) goto ERR;

                ec = getter->GetStructFieldGetter(name, &innerGetter);
                if (FAILED(ec)) goto ERR;
                ec = _struct2jsVal(cx, piStructInfo, innerGetter, &val);
                innerGetter->Release();
                if (FAILED(ec)) goto ERR;
                break;
                                     }
            default:
                assert(0 && "not implemented");
                break;
        }
        JS_SetProperty(cx, pRetObj, (char *)name, &val);

        typeInfo->Release();
        (*pfields)[i]->Release();
    }
    BufferOf<IFieldInfo *>::Free(pfields);
    *jsv = OBJECT_TO_JSVAL(pRetObj);
    return NOERROR;

    ERR:
    typeInfo->Release();
    for (; i < nFieldCount; i++) {
        if((*pfields)[i]) (*pfields)[i]->Release();
    }
    BufferOf<IFieldInfo *>::Free(pfields);
    return ec;*/
}


//ECode Variant2jsVal(const Variant& value, Local<Value> jsv)
//{
//    if (!cx || !jsv) {
//        return E_INVALID_ARGUMENT;
//    }
//
//    if (value.IsReference())
//        return E_NOT_IMPLEMENTED;
//
//    switch(value.Type()) {
//        case CarDataType_Int16:
//            *jsv = INT_TO_JSVAL((Int16)value);
//            break;
//        case CarDataType_Int32:
//            jsv = JSValueMakeNumber(cx, (Int32)value);
//            break;
//        case CarDataType_Int64:
//            jsv = JSValueMakeNumber(cx, (Int64)value);
//            break;
//        case CarDataType_Byte:
//            *jsv = INT_TO_JSVAL((int)(Byte)value);
//            break;
//        case CarDataType_Float:
//            jsv = JSValueMakeNumber(cx, (Float)value);
//            break;
//        case CarDataType_Double:
//            jsv = JSValueMakeNumber(cx, (Double)value);
//            break;
//        case CarDataType_AChar:
//            *jsv = INT_TO_JSVAL((int)(AChar)value);
//            break;
//        case CarDataType_WChar:
//            *jsv = INT_TO_JSVAL((int)(WChar)value);
//            break;
//        case CarDataType_AString:
//            return AString2JsVal(cx, value, jsv);
//        case CarDataType_WString:
//            return AString2JsVal(cx, CW2A(value), jsv);
//        case CarDataType_Boolean:
//            if ((bool)value) {
//                *jsv = BOOLEAN_TO_JSVAL(true);
//            }
//            else {
//                *jsv = BOOLEAN_TO_JSVAL(false);
//            }
//            break;
//        case CarDataType_EMuid:
//            return EMuid2JsVal(cx, (EMuid*)value, jsv);
//        case CarDataType_EGuid:
//            return EGuid2JsVal(cx, (EGuid*)value, jsv);
//        case CarDataType_ECode:
//            jsv = JSValueMakeNumber(cx, (Int32)value);
//            break;
//        case CarDataType_Enum:
//            jsv = JSValueMakeNumber(cx, (Int32)value);
//            break;
//        case CarDataType_ArrayOf:
//        case CarDataType_BufferOf:
//            return Array2JsVal(cx, (IDataTypeInfo*)value.GetTypeInfo(),
//                                (PCarQuintet)value, jsv);
//        case CarDataType_Struct:
//            return Struct2JsVal(cx, (IDataTypeInfo*)value.GetTypeInfo(),
//                                (void*)value, jsv);
//        case CarDataType_Interface:
//            return Interface2JsVal(cx, (PInterface)value, jsv);
//
//        default:
//            return E_NOT_IMPLEMENTED;
//            break;
//    }
//
//    return NOERROR;
//}

#endif