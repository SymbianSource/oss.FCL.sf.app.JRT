/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CAppMngr2MidletSettingsView implementation
*
*/


// INCLUDE FILES
#include <aknViewAppUi.h>                       // CAknViewAppUi
#include <aknradiobuttonsettingpage.h>          // CAknRadioButtonSettingPage
#include <featmgr.h>                            // FeatureManager
#include <StringLoader.h>                       // StringLoader
#include <akntitle.h>                           // CAknTitlePane
#include <aknmessagequerydialog.h>              // CAknMessageQueryDialog
#include <AknUtils.h>                           // AknLayoutUtils
#include <hlplch.h>                             // HlpLauncher
#include "javaapplicationsettings.hlp.hrh"

#include <javaapplicationsettings.rsg>                   // Midlet resource IDs
#include <cmapplicationsettingsui.h>            // CCmApplicationSettingsUi

#include "appmngr2midletsettingscontainer.h"    // CAppMngr2MidletSettingsContainer
#include "appmngr2midletsettingsview.h"         // CAppMngr2MidletSettingsView
#include "javaapplicationsettings.hrh"                   // Midlet command IDs
#include "appmngr2midletsettingshandler.h"
#include "javasymbianoslayer.h"                 // STL <-- --> Symbian conv.
#include "securitystoragedatadefs.h"
#include "connectionmanager.h"                  // ConnectionManager.
#include "javacommonutils.h"
#include "logger.h"                     // LOG
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

#include <vector>
#include <algorithm>

using namespace std;
using namespace java::util;

const int SECURITY_WARNINGS_MODE_PAGE = 1;
const int SECURITY_SETTINGS_PAGE = 2;
const int ON_SCREEN_KEYPAD_PAGE = 3;

const int SENSIBLE_FUNCTION_GROUPS = 9;
const int NET_ACCESS_SETTINGS_INDEX = 0;
const int LOW_LEVEL_NET_ACCESS_SETTINGS_INDEX = 1;
const int CALL_CONTROL_SETTINGS_INDEX = 2;
const int MESSAGING_SETTINGS_INDEX = 3;
const int RESTRICTED_MESSAGING_SETTINGS_INDEX = 4;
const int APPLICATION_AUTO_INVOCATION_SETTINGS_INDEX = 5;
const int LOCAL_CONNECTIVITY_SETTINGS_INDEX = 6;
const int MULTIMEDIA_RECORDING_SETTINGS_INDEX = 7;
const int READ_USER_DATA_ACCESS_SETTINGS_INDEX = 8;

const TUid KOwnUid = { 0x20016BF7 };  // Own Uid, used with help

// ================= MEMBER FUNCTIONS =======================

