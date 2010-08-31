#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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
TARGET=eswtuitestutils
CONFIG += omj java


symbian {
    
	TARGET.UID3 = 0x2002E699

  include(../src/eswtuitestutils.pri)
 
	LIBS += -llibc
	LIBS += -leuser
	LIBS += -llibstdcpp
	LIBS += -ljavautils
	LIBS += -leikcore
}

include(../../../../build/omj.pri)