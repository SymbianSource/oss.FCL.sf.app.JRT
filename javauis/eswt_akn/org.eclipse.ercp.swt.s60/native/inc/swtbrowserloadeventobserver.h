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


#ifndef SWTBROWSERLOADEVENTOBSERVER_H
#define SWTBROWSERLOADEVENTOBSERVER_H


#include <brctlinterface.h>
#include <swtbrowserdialogsproviderobserver.h>
#include "eswt.h"


class MBrowserApiProvider;


/**
 * CSwtBrowserLoadEventObserver
 * Handle load events.
 * This class implements MBrCtlLoadEventObserver, which registers for load events.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserLoadEventObserver)
        : public CBase
        , public MBrCtlLoadEventObserver
        , public MSwtBrowserDialogsProviderObserver
{
// Types
public:
    enum TBrowserLoadState
    {
        ELoadStateIdle = 0x01,
        ELoadStateContentStart = 0x02,
        ELoadStateResponseInProgress = 0x04,
        ELoadStateLoadDone = 0x08
    };

    enum TBrowserLoadUrlType
    {
        ELoadUrlTypeSavedDeck = 0x01,
        ELoadUrlTypeStartPage,
        ELoadUrlTypeEmbeddedBrowserWithUrl,
        ELoadUrlTypeOther
    };

    enum TBrowserLoadStatus
    {
        ELoadStatusFirstChunkArrived = 0x0001,
        ELoadStatusContentDisplayed = 0x0002,
        ELoadStatusMainError = 0x0004,
        ELoadStatusSubError = 0x0008,
        ELoadStatusError = ELoadStatusMainError | ELoadStatusSubError,
        ELoadStatusTitleAvailable = 0x0010,
        ELoadStatusFirstChunkDisplayed = 0x0020,
        ELoadStatusDone = 0x0040,
        ELoadStatusSecurePage = 0x0080,
        ELoadStatusSecureItemNonSecurePage = 0x0100,
        ELoadStatusAllItemIsSecure = 0x0200
    };

// Own methods
public:
    /**
     * Constructor
     */
    CSwtBrowserLoadEventObserver(MBrowserApiProvider* aApiProvider, TSwtPeer aPeer);

    /**
     * Destructor
     */
    virtual ~CSwtBrowserLoadEventObserver();

    /**
     * Start loading
     */
    void DoStartLoad(TBrowserLoadUrlType aLoadUrlType);

    /**
     * End loading
     */
    void DoEndLoad(TBool aIsUserInitiated = EFalse);

    /**
     * Return current Url's type
     */
    TBrowserLoadUrlType LoadUrlType() const;

    /**
     * Return current load state.
     */
    TBrowserLoadState LoadState() const;

    /**
     * Set the url to load
     */
    void SetUrlL(const TDesC& aUrl);

// From MBrCtlLoadEventObserver
public:
    void HandleBrowserLoadEventL(TBrCtlDefs::TBrCtlLoadEvent aLoadEvent,
                                 TUint aSize, TUint16 aTransactionId);

// From MSwtBrowserDialogsProviderObserver
public:
    void ReportDialogEventL(TInt aType, TInt aFlags);

// Own methods
private:
    /**
     * Handle load event ELoadUrlTypeSavedDeck.
     */
    void HandleLoadEventSavedDeckL(TBrCtlDefs::TBrCtlLoadEvent aLoadEvent,
                                   TUint aSize, TUint16 aTransactionId);

    /**
     * Handle load event other than ELoadUrlTypeSavedDeck.
     */
    void HandleLoadEventOtherL(TBrCtlDefs::TBrCtlLoadEvent aLoadEvent,
                               TUint aSize, TUint16 aTransactionId);

    /**
     * Set load state.
     */
    void StateChange(TBrowserLoadState aNextState);

    /**
     * Set specified status..
     */
    void SetStatus(TUint32 aFlag);

    /**
     * Clear specified status
     */
    void ClearStatus(TUint32 aFlag);

    /**
     * Reset all status.
     */
    void ClearStatus();

    /**
    * Concatenate two string with one from resource
    */
    HBufC* FormatEventTextLC(TInt aResourceId, const TDesC& aText);

// Data
private:
    /**
     * Load state
     */
    TBrowserLoadState iLoadState;

    /**
     * Url's type
     */
    TBrowserLoadUrlType iLoadUrlType;

    /**
     * Load status
     */
    TUint32 iStatus;

    /**
     * Pointer to browser API provider.
     * Not own
     */
    MBrowserApiProvider *iApiProvider;

    /**
     * Java side peer
     * Not own
     */
    TSwtPeer iPeer;

    /**
     * Transaction ID
     */
    TUint16 iTransactionId;

    /**
     * Flag to indicate whether the content needs to be restored
     */
    TBool iRestoreContentFlag;

    /**
     * URL to be load
     * Own
     */
    HBufC* iUrl;
};

#endif // SWTBROWSERLOADEVENTOBSERVER_H
