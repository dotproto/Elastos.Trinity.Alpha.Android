#ifndef __CAR_BRIDGE_MACROS_H
#define __CAR_BRIDGE_MACROS_H

#include <android/log.h>

#define CAR_BRIDGE_NAMESPACE_BEGIN \
namespace Elastos { \
namespace CARBridge {
#define CAR_BRIDGE_NAMESPACE_END \
} \
}

#define CAR_BRIDGE ::Elastos::CARBridge

#define CAR_BRIDGE_NAMESPACE_USING \
using namespace ::Elastos::CARBridge;

void Carbridge_throw(const char *func, const char *file, int line, char const * info, int const ecode);
void Carbridge_log(const char* file, int line, const char* format, ...);

#define Throw_LOG(ERRNO, ECODE) Carbridge_throw(__FUNCTION__, __FILE__, __LINE__, Error::GetIntrinsicErrorName(ERRNO), ECODE)
#define Debug_LOG(FORMAT, ...) \
    __android_log_print(ANDROID_LOG_INFO, "chromium", \
                        "[%s at %s:%d] " FORMAT, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)


#endif
