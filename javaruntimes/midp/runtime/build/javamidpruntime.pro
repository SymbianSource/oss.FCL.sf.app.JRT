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

TARGET=javamidpruntime
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

LIBS += -ljavacomms -ljavasecurity
        

symbian {
    INCLUDEPATH += /epoc32/include/ecom     # Due to schemehandler.inl in S60 5.0 2009w18 and SPB 10.1
    LIBS += -lecom -lflogger -lDrmRightsInfo -lcaf -lDRMHelper -lapgrfx -lapparc -ljavautils
}

include(../../../../build/omj.pri)
