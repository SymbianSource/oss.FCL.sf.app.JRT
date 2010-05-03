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


#ifndef SWTBROWSERPREFERENCES_H
#define SWTBROWSERPREFERENCES_H


#include <e32base.h>
#include <brctldefs.h>
#include <centralrepository.h>
#include "browserpreferences.h"


class CSwtBrowserActiveApDb;
class MBrowserApiProvider;
class CRepository;
class TBrowserOverriddenSettings;
class CSwtBrowserApDataHandler;
class CSwtBrowserVpnApItem;
class CSwtBrowserVpnApEngine;


/**
 * CSwtBrowserPreferences
 * Stores Browser settings.
 * Initial settings is obtained from repository for browser
 * @lib eswt
 */
class CSwtBrowserPreferences
        : public CBase
        , public MBrowserPreferences
{

public:

    /**
     * Two-phased constructor.
     *
     * @param aActiveApDb The active ap database
     * @param aApiProvider Api provider.
     * @return The constructed preferences object
     */
    static CSwtBrowserPreferences* NewLC(CSwtBrowserActiveApDb& aActiveApDb,
                                         MBrowserApiProvider& aApiProvider);

    /**
     * Two-phased constructor.
     *
     * @param aActiveApDb The active ap database
     * @param aApiProvider Api provider.
     * @return The constructed preferences object.
     */
    static CSwtBrowserPreferences* NewL(CSwtBrowserActiveApDb& aActiveApDb,
                                        MBrowserApiProvider& aApiProvider);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserPreferences();

// From MActiveApDbObserver

// From MBrowserPreferences
    const TPreferencesValues& AllPreferencesL();
    void AddObserverL(MPreferencesObserver* anObserver);
    void RemoveObserver(MPreferencesObserver* anObserver);
    void NotifyObserversL(TPreferencesEvent aEvent,
                          TBrCtlDefs::TBrCtlSettings aSettingType);
    TUint DefaultAccessPoint() const;
    TBool VpnDataL(TUint aDefaultAccessPoint, CSwtBrowserVpnApItem& aVpnItem);
    TBool HttpSecurityWarnings() const;
    TBool CdmaUiLocalFeatureSupported(const TInt aFeature) const;
    TBool ShowConnectionDialogs() const;
    TBool UiLocalFeatureSupported(const TInt aFeature) const;
    void SetDefaultAccessPointL(TUint aDefaultAccessPoint,
                                TUint aAssocVpn = KWmlNoDefaultAccessPoint);
    void SetFontSizeL(TInt aFontSize);
    void SetEncodingL(TUint32 aEncoding);
    void SetHomePageTypeL(TWmlSettingsHomePage aHomePageType);
    TPtrC SelfDownloadContentTypesL();
    TBool AccessPointSelectionMode();
    void SetAccessPointSelectionModeL(
        TWmlSettingsAccessPointSelectionMode aAccessPointSelectionMode);
    TBool AutoLoadImages() const;
    TBool BackList() const;
    TBool AutoRefresh() const;
    TWmlSettingsCookies Cookies() const;
    TWmlSettingsECMA Ecma() const;
    TWmlSettingsIMEI IMEINotification() const;
    TBool SendReferrer() const;
    TBool PageOverview() const;
    virtual TWmlSettingsFormData FormDataSaving() const;

private:
    /**
     * Constructor.
     * @param aCommsModel Comms model.
     */
    CSwtBrowserPreferences(CSwtBrowserActiveApDb& aActiveApDb,
                           MBrowserApiProvider& aApiProvider);

    /**
     * Constructor.
     */
    void ConstructL();

    /**
     * restores preferences from shared data db. If fails, it uses defaults.
     */
    void RestoreSettingsL();

    /*
     * Get a value from repository
     *
     * @param aKey The key for value
     * @return positive value if successful
     */
    TInt GetIntValue(TUint32 aKey) const;

    /**
     * Enables access to browser api
     * @return Reference of Api provider.
     */
    MBrowserApiProvider& ApiProvider();

private: // Data
    /**
     *  Requested Ap
     */
    MBrowserApiProvider& iApiProvider;

    /**
     *  Custom Ap Id
     */
    TUint   iCustomAp;

    /**
     *  Observers
     */
    CArrayPtrFlat< MPreferencesObserver >* iObservers;

    /**
     *  Active ap database
     */
    CSwtBrowserActiveApDb& iActiveApDb;

    /**
     * Ap data handler
     */
    CSwtBrowserApDataHandler* iApDataHandler;

    /**
     * Vpn Item
     */
    CSwtBrowserVpnApItem* iVpnItem;

    /**
     * Vpn Engine.
     */
    CSwtBrowserVpnApEngine* iVpnEngine;

    /**
     * Repository for browser setttings.
     * !!! NEVER EVER OVERRIDE ANY DEVICE SETTINGS !!!
     * We had a clear use case where the user has been unable to use
     * the native browser because wrong access points have been set!
     */
    CRepository* iRepository;

    /**
     * Engine Local Features Bitmask
     */
    TInt iEngineLocalFeaturesBitmask;

    /**
     * Ui Local Features Bitmask
     */
    TInt iUiLocalFeaturesBitmask;

    /**
     * Overriden settings
     */
    TBrowserOverriddenSettings* iOverridenSettings;

    /**
     * ETrue = the browser is in embedded mode
     */
    TBool iEmbeddedMode;

    /**
     * CDMA UI Local Features Bitmask
     */
    TInt iCdmaUiLocalFeaturesBitmask;

    /**
     * Settings were overriden or not
     */
    TBool iOverriden;

    /**
     * List of such content types. Owned
     */
    HBufC* iSelfDownloadContentTypes;

    /**
     * A buffer struct for holding each setting value
     */
    TPreferencesValues iAllPreferences;
};

#endif // SWTBROWSERPREFERENCES_H
