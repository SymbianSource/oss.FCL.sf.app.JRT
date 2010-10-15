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
* Description:  This file contains the implementation of CJavaSifPlugin
*               class member functions. ECOM Plugin for USIF to handle
*               install, uninstall, get component info requests.
*
*/


#include <apgcli.h>
#include <apmstd.h>
#include <charconv.h>
#include <data_caging_path_literals.hrh>
#include <e32cmn.h>
#include <e32property.h>
#include <string.h>
#include <usif/usiferror.h>

#include "comms.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "javacommonutils.h"
#include "javaoslayer.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"
#include "resultsserver.h"
#include "runtimeexception.h"

#include "javasifplugin.h"


using namespace java::installer;
using namespace java::util;
using namespace java::comms;

_LIT(KPrivateDataCage, "\\private\\");
_LIT(KInboxDataCage, "\\private\\1000484b\\");
_LIT(KJavaInstallerProcess, "Installer*");
_LIT(KJavaInstallCopierProcess, "javainstallcopier.exe");
_LIT(KJavaInstallerDataCage, "\\private\\102033e6\\");
_LIT(KJavaInstallerTmp, "\\private\\102033E6\\installer\\tmp\\");
_LIT(KAnyExtension, ".*");
_LIT(KComponentInfo, " componentinfo");
_LIT(KCommsResult, " -commsresult=");
_LIT(KJad, " -jad=");
_LIT(KJar, " -jar=");
_LIT(KDoubleQuote, "\"");
_LIT(KInstall, " install");
_LIT(KSilent, " -silent");
_LIT(KUninstall, " uninstall");
_LIT(KCid, " -cid=");
_LIT(KDrive, " -drive=");
_LIT(KYes, "yes");
_LIT(KNo, "no");
_LIT(KOcsp, " -ocsp=");
_LIT(KIgnoreOcspWarnings, " -ignore_ocsp_warnings=");
_LIT(KUpgrade, " -upgrade=");
_LIT(KUpgradeData, " -upgrade_data=");
_LIT(KUntrusted, " -untrusted=");
_LIT(KOverwrite, " -overwrite=");
_LIT(KDownload, " -download=");
_LIT(KIap, " -iap=");
_LIT(KSnap, " -snap=");
_LIT(KForceCancel, " -forcecancel");
_LIT(KBase64, " -base64=");
_LIT(KUsername, " -username=");
_LIT(KPassword, " -password=");
_LIT(KSourceUrl, " -sourceurl=");
_LIT(KCharSet, " -charset=");

_LIT(KJadMimeType, "text/vnd.sun.j2me.app-descriptor");
_LIT8(K8JadMimeType, "text/vnd.sun.j2me.app-descriptor");

// Long Java Installer command lines contain max 2 path names (512 chars max) and
// some options -> 1536 is long enough buffer
const TInt KLongCmdLineLen = 1536;

// When starting Java Installer for uninstallation the command line is much shorter
const TInt KShortCmdLineLen = 256;

// Java Installer is executed with same Uid as Java Runtime
_LIT_SECURE_ID(KJavaInstallerSecureID, KJavaMidpSecureId);

// ============================ MEMBER FUNCTIONS ===============================

CJavaSifPlugin* CJavaSifPlugin::NewL()
{
    CJavaSifPlugin* self = new(ELeave) CJavaSifPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CJavaSifPlugin::~CJavaSifPlugin()
{
    mRFs.Close();

    TInt nHandles = mHandlesToClose.Count();
    while (nHandles > 0)
    {
        nHandles--;
        mHandlesToClose[nHandles].Close();
    }
    mHandlesToClose.Reset();

    delete mResultsServer;
    mResultsServer = NULL;

    delete mDummyResults;
    mDummyResults = NULL;

    delete mDummyInfo;
    mDummyInfo = NULL;
}

CJavaSifPlugin::CJavaSifPlugin()
{
}

void CJavaSifPlugin::ConstructL()
{
    User::LeaveIfError(mRFs.Connect());
    User::LeaveIfError(mRFs.ShareProtected());
    mResultsServer = NULL;
    mDummyResults = COpaqueNamedParams::NewL(); // codescanner::forgottoputptroncleanupstack
    mDummyInfo = CComponentInfo::NewL();
}

void CJavaSifPlugin::GetComponentInfo(
    const TDesC& aFileName,
    const TSecurityContext& aSecurityContext,
    CComponentInfo& aComponentInfo,
    TRequestStatus& aStatus)
{
    RFile fileHandle;
    TInt err = fileHandle.Open(mRFs, aFileName, EFileShareReadersOnly | EFileRead);
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::GetComponentInfo Opening file for reading failed with error %d",
              err);
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    GetComponentInfo(fileHandle, aSecurityContext, aComponentInfo, aStatus);
    fileHandle.Close();
}

