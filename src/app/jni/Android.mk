# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc-android/libevent $(LOCAL_PATH)/include/libevent


PATH_TO_LIB_A := ./lib/$(TARGET_ARCH_ABI)
#PATH_TO_LIB_SO := ./ 
LOCAL_MODULE    := ntvplayer
LOCAL_SRC_FILES := ntvplayer.cpp x_lock.c x_queue.c x_thread.c x_time.c
LOCAL_LDLIBS    := -llog -L$(PATH_TO_LIB_A) -llibevent
#LOCAL_CFLAGS    :=-DX_ANDROID

MY_COMMON_FLAGS += -fPIC -D__ANDROID__
LOCAL_CFLAGS   += $(MY_COMMON_FLAGS) 
LOCAL_CXXFLAGS += $(MY_COMMON_FLAGS) -std=c++11 -x c++ -fexceptions -frtti 
LOCAL_CPPFLAGS += $(MY_COMMON_FLAGS) -std=c++11 -x c++ -fexceptions -frtti




include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)
