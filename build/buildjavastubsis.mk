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
# Description: Generate and deploy stub sis file, with package file 
#              made from a cpp preprocessed template.
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

# Include platform independent extension makefile macros
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

# Stub sis path definitions
ifneq (,$(findstring $(PLATFORM),WINS WINSCW wins winscw))
  TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM)/$(CFG)/z/system/install
else
  TARGETDIR=$(EPOCROOT)epoc32/data/z/system/install
endif

# Stub sis
SISFILE=$(TARGETDIR)/java.sis

# Generated package file (variated to prevent parallel build problems)
PKGFILE = java_$(PLATFORM)_$(CFG).pkg

# Commands for preprocessing and sis creation
ifeq ($(notdir $(basename $(SHELL))),sh)
  CPPCMD = gcc -E -x assembler-with-cpp -P -undef -w
else
  CPPCMD = $(EPOCROOT)epoc32/gcc/bin/cpp -lang-asm -P -undef
endif

MAKESIS = $(EPOCROOT)epoc32/tools/makesis

# Generate package file from a template and build stub SIS file
create_sis_file: 
	$(CPPCMD) ../rom/javapkg.template -o $(PKGFILE)
	$(call ifnotexistd,$(TARGETDIR))
	$(MAKESIS) -s $(PKGFILE) $(SISFILE)
	$(call remove,$(PKGFILE))

do_nothing :
	echo do_nothing

MAKMAKE : do_nothing

RESOURCE : create_sis_file

SAVESPACE : BLD

BLD : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

CLEAN :
	-$(call remove,$(SISFILE))

RELEASABLES :
	@echo $(SISFILE)