void CJavaSifPlugin::GetComponentInfo(
    RFile& aFileHandle,
    const TSecurityContext& /* aSecurityContext */,
    CComponentInfo& aComponentInfo,
    TRequestStatus& aStatus)
{
    if (ExitIfJavaInstallerRunning(*mDummyResults, aStatus))
    {
        return;
    }

    RProcess rJavaInstaller;
    TFileName fileName;
    HBufC* pBufCommandLine = NULL;
    TRAPD(err, pBufCommandLine = HBufC::NewL(KLongCmdLineLen))
    if (KErrNone != err)
    {
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    std::auto_ptr<HBufC> bufCommandLine(pBufCommandLine);
    TPtr commandLine = pBufCommandLine->Des();

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(KComponentInfo);

    // Ask Java Installer to send component info back
    // as Comms message. 11000 is IPC_ADDRESS_JAVA_SIF_PLUGIN_C Comms endpoint
    // that our ResultsServer will listen to.
    commandLine.Append(KCommsResult);
    commandLine.AppendNum(IPC_ADDRESS_JAVA_SIF_PLUGIN_C);

    err = aFileHandle.FullName(fileName);

    // Java Installer does not have AllFiles capability.
    // So if the .jad/.jar file is in the private data cage of
    // Remove Application Management system, it must be copied
    // to the private data cage of Java Installer in this
    // plugin which is executed in process which hopefully has
    // AllFiles capability.
    TRAP(err, CopyFilesIfNeededL(fileName));
    if (KErrNone != err)
    {
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    // Check whether the file is .jad or .jar
    TBool isJad;
    err = IsJadFile(aFileHandle, isJad);
    if (KErrNone != err)
    {
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    if (isJad)
    {
        // Installation should be started from JAD file
        commandLine.Append(KJad);
    }
    else
    {
        // from JAR file
        commandLine.Append(KJar);
    }

    // Filename parameter must be surrounded in double quotes to
    // ensure that spaces in filename are passed correctly.
    commandLine.Append(KDoubleQuote);
    commandLine.Append(fileName);
    commandLine.Append(KDoubleQuote);

    // Start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        err = StartResultsServer(*mDummyResults, aComponentInfo);
        if (KErrNone != err)
        {
            // server cannot be started
            rJavaInstaller.Close();
            ELOG1(EJavaInstaller,
                  "CJavaSifPlugin::GetComponentInfo: Cannot start results server, err %d", err);
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, err);
            return;
        }

        // The status of the component info operation == the exit status of Java Installer
        // It will be passed to the asynch caller through aStatus
        rJavaInstaller.Logon(aStatus);
    }
    else
    {
        rJavaInstaller.Close();
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::GetComponentInfo: starting JavaInstaller failed, err=%d", err);
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    rJavaInstaller.Resume();

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    err = mHandlesToClose.Append(rJavaInstaller);
    if ( err )
    {
        rJavaInstaller.Close();
        ELOG1(EJavaInstaller, "CJavaSifPlugin::GetComponentInfo  mHandles"
              "ToClose.Append failed with error %d", err);
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
}

void CJavaSifPlugin::Install(
    const TDesC& aFileName,
    const TSecurityContext& aSecurityContext,
    const COpaqueNamedParams& aArguments,
    COpaqueNamedParams& aResults,
    TRequestStatus& aStatus)
{
    RFile fileHandle;
    TInt err = fileHandle.Open(mRFs, aFileName, EFileShareReadersOnly | EFileRead);
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::Install Opening file for reading failed with error %d", err);
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    Install(fileHandle, aSecurityContext, aArguments, aResults, aStatus);
    fileHandle.Close();
}

void CJavaSifPlugin::Install(
    RFile& aFileHandle,
    const TSecurityContext& aSecurityContext,
    const COpaqueNamedParams& aArguments,
    COpaqueNamedParams& aResults,
    TRequestStatus& aStatus)
{
    if (ExitIfJavaInstallerRunning(aResults, aStatus))
    {
        return;
    }

    // Create buffer for Java Installer command line
    HBufC* pBufCommandLine = NULL;
    TRAPD(err, pBufCommandLine = HBufC::NewL(KLongCmdLineLen))
    if (KErrNone != err)
    {
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, ELowMemory));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    std::auto_ptr<HBufC> bufCommandLine(pBufCommandLine);
    TPtr commandLine = pBufCommandLine->Des();

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(KInstall);

    // Check whether this is silent installation
    TInt silentInstall = 0;
    TRAP_IGNORE(aArguments.GetIntByNameL(KSifInParam_InstallSilently, silentInstall));
    if (silentInstall)
    {
        // Silent installation requires TrustedUI capability
        if (!aSecurityContext.HasCapability(ECapabilityTrustedUI))
        {
            ELOG(EJavaInstaller,
                 "CJavaSifPlugin::Install The caller did not have TrustedUI capability");
            TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, ESecurityError));
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, KErrPermissionDenied);
            return;
        }

        commandLine.Append(KSilent);
    }

    BuildInstallCommandLine(commandLine, aArguments);

    // Ask Java Installer to send installation results back
    // as Comms message. 11000 is IPC_ADDRESS_JAVA_SIF_PLUGIN_C Comms endpoint
    // that our ResultsServer will listen to.
    commandLine.Append(KCommsResult);
    commandLine.AppendNum(IPC_ADDRESS_JAVA_SIF_PLUGIN_C);

    TFileName fileName;
    err = aFileHandle.FullName(fileName);

    // Java Installer does not have AllFiles capability.
    // So if the .jad/.jar file is in the private data cage of
    // Remove Application Management system, it must be copied
    // to the private data cage of Java Installer in this
    // plugin which is executed in process which hopefully has
    // AllFiles capability.
    TRAP(err, CopyFilesIfNeededL(fileName));
    if (KErrNone != err)
    {
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    // Check whether the file is .jad or .jar
    TBool isJad = EFalse;
    err = IsJadFile(aFileHandle, aArguments, isJad);
    if (KErrNone != err)
    {
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    if (isJad)
    {
        // Installation should be started from JAD file
        commandLine.Append(KJad);
    }
    else
    {
        // from JAR file
        commandLine.Append(KJar);
    }
    // Filename parameter must be surrounded in double quotes to
    // ensure that spaces in filename are passed correctly.
    commandLine.Append(KDoubleQuote);
    commandLine.Append(fileName);
    commandLine.Append(KDoubleQuote);

    // Start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    RProcess rJavaInstaller;
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        err = StartResultsServer(aResults, *mDummyInfo);
        if (KErrNone != err)
        {
            // server cannot be started
            rJavaInstaller.Close();
            ELOG1(EJavaInstaller,
                  "CJavaSifPlugin::Install: Cannot start results server, err %d", err);
            TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, err);
            return;
        }

        // The status of the install operation == the exit status of Java Installer
        // It will be passed to the asynch caller through aStatus
        rJavaInstaller.Logon(aStatus);
    }
    else
    {
        rJavaInstaller.Close();
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::Install: starting JavaInstaller failed, err=%d", err);
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    rJavaInstaller.Resume();

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    err = mHandlesToClose.Append(rJavaInstaller);
    if ( err )
    {
        rJavaInstaller.Close();
        ELOG1(EJavaInstaller, "CJavaSifPlugin::Install  mHandles"
              "ToClose.Append failed with error %d", err);
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, ELowMemory));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

}

