/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMIDChoiceGroupItem implements the MID Form ChoiceGroup item
*
*/


#include <avkon.mbg>
#include <j2me/jdebug.h>

#include "CMIDChoiceGroupItem.h"
#include "CMIDChoiceGroupControl.h"
// API for iLabelControl
#include "CMIDItemLabel.h"
#include "CMIDCommandList.h"
#include "CMIDCommand.h"
#include "CMIDPopupNoteController.h"
// needed API for CMIDDisplayable and CMIDCommandList in UpdateCommands function
#include "CMIDDisplayable.h"

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDChoiceGroupItem.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


// class CMIDChoiceGroupItem

CMIDChoiceGroupItem* CMIDChoiceGroupItem::NewL(
    MMIDEnv& aMIDEnv, const TDesC& aLabel, TInt aType,
    RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray, CMIDUIManager* aUIManager)
{
    CMIDChoiceGroupItem* self = new(ELeave) CMIDChoiceGroupItem(
        aMIDEnv, aType, aStringArray, aImageArray, aUIManager);
    CleanupStack::PushL(self);
    self->ConstructL(aLabel);
    CleanupStack::Pop(self);
    return self;
}


CMIDChoiceGroupItem::CMIDChoiceGroupItem(
    MMIDEnv& aMIDEnv,
    TInt aType,
    RArray<TPtrC>& aStringArray,
    RArray<MMIDImage*>& aImageArray,
    CMIDUIManager* aUIManager)
        : CMIDControlItem(MMIDItem::EDefault, aUIManager),
        iChoiceType((MMIDChoiceGroup::TChoiceType) aType),
        iFitPolicy(EWrapDefault),
        iMIDEnv(aMIDEnv),
        iStringArray(aStringArray),
        iImageArray(aImageArray)
{
    // Set this to be a MMIDItem interface implementor
    iMMidItem = this;

    // This is a focusing item
    SetFocusing(ETrue);
}

CMIDChoiceGroupItem::~CMIDChoiceGroupItem()
{
    delete iChoiceGroupControl;
    if (iSelectCommand)
    {
        iSelectCommand->SetObserver(NULL);
        iSelectCommand->Dispose();
        iSelectCommand = NULL;
    }
    if (iMarkCommand)
    {
        iMarkCommand->SetObserver(NULL);
        iMarkCommand->Dispose();
        iMarkCommand = NULL;
    }
    if (iUnmarkCommand)
    {
        iUnmarkCommand->SetObserver(NULL);
        iUnmarkCommand->Dispose();
        iUnmarkCommand = NULL;
    }
}

void CMIDChoiceGroupItem::ConstructL(const TDesC& aLabel)
{
    // Allow parent to construct parts that are common
    CMIDControlItem::ConstructL();

    // Create built-in commands to makes selections in the list
    HBufC* label = NULL;
    if (iChoiceType == EPopup)
    {
        label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_CHANGE);
    }
    else
    {
        label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
    }
    CleanupStack::PushL(label);
    iSelectCommand = CMIDCommand::NewBuiltInCommandL(label->Des(),
                     CMIDCommand::EItem, CMIDCommand::EChoiceGroupSelectCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the string
    label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_MARK);
    CleanupStack::PushL(label);
    iMarkCommand = CMIDCommand::NewBuiltInCommandL(label->Des(),
                   CMIDCommand::EItem, CMIDCommand::EChoiceGroupMarkCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the string
    label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_UNMARK);
    CleanupStack::PushL(label);
    iUnmarkCommand = CMIDCommand::NewBuiltInCommandL(label->Des(),
                     CMIDCommand::EItem, CMIDCommand::EChoiceGroupUnmarkCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the string
    iSelectCommand->SetObserver(this);
    iMarkCommand->SetObserver(this);
    iUnmarkCommand->SetObserver(this);
    if (iChoiceType == EPopup)
    { // in popup the command is always available
        SetBuiltInMSKCommand(iSelectCommand);
    }

    // Parent created label control, set label in it
    SetLabelL(aLabel);
    iLabelControl->SetWidthL(FormClientAreaWidth());

    // Create the choicegroup control to hold the items
    // passed in. The actual control is created when the
    // choicegroup item is added to a form, otherwise it won't
    // have a parent window, and the listbox doesn't like that
    iChoiceGroupControl = new(ELeave) CMIDChoiceGroupControl(iChoiceType, this);
    iChoiceGroupControl->ConstructL(iStringArray, iImageArray);

    // Spy on control
    iChoiceGroupControl->SetObserver(this);

#ifdef RD_SCALABLE_UI_V2
    SetAllowStrayPointers();
#endif

    // This control is ready
    ActivateL();
}

