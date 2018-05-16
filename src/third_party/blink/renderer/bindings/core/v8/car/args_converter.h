#ifndef __ARGS_CONVERTER_H__
#define __ARGS_CONVERTER_H__

#include <v8.h>
#include <elastos.h>

#define MAX_URL_LEN                  256
#define MAX_SYMBOL_LEN               128
#define MAX_PARAMETER_COUNT          32
#define MAX_IMPLEMENTATED_INTERFACE  16
#define DEFAULT_STRINGBUFF_LEN       1024

#define MAX_PREFERENCE_NAME          64
#define MAX_PREFERENCE_TITLE         128
#define MAX_PREFERENCE_STYLE         128

class V8ParameterNormalizer
{
public:
    V8ParameterNormalizer(v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value> args);
    ECode Normalize(IFunctionInfo *pFunctionInfo, IArgumentList *pArgumentList);
    //ECode GetReturnValue(JSValueRef *rval);
    ~V8ParameterNormalizer();

private:
    void FreeParams();
    //ECode NativeVal2JsVal(IParamInfo *paramInfo, void *nativeVal, JSValueRef *rval, Boolean isCallerAllocOut);

private:
    //v8::Isolate*            m_isolate;
    v8::FunctionCallbackInfo<v8::Value> args;
    char                    m_paramBuff[MAX_PARAMETER_COUNT * 8];
    Int32                   m_params[MAX_PARAMETER_COUNT];
    //Int32                   m_outParamCount;
    ArrayOf<IParamInfo*>*   m_paramInfos;
};

#endif // __ARGS_CONVERTER_H__
