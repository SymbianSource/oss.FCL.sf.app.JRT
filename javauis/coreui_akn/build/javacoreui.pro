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
CONFIG -= qt
LIBS += -lAknIcon \
        -laknlayout2 \
        -laknskins \
        -laknskinsrv \
        -lapgrfx \
        -lapparc \
        -lavkon \
        -lbafl \
        -lbitgdi \
        -lCdlEngine \
        -lcone \
        -legul \
        -lefsrv \
        -leikcoctl \
        -leikcore \
        -leiksrvc \ 
        -lfbscli \ 
        -lgdi \ 
        -lgfxtrans \
        -limageconversion \
        -ljavalegacyutils \
        -lws32 \
        -lalfclient \
        -leswt \
        -ljavaruntimestarterutils


include(../../../build/omj.pri)
