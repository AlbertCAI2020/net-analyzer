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

#��ȡԴ�ļ�����Ŀ¼
LOCAL_PATH := $(call my-dir)
#Ϊ�˷�ֹ��������֮����໥���ţ�����clear-var.mk
include $(CLEAR_VARS)
#ָ�����ɵĿ��ļ�����
LOCAL_MODULE    := NamedPipe
	
LOCAL_SRC_FILES := NamedPipe.cpp

#�������ɾ�̬��.a
#include $(BUILD_STATIC_LIBRARY)
#�������ɶ�̬��.so
include $(BUILD_SHARED_LIBRARY)
#�������ɿ�ִ���ļ�
#include $(BUILD_EXECUTABLE)



