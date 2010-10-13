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
* Description:  This file contains the implementation of CJavaInstallLauncher
*               class member functions. ECOM Plugin for native installer.
*
*/


#include <memory>
#include <e32cmn.h>
#include <e32property.h>
#include <charconv.h>
#include <data_caging_path_literals.hrh>
#include <SWInstDefs.h>

#include <CUIUtils.h>
#include <eikenv.h>
#include <SisxUIData.rsg>

#include <javadomainpskeys.h>

#include "javauids.h"
#include "logger.h"
#include "javainstalllauncher.h"
#include "javaoslayer.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"

using namespace SwiUI;
using namespace Java::Installer;
using namespace java::util;

_LIT(KJavaInstUIResourceFileName, "SisxUIData.rsc");
_LIT(KJavaInstUIResourceDir, "\\Resource\\");
_LIT(KPrivateDataCage, "\\private\\");
_LIT(KInboxDataCage, "\\private\\1000484b\\");
_LIT(KJavaInstallerDataCage, "\\private\\102033e6\\");
_LIT(KJavaInstallerTmp, "\\private\\102033E6\\installer\\tmp\\");
_LIT(KAnyExtension, ".*");


/**
 * This function is called to hide the 'Preparing Installation' dialog
 * from a timer after 7 seconds have elapsed.
 */
LOCAL_C TInt HidePreInstDialog(TAny* aInstallLauncher)
{
    CJavaInstallLauncher *pLauncher = (CJavaInstallLauncher *)aInstallLauncher;
    if (NULL != pLauncher)
    {
        TRAP_IGNORE(pLauncher->HidePreparingInstallationDialogL());
    }
    return KErrNone;
}


// ============================ MEMBER FUNCTIONS ===============================

