/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <apgcli.h>
#include <charconv.h>
#include <eikappui.h>
#include <eiksbfrm.h>
#include <uri16.h>
#include <avkon.rsg>
#include <avkon.hrh>
#include <eikon.hrh>
#include <eswtcore.rsg>
#include <eikbtgpc.h>
#include <favouritesfile.h>
#include <favouritesdb.h>
#ifdef ESWT_ON_PUBLIC_SDK
#include <featureinfo.h>
#endif // ESW_ON_PUBLIC_SDK

#include "swtbrowserapsettingshandler.h"
#include "eswtmobileextensions.h"
#include "swtbrowserdialogsproviderproxy.h"
#include "swtbrowseraputils.h"
#include "swtbrowseractiveapdb.h"
#include "swtbrowser.h"
#include "swtbrowsercontainer.h"
#include "swtbrowserloadeventobserver.h"
#include "swtbrowserlinkresolver.h"
#include "swtbrowserstatechangeobserver.h"
#include "swtbrowserspecialloadobserver.h"
#include "swtbrowserdialogsproviderproxy.h"
#include "swtbrowsersoftkeysobserver.h"
#include "swtbrowserpreferences.h"
#include "swtbrowserconnectionmanager.h"
#include "swtuiutils.h"


// Use the same error codes as browsrNG
const TInt KBrsrMalformedUrl = -26018;
const TInt KBrsrUnknownScheme = -26026;
const TInt KMSKCbaButtonIndex = 2;

// A temporary directory location for widgetExtensions
_LIT(KWidgetPath, "C:\\");

// Default URL
_LIT(KUrl, "data:");

typedef CBrCtlInterface*(*FuncPtr_CreateBrowserControlL)(CCoeControl* aParent, TRect aRect,
        TUint aBrCtlCapabilities, TUint aCommandIdBase,
        MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver,
        MBrCtlLinkResolver* aBrCtlLinkResolver,
        MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver,
        MBrCtlLayoutObserver* aBrCtlLayoutObserver,
        MBrCtlDialogsProvider* aBrCtlDialogsProvider,
        MBrCtlWindowObserver* aBrCtlWindowObserver,
        MBrCtlDownloadObserver* aBrCtlDownloadObserver);


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// Constructor. Scrollbar flags are removed before aStyle is passed to the CSwtComposite.
// So, supper class does not know anything about the existence of scrollbar.
// -----------------------------------------------------------------------------
//
CSwtBrowser::CSwtBrowser(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite* aParent
                         , TInt aStyle,TBool aVisibility, TBool aDimmed)
        : CSwtComposite(aDisplay, aPeer, aParent,
                        aStyle & ~KSwtStyleHScroll & ~KSwtStyleVScroll, aVisibility, aDimmed)
        , iBrowserLevelStyle(aStyle)
        , iFeatureManager(EFalse)
{
}

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CSwtBrowser::ConstructL()
{
    CSwtComposite::ConstructL();
    SetBlank(); // needs to draw border at least
    SetComponentsToInheritVisibility(ETrue);
}

