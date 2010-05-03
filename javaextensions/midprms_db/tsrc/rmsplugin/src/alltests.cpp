/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: RMS plugin unit test runner
*
*/


#include "Platform.h"
#include "CommandLineTestRunner.h"

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}

