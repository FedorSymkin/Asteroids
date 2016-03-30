LOCAL_PATH := $(call my-dir)

APP_PLATFORM := android-9
 
include $(CLEAR_VARS)

LOCAL_LDLIBS    := -llog -lGLESv2 -landroid -ldl -lOpenSLES
LOCAL_CFLAGS    := -Werror -DANDROID_NDK -DDISABLE_IMPORTGL
LOCAL_MODULE    := asteroids
LOCAL_SRC_FILES := \
	asteroidsLib.cpp \
    graphics/androidGraphicsEngine.cpp \
	common/utils.cpp \
	common/geometry.cpp \
	game/game.cpp \
    game/actors/figure.cpp \
    game/actors/spaceship.cpp \
    game/actors/bullet.cpp \
    game/actors/actor.cpp \
    game/actors/explosion.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/graphics
LOCAL_C_INCLUDES += $(LOCAL_PATH)/game  
LOCAL_C_INCLUDES += $(LOCAL_PATH)/game/actors
LOCAL_C_INCLUDES += $(LOCAL_PATH)/common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/config
 
include $(BUILD_SHARED_LIBRARY)