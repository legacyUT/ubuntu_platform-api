LOCAL_PATH := $(call my-dir)

ANDROID_VERSION_MAJOR := $(word 1, $(subst ., , $(PLATFORM_VERSION)))
ANDROID_VERSION_MINOR := $(word 2, $(subst ., , $(PLATFORM_VERSION)))
ANDROID_VERSION_PATCH := $(word 3, $(subst ., , $(PLATFORM_VERSION)))

HAS_LIBINPUTSERVICE := $(shell test $(ANDROID_VERSION_MAJOR) -eq 4 -a $(ANDROID_VERSION_MINOR) -gt 2 && echo true)
IS_ANDROID_5 := $(shell test $(ANDROID_VERSION_MAJOR) -eq 5 && echo true)

include $(CLEAR_VARS)

LOCAL_CFLAGS += \
	-DANDROID_VERSION_MAJOR=$(ANDROID_VERSION_MAJOR) \
	-DANDROID_VERSION_MINOR=$(ANDROID_VERSION_MINOR) \
	-DANDROID_VERSION_PATCH=$(ANDROID_VERSION_PATCH)

UPAPI_PATH := $(LOCAL_PATH)/../../

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

ifeq ($(IS_ANDROID_5),true)
LOCAL_C_INCLUDES += bionic/libc/kernel/uapi
endif

LOCAL_SRC_FILES := \
	ubuntu_application_api_for_hybris.cpp \
	ubuntu_application_gps_for_hybris.cpp \
	ubuntu_application_sensors_for_hybris.cpp \
	../default/default_ubuntu_application_sensor.cpp \
	../default/default_ubuntu_ui.cpp \
	application_manager.cpp

LOCAL_MODULE := libubuntu_application_api
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libbinder \
	libinput \
	liblog \
	libutils \
	libgui \
	libEGL \
	libGLESv2 \
	libhardware \
	libhardware_legacy

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test.cpp \

LOCAL_MODULE:= direct_ubuntu_application_api_for_hybris_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_c_api.cpp \

LOCAL_MODULE:= direct_ubuntu_application_c_api_for_hybris_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_session_c_api.cpp \

LOCAL_MODULE:= direct_ubuntu_application_session_c_api_for_hybris_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_trap.cpp \

LOCAL_MODULE:= direct_set_trap
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_osk_visibility.cpp \

LOCAL_MODULE:= direct_osk_visibility
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_sensors_c_api.cpp \

LOCAL_MODULE:= direct_ubuntu_application_sensors_c_api_for_hybris_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_sensors.cpp \

LOCAL_MODULE:= direct_ubuntu_application_sensors_for_hybris_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += \
	-DANDROID_VERSION_MAJOR=$(ANDROID_VERSION_MAJOR) \
	-DANDROID_VERSION_MINOR=$(ANDROID_VERSION_MINOR) \
	-DANDROID_VERSION_PATCH=$(ANDROID_VERSION_PATCH)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	external/skia/include/core \
	frameworks/base/services \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

ifeq ($(IS_ANDROID_5),true)
LOCAL_C_INCLUDES += \
	frameworks/native/services \
	frameworks/base/libs
endif


LOCAL_SRC_FILES:= \
	application_manager.cpp \
	default_application_manager.cpp \

LOCAL_MODULE:= ubuntuappmanager
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libbinder \
	libinput \
	liblog \
	libgui \
	libskia \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

ifeq ($(HAS_LIBINPUTSERVICE),true)
LOCAL_SHARED_LIBRARIES += libinputservice
endif

ifeq ($(IS_ANDROID_5),true)
LOCAL_SHARED_LIBRARIES += \
	libui \
	libinputflinger \
	libinputservice

endif

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	application_manager.cpp \
	default_application_manager_test.cpp \

LOCAL_MODULE:= ubuntuappmanager_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libbinder \
	libandroidfw \
	libutils \
	libEGL \
	libGLESv2 \
	libubuntu_application_api

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=gnu++0x

LOCAL_C_INCLUDES := \
	$(UPAPI_PATH)/include \
	$(UPAPI_PATH)/android/include

LOCAL_SRC_FILES:= \
	test_gps_api.cpp \

LOCAL_MODULE:= direct_ubuntu_application_gps_c_api_for_hybris_test
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libui \
	libutils \
	libEGL \
	libGLESv2 \
	libhardware \
	libhardware_legacy \
	libubuntu_application_api

#include $(BUILD_EXECUTABLE)