CJavaInstallLauncher* CJavaInstallLauncher::NewL()
{
    CJavaInstallLauncher* self = new(ELeave) CJavaInstallLauncher();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CJavaInstallLauncher::~CJavaInstallLauncher()
{
    if (iWaitToHideDialog)
    {
        iWaitToHideDialog->Cancel();
        delete iWaitToHideDialog;
        iWaitToHideDialog = NULL;
    }

    if (iCommonDialogs != NULL)
    {
        TRAP_IGNORE(HidePreparingInstallationDialogL());
    }

    if (iEik && iResourceFileOffset)
    {
        iEik->DeleteResourceFile(iResourceFileOffset);
    }
    iRFs.Close();

    TInt nHandles = iHandlesToClose.Count();
    while (nHandles > 0)
    {
        nHandles--;
        iHandlesToClose[nHandles].Close();
    }
    iHandlesToClose.Reset();
}

void CJavaInstallLauncher::InstallL(RFile& aFile, TInstallReq& aInstallParams,
                                    TBool /* aIsDRM */, TRequestStatus& aStatus)
{
    // aIsDRM parameter is ignored because DRM protected .jar files are always
    // passed to Installer
//    JavaOsLayer::startUpTrace("CJavaInstallLauncher::InstallL start", -1, -1);

    // Show preparing installation dialog to user.
    ShowPreparingInstallationDialogL();

    RProcess rJavaInstaller;
    TFileName fileName;
    // Max two path names and some options -> 1536 is enough
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" install"));

    TInt err = aFile.FullName(fileName);

    // Java Installer does not have AllFiles capability.
    // So if the .jad/.jar file is in the private data cage of
    // Remove Application Management system, it must be copied
    // to the private data cage of Java Installer in this
    // plugin which is executed in native installer which has
    // AllFiles capability.
    CopyFilesIfNeededL(fileName, aFile);

    if (aInstallParams.iMIME == _L8("text/vnd.sun.j2me.app-descriptor"))
    {
        // installation should be started from JAD file
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

    // Find source url. If iSourceURL length is KSWInstHeapPointerLength,
    // iSourceURL contains pointer to URL allocated from heap.
    HBufC* sourceURL= NULL;
    if (aInstallParams.iSourceURL.Length() == KSWInstHeapPointerLength)
    {
        TInstallReqURLPckg urlPckg;
        urlPckg.Copy(aInstallParams.iSourceURL);
        TInstallReqHeapURL heapURL = urlPckg();
        sourceURL = heapURL.iSourcePtr->AllocL();
    }
    else
    {
        sourceURL = aInstallParams.iSourceURL.AllocL();
    }
    // pass also source url
    commandLine.Append(_L(" -sourceurl="));
    commandLine.Append(*sourceURL);
    delete sourceURL;

    commandLine.Append(_L(" -iap="));
    commandLine.AppendNum(aInstallParams.iIAP);

    // Charset is identified by S60 platform Uid,
    // get the Internet-standard name of the charset and
    // pass that to Java Installer
    if (aInstallParams.iCharsetId != 0)
    {
        CCnvCharacterSetConverter *pConverter =
            CCnvCharacterSetConverter::NewL();
        CleanupStack::PushL(pConverter);
        HBufC8* pName =
            pConverter->ConvertCharacterSetIdentifierToStandardNameL(
                aInstallParams.iCharsetId, iRFs);
        CleanupStack::PushL(pName);
        // must convert the 8-bit name to 16-bit
        HBufC16* pLName = Convert(*pName);
        if (pLName)
        {
            commandLine.Append(_L(" -charset="));
            commandLine.Append(*pLName);
        }
        delete pLName;
        CleanupStack::PopAndDestroy(pName);
        CleanupStack::PopAndDestroy(pConverter);
    }

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        // the exit status of Java Installer will be passed to
        // the asynch caller through aStatus
        rJavaInstaller.Logon(aStatus);
    }
    else
    {
        ELOG1(EJavaInstaller,
              "CJavaInstallLauncher::InstallL: starting JavaInstaller failed, err=%d", err);
        User::Leave(err);
    }

    iWaitToHideDialog = CAsyncWaitCallBack::NewL(TCallBack(HidePreInstDialog, this));
    // The active object will wait until Java Installer process calls Rendezvous.
    // If Java Installer specifies reason code EJavaInstaller, then
    // the active object will call call back function that will hide the
    // 'Preparing installation' dialog. If reason code is not EJavaInstaller,
    // the wait object will automatically wait for the next rendezvous.
    //iWaitToHideDialog->Wait( rJavaInstaller, EJavaInstaller );
    iWaitToHideDialog->Wait(rJavaInstaller, 0);

    rJavaInstaller.Resume();

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    iHandlesToClose.Append(rJavaInstaller);
}