// ---------------------------------------------------------------------------
// Separated second phase constructor, specially for the construction of CBrCtlInterface
// ---------------------------------------------------------------------------
//
void CSwtBrowser::ConstructBrowserL(TBool aVisibility)
{
    // Init FavouritesSess
    User::LeaveIfError(iFavouritesSess.Connect());

    // Init FeatureManager
    CSwtLafFacade::InitFeatureManagerL();
    iFeatureManager = ETrue;

    // Init active ap database
    iActiveApDb = CSwtBrowserActiveApDb::NewL();

    // Creating object to hold application settings
    iPreferences = CSwtBrowserPreferences::NewL(*iActiveApDb, *this);
    iPreferences->AddObserverL(this);

    iConnection = new(ELeave) CSwtBrowserConnectionManager(&iActiveApDb->CommsDb(), *this);

    // Set AP to be a default one
    SetRequestedAP(Preferences().DefaultAccessPoint());

    // Create Observers
    iLoadEventObserver = new(ELeave) CSwtBrowserLoadEventObserver(this , iPeer);
    iLinkResolver = new(ELeave) CSwtBrowserLinkResolver(this , iPeer);
    iStateChangeObserver = new(ELeave) CSwtBrowserStateChangeObserver(this, iPeer);
    iSpecialLoadObserver = CSwtBrowserSpecialLoadObserver::NewL(this);
    iDialogsProviderProxy = CSwtBrowserDialogsProviderProxy::NewL(iLoadEventObserver);
    iSoftkeysObserver = new(ELeave) CSwtBrowserSoftkeysObserver(this);

    // BrCtlCapabilities
    TUint capabilityFlags = TBrCtlDefs::ECapabilityLoadHttpFw
                            | TBrCtlDefs::ECapabilityConfirmDownloads
                            | TBrCtlDefs::ECapabilitySavedPage
                            | TBrCtlDefs::ECapabilityUseDlMgr
                            | TBrCtlDefs::ECapabilityCursorNavigation
                            | TBrCtlDefs::ECapabilityFavicon
                            | TBrCtlDefs::ECapabilityToolBar
                            | TBrCtlDefs::ECapabilityFitToScreen;

    if ((iBrowserLevelStyle | KSwtStyleHScroll) || (iBrowserLevelStyle |  KSwtStyleVScroll))
    {
        capabilityFlags = capabilityFlags | TBrCtlDefs::ECapabilityDisplayScrollBar;
    }
    if (Preferences().UiLocalFeatureSupported(
                CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserGraphicalPage)))
    {
        capabilityFlags = capabilityFlags | TBrCtlDefs::ECapabilityGraphicalPage;
    }
    if (Preferences().UiLocalFeatureSupported(
                CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserGraphicalPage)) &&
            Preferences().UiLocalFeatureSupported(
                CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserGraphicalHistory)))
    {
        capabilityFlags = capabilityFlags | TBrCtlDefs::ECapabilityGraphicalHistory;
    }
    if (Preferences().UiLocalFeatureSupported(
                CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserAutoFormFill)))
    {
        capabilityFlags = capabilityFlags | TBrCtlDefs::ECapabilityAutoFormFill;
    }
    if (Preferences().UiLocalFeatureSupported(
                CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserAccessKeys)))
    {
        capabilityFlags = capabilityFlags | TBrCtlDefs::ECapabilityAccessKeys;
    }

    // Create browser container.
    TRect rect=CalcBrowserRect();
    iContainer = CSwtBrowserContainer::NewL(*this);
    iContainer->SetRect(rect);
    iContainer->SetComponentsToInheritVisibility(ETrue);

    TLibraryFunction dllEntry = UiUtils().BrowserDllEntryL();
    FuncPtr_CreateBrowserControlL fptr = (FuncPtr_CreateBrowserControlL) dllEntry;

    iBrCtlInterface = (*fptr)(iContainer,
                              iContainer->Rect(),
                              capabilityFlags,
                              static_cast < TUint >(TBrCtlDefs::ECommandIdBase),    // command base
                              iSoftkeysObserver,
                              iLinkResolver,
                              iSpecialLoadObserver,
                              NULL,
                              iDialogsProviderProxy,
                              NULL,
                              NULL);

    if (!iBrCtlInterface)
    {
        User::Leave(KErrNoMemory);
    }

    TPtrC selfDownloadContentTypes = Preferences().SelfDownloadContentTypesL();
    iBrCtlInterface->SetSelfDownloadContentTypesL(selfDownloadContentTypes);

    // Add two more observers
    iBrCtlInterface->AddLoadEventObserverL(iLoadEventObserver);
    iBrCtlInterface->AddStateChangeObserverL(iStateChangeObserver);

    // Update browser global settings according to values stored in preferences
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoLoadImages, Preferences().AutoLoadImages());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsDisableFlash, !Preferences().AutoLoadImages());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsEmbedded , EFalse);
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsCookiesEnabled, Preferences().Cookies());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsECMAScriptEnabled, Preferences().Ecma());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsIMEINotifyEnabled, Preferences().IMEINotification());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSendRefererHeader, Preferences().SendReferrer());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSecurityWarnings, Preferences().HttpSecurityWarnings());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsPageOverview, Preferences().PageOverview());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsBackList, Preferences().BackList());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoRefresh, Preferences().AutoRefresh());
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoFormFillEnabled, Preferences().FormDataSaving());

    // Update browser local settings according to values stored in preferences
    const TPreferencesValues& pref = iPreferences->AllPreferencesL();
    iFontSize = pref.iFontSize;
    iTextWrap = pref.iTextWrap;
    iEncoding = pref.iEncoding;
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsFontSize, iFontSize);
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsTextWrapEnabled, iTextWrap);
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsCharacterset, iEncoding);
    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsLaunchAppUid, iDisplay.ApplicationUid());

    // WidgetExtension needs a valid base path.
    // Base path would be used to write preferences on Widget runtime context but
    // when embedded into eSWT it does not really write any data. The base path still
    // needs to be provided to avoid any crashes during exit.
    MWidgetExtension* widgetExtension = iBrCtlInterface->CreateWidgetExtensionL(*this);
    widgetExtension->SetParamL(TBrCtlDefs::EWidgetBasePath, KWidgetPath);

    iBrCtlInterface->MakeVisible(aVisibility);
    iContainer->MakeVisible(aVisibility);
    MakeVisible(aVisibility);
    ActivateL();

    if (IsFocused())
    {
        CCoeControl::SetFocus(EFalse, ENoDrawNow);
    }
    ASSERT(!IsFocused());

    iStartedUp = ETrue; // Inddicates Browser is ready for using
}

