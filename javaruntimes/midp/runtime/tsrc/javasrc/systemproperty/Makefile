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

include $(JAVA_SRC_ROOT)/build/Makefile.defs

COMPONENTS = build

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem

TEST_JAD=$(JAVA_SRC_ROOT)/javaruntimes/midp/midpruntime/tsrc/javasrc/systemproperty/build/runtimesystemproptests.jad
TEST_JAR=$(JAVA_SRC_ROOT)/javaruntimes/midp/midpruntime/tsrc/javasrc/systemproperty/build/runtimesystemproptests.jar

ifeq ($(OS),Windows_NT)
  UPDATE_JAD_CMD=if exist $(TEST_JAD) perl -i.bak -e "$$s=-s'$(TEST_JAR)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/$$s/;}print;}" $(TEST_JAD)
else
  UPDATE_JAD_CMD=if test -e $(TEST_JAD); then perl -i.bak -e "\$$s=-s'$(TEST_JAR)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/\$$s/;}print;}" $(TEST_JAD); fi
endif

run: default_target
	$(UPDATE_JAD_CMD)
ifneq ($(OS),Windows_NT)
	$(JAVA_BIN_ROOT)/bin/javainstaller install -jad=$(TEST_JAD)
endif
