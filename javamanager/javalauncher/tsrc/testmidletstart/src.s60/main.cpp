/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: prototyping starting midlet from native applications
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

_LIT(KJavaLauncherExe, "javalauncher.exe");

/**
 *
 */
static void startByJavaLauncherSimpleL()
{
    LOG(EJavaConverters, EInfo,
        "testmidletstart: startByJavaLauncherSimple: Going to start midlet from 'testmidletstart.txt'");

    // Read the content of test data file
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart: startByJavaLauncherSimple: Cannot connect to RFs, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(fs);
    RFile testData;
    err = testData.Open(fs, _L("E:\\testmidletstart.txt"), EFileRead);
    if (KErrNotFound == err)
    {
        err = testData.Open(fs, _L("C:\\testmidletstart.txt"), EFileRead);
    }
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart: startByJavaLauncherSimple: Cannot open E: "
              "or C:\\testmidletstart.txt, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(testData);
    TInt testDataSize = 0;
    err = testData.Size(testDataSize);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart: startByJavaLauncherSimple: Cannot get size of "
              "testmidletstart.txt, err %d", err);
        User::Leave(err);
    }

    HBufC8 *pBuf = HBufC8::NewLC(testDataSize + 8);
    TPtr8 buf = pBuf->Des();
    err = testData.Read(buf);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart: startByJavaLauncherSimple: Reading "
              "testmidletstart.txt failed, err %d", err);
        User::Leave(err);
    }

    printf("File content was %s", (const char *)(buf.PtrZ()));


    TInt pos = 0;
    // Find LF
    TInt end = buf.Locate(0x0A);
    if (KErrNotFound == end)
    {
        end = buf.Length();
    }
    else if (buf.Locate(0x0D) == (end - 1))
    {
        // If there is CR before LF, it does not belong to cmd line
        --end;
    }

    // Read one line from buffer, convert it to 16 bit
    TPtrC8 oneLine = buf.Mid(pos, end - pos);

    TDesC *pCmdLine = NULL;
    TInt res = java::util::S60CommonUtils::ConvertNarrowToWiderL(oneLine, pCmdLine);

    // Create one process using this command line
    RProcess rProcess;
    // start
//    TInt err = rProcess.Create(KJavaLauncherExe, _L("midlet-uid=0x1014e720;startMode=startFromCmdLine;sound=ON;landscapeMode=true;"));  // OK
//    TInt err = rProcess.Create(KJavaLauncherExe, _L("midlet-name=%E6%98%9F%E5%A4%A2%E7%BE%8E%E5%B0%91%E5%A5%B3%E7%A0%8C%E5%9C%96"));
//    TInt err = rProcess.Create(KJavaLauncherExe, _L("midlet-name=%C3%80%C3%80NEST%C3%80.EXE"));
//    err = rProcess.Create(KJavaLauncherExe, _L("midlet-name=SPtesti;mode=nativestart;sounds=ON;difficulty=medium;wizard_mode"));

    err = rProcess.Create(KJavaLauncherExe, *pCmdLine);
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherSimple calling Rendezvous");
        // This call will wait until midlet exits (or panics)
        TRequestStatus status;
        rProcess.Logon(status);

        LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherSimple calling Resume");
        rProcess.Resume();

        // now wait until javalauncher exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            ELOG1(EJavaConverters,
                  "testmidletstart: startByJavaLauncherSimple javalauncher exited with error %d",
                  status.Int());
        }
    }
    else
    {
        ELOG1(EJavaConverters,
              "testmidletstart: startByJavaLauncherSimple Cannot start midlet, error %d", err);
    }

    LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherSimple calling RProcess::Close");

    delete pCmdLine;
    rProcess.Close();

    // free resources before returning
    CleanupStack::PopAndDestroy(3);
}


/**
 *
 */
static void startByAppArcL()
{
    RApaLsSession apaSession;

    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::startByAppArcL RApaLsSession Connect error %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

    // Drive letter must be correct
    cmdLine->SetExecutableNameL(_L("C:\\269712461.fakeapp"));
    cmdLine->SetCommandL(EApaCommandRun);
    cmdLine->SetTailEndL(_L8(" midletparam1 -param2 -param3=no"));

    LOG(EJavaConverters, EInfo, "testmidletstart: startByAppArcL: Going to start midlet SystemProperties");

    err = apaSession.StartApp(*cmdLine);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::startByAppArcL StartApp error %d", err);
    }

    CleanupStack::PopAndDestroy(cmdLine);
    CleanupStack::PopAndDestroy(); // apaSession
}


