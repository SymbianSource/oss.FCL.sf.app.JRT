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
# Description: autostarter is plugin for Java Captain that starts auto start
#   MIDlets
#

TEMPLATE=lib
TARGET=javacaptain_ext_autostarter
CONFIG += omj stl
CONFIG -= qt


LIBS += -ljavacomms -ljavastorage -lapparc -lapgrfx

include(../../../../../build/omj.pri)
