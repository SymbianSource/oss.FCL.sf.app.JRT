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

TARGET=javacomm
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

LIBS +=  -ljavagcf -lefsrv -lc32 -ljavautils

SOURCES += ../src/commconnectionimpljni.cpp 		 					 

symbian {
		INCLUDEPATH +=  ../inc \
		           ../inc.s60

		SOURCES += ../src.s60/dynamicpropertyhandlerjni.cpp \		
								../src.s60/javacommconnection.cpp \
		           ../src.s60/nativecommconnection.cpp 						 

}

include(../../../build/omj.pri)