// ---------------------------------------------------------------------------
// 1st and 2nd phase constructor wrapper.
// ---------------------------------------------------------------------------
//
CSwtBrowser* CSwtBrowser::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               MSwtComposite& aParent, TInt aStyle)
{
    CCoeControl& parentCtrl = aParent.Control()->CoeControl();

    CSwtBrowser* self = new(ELeave) CSwtBrowser(aDisplay, aPeer, &aParent, aStyle,
            parentCtrl.IsVisible(), parentCtrl.IsDimmed());
    CleanupStack::PushL(self);
    self->ConstructL();
    self->UiUtils().NotifyNewBrowserCreated();
    self->ConstructBrowserL(parentCtrl.IsVisible());
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSwtBrowser::~CSwtBrowser()
{
    if (iFeatureManager)
    {
        CSwtLafFacade::UnInitFetureManager();
        iFeatureManager = EFalse;
    }

    CSwtBrowserConnectionManager* inetconman = static_cast <CSwtBrowserConnectionManager*>(iConnection);
    delete inetconman;

    if (iPreferences)
    {
        iPreferences->RemoveObserver(this);
        delete iPreferences;
    }

    delete iActiveApDb;
    iFavouritesSess.Close();

    if (iBrCtlInterface)
    {
        iBrCtlInterface->RemoveLoadEventObserver(iLoadEventObserver);
        iBrCtlInterface->RemoveStateChangeObserver(iStateChangeObserver);
        delete iBrCtlInterface;
    }

    delete iContainer;

    delete iLoadEventObserver;
    delete iLinkResolver;
    delete iStateChangeObserver;
    delete iSpecialLoadObserver;
    delete iDialogsProviderProxy;
    delete iSoftkeysObserver;
}

CBrCtlInterface& CSwtBrowser::BrCtlInterface() const
{
    ASSERT(iBrCtlInterface);
    return *iBrCtlInterface;
}


CSwtBrowserActiveApDb& CSwtBrowser::ActiveApDb() const
{
    ASSERT(iActiveApDb);
    return *iActiveApDb;
}

MBrowserPreferences& CSwtBrowser::Preferences() const
{
    ASSERT(iPreferences);
    return *iPreferences;
}

CSwtBrowserLoadEventObserver& CSwtBrowser::LoadObserver() const
{
    ASSERT(iLoadEventObserver);
    return *iLoadEventObserver;
}

CSwtBrowserSpecialLoadObserver& CSwtBrowser::SpecialLoadObserver() const
{
    ASSERT(iSpecialLoadObserver);
    return *iSpecialLoadObserver;
}

MSwtDisplay& CSwtBrowser::Display() const
{
    return iDisplay;
}

void CSwtBrowser::UpdateCbaL()
{
    if (StartedUp())
    {
        CEikButtonGroupContainer& cba = iDisplay.UiUtils().Cba();

        TBrCtlDefs::TBrCtlElementType elementtype =
            iBrCtlInterface->FocusedElementType();
        // Cba2 must set to cancel and command observer must be added
        // when a inputbox is actvated becasue browser supposes the concel command is there
        // and will send a simulated cba2 event to deactivate input box
        if ((elementtype == TBrCtlDefs::EElementActivatedInputBox)
                || (elementtype == TBrCtlDefs::EElementActivatedObjectBox))
        {
            if (iCbaState != CSwtBrowser::EActivatedInputBox)
            {
                iCbaState = CSwtBrowser::EActivatedInputBox;
                HBufC* label = iEikonEnv->AllocReadResourceL(R_SWT_SOFTKEY_TEXT_CANCEL);
                CleanupStack::PushL(label);
                iDisplay.CommandArranger()->UpdateRightSoftkeyCommandAndObserver(
                    KSwtCommandBaseId + EAknSoftkeyCancel, *label, *this);
                CleanupStack::Pop(label);
            }
            return;
        }

        if (iCbaState != CSwtBrowser::EDefault)
        {
            iCbaState = CSwtBrowser::EDefault;
            iDisplay.CommandArranger()->RefreshCba();
        }
    }
}

#ifdef RD_SCALABLE_UI_V2
void CSwtBrowser::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    iHandlingPointerEv = ETrue;

    // From parent Shell coordinates to CSwtBrowserContainer coordinates
    TPointerEvent event(aPointerEvent);
    event.iPosition -= Rect().iTl;

    iBrCtlInterface->HandlePointerEventL(event);

    // Must grab to allow HandlePointerBufferReadyL to be forwarded to Browser
    // Enables panning / touch scrolling.
    iBrCtlInterface->ClaimPointerGrab(EFalse);

    iHandlingPointerEv = EFalse;
}
#endif

// ---------------------------------------------------------------------------
// This function only gets called on device
// ---------------------------------------------------------------------------
//
TBool CSwtBrowser::StartPreferencesViewL()
{
    TUint32 defaultAp = iPreferences->DefaultAccessPoint();
    TBool retVal(EFalse);
    if (defaultAp == KWmlNoDefaultAccessPoint)
    {
        // Showing AP selection dialog
        CSwtBrowserApSettingsHandler* ApUi = NULL;
        ApUi = CSwtBrowserApSettingsHandler::NewLC();

        TRAPD(err, ApUi->RunSettingsL(defaultAp, defaultAp));
        CleanupStack::PopAndDestroy(ApUi);
        if (err)
        {
            defaultAp = KWmlNoDefaultAccessPoint;
        }
        iPreferences->SetDefaultAccessPointL(defaultAp);
        defaultAp = iPreferences->DefaultAccessPoint(); //get it
    }
    if (defaultAp != KWmlNoDefaultAccessPoint)
    {
        SetRequestedAP(defaultAp);
        retVal = ETrue;
    }
    return retVal;
}

TBool CSwtBrowser::IsConnecting() const
{
    return (LoadObserver().LoadState() != CSwtBrowserLoadEventObserver::ELoadStateIdle);
}

void CSwtBrowser::SetRequestedAP(TInt aAp)
{
    iRequestedAp = aAp;
}

TBool CSwtBrowser::StartedUp() const
{
    return iStartedUp;
}

TUint32 CSwtBrowser::RequestedAp() const
{
    return iRequestedAp;
}

