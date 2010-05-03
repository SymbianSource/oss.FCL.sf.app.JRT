/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <AknUtils.h>
#include <avkon.rsg>
#include <eswtcore.rsg>
#include <swtpopupformproxy.h>
#include "eswtcore.hrh"
#include "swtmessagebox.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMessageBox::RunDlgLD
// ---------------------------------------------------------------------------
//
TInt CSwtMessageBox::RunDlgLD(MSwtDisplay& aDisplay, const TDesC& aTitle,
                              const TDesC& aMessage, TInt aStyle)
{
    CSwtMessageBox* dlg = CSwtMessageBox::NewLC(aDisplay, aTitle, aMessage, aStyle);
    CleanupStack::Pop(dlg);
    int result =  dlg->RunL();
    delete dlg;
    return result;
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::NewLC
// ---------------------------------------------------------------------------
//
CSwtMessageBox* CSwtMessageBox::NewLC(MSwtDisplay& aDisplay, const TDesC& aTitle,
                                      const TDesC& aMessage, TInt aStyle)
{
    CSwtMessageBox* self = new(ELeave) CSwtMessageBox(aDisplay, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL(aTitle, aMessage);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::~CSwtMessageBox
// ---------------------------------------------------------------------------
//
CSwtMessageBox::~CSwtMessageBox()
{
    iDisplay.RemoveResourceChangeObserver(this);
    delete iPopupForm;
    iPopupForm = NULL;
}

TInt CSwtMessageBox::RunL()
{
    return ConvertButtonId(iPopupForm->RunL());
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::OfferResourceChangeL
// From MSwtResourceChangeObserver
// ---------------------------------------------------------------------------
//
void CSwtMessageBox::OfferResourceChangeL(TInt aType)
{
    if (iPopupForm)
    {
        iPopupForm->OfferResourceChangeL(aType, iAnimation);
    }
}


// ---------------------------------------------------------------------------
// CSwtMessageBox::CSwtMessageBox
// ---------------------------------------------------------------------------
//
CSwtMessageBox::CSwtMessageBox(MSwtDisplay& aDisplay, TInt aStyle)
        : iDisplay(aDisplay)
        , iStyle(aStyle)
{
    // We support SWT.APPLICATION_MODAL only
    ASSERT((aStyle & KSwtModalityMask) == KSwtStyleApplicationModal);
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMessageBox::ConstructL(const TDesC& aTitle, const TDesC& aMessage)
{
    iAnimation = Animation();
    iPopupForm = CSwtPopupFormProxy::NewL(aMessage, aTitle, R_SWT_MSGBX_HEADING_LINE,
                                          R_SWT_MSGBX_SPACER_LINE, R_SWT_MSGBX, Button(), iAnimation, iStyle,
                                          R_SWT_MSGBX_MENUBAR, ESwtCommandYes, ESwtCommandNo);
    iDisplay.AddResourceChangeObserverL(this);
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::ConvertButtonId
// ---------------------------------------------------------------------------
//
TInt CSwtMessageBox::ConvertButtonId(TInt aButtonId) const
{
    TInt buttonConverted = 0;
    switch (aButtonId)
    {
    case EAknSoftkeyOk:
        buttonConverted = KSwtMsgBoxStyleOk;
        break;
    case ESwtCommandYes:
        // Fall trough
    case EAknSoftkeyYes:
        buttonConverted = KSwtMsgBoxStyleYes;
        break;
    case ESwtCommandNo:
        // Fall trough
    case EAknSoftkeyNo:
        buttonConverted = KSwtMsgBoxStyleNo;
        break;
    case EAknSoftkeyCancel:
        // Fall trough
    case EEikCmdCanceled:
        buttonConverted = KSwtMsgBoxStyleCancel;
        break;
    case EAknSoftkeyAgain:
        buttonConverted = KSwtMsgBoxStyleRetry;
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    return buttonConverted;
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::Animation
// ---------------------------------------------------------------------------
//
TInt CSwtMessageBox::Animation() const
{
    if (iStyle & KSwtMsgBoxStyleInformation)
    {
        return R_SWT_MSGBX_INFO_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleError)
    {
        return R_SWT_MSGBX_ERROR_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleWarning)
    {
        return R_SWT_MSGBX_WARNING_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleQuestion)
    {
        return R_SWT_MSGBX_QUESTION_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleWorking)
    {
        return R_SWT_MSGBX_WORKING_ANIM;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// CSwtMessageBox::Button
// ---------------------------------------------------------------------------
//
TInt CSwtMessageBox::Button() const
{
    switch (SwtMsgBoxButtons(iStyle))
    {
    case ESwtMsgBoxBtnOk:
        return R_AVKON_SOFTKEYS_OK_EMPTY;
    case ESwtMsgBoxBtnOkCancel:
        return R_AVKON_SOFTKEYS_OK_CANCEL;
    case ESwtMsgBoxBtnYesNo:
        return R_AVKON_SOFTKEYS_YES_NO;
    case ESwtMsgBoxBtnYesNoCancel:
        return R_AVKON_SOFTKEYS_OPTIONS_CANCEL;
    case ESwtMsgBoxBtnRetryCancel:
        return R_SWT_SOFTKEYS_AGAIN_CANCEL;
    default:
        ASSERT(EFalse);
        break;
    }
    return R_AVKON_SOFTKEYS_OK_EMPTY;
}
