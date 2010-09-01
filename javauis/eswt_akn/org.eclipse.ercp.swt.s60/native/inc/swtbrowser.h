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


#ifndef SWTBROWSER_H
#define SWTBROWSER_H


#include <brctlinterface.h>
#include <favouritessession.h>
#include "eswtwidgetsexpanded.h"
#include "swtcomposite.h"
#include "browserapiprovider.h"
#include "browserpreferences.h"


class CSwtBrowserContainer;
class CSwtBrowserLoadEventObserver;
class CSwtBrowserLinkResolver;
class CSwtBrowserStateChangeObserver;
class CSwtBrowserSpecialLoadObserver;
class CSwtBrowserDialogsProviderProxy;
class CSwtBrowserSoftkeysObserver;
class CSwtBrowserPreferences;
class MBrowserConnection;
class CSwtBrowserActiveApDb;


/**
 * CSwtBrowser
 * Core implementation of eSwt Browser
 * Create and Initialize Browser control.
 * Implements MSwtBrowser and MBrowserApiProvider.
 * Handle connection state change and preference change
 * @lib eswt
 */
class CSwtBrowser
        : public CSwtComposite
        , public MSwtBrowser
        , public MBrowserApiProvider
        , public MPreferencesObserver
        , public MEikCommandObserver
        , public MWidgetCallback
{
protected:
    enum TCbaState
    {
        // CBA key scan codes
        EDefault = 0,
        EActivatedInputBox
    };

public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aDisplay The Display to associate with
     * @param aPeer The peer to its java counterpart
     * @param aParent Parent composite
     * @param aStyle The style
     * @return CSwtBrowser*  A pointer to the  created CSwtBroser object
     */
    static CSwtBrowser* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite& aParent, TInt aStyle);

    /**
     * Update  CBA according to its current state.
     */
    void UpdateCbaL();

    /**
     * Handle and post pointer event forwarded by CSwtBrowserContainer.
     * @param aPointerEvent The pointer event.
     */
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif // RD_SCALABLE_UI_V2


// From MBrowserApiProvider
    CBrCtlInterface& BrCtlInterface() const;
    MBrowserPreferences& Preferences() const;
    CSwtBrowserActiveApDb& ActiveApDb() const;
    CSwtBrowserLoadEventObserver& LoadObserver() const;
    CSwtBrowserSpecialLoadObserver& SpecialLoadObserver() const;
    virtual MSwtDisplay& Display() const;
    TBool StartPreferencesViewL();
    TBool IsConnecting() const;
    void SetRequestedAP(TInt aAp);
    TBool StartedUp() const;
    TUint32 RequestedAp() const;
    MBrowserConnection& Connection() const;
    TBool Fetching() const;
    void CancelFetchL(TBool aIsUserInitiated = EFalse);
    void FetchL(const TDesC& aUrl, CSwtBrowserLoadEventObserver::TBrowserLoadUrlType aUrlType
                = CSwtBrowserLoadEventObserver::ELoadUrlTypeOther);
    void FetchL(const TDesC& aUrl, const TDesC& aUserName, const TDesC& aPassword,
                const TFavouritesWapAp& aAccessPoint,
                CSwtBrowserLoadEventObserver::TBrowserLoadUrlType aUrlType
                = CSwtBrowserLoadEventObserver::ELoadUrlTypeOther);
    void FetchSavedDeckL(TInt aUid);

// From MPreferencesObserver
    void HandlePreferencesChangeL(const TPreferencesEvent aEvent,
                                  TPreferencesValues& aValues, TBrCtlDefs::TBrCtlSettings aSettingType);

// From MSwtBrowser
    TBool  BackL();
    TBool  ForwardL();
    HBufC* GetUrlL();
    TBool  IsBackEnabled() const;
    TBool  IsForwardEnabled() const;
    void   RefreshL();
    void   StopL();
    TBool  SetTextL(const TDesC&  aHtml);
    TBool  SetUrlL(const TDesC&  aUrl);
    void   DoSetUrlL(const TDesC&  aUrl, TSwtBrCallBackOperationType aOperationType) ;

