#include <limits.h>
#include <stdio.h>

#include "args_converter.h"

#define PUSH_OUT_PARAM(type) do {                                           \
	pArgumentList->SetOutputArgumentOf##type##Ptr(i, (type *)outParamBuff); \
	m_params[i] = (Int32)outParamBuff;                                      \
	outParamBuff += 8;                                                      \
} while(0)

#define PUSH_STRBUFF_PARAM(type) do {                                       \
	Int32 size;                                                             \
	ec = paramInfo->GetAdvisedCapacity(&size);                              \
	if (FAILED(ec) || size == 0) size = DEFAULT_STRINGBUFF_LEN;             \
	    else size++;                                                        \
	type##StringBuf *pstrBuf = type##StringBuf::Alloc(size);                \
	if (!pstrBuf) {                                                         \
	    ec = E_OUT_OF_MEMORY;                                               \
	    goto ERR;                                                           \
	}                                                                       \
	pArgumentList->SetOutputArgumentOf##type##StringBufPtr(i, pstrBuf);     \
	m_params[i] = (Int32)pstrBuf;                                           \
} while(0)

V8ParameterNormalizer::V8ParameterNormalizer(
    v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value> args)
      : args(args)
      //, m_isolate(isolate)
      //, m_outParamCount(0)
{
    memset(m_paramBuff, 0, sizeof(Int32) * MAX_PARAMETER_COUNT);
    memset(m_params, 0, sizeof(Int32) * MAX_PARAMETER_COUNT);
}

V8ParameterNormalizer::~V8ParameterNormalizer()
{
    FreeParams();
}

