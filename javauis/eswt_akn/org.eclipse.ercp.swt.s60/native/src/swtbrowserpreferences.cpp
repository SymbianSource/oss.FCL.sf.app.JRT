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


#include <eikenv.h>
#include <browseruisdkcrkeys.h>
#include <swtbrowservpnapengine.h>
#include <swtbrowservpnapitem.h>
#include <swtbrowserapitem.h>
#include <swtbrowseractiveapdb.h>
#include <swtbrowserapdatahandler.h>
#include "swtlaffacade.h"
#include "browserapiprovider.h"
#include "swtbrowserpreferences.h"



// ======== MEMBER FUNCTIONS ========


CSwtBrowserPreferences::CSwtBrowserPreferences(CSwtBrowserActiveApDb& aActiveApDb,
        MBrowserApiProvider& aApiProvider)
        : iApiProvider(aApiProvider)
        , iActiveApDb(aActiveApDb)
        , iEmbeddedMode(EFalse)
{
    iAllPreferences.iCookies = EWmlSettingsCookieAllow;
    iAllPreferences.iHttpSecurityWarnings = ETrue;
    iAllPreferences.iConnDialogs = ETrue;
    iAllPreferences.iHomePgURL = NULL;
    iAllPreferences.iSearchPgURL = NULL;
    iAllPreferences.iQueryOnExit = EFalse;
    iAllPreferences.iSendReferrer = EFalse;
    iAllPreferences.iAssocVpn = KWmlNoDefaultAccessPoint;
    iAllPreferences.iDefaultAccessPoint = KWmlNoDefaultAccessPoint;
    iAllPreferences.iDefaultAPDetails = NULL;
}

void CSwtBrowserPreferences::ConstructL()
{
    // Create observer's list object
    // Make it possible for multi-browser sharing the same preference in the future
    iObservers = new(ELeave) CArrayPtrFlat< MPreferencesObserver >(1);

    // Create Ap data hander
    iApDataHandler = CSwtBrowserApDataHandler::NewL(iActiveApDb.CommsDb());

    // Create Vpn engine
    iVpnEngine = CSwtBrowserVpnApEngine::NewL(iActiveApDb.CommsDb());

    // Get settings from repository
    CRepository* repository = CRepository::NewL(KCRUidBrowserUiLV);
    if (repository->Get(CSwtLafFacade::GetUintConstant(
                            CSwtLafFacade::EBrowserCRCommonFlags),
                        iEngineLocalFeaturesBitmask) != KErrNone)
    {
        // in case of problems, assume everything is off
        iEngineLocalFeaturesBitmask = 0;
    }
    if (repository->Get(CSwtLafFacade::GetUintConstant(
                            CSwtLafFacade::EBrowserCRCommonFlags),
                        iUiLocalFeaturesBitmask) != KErrNone)
    {
        // in case of problems
        // disable multiple windows feature
        const TInt multipleWindows = CSwtLafFacade::GetConstant(
                                         CSwtLafFacade::EBrowserMultipleWindows);
        const TInt graphicalPage = CSwtLafFacade::GetConstant(
                                       CSwtLafFacade::EBrowserGraphicalPage);
        const TInt graphicalHistory = CSwtLafFacade::GetConstant(
                                          CSwtLafFacade::EBrowserGraphicalHistory);
        const TInt autoFormFill = CSwtLafFacade::GetConstant(
                                      CSwtLafFacade::EBrowserAutoFormFill);
        const TInt accessKeys = CSwtLafFacade::GetConstant(
                                    CSwtLafFacade::EBrowserAccessKeys);
        iUiLocalFeaturesBitmask &= ~multipleWindows;
        iUiLocalFeaturesBitmask |= graphicalPage | graphicalHistory
                                   | autoFormFill | accessKeys;
    }
    if (repository->Get(CSwtLafFacade::GetUintConstant(
                            CSwtLafFacade::EBrowserCRCdmaFlags),
                        iCdmaUiLocalFeaturesBitmask) != KErrNone)
    {
        // in case of problems, assume everything is off
        iCdmaUiLocalFeaturesBitmask = 0;
    }

    delete repository;


    // Instantiate repositorie

    iRepository = CRepository::NewL(KCRUidBrowser);

    iOverriden = EFalse;

    //Read ini values to local variables.
    TRAP_IGNORE(RestoreSettingsL());
}