void CJavaInstallLauncher::SilentInstallL(RFile& aFile,
        TInstallReq& aInstallParams, TBool /*aIsDRM*/, TInstallOptions& aOptions,
        TRequestStatus& aStatus)
{
//    JavaOsLayer::startUpTrace("CJavaInstallLauncher::SilentInstallL start", -1, -1);
    // aIsDRM parameter is ignored because DRM protected .jar files are always
    // passed to Installer

    RProcess rJavaInstaller;
    TFileName fileName;
    // Max two path name, user name and password and some options ->
    // 1536 is still enough
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" install -silent"));

    TInt err = aFile.FullName(fileName);

    // Java Installer does not have AllFiles capability.
    // So if the .jad/.jar file is in the private data cage of
    // Remove Application Management system, it must be copied
    // to the private data cage of Java Installer in this
    // plugin which is executed in native installer which has
    // AllFiles capability.
    CopyFilesIfNeededL(fileName, aFile);

    if (aInstallParams.iMIME == _L8("text/vnd.sun.j2me.app-descriptor"))
    {
        // installation should be started from JAD file
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

    commandLine.Append(_L(" -iap="));
    commandLine.AppendNum(aInstallParams.iIAP);

    // pass also source url
    commandLine.Append(_L(" -sourceurl="));
    commandLine.Append(aInstallParams.iSourceURL);

    // Charset is identified by S60 platform Uid,
    // get the Internet-standard name of the charset and
    // pass that to Java Installer
    if (aInstallParams.iCharsetId != 0)
    {
        CCnvCharacterSetConverter *pConverter =
            CCnvCharacterSetConverter::NewL();
        CleanupStack::PushL(pConverter);
        HBufC8* pName =
            pConverter->ConvertCharacterSetIdentifierToStandardNameL(
                aInstallParams.iCharsetId, iRFs);
        CleanupStack::PushL(pName);
        // must convert the 8-bit name to 16-bit
        HBufC16* pLName = Convert(*pName);
        if (pLName)
        {
            commandLine.Append(_L(" -charset="));
            commandLine.Append(*pLName);
        }
        delete pLName;
        CleanupStack::PopAndDestroy(pName);
        CleanupStack::PopAndDestroy(pConverter);
    }

    // pass also information in aOptions
    if (EPolicyAllowed == aOptions.iUpgrade)
    {
        commandLine.Append(_L(" -upgrade=yes"));
    }
    else
    {
        commandLine.Append(_L(" -upgrade=no"));
    }

    if (EPolicyAllowed == aOptions.iOCSP)
    {
        commandLine.Append(_L(" -ocsp=yes"));
    }
    else
    {
        commandLine.Append(_L(" -ocsp=no"));
    }

    if (EPolicyAllowed == aOptions.iIgnoreOCSPWarnings)
    {
        commandLine.Append(_L(" -ignore_ocsp_warnings=yes"));
    }
    else
    {
        commandLine.Append(_L(" -ignore_ocsp_warnings=no"));
    }

    if (EPolicyAllowed == aOptions.iUntrusted)
    {
        commandLine.Append(_L(" -untrusted=yes"));
    }
    else
    {
        commandLine.Append(_L(" -untrusted=no"));
    }

    if (EPolicyAllowed == aOptions.iOverwrite)
    {
        commandLine.Append(_L(" -overwrite=yes"));
    }
    else
    {
        commandLine.Append(_L(" -overwrite=no"));
    }

    if (EPolicyAllowed == aOptions.iDownload)
    {
        commandLine.Append(_L(" -download=yes"));
    }
    else
    {
        commandLine.Append(_L(" -download=no"));
    }

    if (aOptions.iLogin.Length() > 0)
    {
        commandLine.Append(_L(" -username="));
        commandLine.Append(aOptions.iLogin);
    }

    if (aOptions.iPassword.Length() > 0)
    {
        commandLine.Append(_L(" -password="));
        commandLine.Append(aOptions.iPassword);
    }

    if (aOptions.iDrive > 'C')
    {
        commandLine.Append(_L(" -drive="));
        commandLine.Append(aOptions.iDrive);
    }

    if (EPolicyAllowed == aOptions.iUpgradeData)
    {
        commandLine.Append(_L(" -upgrade_data=yes"));
    }
    else
    {
        commandLine.Append(_L(" -upgrade_data=no"));
    }

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        // the exit status of Java Installer will be passed to
        // the asynch caller through aStatus
        rJavaInstaller.Logon(aStatus);

        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaInstaller,
              "CJavaInstallLauncher::SilentInstallL: starting JavaInstaller failed, err=%d", err);
    }

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    iHandlesToClose.Append(rJavaInstaller);
}

void CJavaInstallLauncher::UninstallL(const TUid& aUid, const TDesC8&, /*aMIME*/
                                      TRequestStatus& aStatus)
{
//    JavaOsLayer::startUpTrace("CJavaInstallLauncher::UninstallL start", -1, -1);
    RProcess rJavaInstaller;
    TFileName fileName;
    // Max one uid and some options -> 256 is enough
    TBuf<256> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" uninstall"));

    commandLine.Append(_L(" -uid="));
    commandLine.Append(aUid.Name());

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
        ELOG1(EJavaInstaller,
              "CJavaInstallLauncher::UninstallL: starting JavaInstaller failed, err=%d", err);
    }

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    iHandlesToClose.Append(rJavaInstaller);
}