/**
 * Start javalauncher.exe that will start midlet
 */
static void startByJavaLauncherL()
{
    LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherL: Going to start midlet SystemProperties");

    RProcess rProcess;
    // Build command line used to pass all necessary info
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

    cmdLine->SetExecutableNameL(KJavaLauncherExe);
    cmdLine->SetCommandL(EApaCommandRun);

    cmdLine->SetDocumentNameL(_L("C:/data/my_own_doc.kwi"));

    // The midlet to be started can be specified by using opaque data like here or
    // by specifying midlet name and vendor in command line
    TBuf8<4>         opaqueData;     // opaque data will contain one signed 32-bit int
    RDesWriteStream  writeStream(opaqueData);
    writeStream.WriteInt32L(0x10137c4d);   // midlet application Uid
    writeStream.CommitL();
    cmdLine->SetOpaqueDataL(opaqueData);

    LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherL: Create process");

    // start
    TInt err = rProcess.Create(KJavaLauncherExe, _L("midlet-uid=0x10137c4d;startMode=strangeStart"));
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherL: Set process commandline");

        cmdLine->SetProcessEnvironmentL(rProcess);

        LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherL calling Logon");
        // This call will wait until javalauncher exits (or panics)
        TRequestStatus status;
        rProcess.Logon(status);

        LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherL calling Resume");
        rProcess.Resume();

        // now wait until javalauncher exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            ELOG1(EJavaConverters,
                  "testmidletstart: startByJavaLauncherL javalauncher exited with error %d",
                  status.Int());
        }
    }
    else
    {
        ELOG1(EJavaConverters,
              "testmidletstart: startByJavaLauncherL Cannot start midlet, error %d", err);
    }

    LOG(EJavaConverters, EInfo, "testmidletstart: startByJavaLauncherL calling RProcess::Close");
    // free resources before returning
    CleanupStack::PopAndDestroy(cmdLine);
    rProcess.Close();
}


/**
 * Read all midlets from AppArc.
 * Log midlet Uid, Midlet name, installation drive,
 * group name and icon file path name
 */
static void readMidletsFromAppArcL()
{
    TInt   retryCounter = 10;
    TUid   appTypeUid;
    TApaAppInfo   info;
    RApaLsSession apaSession;

    TBuf8<512> midletDesc;


    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::readMidletsFromAppArcL RApaLsSession Connect error %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    // Get the process of getting the list of all non native applications
    // (mostly java applications)
    err =
        apaSession.GetFilteredApps(
            TApaAppCapability::ENonNative, TApaAppCapability::ENonNative);

    TFullName logBuf;

    do
    {
        err = apaSession.GetNextApp(info);
        if (RApaLsSession::EAppListInvalid == err)
        {
            // Application list has not yet been populated,
            // try again after a short delay
            retryCounter--;
            if (retryCounter > 0)
            {
                User::After(500000);
                continue;
            }
            else
            {
                ELOG(EJavaConverters,
                     "testmidletstart::readMidletsFromAppArcL RApaLsSession "
                     "GetNext returned EAppListInvalid for 10 times, error");
                User::Leave(err);
            }
        }
        else if (KErrNone == err)
        {
            // Info contains valid app info. Now check whether it is java app
            err = apaSession.GetAppType(appTypeUid, info.iUid);
            if (KErrNone != err)
            {
                ELOG1(EJavaConverters,
                      "testmidletstart::readMidletsFromAppArcL RApaLsSession "
                      "GetAppType returned error %d", err);
                User::Leave(err);
            }
            if (appTypeUid.iUid == KMidletApplicationTypeUid)
            {
                // This is java application. Log info
                LOG1(EJavaConverters, EInfo, "testmidletstart: readMidletsFromAppArcL: midlet uid %x", info.iUid);

                logBuf = info.iCaption;
                LOG1WSTR(EJavaConverters, EInfo, "testmidletstart: readMidletsFromAppArcL: midlet name %s",
                         (wchar_t *)(logBuf.PtrZ()));

                logBuf = info.iFullName;
                LOG1WSTR(EJavaConverters, EInfo, "testmidletstart: readMidletsFromAppArcL: midlet path %s",
                         (wchar_t *)(logBuf.PtrZ()));

            }
        }

    }
    while (KErrNone == err);

    if (RApaLsSession::ENoMoreAppsInList != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart:readMidletsFromAppArcL: RApaLsSession GetNext returned error %d", err);
        User::Leave(err);
    }

    CleanupStack::PopAndDestroy(); // apaSession
}


