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



#include "swtpopupformproxyprivate.h"
#include "swtpopupformproxy.h"


/*
 * Message box styles (identical with the ones defined in eswt.h)
 */
const TInt KSwtMsgBoxStyleError       = 1;       ///< Message box style for error icon behaviour
const TInt KSwtMsgBoxStyleInformation = 1 <<  1; ///< Message box style for an information icon
const TInt KSwtMsgBoxStyleQuestion    = 1 <<  2; ///< Message box style for a question icon
const TInt KSwtMsgBoxStyleWarning     = 1 <<  3; ///< Message box style for a warning icon
const TInt KSwtMsgBoxStyleWorking     = 1 <<  4; ///< Message box style for a "working" icon


CSwtPopupFormProxy::CSwtPopupFormProxy()
{
}

void CSwtPopupFormProxy::ConstructL(const TDesC& aText, const TDesC& aTitle,
                                    const TInt aHeadResourceId, const TInt aSpaceResourceId, const TInt aDlgResourceId,
                                    const TInt aBtnResourceId, const TInt aAniResourceId, const TInt aStyle,
                                    const TInt aMsgBoxMenuBarResourceId, const TInt aSwtCmdYes,
                                    const TInt aSwtCmdNo, const TInt aTimeOut,
                                    const CEikImage* aImage, const TInt aImageResourceId)
{
    TTone tone(EInformationTone);
    if (aStyle & KSwtMsgBoxStyleInformation)
    {
        tone = EInformationTone;
    }
    else if (aStyle & KSwtMsgBoxStyleError)
    {
        tone = EErrorTone;
    }
    else if (aStyle & KSwtMsgBoxStyleWarning)
    {
        tone = EWarningTone;
    }
    else if (aStyle & KSwtMsgBoxStyleQuestion)
    {
        tone = EConfirmationTone;
    }
    else if (aStyle & KSwtMsgBoxStyleWorking)
    {
        tone = EInformationTone;
    }

    iPopupForm = CSwtPopupFormProxyPrivate::NewL(tone, aMsgBoxMenuBarResourceId,
                 aSwtCmdYes, aSwtCmdNo, aTimeOut);

    iPopupForm->PrepareLC(aDlgResourceId);
    CleanupStack::Pop();

    if ((!aImage) && aAniResourceId)
    {
        iPopupForm->InsertControlL(0, aAniResourceId, EFalse);
    }
    iPopupForm->SetTextL(&aText);
    iPopupForm->SetTitleL(aTitle);
    if (aTitle.Length() > 0)
    {
        iPopupForm->InsertControlL(0, aHeadResourceId, EFalse);
        iPopupForm->InsertControlL(0, aSpaceResourceId, EFalse);
        iPopupForm->SetTitleL(aTitle);
    }

    if (aTimeOut)
    {
        // if timed messagebox
        iPopupForm->SetTimeout(aTimeOut);
        // set image
        if (aImage)
        {
            iPopupForm->InsertControlL(0, aImageResourceId, EFalse);
            iPopupForm->SetImageL(aImage);
            iPopupForm->Layout();
        }
    }
    else
    {
        // if not a timed messagebox
        iPopupForm->ButtonGroupContainer().SetCommandSetL(aBtnResourceId);
    }



#ifdef RD_SCALABLE_UI_V2
    if (AknLayoutUtils::PenEnabled())
    {
        iPopupForm->SetGloballyCapturing(ETrue);
        iPopupForm->SetPointerCapture(ETrue);
    }
#endif // RD_SCALABLE_UI_V2

}

EXPORT_C CSwtPopupFormProxy* CSwtPopupFormProxy::NewL(const TDesC& aText, const TDesC& aTitle,
        const TInt aHeadResourceId, const TInt aSpaceResourceId, const TInt aDlgResourceId,
        const TInt aBtnResourceId, const TInt aAniResourceId, const TInt aStyle,
        const TInt aMsgBoxMenuBarResourceId, const TInt aSwtCmdYes,
        const TInt aSwtCmdNo, const TInt aTimeOut,
        const CEikImage* aImage, const TInt aImageResourceId)
{
    CSwtPopupFormProxy* self = new(ELeave) CSwtPopupFormProxy();
    CleanupStack::PushL(self);

    self->ConstructL(aText, aTitle, aHeadResourceId, aSpaceResourceId,  aDlgResourceId,
                     aBtnResourceId, aAniResourceId, aStyle, aMsgBoxMenuBarResourceId, aSwtCmdYes,
                     aSwtCmdNo, aTimeOut, aImage, aImageResourceId);

    CleanupStack::Pop(self);
    return self;
}

CSwtPopupFormProxy::~CSwtPopupFormProxy()
{
    delete iPopupForm;
}

EXPORT_C TInt CSwtPopupFormProxy::RunL()
{
    CSwtPopupFormProxyPrivate* popupForm(iPopupForm);
    iPopupFormNoDelete = iPopupForm;
    iPopupForm = NULL;
    CleanupStack::PushL(popupForm);
    TInt result = popupForm->RunLD();
    iPopupFormNoDelete = NULL;
    // WORKAROUND: Normally this should have been done in CAknPopupForm
    (CEikonEnv::Static())->EikAppUi()->RemoveFromStack(popupForm);
    return result;
}

EXPORT_C void CSwtPopupFormProxy::OfferResourceChangeL(const TInt aType, const TInt /*aAnimation*/)
{
    if (!iPopupFormNoDelete)
    {
        return;
    }

    iPopupFormNoDelete->HandleResourceChange(aType);
}
