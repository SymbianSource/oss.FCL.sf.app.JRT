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
* Description:  Java MIDlet runtime type for AppMngr2
*
*/


#include "appmngr2midletruntime.h"      // CAppMngr2MidletRuntime
#include "appmngr2midletappinfo.h"      // CAppMngr2MidletAppInfo
#include "appmngr2midletpackageinfo.h"  // CAppMngr2MidletPackageInfo
#include "appmngr2midletpckginfofactory.h" // CAppMngr2MidletPckgInfoFactory
#include "appmngr2midletsettingsview.h" // KMidletSuiteSettingsViewId
#include "appmngr2recognizedfile.h"     // CAppMngr2RecognizedFile
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

#include <appmngr2midlet.mbg>           // icon IDs
#include <appmngr2runtimeobserver.h>    // MAppMngr2RuntimeObserver

#include <data_caging_path_literals.hrh> // KDC_APP_BITMAP_DIR
#include <AknIconArray.h>               // CAknIconArray
#include <AknsUtils.h>                  // AknsUtils
#include <gulicon.h>                    // CGulIcon
#include <eikenv.h>                     // CEikonEnv
#include <SWInstDefs.h>                 // KJadMIMEType, KJarMIMEType
#include <aknViewAppUi.h>               // CAknViewAppUi
#include <bldvariant.hrh>

#include "logger.h"                     // LOG

// MIDlet icon file name
_LIT(KAppMngr2MidletIconFileNameMif, "AppMngr2Midlet.mif");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletRuntime* CAppMngr2MidletRuntime::NewL(
    MAppMngr2RuntimeObserver& aObserver)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::NewL ");
    CAppMngr2MidletRuntime* self
    = new(ELeave) CAppMngr2MidletRuntime(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::NewL ");

    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::~CAppMngr2MidletRuntime()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletRuntime::~CAppMngr2MidletRuntime()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::~CAppMngr2MidletRuntime ");

    // MIDlet suite settings view must be removed and destroyed before
    // MIDlet Runtime Plug-in is unloaded
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>(eikonEnv->EikAppUi());

    if (appUi)
    {
        appUi->RemoveView(KMidletSuiteSettingsViewId);
    }

    if (iPckgFactory)
    {
        delete iPckgFactory;
        iPckgFactory = NULL;
    }

    if (iJavaRegistry)
    {
        delete iJavaRegistry;
        iJavaRegistry = NULL;
    }
    if (iResourceHandler)
    {
        delete iResourceHandler;
        iResourceHandler = NULL;
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::~CAppMngr2MidletRuntime ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::LoadIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::LoadIconsL(CAknIconArray& aIconArray)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::LoadIconsL ");
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    HBufC* iconFilePath = NULL;

    iconFilePath = FullBitmapFileNameLC(KAppMngr2MidletIconFileNameMif);

    // Icon 0: qgn_menu_am_midlet
    CGulIcon* icon = AknsUtils::CreateGulIconL(skinInstance,
                     KAknsIIDQgnMenuAmMidlet, *iconFilePath,
                     EMbmAppmngr2midletQgn_menu_am_midlet,
                     EMbmAppmngr2midletQgn_menu_am_midlet_mask);
    CleanupStack::PushL(icon);
    aIconArray.AppendL(icon);
    CleanupStack::Pop(icon);

    // Icon 1: qgn_prop_drm_exp_large
    icon = AknsUtils::CreateGulIconL(skinInstance,
                                     KAknsIIDQgnPropDrmRightsExpLarge, *iconFilePath,
                                     EMbmAppmngr2midletQgn_prop_drm_exp_large,
                                     EMbmAppmngr2midletQgn_prop_drm_exp_large_mask);
    CleanupStack::PushL(icon);
    aIconArray.AppendL(icon);
    CleanupStack::Pop(icon);

    CleanupStack::PopAndDestroy(iconFilePath);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::LoadIconsL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::GetSupportedDataTypesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::GetSupportedDataTypesL(
    CDataTypeArray& aDataTypeArray)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::GetSupportedDataTypesL ");
    TDataType jarFile(SwiUI::KJarMIMEType);
    aDataTypeArray.AppendL(jarFile);

    TDataType jadFile(SwiUI::KJadMIMEType);
    aDataTypeArray.AppendL(jadFile);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::GetSupportedDataTypesL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::GetInstallationFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::GetInstallationFilesL(
    RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
    const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
    RFs& aFsSession,
    TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::GetInstallationFilesL ");

    if (!iPckgFactory)
    {
        iPckgFactory = CAppMngr2MidletPckgInfoFactory::NewL(*this, aFsSession, *iResourceHandler);
    }

    iPckgFactory->GetInstallationFilesL(aPackageInfos, aFileList,
                                        aStatus);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::GetInstallationFilesL ");
}


// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::CancelGetInstallationFiles()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::CancelGetInstallationFiles()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::CancelGetInstallationFiles ");
    if (iPckgFactory)
    {
        iPckgFactory->Cancel();
    }
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::GetInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::GetInstalledAppsL(
    RPointerArray<CAppMngr2AppInfo>& aApps,
    RFs& aFsSession, TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::GetInstalledAppsL ");
    RArray<TUid> uids;
    CleanupClosePushL(uids);

    iJavaRegistry->GetRegistryEntryUidsL(Java::EGeneralPackage, uids);
    for (TInt index = 0; index < uids.Count(); index++)
    {
        Java::CJavaRegistryPackageEntry* entry
        = (Java::CJavaRegistryPackageEntry*)
          iJavaRegistry->RegistryEntryL(uids[index]);

        //CleanupStack::PushL(entry);
        // Entry ownership is taken.
        TRAP_IGNORE(GetInstalledAppL(aApps, aFsSession, entry));

        //CleanupStack::Pop(aEntry);
    }

    CleanupStack::PopAndDestroy(&uids);

    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, KErrNone);

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::GetInstalledAppsL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::CancelGetInstalledApps()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::CancelGetInstalledApps()
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletRuntime::CancelGetInstalledApps ");
    // Implementation of GetInstalledAppsL in synchronous, therefore there is no
    // need to cancel operation.
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::GetInstalledAppL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::GetInstalledAppL(
    RPointerArray<CAppMngr2AppInfo>& aApps,
    RFs& aFsSession,
    Java::CJavaRegistryPackageEntry* aEntry)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::GetInstalledAppL ");

    // Entry ownership is taken and it must be set to CleanupStack
    // to avoid double deletion.
    CAppMngr2MidletAppInfo* appInfo = CAppMngr2MidletAppInfo::NewLC(
                                          *this, aEntry, aFsSession, *iResourceHandler);
    //CleanupStack::Pop(aEntry);

    aApps.AppendL(appInfo);   // takes ownership
    CleanupStack::Pop(appInfo);

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::GetInstalledAppL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::CAppMngr2MidletRuntime()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletRuntime::CAppMngr2MidletRuntime(
    MAppMngr2RuntimeObserver& aObserver) : CAppMngr2Runtime(aObserver)
{
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletRuntime::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletRuntime::ConstructL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletRuntime::ConstructL ");
    iJavaRegistry = Java::CJavaRegistry::NewL();
    iResourceHandler = CAppMngr2MidletResourceHandler::NewL();
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletRuntime::ConstructL ");
}