// From MWidgetCallback
    TBool DialogMimeFileSelectLC(HBufC*& /*aSelectedFileName*/, const TDesC& /*aMimeType*/)
    {
        return EFalse;
    };
    void SetSoftkeysVisible(TBool /*aVisible*/)
    {
        return;
    };
    void SetDisplayMode(TBrCtlDefs::TBrCtlOrientation /*aOrientation*/)
    {
        return;
    };
    TInt Reserved_1(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_2(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_3(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_4(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_5(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_6(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_7(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_8(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_9(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }
    TInt Reserved_10(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/)
    {
        return 0;
    }

protected:
    void HandleFreeRamEventL(TInt aEventType);

// From ASwtControlBase
protected:
    void DoDelete();

// From MSwtControl
public:
    TSwtPeer Dispose();
    HBufC* MSKLabelL() const;
    TBool IsKeyUsed(TUint aKeyCode) const;
    TBool CbaButtonIsUsed(TInt aCbaButtonIndex) const;
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;

protected:
    MSwtBrowser* BrowserInterface() const;

// From MSwtComposite
public:
    MSwtComposite* Composite();
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);

protected:
    /**
     * Constructor.
     * @param aDisplay The Display to associate with
     * @param aPeer The peer to its java counterpart
     * @param aParent Parent composite
     * @param aStyle The style
     * @param aVisibility The visibility of this control
     * @param aDimmed The dimmed attribute of the control
     */
    CSwtBrowser(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite* aParent,
                TInt aStyle,TBool aVisibility, TBool aDimmed);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowser();

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Separated second phase constructor for CBrCtlInterface
     * @param aVisibility The visibility of this control
     */
    void ConstructBrowserL(TBool aVisibility);

private:
    /**
     * Calculate browser rect.
     * @return the client rect of the browser control in its container
     */
    TRect CalcBrowserRect();

    /**
     * Creating a full url containing username, password,
     * server domain and document path.
     * @return buffer that points to the url
     */
    HBufC* CreateFullUrlLC(const TDesC& aUrl, const TDesC& aUsername,
                           const TDesC& aPassword);

    /**
     * Check validity of AccessPoint.
     * @param aIAPid AccessPoint Id.
     * @return ETrue if AccessPoint Id is valid and EFalse otherwise.
     */
    TBool IsValidAPFromParamL(TUint32 aIAPid);

    /**
     * Checks for any changes in Local Preferences and notifies Browser.
     * @param aValues a struct of all settings values
     */
    void UpdateLocalPreferencesL(TPreferencesValues& aValues);

    /**
     * Checks for any changes in Global Prefeernces and notifies Browser.
     * @param aSettingType an enum defining a setting
     */
    void UpdateGlobalPreferencesL(TBrCtlDefs::TBrCtlSettings aSettingType);

// From CCoeControl
    void SizeChanged();
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);

public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

protected:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

// From MSwtControl
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public:
// From MEikCommandObserver
    void ProcessCommandL(TInt aCmdId);

private: //Data
    /**
     * This variable cantinas full flags. iStyle variable
     * which is declared in supper class exclude scroll flags.
     */
    const TInt iBrowserLevelStyle;

    /**
     *  Flag to indicate whether FeatureManager has be Initialized
     */
    TBool iFeatureManager;

    /**
     *  Font size.
     */
    TInt    iFontSize;

    /**
     *  Character encoding
     */
    TUint32 iEncoding;

    /**
     *  Text wrap
     */
    TBool   iTextWrap;

    /**
     * Indicates if browser is ready to start
     */
    TBool iStartedUp;

    /**
     *  Requested Ap
     */
    TUint32 iRequestedAp;

    /**
     *  Fovourites session
     */
    RFavouritesSession iFavouritesSess;

    /**
     *  Current CBA state.
     */
    TCbaState iCbaState;

    /**
     *  Indicates if a selection key (Ok, Enter ) is pressed.
     */
    TBool iSelectionKeyPressed;

    /**
     * Container for the CBrCtl
     * Own
     */
    CSwtBrowserContainer* iContainer;

    /**
     * Pointer to the browser control interface
     * Own
     */
    CBrCtlInterface* iBrCtlInterface;

    /**
     * Load Event Observer
     * Own
     */
    CSwtBrowserLoadEventObserver* iLoadEventObserver;

    /**
     * Link Resolver
     * Own
     */
    CSwtBrowserLinkResolver* iLinkResolver;

    /**
     * State Change Observer
     * Own
     */
    CSwtBrowserStateChangeObserver* iStateChangeObserver;

    /**
     * Special load observer
     * Own
     */
    CSwtBrowserSpecialLoadObserver* iSpecialLoadObserver;

    /**
     * Dialogs provider proxy
     * Own
     */
    CSwtBrowserDialogsProviderProxy* iDialogsProviderProxy;

    /**
     * Softkeys observer.
     * Own
     */
    CSwtBrowserSoftkeysObserver* iSoftkeysObserver;

    /**
     * Active ap database
     * Own
     */
    CSwtBrowserActiveApDb* iActiveApDb;

    /**
     * Preferences
     * Own
     */
    CSwtBrowserPreferences* iPreferences;

    /**
     * Internet connection
     * Own
     */
    MBrowserConnection* iConnection;

    /**
     * Set to true while handling pointer events.
     */
    TBool iHandlingPointerEv;
};


#endif // SWTBROWSER_H
