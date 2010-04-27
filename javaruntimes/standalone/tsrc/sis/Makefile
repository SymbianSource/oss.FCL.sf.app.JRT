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

# About signing certificates:
#	Used signing certificates can be given by specifying certificate 
#	directories with macro SIGNING_KEY_DIRS. Each .der file in the 
#	directories is used as certificate, and must be accompanied with
# 	a .key file having same basename. Directories must be specificed
#	using slashes as directory separator also in windows. Any path 
#	accessible by OS can be used, including windows share UNCs.

include $(JAVA_SRC_ROOT)/build/Makefile.defs

SISFILES = javastandalone.sis

all: $(SISFILES)

# Find signing certificates from SIGNING_KEY_DIR (*.der, each having .key file)
# Fix path to native separator
SIGNING_CERTS=$(call PATHFIX,$(wildcard $(addsuffix /*.der,$(SIGNING_KEY_DIRS))))

# Recreate the SIS files always
.phony: ALWAYS
.phony: clean reallyclean

%.sis: %.pkg ALWAYS
	makesis $< $@
	echo Signing with $(SIGNING_CERTS) \
	$(foreach cert,$(SIGNING_CERTS), && signsis $@ $@ $(cert) $(basename $(cert)).key)

clean:
	del *.sis* 2>NUL

reallyclean: clean
