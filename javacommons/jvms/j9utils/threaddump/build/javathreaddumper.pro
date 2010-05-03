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

include(../../../../../inc/build_defines.pri)

symbian {
  TEMPLATE=lib
  TARGET=javathreaddumper
  CONFIG += omj staticdata stl
  CONFIG -= qt

  INCLUDEPATH +=  ../../../../../../javaruntimes/inc

  LIBS += -ljavafileutils

}
!symbian {
  # Include as Java-only, just for exporting
  CONFIG += omj javaonly
}

include(../../../../../build/omj.pri)
