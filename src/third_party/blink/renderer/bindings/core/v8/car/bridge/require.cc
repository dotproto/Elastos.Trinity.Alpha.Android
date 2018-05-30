#include <cstddef>
#include <cstdint>
#include <cstring>
#include <nan.h>
#include <elastos.h>
#include "macros.h"
#include "elastos-ext.h"
#include "car-constant.h"
#include "car-data-type.h"
#include "car-imported-module.h"
#include "car-module.h"
#include "car-namespace.h"
#include "car-object.h"
#include "car-type-alias.h"
#include "error.h"
#include "base/logging.h"

using namespace Nan;
using namespace v8;

_ELASTOS_NAMESPACE_USING
CAR_BRIDGE_NAMESPACE_BEGIN

static Local<Value> _Require(IModuleInfo *moduleInfo, char const *entryId)
{
    Nan::EscapableHandleScope scope;

    ECode ec;
    Elastos::Boolean hasImportedModule;
    Elastos::Boolean hasEnum;
    Elastos::Boolean hasStruct;
    Elastos::Boolean hasTypeAlias;
    Elastos::Boolean hasInterface;
    Elastos::Boolean hasClass;

    ec = HasImportedModule(moduleInfo, Elastos::String(entryId), &hasImportedModule);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasImportedModule != FALSE)
    {
        AutoPtr<IModuleInfo> importedModuleInfo;
        IModuleInfo *_importedModuleInfo;
        ec = GetImportedModuleInfo(moduleInfo, Elastos::String(entryId), &_importedModuleInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);
        importedModuleInfo = _importedModuleInfo, _importedModuleInfo->Release();
        return scope.Escape(NewInstance(CARImportedModuleTemplate(importedModuleInfo)).ToLocalChecked());
    }

    if (strncmp(entryId, "$const.", 7) == 0)
    {
        char const *_entryId;
        Elastos::Boolean hasConstant;
        AutoPtr<IConstantInfo> constantInfo;
        IConstantInfo *_constantInfo;
        _entryId = entryId + 7;

        ec = HasConstant(moduleInfo, Elastos::String(_entryId), &hasConstant);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        if (hasConstant == FALSE)
            Throw_LOG(Error::NONENTITY, 0);

        ec = const_cast<IModuleInfo *>(moduleInfo)->GetConstantInfo(Elastos::String(_entryId), &_constantInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        constantInfo = _constantInfo, _constantInfo->Release();
        return CARConstant(constantInfo);
    }

    ec = HasEnum(moduleInfo, Elastos::String(entryId), &hasEnum);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasEnum != FALSE)
    {
        AutoPtr<IEnumInfo> enumInfo;
        IEnumInfo *_enumInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetEnumInfo(Elastos::String(entryId), &_enumInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        enumInfo = _enumInfo, _enumInfo->Release();
        return CAREnum(enumInfo);
    }

    ec = HasStruct(moduleInfo, Elastos::String(entryId), &hasStruct);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasStruct != FALSE)
    {
        AutoPtr<IStructInfo> structInfo;
        IStructInfo *_structInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetStructInfo(Elastos::String(entryId), &_structInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);
        structInfo = _structInfo, _structInfo->Release();
        return CARStruct(structInfo);
    }

    ec = HasTypeAlias(moduleInfo, Elastos::String(entryId), &hasTypeAlias);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasTypeAlias != FALSE)
    {
        AutoPtr<ITypeAliasInfo> typeAliasInfo;
        ITypeAliasInfo *_typeAliasInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetTypeAliasInfo(Elastos::String(entryId), &_typeAliasInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        typeAliasInfo = _typeAliasInfo, _typeAliasInfo->Release();
        return CARTypeAlias(typeAliasInfo);
    }

    ec = HasInterface(moduleInfo, Elastos::String(entryId), &hasInterface);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasInterface != FALSE)
    {
        AutoPtr<IInterfaceInfo > interfaceInfo;
        IInterfaceInfo *_interfaceInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetInterfaceInfo(Elastos::String(entryId), &_interfaceInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);
        interfaceInfo = _interfaceInfo, _interfaceInfo->Release();
        return CARInterface(interfaceInfo);
    }

    ec = HasClass(moduleInfo, Elastos::String(entryId), &hasClass);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasClass != FALSE)
    {
        AutoPtr<IClassInfo> classInfo;
        IClassInfo *_classInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetClassInfo(Elastos::String(entryId), &_classInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);
        classInfo = _classInfo, _classInfo->Release();
        return scope.Escape(GetFunction(CARObject::NewClassTemplate(classInfo)).ToLocalChecked());
    }
    return scope.Escape(NewInstance(CARNamespaceTemplate(moduleInfo, entryId)).ToLocalChecked());
}

Local<Value> Require(char const *ecoPath,
                     uint32_t major, uint32_t minor, uint32_t build, uint32_t revision,
                     size_t nEntryIds, char const *const entryIds[])
{
    ECode ec;
    Local<Array> entries;
    AutoPtr<IModuleInfo> pMdlInfo;
    Elastos::Int32 _major, _minor, _build, _revision;
    ec = CReflector::AcquireModuleInfo(Elastos::String(ecoPath), (IModuleInfo **)&pMdlInfo);
    if (FAILED(ec))
    {
        LOG(ERROR) << "Require: AcquireModuleInfo" << ecoPath << "failed";
        return entries;
    }
    ec = pMdlInfo->GetVersion(&_major, &_minor, &_build, &_revision);
    if (FAILED(ec))
    {
        LOG(ERROR) << "Require: GetVersion failed";
        return entries;
    }
    LOG(INFO) << "Require: GetVersion " << _major << "." << _minor << "." << _build << "_" << _revision;
    LOG(INFO) << "nEntryIds " << nEntryIds << " " << entryIds;
#if 0
    if ((uint32_t)_major != major)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
    if ((uint32_t)_minor < minor)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
    if ((uint32_t)_build != build)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
    if ((uint32_t)_minor == minor && (uint32_t)_revision < revision)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
#endif
    if (nEntryIds == 0)
    {
        Nan::EscapableHandleScope scope;
        return scope.Escape(NewInstance(CARModuleTemplate(pMdlInfo)).ToLocalChecked());
    }

    if (nEntryIds == 1)
        return _Require(pMdlInfo, entryIds[0]);

    entries = New<Array>(nEntryIds);

    for (size_t i = 0; i < nEntryIds; ++i)
    {
        Nan::HandleScope scope;
        Nan::Set(entries, i, _Require(pMdlInfo, entryIds[i]));
    }
    return entries;
}

CAR_BRIDGE_NAMESPACE_END