void CJavaSifPlugin::Uninstall(
    TComponentId aComponentId,
    const TSecurityContext& aSecurityContext,
    const COpaqueNamedParams& aArguments,
    COpaqueNamedParams& aResults,
    TRequestStatus& aStatus)
{
    if (ExitIfJavaInstallerRunning(aResults, aStatus))
    {
        return;
    }

    // Allocate buffer for Java Installer command line
    HBufC* pBufCommandLine = NULL;
    TRAPD(err, pBufCommandLine = HBufC::NewL(KShortCmdLineLen))
    if (KErrNone != err)
    {
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, ELowMemory));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    std::auto_ptr<HBufC> bufCommandLine(pBufCommandLine);
    TPtr commandLine = pBufCommandLine->Des();

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(KUninstall);

    commandLine.Append(KCid);
    commandLine.AppendNum(aComponentId);

    // Check whether this is silent uninstallation
    TInt silentInstall = 0;
    TRAP_IGNORE(aArguments.GetIntByNameL(KSifInParam_InstallSilently, silentInstall));
    if (silentInstall)
    {
        // Silent uninstallation requires TrustedUI capability
        if (!aSecurityContext.HasCapability(ECapabilityTrustedUI))
        {
            ELOG(EJavaInstaller,
                 "CJavaSifPlugin::Uninstall The caller did not have TrustedUI capability");
            TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, ESecurityError));
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, KErrPermissionDenied);
            return;
        }

        commandLine.Append(KSilent);
    }

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    RProcess rJavaInstaller;
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        err = StartResultsServer(aResults, *mDummyInfo);
        if (KErrNone != err)
        {
            // server cannot be started
            rJavaInstaller.Close();
            ELOG1(EJavaInstaller,
                  "CJavaSifPlugin::Uninstall: Cannot start results server, err %d", err);
            TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, err);
            return;
        }

        // the exit status of Java Installer will be passed to
        // the asynch caller through aStatus
        rJavaInstaller.Logon(aStatus);

        rJavaInstaller.Resume();
    }
    else
    {
        rJavaInstaller.Close();
        ELOG1(EJavaInstaller,
             "CJavaSifPlugin::Uninstall: starting JavaInstaller failed, err=%d", err);
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    err = mHandlesToClose.Append(rJavaInstaller);
    if ( err )
    {
        rJavaInstaller.Close();
        ELOG1(EJavaInstaller, "CJavaSifPlugin::Uninstall  mHandles"
              "ToClose.Append failed with error %d", err);
        TRAP_IGNORE(aResults.AddIntL(KSifOutParam_ErrCategory, ELowMemory));
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
}

void CJavaSifPlugin::Activate(
    TComponentId /* aComponentId */,
    const TSecurityContext& /* aSecurityContext */,
    TRequestStatus& aStatus)
{
    // java applications are always active
    TRequestStatus *statusPtr(&aStatus);
    User::RequestComplete(statusPtr, KErrNone);
    return;
}

void CJavaSifPlugin::Deactivate(
    TComponentId /* aComponentId */,
    const TSecurityContext& /* aSecurityContext */,
    TRequestStatus& aStatus)
{
    // java applications are always active
    TRequestStatus *statusPtr(&aStatus);
    User::RequestComplete(statusPtr, KErrNotSupported);
    return;
}

void CJavaSifPlugin::CancelOperation()
{
    // Send cancel message to Java Installer

    // Check whether there is anything to cancel
    if (mHandlesToClose.Count() < 1)
    {
        // No Java Installer process running, do nothing
        WLOG(EJavaInstaller,
            "CJavaSifPlugin::CancelOperation: Cancel called but there is no operation to cancel");
        return;
    }

    CommsMessage message;
    message.setReceiver(IPC_ADDRESS_JAVA_INSTALLER_SERVER_C);
    message.setSender(IPC_ADDRESS_JAVA_SIF_PLUGIN_C);
    message.setMessageId(INSTALLER_CANCEL_MESSAGE_ID);

    CommsMessage replyMessage;
    int timeout = 10; // 10 seconds  // codescanner::magicnumbers

    CommsClientEndpoint comms;
    int err = comms.connect(IPC_ADDRESS_JAVA_INSTALLER_SERVER_C);
    if (KErrNone == err)
    {
        err = comms.sendReceive(message, replyMessage, timeout);
    }
    else
    {
        // Cannot connect to Java Installer Comms end point,
        // for example Java Installer is still starting up or
        // already exiting
        WLOG1(EJavaInstaller,
            "CJavaSifPlugin:CancelOperation: Cannot connect to Java Installer "
            "Comms end point, err %d", err);
        return;
    }
    if (err != 0)
    {
        // Sending message to Java Installer failed.
        ELOG1(EJavaInstaller,
            "CJavaSifPlugin:CancelOperation: Cannot send message to Java Installer, err %d",
            err);
        // Ignore possible errors in disconnect
        (void)comms.disconnect();  // codescanner::voidparameter
        return;
    }

    // Ignore the cancel result returned in replyMessage
    // because current SIF API does not return cancel result

    // Ignore possible errors in disconnect
    (void)comms.disconnect();  // codescanner::voidparameter

    // It takes some time before Java Installer had really cancelled
    // the operation and exited. Wait for it to happen because this function
    // must return only after the original asynchronous call is completed.
    TRequestStatus status;
    // This array contains process handles for all Java Installer processes
    // started from this sif plugin. The last handle is the active one.
    // Check if it is still running
    if (mHandlesToClose[mHandlesToClose.Count()-1].ExitReason() != 0)
    {
        // Process has already closed
        return;
    }
    // Wait until it exits
    mHandlesToClose[mHandlesToClose.Count()-1].Logon(status);
    User::WaitForRequest(status);  // codescanner::userWaitForRequest
    // Ignore the exit status of Java Installer because current SIF API
    // does not return cancel result

    return;
}

void CJavaSifPlugin::CopyFilesIfNeededL(TFileName &aFileName)
{
    // Check if the file is in the private data cage of some process
    TInt idx = aFileName.FindF(KPrivateDataCage);
    if ((idx != KErrNotFound) && (idx < 3))   // codescanner::magicnumbers
    {
        // In case of device Inbox or Java Installer itself do nothing
        if ((aFileName.FindF(KInboxDataCage) != KErrNotFound) ||
                (aFileName.FindF(KJavaInstallerDataCage) != KErrNotFound))
        {
            return;
        }

        // Copy ALL files with the same base filename as aFileName
        // to Java Installer tmp dir so that both .jad file and
        // the corresponding .jar file are copied.
        // (aFileName is <path>\<name>.<ext>, copy <path>\<name>.*
        // to Java Installer tmp dir.)
        TParse fp;
        mRFs.Parse(aFileName, fp);
        TFileName filesToCopy = fp.DriveAndPath();
        filesToCopy.Append(fp.Name());
        filesToCopy.Append(KAnyExtension);

        // Use JavaInstallCopier.exe to copy the files.
        RProcess rJavaInstallCopier;
        TInt err = rJavaInstallCopier.Create(
            KJavaInstallCopierProcess, filesToCopy);
        if (KErrNone == err)
        {
            TRequestStatus status;
            rJavaInstallCopier.Logon(status);
            rJavaInstallCopier.Resume();
            User::WaitForRequest(status); // codescanner::userWaitForRequest
            err = rJavaInstallCopier.ExitReason();
            rJavaInstallCopier.Close();
            if (KErrNone != err)
            {
                ELOG1(EJavaInstaller,
                      "CJavaSifPlugin::CopyFilesIfNeededL: copying files "
                      "to JavaInstaller data cage failed, err=%d", err);
                User::Leave(err);
            }
        }
        else
        {
            rJavaInstallCopier.Close();
            ELOG1(EJavaInstaller,
                  "CJavaSifPlugin::CopyFilesIfNeededL: starting "
                  "JavaInstallCopier failed, err=%d", err);
            User::Leave(err);
        }

        // aFileName must point to the copy of the file
        aFileName = fp.Drive();
        aFileName.Append(KJavaInstallerTmp);
        aFileName.Append(fp.NameAndExt());
    }

    return;
}


TInt CJavaSifPlugin::IsJadFile(
    RFile& aFileHandle,
    const COpaqueNamedParams& aArguments,
    TBool& aIsJad)
{
    TBool mimeTypeGiven = EFalse;

    TRAP_IGNORE(
        const TDesC &desValue = aArguments.StringByNameL(KSifInParam_MimeType);
        if (desValue.Length() > 0)
        {
            mimeTypeGiven = ETrue;
            if (!desValue.Compare(KJadMimeType))
            {
                aIsJad = ETrue;
            }
            else
            {
                aIsJad = EFalse;
            }
        }
    )
    if (mimeTypeGiven)
    {
        return KErrNone;
    }

    // Mime type was not given in the input arguments, must check the contents
    // of the file itself
    return IsJadFile(aFileHandle, aIsJad);
}


TInt CJavaSifPlugin::IsJadFile(
    RFile& aFileHandle,
    TBool& aIsJad)
{
    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "CJavaSifPlugin::IsJadFile RApaLsSession Connect error %d", err);
        return err;
    }

    TDataType jadFileMimeType(K8JadMimeType);
    err = apaSession.RecognizeSpecificData(aFileHandle, jadFileMimeType, aIsJad);
    apaSession.Close();
    if (KErrNone != err)
    {
        // Just log the error
        ELOG1(EJavaInstaller,
            "CJavaSifPlugin::IsJadFile RApaLsSession RecognizeSpecificData error %d",
            err);
        return err;
    }

    return KErrNone;
}