void CMIDChoiceGroupItem::UpdateCommands()
{
    // MSK
    // This method must be called when ever the current list item or list selection changes.
    // It sets the command such that it tells the user what can be done next. E.g. in exlusive
    // list if current item is selected, then we enable context sensitive menu by setting
    // MSK command to NULL. In multiple lists, the command label changes depending on the
    // selected state of the current list item.
    CMIDCommand* cmd = iSelectCommand;
    if (iChoiceType == EExclusive)
    {
        TInt numberOfOkMenuCmds(0);
        if (iForm)
        { // check for number of context menu commands
            CMIDDisplayable& displayable = iForm->CurrentDisplayable();
            numberOfOkMenuCmds = displayable.NumCommandsForOkOptionsMenu();
        }
        // If item is selected and there are commands for context menu
        // then context menu should be displayed - set MSK command to NULL.
        // Since number of context menu commands queried from CMIDDisplayable
        // may not be synchronized with iCommandList at this stage, we also
        // check the number of commands in iCommandList.
        if (iChoiceGroupControl->IsCurrentItemSelected() &&
                (DefaultCommand() || numberOfOkMenuCmds > 0 ||
                 iCommandList->Count() > 0))
        {
            cmd = NULL;
        }
    }
    else if (iChoiceType == EMultiple)
    {
        cmd = iChoiceGroupControl->IsCurrentItemSelected() ? iUnmarkCommand : iMarkCommand;
    }

    // If choice group is empty, MSK command is set to null,
    // except in popup case "change" command is always shown.
    if (iChoiceType != EPopup && iChoiceGroupControl->NumberOfElements() == 0)
    {
        cmd = NULL;
    }

    SetBuiltInMSKCommand(cmd);
#ifndef RD_JAVA_S60_RELEASE_9_2
    iChoiceGroupControl->ShowInfoPopup();
#endif // RD_JAVA_S60_RELEASE_9_2
}

// msk
TBool CMIDChoiceGroupItem::ProcessCommandL(CMIDCommand* /*aCommand*/)
{
    if (iChoiceType == EPopup)
    {
        iChoiceGroupControl->DoPopupL();
    }
    else // for multiple and exclusive
    {
        // toggle selection
        iChoiceGroupControl->ToggleCurrentItemSelectionL();
    }
    return ETrue;
}


// Updates the commands when the form that the item is on becomes the current.
// After LAF changes: If the form is not the current displayable anymore, closes the popup
void CMIDChoiceGroupItem::HandleCurrentL(TBool aCurrent)
{
    if (!aCurrent && iChoiceGroupControl && iChoiceType == MMIDChoiceGroup::EPopup)
    {
        iChoiceGroupControl->ClosePopup();
    }
    if (aCurrent)
    {
        UpdateCommands();
    }
}


// Inserts a new element in the choice
void CMIDChoiceGroupItem::InsertElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->InsertElementL(aIndex, aText, aImage);
    UpdateCommands();
}

// Deletes an element from the choice
void CMIDChoiceGroupItem::DeleteElementL(TInt aIndex)
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->DeleteElementL(aIndex);
    UpdateCommands();
}

// Deletes all elements
void CMIDChoiceGroupItem::DeleteAllL()
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->DeleteAllL();
    UpdateCommands();
}

// Sets properties of element at <aIndex>
void CMIDChoiceGroupItem::SetElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->SetElementL(aIndex, aText, aImage);
}

