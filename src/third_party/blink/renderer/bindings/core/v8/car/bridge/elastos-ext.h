#ifndef __CAR_BRIDGE_ELASTOS_EXT_H
#define __CAR_BRIDGE_ELASTOS_EXT_H
#include <elastos.h>
#include "macros.h"

#define NO_ERROR NOERROR
#define E_FAILED E_FAIL

CAR_BRIDGE_NAMESPACE_BEGIN

extern char const *GetEName(Elastos::ECode ecode) noexcept;
extern Elastos::ECode GetFullName(IDataTypeInfo *dataTypeInfo, Elastos::String *fullName) noexcept;

enum ClassCategory
{
    CLASS_CATEGORY_CLASS,
    CLASS_CATEGORY_GENERIC,
    CLASS_CATEGORY_REGIME,
    CLASS_CATEGORY_ASPECT
};

extern Elastos::ECode GetCategory(IClassInfo *classInfo, enum ClassCategory *category) noexcept;
extern Elastos::ECode GetImportedModuleCount(IModuleInfo *moduleInfo, Elastos::Int32 *count) noexcept;
extern Elastos::ECode GetAllImportedModuleInfos(IModuleInfo *moduleInfo,
        Elastos::ArrayOf<IModuleInfo *> *importedModuleInfos) noexcept;
extern Elastos::ECode HasImportedModule(IModuleInfo *moduleInfo,
                                        Elastos::String const &path,
                                        Elastos::Boolean *has) noexcept;
extern Elastos::ECode GetImportedModuleInfo(IModuleInfo *moduleInfo,
        Elastos::String const &path,
        IModuleInfo **importedModuleInfo) noexcept;
extern Elastos::ECode HasConstant(IModuleInfo *moduleInfo,
                                  Elastos::String const &name,
                                  Elastos::Boolean *has) noexcept;
extern Elastos::ECode HasEnum(IModuleInfo *moduleInfo,
                              Elastos::String const &fullName,
                              Elastos::Boolean *has) noexcept;
extern Elastos::ECode HasStruct(IModuleInfo *moduleInfo,
                                Elastos::String const &name,
                                Elastos::Boolean *has) noexcept;
extern Elastos::ECode HasTypeAlias(IModuleInfo *moduleInfo,
                                   Elastos::String const &name,
                                   Elastos::Boolean *has) noexcept;
extern Elastos::ECode HasInterface(IModuleInfo *moduleInfo,
                                   Elastos::String const &fullName,
                                   Elastos::Boolean *has) noexcept;
extern Elastos::ECode HasClass(IModuleInfo *moduleInfo,
                               Elastos::String const &fullName,
                               Elastos::Boolean *has) noexcept;
CAR_BRIDGE_NAMESPACE_END
#endif
