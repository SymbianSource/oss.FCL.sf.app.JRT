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

TEMPLATE=app
TARGET=jvmargmodifier_tester
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -ljavajvmargsmodifier -ljavafileutils
LIBS += -lCppUTest

INCLUDEPATH += ../../../../../tools/cpputest/include/CppUTest

symbian {
INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Symbian
}
!symbian {
INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Gcc
LIBPATH +=     ../../../../../tools/cpputest/lib
}

include(../../../../../build/omj.pri)
