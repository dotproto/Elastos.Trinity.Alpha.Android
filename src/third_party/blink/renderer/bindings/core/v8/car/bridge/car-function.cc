
#include <cstddef>
#include <cstdlib>

#include <map>
#include <string>

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
static map<AutoPtr<IFunctionInfo>, CopyablePersistent<Object>> _mapFunctionInfoToCARFunction;
#pragma clang diagnostic pop

static Local<Object> _CARFunction(IFunctionInfo *functionInfo, const char *what)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> function;

    ECode ec;

    _ELASTOS String name;

    _ELASTOS String annotation;

    _ELASTOS Int32 nParams;

    AutoPtr<ArrayOf<IParamInfo *> > paramInfos;

    auto &_function = _mapFunctionInfoToCARFunction[functionInfo];
    if (!_function.IsEmpty())
        return scope.Escape(New(_function));

    function = New<Object>();

    DefineOwnProperty(function,
            New("$what").ToLocalChecked(),
            New(what).ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = functionInfo->GetName(&name);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    DefineOwnProperty(function,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

#if 0
    ec = functionInfo->GetAnnotation(&annotation);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    DefineOwnProperty(function,
            New("$annotation").ToLocalChecked(),
            ToValue(annotation),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

#endif
    ec = functionInfo->GetParamCount(&nParams);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    paramInfos = ArrayOf<IParamInfo *>::Alloc(nParams);
    if (paramInfos == 0)
        Throw_LOG(Error::NO_MEMORY, 0);

    ec = functionInfo->GetAllParamInfos(reinterpret_cast<ArrayOf<IParamInfo *> *>(paramInfos.Get()));
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    for (_ELASTOS Int32 i = 0; i < nParams; ++i) {
        ::Nan::HandleScope scope_;

        IParamInfo *paramInfo;

        Local<Object> param;

        _ELASTOS String paramName;

        _ELASTOS Int32 index;

        ParamIOAttribute io;
        const char *_io;

        _ELASTOS Boolean isReturnValue;

        AutoPtr<IDataTypeInfo> typeInfo;
        IDataTypeInfo *_typeInfo;

        paramInfo = (*paramInfos)[i];

        param = New<Object>();

        DefineOwnProperty(param,
                New("$what").ToLocalChecked(),
                New("CARParameter").ToLocalChecked(),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = paramInfo->GetName(&paramName);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        DefineOwnProperty(param,
                New("$name").ToLocalChecked(),
                ToValue(paramName),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = paramInfo->GetIndex(&index);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        DefineOwnProperty(param,
                New("$index").ToLocalChecked(),
                ToValue(index),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = paramInfo->GetIOAttribute(&io);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        if (io == ParamIOAttribute_In)
            _io = "Input";
        else if (io == ParamIOAttribute_CalleeAllocOut)
            _io = "CalleeAllocOutput";
        else if (io == ParamIOAttribute_CallerAllocOut)
            _io = "CallerAllocOutput";
        else
            abort();

        DefineOwnProperty(param,
                New("$io").ToLocalChecked(),
                New(_io).ToLocalChecked(),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = paramInfo->IsReturnValue(&isReturnValue);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        DefineOwnProperty(param,
                New("$isReturnValue").ToLocalChecked(),
                ToValueFromBoolean(isReturnValue),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = paramInfo->GetTypeInfo(&_typeInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        typeInfo = _typeInfo, _typeInfo->Release();

        DefineOwnProperty(param,
                New("$type").ToLocalChecked(),
                CARDataType(typeInfo),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        DefineOwnProperty(function,
                ToValue(index).As<::v8::String>(),
                param,
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        DefineOwnProperty(function,
                ToValue(paramName).As<::v8::String>(),
                param,
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }

    _function.Reset(function);

    return scope.Escape(function);
}

static Local<Object> _CARFunction(size_t nFunctionInfos, IFunctionInfo *functionInfos[], const char *what)
{
    ::Nan::EscapableHandleScope scope;

    ECode ec;

    _ELASTOS String name;

    Local<Object> functionCandidates;

    if (nFunctionInfos == 0)
        Throw_LOG(Error::INVALID_ARGUMENT, 0);

    if (nFunctionInfos == 1)
        return _CARFunction(functionInfos[0], what);

    ec = functionInfos[0]->GetName(&name);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    for (size_t i = 1; i < nFunctionInfos; ++i) {
        _ELASTOS String _name;

        ec = functionInfos[i]->GetName(&_name);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        if (_name != name)
            Throw_LOG(Error::INVALID_ARGUMENT, 0);
    }

    functionCandidates = New<Object>();

    DefineOwnProperty(functionCandidates,
            New("$what").ToLocalChecked(),
            New(string(what) + "+").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    DefineOwnProperty(functionCandidates,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    for (size_t i = 0; i < nFunctionInfos; ++i)
        DefineOwnProperty(functionCandidates,
                ToValue(i).As<::v8::String>(),
                _CARFunction(functionInfos[i], what),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    return scope.Escape(functionCandidates);
}

Local<Object> CARConstructor(IConstructorInfo *constructorInfo)
{
    return _CARFunction(static_cast<IFunctionInfo *>(constructorInfo), "CARConstructor");
}

Local<Object> CARConstructor(size_t nConstructorInfos, IConstructorInfo *constructorInfos[])
{
    return _CARFunction(
            nConstructorInfos, reinterpret_cast<IFunctionInfo **>(constructorInfos),
            "CARConstructor");
}

Local<Object> CARMethod(IMethodInfo *methodInfo)
{
    return _CARFunction(static_cast<IFunctionInfo *>(methodInfo), "CARMethod");
}

Local<Object> CARMethod(size_t nMethodInfos, IMethodInfo *methodInfos[])
{
    return _CARFunction(nMethodInfos, reinterpret_cast<IFunctionInfo **>(methodInfos), "CARMethod");
}

Local<Object> CARCallbackMethod(ICallbackMethodInfo *callbackMethodInfo)
{
    return _CARFunction(static_cast<IFunctionInfo *>(callbackMethodInfo), "CARCallbackMethod");
}

Local<Object> CARCallbackMethod(size_t nCallbackMethodInfos, ICallbackMethodInfo const * callbackMethodInfos[])
{
    return _CARFunction(
            nCallbackMethodInfos, reinterpret_cast<IFunctionInfo **>(const_cast<ICallbackMethodInfo**>(callbackMethodInfos)),
            "CARCallbackMethod");
}

CAR_BRIDGE_NAMESPACE_END