void CJavaInstallLauncher::SilentUninstallL(const TUid& aUid, const TDesC8&, /*aMIME*/
        TUninstallOptions& /*aOptions*/, TRequestStatus& aStatus)
{
//    JavaOsLayer::startUpTrace("CJavaInstallLauncher::SilentUninstallL start", -1, -1);
    RProcess rJavaInstaller;
    TFileName fileName;
    // Max one uid and some options -> 256 is enough
    TBuf<256> commandLine;


#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    // When upgrading Java 2.0 to S60 5.0 devices that already have an older
    // version of Java we have to do one special operation.

    // In old Java environment the PS key that is used to tell how the
    // installation of MIDlet suite proceeds was owned by the native
    // installer (appinstui) process, not by Java Installer.
    // In Java 2.0 environment Java Installer has to own it.

    // We have to destroy the old PS key from this plugin that is loaded and
    // executed by the native installer because the PS key was created so that
    // only the native installer can update and destroy it.
    if (aUid.iUid == KRandomIllegalAppUid)
    {
        // The Uid tells that this is actually request (from javapostupdater.exe)
        // to destroy the PS key.
        LOG(EJavaInstaller, EInfo,
            "CJavaInstallLauncher: SilentUninstallL was called with KRandomIllegalAppUid.");

        TInt err = RProperty::Delete(KUidSystemCategory, KPSUidJavaLatestInstallation);
        if ((err == KErrNone) || (err == KErrNotFound))
        {
            LOG(EJavaInstaller, EInfo,
                "CJavaInstallLauncher: deleted PS key KPSUidJavaLatestInstallation");
            // The caller expects that we leave with cancel status if delete succeeded
            User::Leave(KErrCancel);
        }
        else if (err == KErrPermissionDenied)
        {
            WLOG(EJavaInstaller,
                 "CJavaInstallLauncher: SilentUninstallL: Not owner, so cannot "
                 "delete PS key KPSUidJavaLatestInstallation");
            // It makes no sense to try this again so leave with cancel status
            User::Leave(KErrCancel);
        }
        else
        {
            ELOG1(EJavaInstaller,
                  "CJavaInstallLauncher: SilentUninstallL: Deleting PS key "
                  "KPSUidJavaLatestInstallation error %d", err);
            // Leave with current error code, caller can try deleting PS key again
            User::Leave(err);
        }
    }
#endif

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" uninstall -silent"));

    commandLine.Append(_L(" -uid="));
    commandLine.Append(aUid.Name());

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
        ELOG1(EJavaInstaller,
              "CJavaInstallLauncher::SilentUninstallL: starting JavaInstaller failed, err=%d", err);
    }

    // Do NOT close rJavaInstaller now -> the caller gets notification when the
    // process actually closes.
    iHandlesToClose.Append(rJavaInstaller);
}

TBool CJavaInstallLauncher::IsAppShellUpdate()
{
    // All successfull install/uninstall operations cause need to
    // update AppShell.
    // This method has been marked obsolete in SWInstUIPluginAPI.h
    return ETrue;
}

void CJavaInstallLauncher::Cancel()
{
    // Cancel is not supported.
    // Do nothing.
    return;
}

void CJavaInstallLauncher::CompleteL()
{
    // Do nothing. JavaInstaller process has already exited.
}

CErrDetails* CJavaInstallLauncher::GetErrorDetails()
{
    // Java Installer will log the possible errors
    // also in release builds. It will also show detailed error
    // messages in UI in dialog if not in -silent mode
    return NULL;
}

void CJavaInstallLauncher::CustomUninstallL(TOperation, /*aOperation*/
        const TDesC8&, /*aParams*/
        TRequestStatus& /*aStatus*/)
{
    WLOG(EJavaInstaller, "CJavaInstallLauncher: Not supported CustomUninstallL was called.");
    User::Leave(KErrNotSupported);
}

