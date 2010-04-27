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
* Description:  This file contains the implementation of CJavaSifPlugin
*               class member functions. ECOM Plugin for USIF to handle
*               install, uninstall, get component info requests.
*
*/


#include <apgcli.h>
#include <apmstd.h>
#include <e32cmn.h>
#include <e32property.h>
#include <charconv.h>
#include <data_caging_path_literals.hrh>

#include "comms.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"
#include "javasifplugin.h"
#include "javauids.h"
#include "logger.h"
#include "javaoslayer.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "runtimeexception.h"


using namespace Java::Installer;
using namespace java::util;
using namespace java::comms;

_LIT(KPrivateDataCage, "\\private\\");
_LIT(KInboxDataCage, "\\private\\1000484b\\");
_LIT(KJavaInstallerDataCage, "\\private\\102033e6\\");
_LIT(KJavaInstallerTmp, "\\private\\102033E6\\installer\\tmp\\");
_LIT(KAnyExtension, ".*");


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
    iRFs.Close();

    TInt nHandles = iHandlesToClose.Count();
    while (nHandles > 0)
    {
        nHandles--;
        iHandlesToClose[nHandles].Close();
    }
    iHandlesToClose.Reset();

    delete iResultsServer;
    iResultsServer = NULL;

    delete iDummyResults;
    iDummyResults = NULL;

    delete iDummyInfo;
    iDummyInfo = NULL;
}

CJavaSifPlugin::CJavaSifPlugin()
{
}

void CJavaSifPlugin::ConstructL()
{
    User::LeaveIfError(iRFs.Connect());
    User::LeaveIfError(iRFs.ShareProtected());
    iResultsServer = NULL;
    iDummyResults = COpaqueNamedParams::NewL();
    iDummyInfo = CComponentInfo::NewL();
}

void CJavaSifPlugin::GetComponentInfo(
    const TDesC& aFileName,
    const TSecurityContext& aSecurityContext,
    CComponentInfo& aComponentInfo,
    TRequestStatus& aStatus)
{
    RFile fileHandle;
    TInt err = fileHandle.Open(iRFs, aFileName, EFileShareReadersOnly | EFileRead);
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
    RProcess rJavaInstaller;
    TFileName fileName;
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" componentinfo"));

    // Ask Java Installer to send component info back
    // as Comms message. 11000 is IPC_ADDRESS_JAVA_SIF_PLUGIN_C Comms endpoint
    // that our ResultsServer will listen to.
    commandLine.Append(_L(" -commsresult=11000"));

    TInt err = aFileHandle.FullName(fileName);

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
    RApaLsSession apaSession;
    err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::GetComponentInfo RApaLsSession Connect error %d", err);
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    TDataType jadFileMimeType(_L8("text/vnd.sun.j2me.app-descriptor"));
    TBool isJad = EFalse;
    err = apaSession.RecognizeSpecificData(aFileHandle, jadFileMimeType, isJad);
    apaSession.Close();
    if (KErrNone != err)
    {
        // Just log the error
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::GetComponentInfo RApaLsSession RecognizeSpecificData error %d",
              err);
    }
    if (isJad)
    {
        // Installation should be started from JAD file
        commandLine.Append(_L(" -jad="));
    }
    else
    {
        // from JAR file
        commandLine.Append(_L(" -jar="));
    }
    // Filename parameter must be surrounded in double quotes to
    // ensure that spaces in filename are passed correctly.
    commandLine.Append(_L("\""));
    commandLine.Append(fileName);
    commandLine.Append(_L("\""));

    // Start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        // Destroy old Comms server if it exists
        delete iResultsServer;
        iResultsServer = NULL;
        // Start new Comms server that receives component info and sets it to
        // to aComponentInfo.
        iResultsServer = new ResultsServer(*iDummyResults, aComponentInfo);
        if (NULL == iResultsServer)
        {
            err = KErrNoMemory;
        }
        else
        {
            // Start the server
            err = iResultsServer->start();
        }
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
    iHandlesToClose.Append(rJavaInstaller);
}

