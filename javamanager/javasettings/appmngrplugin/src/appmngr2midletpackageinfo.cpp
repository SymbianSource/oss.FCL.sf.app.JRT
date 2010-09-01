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
* Description:  MidletPackageInfo implementation
*
*/


#include <eikenv.h>                     // CEikonEnv
#include <DRMHelper.h>                  // ContentAccess
#include <browserlauncher.h>            // CBrowserLauncher
#include <StringLoader.h>               // StringLoader

#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <appmngr2drmutils.h>           // TAppMngr2DRMUtils
#include <appmngr2common.hrh>           // Command IDs
#include <javaapplicationsettings.rsg>           // Midlet resource IDs

#include "appmngr2midletpackageinfo.h"  // CAppMngr2MidletPackageInfo
#include "appmngr2midletruntime.h"      // KAppMngr2MidletUid
#include "javaapplicationsettings.hrh"           // Midlet command IDs
#include "appmngr2midletinfoiterator.h" // CAppMngr2MidletInfoIterator

#include "appmngr2midletmanifestreader.h"
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

#include "logger.h"                     // LOG

// Empty string
_LIT(KEmptyString, "");

// Parameter that is passed to command line when launching Browser.
// 4 - Start/Continue the browser specifying a URL
_LIT(KBrowserStartContinueParam, "4 ");

// Icon 0: KAknsIIDQgnMenuAmMidlet - from CAppMngr2MidletRuntime::LoadIconsL()
const TUint KMidletIconId = 0;

