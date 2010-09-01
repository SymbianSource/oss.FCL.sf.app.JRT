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


#ifndef BROWSERAPIPROVIDER_H
#define BROWSERAPIPROVIDER_H


#include "swtbrowserloadeventobserver.h"


class MSwtDisplay;
class CBrCtlInterface;
class MBrowserPreferences;
class CSwtBrowserActiveApDb;
class CBrowserDialogsProvider;
class CSwtBrowserSpecialLoadObserver;
class TFavouritesWapAp;
class MBrowserConnection;


/**
 * MBrowserApiProvider
 * Interface to query several API implementations.
 * @lib eswt
 */
NONSHARABLE_CLASS(MBrowserApiProvider)
{
public:

    /**
     * Returns reference to the implementation of MPrefences.
     * @return An implementation of MPrefences
     */
    virtual MBrowserPreferences& Preferences() const = 0;

    /**
     * Returns a reference to the implementaion of MBrowserConnection,
     * through which some high-level connection specific data can be set
     * @return reference to MBrowserConnection object
     */
    virtual MBrowserConnection& Connection() const = 0;

    /**
     * Returns reference to the implementation of active ap database.
     * @return An active ap database
     */
    virtual CSwtBrowserActiveApDb& ActiveApDb() const = 0;

    /**
     * Returns reference to the CBrCtlInterface
     * @return A reference to CBrCtlInterface
     */
    virtual CBrCtlInterface& BrCtlInterface() const = 0;

    /**
     * Returns reference to CSwtBrowserLoadEventObserver.
     * @return An implementation of MBrCtlLoadEventObserver
     */
    virtual CSwtBrowserLoadEventObserver& LoadObserver() const = 0;

    /**
     * Returns reference to CSwtBrowserSpecialLoadObserver.
     * @return An implementation of MBrCtlSpecialLoadObserver
     */
    virtual CSwtBrowserSpecialLoadObserver& SpecialLoadObserver() const = 0;

    /**
     * Returns reference to the implementation of MSwtDisplay.
     * @return An implementation of MSwtDisplay
     */
    virtual MSwtDisplay& Display() const = 0;

    /**
     * Starts prefrences view if no valid ap
     * @return ETrue if access point has been set.
     */
    virtual TBool StartPreferencesViewL() = 0;

    /**
     * Is there connection procedure ongoing?
     * @return ETrue if connection procedure is ongoing
     */
    virtual TBool IsConnecting() const = 0;

    /**
     * Returns fetching status.
     * @return ETrue if there is a fetch going on, EFalse otherwise.
     */
    virtual TBool Fetching() const = 0;

    /**
     * Initiate fetching of URL.
     * @param aUrl URL to fetch.
     * @param aUserName User name.
     * @param aPassword Password.
     * @param aAccessPoint AccessPoint.
     * @param aUrlType Url's type.
     */
    virtual void FetchL(const TDesC& aUrl, const TDesC& aUserName,
                        const TDesC& aPassword, const TFavouritesWapAp& aAccessPoint,
                        CSwtBrowserLoadEventObserver::TBrowserLoadUrlType aUrlType) = 0;

    /**
     * Initiate fetching of URL with default AP, empty username and password.
     * @param aUrl URL to fetch.
     * @param aUrlType Url's type.
     * @return void.
     */
    virtual void FetchL(const TDesC& aUrl,
                        CSwtBrowserLoadEventObserver::TBrowserLoadUrlType aUrlType
                        = CSwtBrowserLoadEventObserver::ELoadUrlTypeOther) = 0;

    /**
     * Cancel fetching.
     * @param aIsUserInitiated.
     * @return void.
     */
    virtual void CancelFetchL(TBool aIsUserInitiated) = 0;

    /**
     * Sets requested AP.
     * @param aAp the AP to use when connectionneeded is called.
     * @return void.
     */
    virtual void SetRequestedAP(TInt aAp) = 0;

    /**
     * Return a boolean value which indicates whether browser has been started up.
     * @return ETrue if browser has been startedup, EFalse otherwise.
     */
    virtual TBool StartedUp() const = 0;

    /**
     * Returns last used AP.
     * @return requested ap id.
     */
    virtual TUint32 RequestedAp() const = 0;

    /**
     * Initiate fetching of saved deck.
     * @param aUid Uid of saved deck to fetch.
     * @return void.
     */
    virtual void FetchSavedDeckL(TInt aUid) = 0;
};

#endif // BROWSERAPIPROVIDER_H