void CJavaSifPlugin::BuildInstallCommandLine(
    TPtr& aCommandLine,
    const COpaqueNamedParams& aArguments)
{
    // KSifInParam_Drive -> -drive=install_target_drive (A, B, C, ..., Z)
    TInt intValue;
    RArray<TInt> intArray;
    TRAPD(err, intArray = aArguments.IntArrayByNameL(KSifInParam_Drive));
    if (err == KErrNone && intArray.Count() > 0)
    {
        // Java installer supports one installation drive only, so take
        // the first parameter which is the most preferred drive.
        intValue = intArray[0];
        // Value 0 is 'A:' drive and  value 25 is 'Z:' drive
        if ((intValue > -1) && (intValue < 26))  // codescanner::magicnumbers
        {
            aCommandLine.Append(KDrive);
            TChar drive('A');
            drive += intValue;
            aCommandLine.Append(drive);
        }
    }

    // KSifInParam_PerformOCSP Yes/No/AskUser -> -ocsp=yes|no
    intValue = GetPositiveIntParam(KSifInParam_PerformOCSP, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KOcsp);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KOcsp);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_IgnoreOCSPWarnings Yes/No/AskUser -> -ignore_ocsp_warnings=yes|no
    intValue = GetPositiveIntParam(KSifInParam_IgnoreOCSPWarnings, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KIgnoreOcspWarnings);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KIgnoreOcspWarnings);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_AllowUpgrade Yes/No/AskUser -> -upgrade=yes|no
    intValue = GetPositiveIntParam(KSifInParam_AllowUpgrade, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KUpgrade);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KUpgrade);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_AllowUpgradeData Yes/No/AskUser -> -upgrade_data=yes|no
    intValue = GetPositiveIntParam(KSifInParam_AllowUpgradeData, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KUpgradeData);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KUpgradeData);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_AllowUntrusted Yes/No/AskUser -> -untrusted=yes|no
    intValue = GetPositiveIntParam(KSifInParam_AllowUntrusted, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KUntrusted);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KUntrusted);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_AllowOverwrite Yes/No/AskUser -> -overwrite=yes|no
    intValue = GetPositiveIntParam(KSifInParam_AllowOverwrite, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KOverwrite);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KOverwrite);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_AllowDownload Yes/No/AskUser -> -download=yes|no
    intValue = GetPositiveIntParam(KSifInParam_AllowDownload, aArguments);
    if (intValue == Usif::EAllowed) // Yes
    {
        aCommandLine.Append(KDownload);
        aCommandLine.Append(KYes);
    }
    else if (intValue == Usif::ENotAllowed) // No
    {
        aCommandLine.Append(KDownload);
        aCommandLine.Append(KNo);
    }
    // AskUser is not supported

    // KSifInParam_IAP -> -iap=IAP_ID (internet access point id)
    intValue = GetPositiveIntParam(KSifInParam_IAP, aArguments);
    if (intValue > -1)
    {
        // IAP ids are always positive
        aCommandLine.Append(KIap);
        aCommandLine.AppendNum(intValue);
    }

    // KSifInParam_SNAP -> -snap=SNAP_ID (service network access point id)
    intValue = GetPositiveIntParam(KSifInParam_SNAP, aArguments);
    if (intValue > -1)
    {
        // SNAP ids are always positive
        aCommandLine.Append(KSnap);
        aCommandLine.AppendNum(intValue);
    }

    // When this param is given, Java Installer will cancel installation
    // before committing anything. This option is for debugging purposes only.
    // KForceCancel -> -forcecancel
    // The value of parameter is ignored
    TRAP_IGNORE(
        const TDesC &desValue = aArguments.StringByNameL(KForceCancel);
        if (desValue.Length() > 0)
        {
            aCommandLine.Append(KForceCancel);
        }
    )

    std::wstring base64EncodedArgs;
    // KSifInParam_UserName -> -username=download_username
    TRAP_IGNORE(
        const TDesC &desValue = aArguments.StringByNameL(KSifInParam_UserName);
        if (desValue.Length() > 0)
        {
            try
            {
                std::wstring value((wchar_t *)(desValue.Ptr()), desValue.Length());
                std::wstring encodedValue = JavaCommonUtils::wbase64encode(value);
                aCommandLine.Append(KUsername);
                aCommandLine.Append(
                    (const unsigned short *)encodedValue.c_str(), (int)encodedValue.length());
                base64EncodedArgs.append(L"username,");
            }
            catch (std::exception& e)
            {
                ELOG1(EJavaInstaller,
                    "CJavaSifPlugin::BuildInstallCommandLine Base64 encoding user "
                    "name failed, err %s", e.what());
            }
        }
    )

    // KSifInParam_Password -> -password=download_password
    TRAP_IGNORE(
        const TDesC &desValue = aArguments.StringByNameL(KSifInParam_Password);
        if (desValue.Length() > 0)
        {
            try
            {
                std::wstring value((wchar_t *)(desValue.Ptr()), desValue.Length());
                std::wstring encodedValue = JavaCommonUtils::wbase64encode(value);
                aCommandLine.Append(KPassword);
                aCommandLine.Append(
                    (const unsigned short *)encodedValue.c_str(), (int)encodedValue.length());
                base64EncodedArgs.append(L"password,");
            }
            catch (std::exception& e)
            {
                ELOG1(EJavaInstaller,
                    "CJavaSifPlugin::BuildInstallCommandLine Base64 encoding password "
                    "failed, err %s", e.what());
            }
        }
    )

    // KSifInParam_SourceUrl -> -sourceurl=original (HTTP) URL of the JAD or JAR file
    TRAP_IGNORE(
        const TDesC &desValue = aArguments.StringByNameL(KSifInParam_SourceUrl);
        if (desValue.Length() > 0)
        {
            try
            {
                std::wstring value((wchar_t *)(desValue.Ptr()), desValue.Length());
                std::wstring encodedValue = JavaCommonUtils::wbase64encode(value);
                aCommandLine.Append(KSourceUrl);
                aCommandLine.Append(
                    (const unsigned short *)encodedValue.c_str(), (int)encodedValue.length());
                base64EncodedArgs.append(L"sourceurl,");
            }
            catch (std::exception& e)
            {
                ELOG1(EJavaInstaller,
                    "CJavaSifPlugin::BuildInstallCommandLine Base64 encoding source "
                    "url failed, err %s", e.what());
            }
        }
    )

    // KSifInParam_Charset -> -charset=Internet-standard character set name
    TRAP_IGNORE(
        const TDesC &desValue = aArguments.StringByNameL(KSifInParam_Charset);
        if (desValue.Length() > 0)
        {
            try
            {
                std::wstring value((wchar_t *)(desValue.Ptr()), desValue.Length());
                std::wstring encodedValue = JavaCommonUtils::wbase64encode(value);
                aCommandLine.Append(KCharSet);
                aCommandLine.Append(
                    (const unsigned short *)encodedValue.c_str(), (int)encodedValue.length());
                base64EncodedArgs.append(L"charset,");
            }
            catch (std::exception& e)
            {
                ELOG1(EJavaInstaller,
                    "CJavaSifPlugin::BuildInstallCommandLine Base64 encoding charset "
                    "failed, err %s", e.what());
            }
        }
    )

    if (base64EncodedArgs.length() > 0)
    {
        // Tell Java Installer which arguments have been Base64 encoded
        aCommandLine.Append(KBase64);
        aCommandLine.Append(
            (const unsigned short *)base64EncodedArgs.c_str(), (int)base64EncodedArgs.length());
    }

    return;
}

