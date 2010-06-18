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
* Description:  Helper application for uninstalling a java application and
*    then installing new application (java or native)
*
*/

#include <apgcli.h>
#include <apacmdln.h>
#include <apmstd.h>
#include <bacline.h>
#include <e32cmn.h>
#include <s32mem.h>
#include <unistd.h>

#include "exceptionbase.h"
#include "javaoslayer.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"


using namespace java::util;


_LIT(KHexValueStart, "0x");
_LIT(KSemiColon, ";");
_LIT(KUidArg, "uid=");
_LIT(KFileArg, "file=");

const TInt KExtraLenForLogging = 2;
const TInt KArgumentValueMaxLen = 1568;
// Wait for 0.5 sec if ArcApp has not yet initialized
const TInt KDelayWhenWaitingAppArc = 500000;


/**
 * Set the value of the argument specified by aArgName to aArgValue
 *
 * @param aCmdLine command line to be parsed
 * @param aArgName the name of the argument
 * @param aArgValue the value parsed from command line will be returned here
 */
static void getArgValueL(const TPtrC &aCmdLine, const TDesC &aArgName, HBufC **aArgValue)
{
    TBuf<KArgumentValueMaxLen> valueBuf;
    TInt argPos = aCmdLine.FindF(aArgName);
    if (argPos >= 0)
    {
        TInt semicolonPos = aCmdLine.Mid(argPos).Find(KSemiColon);
        if (KErrNotFound == semicolonPos)
        {
            semicolonPos = aCmdLine.Mid(argPos).Length();
        }
        TInt argLen = semicolonPos - aArgName.Length();
        if (argLen >= KArgumentValueMaxLen)
        {
            // Protect from buffer overflow.
            WLOG2(EUtils,
                  "javaupgradeapp: argument value len too long (%d), cutting it to %d",
                  argLen, (KArgumentValueMaxLen - 1));
            argLen = KArgumentValueMaxLen - 1;
        }
        else if (argLen == 0)
        {
            User::Leave(KErrArgument);
        }

        valueBuf = aCmdLine.Mid(argPos + aArgName.Length(),  argLen);
    }

    // Allocate new HBufC and return it
    HBufC *pBufValue = HBufC::NewL(valueBuf.Length() + 2);
    *pBufValue = valueBuf;
    *aArgValue = pBufValue;
}


/**
 * Parse the name from the value of 'file' parameter in
 * command line given in aCmdLine
 *
 * @param aCmdLine  command line to be parsed, the format is
 *  <other_args>;file=YYY;<other_args>
 * @param aFileName will contain the name parsed from command line
 */
static void getNameFromCommandLineL(const TPtrC &aCmdLine, HBufC **aFileName)
{
    TInt err = aCmdLine.FindF(KFileArg);
    User::LeaveIfError(err);

    getArgValueL(aCmdLine, KFileArg, aFileName);
}


/**
 * Parse the Uid from the value of 'uid' parameter in
 * command line given in aCmdLine
 *
 * @param aCmdLine  command line to be parsed, the format is
 *  uid=YYY;<other_args>
 * @param aUid will contain the Uid parsed from command line
 */
static void getUidFromCommandLineL(const TPtrC &aCmdLine, TInt32 &aUid)
{
    TInt err(KErrNone);
    TInt argPos = aCmdLine.FindF(KUidArg);
    if (KErrNotFound != argPos)
    {
        TPtrC uidToParse = aCmdLine.Mid(argPos + KUidArg.iTypeLength);
        TLex parseUid(uidToParse);
        if (uidToParse.FindF(KHexValueStart) == 0)
        {
            parseUid.Inc(2); // skip hex prefix
            TUint32 tmpValue;
            err = parseUid.Val(tmpValue, EHex);
            aUid = tmpValue;
        }
        else
        {
            err = parseUid.Val(aUid);
        }

        if (KErrNone != err)
        {
            ELOG1(EUtils,
                  "javaupgradeapp failed parsing app Uid from cmdline uid param. Error %d",
                  err);
        }
    }
    else
    {
        ELOG(EUtils,
             "javaupgradeapp cannot uninstall app because uid parameter is not given");
    }

    User::LeaveIfError(err);
}


/**
 * Parse the process command line.
 * Determine the uid of the Java application to be uninstalled and
 * the name of the application package to be installed.
 * Leaves if argument 'file' exist but file name cannot be found,
 * leaves if argument 'uid' exist but uid cannot be parsed from command line,
 * does NOT leave if only one of the arguments is present
 *
 * @param aFileName returns value of argument 'file'
 * @param aUid returns Uid of the Java application to be uninstalled
 */
void getFileAndUidL(HBufC **aFileName, TInt32 *aUid)
{
    HBufC *pBufCmdLine =
        HBufC::NewLC(User::CommandLineLength() + KExtraLenForLogging);
    TPtr cmdLineBuf = pBufCmdLine->Des();
    User::CommandLine(cmdLineBuf);

    if (cmdLineBuf.Length() > 0)
    {
        LOG1WSTR(EUtils, EInfo,
                 "javaupgradeapp: full java application cmd line is : %s",
                 (wchar_t *)(cmdLineBuf.PtrZ()));

        // Get the midlet uid from the commandline
        TRAPD(err, getUidFromCommandLineL(cmdLineBuf, *aUid));
        // It is enough that either midlet uid OR installation package name
        // have been given in commandline
        if ((err != KErrNone) && (err != KErrNotFound))
        {
            User::Leave(err);
        }

        // Get the name of the installation package from the commandline
        TRAPD(err2, getNameFromCommandLineL(cmdLineBuf, aFileName))
        if (err2 != KErrNone)
        {
            if (err2 == KErrNotFound)
            {
                if (err == KErrNotFound)
                {
                    // Both arguments missing
                    User::Leave(KErrArgument);
                }
            }
            else
            {
                User::Leave(err2);
            }
        }
    }
    else
    {
        ELOG(EUtils, "javaupgradeapp: empty command line");
        User::Leave(KErrArgument);
    }

    CleanupStack::PopAndDestroy(pBufCmdLine);
}


