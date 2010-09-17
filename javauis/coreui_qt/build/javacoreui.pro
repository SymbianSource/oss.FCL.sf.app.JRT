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
# Description: The build file of the Core UI
#

TEMPLATE=lib
TARGET=javacoreui
CONFIG += omj staticdata java stl

symbian {
    CONFIG += hb
    LIBS += -ltstaskmonitorclient
}

!symbian {
    CONFIG -= qt
}

include(../../../build/omj.pri)