void CJavaSifPlugin::Install(
    const TDesC& aFileName,
    const TSecurityContext& aSecurityContext,
    const COpaqueNamedParams& aArguments,
    COpaqueNamedParams& aResults,
    TRequestStatus& aStatus)
{
    RFile fileHandle;
    TInt err = fileHandle.Open(iRFs, aFileName, EFileShareReadersOnly | EFileRead);
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::Install Opening file for reading failed with error %d", err);
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
    RProcess rJavaInstaller;
    TFileName fileName;
    // Max two path names and some options -> 1536 is enough
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" install"));

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
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, KErrPermissionDenied);
            return;
        }

        commandLine.Append(_L(" -silent"));
    }

    TBool paramFound = EFalse;
    TInt  intValue = 0;
    TDesC desValue = KNullDesC;

    // KSifInParam_Drive -> -drive=install_target_drive (A, B, C, ..., Z)
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_Drive, intValue));
    if (paramFound)
    {
        // Value 0 is 'A:' drive and  value 25 is 'Z:' drive
        if ((intValue > -1) && (intValue < 26))
        {
            commandLine.Append(_L(" -drive="));
            TChar drive('A');
            drive += intValue;
            commandLine.Append(drive);
        }
        else
        {
            WLOG1(EJavaInstaller,
                "CJavaSifPlugin::Install Ignoring illegal KSifInParam_Drive param (value %d)",
                intValue);
        }
    }

    // KSifInParam_PerformOCSP Yes/No/AskUser -> -ocsp=yes|no
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_PerformOCSP, intValue));
    if (paramFound)
    {
        if (intValue == 0) // Yes
        {
            commandLine.Append(_L(" -ocsp=yes"));
        }
        else if (intValue == 1) // No
        {
            commandLine.Append(_L(" -ocsp=no"));
        }
        // AskUser is not supported
    }

    // KSifInParam_IgnoreOCSPWarnings Yes/No/AskUser -> -ignore_ocsp_warnings=yes|no
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_IgnoreOCSPWarnings, intValue));
    if (paramFound)
    {
        if (intValue == 0) // Yes
        {
            commandLine.Append(_L(" -ignore_ocsp_warnings=yes"));
        }
        else if (intValue == 1) // No
        {
            commandLine.Append(_L(" -ignore_ocsp_warnings=no"));
        }
        // AskUser is not supported
    }

    // KSifInParam_AllowUpgrade Yes/No/AskUser -> -upgrade=yes|no
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_AllowUpgrade, intValue));
    if (paramFound)
    {
        if (intValue == 0) // Yes
        {
            commandLine.Append(_L(" -upgrade=yes"));
        }
        else if (intValue == 1) // No
        {
            commandLine.Append(_L(" -upgrade=no"));
        }
        // AskUser is not supported
    }

    // KSifInParam_AllowUntrusted Yes/No/AskUser -> -untrusted=yes|no
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_AllowUntrusted, intValue));
    if (paramFound)
    {
        if (intValue == 0) // Yes
        {
            commandLine.Append(_L(" -untrusted=yes"));
        }
        else if (intValue == 1) // No
        {
            commandLine.Append(_L(" -untrusted=no"));
        }
        // AskUser is not supported
    }

    // KSifInParam_AllowOverwrite Yes/No/AskUser -> -overwrite=yes|no
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_AllowOverwrite, intValue));
    if (paramFound)
    {
        if (intValue == 0) // Yes
        {
            commandLine.Append(_L(" -overwrite=yes"));
        }
        else if (intValue == 1) // No
        {
            commandLine.Append(_L(" -overwrite=no"));
        }
        // AskUser is not supported
    }

    // KSifInParam_AllowDownload Yes/No/AskUser -> -download=yes|no
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_AllowDownload, intValue));
    if (paramFound)
    {
        if (intValue == 0) // Yes
        {
            commandLine.Append(_L(" -download=yes"));
        }
        else if (intValue == 1) // No
        {
            commandLine.Append(_L(" -download=no"));
        }
        // AskUser is not supported
    }

    // KSifInParam_UserName -> -username=download_username
    TRAP_IGNORE(desValue = aArguments.StringByNameL(KSifInParam_UserName));
    if (desValue.Length() > 0)
    {
        commandLine.Append(_L(" -username="));
        commandLine.Append(desValue);
    }

    // KSifInParam_Password -> -password=download_password
    TRAP_IGNORE(desValue = aArguments.StringByNameL(KSifInParam_Password));
    if (desValue.Length() > 0)
    {
        commandLine.Append(_L(" -password="));
        commandLine.Append(desValue);
    }

    // KSifInParam_SourceUrl -> -sourceurl=original (HTTP) URL of the JAD or JAR file
    TRAP_IGNORE(desValue = aArguments.StringByNameL(KSifInParam_SourceUrl));
    if (desValue.Length() > 0)
    {
        commandLine.Append(_L(" -sourceurl="));
        commandLine.Append(desValue);
    }

    // KSifInParam_IAP -> -iap=IAP_ID (internet access point id)
    TRAP_IGNORE(paramFound = aArguments.GetIntByNameL(KSifInParam_IAP, intValue));
    if (paramFound)
    {
        commandLine.Append(_L(" -iap="));
        commandLine.AppendNum(intValue);
    }

    // KSifInParam_Charset -> -charset=Internet-standard character set name
    TRAP_IGNORE(desValue = aArguments.StringByNameL(KSifInParam_Charset));
    if (desValue.Length() > 0)
    {
        commandLine.Append(_L(" -charset="));
        commandLine.Append(desValue);
    }


    // Ask Java Installer to send installation results back
    // as Comms message. 11000 is IPC_ADDRESS_JAVA_SIF_PLUGIN_C Comms endpoint
    // that our ResultsServer will listen to.
    commandLine.Append(_L(" -commsresult=11000"));

    TInt err = aFileHandle.FullName(fileName);

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
    RApaLsSession apaSession;
    err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::Install RApaLsSession Connect error %d", err);
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }
    TDataType jadFileMimeType(_L8("text/vnd.sun.j2me.app-descriptor"));
    TBool isJad = EFalse;
    err = apaSession.RecognizeSpecificData(aFileHandle, jadFileMimeType, isJad);
    apaSession.Close();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin::Install RApaLsSession RecognizeSpecificData error %d", err);
    }
    if (isJad)
    {
        // Installation should be started from JAD file
        commandLine.Append(_L(" -jad="));
    }
    else
    {
        // from JAR file
        commandLine.Append(_L(" -jar="));
    }
    // Filename parameter must be surrounded in double quotes to
    // ensure that spaces in filename are passed correctly.
    commandLine.Append(_L("\""));
    commandLine.Append(fileName);
    commandLine.Append(_L("\""));

    // Start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        // Destroy old Comms server if it exists
        delete iResultsServer;
        iResultsServer = NULL;
        // Start new Comms server that receives component ids, sets them
        // to aResults.
        iResultsServer = new ResultsServer(aResults, *iDummyInfo);
        if (NULL == iResultsServer)
        {
            err = KErrNoMemory;
        }
        else
        {
            // Start the server
            err = iResultsServer->start();
        }
        if (KErrNone != err)
        {
            // server cannot be started
            rJavaInstaller.Close();
            ELOG1(EJavaInstaller,
                  "CJavaSifPlugin::Install: Cannot start results server, err %d", err);
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
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    rJavaInstaller.Resume();

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    iHandlesToClose.Append(rJavaInstaller);
}