ECode V8ParameterNormalizer::Normalize(
    IFunctionInfo *pFunctionInfo,
    IArgumentList *pArgumentList)
{
    if (!pFunctionInfo || !pArgumentList) {
        return E_INVALID_ARGUMENT;
    }
#if 0
    Int32 paramCount;
    ECode ec = pFunctionInfo->GetParamCount(&paramCount);
    if (FAILED(ec)) return ec;
    if (0 == paramCount) return NOERROR;

    m_paramInfos = BufferOf<IParamInfo *>::Alloc(paramCount);
    if (!m_paramInfos) return E_OUT_OF_MEMORY;

    ec = pFunctionInfo->GetAllParamInfos(m_paramInfos);
    if (FAILED(ec)) {
        return FAILED(ec) ? ec : -1;
    }

    IParamInfo *paramInfo;
    IDataTypeInfo *typeInfo;
    CarDataType dataType;
    ParamIOAttrib ioAttrib;

    char *outParamBuff = m_paramBuff;
    for (Int32 i = 0; i < paramCount; i++) {
        paramInfo = (*m_paramInfos)[i];
        if (!paramInfo) return E_NO_INTERFACE;

        paramInfo->GetTypeInfo(&typeInfo);
        typeInfo->GetDataType(&dataType);
        paramInfo->GetIOAttrib(&ioAttrib);
        if (ParamIOAttrib_In == ioAttrib) {
            if (m_argc <= 0) {
                ec = E_INVALID_ARGUMENT;
                goto ERR;
            }
            m_argc--;

            switch(dataType) {
            case CarDataType_Int16: {
                Int16 val;
                ec = jsVal2Int16(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Int16,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfInt16(i, val);
                break;
            }
            case CarDataType_Int32: {
                Int32 val;
                ec = jsVal2Int32(m_ctx, m_argv[i], &val);
                if (FAILED(ec))
                    goto ERR;
                printf("Param %d ,After jsVal2Int32,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfInt32(i, val);
                break;
            }
            case CarDataType_Int64: {
                Int64 val;
                ec = jsVal2Int64(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Int64,val=%I64d\n", i, val);
                pArgumentList->SetInputArgumentOfInt64(i, val);
                break;
            }
            case CarDataType_Byte: {
                Byte val;
                ec = jsVal2Byte(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Float,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfByte(i, val);
                break;
            }
            case CarDataType_Float: {
                Float val;
                ec = jsVal2Float(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Float,val=%f\n", i, val);
                pArgumentList->SetInputArgumentOfFloat(i, val);
                break;
            }
            case CarDataType_Double: {
                Double val;
                ec = jsVal2Double(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Double,val=%f\n", i, val);
                pArgumentList->SetInputArgumentOfDouble(i, val);
                break;
            }
            case CarDataType_AChar: {
                AChar val;
                ec = jsVal2AChar(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2AChar,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfAChar(i, val);
                break;
            }
            case CarDataType_WChar: {
                WChar val;
                ec = jsVal2WChar(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                wprintf(L"Param %d ,After jsVal2WChar,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfWChar(i, val);
                break;
            }
            case CarDataType_AString: {
                char *val = NULL;
                ec = jsVal2AString(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2AString,val=%s\n", i, val);
                pArgumentList->SetInputArgumentOfAString(i, val);
                m_params[i] = (Int32)val;
                break;
            }
            case CarDataType_WString: {
                wchar_t *val = NULL;
                ec = jsVal2WString(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                wprintf(L"Param %d ,After jsVal2WString,val=%s\n", i, val);
                pArgumentList->SetInputArgumentOfWString(i, val);
                m_params[i] = (Int32)val;
                break;
            }
            case CarDataType_Boolean: {
                Boolean val;
                ec = jsVal2Boolean(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Boolean,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfBoolean(i, val);
                break;
            }
            /*
            case CarDataType_EMuid: {
            EMuid *val = (EMuid *)malloc(sizeof(EMuid));
            if (val == NULL) {
            ec = E_OUT_OF_MEMORY;
            goto ERR;
            }
            ec = jsVal2EMuid(m_cx, m_argv[i], val);
            if (FAILED(ec)) {
            free(val);
            goto ERR;
            }
            pArgumentList->SetInputArgumentOfEMuid(i, val);
            m_params[i] = (Int32)val;
            break;
            }
            case CarDataType_EGuid: {
            EGuid *val = (EGuid *)malloc(sizeof(EGuid));
            if (val == NULL) {
            ec = E_OUT_OF_MEMORY;
            goto ERR;
            }

            ec = jsVal2EGuid(m_cx, m_argv[i], val);
            if (FAILED(ec)) {
            free(val);
            goto ERR;
            }
            pArgumentList->SetInputArgumentOfEGuid(i, val);
            m_params[i] = (Int32)val;
            break;
            }*/
            case CarDataType_ECode: {
                ECode val;
                ec = jsVal2ECode(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2ECode,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfECode(i, val);
                break;
            }
            case CarDataType_Enum: {
                Int32 val;
                ec = jsVal2Int32(m_ctx, m_argv[i], &val);
                if (FAILED(ec)) goto ERR;
                printf("Param %d ,After jsVal2Enum,val=%d\n", i, val);
                pArgumentList->SetInputArgumentOfEnum(i, val);
                break;
            }
            case CarDataType_ArrayOf:
            case CarDataType_BufferOf:
                /*{
                IVariableOfCarArray *variable;
                PCarQuintet value;
                ICarArrayInfo * piCarArrayInfo;
                piCarArrayInfo = ICarArrayInfo::Probe(typeInfo);
                if (!piCarArrayInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;

                ec = jsVal2Array(m_cx, m_argv[i], piCarArrayInfo, &variable);
                if (FAILED(ec)) goto ERR;
                ec = variable->GetPayload((PVoid*)&value);
                if (FAILED(ec)) {
                variable->Release();
                goto ERR;
                }
                pArgumentList->SetInputArgumentOfCarArray(i, value);
                m_params[i] = (Int32)variable;
                break;
                }

                case CarDataType_Struct:{
                PVoid value;
                IVariableOfStruct *variable;
                IStructInfo * piStructInfo;
                piStructInfo = IStructInfo::Probe(typeInfo);
                if (!piStructInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;

                ec = jsVal2Struct(m_cx, m_argv[i], piStructInfo, &variable);
                if (FAILED(ec)) goto ERR;

                ec = variable->GetPayload(&value);
                if (FAILED(ec)) {
                variable->Release();
                goto ERR;
                }
                pArgumentList->SetInputArgumentOfStructPtr(i, value);
                m_params[i] = (Int32)variable;
                break;
                }

                case CarDataType_Interface: {
                PInterface object;
                IInterfaceInfo * piInterfaceInfo;
                piInterfaceInfo = IInterfaceInfo::Probe(typeInfo);
                if (!piInterfaceInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;
                ec = jsVal2Interface(m_cx, m_argv[i], piInterfaceInfo, &object);
                if (FAILED(ec)) goto ERR;
                pArgumentList->SetInputArgumentOfObjectPtr(i, object);
                m_params[i] = (Int32)object;
                break;
                }
                */
            default:
                assert(0 && " not implemented");
                ec = E_NOT_SUPPORTED;
                goto ERR;
                break;
            }
        }
        else if (ParamIOAttrib_CallerAllocOut == ioAttrib) {
            m_outParamCount++;
            switch(dataType) {
            case CarDataType_Int16:
                PUSH_OUT_PARAM(Int16);
                break;
            case CarDataType_Int32:
                PUSH_OUT_PARAM(Int32);
                break;
            case CarDataType_Int64:
                PUSH_OUT_PARAM(Int64);
                break;
            case CarDataType_Byte:
                PUSH_OUT_PARAM(Byte);
                break;
            case CarDataType_Float:
                PUSH_OUT_PARAM(Float);
                break;
            case CarDataType_Double:
                PUSH_OUT_PARAM(Double);
                break;

            case CarDataType_AChar:
                PUSH_OUT_PARAM(AChar);
                break;
            case CarDataType_WChar:
                PUSH_OUT_PARAM(WChar);
                break;
            case CarDataType_AStringBuf:
                PUSH_STRBUFF_PARAM(A);
                break;
            case CarDataType_WStringBuf:
                PUSH_STRBUFF_PARAM(W);
                break;
            case CarDataType_Boolean:
                PUSH_OUT_PARAM(Boolean);
                break;
                /*
                case CarDataType_Interface: {
                pArgumentList->SetOutputArgumentOfObjectPtrPtr(i, (PInterface *)outParamBuff);
                m_params[i] = (Int32)outParamBuff;
                outParamBuff += 8;
                break;
                }
                case CarDataType_EMuid: {
                EMuid *val = (EMuid *)malloc(sizeof(EMuid));
                if (val == NULL) {
                ec = E_OUT_OF_MEMORY;
                goto ERR;
                }
                memset(val, 0, sizeof(EMuid));
                pArgumentList->SetOutputArgumentOfEMuidPtr(i, val);
                m_params[i] = (Int32)val;
                break;
                }
                case CarDataType_EGuid: {
                EGuid *val = (EGuid *)malloc(sizeof(EGuid));
                if (val == NULL) {
                ec = E_OUT_OF_MEMORY;
                goto ERR;
                }
                memset(val, 0, sizeof(EGuid));
                pArgumentList->SetOutputArgumentOfEGuidPtr(i, val);
                m_params[i] = (Int32)val;
                break;
                }
                */
            case CarDataType_ECode:
                PUSH_OUT_PARAM(ECode);
                break;
            case CarDataType_Enum:
                pArgumentList->SetOutputArgumentOfEnumPtr(i, (Int32 *)outParamBuff);
                m_params[i] = (Int32)outParamBuff;
                outParamBuff += 8;
                break;
            case CarDataType_ArrayOf:
            case CarDataType_BufferOf:
                /*{
                Int32 size;
                PCarQuintet value;
                IVariableOfCarArray *variable;

                ICarArrayInfo * piCarArrayInfo;
                piCarArrayInfo = ICarArrayInfo::Probe(typeInfo);
                if (!piCarArrayInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;

                ec = paramInfo->GetAdvisedCapacity(&size);
                if (FAILED(ec) || size == 0) size = DEFAULT_STRINGBUFF_LEN;
                ec = piCarArrayInfo->CreateVariable(size, &variable);
                if (FAILED(ec)) goto ERR;

                ec = variable->GetPayload((PVoid*)&value);
                if (FAILED(ec)) {
                variable->Release();
                goto ERR;
                }
                pArgumentList->SetOutputArgumentOfCarArrayPtr(i, value);
                m_params[i] = (Int32)variable;
                break;
                }
                case CarDataType_Struct: {
                IVariableOfStruct *variable;
                PVoid value;
                IStructInfo * piStructInfo;
                piStructInfo = IStructInfo::Probe(typeInfo);
                if (!piStructInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
                if (FAILED(ec)) goto ERR;

                ec = piStructInfo->CreateVariable(&variable);
                if (FAILED(ec)) goto ERR;
                ec = variable->GetPayload(&value);
                if (FAILED(ec)) {
                variable->Release();
                goto ERR;
                }
                pArgumentList->SetOutputArgumentOfStructPtr(i, value);
                m_params[i] = (Int32)variable;
                break;
                }
                */
            default:
                assert(0 && " not implemented");
                ec = E_NOT_SUPPORTED;
                goto ERR;
                break;
            }
        }
        else {
            m_outParamCount++;
            switch(dataType) {
            case CarDataType_AStringBuf:
                pArgumentList->SetOutputArgumentOfAStringBufPtrPtr(i, (AStringBuf **)outParamBuff);
                m_params[i] = (Int32)outParamBuff;
                outParamBuff += 8;
                break;
            case CarDataType_WStringBuf:
                pArgumentList->SetOutputArgumentOfWStringBufPtrPtr(i, (WStringBuf **)outParamBuff);
                m_params[i] = (Int32)outParamBuff;
                outParamBuff += 8;
                break;
            case CarDataType_ArrayOf:
            case CarDataType_BufferOf:
                pArgumentList->SetOutputArgumentOfCarArrayPtrPtr(i, (PCarQuintet *)outParamBuff);
                m_params[i] = (Int32)outParamBuff;
                outParamBuff += 8;
                break;
                //case CarDataType_Struct: IsNeed ?
            default:
                assert(0 && " not implemented");
                ec = E_NOT_SUPPORTED;
                goto ERR;
                break;
            }
        }
        typeInfo->Release();
    }
    return NOERROR;
ERR:
    paramInfo->Release();
    typeInfo->Release();
    return ec;
#endif
    return NOERROR;
}
#if 0
ECode V8ParameterNormalizer::GetReturnValue(JSValueRef *rval)
{
    if (0 == m_outParamCount) return NOERROR;

    if (NULL == rval) return E_INVALID_ARGUMENT;

    IParamInfo *paramInfo;
    ParamIOAttrib ioAttrib;
    ECode ec;
    if (1 == m_outParamCount) {
        for (Int32 i = m_paramInfos->GetUsed() - 1; i >= 0; i--) {
            paramInfo = (*m_paramInfos)[i];
            if (!paramInfo) return E_NO_INTERFACE;

            paramInfo->GetIOAttrib(&ioAttrib);
            if (ParamIOAttrib_In == ioAttrib) continue;
            return NativeVal2JsVal(paramInfo, (void *)m_params[i], rval,
                                   ParamIOAttrib_CallerAllocOut  == ioAttrib);
        }
    } else {
        /*
        JSObject *pRetObj = JS_NewObject(m_cx, &js_ObjectClass, NULL, NULL);
        if (NULL == pRetObj) return E_OUT_OF_MEMORY;

        jsval val;
        for (Int32 i = m_paramInfos->GetUsed() - 1;
        i >= 0 && m_outParamCount; i--) {
        paramInfo = (*m_paramInfos)[i];
        if (!paramInfo) return E_NO_INTERFACE;

        paramInfo->GetIOAttrib(&ioAttrib);
        if (ParamIOAttrib_In == ioAttrib) continue;
        ec = NativeVal2JsVal(paramInfo, (void*)m_params[i], &val,
        ParamIOAttrib_CallerAllocOut  == ioAttrib);

        if (FAILED(ec)) return ec;

        AStringBuf_<MAX_SYMBOL_LEN> valName;
        paramInfo->GetName(&valName);
        JS_SetProperty(m_cx, pRetObj, (char *)valName, &val);

        m_outParamCount--;

        }

        *rval = OBJECT_TO_JSVAL(pRetObj);
        return NOERROR;
        */
        printf("m_outParamCount!=1!\n");
    }

    return NOERROR;
}


ECode V8ParameterNormalizer::NativeVal2JsVal(IParamInfo *paramInfo, void *nativeVal, JSValueRef *rval, Boolean isCallerAllocOut)
{
    assert(paramInfo && nativeVal && rval);

    IDataTypeInfo *typeInfo;
    CarDataType dataType;

    paramInfo->GetTypeInfo(&typeInfo);
    typeInfo->GetDataType(&dataType);
    ECode ec = NOERROR;
    switch(dataType) {
    case CarDataType_Int16:
        *rval = JSValueMakeNumber(m_ctx, *(Int16 *)nativeVal);
        break;
    case CarDataType_Int32:
        *rval = JSValueMakeNumber(m_ctx, *(Int32 *)nativeVal);
        break;
    case CarDataType_Int64:
        *rval = JSValueMakeNumber(m_ctx, (double) * (Int64 *)nativeVal);
        break;
    case CarDataType_Byte: {
        JSStringRef jsStr = JSStringCreateWithUTF8CString((const char *)(Byte *)nativeVal);
        *rval = JSValueMakeString(m_ctx, jsStr);
        break;
    }
    case CarDataType_Float:
        *rval = JSValueMakeNumber(m_ctx, (double) * (Float *)nativeVal);
        break;
    case CarDataType_Double:
        //JS_NewDoubleValue(m_ctx, (jsdouble)*(Double*)nativeVal, rval);
        *rval = JSValueMakeNumber(m_ctx, *(Double *)nativeVal);
        //rval=(JSValueRef*)nativeVal;
        break;
    case CarDataType_AChar: {
        JSStringRef jsStr = JSStringCreateWithUTF8CString((const char *)(AChar *)nativeVal);
        *rval = JSValueMakeString(m_ctx, jsStr);
        break;
    }
    case CarDataType_WChar: {
        //__asm int 3;
        wchar_t wc = *(wchar_t *)nativeVal;
        char *pChar;
        wctomb(pChar, wc);
        JSStringRef jsStr = JSStringCreateWithUTF8CString((const char *)pChar);
        *rval = JSValueMakeString(m_ctx, jsStr);
        break;
    }
    case CarDataType_AStringBuf: {
        //__asm int 3;
        AStringBuf *pstrBuf = *(AStringBuf **)nativeVal;
        char *pCh = pstrBuf->GetPayload();
        JSStringRef jsStr = JSStringCreateWithUTF8CString((const char *)pCh);
        *rval = JSValueMakeString(m_ctx, jsStr);
        break;
    }
    case CarDataType_WStringBuf: {
        WStringBuf *pstrBuf;
        if (isCallerAllocOut) {
            pstrBuf = (WStringBuf *)nativeVal;
        } else {
            pstrBuf = *(WStringBuf **)nativeVal;
        }
        char *pCh = CW2A(pstrBuf->GetPayload());
        JSStringRef jsStr = JSStringCreateWithUTF8CString((const char *)pCh);
        *rval = JSValueMakeString(m_ctx, jsStr);
        break;
    }
    case CarDataType_Boolean: {
        *rval = JSValueMakeBoolean(m_ctx, (bool) * (Boolean *)nativeVal);
        break;
        case CarDataType_ECode:
            *rval = JSValueMakeNumber(m_ctx, *(Int32 *)nativeVal);
            break;
        case CarDataType_Enum:
            *rval = JSValueMakeNumber(m_ctx, *(Int32 *)nativeVal);
            break;
        }
        /*
        case CarDataType_EMuid: {
        EMuid *val = (EMuid *)nativeVal;
        ec = EMuid2JsVal(m_cx, val, rval);
        if (FAILED(ec)) goto EXIT;
        break;
        }
        case CarDataType_EGuid: {
        EGuid *val = (EGuid *)nativeVal;
        ec = EGuid2JsVal(m_cx, val, rval);
        if (FAILED(ec)) goto EXIT;
        break;
        }*/

    case CarDataType_ArrayOf:
    case CarDataType_BufferOf:
        /*{
        if (isCallerAllocOut) {
        ICarArrayInfo * piCarArrayInfo;
        piCarArrayInfo = ICarArrayInfo::Probe(typeInfo);
        if (!piCarArrayInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
        if (FAILED(ec)) goto EXIT;
        ec = Array2JsVal(m_cx, piCarArrayInfo,
        (IVariableOfCarArray *)nativeVal, rval);
        }
        else {
        ec = Array2JsVal(m_cx, typeInfo,
        (PCarQuintet)*(PCarQuintet*)nativeVal, rval);
        }
        break;
        }
        case CarDataType_Struct:{
        IStructInfo * piStructInfo;
        piStructInfo = IStructInfo::Probe(typeInfo);
        if (!piStructInfo) ec = E_NO_INTERFACE; else ec = NOERROR;
        if (FAILED(ec)) goto EXIT;

        IVariableOfStruct *variable = (IVariableOfStruct *)nativeVal;
        ec = Struct2JsVal(m_cx, piStructInfo, variable, rval);
        break;
        }
        case CarDataType_Interface: {
        PInterface object = *(PInterface*)nativeVal;
        ec = Interface2JsVal(m_cx, object, rval);
        break;
        }
        */
    default:
        assert(0 && " not implemented");
        break;
    }

EXIT:
    typeInfo->Release();
    return ec;
}
#endif

void  V8ParameterNormalizer::FreeParams()
{
    if (!m_paramInfos) return ;
    /*
    Int32 count = m_paramInfos->GetUsed();
    IParamInfo * paramInfo;
    IDataTypeInfo * typeInfo;
    CarDataType dataType;
    ParamIOAttrib ioAttrib;
    ECode ec = NOERROR;

    for (Int32 i = 0; i < count; i++) {
    if (0 != m_params[i]) {
    paramInfo = (*m_paramInfos)[i];
    if (!paramInfo) continue;

    paramInfo->GetTypeInfo(&typeInfo);
    typeInfo->GetDataType(&dataType);
    paramInfo->GetIOAttrib(&ioAttrib);

    switch(dataType) {
    case CarDataType_AString:
    case CarDataType_WString:
    free((void*)m_params[i]);
    break;
    case CarDataType_AStringBuf: {
    AStringBuf *pstrBuf;
    if (ParamIOAttrib_CallerAllocOut == ioAttrib) {
    pstrBuf = (AStringBuf*)m_params[i];
    }
    else {
    pstrBuf = *(AStringBuf**)m_params[i];
    }
    if (pstrBuf) AStringBuf::Free(pstrBuf);
    break;
    }
    case CarDataType_WStringBuf: {
    WStringBuf *pstrBuf;
    if (ParamIOAttrib_CallerAllocOut == ioAttrib) {
    pstrBuf = (WStringBuf*)m_params[i];
    }
    else {
    pstrBuf = *(WStringBuf**)m_params[i];
    }
    if (pstrBuf) WStringBuf::Free(pstrBuf);
    break;
    }
    case CarDataType_EMuid: {
    EMuid *val = (EMuid *)m_params[i];
    if (val) free((void*)val);
    break;
    }
    case CarDataType_EGuid: {
    EGuid *val = (EGuid *)m_params[i];
    if (val) {
    if (val->pUunm) {
    free(val->pUunm);
    }
    free((void*)val);
    }
    break;
    }
    case CarDataType_Interface: {
    if (ParamIOAttrib_In == ioAttrib) {
    PInterface object = (PInterface)m_params[i];
    object->Release();
    }
    else {
    PInterface object = *(PInterface*)m_params[i];
    if (object) object->Release();
    }
    break;
    }
    case CarDataType_Struct: {
    IVariableOfStruct *variable =
    (IVariableOfStruct *)m_params[i];
    if (1) {
    AStringBuf_<256>  name;
    typeInfo->GetName(&name);
    if (!strcmp("VARIANT", name)) {
    Variant * var;
    ec = variable->GetPayload((PVoid*)&var);
    if (SUCCEEDED(ec)) {
    var->Clear();
    }
    }
    }
    variable->Release();
    break;
    }
    case CarDataType_ArrayOf:
    case CarDataType_BufferOf: {
    ICarArrayInfo * piCarArrayInfo;
    piCarArrayInfo = ICarArrayInfo::Probe(typeInfo);
    if (ParamIOAttrib_CalleeAllocOut == ioAttrib) {
    IVariableOfCarArray *variable;
    piCarArrayInfo->CreateVariableBox( //todo
    (PCarQuintet)*(Int32*)m_params[i],
    &variable);
    FreeArray(piCarArrayInfo, variable);
    free((PCarQuintet)*(Int32*)m_params[i]);
    }
    else {
    FreeArray(piCarArrayInfo, (IVariableOfCarArray *)m_params[i]);
    }
    break;
    }
    default:
    break;
    }
    typeInfo->Release();
    }
    (*m_paramInfos)[i]->Release();
    }
    BufferOf<IParamInfo *>::Free(m_paramInfos);
    */
}