void CleanUpResetAndDestroy(TAny *aArray)
{
    if (aArray)
    {
        RPointerArray<HBufC>* array=(RPointerArray<HBufC>*)aArray;
        array->ResetAndDestroy();
        array->Close();
    }
}


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPackageInfo* CAppMngr2MidletPackageInfo::NewL(
    CAppMngr2Runtime& aRuntime,
    const TDesC& aJadFileName,
    const TDesC& aJarFileName,
    RFs& aFs,
    CAppMngr2MidletResourceHandler& aResourceHandler)
{
    CAppMngr2MidletPackageInfo* self = CAppMngr2MidletPackageInfo::NewLC(
                                           aRuntime, aJadFileName, aJarFileName, aFs, aResourceHandler);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPackageInfo* CAppMngr2MidletPackageInfo::NewLC(
    CAppMngr2Runtime& aRuntime,
    const TDesC& aJadFileName,
    const TDesC& aJarFileName,
    RFs& aFs,
    CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::NewLC ");
    CAppMngr2MidletPackageInfo* self
    = new(ELeave) CAppMngr2MidletPackageInfo(aRuntime, aFs, aResourceHandler);
    CleanupStack::PushL(self);
    self->ConstructL(aJadFileName, aJarFileName);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::NewLC ");
    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::~CAppMngr2MidletPackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPackageInfo::~CAppMngr2MidletPackageInfo()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::~CAppMngr2MidletPackageInfo ");
    CancelCommand();
    delete iName;
    iName = NULL;
    delete iDetails;
    iDetails = NULL;
    delete iFileName;
    iFileName = NULL;
    delete iJadFileName;
    iJadFileName = NULL;
    delete iJarFileName;
    iJarFileName = NULL;
    delete iMimeType;
    iMimeType = NULL;
    delete iInfoIterator;
    iInfoIterator = NULL;
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::~CAppMngr2MidletPackageInfo ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::IconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2MidletPackageInfo::IconIndex() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::IconIndex ");
    return KMidletIconId;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::Name()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletPackageInfo::Name() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::Name ");
    if (iName)
    {
        return *iName;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::Details()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletPackageInfo::Details() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::Details ");
    if (iDetails)
    {
        return *iDetails;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::IndicatorIconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2MidletPackageInfo::IndicatorIconIndex() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::IndicatorIconIndex ");
    if (iLocation == EAppMngr2LocationMemoryCard)
    {
        return EAppMngr2IconQgnIndiAmInstMmcAdd;
    }
    else if (iLocation == EAppMngr2LocationMassStorage)
    {
        return EAppMngr2IconQgnIndiFmgrMsAdd;
    }
    return EAppMngr2NoIndicatorIcon;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::GetMenuItemsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::GetMenuItemsL(
    RPointerArray<CEikMenuPaneItem::SData>& aMenuCmds)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::GetMenuItemsL ");
    TInt resourceOffset = 0;
    if (InfoIteratorL().GetInfoUrlL() != KEmptyString)
    {
        CEikMenuPaneItem::SData* menuItemData = new(ELeave) CEikMenuPaneItem::SData;
        CleanupStack::PushL(menuItemData);
        resourceOffset = iResourceHandler.AddResourceFileL();
        ReadMenuItemDataFromResourceL(MIDLET_GOTO_URL_MENU_ITEM, *menuItemData);
        if (resourceOffset > 0)
        {
            iResourceHandler.DeleteResourceFile(resourceOffset);
        }
        aMenuCmds.AppendL(menuItemData);
        CleanupStack::Pop(menuItemData);
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::GetMenuItemsL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::SupportsGenericCommand()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2MidletPackageInfo::SupportsGenericCommand(TInt /*aCmdId*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::SupportsGenericCommand ");
    return ETrue;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::HandleCommandL(TInt aCommandId, TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::HandleCommandL ");
    switch (aCommandId)
    {
    case EAppMngr2CmdRemove :
    {
        TInt ret(KErrNone);
        if (ShowDeleteConfirmationQueryL())
        {
            RemoveL();
        }
        else
        {
            ret = KErrGeneral;
        }
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, ret);
        break;
    }
    case EAppMngr2CmdInstall :
    {
        InstallL(aStatus);
        break;
    }
    case EAppMngr2CmdViewDetails:
    {
        ShowDetailsL();
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        break;
    }
    case EAppMngrCmdGoToUrl:
    {
        ShowInfoUrlL();
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        break;
    }
    default:
    {
        // Uknown command
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNotSupported);
        break;
    }
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::HandleCommandL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::HandleCommandResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::HandleCommandResultL(TInt /*aStatus*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::HandleCommandResultL ");
    // iSWInstLauncher is the only thing that can be ongoing
    if (iSWInstLauncher)
    {
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::HandleCommandResultL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::CancelCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::CancelCommand()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::CancelCommand ");
    if (iSWInstLauncher)
    {
        iSWInstLauncher->CancelAsyncRequest(SwiUI::ERequestInstall);
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::CancelCommand ");
}

// -----------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::HandleContentL()
// -----------------------------------------------------------------------------
//
TBool CAppMngr2MidletPackageInfo::HandleContentL(const TDesC& /*aFileName*/,
        const CAiwGenericParamList& /*aParamList*/,
        TBool& /*aContinue */)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::HandleContentL ");
    return EFalse;
}

// -----------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::HandleDownloadedContentHandlerReserved1()
// -----------------------------------------------------------------------------
//
TAny* CAppMngr2MidletPackageInfo::DownloadedContentHandlerReserved1(
    TAny* /*aAnyParam*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngrMidletPackageInfo::DownloadedContentHandlerReserved1 ");
    return NULL;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::JadFileName()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletPackageInfo::JadFileName() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::JadFileName ");
    if (iJadFileName)
    {
        return *iJadFileName;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::JarFileName()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletPackageInfo::JarFileName() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::JarFileName ");
    if (iJarFileName)
    {
        return *iJarFileName;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::MimeType()
// ---------------------------------------------------------------------------
//
const TDesC8& CAppMngr2MidletPackageInfo::MimeType() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::MimeType ");
    if (iMimeType)
    {
        return *iMimeType;
    }
    return KNullDesC8;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::FileName()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletPackageInfo::FileName() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPackageInfo::FileName ");
    if (iFileName)
    {
        return *iFileName;
    }
    return KNullDesC;
}

// ---------------------------------------------------------
// CAppMngr2MidletPackageInfo::IsDRMProtected()
// ---------------------------------------------------------
//
TBool CAppMngr2MidletPackageInfo::IsDRMProtected() const
{
    return iIsDRMProtected;
}

// ---------------------------------------------------------
// CAppMngr2MidletPackageInfo::ProtectedFileName()
// ---------------------------------------------------------
//
const TDesC& CAppMngr2MidletPackageInfo::ProtectedFileName() const
{
    if (iIsDRMProtected)
    {
        return *iFileName;
    }
    else
    {
        return KNullDesC;
    }
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::CAppMngr2MidletPackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPackageInfo::CAppMngr2MidletPackageInfo(CAppMngr2Runtime& aRuntime,
        RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler) : CAppMngr2PackageInfo(aRuntime, aFs), iResourceHandler(aResourceHandler)
{
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::ConstructL(const TDesC& aJadFileName,
        const TDesC& aJarFileName)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::ConstructL ");
    CAppMngr2PackageInfo::ConstructL();    // base class

    iIsDRMProtected = EFalse;
    TEntry fileEntry;

    iJadFileName = aJadFileName.AllocL();
    iJarFileName = aJarFileName.AllocL();

    if (aJadFileName != KNullDesC)
    {
        User::LeaveIfError(iFs.Entry(aJadFileName, fileEntry));
        iName = fileEntry.iName.AllocL();
        iFileName = aJadFileName.AllocL();
        iMimeType =SwiUI::KJadMIMEType().AllocL();
    }
    else
    {
        User::LeaveIfError(iFs.Entry(aJarFileName, fileEntry));
        iName = fileEntry.iName.AllocL();
        iFileName = aJarFileName.AllocL();
        iMimeType = SwiUI::KJarMIMEType().AllocL();
    }

    // Leave if there is no installation file
    User::LeaveIfNull(iFileName);

    if (aJarFileName != KNullDesC)
    {
        User::LeaveIfError(iFs.Entry(aJarFileName, fileEntry));
        iDetails = SizeStringWithUnitsL(fileEntry.iSize);
        iIsDRMProtected = TAppMngr2DRMUtils::IsDRMProtected(aJarFileName);
    }
    else
    {
        std::auto_ptr<AppMngr2MidletManifestReader>reader(
            new AppMngr2MidletManifestReader(iFs));

        RPointerArray<MJavaAttribute> attributes;

        reader->ReadManifestL(aJadFileName, attributes);

        _LIT(KAttrJarSize, "MIDlet-Jar-Size");

        for (TInt i = 0; i < attributes.Count(); i ++)
        {
            if (attributes[i]->Name() == KAttrJarSize)
            {
                TLex lex(attributes[i]->Value());
                TInt jarSize = 0;
                lex.Val(jarSize);
                // If there is error value is left as zero.
                iDetails = SizeStringWithUnitsL(jarSize);
                break;
            }
        }

        attributes.ResetAndDestroy();
    }

    iLocation = TAppMngr2DriveUtils::LocationFromFileNameL(*iFileName, iFs);
    iLocationDrive = TDriveUnit(*iFileName);

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::ConstructL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::RemoveL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::RemoveL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::RemoveL ");
    if ((*iJadFileName).Length())
    {
        User::LeaveIfError(iFs.Delete(*iJadFileName));
    }
    if ((*iJarFileName).Length())
    {
        User::LeaveIfError(iFs.Delete(*iJarFileName));
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::RemoveL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::InstallL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::InstallL(TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::InstallL ");
    if (!iSWInstLauncher)
    {
        SwiUI::RSWInstLauncher* swInstLauncher
        = new(ELeave) SwiUI::RSWInstLauncher;
        CleanupStack::PushL(swInstLauncher);
        User::LeaveIfError(swInstLauncher->Connect());
        CleanupStack::Pop(swInstLauncher);
        iSWInstLauncher = swInstLauncher;
        swInstLauncher = NULL;
    }
    iSWInstLauncher->Install(aStatus, *iFileName);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::InstallL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::ShowDetailsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::ShowDetailsL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::ShowDetailsL ");
    SwiUI::CommonUI::CCUIDetailsDialog* details
    = SwiUI::CommonUI::CCUIDetailsDialog::NewL();
    details->ExecuteLD(InfoIteratorL());
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPackageInfo::ShowDetailsL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::ShowInfoUrlL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPackageInfo::ShowInfoUrlL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::ShowInfoUrlL ");
    CBrowserLauncher* launcher = CBrowserLauncher::NewLC();
    // open browser with info url from jad
    const TDesC* url = &InfoIteratorL().GetInfoUrlL();

    TInt urlLength = url->Length();

    HBufC* urlParam = HBufC::NewLC(urlLength
                                   + KBrowserStartContinueParam().Length());
    TPtr urlParamPtr = urlParam->Des();
    urlParamPtr.Append(KBrowserStartContinueParam);
    urlParamPtr.Append(*url);

    launcher->LaunchBrowserSyncEmbeddedL(urlParamPtr, this);

    CleanupStack::PopAndDestroy(urlParam);
    CleanupStack::PopAndDestroy(launcher);

    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPackageInfo::ShowInfoUrlL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::InfoIteratorL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletInfoIterator& CAppMngr2MidletPackageInfo::InfoIteratorL()
{
    if (!iInfoIterator)
    {
        iInfoIterator = CAppMngr2MidletInfoIterator::NewL(*this,
                        EAppMngr2StatusNotInstalled, iFs, iResourceHandler);
    }
    return *iInfoIterator;
}
