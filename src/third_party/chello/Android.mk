LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
                                                                                                                                                                                               
LOCAL_C_INCLUDES :=$(LOCAL_PATH)

LOCAL_SRC_FILES:= \
    CHello.cpp

LOCAL_PRELINK_MODULE := true

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils

LOCAL_C_INCLUDES += prebuilts/ndk/9/sources/cxx-stl/stlport/stlport


LOCAL_CFLAGS = -fPIC

LOCAL_PROTOC_OPTIMIZE_TYPE := lite

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib  -llog \
		   prebuilts/ndk/9/sources/cxx-stl/stlport/libs/x86/libstlport_static.a

LOCAL_MODULE:= libchello
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
