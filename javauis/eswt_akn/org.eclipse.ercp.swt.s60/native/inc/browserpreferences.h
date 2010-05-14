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


#ifndef BROWSERPREFERENCES_H
#define BROWSERPREFERENCES_H


#include "eswtexpanded.hrh"


class CSwtBrowserVpnApItem;
class CSwtBrowserApItem;

const TUint KWmlNoDefaultAccessPoint = KMaxTUint;


/**
 * Struct for MPreferencesObserver
 */
struct TPreferencesValues
{
    // Setting Values
    TUint                           iDefaultAccessPoint;
    CSwtBrowserApItem*              iDefaultAPDetails;  //cache all data
    TUint                           iAssocVpn;
    TBool                           iAccessPointSelectionMode;
    TBool                           iAutoLoadImages;
    TUint                           iFontSize;
    TBool                           iTextWrap;
    TBool                           iCookies;
    TBool                           iPageOverview;
    TBool                           iBackList;
    TBool                           iAutoRefresh;
    TBool                           iEcma;
    TBool                           iIMEINotification;
    TUint32                         iEncoding;
    TBool                           iFullScreen;
    TBool                           iQueryOnExit;
    TBool                           iSendReferrer;
    TUint                           iHomePgType;
    TBool                           iHTTPSecuritySupressed;
    TBool                           iConnDialogs;
    TBool                           iHttpSecurityWarnings;
    TUint                           iMediaVolume;
    HBufC*                          iSearchPgURL;
    HBufC*                          iHomePgURL;
    TBool                           iPopupBlocking;
    TUint                           iFormDataSaving;
    TUint                           iAutomaticUpdating;
    TUint                           iAutomaticUpdatingAP;
    TUint                           iZoomMin;
    TUint                           iZoomMax;
    TUint                           iZoomDef;
};


/**
 * Enum for MPreferencesObserver
 */
enum TPreferencesEvent
{
    EPreferencesActivate,   // Preferences opened
    EPreferencesDeactivate, // Preferences closed
    EPreferencesItemChange  // Setting change has occurred
};


/**
 * MPreferencesObserver
 * Observer for the MPreferences Class.
 * @lib eswt
 */
class MPreferencesObserver
{
public:

    /**
     * From MPreferencesObserver
     * Observer for Preference changes
     * @param aEvent The type of preference event which happened
     * @param aValues A struct of all settings values
     * @return void
     */
    virtual void HandlePreferencesChangeL(const TPreferencesEvent aEvent,
                                          TPreferencesValues& aValues, TBrCtlDefs::TBrCtlSettings aSettingType) = 0;
};


/**
 * MBrowserPreferences
 * Interface for Preferences handling.
 * @lib eswt
 */
class MBrowserPreferences
{
public: // New functions

    /**
     * Returns a structure containing all of the settings according to the
     * central repository values
     * @return Returns references of TPreferencesValues which contains all preferences
     */
    virtual const TPreferencesValues& AllPreferences() const = 0;

    /**
     * To access DefaultAccessPoint setting
     * @return UID for default accesspoint
     */
    virtual TUint DefaultAccessPoint() const = 0;

    /**
     * To access vpn item if exists.
     * @param aDefaultAccessPoint UID for the accesspoint
     * @param aVpnItem Vpn Item
     * @return ETrue when vpn item is valid, EFalse otherwise.
     */
    virtual TBool VpnDataL(TUint aDefaultAccessPoint,
                           CSwtBrowserVpnApItem& aVpnItem) = 0;

    /**
     * To access HTTP security warnings setting
     * @return ETrue if security warnings are enabled
     */
    virtual TBool HttpSecurityWarnings() const = 0;

    /**
     * To check whether the CDMA UI local feature ("R-BrUI2") is
     * supported or not.
     * @param aFeature The feature to check.
     * @return ETrue when the feature is supported, EFalse otherwise.
     */
    virtual TBool CdmaUiLocalFeatureSupported(const TInt aFeature) const  = 0;