// Sets selection state of element at <aIndex>
void CMIDChoiceGroupItem::SelectElementL(TInt aIndex, TBool aSelected)
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->SelectElementL(aIndex, aSelected);
}

// Returns selection state of element at <aIndex>
TBool CMIDChoiceGroupItem::IsSelected(TInt aIndex)
{
    ASSERT(iChoiceGroupControl);
    return iChoiceGroupControl->IsSelected(aIndex);
}

// Sets element text font
void CMIDChoiceGroupItem::SetFontL(TInt aIndex, MMIDFont* aFont)
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->SetFontL(aIndex, aFont);
}

// Sets element text fit policy
void CMIDChoiceGroupItem::SetFitPolicyL(TInt aFitPolicy)
{
    ASSERT(iChoiceGroupControl);
    iChoiceGroupControl->SetFitPolicyL(aFitPolicy);
}

// --- from MMIDItem ---


// --- from CCoeControl ---

// Set the containing window
void CMIDChoiceGroupItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    // Set this controls parent
    CMIDControlItem::SetContainerWindowL(aContainer);
    SetObserver(iForm);
}


// Count component controls in this compound control
TInt CMIDChoiceGroupItem::CountComponentControls() const
{
    TInt count = 0;
    CCoeControl* controls[] = { iLabelControl, iChoiceGroupControl };
    for (TUint ii = 0; ii < sizeof(controls) / sizeof(CCoeControl*); ii++)
    {
        if (controls[ii])
        {
            count++;
        }
    }
    return count;
}


// Return requested component control
CCoeControl* CMIDChoiceGroupItem::ComponentControl(TInt aIndex) const
{
    CCoeControl* controls[] = { iLabelControl, iChoiceGroupControl};

    for (TUint ii=0; ii < sizeof(controls) / sizeof(CCoeControl*); ii++)
    {
        // Index is decremented only if control is not NULL
        if ((controls[ii]) && (aIndex-- == 0))
        {
            return controls[ii];
        }
    }
    return NULL;
}


// Calculate size and layout
void CMIDChoiceGroupItem::SizeChanged()
{
    TRect labelRect;
    TRect choiceRect;

    CalcChildSizes(Rect(), labelRect, choiceRect);

    if (iLabelControl)
    {
        iLabelControl->SetRect(labelRect);
    }

    if (iChoiceGroupControl)
    {
        iChoiceGroupControl->SetRect(choiceRect);
    }

    CMIDControlItem::SizeChanged();
}


// Calculates and returns the minimum size of this item
TSize CMIDChoiceGroupItem::MinimumSize()
{
    ASSERT(iChoiceGroupControl);

    TInt height = LabelHeight();

    if (height == 0 && iChoiceGroupControl->NumberOfElements() == 0)
    {
        return TSize(0, 0);
    }
    else
    {
        height += iChoiceGroupControl->MinimumSize().iHeight;
        return TSize(FormClientAreaWidth(), height);
    }
}


// Offers key events to the choicegroup control.
TKeyResponse CMIDChoiceGroupItem::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    // For now, just offer key events to choice control
    TKeyResponse resp = EKeyWasNotConsumed;

    if (iChoiceGroupControl)
    {
        resp = iChoiceGroupControl->OfferKeyEventL(aKeyEvent, aType);
    }

    return resp;
}

// Responds to focus change.
// From CCoeControl.
void CMIDChoiceGroupItem::FocusChanged(TDrawNow aDrawNow)
{
    CMIDControlItem::FocusChanged(aDrawNow);

    // update focus state of choice group control regarding to current focus state
    if (iChoiceGroupControl)
    {
        iChoiceGroupControl->SetFocus(IsFocused(), aDrawNow);
    }
}


#ifdef RD_SCALABLE_UI_V2
void CMIDChoiceGroupItem::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (!AknLayoutUtils::PenEnabled())
    {
        return;
    }
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        iLongTapDetected = EFalse;
    }
    iLongTapDetected = iForm->TryDetectLongTapL(aPointerEvent);