void CSwtBrowser::FetchSavedDeckL(TInt aUid)
{
    _LIT(KSavedPage,  "savedpage://");
    _LIT(KUidFormat, "%d");
    const TInt KUidFullMaxLength = 22;// max uid length, containing length of KSavedPage
    const TInt KUidMaxLength = 10;// max uid length, not containing length of KSavedPage


    User::LeaveIfError(iFavouritesSess.Connect());
    RFavouritesDb db;
    User::LeaveIfError(db.Open(iFavouritesSess, KBrowserSavedPages));
    CleanupClosePushL<RFavouritesDb>(db);
    RFavouritesFile favFile;
    if (favFile.Open(db, aUid) == KErrNone)
    {
        TInt siz;
        favFile.Size(siz);
        HBufC8* buf  = HBufC8::NewLC(siz);
        TPtr8 des = buf->Des();
        TInt err = favFile.Read(des);
        if (err == KErrNone)
        {
            TBuf < KUidFullMaxLength > uidText;
            TBuf < KUidMaxLength > uid ;
            uid.Format(KUidFormat, aUid);
            uidText.Copy(KSavedPage);
            uidText.Append(uid);
            HBufC16* copyBuf = HBufC::NewLC(siz);
            copyBuf->Des().Copy(des);
            LoadObserver().DoStartLoad(CSwtBrowserLoadEventObserver::ELoadUrlTypeOther);
            BrCtlInterface().LoadSavedPageL(uidText , *copyBuf);
            CleanupStack::PopAndDestroy(copyBuf);
        }
        favFile.Close();
        CleanupStack::PopAndDestroy(buf);
    }
    CleanupStack::PopAndDestroy(); // db
}

MBrowserConnection& CSwtBrowser::Connection() const
{
#if defined( _DEBUG )
    _LIT(KAppName, "eSwtBrowser");
#endif
    __ASSERT_DEBUG(iConnection, User::Panic(KAppName, 0));
    return *iConnection;
}

void CSwtBrowser::HandlePreferencesChangeL(TPreferencesEvent aEvent, TPreferencesValues& aValues,
        TBrCtlDefs::TBrCtlSettings aSettingType)
{
    switch (aEvent)
    {
    case EPreferencesActivate:
    {
        // Provide settings
        aValues.iFontSize = iFontSize;
        aValues.iTextWrap = iTextWrap;
        aValues.iEncoding = iEncoding;
        break;
    }
    case EPreferencesDeactivate:
    {
        // Save preferences and notify Browser Control
        UpdateLocalPreferencesL(aValues);
        break;
    }
    case EPreferencesItemChange:
    {
        // All preferences changes should notify the Browser Control
        UpdateGlobalPreferencesL(aSettingType);

        // Change the local setting to the changed setting.
        UpdateLocalPreferencesL(aValues);
        break;
    }
    default:
        break;
    }
}

// ---------------------------------------------------------
// The funtion just posts a callback event.
// Loading URL will be started by callback event framework
// ---------------------------------------------------------
//
TBool CSwtBrowser::BackL()
{
    HBufC* url = iEikonEnv->AllocReadResourceAsDes16LC(R_SWT_URL_BACK);
    iLoadEventObserver->SetUrlL(*url);
    iDisplay.PostLocationChangingEventL(iPeer, *this, ETrue, ETrue, *url, ESwtBrGoBack);
    CleanupStack::PopAndDestroy(url);
    return ETrue;
}

// ---------------------------------------------------------
// The funtion just posts a callback event. Loading URL will be started by callback event framework
// ---------------------------------------------------------
//
TBool CSwtBrowser::ForwardL()
{
    HBufC* url = iEikonEnv->AllocReadResourceAsDes16LC(R_SWT_URL_FORWARD);
    iLoadEventObserver->SetUrlL(*url);
    iDisplay.PostLocationChangingEventL(iPeer, *this, ETrue, ETrue, *url, ESwtBrGoForward);
    CleanupStack::PopAndDestroy(url);
    return ETrue;
}

// ---------------------------------------------------------
// The URL is available only when PageInfo is available
// ---------------------------------------------------------
//
HBufC* CSwtBrowser::GetUrlL()
{
    HBufC * urlText = NULL;
    HBufC * url = iBrCtlInterface->PageInfoLC(TBrCtlDefs::EPageInfoUrl);

    if (url)
    {
        urlText = url->AllocL(); // Not push to CleanupStack becasue of no more leave function
    }
    else
    {
        urlText = iEikonEnv->AllocReadResourceAsDes16L(R_SWT_NO_NAME);
    }
    CleanupStack::PopAndDestroy(url);
    return urlText;
}

TBool CSwtBrowser::IsBackEnabled() const
{
    return  iBrCtlInterface->NavigationAvailable(TBrCtlDefs::ENavigationBack);
}

TBool CSwtBrowser::IsForwardEnabled() const
{
    return  iBrCtlInterface->NavigationAvailable(TBrCtlDefs::ENavigationForward);
}

void CSwtBrowser::RefreshL()
{
    HBufC * url = iBrCtlInterface->PageInfoLC(TBrCtlDefs::EPageInfoUrl);

    // Crashes on Avkon side when no correct URL is set
    if (*url != KUrl)
    {
        iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandReload +
                                        TBrCtlDefs::ECommandIdBase);
    }

    CleanupStack::PopAndDestroy(url);
}

void CSwtBrowser::StopL()
{
    CancelFetchL();
}

TBool CSwtBrowser::SetTextL(const TDesC& aHtml)
{
    // convert html from 16bit to 8bit
    TInt htmlSize = aHtml.Size();
    HBufC8* html = HBufC8::NewLC(htmlSize);
    TPtr8  ptr = html->Des();
    ptr.Copy(aHtml);

    _LIT8(KDataType, "text/html");
    TDataType dataType(KDataType());

    TUid uid;
    //uid.iUid = KCharacterSetIdentifierIso88591;
    uid.iUid = iEncoding;

    HBufC * url = iEikonEnv->AllocReadResourceAsDes16LC(R_SWT_URL_SETTEXT);
    iLoadEventObserver->SetUrlL(*url);
    CleanupStack::PopAndDestroy(url);

    iBrCtlInterface->LoadDataL(KUrl, *html, dataType, uid);

    CleanupStack::PopAndDestroy(html);
    return ETrue;
}