    /**
     * To access ShowConnectionDialogs setting
     * @return ETrue if the dialog is confirmed, otherwise EFalse..
     */
    virtual TBool ShowConnectionDialogs() const = 0;

    /**
     * To check that a ui local feature is supported or not
     * @param aFeature Feature to check
     * @return Returns ETrue if feature is supported
     */
    virtual TBool UiLocalFeatureSupported(const TInt aFeature) const  = 0;

    /**
     * To modify DefaultAccessPoint setting.
     * @param aDefaultAccessPoint Id of default accesspoint/
     * @param aAssocVpn Associated Vpn.
     * @return void.
     */
    virtual void SetDefaultAccessPointL(TUint aDefaultAccessPoint,
                                        TUint aAssocVpn = KWmlNoDefaultAccessPoint) = 0;

    /**
     * To modify FontSize setting
     * @param aFontSize Set new font size
     * @return void.
     */
    virtual void SetFontSizeL(TInt aFontSize) = 0;

    /**
    * To modify Encoding setting
    * @param aEncoding The encoding to set.
    * @return void.
    */
    virtual void SetEncodingL(TUint32 aEncoding) = 0;

    /**
     * To access to the list of self download content types.
     * See CBrCtlInterface::SetSelfDownloadContentTypesL()!
     * @return Returns the pointer to the string which indicates download content types.
     */
    virtual TPtrC SelfDownloadContentTypesL() = 0;

    /**
     * To access Default Access Point Always Ask settings.
     * @return Returns ETrue when always ask mode is on, otherwise EFalse.
     */
    virtual TBool AccessPointSelectionMode() = 0;

    /**
     * To modify Default Access Point Settings
     * @param aAccessPointSelectionMode Access point selection mode.
     * @return void.
     */
    virtual void SetAccessPointSelectionModeL(
        TWmlSettingsAccessPointSelectionMode aAccessPointSelectionMode) = 0;

    /**
     * To access AutoLoadImages setting.
     * @return Value of setting
     */
    virtual TBool AutoLoadImages() const = 0;

    /**
     * To access BackList setting.
     * @return Value of setting
     */
    virtual TBool BackList() const = 0;

    /**
     * To access AutoRefresh setting.
     * @return Value of setting
     */
    virtual TBool AutoRefresh() const = 0;

    /**
     * To access Cookies setting
     * @return TWmlSettingsCookies
     */
    virtual TBool Cookies() const = 0;

    /**
     * To access ECMA setting
     * @return TWmlSettingsECMA
     */
    virtual TBool Ecma() const = 0;

    /**
     * Get IMEI notification setting
     * @return EWmlSettingsIMEIEnable if notification is enabled,
     * otherwise EWmlSettingsIMEIDisable
     */
    virtual TBool IMEINotification() const = 0;

    /**
     * Sends/Don not send the referrer header
     * 0 = Do not send the referrer header
     * 1 = Send the referrer header
     * @return ETrue if Send referrer header is enabled
     */
    virtual TBool SendReferrer() const = 0;

    /**
     * To access PageOverview setting.
     * @return Value of setting
     */
    virtual TBool PageOverview() const = 0;

    /**
     * To access Form Data Saving setting.
     * @return value of setting
     */
    virtual TUint FormDataSaving() const = 0;

    /**
     * Adds an observer to be notified about changes. Derived classes MUST
     * notify all observers when preferences changed!
     * @param anObserver Observer to be notified about events.
     * @return void.
     */
    virtual void AddObserverL(MPreferencesObserver* anObserver) = 0;

    /**
     * Removes an observer.
     * @param anObserver The observer.
     * @return void.
     */
    virtual void RemoveObserver(MPreferencesObserver* anObserver) = 0;

    /**
     * Notifies observer.
     * @param TPreferencesEvent The preference event.
     * @param aSettingType the setting type.
     * @return void.
     */
    virtual void NotifyObserversL(TPreferencesEvent aEvent,
                                  TBrCtlDefs::TBrCtlSettings aSettingType) = 0;
};

#endif // BROWSERPREFERENCES_H