#else
    if (!AknLayoutUtils::PenEnabled() || iForm->TryDetectLongTapL(aPointerEvent))
    {
        return;
    }
#endif // RD_JAVA_S60_RELEASE_9_2    

    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EDrag:
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (iLongTapDetected)
        {
            return;
        }
#endif // RD_JAVA_S60_RELEASE_9_2

        if (aPointerEvent.iPosition.iY >= 0 &&
                aPointerEvent.iPosition.iY < FormRect().Height() &&
                iChoiceGroupControl->Rect().Contains(aPointerEvent.iPosition))
        {
#ifdef RD_JAVA_S60_RELEASE_9_2
            // Drag events inside ChoiceGroup area are forwarded to the control.
            iChoiceGroupControl->HandlePointerEventL(aPointerEvent);
#else
            if (iForm->PhysicsEnabled())
            {
            }
            else
            {
                // Forward drag events directly to the control (i.e. to listbox). This enables
                // the moving of the highlight inside list box when foucused item is changed
                // by dragging on the form. Calling CCoeControl::HandlePointerEventL() forwards the
                // drag events to grabbing component only and setting the grabbing component properly
                // is not always possible. See CCoeControl implementation.
                iChoiceGroupControl->HandlePointerEventL(aPointerEvent);

            }
#endif // RD_JAVA_S60_RELEASE_9_2

        }
        break;
    }

    case TPointerEvent::EButton1Up:
    {
        if (iChoiceType == MMIDChoiceGroup::EPopup &&
                !iForm->IsScrolledOnPointerDown() &&
#ifdef RD_JAVA_S60_RELEASE_9_2
                !iLongTapDetected &&
#else
                // In non-single click UI style, item must be focused
                // before opening pop-up choicegroup.
                !iForm->IsFocusChangingWithPen() &&
#endif // RD_JAVA_S60_RELEASE_9_2                 
                !iForm->PhysicsScrolling())
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            MTouchFeedback* feedback = MTouchFeedback::Instance();
            if (feedback)
            {
                feedback->InstantFeedback(ETouchFeedbackList);
            }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iChoiceGroupControl->DoPopupL();
        }
#ifdef RD_JAVA_S60_RELEASE_9_2
        // Forward pointer event except if long tap has been detected,
        // because we want to keep highlight visible.
        if (iLongTapDetected)
        {
            iPendingUpEvent = aPointerEvent;
        }
        else
        {
            CMIDControlItem::HandlePointerEventL(aPointerEvent);
        }
#else
        if (iForm->PhysicsEnabled())
        {
            if (!iForm->PhysicsScrolling())
            {
                CMIDControlItem::HandlePointerEventL(aPointerEvent);
            }
        }
        else
        {
            CMIDControlItem::HandlePointerEventL(aPointerEvent);
        }
#endif // RD_JAVA_S60_RELEASE_9_2
        break;
    }

    default:
        CMIDControlItem::HandlePointerEventL(aPointerEvent);
        break;
    }

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        // highlight should be disabled when choicegroup taped
        iChoiceGroupControl->SingleClickDisableHighlightL(ETrue);
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}
#endif //RD_SCALABLE_UI_V2

// --- from CMIDControlItem

// Returns a rect that should be partially visible for an Item to have focus
// (ie the choicegroup control area)
TRect CMIDChoiceGroupItem::FocusableRect()
{
    if (iChoiceGroupControl)
    {
        return iChoiceGroupControl->Rect();
    }
    else
    {
        // Just return the items rect.
        return Rect();
    }
}

void CMIDChoiceGroupItem::RestoreInnerFocus()
{
    if (iChoiceGroupControl)
        iChoiceGroupControl->RestoreFocus();
}

// --- from MMIDChoiceGroupControlObserver

