#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

NONQTSUBSYSTEMS = \
    build \
    mainapp/build \
    rmsbenchmark/build 

COMPONENTS = \
    rmsplugin/build

include $(JAVA_SRC_ROOT)/build/Makefile.subsystem

TEST_ROOT=$(JAVA_SRC_ROOT)/javaextensions/midprms_db/tsrc/build

TEST_JAD=$(TEST_ROOT)/rmstests.jad
TEST_JAR=$(TEST_ROOT)/rmstests.jar

TEST_JAD2=$(TEST_ROOT)/rmssharedtests.jad
TEST_JAR2=$(TEST_ROOT)/rmssharedtests.jar

TEST_JAD3=$(TEST_ROOT)/rmsintersuite.jad
TEST_JAR3=$(TEST_ROOT)/rmsintersuite.jar

ifeq ($(OS),Windows_NT)
  UPDATE_JAD_CMD=if exist $(TEST_JAD) perl -i.bak -e "$$s=-s'$(TEST_JAR)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/$$s/;}print;}" $(TEST_JAD)
  UPDATE_JAD_CMD2=if exist $(TEST_JAD2) perl -i.bak -e "$$s=-s'$(TEST_JAR2)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/$$s/;}print;}" $(TEST_JAD2)
  UPDATE_JAD_CMD3=if exist $(TEST_JAD3) perl -i.bak -e "$$s=-s'$(TEST_JAR3)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/$$s/;}print;}" $(TEST_JAD3)
else
  UPDATE_JAD_CMD=if test -e $(TEST_JAD); then perl -i.bak -e "\$$s=-s'$(TEST_JAR)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/\$$s/;}print;}" $(TEST_JAD); fi
  UPDATE_JAD_CMD2=if test -e $(TEST_JAD2); then perl -i.bak -e "\$$s=-s'$(TEST_JAR2)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/\$$s/;}print;}" $(TEST_JAD2); fi
  UPDATE_JAD_CMD3=if test -e $(TEST_JAD3); then perl -i.bak -e "\$$s=-s'$(TEST_JAR3)';while(<>){if(/^MIDlet-Jar-Size:/){s/\d+/\$$s/;}print;}" $(TEST_JAD3); fi
endif

run: default_target
	$(UPDATE_JAD_CMD)
	$(UPDATE_JAD_CMD2)
	$(UPDATE_JAD_CMD3)
ifeq ($(OS),Windows_NT)
	echo Copying JAD/JAR files to emulator install dir..
	copy $(JAVA_SRC_ROOT)\javaextensions\midprms_db\tsrc\build\*.jar \epoc32\winscw\c\data\Installs
	copy $(JAVA_SRC_ROOT)\javaextensions\midprms_db\tsrc\build\*.jad \epoc32\winscw\c\data\Installs
	copy $(JAVA_SRC_ROOT)\javaextensions\midprms_db\tsrc\rmsbenchmark\build\*.jar \epoc32\winscw\c\data\Installs
else
	$(JAVA_BIN_ROOT)/bin/javarms_tester -v
	sleep 3
	$(JAVA_BIN_ROOT)/bin/javainstaller install -jad=$(TEST_JAD) -silent
	$(JAVA_BIN_ROOT)/bin/javainstaller install -jad=$(TEST_JAD2) -silent
	$(JAVA_BIN_ROOT)/bin/javainstaller install -jad=$(TEST_JAD3) -silent
	$(JAVA_BIN_ROOT)/bin/javainstaller test -mainclass=com.nokia.mj.test.rms.RmsMain	
endif
