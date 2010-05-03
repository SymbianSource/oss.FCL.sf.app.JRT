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
* Description:  ?Description
*
*/


#include "Platform.h"
#include "CommandLineTestRunner.h"
#include "commsclientendpoint.h"

int main(int ac, char** av)
{
    {
        // creates comms static data so that tests do no report false memory leaks
        java::comms::CommsClientEndpoint client;
    }
    return CommandLineTestRunner::RunAllTests(ac, av);
}

#include "alltests.h"