CAppMngr2MidletSettingsView* CAppMngr2MidletSettingsView::NewL(
    const TDesC& aMidletSuiteName,
    TUid aMidletSuiteUid,
    CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::NewL ");
    CAppMngr2MidletSettingsView* self
    = new(ELeave) CAppMngr2MidletSettingsView(aResourceHandler);
    CleanupStack::PushL(self);
    self->ConstructL(aMidletSuiteName, aMidletSuiteUid);
    CleanupStack::Pop(self);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::NewL ");
    return self;
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::CAppMngr2MidletSettingsView
// ---------------------------------------------------------
//
CAppMngr2MidletSettingsView::CAppMngr2MidletSettingsView(CAppMngr2MidletResourceHandler& aResourceHandler)
        : iContainer(NULL), iSettingsHandler(NULL), iOnscreenKeypadSettingsVisible(EFalse), iSecuritySettingsVisible(EFalse), iResourceFileOffset(0), iResourceHandler(aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::CAppMngr2MidletSettingsView ");
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::CAppMngr2MidletSettingsView ");
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsView::ConstructL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::ConstructL ");
    iResourceFileOffset = iResourceHandler.AddResourceFileL();
    BaseConstructL(R_APPMNGR_VIEW_CONTEXT_SPECIFIC);
    InitL(aMidletSuiteName,aMidletSuiteUid);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::ConstructL ");
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::~CAppMngr2MidletSettingsView()
// Class destructor
// ---------------------------------------------------------
//
CAppMngr2MidletSettingsView::~CAppMngr2MidletSettingsView()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::~CAppMngr2MidletSettingsView ");
    if (iContainer)
    {
        AppUi()->RemoveFromViewStack(*this, iContainer);
    }
    delete iSettingsHandler;
    delete iContainer;
    delete[] iBlanketSettings;
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::~CAppMngr2MidletSettingsView ");
}

// ---------------------------------------------------------
// TUid CAppMngr2MidletSettingsView::Id()
// Return settings view id
// ---------------------------------------------------------
//
TUid CAppMngr2MidletSettingsView::Id() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsView::Id ");
    return KMidletSuiteSettingsViewId;
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsView::HandleClientRectChange()
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsView::HandleClientRectChange ");
    if (iContainer)
    {
        iContainer->SetRect(ClientRect());
    }
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::DoActivateL(...)
// Activates the view
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsView::DoActivateL(
    const TVwsViewId& aPrevViewId, TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::DoActivateL ");
    iPrevViewId = aPrevViewId;
    iContainer->MakeVisible(ETrue);
    iContainer->SetRect(ClientRect());
    iContainer->ActivateL();
    iContainer->ListBox()->SetListBoxObserver(this);
    iContainer->ListBox()->DrawNow();
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::DoActivateL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletSettingsView::HandleCommandL(TInt aCommand)
// ---------------------------------------------------------------------------
void CAppMngr2MidletSettingsView::HandleCommandL(TInt aCommand)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::HandleCommand ");
    switch (aCommand)
    {
    case EAknSoftkeyChange:
    case EAppMngrCmdOkPressed:
    case EAknSoftkeySelect:
    {
        HandleSettingsL();
        break;
    }
    case EAknSoftkeyBack:
    {
        AppUi()->ActivateLocalViewL(iPrevViewId.iViewUid);
        break;
    }
    case EAknSoftkeyExit:
    {
        static_cast<MEikCommandObserver*>(AppUi())->ProcessCommandL(EAknCmdExit);
        break;
    }
    case EAknCmdHelp:
        if (FeatureManager::FeatureSupported(KFeatureIdHelp))
        {
            CArrayFix<TCoeHelpContext>* contexts =
                new(ELeave) CArrayFixFlat<TCoeHelpContext>(1);
            CleanupStack::PushL(contexts);
            TCoeHelpContext context;
            context.iMajor = KOwnUid;
            context.iContext = HLP_JAVA_APPLICATION_SETTINGS;
            contexts->AppendL(context);
            CleanupStack::Pop(contexts);
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), contexts);
        }
        break;
    default:
    {
        AppUi()->HandleCommandL(aCommand);
        break;
    }
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::HandleCommand ");
}


// ---------------------------------------------------------------------------
// CAppMngr2MidletSettingsView::HandleListBoxEventL
//
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletSettingsView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
        TListBoxEvent aEventType)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsView::HandleListBoxEventL ");
    switch (aEventType)
    {
    case EEventEnterKeyPressed:
    case EEventItemDoubleClicked:
    case EEventItemSingleClicked:
        HandleSettingsL();
        break;
    default:
        break;
    }
}

void CAppMngr2MidletSettingsView::HandleSettingsL()
{
    switch (iContainer->ListBox()->CurrentItemIndex())
    {
    case 0:
        ShowAccessPointPageL();
        break;
    case 1:
        if (iOnscreenKeypadSettingsVisible)
        {
            ShowPageL(ON_SCREEN_KEYPAD_PAGE);
        }
        else if (iSecuritySettingsVisible)
        {
            ShowPageL(SECURITY_WARNINGS_MODE_PAGE);
        }
        break;
    case 2:
        if (iOnscreenKeypadSettingsVisible && iSecuritySettingsVisible)
        {
            ShowPageL(SECURITY_WARNINGS_MODE_PAGE);
            break;
        }
    default:
        ShowPageL(SECURITY_SETTINGS_PAGE);
    }
}

/**
 * Start settings page
 */
