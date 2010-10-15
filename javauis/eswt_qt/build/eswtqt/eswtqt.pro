#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

include(../../../../inc/build_defines.pri)

TEMPLATE=lib
TARGET=eswtqt
CONFIG += omj java

symbian {
    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
        CONFIG += qt hb
        TRANSLATIONS = javauitoolkits.ts
    }
}


include(../../org.eclipse.swt/Eclipse_SWT_PI/qt/library/eswt_widgets.pri)
include(../../org.eclipse.swt/Eclipse_SWT_PI/qt/library/graphics/graphics.pri)
          
include(../../../../build/omj.pri)