/**
 * Uninstall the java application specified by aUid parameter.
 *
 * @param aUid the Uid of the java application to be uninstalled
 */
void uninstallJavaAppL(TInt32 aUid)
{
    RProcess rJavaInstaller;
    TFileName fileName;
    // Max one path name and some options -> 1536 is enough
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    TInt len = strlen(java::runtime::JAVA_INSTALLER_STARTER_DLL);
    TPtr8 ptr8InstallerDll((TUint8 *)java::runtime::JAVA_INSTALLER_STARTER_DLL, len, len);
    commandLine.Copy(ptr8InstallerDll);

    // Use command line options that make sure that uninstallation is done
    // always, silently and so that the uninstalled java application will
    // be preinstalled again if the user uninstalls it
    commandLine.Append(_L(" uninstall -uid="));
    commandLine.AppendNum(aUid);
    commandLine.Append(_L(" -forceuninstall -silent -preinstall_always"));

    LOG1WSTR(EUtils, EInfo,
        "javaupgradeapp:uninstallJavaAppL Java Installer command line is %s",
        (wchar_t *)(commandLine.PtrZ()));

    // start JavaInstaller
    TBuf<64> installerProcess;  // Actual len of the process name is 9
    len = strlen(java::runtime::JAVA_PROCESS);
    TPtr8 ptr8Process((TUint8 *)java::runtime::JAVA_PROCESS, len, len);
    installerProcess.Copy(ptr8Process);

    TRequestStatus status;
    TInt err = rJavaInstaller.Create(installerProcess, commandLine);
    if (KErrNone == err)
    {
        LOG(EUtils, EInfo, "javaupgradeapp:uninstallJavaAppL calling Logon");
        // Get notification when Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EUtils, EInfo, "javaupgradeapp:uninstallJavaAppL calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EUtils,
              "javaupgradeapp:uninstallJavaAppL Cannot start Java Installer, error %d",
              err);
        User::Leave(err);
    }

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    LOG(EUtils, EInfo, "javaupgradeapp:uninstallJavaAppL calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();
}


/**
 *
 *
 *
 */
void installAppPackageL(HBufC *aBufFileName)
{
    // Open file using default handler,
    // if the file is an installation package, it will be installed
    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG(EUtils,
             "javaupgradeapp:installAppPackageL: Cannot connect to AppArc server");
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    TInt      retryCounter(10);
    TThreadId handlerTreadId;
    do
    {
        err = apaSession.StartDocument(*aBufFileName, handlerTreadId);
        if (RApaLsSession::EAppListInvalid == err)
        {
            // Application list has not yet been populated,
            // try again after a short delay
            retryCounter--;
            if (retryCounter > 0)
            {
                User::After(KDelayWhenWaitingAppArc);
                continue;
            }
            else
            {
                ELOG(EUtils,
                     "javaupgradeapp:installAppPackageL: RApaLsSession "
                     "StartDocument returned EAppListInvalid for 10 times, exiting");
                User::Leave(err);
            }
        }
        else if (KErrNone != err)
        {
            ELOG1(EUtils,
                "javaupgradeapp:installAppPackageL: RApaLsSession "
                     "StartDocument returned error %d", err);
            User::Leave(err);
        }

    } while (RApaLsSession::EAppListInvalid == err);

    CleanupStack::PopAndDestroy(); // apaSession
}


/**
 * Get the uid of the Java application to be uninstaller and
 * start Java Installer to uninstall the application.
 * Then install the new application package given in cmdline
 *
 */
void handleUpgradeL(void)
{
    HBufC *pBufFileName = NULL;
    TInt32 uid(0);

    getFileAndUidL(&pBufFileName, &uid);

    if (uid != 0)
    {
        LOG1(
            EUtils,
            EInfo,
            "javaupgradeapp uninstalling app uid %d", uid);
        uninstallJavaAppL(uid);
    }

    if (pBufFileName != NULL)
    {
        LOG1WSTR(EUtils, EInfo,
            "javaupgradeapp: installing new app package %s",
            (wchar_t *)(pBufFileName->Des().PtrZ()));
        installAppPackageL(pBufFileName);

        delete pBufFileName;
    }
}


/**
 * Main function of executable javaupgradeapp.exe.
 * "installer app" created by Services team starts this executable when
 * the "installer app" must be replaced with the real application (java/native or wrt)
 *
 * The command line format is
 * uid=<Uid>;file=<full_path_to_installation_file>
 * for example
 * uid=0x10137c4d;file=D:\\temp\\upgrade\\package.sis
 *
 * Sample code for starting this application from the "installer app" MIDlet
 * @code

    MIDlet.platformRequest(
        “nativeapp://application-exe=javaupgradeapp.exe;application-args=uid=0x10137c4d;file=D:\\temp\\upgrade\\package.sis”);

 * @endcode
 *
 */
TInt E32Main(void)
{
    // TODO: check that only MIDP runtime process can start this in final version
    // that goes to ROM


    CTrapCleanup *pCleanupStack = CTrapCleanup::New();
    if (NULL == pCleanupStack)
    {
        ELOG(EUtils, "Cannot create CleanupStack in javaupgradeapp.exe main()");
        return KErrNoMemory;
    }

    TRAPD(err, handleUpgradeL());
    if (KErrNone != err)
    {
        ELOG1(EUtils, "javaupgradeapp.exe: handleUpgradeL leaved with err %d", err);
    }

    delete pCleanupStack;
    return err;
}

