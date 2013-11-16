#================================================================#
# Copyright (c) 2010-2011 Zipline Games, Inc.
# All Rights Reserved.
# http://getmoai.com
#================================================================#

	include $(CLEAR_VARS)

	LOCAL_MODULE 		:= libyaml
	LOCAL_ARM_MODE 		:= $(MY_ARM_MODE)

	LOCAL_CFLAGS	+= -DHAVE_CONFIG_H

	LOCAL_C_INCLUDES 	:= $(MY_HEADER_SEARCH_PATHS)
	LOCAL_C_INCLUDES	+= $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/include
	LOCAL_C_INCLUDES	+= $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/api.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/dumper.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/emitter.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/loader.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/parser.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/reader.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/scanner.c
	LOCAL_SRC_FILES += $(MY_MOAI_ROOT)/3rdparty/libyaml-0.1.4/src/writer.c

	include $(BUILD_STATIC_LIBRARY)
