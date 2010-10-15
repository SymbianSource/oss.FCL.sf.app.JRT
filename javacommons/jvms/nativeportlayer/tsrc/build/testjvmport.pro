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
         TARGET=testjvmport
                CONFIG += omj no_icon stl
                          CONFIG -= qt

                                    LIBS += -lCppUTest -ljvmnativeport -lefsrv

                                            INCLUDEPATH += ../../../../../tools/cpputest/include/CppUTest

                                                    INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Symbian
                                                            INCLUDEPATH += ../../inc.s60
                                                                    TARGET.CAPABILITY = all -tcb

                                                                            include(../../../../../build/omj.pri)
