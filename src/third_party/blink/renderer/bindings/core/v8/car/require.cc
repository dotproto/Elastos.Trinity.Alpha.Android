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

using namespace Nan;
using namespace v8;

_ELASTOS_NAMESPACE_USING
CAR_BRIDGE_NAMESPACE_BEGIN

static Local<Value> _Require(IModuleInfo *moduleInfo, char const *entryId)
{
    Nan::EscapableHandleScope scope;

    ECode ec;
    //Elastos::Boolean hasImportedModule;
    Elastos::Boolean hasEnum;
    Elastos::Boolean hasStruct;
    Elastos::Boolean hasTypeAlias;
    Elastos::Boolean hasInterface;
    Elastos::Boolean hasClass;

    Debug_LOG("debug entryId: %s", entryId);
    Int32 nClassCnt;
    ec = moduleInfo->GetClassCount(&nClassCnt);
    if (FAILED(ec)) {
		Throw_LOG(Error::TYPE_ELASTOS, ec);
    }
    ArrayOf<IClassInfo*>* pClassInfoArray = ArrayOf<IClassInfo*>::Alloc(nClassCnt);
    moduleInfo->GetAllClassInfos(pClassInfoArray);
    if (FAILED(ec)) {
		Throw_LOG(Error::TYPE_ELASTOS, ec);
    }

    Debug_LOG("nClassCnt: %d", nClassCnt);
    for(int i = 0 ; i < nClassCnt ; i++)  {
	    IClassInfo* pClassInfo = (*pClassInfoArray)[i];
		Elastos::String className;
		pClassInfo->GetName(&className);
		if (FAILED(ec)) {
			Throw_LOG(Error::TYPE_ELASTOS, ec);
		}

        // get all method
		Int32 nMtdCnt;
		ec = pClassInfo->GetMethodCount(&nMtdCnt);
		if (FAILED(ec)) {
			Throw_LOG(Error::TYPE_ELASTOS, ec);
		}
		Debug_LOG("calss:%s has method %d", className.string(), nMtdCnt);

        ArrayOf<IMethodInfo*>* pMethodInfoArray = ArrayOf<IMethodInfo*>::Alloc(nMtdCnt);
        ec = pClassInfo->GetAllMethodInfos(pMethodInfoArray);
        if (FAILED(ec)) {
			Throw_LOG(Error::TYPE_ELASTOS, ec);
        }

		for(int j = 0 ; j < nMtdCnt ; j++)	{
			IMethodInfo* pMethodInfo = (*pMethodInfoArray)[j];
			Elastos::String szMtdName;
			ec = pMethodInfo->GetName(&szMtdName);
			if (FAILED(ec)) {
				Throw_LOG(Error::TYPE_ELASTOS, ec);
			}
			Debug_LOG("    %s", szMtdName.string());
		}
    }

#if 0//ignore ImportModule
    ec = HasImportedModule(moduleInfo, Elastos::String(entryId), &hasImportedModule);
    if (FAILED(ec)) {
        Throw_LOG(Error::TYPE_ELASTOS, ec);
   	}

    if (hasImportedModule != FALSE)
    {
        AutoPtr<IModuleInfo> importedModuleInfo;
        IModuleInfo *_importedModuleInfo;
        ec = GetImportedModuleInfo(moduleInfo, Elastos::String(entryId), &_importedModuleInfo);
        if (FAILED(ec)) {
            Throw_LOG(Error::TYPE_ELASTOS, ec);
        }

        importedModuleInfo = _importedModuleInfo, _importedModuleInfo->Release();
        return scope.Escape(NewInstance(CARImportedModuleTemplate(importedModuleInfo)).ToLocalChecked());
    }
#endif
    if (strncmp(entryId, "$const.", 7) == 0)
    {
        Debug_LOG("debug");
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

    Debug_LOG("");
    if (hasEnum != FALSE)
    {
    Debug_LOG("");
        AutoPtr<IEnumInfo> enumInfo;
        IEnumInfo *_enumInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetEnumInfo(Elastos::String(entryId), &_enumInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        enumInfo = _enumInfo, _enumInfo->Release();
        return CAREnum(enumInfo);
    }

    Debug_LOG("");
    ec = HasStruct(moduleInfo, Elastos::String(entryId), &hasStruct);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasStruct != FALSE)
    {
    Debug_LOG("");
        AutoPtr<IStructInfo> structInfo;
        IStructInfo *_structInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetStructInfo(Elastos::String(entryId), &_structInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        structInfo = _structInfo, _structInfo->Release();
        return CARStruct(structInfo);
    }

    Debug_LOG("");
    ec = HasTypeAlias(moduleInfo, Elastos::String(entryId), &hasTypeAlias);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasTypeAlias != FALSE)
    {
    Debug_LOG("");
        AutoPtr<ITypeAliasInfo> typeAliasInfo;
        ITypeAliasInfo *_typeAliasInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetTypeAliasInfo(Elastos::String(entryId), &_typeAliasInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        typeAliasInfo = _typeAliasInfo, _typeAliasInfo->Release();
        return CARTypeAlias(typeAliasInfo);
    }

    Debug_LOG("");
    ec = HasInterface(moduleInfo, Elastos::String(entryId), &hasInterface);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasInterface != FALSE)
    {
    Debug_LOG("");
        AutoPtr<IInterfaceInfo > interfaceInfo;
        IInterfaceInfo *car_interfaceInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetInterfaceInfo(Elastos::String(entryId), &car_interfaceInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        interfaceInfo = car_interfaceInfo, car_interfaceInfo->Release();
		Debug_LOG("CARInterface %s", entryId);

        return scope.Escape(CARInterface(interfaceInfo));
    }

    Debug_LOG("");
    ec = HasClass(moduleInfo, Elastos::String(entryId), &hasClass);
    if (FAILED(ec))
        Throw_LOG(Error::TYPE_ELASTOS, ec);

    if (hasClass != FALSE)
    {
    Debug_LOG("");
        AutoPtr<IClassInfo> classInfo;
        IClassInfo *_classInfo;
        ec = const_cast<IModuleInfo *>(moduleInfo)->GetClassInfo(Elastos::String(entryId), &_classInfo);
        if (FAILED(ec))
            Throw_LOG(Error::TYPE_ELASTOS, ec);

        classInfo = _classInfo, _classInfo->Release();
		Debug_LOG("NewClassTemplate %s", entryId);
        return scope.Escape(GetFunction(CARObject::NewClassTemplate(classInfo)).ToLocalChecked());
    }

    Debug_LOG("");
    return scope.Escape(NewInstance(CARNamespaceTemplate(moduleInfo, entryId)).ToLocalChecked());
}

Local<Value> Require(char const *ecoPath, char const *const entryId)
{
    ECode ec;
    Local<Array> entries;
    AutoPtr<IModuleInfo> pMdlInfo;
    Elastos::Int32 _major = 0, _minor = 0, _build = 0, _revision = 0;

    Debug_LOG("Require %s %s", ecoPath, entryId);
    ec = CReflector::AcquireModuleInfo(Elastos::String(ecoPath), (IModuleInfo **)&pMdlInfo);
    if (FAILED(ec))
    {
        Debug_LOG("Require: AcquireModuleInfo %s failed", ecoPath);
        return entries;
    }

    ec = pMdlInfo->GetVersion(&_major, &_minor, &_build, &_revision);
    if (FAILED(ec))
    {
		Debug_LOG("%s", "Require: GetVersion failed");
        return entries;
    }

#if 0//ParseURI is not implemented.
    if ((uint32_t)_major != major)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
    if ((uint32_t)_minor < minor)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
    if ((uint32_t)_build != build)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);
    if ((uint32_t)_minor == minor && (uint32_t)_revision < revision)
        Throw_LOG(Error::INCOMPATIBLE_VERSION, 0);

    if (nEntryIds == 0)
    {
        Nan::EscapableHandleScope scope;
        return scope.Escape(NewInstance(CARModuleTemplate(pMdlInfo)).ToLocalChecked());
    }
#endif

    return _Require(pMdlInfo, entryId);

#if 0
    entries = New<Array>(nEntryIds);

    for (size_t i = 0; i < nEntryIds; ++i)
    {
        Nan::HandleScope scope;
        Nan::Set(entries, i, _Require(pMdlInfo, entryIds[i]));
    }

    return entries;
#endif
}

CAR_BRIDGE_NAMESPACE_END
