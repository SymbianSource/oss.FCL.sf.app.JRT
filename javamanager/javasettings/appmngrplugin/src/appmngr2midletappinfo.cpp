/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MidletAppInfo implementation
*
*/


#include <caf/caf.h>                    // ContentAccess::CContent
#include <AknsUtils.h>                  // AknsUtils
#include <eikenv.h>                     // CEikonEnv
#include <eikappui.h>                   // CEikAppUi
#include <aknViewAppUi.h>               // CAknViewAppUi
#include <gulicon.h>                    // CGulIcon
#include <browserlauncher.h>            // CBrowserLauncher
#include <javaregistryincludes.h>       // JavaRegistry
#include <x509certchain.h>              // CX509CertChain
#include <imcvcodc.h>                   // TImCodecB64

#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <appmngr2drmutils.h>           // TAppMngr2DRMUtils
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL

#include <javaapplicationsettings.rsg>           // Midlet resource IDs
#include "appmngr2midletappinfo.h"      // CAppMngr2MidletAppInfo
#include "appmngr2midletruntime.h"      // KAppMngr2MidletUid
#include "javaapplicationsettings.hrh"           // Midlet command IDs
#include "appmngr2midletinfoiterator.h" // CAppMngr2MidletInfoIterator
#include "appmngr2midletsettingsview.h" // CAppMngr2MidletSettingsView
#include "appmngr2midletsettingshandler.h" // CAppMngr2MidletSettingsHandler
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

#include "appmngr2midletstorageutil.h"  // Storage util
#include "exceptionbase.h"              // java::util::ExceptionBase exception.
#include "logger.h"                     // LOG
#include "securitystoragedatadefs.h"


// Parameter that is passed to command line when launching Browser.
// 4 - Start/Continue the browser specifying a URL
_LIT(KBrowserStartContinueParam, "4 ");

// MIDlet suite attributes
_LIT(KMIDletDataSize, "MIDlet-Data-Size");
_LIT(KMIDletDescription, "MIDlet-Description");
_LIT(KMIDletInfoURL, "MIDlet-Info-URL");
_LIT(KNokiaUpdateUrl, "Nokia-Update-URL");
_LIT(KNokiaUpdate, "Nokia-Update");
_LIT(KMIDletPermissions, "MIDlet-Permissions");
_LIT(KNokiaMIDletName, "Nokia-MIDlet-Localized-Name");
_LIT(KNokiaMIDletN, "Nokia-MIDlet-Localized-");