CSwtBrowserPreferences* CSwtBrowserPreferences::NewLC
(CSwtBrowserActiveApDb& aActiveApDb, MBrowserApiProvider& aApiProvider)
{
    CSwtBrowserPreferences* result;
    result = new(ELeave) CSwtBrowserPreferences(aActiveApDb, aApiProvider);
    CleanupStack::PushL(result);
    result->ConstructL();
    return result;
}

CSwtBrowserPreferences* CSwtBrowserPreferences::NewL
(CSwtBrowserActiveApDb& aActiveApDb, MBrowserApiProvider& aApiProvider)
{
    CSwtBrowserPreferences* result;
    result = CSwtBrowserPreferences::NewLC(aActiveApDb, aApiProvider);
    CleanupStack::Pop(result);
    return result;
}

CSwtBrowserPreferences::~CSwtBrowserPreferences()
{
    delete iObservers;
    delete iApDataHandler;
    delete iVpnEngine;
    delete iVpnItem;
    delete iSelfDownloadContentTypes;
    iSelfDownloadContentTypes = 0;

    delete iAllPreferences.iSearchPgURL;
    iAllPreferences.iSearchPgURL = NULL;
    delete iAllPreferences.iHomePgURL;
    iAllPreferences.iHomePgURL = NULL;
    delete iAllPreferences.iDefaultAPDetails;
    delete iRepository;
}

void CSwtBrowserPreferences::RestoreSettingsL()
{
    iAllPreferences.iAutoLoadImages = GetIntValue(KBrowserNGImagesEnabled);
    iAllPreferences.iFontSize = GetIntValue(KBrowserNGFontSize);
    iAllPreferences.iCookies = GetIntValue(KBrowserNGCookiesEnabled);
    iAllPreferences.iEcma = GetIntValue(KBrowserNGECMAScriptSupport);

    TInt encoding;
    iRepository->Get(KBrowserNGEncoding, encoding);
    iAllPreferences.iEncoding = (TUint32)encoding;

    iAllPreferences.iPageOverview = GetIntValue(KBrowserNGPageOverview);
    iAllPreferences.iBackList = GetIntValue(KBrowserNGBackList);
    iAllPreferences.iAutoRefresh = GetIntValue(KBrowserNGAutoRefresh);
    iAllPreferences.iIMEINotification = GetIntValue(KBrowserIMEINotification);

    // Read suppress security UI setting
    iAllPreferences.iHTTPSecuritySupressed = GetIntValue(KBrowserSecurityUI);

    // Read show connection queries setting
    iAllPreferences.iConnDialogs = GetIntValue(CSwtLafFacade::GetUintConstant(
                                       CSwtLafFacade::EBrowserConnectionDialogs));

    if (!iAllPreferences.iHTTPSecuritySupressed)
    {
        // Read HTTP security warnings setting
        iAllPreferences.iHttpSecurityWarnings
        = GetIntValue(KBrowserNGShowSecurityWarnings);
    }
    else
    {
        // Don't want to see sec warning because they're supressed
        iAllPreferences.iHttpSecurityWarnings = EFalse;
    }

    // Pop up Blocking
    iAllPreferences.iPopupBlocking = GetIntValue(CSwtLafFacade::GetUintConstant(
                                         CSwtLafFacade::EBrowserNGPopupBlocking));

    // Form Data Saving
    iAllPreferences.iFormDataSaving = GetIntValue(
                                          CSwtLafFacade::GetUintConstant(CSwtLafFacade::EBrowserFormDataSaving));

    // Search Page, dont't want search page
    if (iAllPreferences.iSearchPgURL)
    {
        delete iAllPreferences.iSearchPgURL;
        iAllPreferences.iSearchPgURL = NULL;
    }

    // Web feeds settings
    iAllPreferences.iAutomaticUpdating = static_cast< TWmlSettingsAutomaticUpdating >
                                         (GetIntValue(KBrowserNGAutomaticUpdating));

    iAllPreferences.iAutomaticUpdatingAP = GetIntValue(
                                               KBrowserNGAutomaticUpdatingAccessPoint);

#ifdef RD_JAVA_S60_RELEASE_9_2
    iAllPreferences.iZoomMax = GetIntValue(KBrowserNGZoomMax);
    iAllPreferences.iZoomMin = GetIntValue(KBrowserNGZoomMin);
    iAllPreferences.iZoomDef = GetIntValue(KBrowserNGZoomDefault);
#endif

    // Read Accesss point selection mode for advanced settings
    iAllPreferences.iAccessPointSelectionMode
    = GetIntValue(KBrowserAccessPointSelectionMode);

    // Read default AP setting
    // Get application specific ap
    TUint32 appId =  iApiProvider.Display().ApplicationUid();

    // Get applicatin specific ap
    TUint32 ap = CSwtLafFacade::JavaAPNL(appId);

    if (ap == CSwtLafFacade::GetUintConstant(
                CSwtLafFacade::EMIDletSuiteAPNNotSpecified))
    {
        // if app specific ap not found, try to get java speicific ap
        ap = CSwtLafFacade::JavaAPNL(CSwtLafFacade::GetUintConstant(
                                         CSwtLafFacade::EJavaDefaultAPNKey));
    }

    if (ap == CSwtLafFacade::GetUintConstant(
                CSwtLafFacade::EMIDletSuiteAPNNotSpecified))
    {
        // Try to get ap from browser NG reposictory
        ap = GetIntValue(KBrowserDefaultAccessPoint);
    }
    if (!ap)
    {
        ap = KWmlNoDefaultAccessPoint;
    }

    SetDefaultAccessPointL(ap);
}