TBool CSwtBrowser::SetUrlL(const TDesC&  aUrl)
{
    TUriParser uriParser;
    TInt err = uriParser.Parse(aUrl);
    if (err || !aUrl.Length())
    {
        iCoeEnv->HandleError(KBrsrUnknownScheme);
        return EFalse;
    }


    iLoadEventObserver->SetUrlL(aUrl);

    iDisplay.PostLocationChangingEventL(iPeer, *this, ETrue, ETrue, aUrl, ESwtBrLoadUrl);

    return ETrue;
}

void CSwtBrowser::DoSetUrlL(const TDesC&  aUrl,
                            TSwtBrCallBackOperationType aOperationType)
{

    switch (aOperationType)
    {
    case ESwtBrLoadUrl: // load a Url
        FetchL(aUrl);  // FetchL will notify  load event observer
        break;
    case ESwtBrGoBack: // go back
        iLoadEventObserver->DoStartLoad(
            CSwtBrowserLoadEventObserver::ELoadUrlTypeOther);
        iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandBack
                                        + TBrCtlDefs::ECommandIdBase);
        break;
    case ESwtBrGoForward: //go forward
        iLoadEventObserver->DoStartLoad(
            CSwtBrowserLoadEventObserver::ELoadUrlTypeOther);
        iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandForward
                                        + TBrCtlDefs::ECommandIdBase);
        break;
    case ESwtBrDoNothing:// do nothing
        break;
    default:
        ASSERT(EFalse);
    }
}

void CSwtBrowser::HandleFreeRamEventL(TInt aEventType)
{

    if (aEventType ==  CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserFreeRam))
    {
        BrCtlInterface().HandleCommandL(
            (TInt)TBrCtlDefs::ECommandFreeMemory + (TInt)TBrCtlDefs::ECommandIdBase);
    }
    else if (aEventType == CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserMemoryGood))
    {
        BrCtlInterface().HandleCommandL(
            (TInt)TBrCtlDefs::ECommandMemoryGood + (TInt)TBrCtlDefs::ECommandIdBase);
    }
}

void CSwtBrowser::DoDelete()
{
    UiUtils().GarbageCollect(*this);
}

TSwtPeer CSwtBrowser::Dispose()
{
    if (Fetching())
    {
        TRAP_IGNORE(CancelFetchL());
    }

    if (iBrCtlInterface)
    {
        if (IsFocused())
        {
            CoeControl().SetFocus(EFalse);
            TRAP_IGNORE(iBrCtlInterface->HandleCommandL(
                            TBrCtlDefs::ECommandLoseFocus + TBrCtlDefs::ECommandIdBase));
        }
    }

    //if( !iBrCtlInterface->OkToExit() )
    //{
    //return;
    //ASSERT( EFalse );
    //}

    if (iConnection)
    {
        TRAP_IGNORE(iBrCtlInterface->HandleCommandL(
                        TBrCtlDefs::ECommandDisconnect + TBrCtlDefs::ECommandIdBase));
        iConnection->Disconnect();
    }
    iBrCtlInterface->ClearCache();

    // If being garbage collected, must hide. If disposing for real, avoid cursor crash.
    MakeVisible(EFalse);

    return CSwtComposite::Dispose();
}

HBufC* CSwtBrowser::MSKLabelL() const
{
    return KNullDesC().AllocL();
}

TBool CSwtBrowser::IsKeyUsed(TUint aKeyCode) const
{
    if (aKeyCode == EKeyCBA1 || aKeyCode == EKeyCBA2)
    {
        return EFalse;
    }
    return ETrue;
}

TBool CSwtBrowser::CbaButtonIsUsed(TInt aCbaButtonIndex) const
{
    if (aCbaButtonIndex == KMSKCbaButtonIndex)
    {
        return ETrue;
    }

    return EFalse;
}

TBool CSwtBrowser::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    // Don't want CSwtComposite's focusability behavior.
    return ASwtScrollableBase::IsFocusable(aReason);
}

MSwtBrowser* CSwtBrowser::BrowserInterface() const
{
    const MSwtBrowser* browser = this;
    return const_cast<MSwtBrowser*>(browser);
}

MSwtComposite* CSwtBrowser::Composite()
{
    return this;
}

TSize CSwtBrowser::ComputeSizeL(TInt aWHint, TInt aHHint)
{

    TSize computeSize(0, 0);

    if (!iBrCtlInterface)
    {
        if (aWHint != KSwtDefault)
        {
            computeSize.iWidth = aWHint;
        }
        if (aHHint != KSwtDefault)
        {
            computeSize.iHeight = aHHint;
        }
        return computeSize;
    }

    // Start from content size
    computeSize = iBrCtlInterface->ContentSize();

    // Update according to minimum size
    TSize minimumSize = iBrCtlInterface->MinimumSize();
    if (computeSize.iWidth < minimumSize.iWidth)
    {
        computeSize.iWidth = minimumSize.iWidth;
    }
    if (computeSize.iHeight < minimumSize.iHeight)
    {
        computeSize.iHeight = minimumSize.iHeight;
    }

    // Update according to top shell size
    TSize shellSize = iDisplay.UiUtils().GetControlTopShell(*this).Control()->
                      ClientRect().Size();
    if (computeSize.iWidth > shellSize.iWidth)
    {
        computeSize.iWidth = shellSize.iWidth;
    }
    if (computeSize.iHeight > shellSize.iHeight)
    {
        computeSize.iHeight = shellSize.iHeight;
    }

    // Update to hints suggested size
    if (aWHint != KSwtDefault)
    {
        computeSize.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        computeSize.iHeight = aHHint;
    }

    return computeSize;
}

