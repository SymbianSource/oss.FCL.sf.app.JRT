/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AllTests
*
*/


#include <stdio.h>

#include "commsclientendpoint.h"

#include "Platform.h"
#include "CommandLineTestRunner.h"

using java::comms::CommsClientEndpoint;

int main(int ac, char** av)
{
    // kludge to get rid of mem leak warnings due to global context impl in comms
    {
        CommsClientEndpoint client2;
    }

    int status = CommandLineTestRunner::RunAllTests(ac, av);

    getchar();

    return status;
}

#include "alltests.h"

