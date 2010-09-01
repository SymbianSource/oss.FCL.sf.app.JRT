/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/



#include <browserdialogsprovider.h>         // DOMAIN API
#include <browserdialogsproviderobserver.h> // DOMAIN API
#include "swtbrowserdialogsproviderobserver.h"
#include "swtbrowserdialogsproviderobserverproxyprivate.h"
#include "swtbrowserdialogsproviderproxy.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtBrowserDialogsProviderProxy* CSwtBrowserDialogsProviderProxy::NewL(
    MSwtBrowserDialogsProviderObserver* aObserver)
{
    CSwtBrowserDialogsProviderProxy* self =
        new(ELeave) CSwtBrowserDialogsProviderProxy(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::~CSwtBrowserDialogsProviderProxy
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtBrowserDialogsProviderProxy::~CSwtBrowserDialogsProviderProxy()
{
    // Not own
    iObserver = NULL;

    // Own
    delete iDelegate;
    iDelegate = NULL;
    delete iDelegateObserver;
    iDelegateObserver = NULL;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogNotifyErrorL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::DialogNotifyErrorL(TInt aErrCode)
{
    iDelegate->DialogNotifyErrorL(aErrCode);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::ENotifyError,
            aErrCode);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogNotifyHttpErrorL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::DialogNotifyHttpErrorL(
    TInt aErrCode, const TDesC& aUri)
{
    iDelegate->DialogNotifyHttpErrorL(aErrCode, aUri);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::ENotifyHttpError,
            aErrCode);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogFileSelectLC
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtBrowserDialogsProviderProxy::DialogFileSelectLC(
    const TDesC& aStartPath, const TDesC& aRootPath, HBufC*& aSelectedFileName)
{
    TBool res = iDelegate->DialogFileSelectLC(aStartPath, aRootPath, aSelectedFileName);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EFileSelect,
            static_cast<TInt>(res));
    }
    return res;
}
// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogSelectOptionL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtBrowserDialogsProviderProxy::DialogSelectOptionL(
    const TDesC& aTitle, TBrCtlSelectOptionType aSelectOptionType,
    CArrayFix<TBrCtlSelectOptionData>& aOptions)
{
    TBool res = iDelegate->DialogSelectOptionL(aTitle, aSelectOptionType, aOptions);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::ESelectOption,
            static_cast<TInt>(res));
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogUserAuthenticationLC
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtBrowserDialogsProviderProxy::DialogUserAuthenticationLC(
    const TDesC& aUrl, const TDesC& aRealm, const TDesC& aDefaultUserName,
    HBufC*& aReturnedUserName, HBufC*& aReturnedPasswd,
    TBool aBasicAuthentication /*= EFalse*/)
{
    TBool res = iDelegate->DialogUserAuthenticationLC(aUrl, aRealm, aDefaultUserName,
                aReturnedUserName, aReturnedPasswd, aBasicAuthentication);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EUserAuthentication,
            static_cast<TInt>(res));
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogNoteL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::DialogNoteL(const TDesC& aMessage)
{
    iDelegate->DialogNoteL(aMessage);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::ENote, 0);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogAlertL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::DialogAlertL(const TDesC& aTitle,
        const TDesC& aMessage)
{
    iDelegate->DialogAlertL(aTitle, aMessage);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EAlert, 0);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogConfirmL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtBrowserDialogsProviderProxy::DialogConfirmL(const TDesC& aTitle,
        const TDesC& aMessage, const TDesC& aYesMessage, const TDesC& aNoMessage)
{
    TBool res = iDelegate->DialogConfirmL(aTitle, aMessage, aYesMessage, aNoMessage);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EConfirm,
            static_cast<TInt>(res));
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogPromptLC
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtBrowserDialogsProviderProxy::DialogPromptLC(const TDesC& aTitle,
        const TDesC& aMessage, const TDesC& aDefaultInput, HBufC*& aReturnedInput)
{
    TBool res = iDelegate->DialogPromptLC(aTitle, aMessage, aDefaultInput, aReturnedInput);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EPrompt,
            static_cast<TInt>(res));
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogDownloadObjectL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtBrowserDialogsProviderProxy::DialogDownloadObjectL(
    CBrCtlObjectInfo* aBrCtlObjectInfo)
{
    TBool res = iDelegate->DialogDownloadObjectL(aBrCtlObjectInfo);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EDownloadObject,
            static_cast<TInt>(res));
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogDisplayPageImagesL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::DialogDisplayPageImagesL(
    CArrayFixFlat<TBrCtlImageCarrier>& aPageImages)
{
    iDelegate->DialogDisplayPageImagesL(aPageImages);
    if (iObserver)
    {
        iObserver->ReportDialogEventL(
            MSwtBrowserDialogsProviderObserver::EDisplayPageImages, 0);
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::CancelAll
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::CancelAll()
{
    iDelegate->CancelAll();
    if (iObserver)
    {
        TRAP_IGNORE(iObserver->ReportDialogEventL(
                        MSwtBrowserDialogsProviderObserver::ECancelAll, 0));
    }
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::DialogFindL
// From MBrCtlDialogsProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtBrowserDialogsProviderProxy::DialogFindL()
{
    iDelegate->DialogFindL();
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::CSwtBrowserDialogsProviderProxy
// ---------------------------------------------------------------------------
//
CSwtBrowserDialogsProviderProxy::CSwtBrowserDialogsProviderProxy(
    MSwtBrowserDialogsProviderObserver* aObserver)
        : CBase()
        , iObserver(aObserver)
{
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderProxy::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtBrowserDialogsProviderProxy::ConstructL()
{
    iDelegateObserver = new(ELeave) CSwtBrowserDialogsProviderObserverProxyPrivate(iObserver);
    iDelegate = CBrowserDialogsProvider::NewL(iDelegateObserver);
}