/**
 *
 */
static void registerAsMimeHandlerL()
{
    TInt   retryCounter = 10;
    TUid   appTypeUid;
    TApaAppInfo   info;
    RApaLsSession apaSession;


    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::registerAsMimeHandlerL RApaLsSession Connect error %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    // Get current handler for file e:\tif.tif
    TUid      handlerUid;
    TDataType mimeType;
    err = apaSession.AppForDocument(_L("e:\\tif.tif"), handlerUid, mimeType);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::registerAsMimeHandlerL AppForDocument error %d", err);
        User::Leave(err);
    }

    TBuf8<256> mimeTypeName;
    mimeTypeName = mimeType.Des8();
    mimeTypeName.Append('\0');
    LOG1(EJavaConverters, EInfo,
         "testmidletstart::registerAsMimeHandlerL file MIME type is %s", &(mimeTypeName[0]));
    LOG1(EJavaConverters, EInfo,
         "testmidletstart::registerAsMimeHandlerL handler for it is %x", handlerUid.iUid);

    TUid midletUid;
    midletUid.iUid = 0x10137c4d;
    TDataTypePriority priority(100);
    err = apaSession.InsertDataMapping(mimeType, priority, midletUid);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::registerAsMimeHandlerL InsertDataMapping error %d", err);
        User::Leave(err);
    }

    TUid  newHandlerUid;
    err = apaSession.AppForDocument(_L("e:\\tif.tif"), newHandlerUid, mimeType);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::registerAsMimeHandlerL AppForDocument2 error %d", err);
        User::Leave(err);
    }
    LOG1(EJavaConverters, EInfo,
         "testmidletstart::registerAsMimeHandlerL NOW handler is %x", newHandlerUid.iUid);



    // Restore old handler
    TDataTypePriority oldHandlerPriority(300);
    TUid originalHandlerUid;
    originalHandlerUid.iUid = 0x1000599d;
    err = apaSession.InsertDataMapping(mimeType, oldHandlerPriority, originalHandlerUid);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testmidletstart::registerAsMimeHandlerL InsertDataMapping (restore old handler) error %d", err);
        User::Leave(err);
    }

    /*
        // Restore old handler
        TDataTypePriority oldHandlerPriority(300);
        TUid originalHandlerUid;
        originalHandlerUid.iUid = 0x101f4d90;
        err = apaSession.InsertDataMapping(mimeType, oldHandlerPriority, originalHandlerUid);
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters,
                "testmidletstart::registerAsMimeHandlerL InsertDataMapping (restore old handler) error %d", err);
            User::Leave(err);
        }
    */

    CleanupStack::PopAndDestroy(); // apaSession
}


/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
//    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TRAPD(err, readMidletsFromAppArcL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testmidletstart: readMidletsFromAppArcL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);

    TRAP(err, startByJavaLauncherSimpleL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testmidletstart: startByJavaLauncherSimple leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);

    /*
        TRAP(err, startByAppArcL());
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters, "testmidletstart: startByAppArcL leaved with err %d", err);
        }

        // Wait for a moment
        User::After(500000);

        TRAP(err, startByJavaLauncherL());
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters, "testmidletstart: startByJavaLauncherL leaved with err %d", err);
        }


        // Wait for a moment
        User::After(500000);

        TRAP(err, registerAsMimeHandlerL());
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters, "testmidletstart: registerAsMimeHandlerL leaved with err %d", err);
        }
    */

    delete cleanupStack;
//    __UHEAP_MARKEND;
    return KErrNone;
}
// eof
