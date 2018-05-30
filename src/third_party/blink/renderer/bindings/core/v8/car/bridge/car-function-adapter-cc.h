#ifndef __CAR_BRIDGE_CAR_FUNCTION_ADAPTER_CC_H
#define __CAR_BRIDGE_CAR_FUNCTION_ADAPTER_CC_H

#include <cstdarg>
#include <cstddef>
#include <memory>
#include <new>
#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "elastos-ext.h"
#include "error.h"

CAR_BRIDGE_NAMESPACE_BEGIN

template<class T>
Elastos::ECode CARFunctionAdapter::operator()(T firstArgument, ...) noexcept
{
    va_list ap;
    Elastos::ECode ec;
    va_start(ap, firstArgument);
    ec = Call(firstArgument, ap);
    va_end(ap);
    return ec;
}

template<class T>
Elastos::ECode CARFunctionAdapter::Call(T firstArgument, ...) noexcept
{
    va_list ap;
    Elastos::ECode ec;
    va_start(ap, firstArgument);
    ec = Call(firstArgument, ap);
    va_end(ap);
    return ec;
}

template<class T>
Elastos::ECode CARFunctionAdapter::Call(T firstArgument, va_list ap) noexcept
{
#if 0//?try
    try {
#endif
        Nan::HandleScope scope;
        size_t argc;
        argc = _paramInfos->GetLength();
        ::std::unique_ptr<v8::Local<v8::Value> []> argv(new(::std::nothrow) v8::Local<v8::Value>[argc]);
        if (argv == nullptr)
            Throw_LOG(Error::NO_MEMORY, 0);
        SetArgumentOf((*_paramInfos)[0], argc, argv.get(), 0, &firstArgument);
        for (size_t i = 1; i < argc; ++i)
            SetArgumentOf((*_paramInfos)[i], argc, argv.get(), i, ap);
        CallFunction(argc, argv.get());
#if 0//?try
    }
    catch (Error const &error)
    {
        return ToECode(error);
    }
    catch (...)
    {
        return E_FAILED;
    }
#endif
    return NO_ERROR;
}
CAR_BRIDGE_NAMESPACE_END
#endif
