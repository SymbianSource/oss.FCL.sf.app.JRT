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

TARGET=tckrunner
TEMPLATE=app

CONFIG += omj no_icon stl
CONFIG -= qt

INCLUDEPATH += ../../../../javaruntimes/inc

LIBS += -ljavacomms -ljavaruntimestarterutils

symbian {
    TARGET.EPOCHEAPSIZE  = 0x020000 0x800000
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.UID2          = 0x20004c46
    TARGET.UID3          = 0x2002121E
    TARGET.CAPABILITY    = all -tcb -allfiles -drm

MMP_RULES += "START RESOURCE ../data/tckrunner_reg.rss"
MMP_RULES += "TARGETPATH /private/10003a3f/apps"
MMP_RULES += "END"
}

!symbian {
    LIBS += -ljavaipc -lpthread -ljavastorage
}

include(../../../../build/omj.pri)
