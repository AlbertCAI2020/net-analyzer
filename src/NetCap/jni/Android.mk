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
LOCAL_MODULE    := pcap
LIBPCAP_SRC_FILES :=\
	pcap-linux.c\
	pcap-netfilter-linux.c\
	fad-gifc.c\
	pcap.c\
	inet.c\
	gencode.c\
	optimize.c\
	nametoaddr.c\
	etherent.c\
	savefile.c\
	sf-pcap.c\
	sf-pcap-ng.c\
	pcap-common.c\
	bpf_image.c\
	bpf_dump.c\
	scanner.c\
	bpf_filter.c\
	version.c\
	grammar.c
	
LOCAL_SRC_FILES := $(addprefix libpcap/, $(LIBPCAP_SRC_FILES))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/libpcap
LOCAL_CFLAGS += -DHAVE_CONFIG_H -DNDEBUG -D_U_="__attribute__((unused))" -fomit-frame-pointer -Wall -pipe
#编译生成静态库.a
include $(BUILD_STATIC_LIBRARY)
#编译生成动态库.so
#include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := protobuf

LOCAL_CPP_EXTENSION := .cc
LOCAL_CPPFLAGS += -frtti

LOCAL_SRC_FILES :=  \
	google/protobuf/io/coded_stream.cc \
	google/protobuf/stubs/common.cc \
	google/protobuf/extension_set.cc \
	google/protobuf/generated_message_util.cc \
	google/protobuf/message_lite.cc \
	google/protobuf/stubs/once.cc \
	google/protobuf/repeated_field.cc \
	google/protobuf/stubs/stringprintf.cc \
	google/protobuf/wire_format_lite.cc \
	google/protobuf/io/zero_copy_stream.cc \
	google/protobuf/io/zero_copy_stream_impl_lite.cc
   
LOCAL_C_INCLUDES := $(LOCAL_PATH)/google/protobuf
         
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := netcap
LOCAL_CPP_EXTENSION := .cc .cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/libpcap
LOCAL_SRC_FILES := \
	main.cpp \
	unittests.cpp \
	NetCapServer.cpp \
	message.pb.cc \
	Log.cpp \
	PacketCapture.cpp \
	PacketAnalyzer.cpp \
	PacketHeader.cpp \
	IpProtoHandler.cpp \
	TcpProtoHandler.cpp \
	TcpSessionHandler.cpp \
	TcpStatData.cpp \
	DnsPktInfo.cpp \
	UdpProtoHandler.cpp \
	DnsProtoHandler.cpp
	
LOCAL_CPPFLAGS += -D_COMPILE_UNITTEST
LOCAL_CPPFLAGS += -pie -fPIE -frtti -fexceptions
LOCAL_LDFLAGS += -pie -fPIE
LOCAL_STATIC_LIBRARIES := pcap protobuf
#LOCAL_LDLIBS    := -llog -ldl -pthread

include $(BUILD_EXECUTABLE)



