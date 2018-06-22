#include <elastos.h>
#include "macros.h"
#include "elastos-ext.h"
#include "error.h"

_ELASTOS_NAMESPACE_USING
CAR_BRIDGE_NAMESPACE_BEGIN

const char *GetEName(ECode ecode) noexcept
{
    switch (ERROR(ecode))
    {
    case NOERROR:
        return "NOERROR";
    case E_DOES_NOT_EXIST:
        return "E_DOES_NOT_EXIST";
    case E_INVALID_OPERATION:
        return "E_INVALID_OPERATION";
    case E_TIMED_OUT:
        return "E_TIMED_OUT";
    case E_INTERRUPTED:
        return "E_INTERRUPTED";
    case E_FILE_NOT_FOUND:
        return "E_FILE_NOT_FOUND";
    case E_NOT_SUPPORTED:
        return "E_NOT_SUPPORTED";
    case E_OUT_OF_MEMORY:
        return "E_OUT_OF_MEMORY";
    case E_INVALID_ARGUMENT:
        return "E_INVALID_ARGUMENT";
    case E_NOT_IMPLEMENTED:
        return "E_NOT_IMPLEMENTED";
    case E_NO_INTERFACE:
        return "E_NO_INTERFACE";
    case E_ABORTED:
        return "E_ABORTED";
    case E_FAIL:
        return "E_FAIL";
    case E_NO_DEFAULT_CTOR:
        return "E_NO_DEFAULT_CTOR";
    case E_CLASS_NO_AGGREGATION:
        return "E_CLASS_NO_AGGREGATION";
    case E_CLASS_NOT_AVAILABLE:
        return "E_CLASS_NOT_AVAILABLE";
    case E_ASPECT_CANNOT_AGGREGATE_ASPECT:
        return "E_ASPECT_CANNOT_AGGREGATE_ASPECT";
    case E_UNAGGREGATE_FAILED:
        return "E_UNAGGREGATE_FAILED";
    case E_NO_GENERIC:
        return "E_NO_GENERIC";
    case E_WRONG_GENERIC:
        return "E_WRONG_GENERIC";
    case E_ASPECT_REFUSE_TO_ATTACH:
        return "E_ASPECT_REFUSE_TO_ATTACH";
    case E_ZOMBIE_ASPECT:
        return "E_ZOMBIE_ASPECT";
    case E_DUPLICATE_ASPECT:
        return "E_DUPLICATE_ASPECT";
    case E_CONFLICT_WITH_LOCAL_KEYWORD:
        return "E_CONFLICT_WITH_LOCAL_KEYWORD";
    case E_NOT_IN_PROTECTED_ZONE:
        return "E_NOT_IN_PROTECTED_ZONE";
    case E_COMPONENT_CANNOT_UNLOAD_NOW:
        return "E_COMPONENT_CANNOT_UNLOAD_NOW";
    case E_NOT_CAR_COMPONENT:
        return "E_NOT_CAR_COMPONENT";
    case E_COMPONENT_NOT_LOADED:
        return "E_COMPONENT_NOT_LOADED";
    case E_REMOTE_FAIL:
        return "E_REMOTE_FAIL";
    case E_OUT_OF_NUMBER:
        return "E_OUT_OF_NUMBER";
    case E_DATAINFO_EXIST:
        return "E_DATAINFO_EXIST";
    case E_NO_CLASS_INFO:
        return "E_NO_CLASS_INFO";
    case E_NO_EXPORT_OBJECT:
        return "E_NO_EXPORT_OBJECT";
    case E_NO_IMPORT_OBJECT:
        return "E_NO_IMPORT_OBJECT";
    case E_MARSHAL_DATA_TRANSPORT_ERROR:
        return "E_MARSHAL_DATA_TRANSPORT_ERROR";
    case E_NOT_CALLBACK_THREAD:
        return "E_NOT_CALLBACK_THREAD";
    case E_CANCLE_BOTH_EVENTS:
        return "E_CANCLE_BOTH_EVENTS";
    case E_NO_DELEGATE_REGISTERED:
        return "E_NO_DELEGATE_REGISTERED";
    case E_DELEGATE_ALREADY_REGISTERED:
        return "E_DELEGATE_ALREADY_REGISTERED";
    case E_CALLBACK_CANCELED:
        return "E_CALLBACK_CANCELED";
    case E_CALLBACK_IS_BUSY:
        return "E_CALLBACK_IS_BUSY";
    case E_CONNECT_NOCONNECTION:
        return "E_CONNECT_NOCONNECTION";
    case E_CONNECT_ADVISELIMIT:
        return "E_CONNECT_ADVISELIMIT";
    case E_CONNECT_CANNOTCONNECT:
        return "E_CONNECT_CANNOTCONNECT";
    case E_CONNECT_OVERRIDDEN:
        return "E_CONNECT_OVERRIDDEN";
    default:
        return "";
    }
}

