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

BUILD_XML_FILE = build.xml

ifeq ($(CFG),UDEB)
  export VARIANT = debug
else
ifeq ($(CFG),udeb)
  export VARIANT = debug
else
  export VARIANT = release
endif
endif

ifeq ($(PLATFORM),WINSCW)
  export _PLATF_ = winscw
else
  export _PLATF_ = armv5
endif

ANTCALL = call ant -q -buildfile $(BUILD_XML_FILE) -Dtarget.platform=$(_PLATF_) -Dtarget.cfg=$(VARIANT)

build_java : 
	$(ANTCALL)

FREEZE : 
	rem do_nothing

LIB : 
	rem do_nothing

MAKMAKE : 
	echo $(CFG)

RESOURCE : 
	rem do_nothing

BLD : build_java

SAVESPACE : 
	rem do_nothing

FINAL :
	rem do_nothing

CLEAN : 
	$(ANTCALL) clean

RELEASABLES : 
	$(ANTCALL) -q releasables
