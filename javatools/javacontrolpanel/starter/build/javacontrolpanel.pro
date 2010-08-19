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

TEMPLATE=app
TARGET=javacontrolpanel
CONFIG += omj no_icon stl
CONFIG -= qt

INCLUDEPATH += ../../../../javaruntimes/inc
LIBS += -ljavacomms -ljavaruntimestarterutils

symbian {
    TARGET.EPOCHEAPSIZE  = 0x020000 0x800000
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.UID2 = 0x20004c45
    TARGET.UID3 = 0x2002FF61
    TARGET.SID = 0x2002FF61

    TARGET.CAPABILITY = AllFiles WriteDeviceData PowerMgmt NetworkControl

    MMP_RULES += "START RESOURCE ../data/javacontrolpanel_reg.rss"
    MMP_RULES += "TARGETPATH /private/10003a3f/apps"
    MMP_RULES += "END"
}

include(../../../../build/omj.pri)
