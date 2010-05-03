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

TEMPLATE=lib
TARGET=javainstallerstarter
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -ljavacomms -ljavaruntimestarterutils
!symbian: LIBS += -ljavaipc -lpthread -ljavastorage

# S60 resources for javainstaller
MMP_RULES += "START RESOURCE ../data/javainstaller_reg.rss"
MMP_RULES += "TARGETPATH /private/10003a3f/apps"
MMP_RULES += "END //RESOURCE"
MMP_RULES += "START RESOURCE ../data/javainstaller_loc.rss"
MMP_RULES += "TARGETPATH /resource/apps"
MMP_RULES += "END //RESOURCE"

include(../../../../build/omj.pri)
