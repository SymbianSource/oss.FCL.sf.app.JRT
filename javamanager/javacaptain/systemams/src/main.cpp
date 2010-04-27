/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Start point of the OMJ trader process
*
*/


#include <e32base.h>

#include "logger.h"

#include "systemamstrader.h"


int main(int argc, char **argv[])
{
    JELOG2(EJavaSystemAMS);
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    // Pass 'iad' argument if it has been given

    TInt  err(KErrNone);
    TBool iadStart(EFalse);
    for (TInt nInd = 0; nInd < argc; nInd++)
    {
        char **tmp = argv[nInd];
        char *curArg = (char *)tmp;
        if (!strcmp(curArg, "iad"))
        {
            iadStart = ETrue;
        }
    }

    if (iadStart)
    {
        LOG(EJavaSystemAMS, EInfo, "systemams.exe started with iad param");
    }
    else
    {
        LOG(EJavaSystemAMS, EInfo, "systemams.exe started without iad param");
    }

    TRAP(err, CSystemAMSTrader::StartL(iadStart));
    if (KErrNone != err)
    {
        ELOG1(EJavaSystemAMS, "systemams.exe: StartL leaved with error %d", err);
    }
    delete cleanupStack;
}
