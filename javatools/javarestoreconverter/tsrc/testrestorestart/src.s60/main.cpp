/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: start javarestoreconverter for testing
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <apgcli.h> // for RApaLsSession
#include <e32property.h>
#include <swinstallerinternalpskeys.h>
#include <apacmdln.h> //CApaCommandLine
#include <s32mem.h>   // RDesWriteStream
#include <stdio.h>
#include <s60commonutils.h>

#include "javauids.h"
#include "logger.h"

_LIT(KJavaRestoreConverterExe, "javarestoreconverter.exe");

/**
 *
 */
static void simpleStartL()
{
    LOG(EJavaConverters, EInfo,
        "testrestorestart: startByJavaLauncherSimple: Going to start javarestoreconverter.exe");


    // Create one process using this command line
    RProcess rProcess;
    // start
    TInt err = rProcess.Create(KJavaRestoreConverterExe, _L("-driveC"));
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "testrestorestart: simpleStartL calling Rendezvous");
        // This call will wait until process exits (or panics)
        TRequestStatus status;
        rProcess.Logon(status);

        LOG(EJavaConverters, EInfo, "testrestorestart: simpleStartL calling Resume");
        rProcess.Resume();

        // now wait until javalauncher exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            ELOG1(EJavaConverters,
                  "testrestorestart: simpleStartL javarestoreconverter exited with error %d",
                  status.Int());
        }
    }
    else
    {
        ELOG1(EJavaConverters,
              "testrestorestart: simpleStartL Cannot start javaestoreconverter, error %d", err);
    }

    LOG(EJavaConverters, EInfo, "testrestorestart: simpleStartL calling RProcess::Close");

    rProcess.Close();

}

/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();



    TRAPD(err, simpleStartL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testrestorestart: simpleStartL leaved with err %d", err);
    }

    delete cleanupStack;
    __UHEAP_MARKEND;
    return KErrNone;
}
// eof
