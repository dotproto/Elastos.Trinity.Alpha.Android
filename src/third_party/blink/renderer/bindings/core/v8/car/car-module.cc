#include <cstdio>
#include <map>
#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "nan-ext.h"
#include "elastos-ext.h"
#include "car-constant.h"
#include "car-data-type.h"
#include "car-imported-module.h"
#include "car-namespace.h"
#include "car-type-alias.h"
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
static map<AutoPtr<IModuleInfo >, CopyablePersistent<ObjectTemplate>> _mapModuleInfoToCARModule;
#pragma clang diagnostic pop

Local<ObjectTemplate> CARModuleTemplate(IModuleInfo const *pmoduleInfo)
{
    Nan::EscapableHandleScope scope;
    Local<ObjectTemplate> moduleTemplate;
    ECode ec;
    Elastos::String path;
    Elastos::Int32 major=0, minor=0, build=0, revision=0;
    Elastos::Int32 nImportedModules;
    Elastos::Int32 nConstants;
    Elastos::Int32 nStructs;
    Elastos::Int32 nTypeAliases;

    IModuleInfo *moduleInfo = const_cast<IModuleInfo *>(pmoduleInfo);
    auto &_moduleTemplate = _mapModuleInfoToCARModule[moduleInfo];
    if (!_moduleTemplate.IsEmpty())
        return scope.Escape(New(_moduleTemplate));

    Debug_LOG("%s", "CARModuleTemplate start");
    moduleTemplate = CARNamespaceTemplate(moduleInfo, "");
    ec = moduleInfo->GetPath(&path);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    Debug_LOG("set $path: %s", path.string());
    SetTemplate(moduleTemplate,
                New("$path").ToLocalChecked(),
                ToValue(path),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = moduleInfo->GetVersion(&major, &minor, &build, &revision);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    char version[48];
    sprintf(version, "%d.%d.%d.%d", major, minor, build, revision);
	Debug_LOG("set $version: %s", version);
    SetTemplate(moduleTemplate,
                New("$version").ToLocalChecked(),
                New(version).ToLocalChecked(),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = GetImportedModuleCount(moduleInfo, &nImportedModules);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (nImportedModules > 0)
    {
	    Debug_LOG("imported modules count: %d", nImportedModules);
        AutoPtr<ArrayOf<IModuleInfo *> > importedModuleInfos;
        importedModuleInfos = ArrayOf<IModuleInfo *>::Alloc(nImportedModules);
        if (importedModuleInfos == 0)
            Throw_LOG(Error::NO_MEMORY, 0);

        ec = GetAllImportedModuleInfos(moduleInfo, importedModuleInfos);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        for (Elastos::Int32 i = 0; i < nImportedModules; ++i)
        {
            Nan::HandleScope scope_;
            IModuleInfo *importedModuleInfo;
            Elastos::String importedModulePath;

            importedModuleInfo = (*importedModuleInfos)[i];
            ec = importedModuleInfo->GetPath(&importedModulePath);
            if (FAILED(ec))
                Throw_LOG(Error::TYPE_ELASTOS, ec);
#if 0//?jw
            SetTemplate(moduleTemplate,
                        ToValue(importedModulePath).As<v8::String>(),
                        CARImportedModuleTemplate(importedModuleInfo),
                        static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
#endif
        }
    }
    ec = moduleInfo->GetConstantCount(&nConstants);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (nConstants > 0)
    {
	    Debug_LOG("constants count: %d", nConstants);
        AutoPtr<ArrayOf<IConstantInfo *> > constantInfos;
        Local<Object> constants;
        constantInfos = ArrayOf<IConstantInfo *>::Alloc(nConstants);
        if (constantInfos == 0)
            Throw_LOG(Error::NO_MEMORY, 0);

        ec = moduleInfo->GetAllConstantInfos(reinterpret_cast<ArrayOf<IConstantInfo *> *>(constantInfos.Get()));
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        constants = New<Object>();
        for (Elastos::Int32 i = 0; i < nConstants; ++i)
        {
            Nan::HandleScope scope_;
            IConstantInfo *constantInfo;
            Elastos::String constantName;
            constantInfo = (*constantInfos)[i];
            ec = constantInfo->GetName(&constantName);
            if (FAILED(ec))
                Throw_LOG(Error::TYPE_ELASTOS, ec);

            DefineOwnProperty(constants,
                              ToValue(constantName).As<v8::String>(),
                              CARConstant(constantInfo),
                              static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        }

        SetTemplate(moduleTemplate,
                    New("$const").ToLocalChecked(),
                    constants,
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }
    ec = moduleInfo->GetStructCount(&nStructs);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (nStructs > 0)
    {
	    Debug_LOG("structs count: %d", nStructs);
        AutoPtr<ArrayOf<IStructInfo *> > structInfos;
        structInfos = ArrayOf<IStructInfo *>::Alloc(nStructs);
        if (structInfos == 0)
            Throw_LOG(Error::NO_MEMORY, 0);

        ec = moduleInfo->GetAllStructInfos(reinterpret_cast<ArrayOf<IStructInfo *> *>(structInfos.Get()));
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        for (Elastos::Int32 i = 0; i < nStructs; ++i)
        {
            Nan::HandleScope scope_;
            IStructInfo *structInfo;
            Elastos::String structName;
            structInfo = (*structInfos)[i];
            ec = structInfo->GetName(&structName);
            if (FAILED(ec))
                Throw_LOG(Error::TYPE_ELASTOS, ec);

            SetTemplate(moduleTemplate,
                        ToValue(structName).As<v8::String>(),
                        CARStruct(structInfo),
                        static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        }
    }
    ec = moduleInfo->GetTypeAliasCount(&nTypeAliases);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (nTypeAliases > 0)
    {
	    Debug_LOG("type aliases count: %d", nTypeAliases);
        AutoPtr<ArrayOf<ITypeAliasInfo *> > typeAliasInfos;
        typeAliasInfos = ArrayOf<ITypeAliasInfo *>::Alloc(nTypeAliases);
        if (typeAliasInfos == 0)
            Throw_LOG(Error::NO_MEMORY, 0);

        ec = moduleInfo->GetAllTypeAliasInfos(reinterpret_cast<ArrayOf<ITypeAliasInfo *> *>(typeAliasInfos.Get()));
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        for (Elastos::Int32 i = 0; i < nTypeAliases; ++i)
        {
            Nan::HandleScope scope_;
            ITypeAliasInfo *typeAliasInfo;
            Elastos::String typeAliasName;
            typeAliasInfo = (*typeAliasInfos)[i];
            ec = typeAliasInfo->GetName(&typeAliasName);
            if (FAILED(ec))
                Throw_LOG(Error::TYPE_ELASTOS, ec);

#if 0//?jw
            SetTemplate(moduleTemplate,
                        ToValue(typeAliasName).As<v8::String>(),
                        CARTypeAlias(typeAliasInfo),
                        static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
#endif
        }
    }

    _moduleTemplate.Reset(moduleTemplate);
    return scope.Escape(moduleTemplate);
}

CAR_BRIDGE_NAMESPACE_END
