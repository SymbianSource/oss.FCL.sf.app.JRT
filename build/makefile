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

COMPONENT_NAME = jrt

include $(JAVA_SRC_ROOT)/build/Makefile.defs

# Using from component
#   make -f %JAVA_SRC_ROOT%/build/Makefile.comp
#
# Overridable definitions
#   Symbian:
#     NO_DEPENDENCIES=-nd    # ignore symbian dependencies on makmake
#     REALABLD=1             # use real abld (does not stop on errors)
#
#   General:  
#     SKIPTARGETS            # list of targets not to be built e.g. 
#                            # "SKIPTARGETS=javaextensions javamanager"


# -------------------------------------------------------
# Define which kind of rules are used

# Platform APIs and stub sis files are needed only in Symbian builds
ifeq ($(BUILDENV),symbian)

  ifdef RD_JAVA_UI_QT
    # Java compilation is added as a pro-file component on Qt builds, so that 
    # Java is first in created top level bld.inf. This is required so that
    # abld executes the sub-generation for Java on makmake phase, before trying
    # to do makmake for actual components using those (stub) source files.
    COMPONENTS += build/nativejava
  else
    # Java compilation is added as bld.inf, as non-Qt builds rewrite the generated
    # bld.inf files with their own, and the build/bld.inf is automatically first.
    NONQTSUBSYSTEMS += build
  endif

  NONQTSUBSYSTEMS += \
	  jrt_plat/group
endif

SUBSYSTEMS += \
	javacommons \
	javaruntimes \
	javauis \
	javamanager \
	javatools \
	javaextensions

# Set order explicitly to get Java building as first
SUBTARGETS = $(COMPONENTS) $(NONQTSUBSYSTEMS) $(SUBSYSTEMS)

# -------------------------------------------------------

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem

# Prepare delivery for S60/SF
prep: sysdefsetup generatereleasefile configure
ifdef RD_JAVA_S60_RELEASE_9_2
prep: qmake
endif
ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
prep: pkgdefsetup
endif

SYSDEF_TEMPLATE = $(JAVA_SRC_ROOT)/build/templates/$(RD_JAVA_S60_RELEASE).layers.sysdef.xml
sysdefsetup:
	$(call CATCOMMAND,$(wildcard $(SYSDEF_TEMPLATE)))>$(JAVA_SRC_ROOT)/layers.sysdef.xml

pkgdefsetup:
	$(call CATCOMMAND,$(JAVA_SRC_ROOT)/build/templates/pkgmap_template.xml)>$(JAVA_SRC_ROOT)/package_map.xml
	$(call CATCOMMAND,$(JAVA_SRC_ROOT)/build/templates/pkgdef_template.xml)>$(JAVA_SRC_ROOT)/package_definition.xml

# Cleaning java from top level cleans also the API jars
clean_java: cleanapijars
cleanapijars:
	$(ANT) -f $(JAVA_SRC_ROOT)/build/utilities.xml -q -Dtarget.platform=$(PLATFORM) -Dtarget.cfg=$(VARIANT) clean.api.jars

sis:
ifeq ($(BUILDENV),symbian)
	$(MAKE) -C $(JAVA_SRC_ROOT)/build/sis all deploy
endif

# Generate jar for signature testings.
finalactions:signature

signature:
	$(ANT) -f $(JAVA_SRC_ROOT)/build/utilities.xml -q -Dtarget.platform=$(PLATFORM) -Dtarget.cfg=$(VARIANT) $(ANT_PROJECT_DEFINES) generate.signature.jars

javareleasables: commonjavareleasables

commonjavareleasables:
	$(ANT) -f $(JAVA_SRC_ROOT)/build/utilities.xml -q -Dtarget.platform=$(PLATFORM) -Dtarget.cfg=$(VARIANT) $(ANT_PROJECT_DEFINES) common.releasables > $(DEVNULL)
	$(call CATCOMMAND,$(JAVA_SRC_ROOT)/build/java_releasables_$(PLATFORM)_$(VARIANT).tmp)
	$(call RMFILE,$(JAVA_SRC_ROOT)/build/java_releasables_$(PLATFORM)_$(VARIANT).tmp)



ifdef RD_JAVA_S60_RELEASE_5_0
# Clean old middleware exports, as Java now uses application layer.
# This is needed when ever building on top of fresh S60 5.0 environment

configure: clean_s60_middleware
clean_s60_middleware:
	-@if exist \epoc32\include\domain\middleware\midp2permissiondb.h del /Q \epoc32\include\domain\middleware\midp2permissiondb.h
	-@if exist \epoc32\include\domain\middleware\javaattribute.h del /Q \epoc32\include\domain\middleware\javaattribute.h
	-@if exist \epoc32\include\domain\middleware\javadomainpskeys.h del /Q \epoc32\include\domain\middleware\javadomainpskeys.h
	-@if exist \epoc32\include\domain\middleware\javaenvinfo.h del /Q \epoc32\include\domain\middleware\javaenvinfo.h
	-@if exist \epoc32\include\domain\middleware\javaregistry.h del /Q \epoc32\include\domain\middleware\javaregistry.h
	-@if exist \epoc32\include\domain\middleware\javaregistryapplicationentry.h del /Q \epoc32\include\domain\middleware\javaregistryapplicationentry.h
	-@if exist \epoc32\include\domain\middleware\javaregistryentry.h del /Q \epoc32\include\domain\middleware\javaregistryentry.h
	-@if exist \epoc32\include\domain\middleware\javaregistryentrytype.h del /Q \epoc32\include\domain\middleware\javaregistryentrytype.h
	-@if exist \epoc32\include\domain\middleware\javaregistryincludes.h del /Q \epoc32\include\domain\middleware\javaregistryincludes.h
	-@if exist \epoc32\include\domain\middleware\javaregistrypackageentry.h del /Q \epoc32\include\domain\middleware\javaregistrypackageentry.h
	-@if exist \epoc32\include\domain\middleware\mjavaattribute.h del /Q \epoc32\include\domain\middleware\mjavaattribute.h
endif