void CJavaSifPlugin::Uninstall(
    TComponentId aComponentId,
    const TSecurityContext& aSecurityContext,
    const COpaqueNamedParams& aArguments,
    COpaqueNamedParams& /* aResults */,
    TRequestStatus& aStatus)
{
    RProcess rJavaInstaller;
    TFileName fileName;
    // Max one uid and some options -> 256 is enough
    TBuf<256> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" uninstall"));

    commandLine.Append(_L(" -cid="));
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
            TRequestStatus *statusPtr(&aStatus);
            User::RequestComplete(statusPtr, KErrPermissionDenied);
            return;
        }

        commandLine.Append(_L(" -silent"));
    }

    // No need to start iResultsServer because Uninstall() does not
    // return anything usefull in aResults. We could return extended
    // error code there.

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    TInt err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
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
        TRequestStatus *statusPtr(&aStatus);
        User::RequestComplete(statusPtr, err);
        return;
    }

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    iHandlesToClose.Append(rJavaInstaller);
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
    if (iHandlesToClose.Count() < 1)
    {
        // No Java Installer process running, do nothing
        WLOG(EJavaInstaller,
            "CJavaSifPlugin::CancelOperation: Cancel called but there is no operation to cancel");
        return;
    }

    try
    {
        CommsMessage message;
        message.setModuleId(PLUGIN_ID_SAMPLE_C);
        message.setReceiver(IPC_ADDRESS_JAVA_INSTALLER_SERVER_C);
        message.setSender(IPC_ADDRESS_JAVA_SIF_PLUGIN_C);
        message.setMessageId(INSTALLER_CANCEL_MESSAGE_ID);

        CommsMessage replyMessage;
        int timeout = 10; // 10 seconds

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
            (void)comms.disconnect();
            return;
        }

        // Ignore the cancel result returned in replyMessage
        // because current SIF API does not return cancel result

        // Ignore possible errors in disconnect
        (void)comms.disconnect();
    }
    catch (ExceptionBase& e)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin: Send cancel msg failed: ExceptionBase caught: %s ",
              e.toString().c_str());
        return;
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaInstaller,
              "CJavaSifPlugin: Send cancel msg failed: Exception %s caught", e.what());
        return;
    }

    // It takes some time before Java Installer had really cancelled
    // the operation and exited. Wait for it to happen because this function
    // must return only after the original asynchronous call is completed.
    TRequestStatus status;
    // This array contains process handles for all Java Installer processes
    // started from this sif plugin. The last handle is the active one.
    // Check if it is still running
    if (iHandlesToClose[iHandlesToClose.Count()-1].ExitReason() != 0)
    {
        // Process has already closed
        return;
    }
    // Wait until it exits
    iHandlesToClose[iHandlesToClose.Count()-1].Logon(status);
    User::WaitForRequest(status);
    // Ignore the exit status of Java Installer because current SIF API
    // does not return cancel result

    return;
}

void CJavaSifPlugin::CopyFilesIfNeededL(TFileName &aFileName)
{
    // Check if the file is in the private data cage of some process
    TInt idx = aFileName.FindF(KPrivateDataCage);
    if ((idx != KErrNotFound) && (idx < 3))
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
        iRFs.Parse(aFileName, fp);

        CFileMan* fm = CFileMan::NewL(iRFs);
        TFileName filesToCopy = fp.DriveAndPath();
        filesToCopy.Append(fp.Name());
        filesToCopy.Append(KAnyExtension);
        TInt err = fm->Copy(filesToCopy, KJavaInstallerTmp, CFileMan::ERecurse);
        delete fm;
        if (KErrNone != err)
        {
            ELOG1(EJavaInstaller,
                  "CJavaSifPlugin::CopyFilesIfNeededL: copying files "
                  "to Java Installer data cage failed, err=%d", err);
            User::Leave(err);
        }

        // aFileName must point to the copy of the file
        aFileName = fp.Drive();
        aFileName.Append(KJavaInstallerTmp);
        aFileName.Append(fp.NameAndExt());
    }

    return;
}

//  End of File