// ----------------------------------------------------
// Browser rect = container rect - border - scrollbars
// ----------------------------------------------------
//
TRect CSwtBrowser::CalcBrowserRect()
{
    // Calculate rect relative to browser's parent
    TRect inclusiveRect(BorderInnerRect());
    TRect clientRect(SbInnerRect(inclusiveRect));
    return clientRect;
}

void CSwtBrowser::FetchL(const TDesC& aUrl,
                         CSwtBrowserLoadEventObserver::TBrowserLoadUrlType aUrlType)
{
    TFavouritesWapAp ap;
    ap.SetDefault();
    FetchL(aUrl, KNullDesC, KNullDesC, ap, aUrlType);
}

void CSwtBrowser::FetchL(const TDesC& aUrl, const TDesC& aUsername,
                         const TDesC& aPassword, const TFavouritesWapAp& aAccessPoint,
                         CSwtBrowserLoadEventObserver::TBrowserLoadUrlType aUrlType)
{
    // Cancel the previous fetch if there is any
    if (Fetching())
    {
        CancelFetchL();
    }

    TUriParser uriParser;
    TInt err = uriParser.Parse(aUrl);
    if (err || !aUrl.Length())
    {
        return iCoeEnv->HandleError(KBrsrUnknownScheme);
    }
    HBufC* resultUrlBuf = CreateFullUrlLC(aUrl, aUsername, aPassword);

    // Set requested ap in following order
    // 1. from user provided ap
    // 2. from preference


    if (aAccessPoint.IsNull() || aAccessPoint.IsDefault()
            || (!IsValidAPFromParamL(aAccessPoint.ApId())))
    {
        // user didn't provide volid ap
        // use default ap from preferences
        iRequestedAp = Preferences().DefaultAccessPoint();
    }
    else
    {
        // user's ap seems good
        iRequestedAp = aAccessPoint.ApId();
    }

    // check validity of the requested ap
    // set always ask mode on if request ap is invalid

    // define dummy ap
    const TInt KDummyAp(2);
    const TInt KDummyApId(2);

#ifndef __WINSCW__
    TUint32 IAPid(iRequestedAp);
    if (iRequestedAp == KWmlNoDefaultAccessPoint)
    {
        // no defined ap
        // do not start ap setting dialog here becasue network connection is not alwasys needed,
        // e.g. loading url locally

        // set always ask mode
        iPreferences->SetAccessPointSelectionModeL(
            EWmlSettingsAccessPointSelectionModeAlwaysAsk);
        // just set dummy id not to let Ap engine or Ap Util to leave
        iRequestedAp = KDummyAp;
        IAPid = KDummyApId;
    }
    else
    {
        // there is a defined ap
        CCommsDatabase& db = iActiveApDb->CommsDb();
        CSwtBrowserApUtils* apUtils = CSwtBrowserApUtils::NewL(db);
        CleanupStack::PushL(apUtils);
        IAPid = apUtils->IapIdFromWapIdL(iRequestedAp);
        CleanupStack::PopAndDestroy(apUtils);
        // set user selection mode
        iPreferences->SetAccessPointSelectionModeL(
            EWmlSettingsAccessPointSelectionModeUserDefined);
    }
#else
    // On WINS, don't start setting dialog here
    // as the connection manager will popup a selection dialog
    // just use dummy id
    iRequestedAp = KDummyAp;
    TUint32 IAPid = KDummyApId;
    // set always ask mode
    iPreferences->SetAccessPointSelectionModeL(
        EWmlSettingsAccessPointSelectionModeAlwaysAsk);
#endif //__WINSCW__

    //Load Url
    LoadObserver().DoStartLoad(aUrlType);   //notify load observer
    if (aUrlType == CSwtBrowserLoadEventObserver::ELoadUrlTypeSavedDeck)
    {
        TRAP(err, BrCtlInterface().LoadUrlL(resultUrlBuf->Des()
                                            , IAPid, TBrCtlDefs::ECacheModeNoCache));
    }
    else
    {
        TRAP(err, BrCtlInterface().LoadUrlL(resultUrlBuf->Des(), IAPid));
    }
    CleanupStack::PopAndDestroy(resultUrlBuf);

    if (err != KErrNone)
    {
        CancelFetchL();
    }

    //Handle errors
    switch (err)
    {
    case KErrNotFound:
        iCoeEnv->HandleError(KBrsrMalformedUrl);
        break;
    case KErrNone:
        break;
    default:
        User::Leave(err);    // pass error code
        break;
    }
}

TBool CSwtBrowser::Fetching() const
{
    return (LoadObserver().LoadState() != CSwtBrowserLoadEventObserver::ELoadStateIdle);
}

