#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TARGET = AllTests


SRCS = ../src/AllTests.cpp ../src/testserverconnection.cpp \
	../src/servertestutils.cpp ../src/testconcurrentconnections.cpp \
	../src/testrobustness.cpp

OBJS = $(SRCS:.cpp=.o)

CC = g++

TARGET_DIR = .

LIBS =\
 -lstdc++\
 -ljavastorage\
 -ljavalogger\
 -ljavacommonutils\
 -ljavaoslayer\
 -ljavacomms\
 -lCppUTest

CPP_U_TEST = $(JAVA_SRC_ROOT)/tools/cpputest

LIBDIR = $(CPP_U_TEST)/lib

LIBPATH =\
 -L$(LIBDIR) -L$(JAVA_BIN_ROOT)/lib -L$(JAVA_BIN_ROOT)/jsr/lib

INCLUDE =\
 -I$(CPP_U_TEST)/include/CppUTest\
 -I$(CPP_U_TEST)/include/Platforms/Gcc\
 -I$(JAVA_SRC_ROOT)/javautils/javastorage/inc -I$(JAVA_SRC_ROOT)/inc -I. \
 -I$(JAVA_SRC_ROOT)/javautils/javastorage/tsrc/storageserver/src \
-I$(JAVA_SRC_ROOT)/javautils/javastorage/src

include $(CPP_U_TEST)/build/TestExeMakefile

%.o:	%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJS)
	$(CXX) -o $(TARGET_DIR)/$@ $^ $(LIBPATH) $(LIBS)

clean:
	-$(RM) -f ../src/*~ ../src/*.o $(TARGET_DIR)/$(TARGET)