void CAppMngr2MidletSettingsView::ShowAccessPointPageL()
{
    TUint32 snapId = JavaCommonUtils::wstringToInt(iContainer->GetListBoxItem(0).getValue().getId());

    // show the dialog
    CCmApplicationSettingsUi* handler = CCmApplicationSettingsUi::NewLC();
    TUint listItems = CMManager::EShowAlwaysAsk
                      | CMManager::EShowDefaultConnection
                      | CMManager::EShowDestinations;
    TBearerFilterArray filter;
    TCmSettingSelection selection;
    switch (snapId)
    {
    case(TUint)KAlwaysAskId:
        selection.iResult = CMManager::EAlwaysAsk;
        break;
    case(TUint)KJavaNetworkAccessNotSpecified:
    case(TUint)KDefaultConnection:
        selection.iResult = CMManager::EDefaultConnection;
        break;
    default:    // User defined SNAP
        selection.iResult = CMManager::EDestination;
    }
    selection.iId = snapId;
    TBool ret = handler->RunApplicationSettingsL(selection, listItems, filter);
    CleanupStack::PopAndDestroy(handler);
    if (ret)
    {
        switch (selection.iResult)
        {
        case CMManager::EAlwaysAsk:
            snapId = (TUint)KAlwaysAskId;
            break;
        case CMManager::EDestination:
            snapId = selection.iId;
            break;
        case CMManager::EDefaultConnection:
            snapId = (TUint)KDefaultConnection;
            break;
        default:
            User::Leave(KErrNotSupported);
        }
        CAppMngr2SuiteSnapItem* snap = new(ELeave) CAppMngr2SuiteSnapItem;
        CleanupStack::PushL(snap);
        snap->iId = snapId;
        iSettingsHandler->SetSnapL(*snap);
        CleanupStack::PopAndDestroy(snap);
        CAppMngr2SuiteSnapItem* newSnap = iSettingsHandler->GetSnapL();
        CleanupStack::PushL(newSnap);
        iContainer->RefreshListBoxContentL(0,  0, AppMngr2MidletSettingsUtil::SnapToListItem(*newSnap, true /* enabled */));
        CleanupStack::PopAndDestroy(newSnap);
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::ShowSuiteSnapSelectDialogL ");
}

/**
 * Start page
 */
void CAppMngr2MidletSettingsView::ShowPageL(int pageId)
{
    TInt listBoxContentCurrentItemIndex = iContainer->ListBox()->CurrentItemIndex();
    if (!iOnscreenKeypadSettingsVisible)
    {
        listBoxContentCurrentItemIndex++;
    }
    ListItem currentItem = iContainer->GetListBoxItem(listBoxContentCurrentItemIndex);
    TInt selectionIndex = currentItem.getCurrentValue();
    int currentSelection = currentItem.getCurrentValue();
    vector<LocalizedString> values = currentItem.getValues();

    if (values.size() == 1)
    {
        // stop right here
        return;
    }

    if (values.size() == 2)
    {
        // if there are only two values, then do a quick selection (change to the other one)
        selectionIndex = (currentSelection == 0 ? 1 : 0);
    }
    else
    {
        // if there are more than two values, go on show the list
        CDesCArrayFlat* itemArray = new(ELeave) CDesCArrayFlat(values.size());
        CleanupStack::PushL(itemArray);
        for (int i=0; i<values.size(); i++)
        {
            wstring value = values[i].getValue();
            TPtr ptr((unsigned short*)value.c_str(), value.size());
            ptr.SetLength(value.size());
            itemArray->AppendL(ptr);
        }
        CAknSettingPage* dlg = new(ELeave) CAknRadioButtonSettingPage(
            AppMngr2MidletSettingsUtil::GetLocalizedSettingsName(
                currentItem.getName().getId()),
            selectionIndex, itemArray);

        // execute the setting page and check if the setting value was changed:
        dlg->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
        CleanupStack::PopAndDestroy(itemArray);
    }
    // handle the selection
    if (currentSelection != selectionIndex)
    {
        iContainer->SetListBoxItemValue(
            listBoxContentCurrentItemIndex,
            selectionIndex);
        int err = KErrNone;
        MidletSuiteSecuritySettings settings;
        switch (pageId)
        {
        case SECURITY_WARNINGS_MODE_PAGE:
            err = iSettingsHandler->SetSecurityWarningsMode(
                      (selectionIndex == 0 ? SECURITY_WARNINGS_DEFAULT_MODE:SECURITY_WARNINGS_USER_DEFINED_MODE));
            if (err == KErrNone)
            {
                iContainer->RefreshListBoxContentL(
                    listBoxContentCurrentItemIndex,
                    selectionIndex,
                    (selectionIndex == 1) /* aAllEnabled*/);
            }
            break;
        case SECURITY_SETTINGS_PAGE:
            settings = AppMngr2MidletSettingsUtil::ListItemToSettings(
                           iContainer->GetListBoxItem(listBoxContentCurrentItemIndex));
            if (!CheckBlanketSettingsL(settings))
            {
                err = KErrNotFound;
                break;
            }
            err = iSettingsHandler->SetSecuritySettings(
                      settings.getName(), settings.getCurrentInteractionMode());
            if (err == KErrNone)
            {
                if (settings.getCurrentInteractionMode() == BLANKET_INTERACTION_MODE)
                {
                    // "blanket mode, if selected by the user, MUST NOT result in any
                    // additional prompts" (MIDP spec) -> mark down that the prompt
                    // in blanket mode was shown
                    iSettingsHandler->SetUserSecuritySettingsPromptFlag(
                        settings.getName(),
                        true /* prompt is blanket mode was shown */);
                }
                // do the refresh
                iContainer->RefreshListBoxContentL(
                    listBoxContentCurrentItemIndex, selectionIndex);
            }
            break;
        case ON_SCREEN_KEYPAD_PAGE:
            err = iSettingsHandler->SetOnScreenKeypadValueL(
                      iContainer->GetListBoxItem(listBoxContentCurrentItemIndex)
                      .getValue().getId());
            if (err == KErrNone)
            {
                iContainer->RefreshListBoxContentL(
                    listBoxContentCurrentItemIndex,  selectionIndex);
            }
            break;
        }
        if (err != KErrNone)
        {
            // roll back to the original selection for the value
            iContainer->SetListBoxItemValue(
                listBoxContentCurrentItemIndex, currentSelection);
        }
    }
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::DoDeactivate()
// Deactivate this view
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsView::DoDeactivate()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::DoDeactivate ");
    if (iContainer)
    {
        iContainer->MakeVisible(EFalse);
    }
    if (iResourceFileOffset > 0)
    {
        iResourceHandler.DeleteResourceFile(iResourceFileOffset);
        iResourceFileOffset = 0;
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::DoDeactivate ");
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::ShowWarningSecDlgL()
// high risk (net acces, messaging, call control, local connectivity) + (multimedia, read user data)
// ---------------------------------------------------------
//
bool CAppMngr2MidletSettingsView::ShowWarningSecDlgL(bool networkUsage)
{
    HBufC* header = StringLoader::LoadLC(R_JAVA_SEC_HEADER);
    HBufC* warning_sec = NULL;
    if (networkUsage)
    {
        warning_sec = StringLoader::LoadLC(R_JAVA_QUERY_MIDLET_PERM_NET);
    }
    else
    {
        warning_sec = StringLoader::LoadLC(R_JAVA_QUERY_MIDLET_PERM_SEC);
    }
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*warning_sec);
    CleanupStack::PushL(dlg);
    dlg->PrepareLC(R_APPMNGR_QUERY_DIALOG_OK_CANCEL);
    dlg->QueryHeading()->SetTextL(*header);
    dlg->QueryHeading()->SetLayout(CAknPopupHeadingPane::EQueryHeadingPane);
    CleanupStack::Pop(dlg);
    bool ret = (dlg->RunLD() == EAknSoftkeyOk);
    CleanupStack::PopAndDestroy(warning_sec);
    CleanupStack::PopAndDestroy(header);
    return ret;
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsView::ShowWarningExclDlgL()
// mutually exclusive auto invocation + net access
// ---------------------------------------------------------
//
bool CAppMngr2MidletSettingsView::ShowWarningExclDlgL()
{
    HBufC* header = StringLoader::LoadLC(R_JAVA_SEC_HEADER);
    HBufC* warning_excl = StringLoader::LoadLC(R_JAVA_QUERY_MIDLET_PERM_EXCL);
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*warning_excl);
    CleanupStack::PushL(dlg);
    dlg->PrepareLC(R_APPMNGR_QUERY_DIALOG_OK_CANCEL);
    dlg->QueryHeading()->SetTextL(*header);
    dlg->QueryHeading()->SetLayout(CAknPopupHeadingPane::EQueryHeadingPane);
    CleanupStack::Pop(dlg);
    bool ret = (dlg->RunLD() == EAknSoftkeyOk);
    CleanupStack::PopAndDestroy(warning_excl);
    CleanupStack::PopAndDestroy(header);
    return ret;
}

/**
 */
void CAppMngr2MidletSettingsView::InitSettings(const std::vector<MidletSuiteSecuritySettings>& aAllSettings)
{
    iBlanketSettings = new bool[SENSIBLE_FUNCTION_GROUPS];
    for (int i=0; i<SENSIBLE_FUNCTION_GROUPS; i++)
    {
        iBlanketSettings[i] = false;
    }
    for (int i=0; i<aAllSettings.size(); i++)
    {
        if (aAllSettings[i].getName() == NET_ACCESS_SETTINGS)
        {
            iMutuallyExclusiveSettings.push_back(aAllSettings[i]);
            iBlanketSettings[NET_ACCESS_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == LOW_LEVEL_NET_ACCESS_SETTINGS)
        {
            iBlanketSettings[LOW_LEVEL_NET_ACCESS_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == MESSAGING_SETTINGS)
        {
            iBlanketSettings[MESSAGING_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == RESTRICTED_MESSAGING_SETTINGS)
        {
            iBlanketSettings[RESTRICTED_MESSAGING_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == CALL_CONTROL_SETTINGS)
        {
            iBlanketSettings[CALL_CONTROL_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == LOCAL_CONNECTIVITY_SETTINGS)
        {
            iBlanketSettings[LOCAL_CONNECTIVITY_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == MULTIMEDIA_RECORDING_SETTINGS)
        {
            iBlanketSettings[MULTIMEDIA_RECORDING_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == READ_USER_DATA_ACCESS_SETTINGS)
        {
            iBlanketSettings[READ_USER_DATA_ACCESS_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
        else if (aAllSettings[i].getName() == APPLICATION_AUTO_INVOCATION_SETTINGS)
        {
            iMutuallyExclusiveSettings.push_back(aAllSettings[i]);
            iBlanketSettings[APPLICATION_AUTO_INVOCATION_SETTINGS_INDEX] =
                (aAllSettings[i].getCurrentInteractionMode() == BLANKET_INTERACTION_MODE);
        }
    }
}

void CAppMngr2MidletSettingsView::UpdateBlanketSettings(const MidletSuiteSecuritySettings& aSettings, bool aBlanket)
{
    if (aSettings.getName() == NET_ACCESS_SETTINGS)
    {
        iBlanketSettings[NET_ACCESS_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == LOW_LEVEL_NET_ACCESS_SETTINGS)
    {
        iBlanketSettings[LOW_LEVEL_NET_ACCESS_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == MESSAGING_SETTINGS)
    {
        iBlanketSettings[MESSAGING_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == RESTRICTED_MESSAGING_SETTINGS)
    {
        iBlanketSettings[RESTRICTED_MESSAGING_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == CALL_CONTROL_SETTINGS)
    {
        iBlanketSettings[CALL_CONTROL_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == LOCAL_CONNECTIVITY_SETTINGS)
    {
        iBlanketSettings[LOCAL_CONNECTIVITY_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == MULTIMEDIA_RECORDING_SETTINGS)
    {
        iBlanketSettings[MULTIMEDIA_RECORDING_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == READ_USER_DATA_ACCESS_SETTINGS)
    {
        iBlanketSettings[READ_USER_DATA_ACCESS_SETTINGS_INDEX] = aBlanket;
    }
    else if (aSettings.getName() == APPLICATION_AUTO_INVOCATION_SETTINGS)
    {
        iBlanketSettings[APPLICATION_AUTO_INVOCATION_SETTINGS_INDEX] = aBlanket;
    }
}

bool CAppMngr2MidletSettingsView::CheckBlanketSettingsL(const MidletSuiteSecuritySettings& aSettings)
{
    if (aSettings.getCurrentInteractionMode() != BLANKET_INTERACTION_MODE)
    {
        UpdateBlanketSettings(aSettings, false);
        return true;
    }
    if ((aSettings.getName() == NET_ACCESS_SETTINGS
            && iBlanketSettings[APPLICATION_AUTO_INVOCATION_SETTINGS_INDEX])
            || (aSettings.getName() == APPLICATION_AUTO_INVOCATION_SETTINGS
                && iBlanketSettings[NET_ACCESS_SETTINGS_INDEX]))
    {
        bool accept = ShowWarningExclDlgL();
        if (accept)
        {
            UpdateBlanketSettings(aSettings, true);
            // change the other setting to the next maximum allowed
            HandleMutuallyExclusiveSettingsL(aSettings);
        }
        return accept;
    }
    if (((aSettings.getName() == NET_ACCESS_SETTINGS
            || aSettings.getName() == LOW_LEVEL_NET_ACCESS_SETTINGS
            || aSettings.getName() == MESSAGING_SETTINGS
            || aSettings.getName() == RESTRICTED_MESSAGING_SETTINGS
            || aSettings.getName() == CALL_CONTROL_SETTINGS
            || aSettings.getName() == LOCAL_CONNECTIVITY_SETTINGS)
            && (iBlanketSettings[MULTIMEDIA_RECORDING_SETTINGS_INDEX]
                || iBlanketSettings[READ_USER_DATA_ACCESS_SETTINGS_INDEX]))
            || ((aSettings.getName() == MULTIMEDIA_RECORDING_SETTINGS
                 || aSettings.getName() == READ_USER_DATA_ACCESS_SETTINGS)
                && (iBlanketSettings[NET_ACCESS_SETTINGS_INDEX]
                    || iBlanketSettings[LOW_LEVEL_NET_ACCESS_SETTINGS_INDEX]
                    || iBlanketSettings[MESSAGING_SETTINGS_INDEX]
                    || iBlanketSettings[RESTRICTED_MESSAGING_SETTINGS_INDEX]
                    || iBlanketSettings[CALL_CONTROL_SETTINGS_INDEX]
                    || iBlanketSettings[LOCAL_CONNECTIVITY_SETTINGS_INDEX])))
    {
        bool accept = ShowWarningSecDlgL(
                          (aSettings.getName() != LOCAL_CONNECTIVITY_SETTINGS
                           && !iBlanketSettings[LOCAL_CONNECTIVITY_SETTINGS_INDEX]));
        if (accept)
        {
            UpdateBlanketSettings(aSettings, true);
        }
        return accept;
    }
    else if (aSettings.getCurrentInteractionMode() == BLANKET_INTERACTION_MODE)
    {
        UpdateBlanketSettings(aSettings, true);
    }
    return true;
}

void CAppMngr2MidletSettingsView::InitL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsView::InitL ");

    // init the settings handler
    iSettingsHandler = CAppMngr2MidletSettingsHandler::NewL(aMidletSuiteName,aMidletSuiteUid, iResourceHandler);
    // Show on-screen Keypad settings only in devices having touch display
    iOnscreenKeypadSettingsVisible = AknLayoutUtils::PenEnabled();
    // Construct the container and make it visible
    iContainer = new(ELeave) CAppMngr2MidletSettingsContainer(*this);
    // Set the MObjectProvider parent for the container window
    iContainer->SetMopParent(this);
    iContainer->ConstructL(ClientRect());
    HBufC* jamSetTitle = StringLoader::LoadLC(R_JAVA_SET_TITLE, aMidletSuiteName);
    CEikStatusPane* statusPane = StatusPane();
    CAknTitlePane* titlePane;
    titlePane = (CAknTitlePane*) statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle));
    titlePane->SetTextL(jamSetTitle->Des());
    CleanupStack::PopAndDestroy(jamSetTitle);
    // fill in the list items
    std::vector<ListItem> listBoxItems;
    // access point
    CAppMngr2SuiteSnapItem* snap = iSettingsHandler->GetSnapL();
    CleanupStack::PushL(snap);
    listBoxItems.push_back(AppMngr2MidletSettingsUtil::SnapToListItem(
                               *snap, true /* enabled */));
    CleanupStack::PopAndDestroy(snap);
    // on screen Keypad
    wstring onScreenKeypad = iSettingsHandler->GetOnScreenKeypadValueL();
    if (iSettingsHandler->OnScreenKeypadValuePreDefinedL())
    {
        iOnscreenKeypadSettingsVisible = EFalse;
    }
    listBoxItems.push_back(AppMngr2MidletSettingsUtil::OnScreenKeypadToListItem(
                               onScreenKeypad, iOnscreenKeypadSettingsVisible));
    // security warnings mode
    vector<MidletSuiteSecuritySettings> settings;
    iSettingsHandler->GetSecuritySettings(settings);
    iSecuritySettingsVisible = (settings.size() > 0);
    wstring securityWarningsMode = iSettingsHandler->GetSecurityWarningsMode();
    listBoxItems.push_back(AppMngr2MidletSettingsUtil::SecurityWarningsModeToListItem(
                               securityWarningsMode, iSecuritySettingsVisible));
    // blanket settings
    InitSettings(settings);
    // function groups
    for (int i=0; i<settings.size(); i++)
    {
        listBoxItems.push_back(AppMngr2MidletSettingsUtil::SettingsToListItem(
                                   settings[i],
                                   (iSecuritySettingsVisible && securityWarningsMode == SECURITY_WARNINGS_USER_DEFINED_MODE)));
    }
    // sort the vector
    std::sort(listBoxItems.begin(), listBoxItems.end(),
              AppMngr2MidletSettingsUtil::AscendingListItemSort());
    iContainer->InitListBoxContentL(listBoxItems);
    AppUi()->AddToStackL(*this, iContainer);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsView::InitL ");
}

void CAppMngr2MidletSettingsView::HandleMutuallyExclusiveSettingsL(const MidletSuiteSecuritySettings& aSettings)
{
    for (int i=0; i<iMutuallyExclusiveSettings.size(); i++)
    {
        if (iMutuallyExclusiveSettings[i].getName() != aSettings.getName())
        {
            TInt allowedInteractionModes = JavaCommonUtils::wstringToInt(
                                               iMutuallyExclusiveSettings[i].getAllowedInteractionModes());
            wstring currentInteractionMode = iMutuallyExclusiveSettings[i]
                                             .getCurrentInteractionMode();
            if ((allowedInteractionModes & INTERACTION_MODE_SESSION) > 0)
            {
                currentInteractionMode = SESSION_INTERACTION_MODE;
            }
            else if ((allowedInteractionModes & INTERACTION_MODE_ONESHOT) > 0)
            {
                currentInteractionMode = ONESHOT_INTERACTION_MODE;
            }
            else if ((allowedInteractionModes & INTERACTION_MODE_DENIED) > 0)
            {
                currentInteractionMode = DENIED_INTERACTION_MODE;
            }
            int err = iSettingsHandler->SetSecuritySettings(
                          iMutuallyExclusiveSettings[i].getName(), currentInteractionMode);
            if (err == KErrNone)
            {
                UpdateBlanketSettings(iMutuallyExclusiveSettings[i], false);
                // do the refresh
                ListItem listItem1 = AppMngr2MidletSettingsUtil::SettingsToListItem(
                                         iMutuallyExclusiveSettings[i], true);
                ListItem listItem2(listItem1);
                std::vector<LocalizedString> values = listItem1.getValues();
                for (int j=0; j<values.size(); j++)
                {
                    if (values[j].getId() == currentInteractionMode)
                    {
                        listItem2.setCurrentValue(j);
                        break;
                    }
                }
                iContainer->RefreshListBoxContentL(listItem1, listItem2);
            }
        }
    }
}

// End of File