void CSwtBrowser::CancelFetchL(TBool aIsUserInitiated /*= EFalse*/)
{
    LoadObserver().DoEndLoad(aIsUserInitiated);   // notify load observer
    iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandCancelFetch
                                    + TBrCtlDefs::ECommandIdBase);  // send cancel command
}

HBufC* CSwtBrowser::CreateFullUrlLC(const TDesC& aUrl, const TDesC& aUsername,
                                    const TDesC& aPassword)
{
    // username[:password]@server/document
    TInt lPw = aPassword.Length();
    TInt lUn = aUsername.Length();
    _LIT(KBrowserScheme, "://");
    const TInt KBrowserSchemeLength = 3;
    TInt index = aUrl.Find(KBrowserScheme);
    if (index == KErrNotFound)
        index = 0;  // insert at first position
    else
        index += KBrowserSchemeLength;  // insert after scheme

    HBufC *fullUrl = HBufC::NewLC(aUrl.Length() + lUn + lPw + KBrowserSchemeLength);
    fullUrl->Des().Copy(aUrl);
    if (lPw + lUn)
    {
        _LIT(KAt, "@");
        fullUrl->Des().Insert(index, KAt);
    }
    if (lPw)
    {
        _LIT(KColon, ":");
        fullUrl->Des().Insert(index, aPassword);
        fullUrl->Des().Insert(index, KColon);
    }
    if (lUn)
    {
        fullUrl->Des().Insert(index, aUsername);
    }
    fullUrl->Des().ZeroTerminate();
    return fullUrl;
}

TBool CSwtBrowser::IsValidAPFromParamL(TUint32 aIAPid)
{
    CCommsDatabase& db = iActiveApDb->CommsDb();
    CSwtBrowserApUtils* apUtils = CSwtBrowserApUtils::NewL(db);
    CleanupStack::PushL(apUtils);
    TBool retVal = apUtils->WapApExistsL(aIAPid);
    CleanupStack::PopAndDestroy(apUtils);
    return retVal;
}

void CSwtBrowser::UpdateLocalPreferencesL(TPreferencesValues& aValues)
{
    if (iFontSize != aValues.iFontSize)
    {
        iFontSize = aValues.iFontSize;
        iBrCtlInterface->SetBrowserSettingL(
            static_cast < TUint >(TBrCtlDefs::ESettingsFontSize), iFontSize) ;
    }

    if (iTextWrap != aValues.iTextWrap)
    {
        iTextWrap = aValues.iTextWrap;
        iBrCtlInterface->SetBrowserSettingL(
            static_cast < TUint >(TBrCtlDefs::ESettingsTextWrapEnabled), iTextWrap);
    }
    if (iEncoding != aValues.iEncoding)
    {
        iEncoding = aValues.iEncoding;
        iBrCtlInterface->SetBrowserSettingL(
            static_cast < TUint >(TBrCtlDefs::ESettingsCharacterset), iEncoding);
    }
}

void CSwtBrowser::UpdateGlobalPreferencesL(TBrCtlDefs::TBrCtlSettings aSettingType)
{
    // Local Settings should be ignored
    switch (aSettingType)
    {
    case TBrCtlDefs::ESettingsAutoLoadImages:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoLoadImages, Preferences().AutoLoadImages());
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsDisableFlash, !Preferences().AutoLoadImages());
        break;
    }
    case TBrCtlDefs::ESettingsEmbedded:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsEmbedded, 0);   // Always disable embeded mode
        break;
    }
    case TBrCtlDefs::ESettingsCookiesEnabled:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsCookiesEnabled, Preferences().Cookies());
        break;
    }
    case TBrCtlDefs::ESettingsECMAScriptEnabled:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsECMAScriptEnabled, Preferences().Ecma());
        break;
    }
    case TBrCtlDefs::ESettingsIMEINotifyEnabled:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsIMEINotifyEnabled, Preferences().IMEINotification());
        break;
    }
    case TBrCtlDefs::ESettingsSendRefererHeader:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSendRefererHeader, Preferences().SendReferrer());
        break;
    }
    case TBrCtlDefs::ESettingsSecurityWarnings:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSecurityWarnings, Preferences().HttpSecurityWarnings());
        break;
    }

    case TBrCtlDefs::ESettingsPageOverview:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsPageOverview, Preferences().PageOverview());
        break;
    }
    case TBrCtlDefs::ESettingsBackList:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsBackList, Preferences().BackList());
        break;
    }
    case TBrCtlDefs::ESettingsAutoRefresh:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoRefresh, Preferences().AutoRefresh());
        break;
    }

    case TBrCtlDefs::ESettingsAutoFormFillEnabled:
    {
        iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoFormFillEnabled, Preferences().FormDataSaving());
        break;
    }

    // LOCAL SETTINGS should be ignored
    case TBrCtlDefs::ESettingsTextWrapEnabled:
    case TBrCtlDefs::ESettingsFontSize:
    case TBrCtlDefs::ESettingsCharacterset:
    {
        break;
    }

    case TBrCtlDefs::ESettingsUnknown: // ignore
    default:
        break;
    }
}


// ---------------------------------------------------------
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSwtBrowser::SizeChanged()
{
    if (StartedUp())
    {
        CSwtComposite::SizeChanged();
    }

    if (iContainer  && iBrCtlInterface)
    {
        iContainer->SetRect(CalcBrowserRect());
    }
}

// ---------------------------------------------------------
// Called by framework when the view position is changed
// ---------------------------------------------------------
//
void CSwtBrowser::PositionChanged()
{
    CSwtComposite::PositionChanged();
    if (iContainer  && iBrCtlInterface)
    {
        iContainer->SetPosition(CalcBrowserRect().iTl);
    }
}

