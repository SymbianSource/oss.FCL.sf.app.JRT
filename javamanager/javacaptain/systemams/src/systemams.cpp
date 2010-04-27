/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Start point of the VM process
*
*/


#include <unistd.h>

#include "logger.h"

using namespace java::util;

_LIT(KExecutableName, "javacaptain.exe");

int main(int /*argc*/, char **/*argv[]*/)
{
    RProcess proc;
    int st = proc.Create(KExecutableName, KExecutableName);
    if (st == KErrNone)
    {
        proc.Resume();
        proc.Close();
    }

}
