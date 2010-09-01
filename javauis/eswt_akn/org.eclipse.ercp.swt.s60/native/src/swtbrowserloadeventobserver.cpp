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


#include <eswtcore.rsg>
#include <StringLoader.h>
#include "swtbrowserloadeventobserver.h"
#include "browserapiprovider.h"


// Error code for card not in deck, from BrowserNG
const TInt KBrsrWmlbrowserCardNotInDeck = -26144;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::CSwtBrowserLoadEventObserver
// ---------------------------------------------------------------------------
//
CSwtBrowserLoadEventObserver::CSwtBrowserLoadEventObserver(
    MBrowserApiProvider* aApiProvider, TSwtPeer aPeer)
        : iApiProvider(aApiProvider)
        , iPeer(aPeer)
        , iTransactionId(0)
        , iRestoreContentFlag(EFalse)
{
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::CSwtBrowserLoadEventObserver
// ---------------------------------------------------------------------------
//
CSwtBrowserLoadEventObserver::~CSwtBrowserLoadEventObserver()
{
    delete iUrl;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::DoStartLoad
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::DoStartLoad(TBrowserLoadUrlType aLoadUrlType)
{
    if (iLoadState == ELoadStateIdle)
    {
        ClearStatus();
        iLoadUrlType = aLoadUrlType;
        iRestoreContentFlag = EFalse;
        iTransactionId = 0;
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::DoEndLoad
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::DoEndLoad(
    TBool aIsUserInitiated)
{
    if (aIsUserInitiated && (iLoadUrlType != ELoadUrlTypeSavedDeck))
    {
        StateChange(ELoadStateLoadDone);
    }
    else
    {
        StateChange(ELoadStateIdle);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::LoadUrlType
// ---------------------------------------------------------------------------
//
CSwtBrowserLoadEventObserver::TBrowserLoadUrlType CSwtBrowserLoadEventObserver::LoadUrlType() const
{
    return iLoadUrlType;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::LoadState
// ---------------------------------------------------------------------------
//
CSwtBrowserLoadEventObserver::TBrowserLoadState CSwtBrowserLoadEventObserver::LoadState() const
{
    return iLoadState;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::HandleBrowserLoadEventL
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::HandleBrowserLoadEventL(
    TBrCtlDefs::TBrCtlLoadEvent aLoadEvent, TUint aSize, TUint16 aTransactionId)
{
    switch (iLoadUrlType)
    {
    case ELoadUrlTypeSavedDeck:
    {
        HandleLoadEventSavedDeckL(aLoadEvent, aSize, aTransactionId);
        break;
    }
    default:
    {
        HandleLoadEventOtherL(aLoadEvent, aSize, aTransactionId);
        break;
    }
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::ReportDialogEventL
// From MSwtBrowserDialogsProviderObserver
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::ReportDialogEventL(TInt aType, TInt aFlags)
{
    switch (aType)
    {
    case MSwtBrowserDialogsProviderObserver::ENotifyError:
    {
        iRestoreContentFlag = (aFlags == KBrsrWmlbrowserCardNotInDeck);
        break;
    }
    case MSwtBrowserDialogsProviderObserver::ENotifyHttpError:
    {
        iRestoreContentFlag =  EFalse;
        break;
    }
    case MSwtBrowserDialogsProviderObserver::EUserAuthentication:
    {
        iRestoreContentFlag =  aFlags;
        break;
    }
    case MSwtBrowserDialogsProviderObserver::EConfirm:
    {
        if (!aFlags)
        {
            DoEndLoad(EFalse);
        }
        iRestoreContentFlag =  !aFlags;
        break;
    }
    case MSwtBrowserDialogsProviderObserver::EUploadProgress:
    {
        if (aFlags == KErrCancel)
        {
            iApiProvider->BrCtlInterface().HandleCommandL(
                TBrCtlDefs::ECommandCancelFetch +
                TBrCtlDefs::ECommandIdBase);
        }
        break;
    }
    default:
        break;
    }

    HBufC* textbuf= StringLoader::LoadL(R_SWT_STATUS_TEXT_LOAD_ERROR);
    if (textbuf)
    {
        CleanupStack::PushL(textbuf);
        iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
        CleanupStack::PopAndDestroy(textbuf);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::HandleLoadEventSavedDeckL
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::HandleLoadEventSavedDeckL(
    TBrCtlDefs::TBrCtlLoadEvent aLoadEvent, TUint aSize,
    TUint16 aTransactionId)
{
    HBufC* textbuf = NULL;
    switch (aLoadEvent)
    {
    case TBrCtlDefs::EEventLoadError:
    {
        if (iTransactionId && aTransactionId == iTransactionId)
        {
            SetStatus(ELoadStatusMainError);
        }
        else
        {
            SetStatus(ELoadStatusSubError);
        }
        textbuf = StringLoader::LoadL(R_SWT_STATUS_TEXT_LOAD_ERROR);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        break;
    }
    case TBrCtlDefs::EEventTitleAvailable:
    {
        SetStatus(ELoadStatusTitleAvailable);
        textbuf = iApiProvider->BrCtlInterface().PageInfoLC(TBrCtlDefs::EPageInfoTitle);
        if (textbuf)
        {
            iApiProvider->Display().PostTitleEventL(iPeer, *textbuf);
        }
        CleanupStack::PopAndDestroy(textbuf);
        break;
    }
    case TBrCtlDefs::EEventUrlLoadingStart:
    {
        if (iTransactionId == 0)
        {
            iTransactionId = aTransactionId;
            if (iUrl)
            {

                textbuf = FormatEventTextLC(R_SWT_STATUS_TEXT_URL_LOADING_START, *iUrl) ;
                if (textbuf)
                {
                    iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
                }
                CleanupStack::PopAndDestroy(textbuf);
            }
        }
        break;
    }
    case TBrCtlDefs::EEventNewUrlContentArrived:
    case TBrCtlDefs::EEventMoreUrlContentArrived:
    {
        if (aTransactionId == iTransactionId)
        {
            SetStatus(ELoadStatusFirstChunkArrived);
            iApiProvider->Display().PostProgressEventL(iPeer, aTransactionId, aSize);
        }
        break;
    }
    case TBrCtlDefs::EEventContentFinished:
    {
        StateChange(ELoadStateIdle);
        iTransactionId = 0;
        iApiProvider->Display().PostProgressCompletedEventL(iPeer, aTransactionId, 0);
        textbuf = StringLoader::LoadL(R_SWT_STATUS_TEXT_CONTENT_FINISHED);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        if (iUrl)
        {
            iApiProvider->Display().PostLocationChangedEventL(iPeer, ETrue, ETrue, *iUrl);
        }
        break;
    }
    case TBrCtlDefs::EEventAuthenticationFailed:
    {
        ClearStatus(ELoadStatusFirstChunkArrived);
        break;
    }
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::SetUrlL
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::SetUrlL(const TDesC& aUrl)
{
    delete iUrl;
    iUrl = NULL;
    iUrl = aUrl.AllocL();
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::HandleLoadEventOtherL
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::HandleLoadEventOtherL(
    TBrCtlDefs::TBrCtlLoadEvent aLoadEvent,
    TUint aSize,
    TUint16 aTransactionId)
{
    HBufC* textbuf = NULL;
    switch (aLoadEvent)
    {
    case TBrCtlDefs::EEventLoadError:
    {
        if (iTransactionId && aTransactionId == iTransactionId)
        {
            SetStatus(ELoadStatusMainError);
        }
        else
        {
            SetStatus(ELoadStatusSubError);
        }
        textbuf= StringLoader::LoadL(R_SWT_STATUS_TEXT_LOAD_ERROR);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        break;
    }
    case TBrCtlDefs::EEventEnteringSecurePage:
    {
        SetStatus(ELoadStatusSecurePage);
        SetStatus(ELoadStatusAllItemIsSecure);
        textbuf= StringLoader::LoadL(R_SWT_STATUS_TEXT_ENTERING_SECURE_PAGE);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        break;
    }
    case TBrCtlDefs::EEventSomeItemsNotSecure:
    {
        ClearStatus(ELoadStatusAllItemIsSecure);
        break;
    }
    case TBrCtlDefs::EEventSecureItemInNonSecurePage:
    {
        SetStatus(ELoadStatusSecureItemNonSecurePage);
        break;
    }
    case TBrCtlDefs::EEventExitingSecurePage:
    case TBrCtlDefs::EEventSubmittingToNonSecurePage:
    {
        ClearStatus(ELoadStatusSecurePage);
        ClearStatus(ELoadStatusAllItemIsSecure);
        textbuf = StringLoader::LoadL(R_SWT_STATUS_TEXT_EXITING_SECURE_PAGE);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        break;
    }
    case TBrCtlDefs::EEventTitleAvailable:
    {
        SetStatus(ELoadStatusTitleAvailable);
        textbuf = iApiProvider->BrCtlInterface().PageInfoLC(TBrCtlDefs::EPageInfoTitle);
        if (textbuf)
        {
            iApiProvider->Display().PostTitleEventL(iPeer, *textbuf);
        }
        CleanupStack::PopAndDestroy(textbuf);
        break;

    }
    case TBrCtlDefs::EEventNewContentStart:
    {
        StateChange(ELoadStateResponseInProgress);
        iApiProvider->Display().PostProgressEventL(iPeer, aTransactionId, aSize);
        textbuf = StringLoader::LoadL(R_SWT_STATUS_TEXT_NEW_CONTENT_START);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        break;
    }
    case TBrCtlDefs::EEventUrlLoadingStart:
    {
        if (iLoadState & ELoadStateResponseInProgress)
        {
            if (iTransactionId == 0)
            {
                iTransactionId = aTransactionId;
                iApiProvider->Display().PostProgressEventL(iPeer, 0, 0);
            }
            textbuf = FormatEventTextLC(R_SWT_STATUS_TEXT_URL_LOADING_START, *iUrl) ;
            if (textbuf)
            {
                iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            }
            CleanupStack::PopAndDestroy(textbuf);
        }
        break;
    }
    case TBrCtlDefs::EEventNewUrlContentArrived:
    {
        if (iLoadState & ELoadStateResponseInProgress)
        {
            if (aTransactionId == iTransactionId)
            {
                SetStatus(ELoadStatusFirstChunkArrived);
            }
            iApiProvider->Display().PostProgressEventL(iPeer, aTransactionId, aSize);
        }
        break;
    }
    case TBrCtlDefs::EEventMoreUrlContentArrived:
    {
        iApiProvider->Display().PostProgressEventL(iPeer, aTransactionId, aSize);
        break;
    }
    case TBrCtlDefs::EEventContentFinished:
    {
        StateChange(ELoadStateIdle);
        iTransactionId = 0;
        textbuf = StringLoader::LoadL(R_SWT_STATUS_TEXT_CONTENT_FINISHED);
        if (textbuf)
        {
            CleanupStack::PushL(textbuf);
            iApiProvider->Display().PostStatusTextEventL(iPeer, *textbuf);
            CleanupStack::PopAndDestroy(textbuf);
        }
        if (iUrl)
        {
            iApiProvider->Display().PostLocationChangedEventL(iPeer, ETrue, ETrue, *iUrl);
        }
        break;
    }
    case TBrCtlDefs::EEventNewContentDisplayed:
    {
        SetStatus(ELoadStatusFirstChunkDisplayed);
        iRestoreContentFlag = ETrue;
        break;
    }
    case TBrCtlDefs::EEventMoreContentDisplayed:
    {
        SetStatus(ELoadStatusContentDisplayed);
        iRestoreContentFlag = ETrue;
        break;
    }
    case TBrCtlDefs::EEventUrlLoadingFinished:
    {
        iApiProvider->Display().PostProgressCompletedEventL(iPeer, aTransactionId, 0);
        if (aTransactionId == iTransactionId)
        {
            iRestoreContentFlag = ETrue;
        }
        break;
    }
    case TBrCtlDefs::EEventAuthenticationFailed:
    {
        ClearStatus(ELoadStatusFirstChunkArrived);
        break;
    }
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::StateChange
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::StateChange(TBrowserLoadState aNextState)
{
    iLoadState = aNextState;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::SetStatus
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::SetStatus(TUint32 aFlag)
{
    iStatus |= aFlag;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::ClearStatus
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::ClearStatus(TUint32 aFlag)
{
    iStatus &= ~aFlag;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::ClearStatus
// ---------------------------------------------------------------------------
//
void CSwtBrowserLoadEventObserver::ClearStatus()
{
    iStatus = 0;
}

// ---------------------------------------------------------------------------
// CSwtBrowserLoadEventObserver::FormatEventTextLC
// ---------------------------------------------------------------------------
//
HBufC* CSwtBrowserLoadEventObserver::FormatEventTextLC(TInt aResourceId, const TDesC& aText)
{
    HBufC * resString = StringLoader::LoadL(aResourceId);
    CleanupStack::PushL(resString);
    HBufC * buf=HBufC::NewL(resString->Length() + aText.Length());
    TPtr tptr=buf->Des();
    tptr.Copy(*resString);
    CleanupStack::PopAndDestroy(resString);
    tptr.Append(aText);
    CleanupStack::PushL(buf);
    return buf;
}