void CSwtBrowserPreferences::AddObserverL(MPreferencesObserver* anObserver)
{
    iObservers->AppendL(anObserver);
}

void CSwtBrowserPreferences::RemoveObserver(MPreferencesObserver* anObserver)
{
    TInt i(0);
    TInt count = iObservers->Count();
    for (i = 0; i < count; i++)
    {
        if (iObservers->At(i) == anObserver)
        {
            iObservers->Delete(i);
            break;
        }
    }
}

void CSwtBrowserPreferences::NotifyObserversL(TPreferencesEvent aEvent,
        TBrCtlDefs::TBrCtlSettings aSettingType = TBrCtlDefs::ESettingsUnknown)
{
    TInt i;
    TInt count = iObservers->Count();

    // Passed struct of all preferences to observers
    for (i = 0; i < count; i++)
    {
        iObservers->At(i)->HandlePreferencesChangeL(
            aEvent, iAllPreferences, aSettingType);
    }
}

TBool CSwtBrowserPreferences::VpnDataL(
    TUint aDefaultAccessPoint, CSwtBrowserVpnApItem& aVpnItem)
{
    TBool ret(EFalse);

    if (iVpnEngine->IsVpnApL(aDefaultAccessPoint))
    {
        iVpnEngine->VpnDataL(aDefaultAccessPoint, aVpnItem);
        ret = ETrue;
    }

    return ret;
}

