
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <elastos.h>
#include <elastos/core/Object.h>
#include <elastos/coredef.h>

#include "CTest.h"

#include <android/log.h>
	
#define basename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
#define Debug_LOG(FORMAT, ...) \
		__android_log_print(ANDROID_LOG_INFO, "chromium", \
			"[%s:%d %s] " FORMAT, basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)


CTest::~CTest()
{
    Debug_LOG("debug");
}

CAR_OBJECT_IMPL(CTest)

CAR_INTERFACE_IMPL(CTest, Object, ITest);

ECode CTest::constructor(void)
{
    Debug_LOG("debug");
    return NOERROR;
}

ECode CTest::TestSelf(void)
{
    Debug_LOG("Interface (ITest *)");
    return NOERROR;
}

ECode CTest::TestVoid(void)
{
    Debug_LOG("void (void)");
    return NOERROR;
}

ECode CTest::TestInt16(
        /* [in] */ Int16 i16)
{
    Debug_LOG("%d", i16);
    return NOERROR;
}

ECode CTest::TestInt32(
        /* [in] */ Int32 i32)
{
    Debug_LOG("%d", i32);
    return NOERROR;
}

ECode CTest::TestInt64(
        /* [in] */ Int64 i64)
{
    Debug_LOG("%lld", i64);
    return NOERROR;
}

ECode CTest::TestByte(
        /* [in] */ Byte byte)
{
	Debug_LOG("%u", byte);
    return NOERROR;
}

ECode CTest::TestFloat(
        /* [in] */ Float f)
{
	Debug_LOG("%f", f);
    return NOERROR;
}

ECode CTest::TestDouble(
        /* [in] */ Double d)
{
	Debug_LOG("%lf", d);
    return NOERROR;
}

ECode CTest::TestChar32(
        /* [in] */ Char32 c32)
{
	Debug_LOG("\'%lc\'", c32);
    return NOERROR;
}

ECode CTest::TestString(
        /* [in] */ String const &s)
{
    Debug_LOG("%s", s.string());
    return NOERROR;
}

ECode CTest::TestBoolean(
        /* [in] */ Boolean b)
{
    if (b != FALSE) {
	    Debug_LOG("true");

    } else {
	    Debug_LOG("false");
    }

    return NOERROR;
}

ECode CTest::TestEMuid(
        /* [in] */ EMuid const &id)
{
    Debug_LOG("{ mData1 : %u,", id.mData1);
    Debug_LOG("  mData2 : %u,", id.mData2);
    Debug_LOG("  mData3 : %u,", id.mData3);
    Debug_LOG("  mData4 : [ %u, %u, %u, %u, %u, %u, %u, %u ]", 
        id.mData4[0], id.mData4[1], id.mData4[2], id.mData4[3], id.mData4[4], id.mData4[5], id.mData4[6], id.mData4[7]);
    Debug_LOG(" }");

    return NOERROR;
}

ECode CTest::TestEGuid(
        /* [in] */ EGuid const &id)
{
    Debug_LOG("\"mClsid\": ");
	TestEMuid(id.mClsid);
    Debug_LOG("\"mUunm\": \"%s\"",id.mUunm);
    Debug_LOG("\"mCarcode\": %u",  id.mCarcode);


    return NOERROR;
}

ECode CTest::TestECode(
        /* [in] */ ECode ecode)
{
    Debug_LOG("%d", ecode);
    return NOERROR;
}

ECode CTest::TestLocalPtr(
        /* [in] */ PVoid ptr)
{
    Debug_LOG("%p", ptr);
    return NOERROR;
}

ECode CTest::TestEnum(
        /* [in] */ ETest etest)
{
    switch (etest) {
    case ETest_0:
        Debug_LOG("\"ETest_0\"");
        break;

    case ETest_1:
        Debug_LOG("\"ETest_1\"");
        break;

    default:
        Debug_LOG("\"Unknow\"");
    }

    return NOERROR;
}

ECode CTest::TestStringOut(
    /* [out] */ String * pName)
{
    VALIDATE_NOT_NULL(pName)
    *pName = "Elastos Hello world.";
    Debug_LOG("%s", pName->string());
    return NOERROR;
}

#if 0
ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Int16> const &i16s)
{
    size_t size = i16s.GetLength();
	
    for (size_t i = 0; i < size - 1; ++i) {
		Debug_LOG("%d", i);
		TestInt16(array[i]);
    }

    return NOERROR;
}
#endif