// Called when the model has changed. Based on the change, will cause
// just a redraw of the control, or size recalculation
void CMIDChoiceGroupItem::HandleChoiceGroupControlEventL(
    CMIDChoiceGroupControl* aControl,
    TChoiceGroupControlEvent aEvent,
    TAny* aParam)
{
    ASSERT(iChoiceGroupControl);

    // If not from our control, ignore
    if (aControl != iChoiceGroupControl)
    {
        return;
    }

    switch (aEvent)
    {
    case ESizeChanged:
        break;

    case EScrollRequest:
        // Make form scroll the requested amount and redraw
        if (iForm)
        {
            iForm->ScrollRelative((TInt) aParam);
            iForm->UpdatePhysics();
        }
        break;

    default:
        break;
    }
}


// Need to construct the choice listbox when adding to a form
// (the listbox very much likes to have a parent window when created)
void CMIDChoiceGroupItem::ItemAddedToFormL()
{
    // Form must've been set before
    ASSERT(iForm);

    // NOTE: The form calls SetContainerWindow() only after
    // calling SetForm(). That's why the form and not this is
    // passed as parent.

    // Create a choice group control here, so that we can
    // give a parent window to the listbox
    CreateChoiceGroupControlL(iForm);

    iChoiceGroupControl->EnableListHighlight(EFalse);
}


// Discard the listbox control when removed from a form
void CMIDChoiceGroupItem::ItemRemovedFromForm()
{
    // Get rid of the actual control, but keep the
    // choicegroup control around, as it holds the model with
    // all the added elements and can be reattached to a form
    if (iChoiceGroupControl)
    {
        iChoiceGroupControl->DeleteControl();
    }
}

void CMIDChoiceGroupItem::NotifyScrollingCompleted()
{
    // When this cg becomes focused, show info popup after the form scrolling is completed
    // and new position has been set to this item. If the note was shown before the new position
    // is set, the note might be in wrong position (or might be hidden by form, see
    // CMIDForm::UpdateScrollBar and CMICForm::RawScrollFinalize).
    if (IsFocused())
    {
        iChoiceGroupControl->ShowInfoPopup();
    }
}

void CMIDChoiceGroupItem::PostItemStateChangedEventL()
{
    ReportEventL(MCoeControlObserver::EEventStateChanged);
}

#ifdef RD_JAVA_S60_RELEASE_9_2
void CMIDChoiceGroupItem::PostPendingUpEventL()
{
    if (iLongTapDetected)
    {
        iChoiceGroupControl->HandlePointerEventL(iPendingUpEvent);
        iLongTapDetected = EFalse;
    }
}

TBool CMIDChoiceGroupItem::LongTapDetected()
{
    return iLongTapDetected;
}
#endif // RD_JAVA_S60_RELEASE_9_2

void CMIDChoiceGroupItem::PostFocusTransferEvent(TBool aFocus, CMIDForm::TDirection aDirection)
{
    if (aFocus)
    {
        if (aDirection == CMIDForm::EDown || aDirection == CMIDForm::ERight ||
                aDirection == CMIDForm::ENone)
        {
            iChoiceGroupControl->HiliteFirstElement(ETrue);
        }
        else
        {
            iChoiceGroupControl->HiliteLastElement(ETrue);
        }
        UpdateCommands();
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (aDirection != CMIDForm::ENone)
        {
            iChoiceGroupControl->ShowInfoPopup();
        }
#endif // RD_JAVA_S60_RELEASE_9_2        
    }
    else
    {
        iChoiceGroupControl->EnableListHighlight(EFalse);
    }
}


// Return the items preferred height without the label part
// (the "payload" size)
TInt CMIDChoiceGroupItem::ItemPreferredHeightWithoutLabel()
{
    return iChoiceGroupControl ? iChoiceGroupControl->MinimumSize().iHeight : 0;
}


void CMIDChoiceGroupItem::CreateChoiceGroupControlL(CCoeControl* aParent)
{
    // Create an actual choice control
    if (iChoiceGroupControl)
    {
        // Create one
        TRect labelRect;
        TRect choiceRect;
        CalcChildSizes(Rect(), labelRect, choiceRect);

        iChoiceGroupControl->CreateControlL(aParent, choiceRect);
    }
}