void CSwtBrowser::FocusChanged(TDrawNow aDrawNow)
{
    if (iContainer && iBrCtlInterface)
    {
        iContainer->SetFocus(IsFocused());
        if (IsFocused())
        {
            TRAP_IGNORE(iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandGainFocus +
                        TBrCtlDefs::ECommandIdBase));
        }
        else
        {

            if (iCbaState ==  CSwtBrowser::EActivatedInputBox)
            {
                TRAP_IGNORE(iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandAccept +
                            TBrCtlDefs::ECommandIdBase));
            }

            TRAP_IGNORE(iBrCtlInterface->HandleCommandL(
                            TBrCtlDefs::ECommandLoseFocus + TBrCtlDefs::ECommandIdBase));
        }
    }

    CAknControl::FocusChanged(ENoDrawNow);

    if (StartedUp())
    {
        HandleFocusChanged(aDrawNow);
        if (iCbaState ==  CSwtBrowser::EActivatedInputBox)
        {
            iCbaState = CSwtBrowser::EDefault; // force to update cba2
            TRAP_IGNORE(UpdateCbaL());
        }
    }
}

TInt CSwtBrowser::CountComponentControls() const
{
    TInt result = 0;

    // Scroll-bar frame's components
    if (iSbFrame)
    {
        result += iSbFrame->CountComponentControls();
    }
    if (iContainer)   // browser control
    {
        result += 1;
    }
    return result;
}

CCoeControl* CSwtBrowser::ComponentControl(TInt aIndex) const
{
    // Scroll-bar frame's components
    TInt count = 0;
    if (iSbFrame)
    {
        count = iSbFrame->CountComponentControls();
        if (aIndex < count)
        {
            return iSbFrame->ComponentControl(aIndex);
        }
    }

    if (iContainer && aIndex >= count && aIndex < CountComponentControls())
    {
        return iContainer;
    }

    ASSERT(EFalse);
    return NULL;
}

TKeyResponse CSwtBrowser::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // Well, well, well :) The history view (back view) translates
    // pointer events into EEventKeyUp Select key events. If we
    // detect that the new key event starts with EEventKeyUp and
    // not EEventKeyDown as it should, we assume it is due to the
    // history view trick and therefore we just forward the key
    // to the native browser.
    TKeyResponse res = EKeyWasNotConsumed;
    if (iHandlingPointerEv)
    {
        res = iBrCtlInterface->OfferKeyEventL(aKeyEvent, aType);
    }
    else
    {
        res = HandleKeyL(aKeyEvent, aType, EFalse);
    }
    return res;
}

void CSwtBrowser::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TKeyResponse result(EKeyWasNotConsumed);
    TKeyEvent keyEvent(aKeyEvent);

    // convert Enter key to KeyOk, this is only for emulator
    if (aType == EEventKey &&  keyEvent.iCode == EKeyEnter)
    {
        keyEvent.iCode = EKeyOK;
    }

    if (aType == EEventKey && keyEvent.iCode == EKeyOK)
    {
        if (keyEvent.iRepeats && iSelectionKeyPressed)
        {
            TBrCtlDefs::TBrCtlElementType elementtype =
                iBrCtlInterface->FocusedElementType();
            TInt command(KErrNotFound);
            switch (elementtype)
            {
            case TBrCtlDefs::EElementAnchor:
            {
                command = TBrCtlDefs::ECommandOpen;
                break;
            }
            default:
                break;
            }
            if (command == TBrCtlDefs::ECommandOpen)
            {
                iSelectionKeyPressed = EFalse;
                keyEvent.iCode = EKeyNull;
                result = EKeyWasConsumed;
                iBrCtlInterface->HandleCommandL(
                    command + TBrCtlDefs::ECommandIdBase);
            }
        }
    }

    // Stop the event handling when find item was actived
    if ((keyEvent.iScanCode == EStdKeyDevice3 || keyEvent.iScanCode == EStdKeyXXX)
            && aType == EEventKeyDown)
    {
        iSelectionKeyPressed = ETrue;
        keyEvent.iCode = EKeyNull;
    }

    if (result == EKeyWasNotConsumed)
    {
        result = iBrCtlInterface->OfferKeyEventL(keyEvent, aType);
    }

    if (result == EKeyWasNotConsumed &&
            (keyEvent.iScanCode == EStdKeyDevice3 || keyEvent.iScanCode == EStdKeyXXX) &&
            aType == EEventKeyUp  &&
            iSelectionKeyPressed)
    {
        // Selection key was released and it is not a long press
        iSelectionKeyPressed = EFalse;
        aType = EEventKey;
        keyEvent.iCode = EKeyOK;

        iBrCtlInterface->OfferKeyEventL(keyEvent, aType);
    }
}

void CSwtBrowser::ProcessCommandL(TInt aCmdId)
{
    if (aCmdId-KSwtCommandBaseId == EAknSoftkeyCancel)
    {
        iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandCancel + TBrCtlDefs::ECommandIdBase);
    }
    else if (aCmdId-KSwtCommandBaseId == EAknSoftkeyOk)
    {
        if (iCbaState == CSwtBrowser::EActivatedInputBox)
        {
            iBrCtlInterface->HandleCommandL(
                TBrCtlDefs::TBrCtlDefs::ECommandAccept + TBrCtlDefs::ECommandIdBase);
        }
    }
}