// ----------------------------------------------------------------------------
// If aDefaultAccessPoint is a valid ap, reseet default ap.
// If aDefaultAccessPoint is a invalid ap, select the first ap from ap list and reset default ap
// For wins, always reset defualt ap to dummy ap
// If there is not a valid ap, reset default ap to dummy ap;
// ----------------------------------------------------------------------------
//
void CSwtBrowserPreferences::SetDefaultAccessPointL(TUint aDefaultAccessPoint
        , TUint aAssocVpn)
{
    if ((aDefaultAccessPoint != KWmlNoDefaultAccessPoint)
            && iVpnEngine->IsVpnApL(aDefaultAccessPoint))
    {
        // aDefaultAccessPoint is good

        // Delete old VpnItem
        delete iVpnItem;
        iVpnItem = NULL;

        // Construct a new one
        iVpnItem = CSwtBrowserVpnApItem::NewL();
        CleanupStack::Pop(iVpnItem);
        iVpnEngine->VpnDataL(aDefaultAccessPoint, *iVpnItem);
        TUint32 ass(aDefaultAccessPoint);
        iVpnItem->ReadVpnApUid(ass);
        aAssocVpn = ass;
        // get real WAP id
        TUint32 ap(aDefaultAccessPoint);
        iVpnItem->ReadRealIapId(ap);
        aDefaultAccessPoint = ap;
    }

    // Update data member with default ap  and default Vpn
    iAllPreferences.iDefaultAccessPoint = aDefaultAccessPoint;
    iAllPreferences.iAssocVpn = aAssocVpn;

    // Reads up one access point's data from the database
    CSwtBrowserApItem* api = CSwtBrowserApItem::NewL();
    CleanupStack::PushL(api);
    TInt err;
    TRAP(err, iApDataHandler->AccessPointDataL(iAllPreferences.iDefaultAccessPoint
            , api->ApItem()));

    //Reset default AP pointer and delete the data
    delete iAllPreferences.iDefaultAPDetails;
    iAllPreferences.iDefaultAPDetails = NULL;
    if (err != KErrNone)    //aDefaultAccessPoint is a invalid ap, select the first from ap list
    {
#ifndef __WINSCW__  // If real device is the case
        // The first valid access point has to be selected if exists


        TUint32 firstApUid = iActiveApDb.FirstAccessPointUidL();
        if (firstApUid)   // If the first item exists
        {
            iAllPreferences.iDefaultAccessPoint = firstApUid;
            iApDataHandler->AccessPointDataL(iAllPreferences.iDefaultAccessPoint, api->ApItem());   // Get details
            iAllPreferences.iDefaultAPDetails = api;//save the ap details
        }
        else
        {
            iAllPreferences.iDefaultAccessPoint = KWmlNoDefaultAccessPoint;
            iAllPreferences.iDefaultAPDetails = NULL;
            delete api;
        }
        CleanupStack::Pop(api);
#else
        // If emulator is the case, defaultap will be assigned a uid indicating that
        // there is no default ap )
        iAllPreferences.iDefaultAccessPoint = KWmlNoDefaultAccessPoint;
        iAllPreferences.iDefaultAPDetails = NULL;
        CleanupStack::PopAndDestroy(api);   // also delete it
#endif//WINSCW  
    }
    else
    {
        iAllPreferences.iDefaultAPDetails = api;//store new default accesspoint details
        CleanupStack::Pop(api);
    }

    // If VPN set VPN AP ID to default access point
    if (iAllPreferences.iAssocVpn != KWmlNoDefaultAccessPoint)
    {
        iAllPreferences.iDefaultAccessPoint = iAllPreferences.iAssocVpn;
    }

    //Store ini value
    //SetIntValueL ( KBrowserDefaultAccessPoint
    //    , iAllPreferences.iDefaultAccessPoint ); //don't update browserNG repository
    NotifyObserversL(EPreferencesItemChange);

}

void CSwtBrowserPreferences::SetFontSizeL(TInt aFontSize)
{
    if (aFontSize != iAllPreferences.iFontSize)
    {
        iAllPreferences.iFontSize = aFontSize;
        NotifyObserversL(EPreferencesItemChange, TBrCtlDefs::ESettingsFontSize);
    }
}

void CSwtBrowserPreferences::SetEncodingL(TUint32 aEncoding)
{
    if (aEncoding != iAllPreferences.iEncoding)
    {
        iAllPreferences.iEncoding = aEncoding;
        NotifyObserversL(EPreferencesItemChange, TBrCtlDefs::ESettingsCharacterset);
    }
}

TInt CSwtBrowserPreferences::GetIntValue(TUint32 aKey) const
{
    TInt retVal = 0;
    if (iRepository)
    {
        iRepository->Get(aKey, retVal);
    }
    return retVal;
}

