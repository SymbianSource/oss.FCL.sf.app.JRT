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

JAVABUILDDIR = $(if $(wildcard $(TO_BLDINF)/subsystem.mk),$(TO_BLDINF),$(TO_BLDINF)/..)

JAVABUILDCMD = $(MAKE) -C $(JAVABUILDDIR) -f subsystem.mk BLD_INF_JAVA=1

FREEZE :
LIB :
MAKMAKE :
	$(JAVABUILDCMD) PLATFORM=$(PLATFORM) CFG=$(CFG) makestubs
RESOURCE :
	$(JAVABUILDCMD) PLATFORM=$(PLATFORM) CFG=UDEB java finalactions
	$(JAVABUILDCMD) PLATFORM=$(PLATFORM) CFG=UREL java finalactions
BLD :
SAVESPACE :
FINAL :
CLEAN :
	$(JAVABUILDCMD) PLATFORM=$(PLATFORM) CFG=$(CFG) clean_java

RELEASABLES :
	$(JAVABUILDCMD) PLATFORM=$(PLATFORM) CFG=$(CFG) -s javareleasables

