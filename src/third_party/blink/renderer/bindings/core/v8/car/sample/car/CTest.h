#ifndef __CTEST_H__
#define __CTEST_H__

#include <stddef.h>
#include <stdio.h>

#include <elastos.h>
#include <elastos/core/Object.h>
#include <elastos/coredef.h>
#include "_CTest.h"
#include "elastos/core/Object.h"
#include "TypeTraits.h"
using Elastos::Core::IComparable;
using Elastos::Core::EIID_IComparable;


CarClass(CTest)
    , public Object
    , public ITest
{
public:
    ~CTest();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(void);

    CARAPI TestSelf();

    CARAPI TestVoid();

    CARAPI TestInt16(
        /* [in] */ Int16 i16);

    CARAPI TestInt32(
        /* [in] */ Int32 i32);

    CARAPI TestInt64(
        /* [in] */ Int64 i64);

    CARAPI TestByte(
        /* [in] */ Byte byte);

    CARAPI TestFloat(
        /* [in] */ Float f);

    CARAPI TestDouble(
        /* [in] */ Double d);

    CARAPI TestChar32(
        /* [in] */ Char32 c32);

    CARAPI TestString(
        /* [in] */ const String& s);

    CARAPI TestBoolean(
        /* [in] */ Boolean b);

    CARAPI TestEMuid(
        /* [in] */ const EMuid & id);

    CARAPI TestEGuid(
        /* [in] */ const EGuid & id);

    CARAPI TestECode(
        /* [in] */ ECode ecode);

    CARAPI TestLocalPtr(
        /* [in] */ PVoid pPtr);

    CARAPI TestEnum(
        /* [in] */ ETest etest);

    CARAPI TestStringOut(
        /* [out] */ String * pName);

};


#endif // __CTEST_H__
