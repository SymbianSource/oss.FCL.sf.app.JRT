#/*******************************************************************************
# * Copyright (c) 2007 Nokia Corporation.
# * All rights reserved. This program and the accompanying materials 
# * are made available under the terms of the Eclipse Public License v1.0
# * which accompanies this distribution, and is available at
# * http://www.eclipse.org/legal/epl-v10.html
# * 
# * Contributors:
# *     Nokia Corporation - S60 implementation
# *******************************************************************************/


# Local vars
EPOC32_BIN_PATH = $(EPOCROOT)epoc32\release
ESWT_BIN_PATH = ..\..\org.eclipse.ercp.swt.s60\native\bin
ESWT_APIFACADE_BIN_FILES = eswtapifacade.*


# Rules
MAKMAKE : DO_NOTHING
BLD : DO_NOTHING
LIB : DO_NOTHING
CLEANLIB : DO_NOTHING
RESOURCE : DO_NOTHING
FREEZE : DO_NOTHING
SAVESPACE : DO_NOTHING
RELEASABLES : DO_NOTHING
CLEAN : DO_NOTHING
CHECK : DO_NOTHING
FINAL : eswtcvsexportsbin

eswtcvsexportsbin :
	@echo eswt_cvs_exports.mk: Export eSwtApiFacade binaries to eSWT CVS folders
	if not exist $(ESWT_BIN_PATH)\$(PLATFORM)\$(CFG) mkdir $(ESWT_BIN_PATH)\$(PLATFORM)\$(CFG)
	xcopy /R /Y $(EPOC32_BIN_PATH)\$(PLATFORM)\$(CFG)\$(ESWT_APIFACADE_BIN_FILES) $(ESWT_BIN_PATH)\$(PLATFORM)\$(CFG)
	if exist $(EPOC32_BIN_PATH)\$(PLATFORM)\lib\$(ESWT_APIFACADE_BIN_FILES) xcopy /R /Y $(EPOC32_BIN_PATH)\$(PLATFORM)\lib\$(ESWT_APIFACADE_BIN_FILES) $(ESWT_BIN_PATH)\$(PLATFORM)\lib

DO_NOTHING :
	rem Do nothing
