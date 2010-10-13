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


ifdef RD_JAVA_APPLICATION_SETTINGS_QT
COMPONENTS += appsettingsview_qt/build
SYMBIAN_ONLY += appsettingsview_qt/build
else
ifndef RD_JAVA_S60_RELEASE_5_0_IAD
NONQTSUBSYSTEMS += appmngrplugin/build
SYMBIAN_ONLY += appmngrplugin/build
endif
endif

# Declare that release preparation removes subdirectories not in build, except
# listed special cases
REMOVE_NOTBUILT_EXCLUDING = dummy

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