template<class NamespacedDataTypeInfo>
static ECode _GetFullName(NamespacedDataTypeInfo *namespacedDataTypeInfo, String *fullName) noexcept
{
    ECode ec;
    String namespace_;
    String name;

    if (namespacedDataTypeInfo == 0) {
			Debug_LOG("E_INVALID_ARGUMENT");
			return E_INVALID_ARGUMENT;
    }

    if (fullName == 0) {
	    Debug_LOG("E_INVALID_ARGUMENT");
	    return E_INVALID_ARGUMENT;
    }

    ec = namespacedDataTypeInfo->GetNamespace(&namespace_);
    if (FAILED(ec)){
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    ec = namespacedDataTypeInfo->GetName(&name);
    if (FAILED(ec)){
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *fullName = namespace_ + "." + name;
    return NOERROR;
}

ECode GetFullName(IDataTypeInfo *dataTypeInfo, String *fullName) noexcept
{
    ECode ec;
    CarDataType dataType;

    if (dataTypeInfo == 0){
	    Debug_LOG("E_INVALID_ARGUMENT");
	    return E_INVALID_ARGUMENT;
    }

    if (fullName == 0){
	    Debug_LOG("E_INVALID_ARGUMENT");
	    return E_INVALID_ARGUMENT;
    }

    ec = dataTypeInfo->GetDataType(&dataType);
    if (FAILED(ec)){
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    switch (dataType)
    {
    case CarDataType_Enum:
        ec = _GetFullName(static_cast<IEnumInfo *>(dataTypeInfo), fullName);
        if (FAILED(ec)){
		    Throw_LOG(Error::FAILED, ec);
            return ec;
        }

        break;
    case CarDataType_CppVector:
        break;

    case CarDataType_Interface:
        ec = _GetFullName(static_cast<IInterfaceInfo *>(dataTypeInfo), fullName);
        if (FAILED(ec)){
		    Throw_LOG(Error::FAILED, ec);
            return ec;
        }
        break;
    default:
        ec = dataTypeInfo->GetName(fullName);
        if (FAILED(ec)){
		    Throw_LOG(Error::FAILED, ec);
            return ec;
        }
    }
    return NOERROR;
}

ECode GetCategory(IClassInfo *classInfo, enum ClassCategory *category) noexcept
{
    ECode ec;
    Boolean isGeneric;
    Boolean isRegime;
    Boolean isAspect;

    if (classInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (category == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = classInfo->IsGeneric(&isGeneric);
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    if (isGeneric != FALSE)
    {
        *category = CLASS_CATEGORY_GENERIC;
        return NOERROR;
    }

    ec = classInfo->IsRegime(&isRegime);
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    if (isRegime != FALSE)
    {
        *category = CLASS_CATEGORY_REGIME;
        return NOERROR;
    }

    ec = classInfo->IsAspect(&isAspect);
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    if (isAspect != FALSE)
    {
        *category = CLASS_CATEGORY_ASPECT;
        return NOERROR;
    }

    *category = CLASS_CATEGORY_CLASS;
    return NOERROR;
}

ECode GetImportedModuleCount(IModuleInfo *moduleInfo, Int32 *count) noexcept
{
    if (moduleInfo == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (count == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    return moduleInfo->GetImportModuleInfoCount(count);
}

ECode GetAllImportedModuleInfos(IModuleInfo *moduleInfo,
                                ArrayOf<IModuleInfo *> *importedModuleInfos) noexcept
{
    if (moduleInfo == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (importedModuleInfos == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    return moduleInfo->GetAllImportModuleInfos(reinterpret_cast<ArrayOf<IModuleInfo *> *>(importedModuleInfos));
}

ECode GetImportedModuleInfo(IModuleInfo *moduleInfo,
                            String const &path,
                            IModuleInfo **importedModuleInfo) noexcept;

ECode HasImportedModule(IModuleInfo *moduleInfo, String const &path, Boolean *has) noexcept
{
    ECode ec;
    IModuleInfo* _moduleInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
	    return E_INVALID_ARGUMENT;
    }

    ec = GetImportedModuleInfo(moduleInfo, path, &_moduleInfo);
	Debug_LOG("ec:%d ?= %d", ec, E_DOES_NOT_EXIST);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }
	else if (FAILED(ec))
	{
	    Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *has = TRUE;
    return ec;
}

ECode GetImportedModuleInfo(IModuleInfo *moduleInfo,
                            String const &path,
                            IModuleInfo **importedModuleInfo) noexcept
{
    ECode ec;
    Int32 nImportedModules;
    AutoPtr<ArrayOf<IModuleInfo *> > importedModuleInfos;

    Debug_LOG("debug path:%s", path.string() );

    if (moduleInfo == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (importedModuleInfo == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = moduleInfo->GetImportModuleInfoCount(&nImportedModules);
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    importedModuleInfos = ArrayOf<IModuleInfo *>::Alloc(nImportedModules);
    if (importedModuleInfos == 0){
		Throw_LOG(Error::NO_MEMORY, ec);
        return ec;
    }

    ec = moduleInfo->GetAllImportModuleInfos(reinterpret_cast<ArrayOf<IModuleInfo *> *>(importedModuleInfos.Get()));
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    for (Int32 i = 0; i < nImportedModules; ++i)
    {
        IModuleInfo *_importedModuleInfo;
        String _path;

       _importedModuleInfo = (*importedModuleInfos)[i];
        ec = _importedModuleInfo->GetPath(&_path);
		Debug_LOG("importedModuleInfos[%d] path:%s", i, _path.string() );

        if (FAILED(ec)) {
			Throw_LOG(Error::FAILED, ec);
            return ec;
        }

        if (_path == path)
        {
            *importedModuleInfo = _importedModuleInfo;
            return NOERROR;
        }
    }

	Debug_LOG("Not find ImportedModuleInfo for %s ", path.string() );
    *importedModuleInfo = 0;
    return E_DOES_NOT_EXIST;
}

ECode HasConstant(IModuleInfo *moduleInfo, String const &name, Boolean *has) noexcept
{
    ECode ec;

    IConstantInfo* constantInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0){
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = moduleInfo->GetConstantInfo(name, &constantInfo);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    return ec;
}

ECode HasEnum(IModuleInfo *moduleInfo, String const &fullName, Boolean *has) noexcept
{
    ECode ec;
    IEnumInfo* enumInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = moduleInfo->GetEnumInfo(fullName, &enumInfo);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }

    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *has = TRUE;

    return ec;
}

ECode HasStruct(IModuleInfo *moduleInfo, String const &name, Boolean *has) noexcept
{
    ECode ec;

    IStructInfo* structInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetStructInfo(name, &structInfo);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }

    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *has = TRUE;
 
    return ec;
}

ECode HasTypeAlias(IModuleInfo *moduleInfo, String const &name, Boolean *has) noexcept
{
    ECode ec;
    ITypeAliasInfo* typeAliasInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = moduleInfo->GetTypeAliasInfo(name, &typeAliasInfo);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *has = TRUE;
    return ec;
}

ECode HasInterface(IModuleInfo *moduleInfo, String const &fullName, Boolean *has) noexcept
{
    ECode ec;
    IInterfaceInfo* interfaceInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = moduleInfo->GetInterfaceInfo(fullName, &interfaceInfo);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }
    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *has = TRUE;
    return ec;
}

ECode HasClass(IModuleInfo *moduleInfo, String const &fullName, Boolean *has) noexcept
{
    ECode ec;
    IClassInfo* classInfo;

    if (moduleInfo == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    if (has == 0) {
		Debug_LOG("E_INVALID_ARGUMENT");
        return E_INVALID_ARGUMENT;
    }

    ec = moduleInfo->GetClassInfo(fullName, &classInfo);
    if (ec == E_DOES_NOT_EXIST)
    {
        *has = FALSE;
        return NOERROR;
    }

    if (FAILED(ec)) {
		Throw_LOG(Error::FAILED, ec);
        return ec;
    }

    *has = TRUE;
    return ec;
}
CAR_BRIDGE_NAMESPACE_END

