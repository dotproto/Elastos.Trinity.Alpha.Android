#ifndef __CAR_BRIDGE_ELASTOS_EXT_H
# define __CAR_BRIDGE_ELASTOS_EXT_H

# include <elastos.h>

# include "macros.h"



# define NO_ERROR NOERROR

# define E_FAILED E_FAIL

CAR_BRIDGE_NAMESPACE_BEGIN

extern char const *GetEName(_ELASTOS ECode ecode) noexcept;

extern _ELASTOS ECode GetFullName(IDataTypeInfo *dataTypeInfo, _ELASTOS String *fullName) noexcept;

enum ClassCategory {
    CLASS_CATEGORY_CLASS,
    CLASS_CATEGORY_GENERIC,
    CLASS_CATEGORY_REGIME,
    CLASS_CATEGORY_ASPECT
};

extern _ELASTOS ECode GetCategory(IClassInfo *classInfo, enum ClassCategory *category) noexcept;

extern _ELASTOS ECode GetImportedModuleCount(IModuleInfo *moduleInfo, _ELASTOS Int32 *count) noexcept;

extern _ELASTOS ECode GetAllImportedModuleInfos(IModuleInfo *moduleInfo,
        _ELASTOS ArrayOf<IModuleInfo *> *importedModuleInfos) noexcept;

extern _ELASTOS ECode HasImportedModule(IModuleInfo *moduleInfo,
        _ELASTOS String const &path,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode GetImportedModuleInfo(IModuleInfo *moduleInfo,
        _ELASTOS String const &path,
        IModuleInfo **importedModuleInfo) noexcept;

extern _ELASTOS ECode HasConstant(IModuleInfo *moduleInfo,
        _ELASTOS String const &name,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasEnum(IModuleInfo *moduleInfo,
        _ELASTOS String const &fullName,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasStruct(IModuleInfo *moduleInfo,
        _ELASTOS String const &name,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasTypeAlias(IModuleInfo *moduleInfo,
        _ELASTOS String const &name,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasInterface(IModuleInfo *moduleInfo,
        _ELASTOS String const &fullName,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasClass(IModuleInfo *moduleInfo,
        _ELASTOS String const &fullName,
        _ELASTOS Boolean *has) noexcept;

CAR_BRIDGE_NAMESPACE_END

#endif
