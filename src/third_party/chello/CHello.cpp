#include "CHello.h"
#include <string.h>

#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "CHello", __VA_ARGS__)
#define DLL_PUBLIC  __attribute__ ((visibility("default")))

extern "C" DLL_PUBLIC  IHello* Create()
{
    return new CHello();
}

extern "C" DLL_PUBLIC void Destroy(IHello* p)
{
    delete p;
}

const char* CHello::hello(const char* who)
{
    char* temp = (char*)malloc(256);
    memset(temp, 0, 256);
    memcpy(temp, who, strlen(who));
    memcpy(temp+strlen(who), ": hello!", strlen(": hello!"));
    LOGD("%s", temp);
    return temp;
}