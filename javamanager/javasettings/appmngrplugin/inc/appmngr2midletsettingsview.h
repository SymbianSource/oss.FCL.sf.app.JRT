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
* Description:  Declares view for application.
*
*/


#ifndef APPMNGR2MIDLETSETTINGSVIEW_H
#define APPMNGR2MIDLETSETTINGSVIEW_H

// INCLUDES
#include <aknview.h>                    // CAknView
#include <eiklbo.h>                     // MEikListBoxObserver
#include <vector>
#include <string>
#include "appmngr2midletsettingsutil.h"

// CONSTANTS
// MIDlet suite settings view ID
const TUid KMidletSuiteSettingsViewId = { 0x20016BF8 };

// FORWARD DECLARATIONS
class CAppMngr2MidletSettingsContainer;
class CAppMngr2MidletSettingsHandler;
class CAppMngr2SuiteSnapItem;
class CPKIXValidationResult;
class CAppMngr2MidletResourceHandler;

// CLASS DECLARATION

/**
 * MIDlet suite settings view class.
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
class CAppMngr2MidletSettingsView : public CAknView, public MEikListBoxObserver
{
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CAppMngr2MidletSettingsView* NewL(
        const TDesC& aMidletSuiteName,
        TUid aMidletSuiteUid,
        CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Destructor.
     */
    virtual ~CAppMngr2MidletSettingsView();

public: // Functions from base classes

    /**
     * From ?base_class ?member_description
     */
    TUid Id() const;

    /**
     * From ?base_class ?member_description
     */
    void HandleCommandL(TInt aCommand);

    /**
     * From ?base_class ?member_description
     */
    void HandleClientRectChange();


protected: //from MEikListBoxObserver

    /**
     * Handles the commands given with keys
     * @param aListbox is the listbox concerned.
     * @param aEventType is the corrent event that took place.
     */
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);


protected: //from MAknSettingPageObserver

private:
    /**
     * Constructor
     */
    CAppMngr2MidletSettingsView(CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Default constructor.
     */
    void ConstructL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid);

    /**
     * Initializas the container
     */
    void InitL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid);

    /**
     * From AknView, ?member_description
     */
    void DoActivateL(const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage);

    /**
     * From AknView, ?member_description
     */
    void DoDeactivate();

    /**
     * Handles settings by showing proper settings page/view
     * (access point page, on screen keypad page or security settings page)
     */
    void HandleSettingsL();

    /**
     * Shows the view for settings the internet access point
     */
    void ShowAccessPointPageL();

    /**
     * Shows the view for settings either the on screen
     * keypad value or for handling security settings, depending of the pageId
     */
    void ShowPageL(int aPageId);

    /**
     * Initializes all the security settings
     */
    void InitSettings(const std::vector<MidletSuiteSecuritySettings>& aAllSettings);

    /**
     * Updates the flag indicating if certain settings are set to Blanket
     */
    void UpdateBlanketSettings(const MidletSuiteSecuritySettings& aSettings, bool aBlanket);

    /**
     * Checks if certain settings obey the mutually exclusive rules
     */
    bool CheckBlanketSettingsL(const MidletSuiteSecuritySettings& aSettings);

    /**
     * Handles the mutually exclusive rules by showing notes to tge user
     */
    void HandleMutuallyExclusiveSettingsL(const MidletSuiteSecuritySettings& aSettings);

    /**
     * Show warning security dialog
     * Runs the dialog and returns a boolean indicating if the dialog was accepted
     *
     * @return a boolean indicating if the dialog was accepted
     */
    bool ShowWarningSecDlgL(bool networkUsage);

    /**
     * Show warning exclude dialog
     * Runs the dialog and returns a boolean indicating if the dialog was accepted
     *
     * @return a boolean indicating if the dialog was accepted
     */
    bool ShowWarningExclDlgL();

private: // Data

    /**
     * MIDlet suite settings container
     * Owned
     */
    CAppMngr2MidletSettingsContainer* iContainer;

    /**
     * MIDlet suite settings handler
     * Owned
     */
    CAppMngr2MidletSettingsHandler* iSettingsHandler;

    /**
     * Previous view ID
     */
    TVwsViewId iPrevViewId;

    /**
     * OnScreen Keypad settings vissibility
     */
    TBool iOnscreenKeypadSettingsVisible;

    /**
     * Security settings vissibility
     */
    TBool iSecuritySettingsVisible;

    /*
     * Vector containing the settings which are subject to
     * mutually exclusive rules
     */
    std::vector<MidletSuiteSecuritySettings> iMutuallyExclusiveSettings;

    /*
     * Vector containing flags indicating which settings (from the ones which
     * are subject to mutually exclusive rules) are set to Blanket
     */
    bool* iBlanketSettings;

    /**
     * Resource file offset
     */
    TInt iResourceFileOffset;
    CAppMngr2MidletResourceHandler& iResourceHandler;
};

#endif // APPMNGR2MIDLETSETTINGSVIEW_H

// End of File