//  Calculates child control sizes
void CMIDChoiceGroupItem::CalcChildSizes(
    const TRect& aItemRect,
    TRect& aLabelRect,
    TRect& aChoiceRect)
{
    aLabelRect.SetRect(aItemRect.iTl, TSize(aItemRect.Width(), LabelHeight()));

    aChoiceRect = aItemRect;
    aChoiceRect.iTl.iY += aLabelRect.Height();
}

/** */
void CMIDChoiceGroupItem::ResolutionChange(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch && iForm)
    { // dynamic resolution change
        TRAP_IGNORE(CreateChoiceGroupControlL(iForm));
        // After the listbox is recreated it shows the focus indication by default.
        // Clear it if this item is not the one having focus on the form.
        if (iForm->CurrentItem() != this)
        {
            iChoiceGroupControl->EnableListHighlight(EFalse);
        }
    }

    // warn choice group control of layout change
    if (aType == KEikDynamicLayoutVariantSwitch && iChoiceGroupControl)
    {
        iChoiceGroupControl->HandleResourceChange(aType);
    }
}

void CMIDChoiceGroupItem::ColorChange(TInt /*aType*/)
{
    if (iForm)
    {
        TRAP_IGNORE(CreateChoiceGroupControlL(iForm));
        // After the listbox is recreated it shows the focus indication by default.
        // Clear it if this item is not the one having focus on the form.
        if (iForm->CurrentItem() != this)
        {
            iChoiceGroupControl->EnableListHighlight(EFalse);
        }
    }
}

void CMIDChoiceGroupItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
}

void CMIDChoiceGroupItem::SetLayoutL(TLayout aLayout)
{
    CMIDControlItem::SetLayoutL(aLayout);
}

void CMIDChoiceGroupItem::Dispose()
{
    delete this;
}

void CMIDChoiceGroupItem::AddCommandL(MMIDCommand* aCommand)
{
    // Add command to item, don't notify form twice
    CMIDControlItem::AddCommandL(aCommand, EFalse);
    // UpdateCommands() calls  form notification
    UpdateCommands();
}

void CMIDChoiceGroupItem::RemoveCommand(MMIDCommand* aCommand)
{
    // Remove command to item, don't notify form twice
    CMIDControlItem::RemoveCommand(aCommand, EFalse);
    // UpdateCommands() calls  form notification
    UpdateCommands();
}

void CMIDChoiceGroupItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    CMIDControlItem::SetDefaultCommand(aCommand);
}

void CMIDChoiceGroupItem::SetPreferredSizeL(const TSize& aSize)
{
    CMIDControlItem::SetPreferredSize(aSize, MinimumSize());
}

TSize CMIDChoiceGroupItem::PreferredSize() const
{
    return ResetPreferredSize();
}

TSize CMIDChoiceGroupItem::MinimumSize() const
{
    // Const cast to make this call the CCoeControl overridden MinimumSize()
    CCoeControl* control = const_cast<CMIDChoiceGroupItem *>(this);
    return control->MinimumSize();
}

void CMIDChoiceGroupItem::SafeDrawNow()
{
    if (DrawableWindow())
    {
        DrawNow();
    }
}

TRect CMIDChoiceGroupItem::FormRect()
{
    if (iForm)
    {
        return iForm->Rect();
    }
    else
    {
        return (TRect(0, 0, 0, 0));
    }
}

#ifdef RD_JAVA_S60_RELEASE_9_2
void CMIDChoiceGroupItem::SetHighlight(const TBool aHighlight)
{
    CMIDControlItem::SetHighlight(aHighlight);
    if (iChoiceGroupControl
            && iChoiceGroupControl->ChoiceType() != MMIDChoiceGroup::EPopup
            && iChoiceGroupControl->InnerListBoxControl()
            && iChoiceGroupControl->InnerListBoxModel())
    {
        // Try to update icons (due to color change), if it leaves
        // do nothing
        TRAP_IGNORE(iChoiceGroupControl->InnerListBoxModel()->UpdateIconArrayL(aHighlight));
        iChoiceGroupControl->InnerListBoxControl()->UpdateColors();
    }
}
#endif // RD_JAVA_S60_RELEASE_9_2  


// End of File
