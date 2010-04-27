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
# Description: Unit test instructions
#

How to run RMS unit tests

S60
- modify installerodclist to include javarms.odc then export it
- rename javajvmargsmodifierfile.dll to javajvmargsmodifier.dll
- build unit tests with 'make -f subsystem.mk run'
- use jiut.bat in eshell to install and run unit tests

linux
- start javacaptain
- make and run unit tests with 'make -f subsystem.mk run'