TInt CJavaSifPlugin::GetPositiveIntParam(
        const TDesC& aName,
        const COpaqueNamedParams& aArguments)
{
    // Assume that the value will not be found
    TInt intValue = -1;

    // If the value can be read, intValue will be set, otherwise this function
    // returns -1
    TRAP_IGNORE((void)aArguments.GetIntByNameL(aName, intValue));  // codescanner::voidparameter

    return intValue;
}


TInt CJavaSifPlugin::StartResultsServer(
    COpaqueNamedParams& aResults,
    CComponentInfo& aComponentInfo)
{
    // Destroy old Comms server if it exists
    delete mResultsServer;
    mResultsServer = NULL;

    // Start new Comms server
    mResultsServer = new ResultsServer(aResults, aComponentInfo);  // codescanner::nonleavenew
    if (NULL == mResultsServer)
    {
        return KErrNoMemory;
    }
    else
    {
        // Start the server
        return mResultsServer->start();
    }
}


TBool CJavaSifPlugin::ExitIfJavaInstallerRunning(
    COpaqueNamedParams& aResults,
    TRequestStatus& aStatus)
{
    // If Java Installer is already running, set error category EInstallerBusy etc
    // to aResults and return

    TInt err(KErrNone);
    RProcess proc;
    TFindProcess finder(KJavaInstallerProcess);
    TFullName procName;

    // Java Installer process SID is 0x102033E6 and name is "Installer"
    while (finder.Next(procName) == KErrNone)
    {
        if (proc.Open(finder) != KErrNone)
        {
            continue;
        }
        if (proc.SecureId() == KJavaInstallerSecureID)
        {
            if (proc.ExitType() == EExitPending)
            {
                // Java Installer process is already running
                proc.Close();

                // return error information
                TRAP(err, aResults.AddIntL(KSifOutParam_ErrCategory, EInstallerBusy));
                if (KErrNone != err)
                {
                    ELOG1(EJavaInstaller,
                        "CJavaSifPlugin::ExitIfJavaInstallerRunning aResults.AddIntL "
                        "ErrCategory err %d", err);
                }

                TRAP(err, aResults.AddIntL(KSifOutParam_ErrCode, KErrInUse));
                if (KErrNone != err)
                {
                    ELOG1(EJavaInstaller,
                        "CJavaSifPlugin::ExitIfJavaInstallerRunning aResults.AddIntL "
                        "ErrCode err %d", err);
                }

                // TODO: return also localized error message from usif
                // common localization file after the localized strings are available

                TRequestStatus *statusPtr(&aStatus);
                User::RequestComplete(statusPtr, KErrInUse);
                return ETrue;
            }
       }
       proc.Close();
    }

    return EFalse;
}

//  End of File
