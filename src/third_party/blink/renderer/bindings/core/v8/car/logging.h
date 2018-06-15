#ifndef LOGGING_H_
#define LOGGING_H_

#include <android/log.h>

#define basename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
#define Debug_LOG(FORMAT, ...) \
    __android_log_print(ANDROID_LOG_INFO, "chromium", \
                        "[%s:%d %s] " FORMAT, basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif // LOGGING_H_