const TPreferencesValues& CSwtBrowserPreferences::AllPreferences() const
{
    return iAllPreferences;
}

TPtrC CSwtBrowserPreferences::SelfDownloadContentTypesL()
{
    TInt error = KErrNone;
    const TInt KInitialLength(32);   // 32 characters
    TInt actualSize(0);

    // Create a 32 length buf to get data from repository
    HBufC* temp = HBufC::NewL(KInitialLength);
    delete iSelfDownloadContentTypes;
    iSelfDownloadContentTypes = temp;
    TPtr ptr = iSelfDownloadContentTypes->Des();

    error = iRepository->Get(KBrowserSelfDownloadContentTypes, ptr, actualSize);  // Have a try

    if (error == KErrOverflow && actualSize > KInitialLength)   // If failed
    {
        // Clean created buf, and create another one with accurate size
        HBufC* temp = HBufC::NewL(actualSize);
        delete iSelfDownloadContentTypes;
        iSelfDownloadContentTypes = temp;
        TPtr ptr = iSelfDownloadContentTypes->Des();
        error = iRepository->Get(KBrowserSelfDownloadContentTypes, ptr);   // Get data again
    }

    ASSERT(error != KErrOverflow);

    return (*iSelfDownloadContentTypes);
}

TBool CSwtBrowserPreferences::AccessPointSelectionMode()
{
    // returns ETrue for 'Always Ask', EFalse for 'User Defined'
    return iAllPreferences.iAccessPointSelectionMode;
}

void CSwtBrowserPreferences::SetAccessPointSelectionModeL(
    TWmlSettingsAccessPointSelectionMode aAccessPointSelectionMode)
{
    if (aAccessPointSelectionMode != iAllPreferences.iAccessPointSelectionMode)
    {
        iAllPreferences.iAccessPointSelectionMode = aAccessPointSelectionMode;
    }
}

TBool CSwtBrowserPreferences::AutoLoadImages() const
{
    return iAllPreferences.iAutoLoadImages;
}

TBool CSwtBrowserPreferences::BackList() const
{
    return iAllPreferences.iBackList;
}

TBool CSwtBrowserPreferences::AutoRefresh() const
{
    return iAllPreferences.iAutoRefresh;
}

TBool CSwtBrowserPreferences::Cookies() const
{
    return iAllPreferences.iCookies;
}

TBool CSwtBrowserPreferences::Ecma() const
{
    return iAllPreferences.iEcma;
}

TBool CSwtBrowserPreferences::IMEINotification() const
{
    return iAllPreferences.iIMEINotification;
}

TBool CSwtBrowserPreferences::SendReferrer() const
{
    return iAllPreferences.iSendReferrer;
}

TBool CSwtBrowserPreferences::PageOverview() const
{
    return iAllPreferences.iPageOverview;
}

TUint CSwtBrowserPreferences::FormDataSaving() const
{
    return iAllPreferences.iFormDataSaving;
}

TUint CSwtBrowserPreferences::DefaultAccessPoint() const
{
    if ((iOverridenSettings) && iCustomAp)
    {
        return iCustomAp;
    }
    else
    {
        return iAllPreferences.iDefaultAccessPoint ;
    }
}

TBool CSwtBrowserPreferences::HttpSecurityWarnings() const
{
    return iAllPreferences.iHttpSecurityWarnings;
}

TBool CSwtBrowserPreferences::CdmaUiLocalFeatureSupported(
    const TInt aFeature) const
{
    return (iCdmaUiLocalFeaturesBitmask & aFeature);
}

TBool CSwtBrowserPreferences::ShowConnectionDialogs() const
{
    return iAllPreferences.iConnDialogs;
}

TBool CSwtBrowserPreferences::UiLocalFeatureSupported(const TInt aFeature) const
{
    return (iUiLocalFeaturesBitmask & aFeature);
}

MBrowserApiProvider& CSwtBrowserPreferences::ApiProvider()
{
    return iApiProvider;
}
