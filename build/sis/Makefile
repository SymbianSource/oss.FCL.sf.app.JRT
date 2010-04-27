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

# About signing certificates:
#	Used signing certificates can be given by specifying certificate 
#	directories with macro SIGNING_KEY_DIRS. Each .der file in the 
#	directories is used as certificate, and must be accompanied with
# 	a .key file having same basename. Directories must be specificed
#	using slashes as directory separator also in windows. Any path 
#	accessible by OS can be used, including windows share UNCs.

include $(JAVA_SRC_ROOT)/build/Makefile.defs

SISFILES = s60_omj_tck.sis s60_omj_no_prompt_policy.sis java_rnd_tools.sis
ifdef RD_JAVA_S60_RELEASE_5_0_IAD
SISFILES += s60_omj_restore_midlets.sis
endif
ifdef RD_JAVA_S60_RELEASE_10_1
SISFILES += java_3_1.sis
else
SISFILES += java_2_0.sis
endif

# Files which should be deployed for testing
DEPLOYFILES = s60_omj_tck.sis s60_omj_no_prompt_policy.sis mapfiles.zip java_rnd_tools.sis
ifdef RD_JAVA_S60_RELEASE_10_1
DEPLOYFILES += unsigned.java_3_1.sis java_3_1.sis
else
DEPLOYFILES += unsigned.java_2_0.sis java_2_0.sis
endif

all: init $(SISFILES)

# Deploying, if DEPLOYDIR is defined
ifdef DEPLOYDIR
override DEPLOYDIR := $(subst \,/,$(DEPLOYDIR))
override WINDEPLOYDIR := $(subst /,\,$(DEPLOYDIR))
deploy: $(addprefix $(DEPLOYDIR)/, $(DEPLOYFILES))
all: deploy

endif

# Find signing certificates from SIGNING_KEY_DIR (*.der, each having .key file)
# Fix path to native separator
SIGNING_CERTS=$(call PATHFIX,$(wildcard $(addsuffix /*.der,$(SIGNING_KEY_DIRS))))

$(DEPLOYDIR)/% :: % 
	if not exist $(WINDEPLOYDIR) mkdir $(WINDEPLOYDIR)
	copy $< $(subst /,\,$@)

# Recreate the SIS files always
.phony: ALWAYS
.phony: default qt_lcdui old_lcdui init all deploy clean reallyclean

init: ALWAYS
	if exist mapfiles.zip del mapfiles.zip
	python $(JAVA_SRC_ROOT)\build\sis\nouserpromptspolicy\generatePolicies.py

CPP_SIS_CMD = cpp -lang-asm -P -undef

%.sis: %.pkg ALWAYS
	$(CPP_SIS_CMD) $< $<.pkg
	makesis $<.pkg unsigned.$@
	copy unsigned.$@ $@
	echo Signing with $(SIGNING_CERTS) \
	$(foreach cert,$(SIGNING_CERTS), && signsis $@ $@ $(cert) $(basename $(cert)).key)
	python $(JAVA_SRC_ROOT)\build\buildutils\compressmapfiles.py $<.pkg
	del $<.pkg

clean:
	del mapfiles.zip 2>NUL
	del *.sis* 2>NUL
	del nouserpromptspolicy\*.ser 2>NUL

reallyclean: clean