_LIT(KListSeparatorAndSpace, ", ");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletAppInfo* CAppMngr2MidletAppInfo::NewL(
    CAppMngr2Runtime& aRuntime,
    Java::CJavaRegistryPackageEntry* aEntry,
    RFs& aFs,
    CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::NewL");
    CAppMngr2MidletAppInfo* self
    = CAppMngr2MidletAppInfo::NewLC(aRuntime, aEntry, aFs, aResourceHandler);
    CleanupStack::Pop(self);
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::NewL");
    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletAppInfo* CAppMngr2MidletAppInfo::NewLC(
    CAppMngr2Runtime& aRuntime,
    Java::CJavaRegistryPackageEntry* aEntry,
    RFs& aFs,
    CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::NewLC");
    CAppMngr2MidletAppInfo* self
    = new(ELeave) CAppMngr2MidletAppInfo(aRuntime, aFs, aResourceHandler);
    CleanupStack::PushL(self);
    self->ConstructL(aEntry);
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::NewLC");
    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::~CAppMngr2MidletAppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletAppInfo::~CAppMngr2MidletAppInfo()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::~CAppMngr2MidletAppInfo");

    CAknViewAppUi* appUi
    = static_cast<CAknViewAppUi*>(CEikonEnv::Static()->EikAppUi());
    appUi->RemoveView(KMidletSuiteSettingsViewId);

    CancelCommand();

    delete iEntry;
    iEntry = NULL;

    delete iJarPath;
    iJarPath = NULL;

    delete iJadPath;
    iJadPath = NULL;

    delete iDetails;
    iDetails = NULL;

    delete iVersion;
    iVersion = NULL;

    delete iMimeType;
    iMimeType = NULL;

    delete iDataSize;
    iDataSize = NULL;

    delete iDescription;
    iDescription = NULL;

    delete iInfoUrl;
    iInfoUrl = NULL;

    delete iUpdateUrl;
    iUpdateUrl = NULL;

    delete iLocalizedMIDletName;
    iLocalizedMIDletName = NULL;

    delete iContent;
    iContent = NULL;

    delete iPermissions;
    iPermissions = NULL;

    delete iAppBitmap;
    iAppBitmap = NULL;

    delete iAppMask;
    iAppMask = NULL;

    delete iInfoIterator;
    iInfoIterator = NULL;

    iCertificates.ResetAndDestroy();

    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::~CAppMngr2MidletAppInfo");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::IconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2MidletAppInfo::IconIndex() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::IconIndex");
    if (iAppBitmap && iAppMask)
    {
        return EAppMngr2UseSpecificIcon;
    }
    return 0;   // Icon 0: qgn_menu_am_midlet
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::SpecificIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CAppMngr2MidletAppInfo::SpecificIconL() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::SpecificIconL");
    CGulIcon* icon = CGulIcon::NewL(iAppBitmap, iAppMask);
    icon->SetBitmapsOwnedExternally(ETrue);
    return icon;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Name()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Name() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Name");
    return *iLocalizedMIDletName;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Details()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Details() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Details");
    if (iDetails)
    {
        return *iDetails;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::IndicatorIconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2MidletAppInfo::IndicatorIconIndex() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::IndicatorIconIndex");
    if (iLocation == EAppMngr2LocationMemoryCard)
    {
        return EAppMngr2IconQgnIndiAmInstMmcAdd;
    }
    if (iLocation == EAppMngr2LocationMassStorage)
    {
        return EAppMngr2IconQgnIndiFmgrMsAdd;
    }
    return EAppMngr2NoIndicatorIcon;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::GetMenuItemsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::GetMenuItemsL(
    RPointerArray<CEikMenuPaneItem::SData>& aMenuCmds)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::GetMenuItemsL");
    TInt resourceOffset = iResourceHandler.AddResourceFileL();
    CEikMenuPaneItem::SData* menuItemData = new(ELeave) CEikMenuPaneItem::SData;
    CleanupStack::PushL(menuItemData);
    ReadMenuItemDataFromResourceL(MIDLET_SUITE_SETTINGS_MENU_ITEM, *menuItemData);
    aMenuCmds.AppendL(menuItemData);
    CleanupStack::Pop(menuItemData);

    if (InfoIteratorL().GetUpdateUrlL() != KNullDesC)
    {
        menuItemData = new(ELeave) CEikMenuPaneItem::SData;
        CleanupStack::PushL(menuItemData);
        ReadMenuItemDataFromResourceL(MIDLET_START_UPDATE_MENU_ITEM, *menuItemData);
        aMenuCmds.AppendL(menuItemData);
        CleanupStack::Pop(menuItemData);
    }

    if (InfoIteratorL().GetInfoUrlL() != KNullDesC)
    {
        menuItemData = new(ELeave) CEikMenuPaneItem::SData;
        CleanupStack::PushL(menuItemData);
        ReadMenuItemDataFromResourceL(MIDLET_GOTO_URL_MENU_ITEM, *menuItemData);
        aMenuCmds.AppendL(menuItemData);
        CleanupStack::Pop(menuItemData);
    }
    if (resourceOffset > 0)
    {
        iResourceHandler.DeleteResourceFile(resourceOffset);
    }
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::GetMenuItemsL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::SupportsGenericCommand()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2MidletAppInfo::SupportsGenericCommand(TInt /*aCmdId*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::SupportsGenericCommand");
    return ETrue;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::HandleCommandL(
    TInt aCommandId, TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::HandleCommandL");
    switch (aCommandId)
    {
    case EAppMngr2CmdViewDetails:
    {
        ShowDetailsL();
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        break;
    }
    case EAppMngr2CmdUninstall:
    {
        UninstallL(aStatus);
        break;
    }
    case EAppMngrCmdSetSuiteSettings:
    {
        ShowSettingsL();
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        break;
    }
    case EAppMngrCmdStartUpdate:
    {
        UpdateL();
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
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::HandleCommandL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::HandleCommandResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::HandleCommandResultL(TInt /*aStatus*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::HandleCommandResultL");
    if (iSWInstLauncher)
    {
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
    }
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::HandleCommandResultL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::CancelCommand()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::CancelCommand()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::CancelCommand");
    if (iSWInstLauncher)
    {
        iSWInstLauncher->CancelAsyncRequest(SwiUI::ERequestUninstall);
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
    }
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::CancelCommand");
}

// -----------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::HandleContentL()
// -----------------------------------------------------------------------------
//
TBool CAppMngr2MidletAppInfo::HandleContentL(
    const TDesC& /*aFileName*/,
    const CAiwGenericParamList& /*aParamList*/,
    TBool& /*aContinue */)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::HandleContentL");
    return EFalse;
}

// -----------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::HandleDownloadedContentHandlerReserved1()
// -----------------------------------------------------------------------------
//
TAny* CAppMngr2MidletAppInfo::DownloadedContentHandlerReserved1(
    TAny* /*aAnyParam*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngrMidletAppInfo::DownloadedContentHandlerReserved1");
    return NULL;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::JarFileName()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::JarFileName() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::JarFileName");
    if (iJarPath)
    {
        return *iJarPath;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::JadFileName()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::JadFileName() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::JadFileName");
    if (iJadPath)
    {
        return *iJadPath;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::MimeType()
// ---------------------------------------------------------------------------
//
const TDesC8& CAppMngr2MidletAppInfo::MimeType() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::MimeType");
    if (iMimeType)
    {
        return *iMimeType;
    }
    return KNullDesC8;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Version()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Version() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Version");
    if (iVersion)
    {
        return *iVersion;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Vendor()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Vendor() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Vendor");
    return iEntry->Vendor();
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::SecurityDomainCategory()
// ---------------------------------------------------------------------------
//
const std::wstring CAppMngr2MidletAppInfo::SecurityDomainCategory() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletAppInfo::MidletSuiteSecurityDomainCategory ");
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletAppInfo::MidletSuiteSecurityDomainCategory ");
    return iSecurityDomainCategory;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::SecurityDomainName()
// ---------------------------------------------------------------------------
//
const std::wstring CAppMngr2MidletAppInfo::SecurityDomainName() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletAppInfo::MidletSuiteSecurityDomainName ");
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletAppInfo::MidletSuiteSecurityDomainName ");
    return iSecurityDomainName;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::DataSize() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::DataSize");
    if (iDataSize)
    {
        return *iDataSize;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Description()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Description() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Description");
    if (iDescription)
    {
        return *iDescription;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::InfoUrl()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::InfoUrl() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::InfoUrl");
    if (iInfoUrl)
    {
        return *iInfoUrl;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::UpdateUrl()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::UpdateUrl() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::UpdateUrl");
    if (iUpdateUrl)
    {
        return *iUpdateUrl;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Content()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Content() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Content");
    if (iContent)
    {
        return *iContent;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::Permissions()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2MidletAppInfo::Permissions() const
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletAppInfo::Permissions");
    if (iPermissions)
    {
        return *iPermissions;
    }
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::CAppMngr2MidletAppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletAppInfo::CAppMngr2MidletAppInfo(CAppMngr2Runtime& aRuntime,
        RFs& aFsSession, CAppMngr2MidletResourceHandler& aResourceHandler) : CAppMngr2AppInfo(aRuntime, aFsSession), iResourceHandler(aResourceHandler)
{
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::ConstructL(
    Java::CJavaRegistryPackageEntry* aEntry)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::ConstructL");
    CAppMngr2AppInfo::ConstructL();

    iEntry = aEntry;

    std::auto_ptr<AppMngr2MidletStorageUtil>storageUtils(
        new AppMngr2MidletStorageUtil());
    storageUtils->populateSuiteInformation(iEntry->Uid());

    _LIT(KJarPath, "JAR_PATH");
    _LIT(KJadPath, "JAD_PATH");

    try
    {
        iJarPath = storageUtils->attribute(KJarPath);    // Ownership taken.
        iJadPath = storageUtils->attribute(KJadPath);    // Ownership taken.
    }
    catch (java::util::ExceptionBase)
    {
        User::Leave(KErrNoMemory);
    }

    iDetails = SizeStringWithUnitsL(iEntry->UsedUserDiskSpace());

    iLocation = TAppMngr2DriveUtils::LocationFromFileNameL(*iJarPath, iFs);
    iLocationDrive = TDriveUnit(*iJarPath);

    // Version
    TAppVersion ver = iEntry->Version();
    TVersion verType(ver.iMajor, ver.iMinor, ver.iBuild);
    iVersion = verType.Name().AllocL();

    // MIME type - AllFiles capability required.
    HBufC* mimeType = HBufC::NewLC(KMaxDataTypeLength);
    ContentAccess::CContent* pkgContent
    = ContentAccess::CContent::NewLC(iJarPath->Des(),
                                     ContentAccess::EContentShareReadOnly);
    TPtr mimePtr(mimeType->Des());
    pkgContent->GetStringAttribute(ContentAccess::EMimeType, mimePtr);
    CleanupStack::PopAndDestroy(pkgContent);
    iMimeType = HBufC8::NewL(mimeType->Length());
    iMimeType->Des().Copy(*mimeType);
    CleanupStack::PopAndDestroy(mimeType);

    // Data size
    const Java::MJavaAttribute* attribute
    = iEntry->AttributeL(KMIDletDataSize()); // no ownership
    if (attribute)
    {
        iDataSize = attribute->Value().AllocL();
    }
    // Description
    attribute = aEntry->AttributeL(KMIDletDescription()); // no ownership
    if (attribute)
    {
        iDescription = attribute->Value().AllocL();
    }
    // Info URL
    attribute = aEntry->AttributeL(KMIDletInfoURL()); // no ownership
    if (attribute)
    {
        iInfoUrl = attribute->Value().AllocL();
    }
    // Update URL
    attribute = aEntry->AttributeL(KNokiaUpdate()); // no ownership
    if (attribute)
    {
        iUpdateUrl = attribute->Value().AllocL();
    }
    else
    {
        attribute = aEntry->AttributeL(KNokiaUpdateUrl()); // no ownership
        if (attribute)
        {
            iUpdateUrl = attribute->Value().AllocL();
        }
    }
    // Use localized midlet suite name if it exists
    attribute = aEntry->AttributeL(KNokiaMIDletName()); // no ownership
    if (attribute)
    {
        iLocalizedMIDletName = attribute->Value().AllocL();
    }
    else
    {
        // Use non-localized name
        iLocalizedMIDletName = aEntry->Name().AllocL();
    }

    RArray<TUid> midletUids;
    CleanupClosePushL(midletUids);
    iEntry->GetEmbeddedEntries(midletUids);


    // Content
    TBuf<50> attributeName;
    TInt contentLength(0);
    // Count length for content descriptor
    for (TInt i(0); i < midletUids.Count(); i++)
    {
        // Localized MIDlet name
        attributeName = KNokiaMIDletN();
        attributeName.AppendNum((i+1));
        attribute = aEntry->AttributeL(attributeName);
        if (attribute)
        {
            contentLength += attribute->Value().Length();
        }
        else
        {
            // Non-localized MIDlet name
            Java::CJavaRegistryApplicationEntry* midlet
            = (Java::CJavaRegistryApplicationEntry*) iEntry->EmbeddedEntryByNumberL(i);
            if (midlet)
            {
                CleanupStack::PushL(midlet);
                contentLength += midlet->Name().Length();
                CleanupStack::PopAndDestroy(midlet);
            }
        }
        contentLength += KListSeparatorAndSpace().Length();
    }

    LOG1(EJavaAppMngrPlugin, EInfo, "contentLength is %d", contentLength);

    // Create content descriptor
    iContent = HBufC::NewL(contentLength);

    // Fill content descriptor
    TPtr ptrContent = iContent->Des();
    for (TInt i(0); i < midletUids.Count(); i++)
    {
        attributeName = KNokiaMIDletN();
        attributeName.AppendNum((i+1));
        attribute = aEntry->AttributeL(attributeName);
        if (attribute)
        {
            // Localized MIDlet name
            if (ptrContent == KNullDesC)
            {
                ptrContent.Append(attribute->Value());
            }
            else
            {
                ptrContent.Append(KListSeparatorAndSpace);
                ptrContent.Append(attribute->Value());
            }
        }
        else
        {
            // Non-localized MIDlet name
            Java::CJavaRegistryApplicationEntry* midlet
            = (Java::CJavaRegistryApplicationEntry*) iEntry->EmbeddedEntryByNumberL(i);
            if (midlet)
            {
                CleanupStack::PushL(midlet);
                if (ptrContent == KNullDesC)
                {
                    ptrContent.Append(midlet->Name());
                }
                else
                {
                    ptrContent.Append(KListSeparatorAndSpace);
                    ptrContent.Append(midlet->Name());
                }
                CleanupStack::PopAndDestroy(midlet);
            }
        }
    }

    // DRM
    iIsDRMProtected = TAppMngr2DRMUtils::IsDRMProtected(*iJarPath);
    iIsROMissingOrExpired = EFalse;
    if (iIsDRMProtected)
    {
        iIsROMissingOrExpired
        = TAppMngr2DRMUtils::IsDRMRightsObjectExpiredOrMissingL(
              *iJarPath);
    }

    // MIDlet suite specific icons
    if (midletUids.Count() > 0)
    {
        AknsUtils::CreateAppIconLC(AknsUtils::SkinInstance(), midletUids[ 0 ],
                                   EAknsAppIconTypeList, iAppBitmap, iAppMask);
        CleanupStack::Pop(2); // iAppBitmap, iAppMask
    }
    CleanupStack::PopAndDestroy(&midletUids);

    // security domain
    CAppMngr2MidletSettingsHandler* settingsHandler
    = CAppMngr2MidletSettingsHandler::NewL(*iLocalizedMIDletName, iEntry->Uid(), iResourceHandler);
    CleanupStack::PushL(settingsHandler);
    iSecurityDomainCategory = settingsHandler->GetSecurityDomainCategory();
    iSecurityDomainName = settingsHandler->GetSecurityDomainName();
    CleanupStack::PopAndDestroy(settingsHandler);
    if (iSecurityDomainCategory != UNIDENTIFIED_THIRD_PARTY_DOMAIN_CATEGORY)
    {
        attribute = aEntry->AttributeL(KMIDletPermissions()); // no ownership
        if (attribute)
        {
            iPermissions = attribute->Value().AllocL();
            iPermissions->Des().TrimAll();
        }
    }

    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::ConstructL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::UninstallL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::UninstallL(TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::UninstallL");
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
    iSWInstLauncher->Uninstall(aStatus, iEntry->Uid(), SwiUI::KJarMIMEType());
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::UninstallL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::ShowDetailsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::ShowDetailsL()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::ShowDetailsL");
    TRAP_IGNORE(ReadCertificatesInfoL());
    CAppMngr2MidletInfoIterator& iterator = InfoIteratorL(); // Not owned
    SwiUI::CommonUI::CCUIDetailsDialog* details
    = SwiUI::CommonUI::CCUIDetailsDialog::NewL();

    if (iIsDRMProtected && !iIsROMissingOrExpired)
    {
        RFile fileHandle;
        TInt err = fileHandle.Open(
                       iFs, *iJarPath,  EFileShareReadersOnly | EFileRead);
        CleanupClosePushL(fileHandle);

        if (iCertificates.Count())
        {
            details->ExecuteLD(iterator, iCertificates, fileHandle);
        }
        else
        {
            details->ExecuteLD(iterator, fileHandle);
        }

        CleanupStack::PopAndDestroy(&fileHandle);
    }
    else
    {
        if (iCertificates.Count())
        {
            details->ExecuteLD(iterator, iCertificates);
        }
        else
        {
            details->ExecuteLD(iterator);
        }
    }
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::ShowDetailsL");
}


// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::ShowSettingsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::ShowSettingsL()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::ShowSettingsL");
    CAknViewAppUi* appUi
    = static_cast<CAknViewAppUi*>(CEikonEnv::Static()->EikAppUi());
    appUi->RemoveView(KMidletSuiteSettingsViewId);
    CAppMngr2MidletSettingsView* settingsView = CAppMngr2MidletSettingsView::NewL(
                *iLocalizedMIDletName,
                iEntry->Uid(),
                iResourceHandler);
    CleanupStack::PushL(settingsView);
    appUi->AddViewL(settingsView);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop(settingsView);
    appUi->ActivateLocalViewL(KMidletSuiteSettingsViewId);
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::ShowSettingsL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::UpdateL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::UpdateL()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::UpdateL");
    CBrowserLauncher* launcher = CBrowserLauncher::NewLC();
    // open browser with info url from jad
    const TDesC* url = &InfoIteratorL().GetUpdateUrlL();

    HBufC* urlParam = HBufC::NewLC(url->Length()
                                   + KBrowserStartContinueParam().Length());
    TPtr urlParamPtr = urlParam->Des();
    urlParamPtr.Append(KBrowserStartContinueParam);
    urlParamPtr.Append(*url);

    launcher->LaunchBrowserSyncEmbeddedL(urlParamPtr, this);

    CleanupStack::PopAndDestroy(urlParam);
    CleanupStack::PopAndDestroy(launcher);
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::UpdateL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::ShowInfoUrlL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::ShowInfoUrlL()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::ShowInfoUrlL");
    CBrowserLauncher* launcher = CBrowserLauncher::NewLC();
    // open browser with info url from jad
    const TDesC* url = &InfoIteratorL().GetInfoUrlL();

    HBufC* urlParam = HBufC::NewLC(url->Length()
                                   + KBrowserStartContinueParam().Length());
    TPtr urlParamPtr = urlParam->Des();
    urlParamPtr.Append(KBrowserStartContinueParam);
    urlParamPtr.Append(*url);

    launcher->LaunchBrowserSyncEmbeddedL(urlParamPtr, this);

    CleanupStack::PopAndDestroy(urlParam);
    CleanupStack::PopAndDestroy(launcher);
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::ShowInfoUrlL");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::InfoIteratorL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletInfoIterator& CAppMngr2MidletAppInfo::InfoIteratorL()
{
    if (!iInfoIterator)
    {
        iInfoIterator = CAppMngr2MidletInfoIterator::NewL(*this,
                        EAppMngr2StatusInstalled, iFs, iResourceHandler);
    }
    return *iInfoIterator;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::ReadCertificatesInfoL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletAppInfo::ReadCertificatesInfoL()
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletAppInfo::ReadCertificatesInfoL");
    if (iCertsRead)
    {
        return;
    }

    // Creates certificate info
    for (TInt i(0); i < iEntry->NumberOfCertificateChains(); i++)
    {
        CX509CertChain* certChain = new(ELeave)CX509CertChain();
        CleanupStack::PushL(certChain);
        CArrayPtrFlat<CX509Certificate>* chain = NULL;
        // In case of leave, decode next certificate
        TRAPD(err, chain = certChain->DecodeCertsL(iEntry->CertificateChain(i)));
        if (err != KErrNone)
        {
            CleanupStack::PopAndDestroy(certChain);
            continue;
        }
        CleanupStack::PushL(chain); // chain
        CleanupResetAndDestroyPushL(*chain); // *chain
        for (TInt certs(0); certs < chain->Count(); certs++)
        {
            CX509Certificate* cert = NULL;
            cert = static_cast<CX509Certificate*>(chain->At(certs));
            SwiUI::CommonUI::CCUICertificateInfo* commonCert =
                SwiUI::CommonUI::CCUICertificateInfo::NewL(*cert);
            iCertificates.Append(commonCert); // ownership transfered
        }
        CleanupStack::PopAndDestroy(chain); // *chain.ResetAndDestroy()
        CleanupStack::PopAndDestroy(chain); // chain
        CleanupStack::PopAndDestroy(certChain);
    }

    iCertsRead = ETrue;
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletAppInfo::ReadCertificatesInfoL");
}
