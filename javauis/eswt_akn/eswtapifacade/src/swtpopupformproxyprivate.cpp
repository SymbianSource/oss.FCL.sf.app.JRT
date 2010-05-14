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


#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#include "swtpopupformproxyprivate.h"


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSwtPopupFormProxyPrivate::CSwtPopupFormProxyPrivate(const TTone& aTone,
        const TInt aMsgBoxMenuBarResourceId,
        const TInt aSwtCmdYes, const TInt aSwtCmdNo, const TInt aTimeOut)
        : CAknPopupForm(aTone, 0, EAknNote)
        , iMsgBoxMenuBarResourceId(aMsgBoxMenuBarResourceId)
        , iSwtCommandYes(aSwtCmdYes)
        , iSwtCommandNo(aSwtCmdNo)
        , iTimeOut(aTimeOut)
{
}

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSwtPopupFormProxyPrivate* CSwtPopupFormProxyPrivate::NewL(const TTone& aTone,
        const TInt aMsgBoxMenuBarResourceId,
        const TInt aSwtCmdYes, const TInt aSwtCmdNo, const TInt aTimeOut)
{
    CSwtPopupFormProxyPrivate* self = new(ELeave) CSwtPopupFormProxyPrivate(aTone,
            aMsgBoxMenuBarResourceId, aSwtCmdYes, aSwtCmdNo, aTimeOut);
    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

CSwtPopupFormProxyPrivate::~CSwtPopupFormProxyPrivate()
{
}


// ---------------------------------------------------------------------------
// CSwtPopupFormProxyPrivate::RunLD
// From CEikDialog
// ---------------------------------------------------------------------------
//
TInt CSwtPopupFormProxyPrivate::RunLD()
{
    CAknPopupForm::LayoutAndDraw();
    return  CAknPopupForm::RunLD();
}

// ---------------------------------------------------------------------------
// CSwtPopupFormProxyPrivate::MappedCommandId
// From CEikDialog
// ---------------------------------------------------------------------------
//
TInt CSwtPopupFormProxyPrivate::MappedCommandId(TInt aButtonId)
{
    // This innocent looking function overrides the platform default behaviour
    // which is to replace No commands with Cancel commands
    return aButtonId;
}

// ---------------------------------------------------------------------------
// CSwtPopupFormProxyPrivate::OkToExitL
// From CAknDialog
// ---------------------------------------------------------------------------
//
TBool CSwtPopupFormProxyPrivate::OkToExitL(TInt aButtonId)
{
    if (aButtonId == EAknSoftkeyOptions)
    {
        iMenuBar->SetMenuTitleResourceId(iMsgBoxMenuBarResourceId);
        DisplayMenuL();
        return EFalse;
    }
    else
    {
        return ETrue;
    }
}


// ---------------------------------------------------------------------------
// CSwtPopupFormProxyPrivate::ProcessCommandL
// From MEikCommandObserver
// ---------------------------------------------------------------------------
//
void CSwtPopupFormProxyPrivate::ProcessCommandL(TInt aCommandId)
{
    if (iTimeOut)
    {
        // if timed messsagebox
        return CAknPopupForm::ProcessCommandL(aCommandId);
    }

    // if not a timed messagebox
    HideMenu();
    if ((aCommandId == iSwtCommandYes) || (aCommandId == iSwtCommandNo))
    {
        TryExitL(aCommandId);
    }
}


// ---------------------------------------------------------------------------
// CSwtPopupFormProxyPrivate::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtPopupFormProxyPrivate::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    if (!iTimeOut)
    {
        // Other than TimedMessageBox
        TKeyResponse res = CAknPopupForm::OfferKeyEventL(aKeyEvent, aType);
        if (aType == EEventKey && res == EKeyWasNotConsumed)
        {
            if (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK)
            {
                // Trigger the positive CBA action / soft key
                TKeyEvent ev = {EKeyCBA1, EStdKeyDevice0, 0, 0};
                TRAP_IGNORE(CCoeEnv::Static()->SimulateKeyEventL(ev, EEventKey));
            }
        }
    }
    else
    {
        // TimedMessageBox
        if (aType == EEventKey)
        {
            CAknPopupForm::StaticDeleteL(this);
        }
        else
        {
            CAknPopupForm::OfferKeyEventL(aKeyEvent, aType);
        }
    }

    // Keys cannot be allowed to "pass trough" the dialog.
    return EKeyWasConsumed;
}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CSwtPopupFormProxyPrivate::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtPopupFormProxyPrivate::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
{
    if (!iTimeOut)
    {
        // if not a timed messagebox
        return CAknPopupForm::HandlePointerEventL(aPointerEvent);
    }
    // if timed messagebox
    if (AknLayoutUtils::PenEnabled())
    {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        //Closing timed message box by tapping:
        //there should be feedback on touch down
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            MTouchFeedback *feedback = MTouchFeedback::Instance();
            if (feedback)
            {
                feedback->InstantFeedback(ETouchFeedbackPopUp);
            }
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        CCoeControl::HandlePointerEventL(aPointerEvent);
        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            CAknPopupForm::StaticDeleteL(this);
        }
    }
}
#endif // RD_SCALABLE_UI_V2

