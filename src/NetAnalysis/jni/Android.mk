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

#获取源文件所在目录
LOCAL_PATH := $(call my-dir)
#为了防止环境变量之间的相互干扰，调用clear-var.mk
include $(CLEAR_VARS)
#指定生成的库文件命令
LOCAL_MODULE    := NamedPipe
	
LOCAL_SRC_FILES := NamedPipe.cpp

#编译生成静态库.a
#include $(BUILD_STATIC_LIBRARY)
#编译生成动态库.so
include $(BUILD_SHARED_LIBRARY)
#编译生成可执行文件
#include $(BUILD_EXECUTABLE)