void CJavaInstallLauncher::SilentCustomUninstallL(TOperation, /*aOperation*/
        TUninstallOptions&, /*aOptions*/
        const TDesC8&, /*aParams*/
        TRequestStatus& /*aStatus*/)
{
    WLOG(EJavaInstaller, "CJavaInstallLauncher: Not supported SilentCustomUninstallL was called.");
    User::Leave(KErrNotSupported);
}

CJavaInstallLauncher::CJavaInstallLauncher()
{
}

void CJavaInstallLauncher::ConstructL()
{
    User::LeaveIfError(iRFs.Connect());
    iWaitToHideDialog = NULL;
}

HBufC16* CJavaInstallLauncher::Convert(const TDesC8& aSrc)
{
    if (aSrc.Length() < 1)
    {
        return NULL;
    }

    HBufC16* buf = HBufC16::New(aSrc.Length() + 1);
    if (buf)
    {
        TPtr16 ptr = buf->Des();
        ptr.Copy(aSrc);
        ptr.ZeroTerminate();
    }
    return buf;
}

void CJavaInstallLauncher::OpenUiResourcesL()
{
    // Open UI env and resource file.
    if (!iEik)
    {
        iEik = CEikonEnv::Static();
    }
    if (!iEik)
    {
        ELOG(EJavaInstaller, "CJavaInstallLauncher: CEikonEnv::Static() call returned NULL.");
        User::Leave(KErrGeneral);
    }
    if (!iResourceFileOffset)
    {
        TFileName resourceFileName;
        resourceFileName.Append(KJavaInstUIResourceDir);
        resourceFileName.Append(KJavaInstUIResourceFileName);
        iResourceFileOffset = CommonUI::CUIUtils::LoadResourceFileL(resourceFileName, iEik);
    }
}

void CJavaInstallLauncher::ShowPreparingInstallationDialogL()
{
    // Open UI resources before showing the dialog.
    OpenUiResourcesL();
    // Open wait dialog.
    iCommonDialogs = CommonUI::CCUIDialogs::NewL();
    iCommonDialogs->ShowWaitDialogL(R_SISXUI_PREPARING, this, R_AVKON_SOFTKEYS_EMPTY);
}

void CJavaInstallLauncher::HidePreparingInstallationDialogL()
{
    // Stop further timed calls
    if (iWaitToHideDialog)
    {
        iWaitToHideDialog->Cancel();
    }

    // Close wait dialog.
    if (iCommonDialogs)
    {
        iCommonDialogs->CloseWaitDialogL();
        delete iCommonDialogs;
        iCommonDialogs = NULL;
    }
}

TBool CJavaInstallLauncher::HandleDialogExitL(TInt /*aButtonId*/)
{
    // There is no way to cancel wait dialog, so this method
    // will never be called.
    return ETrue;
}

void CJavaInstallLauncher::CopyFilesIfNeededL(
    TFileName &aFileName,
    RFile& aFile)
{
    // If the file in the private data cage of some process?
    TInt idx = aFileName.FindF(KPrivateDataCage);
    if ((idx != KErrNotFound) && (idx < 3))
    {
        // In case of device Inbox or Java Installer itself do nothing
        if ((aFileName.FindF(KInboxDataCage) != KErrNotFound)
                || (aFileName.FindF(KJavaInstallerDataCage) != KErrNotFound))
        {
            return;
        }

        // The file handle must be shared or copy operation fails
        TInt tmpErr = aFile.ChangeMode(EFileShareReadersOnly);
        if (KErrNone != tmpErr)
        {
            ELOG1(EJavaInstaller,
                  "CJavaInstallLauncher::CopyFilesIfNeededL: "
                  "Err (%d) when changing the mode of the file handle to shared.", tmpErr);
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

        TInt err = fm->Copy(
                       filesToCopy, KJavaInstallerTmp, CFileMan::ERecurse | CFileMan::EOverWrite);
        delete fm;
        if (KErrNone != err)
        {
            ELOG1(EJavaInstaller,
                  "CJavaInstallLauncher::CopyFilesIfNeededL: err (%d), copying files "
                  "to Java Installer data cage failed", err);
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
