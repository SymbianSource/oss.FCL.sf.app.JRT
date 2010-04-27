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

JAVA_ARTIFACTS = $(JAVA_BIN_ROOT)/javabuild/$(JAVA_COMPONENT)
JAVAH_ARTIFACTS = $(JAVA_ARTIFACTS)/inc.javah

RUN_ANT = ant -q

ifndef SBOX_CPUTRANSPARENCY_LOG
DO_COMPILE_JAVA = true
endif


