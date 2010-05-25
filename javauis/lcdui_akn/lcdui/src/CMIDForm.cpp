/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements the Form LCDUI component.
*
*/


//RD_SCALABLE_UI definition
#include <bldvariant.hrh>
// using CEikScrollBarFrame API for iSBFrame
#include <eiksbfrm.h>

// API used for retrieving layout for initial alignment (in ConstructL)
#include <centralrepository.h>
// constants used in retrieving layout for initial alignment (in ConstructL)
#include <AvkonInternalCRKeys.h>

// control context that provides a layout background
#include <AknsListBoxBackgroundControlContext.h>
// highlight background
#include <AknsFrameBackgroundControlContext.h>
// API for text formatting used in DrawNoDataStringL function
#include <AknBidiTextUtils.h>

#include "CMIDForm.h"
#include "CMIDFormRow.h"
// API for iDisplayable
#include "CMIDDisplayable.h"
#include "CMIDItem.h"
#include "CMIDControlItem.h"
#include "CMIDGaugeItem.h"
#include "CMIDImageItem.h"
#include "CMIDStringItem.h"
// CMIDLabelContainerItem API used to obtain CMIDStringItem item
#include "CMIDLabelContainerItem.h"
#include "CMIDTextFieldItem.h"
#include "CMIDChoiceGroupItem.h"
#include "CMIDSpacer.h"
#include "CMIDCustomItem.h"
#include "CMIDDateFieldItem.h"
#include "CMIDComponentFactory.h"
#include "CMIDUtils.h"
#include "CMIDItemLabel.h"
#include "CMIDTicker.h"
#include "CMIDFont.h"
#include "CMIDPopupNoteController.h"
// Kinetic scrolling
#include "CMIDFormPhysics.h"

#include <applayout.cdl.h>
// LAF
#include <aknlayoutscalable_avkon.cdl.h>
// LAF
#include <layoutmetadata.cdl.h>

// Api for skin layout
#include <skinlayout.cdl.h>
using namespace SkinLayout;

#include <j2me/jdebug.h>

#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDForm.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

#ifdef RD_SCALABLE_UI_V2
/** Dividers used when the the scroll amount is calculated*/
const TInt KScrollAmountDivider = 4;
const TInt KScrollAmountDividerForThumbDrag = 10;

/** Interval for scrolling when stylus is dragged outside the form and held down */
const TInt KScrollInterval = 100000; // 0,1 sec
#endif //RD_SCALABLE_UI_V2

/** The minimum amount to scroll */
const TInt KMinScrollAmount = 40;

/** The timeout in microseconds for visual updates. When items are added or removed
or modified, the form is not redrawn immediately but only after this timeout. This way
if more items are being added or removed or changed we reduce the number of redraws.
*/
const TInt KLayoutTimeout = 60000; //60 msecs

/** If layout has been requested for a period longer than the cap then perform layout
even if the midlet is still performing form insert/delete operations. */
const TInt KLayoutCap = 1000000; // 1 sec

///////////////////////////////////////////////////////////////////////////////////////////////////
// MMIDForm interface
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Add items list. Note: this is no longer called by framework!!
 */
void CMIDForm::SetAllItemsL(const RArray<MMIDItem*>& aItems)
{
    DEBUG("CMIDForm::SetAllItemsL");

    ASSERT(iItems.Count() == 0);
    //
    TInt count = aItems.Count();
    for (TInt i=0; i<count; i++)
    {
        User::LeaveIfError(iItems.Append(aItems[i]));
        CMIDControlItem& ci = ControlItem(i);

        AddItemToFormL(ci);
        ci.MakeVisible(ETrue);
    }

    RequestLayoutL();
}

/**
 * Replaces an item. Keeps the same item focused which had focus before,
 * and if that's the one that this is replacing, then this will be focused
 * if possible.
 */
void CMIDForm::SetItemL(MMIDItem& aItem,TInt aIndex)
{
    DEBUG("CMIDForm::SetItemL");

    CMIDControlItem& oldCi = ControlItem(aIndex);
    RemoveItemFromForm(oldCi);

    iItems[aIndex] = &aItem;
    CMIDControlItem& ci = ControlItem(aIndex);
    AddItemToFormL(ci);

    ci.MakeVisible(ETrue);

    if (iFocused == aIndex)
    {
        iDisplayable.MenuHandler()->HideMenuIfVisible();

        if (ci.IsSelectable())
        {
            SetFocusedItem(aIndex);
        }
        else
        {
            SetFocusedItem(KErrNotFound);
        }
    }

    RequestLayoutL();
}

/**
 * Inserts a new Item.
 */
void CMIDForm::InsertItemL(MMIDItem& aItem,TInt aIndex)
{
    DEBUG("CMIDForm::InsertItemL");

    User::LeaveIfError(iItems.Insert(&aItem,aIndex));
    CMIDControlItem& ci = ControlItem(aIndex);

    AddItemToFormL(ci);
    ci.MakeVisible(ETrue);
    // if insert item then index of pointed control is increase
    if (aIndex <= iIndexPointedControl)
    {
        iIndexPointedControl++;
    }

    // If there isn't any focused item yet, we have to set focus
    // to first item which is focusable. Once the item has focus,
    // we can skip this step.
    if (iFocused == KErrNotFound)
    {
        SetFocusedItem(ci.IsSelectable() ? aIndex : KErrNotFound);
    }

    RequestLayoutL();
}

void CMIDForm::DeleteItemL(TInt aIndex)
{
    DEBUG("CMIDForm::DeleteItemL");

    TBool updateFocused = EFalse;
    // if delete item then index of pointed control is decrease
    if (iIndexPointedControl > aIndex)
    {
        iIndexPointedControl--;
    }
    else
        // if delete item is same as index of pointed control then index is set to not active
    {
        if (iIndexPointedControl == aIndex)
        {
            iIndexPointedControl = -1;
        }
    }
    if (iFocused > aIndex)
    {
        iFocused--;
    }
    else if (iFocused == aIndex)
    {
        iDisplayable.MenuHandler()->HideMenuIfVisible();
        ReplaceFocusedItem();
        updateFocused = ETrue;
    }

    CMIDControlItem& ci = ControlItem(aIndex);
    RemoveItemFromForm(ci);
    if (&ci == iPointedControl)
    {
        iPointedControl = NULL;
    }

    iItems.Remove(aIndex);

    if (iFocused != KErrNotFound && updateFocused)
    {
        iFocused--;
    }

    RequestLayoutL();
}

void CMIDForm::DeleteAllItemsL()
{
    DEBUG("CMIDForm::DeleteAllItemsL");
    iDisplayable.MenuHandler()->HideMenuIfVisible();
    SetFocusedItem(KErrNotFound);

    TInt itemCount = iItems.Count();
    for (TInt i=0; i < itemCount; i++)
    {
        CMIDControlItem& ci = ControlItem(i);
        RemoveItemFromForm(ci);
    }
    iItems.Reset();

    iScroll = 0;
    // if delete all then index of pointed control is set to not active
    iIndexPointedControl = -1;

    iPointedControl = NULL;
    RequestLayoutL();
}

/**
 * Called when the item has changed its appearance in some way.
 */
void CMIDForm::RefreshItemL(TInt aIndex)
{
    DEBUG("CMIDForm::RefreshItemL");

    if (aIndex != KErrNotFound)
    {
        RequestLayoutL();
    }
}

/**
 * Returns whether the item is actually visible to the user. Must therefore take into
 * account the visibility of the entire form, and whether the specific item is scrolled
 * in/out of the viewable area.
 */
TBool CMIDForm::IsItemVisible(TInt aIndex)
{
    DEBUG("CMIDForm::IsItemVisible");

    CMIDControlItem& control = ControlItem(aIndex);

    if ((control.Position().iY > Height()) ||
            ((control.Position().iY + control.Size().iHeight) < 0))
    {
        return EFalse;
    }
    return ETrue;
}

/**
 * Called prior to the Form itself being made the current Displayable.
 * It provides an opportunity to prepare the form such that this item will
 * be visible and focused when the form is eventually made current
 */
void CMIDForm::SetCurrentItemL(TInt aIndex)
{
    DEBUG("CMIDForm::SetCurrentItemL");

    // If it is pending request for layout, than layout now.
    if (LayoutPending())
    {
        LayoutAndDrawL();
    }

    // Set focus of item to aIndex and scroll to it.
    SetFocusedItem(aIndex);

}


//
// End of MMIDForm interface
//

TInt CMIDForm::CountComponentControls() const
{
    return iRows.Count();
}

CCoeControl* CMIDForm::ComponentControl(TInt aIndex) const
{
    return iRows[aIndex];
}

TKeyResponse CMIDForm::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    DEBUG("< CMIDForm::OfferKeyEventL");

    TBool isArrowKey = (aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow ||
                        aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow);

    if (iFocused != KErrNotFound)
    {
        CMIDControlItem& controlItem = ControlItem(iFocused);
        TRect controlRect = GetControlRect(iFocused);
        TBool visible = RectPartiallyVisible(controlRect);

        // arrow key events are not sent to the hidden focused item
        if ((visible || !isArrowKey) &&
                controlItem.OfferKeyEventL(aKeyEvent,aType) == EKeyWasConsumed)
        {
            DEBUG("CMIDForm::OfferKeyEventL - out after item consumed key");

#ifdef RD_SCALABLE_UI_V2
            // If focused TextField or DateField item is at least partly out of
            // Form viewport, then key event (excluding arrow keys and soft keys)
            // scrolls the item completely visible. This situation
            // is possible when scrolling has been done with scrollbar.

            // Soft keys and arrow keys are ignored. Filtering must be done comparing
            // scan codes because the aEventType.iCode is not available for all key presses.
            TBool ignoreKey = (aKeyEvent.iScanCode == EStdKeyRightArrow ||
                               aKeyEvent.iScanCode == EStdKeyUpArrow ||
                               aKeyEvent.iScanCode == EStdKeyDownArrow ||
                               aKeyEvent.iScanCode == EStdKeyLeftArrow ||
                               aKeyEvent.iScanCode == EStdKeyDevice0 ||
                               aKeyEvent.iScanCode == EStdKeyDevice1 ||
                               aKeyEvent.iScanCode == EStdKeyDevice3);

            if ((controlItem.Type() == MMIDComponent::ETextField ||
                    controlItem.Type() == MMIDComponent::EDateField) &&
                    !RectFullyVisible(controlRect) && !ignoreKey)
            {
                // Scroll up if controlItem is above the Form viewport.
                // Otherwise scroll down.
                if (controlRect.iTl.iY < 0)
                {
                    ScrollToFocused(EUp, ETrue);
                }
                else
                {
                    ScrollToFocused(EDown, ETrue);
                }

            }
#endif // RD_SCALABLE_UI_V2

            return EKeyWasConsumed;
        }
    }

    DEBUG("CMIDForm::OfferKeyEventL - got chance to scroll");

    if ((aType == EEventKey) && isArrowKey && (iItems.Count() > 0))
    {
        DEBUG("CMIDForm::OfferKeyEventL - about to call Traverse");

        Traverse(ConvertKeyToDirection(aKeyEvent.iCode));

        DEBUG("CMIDForm::OfferKeyEventL - out after Traverse");
        return EKeyWasConsumed;
    }

    // msk: this is needed if MSK is not enabled
    if ((aType == EEventKey) &&
            ((aKeyEvent.iScanCode == EStdKeyDevice3) || (aKeyEvent.iCode == EKeyEnter))
       )
    {
        if (!DoDefaultCommand())
        {
            iDisplayable.ShowOkOptionsMenuL();
        }

        DEBUG("CMIDForm::OfferKeyEventL - out after default command or ok menu");
        return EKeyWasConsumed;
    }
    // end msk
    DEBUG("> CMIDForm::OfferKeyEventL");
    return EKeyWasNotConsumed;
}

/**  Handle a focus change. Make sure there is no layout pending and if there is
one then do it, see TrappedLayoutAndDraw(). This is because when there is a layout
pending we are in a temporary inconsistent phase with no rows and the rows are the
intermediary to the items via the CountComponentControl() and ComponentControl() calls.
Then pass on focus to our children according to the value stored in iFocused. Finally
update the scroll bars if gaining focus and draw. */
void CMIDForm::FocusChanged(TDrawNow /*aDrawNow*/)
{
    DEBUG("CMIDForm::FocusChanged");

    TrappedLayoutAndDraw(ETrue);

    TInt i = -1;
    TInt itemCount = iItems.Count();
    while (++i < itemCount)
    {
        if (i == iFocused)
        {
            ControlItem(iFocused).SetFocus(IsFocused());
        }
        else
        {
            ControlItem(i).SetFocus(EFalse);
        }
    }

    // make sure scrollbar is up to date when we get focus
    if (IsFocused())
    {
        UpdateScrollBar();
    }

    DrawNow();
}

void CMIDForm::SizeChanged()
{
    DEBUG("CMIDForm::SizeChanged");

    iBackgroundControlContext->SetParentPos(PositionRelativeToScreen()) ;
    iBackgroundControlContext->SetRect(Rect()) ;
}

/** Before calling CCoeControl::InputCapabilities(), make sure there is
no outstanding layout. This is because if there is one outstanding, we have
no rows and hence ComponentControls() returns zero. This means that, for example
if there are any editors in the items, CCoeControl::InputCapabilities() will not
reach them and the FEP will not be initialised. @see TrappedLayoutAndDraw(),
CCoeControl::InputCapabilities(), CAknFepManager::HandleChangeInFocusL() and
CCoeAppUi::InputCapabilities() */
TCoeInputCapabilities CMIDForm::InputCapabilities() const
{
    if (iFocused != KErrNotFound)
    {
        return ControlItem(iFocused).InputCapabilities();
    }
    return     CCoeControl::InputCapabilities(); //TCoeInputCapabilities::ENone
}

TTypeUid::Ptr CMIDForm::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == MAknsControlContext::ETypeId && iBackgroundControlContext)
    {
        return MAknsControlContext::SupplyMopObject(aId, iBackgroundControlContext);
    }

    CMIDMenuHandler* menuHandler = iDisplayable.MenuHandler();
    return SupplyMopObject(aId, menuHandler->Cba(), menuHandler->MenuBar());
}


#ifdef RD_SCALABLE_UI_V2
void CMIDForm::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (!AknLayoutUtils::PenEnabled())
    {
        return;
    }

    if (iPhysics)
    {
        HandlePhysicsPointerEventL(aPointerEvent);
        return;
    }

    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
    {
        DEBUG_INT3("CMIDForm::HandlePointerEventL(): event=%D, x,y=%D,%D",
                   aPointerEvent.iType, aPointerEvent.iPosition.iX, aPointerEvent.iPosition.iY);
        iLastValidPointerEvent = aPointerEvent;

        CMIDControlItem* ci = ControlItemAtPoint(aPointerEvent.iPosition);
        // stored on ButtonDown value

        if (ci && ci->IsSelectable())
        {
            TInt itemIndex = ItemIndex(*ci);
            iIndexPointedControl = itemIndex;
            iLastValidPointedItemPosition = ci->Position();
            TBool changeFocus = (itemIndex != iFocused);

            if (changeFocus)
            {
                // changing the focused item by dragging must start from unfocused item
                iCanDragFocus = ETrue;
                iFocusChangingWithPen = ETrue;
                // give basic tactile feedback on button down when move focus
#ifdef RD_TACTILE_FEEDBACK
                DoFeedbackOnFocusChange(*ci);
#endif // RD_TACTILE_FEEDBACK
                SetFocusedItem(itemIndex, ENone, EFalse);
            }

            TInt scroll = ScrollDistanceToTappedItem(ci);

            iScrollOnPointerDown = (scroll != 0);

            CCoeControl::HandlePointerEventL(aPointerEvent);

            if (iScrollOnPointerDown)
            {
                ScrollRelative(scroll);
                ControlItem(iFocused).NotifyScrollingCompleted();
            }
            else if (changeFocus)
            {
                HandleItemVisibilityChange();
                DrawNow();
            }
            else
            {
                iCanDragFocus = ETrue;
            }
        }
        else // pointer down did not happen on any control item, allow starting drag
        {
            iCanDragFocus = ETrue;
        }

        break;
    }

    case TPointerEvent::EButton1Up:
    {
        if (iIndexPointedControl>-1 && &ControlItem(iIndexPointedControl) == ControlItemAtPoint(aPointerEvent.iPosition))
            // When this is item where TPointerEvent::EButton1Down was happened,
            // then event is forwarded to item. It is its responsibility check that
            // up event occured if it have some actions on up event.
        {
            DEBUG_INT3("CMIDForm::HandlePointerEventL(): event=%D, x,y=%D,%D",
                       aPointerEvent.iType, aPointerEvent.iPosition.iX, aPointerEvent.iPosition.iY);

            HandleDragEventL(aPointerEvent, ETrue);
        }
        else
        {
            // When this is not item where TPointerEvent::EButton1Down was happened,
            // then e`vent is forwarded to item where originally. It is its responsibility
            // check that up event occured if it have some actions on up event.
            // Setting default values of related variables.

            if (iIndexPointedControl>-1 && &ControlItem(iIndexPointedControl))
            {
                TBool bbutton = EFalse;

                if (IsStringItem(ControlItem(iIndexPointedControl)))
                {
                    // get appearance from StringItem
                    CMIDStringItem *strItem = static_cast< CMIDStringItem* >(&ControlItem(iIndexPointedControl));
                    MMIDItem::TAppearance appearance = strItem->Appearance();
                    bbutton = (appearance == MMIDItem::EButton || appearance == MMIDItem::EHyperLink);
                }

                if (ControlItem(iIndexPointedControl).Type() != MMIDComponent::EChoiceGroup && !bbutton)
                {
                    TPointerEvent pointerevent;
                    pointerevent.iType = TPointerEvent::EButton1Up;
                    pointerevent.iModifiers = aPointerEvent.iModifiers;
                    pointerevent.iPosition = iLastValidPointerEvent.iPosition +
                                             ControlItem(iIndexPointedControl).Position() - iLastValidPointedItemPosition;
                    pointerevent.iParentPosition = iLastValidPointerEvent.iParentPosition;

                    ControlItem(iIndexPointedControl).HandlePointerEventL(pointerevent);

                    DEBUG_INT3("CMIDForm::HandlePointerEventL(): event=%D, x,y=%D,%D",
                               pointerevent.iType, pointerevent.iPosition.iX, pointerevent.iPosition.iY);

                }

                // forward event for aPointerEvent
                CCoeControl::HandlePointerEventL(aPointerEvent);
            }
        }

        // If it is pending request for layout, than layout form now and refresh.
        if (LayoutPending())
        {
            LayoutFormL();
            DrawDeferred();
        }

        iCanDragFocus = EFalse;
        iFocusChangingWithPen = EFalse;
        iScrollOnPointerDown = EFalse;
        iIndexPointedControl = -1;

        break;
    }

    case TPointerEvent::EDrag:
        // for dragging
    {
        if (iIndexPointedControl>-1 && &ControlItem(iIndexPointedControl) == ControlItemAtPoint(aPointerEvent.iPosition))
        {
            // When this is item where TPointerEvent::EButton1Down was happened,
            // then it is updated iLastValidPointerEvent.
            iLastValidPointerEvent = aPointerEvent;
            // If it is pending request for layout, than layout Form now and refresh
            // elsewhere call HandleDragEventL and forward EDrag events to event.
            if (LayoutPending())
            {
                LayoutFormL();
                DrawDeferred();
            }
            else
            {
                HandleDragEventL(aPointerEvent, ETrue);
            }
            DEBUG_INT3("CMIDForm::HandlePointerEventL(): event=%D, x,y=%D,%D",
                       aPointerEvent.iType, aPointerEvent.iPosition.iX,
                       aPointerEvent.iPosition.iY);
        }
        else
        {
            // When this is not item where TPointerEvent::EButton1Down was happened,
            // then we do only dragging. There it is not send event to item.
            HandleDragEventL(aPointerEvent, EFalse);
        }
        break;
    }

    case TPointerEvent::EButtonRepeat:
        // For button repeating
    {
        DEBUG_INT3("CMIDForm::HandlePointerEventL(): event=%D, x,y=%D,%D",
                   aPointerEvent.iType, aPointerEvent.iPosition.iX,
                   aPointerEvent.iPosition.iY);

        RequestPointerRepeat(aPointerEvent);

        if (iCanDragFocus)
        {
            if (aPointerEvent.iPosition.iY < 0  && CanScrollUp())
            {
                TimedScroll(EUp);
            }
            else if (aPointerEvent.iPosition.iY > Height() && CanScrollDown())
            {
                TimedScroll(EDown);
            }
        }
        else
        {
            CCoeControl::HandlePointerEventL(aPointerEvent);
        }

        break;
    }
    default:
        // By defalt only implementation of base class.
    {
        CCoeControl::HandlePointerEventL(aPointerEvent);
        DEBUG_INT3("CMIDForm::HandlePointerEventL(): event=%D, x,y=%D,%D",
                   aPointerEvent.iType, aPointerEvent.iPosition.iX,
                   aPointerEvent.iPosition.iY);
        break;
    }
    }
}


void CMIDForm::HandleDragEventL(const TPointerEvent& aPointerEvent, TBool aForItem)
{
    RequestPointerRepeat(aPointerEvent);

    if (iCanDragFocus)
    {
        // formRect covers the whole vertical form area, but excludes
        // the left and right margins and the scroll bar area
        TRect formRect(TPoint(iFormRect.Rect().iTl.iX, 0),
                       TPoint(iFormRect.Rect().iBr.iX, Height()));

        // Try to find the new focused item, if needed
        if (formRect.Contains(aPointerEvent.iPosition) &&
                !FocusedItemContainsPoint(aPointerEvent.iPosition))
        {
            CMIDControlItem* ci = ControlItemAtPoint(aPointerEvent.iPosition);

            if (ci && ci->IsSelectable())
            {
                TInt itemIndex = ItemIndex(*ci);

                // Find the direction to which the focus is dragged
                TDirection direction = ENone;
                if (iFocused != KErrNotFound)
                {
                    TRect rect = ControlItem(iFocused).Rect();

                    if (aPointerEvent.iPosition.iY < rect.iTl.iY)
                    {
                        direction = EUp;
                    }
                    else if (aPointerEvent.iPosition.iY >= rect.iBr.iY)
                    {
                        direction = EDown;
                    }
                    else // focus is moved horizontally
                    {
                        direction = (itemIndex < iFocused) ? ELeft : ERight;
                    }
                }

#ifdef RD_TACTILE_FEEDBACK
                if (iFocused != itemIndex)
                {
                    iFeedback->InstantFeedback(ETouchFeedbackSensitive);
                }
#endif
                SetFocusedItem(itemIndex, direction, EFalse);
                HandleItemVisibilityChange();
                DrawNow();
            }
        }

        if (FocusedItemContainsPoint(aPointerEvent.iPosition))
        {
            // forward EDrag events to the focused so that the focus can
            // move inside the item
            // (only when this is item where TPointerEvent::EButton1Down was occured)

            CMIDControlItem* focusedControl = ControlItemAtPoint(aPointerEvent.iPosition);

            if (aForItem || (iIndexPointedControl > -1 && &ControlItem(iIndexPointedControl) &&
                             (IsChoiceGroup(ControlItem(iIndexPointedControl)) || IsCustomItem(ControlItem(iIndexPointedControl)))) ||
                    focusedControl && IsChoiceGroup(*focusedControl) ||
                    focusedControl && IsCustomItem(*focusedControl))
            {
                ControlItem(iFocused).HandlePointerEventL(aPointerEvent);
            }
            else
            {
                if (iIndexPointedControl > -1 && &ControlItem(iIndexPointedControl))
                {
                    TBool bbutton = EFalse;

                    if (IsStringItem(ControlItem(iIndexPointedControl)))
                    {
                        CMIDStringItem *strItem = static_cast< CMIDStringItem* >(&ControlItem(iIndexPointedControl));
                        MMIDItem::TAppearance appearance = strItem->Appearance();
                        bbutton = (appearance == MMIDItem::EButton || appearance == MMIDItem::EHyperLink);
                    }

                    //forward event (only for changing focus) for Button & Hyperlink
                    if (bbutton && &ControlItem(iIndexPointedControl))
                    {
                        ControlItem(iIndexPointedControl).HandlePointerEventL(aPointerEvent);
                    }
                    else
                    {
                        iDisplayable.TryDetectLongTapL(aPointerEvent);
                    }
                }
                else
                {
                    iDisplayable.TryDetectLongTapL(aPointerEvent);
                }
            }
        }
        else
        {
            // make sure that long tap animation is canceled in this case
            iDisplayable.TryDetectLongTapL(aPointerEvent);
        }
    }
    else // cannot drag focus
    {
        CCoeControl::HandlePointerEventL(aPointerEvent);
    }
}


void CMIDForm::RequestPointerRepeat(const TPointerEvent& aPointerEvent)
{
    if (aPointerEvent.iPosition.iY < 0 ||
            aPointerEvent.iPosition.iY > Height())
    {
        TRect screen;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screen);

        TRect repeatRect = screen;
        if (aPointerEvent.iPosition.iY < 0)
        {
            repeatRect.iBr.iY = iMidpFormRect.Rect().iTl.iY;
        }
        else
        {
            repeatRect.iTl.iY = iMidpFormRect.Rect().iBr.iY;
        }

        // repeatRect must be given in propotion to form's window, move it up
        repeatRect.Move(0, -iMidpFormRect.Rect().iTl.iY);
        Window().RequestPointerRepeatEvent(KScrollInterval, repeatRect);
    }
}


void CMIDForm::TimedScroll(TDirection aDirection)
{
    ASSERT(aDirection == EDown || aDirection == EUp);

    // First find the row that is the first or last row (depending on
    // the scroll direction) that is at least partailly visible on the form.
    // Search direction is opposite to aDirection.
    TDirection searchDirection = (aDirection == EUp) ? EDown : EUp;
    TInt idx = FirstRowOnScreen(searchDirection);

    if (idx == KErrNotFound)
    {
        return;
    }

    TInt scroll = Height() / KScrollAmountDividerForThumbDrag;

    // Find the row that will be visible (at least partially) after
    // after the scrolling.
    if (aDirection == EUp)
    {
        while (idx > 0 && (iRows[idx]->Rect().iTl.iY + scroll > 0))
        {
            --idx;
        }
    }
    else
    {
        while ((idx < iRows.Count() - 1) &&
                (iRows[idx]->Rect().iBr.iY - scroll < Height()))
        {
            ++idx;
        }
    }

    TBool rowFocused = EFalse;
    if (iFocused != KErrNotFound)
    {
        rowFocused = GetControlRect(iFocused).Intersects(iRows[idx]->Rect());
    }

    // If the focus is not yet on the row, try to set one of its items focused
    if (!rowFocused)
    {
        for (TInt i = 0; i < iRows[idx]->NumItems(); ++i)
        {
            CMIDControlItem* item = iRows[idx]->Item(i);
            if (item->IsSelectable())
            {
                SetFocusedItem(ItemIndex(*item), aDirection, EFalse);
            }
        }
    }

#ifdef RD_TACTILE_FEEDBACK
    iFeedback->InstantFeedback(ETouchFeedbackSensitive);
#endif

    ScrollRelative((aDirection == EUp) ? scroll : -scroll);
}


TBool CMIDForm::CanScrollUp() const
{
    return (iScroll < 0);
}


TBool CMIDForm::CanScrollDown() const
{
    return ((iFormHeight - iScroll) < iTotalFormHeight);
}


TBool CMIDForm::FocusedItemContainsPoint(const TPoint& aPoint) const
{
    if (iFocused != KErrNotFound)
    {
        if (ItemIsUnconstrained(iFocused))
        {
            return StringItemContainsPoint(
                       static_cast<CMIDStringItem*>(&ControlItem(iFocused)), aPoint);
        }
        else
        {
            return ControlItem(iFocused).Rect().Contains(aPoint);
        }
    }
    else
    {
        return EFalse;
    }
}

CMIDControlItem* CMIDForm::ControlItemAtPoint(const TPoint& aPoint) const
{
    TInt idx = RowAtPoint(aPoint);

    if (idx != KErrNotFound)
    {
        for (TInt i = 0; i < iRows[idx]->NumItems(); ++i)
        {
            CMIDControlItem* ci = iRows[idx]->Item(i);
            if (ci && ci->Rect().Contains(aPoint))
            {
                return ci;
            }
        }
    }

    return NULL;
}


TInt CMIDForm::RowAtPoint(const TPoint& aPoint) const
{
    TInt rowCount = iRows.Count();
    // loop until the first row fully below the visible form area
    for (TInt i = 0; i < rowCount && (iRows[i]->Position().iY < iFormHeight); ++i)
    {
        if (iRows[i]->Rect().Contains(aPoint))
        {
            return i;
        }
    }
    return KErrNotFound;
}

#endif // #ifdef RD_SCALABLE_UI_V2


void CMIDForm::SetFocusedItemIfNone(CMIDItem* aItem)
{
    if (iFocused == KErrNotFound)
    {
        ASSERT(aItem);
        TInt index = Index(aItem);

        if (index >= 0 && index < iItems.Count())
        {
            CMIDControlItem& ci = ControlItem(iItems[index]);
            if (ci.IsSelectable())
            {
                SetFocusedItem(index);
                ScrollToFocused();
            }
        }
    }
}

/** This method is called when an item loses focusability,
for example because it loses all its commands. If this item
is focused, call ReplaceFocusedItem() to find another focusable
item. @see CMIDItem::RemoveCommand() and  ReplaceFocusedItem*/
void CMIDForm::RemoveFocusIfFocused(CMIDItem* aItem)
{
    ASSERT(aItem);
    TInt index = Index(aItem);
    if (iFocused == index)
    {
        // Hide menu because item has lost all item commands and
        // another item may have become focused behind the menu.
        iDisplayable.MenuHandler()->HideMenuIfVisible();
        ReplaceFocusedItem();
    }
}

/**
 * Traverse is a method to go thorough the form; it includes moving focus from
 * item to another possibly scrolling the form. Note that sometimes traversal
 * does not move focus, because an item is larger that the screen and thus form
 * is scrolled little by little so that the user can see the whole content. On
 * other cases, traversing means only moving focus from item to another as both
 * items are visible on the screen.
 *
 * @param  aDirection defines the direction of the traversing (up, down, left, right).
 **/
void CMIDForm::Traverse(TDirection aDirection)
{
    DEBUG("< CMIDForm::Traverse");
    DEBUG_INT("< CMIDForm::Traverse Direction is %D", aDirection);
    TBool horizontalScroll = EFalse;

    if (LayoutPending())
    {
        DEBUG("CMIDForm::Traverse - calling layout");
        // lets do the layout now so we can always assume that we are layed out before doing this stuff
        TrappedLayoutAndDraw();
    }

#ifdef RD_SCALABLE_UI_V2
    TBool focusHidden = EFalse;
    // If the focused item is currently fully outside the visible form area,
    // it means that form has been scrolled by moving the scrollbar with pen. In this case
    // set iFocused (temporarily) to KErrNone in order that scrolling with arrow keys works ok.
    if (iFocused != KErrNotFound && !RectPartiallyVisible(GetControlRect(iFocused)))
    {
        focusHidden = ETrue;
        SetFocusedItem(KErrNotFound, ENone, EFalse);
    }
#endif

    if (aDirection == ELeft || aDirection == ERight)
    {
        DEBUG("CMIDForm::Traverse - move focus horizontally");
        if (!TryMovingFocusHorizontally(aDirection))
        {
            if (iInitialAlignment == MMIDItem::ERight)
            {//RIGHT TO LEFT
                TryMovingFocusVertically(aDirection == ELeft ? EDown : EUp, EFalse);
            }
            else
            {//LEFT TO RIGHT
                TryMovingFocusVertically(aDirection == ELeft ? EUp : EDown, EFalse);
            }
        }
        else
        {
            horizontalScroll = ETrue;
        }
    }
    else if (aDirection == EUp || aDirection == EDown)
    {
        DEBUG("CMIDForm::Traverse - move focus vertically");
        TryMovingFocusVertically(aDirection, ETrue);
    }

    // if there is still a focused item, make sure it is still on the screen.
    // if it isn't remove focus as we've already tried out best to have a
    // focussed item on screen
    if (iFocused != KErrNotFound)
    {
        TRect rect = GetControlRect(iFocused);
        if (!RectPartiallyVisible(rect))
        {
            SetFocusedItem(KErrNotFound, ENone);
        }
    }
#ifdef RD_SCALABLE_UI_V2
    else if (focusHidden)
    {
        // Try to bring the focus back to visible area by setting the last focusable
        // item in the scroll direction focused.
        TDirection searchDirection = (aDirection == EDown || aDirection == ERight) ?
                                     EUp : EDown;
        TInt newFocusIndex  = IndexOfFirstFocusableItemOnScreen(searchDirection);
        if (newFocusIndex != KErrNotFound)
        {
            // Pass searchDirection also to SetFocusedItem(). This is needed at least
            // for choice group and custom item cases, so that they can set there
            // internal focus to the last element in the actual scroll direction.
            SetFocusedItem(newFocusIndex, searchDirection, EFalse);
            DrawDeferred();
        }
    }
#endif

    RawScrollFinalize(horizontalScroll);
    DEBUG("> CMIDForm::Traverse");
}


// -----------------------------------------------------------------------------
// Form scrolling methods
// -----------------------------------------------------------------------------
/**
 * Scroll to the focused control if there is one and update form visibility (draw) and scroll bar.
 * @see ScrollToRect() and SetRowExtentsWithRespectToScrolledLocation().
 **/
void CMIDForm::ScrollToFocused(TDirection aDirection /*=EDown*/, TBool aReset /*= ETrue*/)
{
    DEBUG("< CMIDForm::ScrollToFocused");
    if (!LayoutPending())
    {
        if (iFocused != KErrNotFound)
        {
            TRect rect = GetControlRect(iFocused);
            DEBUG_INT4("CMIDForm::ScrollToFocused - rect is %D %D %D %D",
                       rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);
            RawScrollToRect(rect, aDirection, aReset);
        }

        RawScrollFinalize(ETrue);

        if (iFocused != KErrNotFound)
        {
            ControlItem(iFocused).NotifyScrollingCompleted();
        }
    }

    UpdatePhysics();

    DEBUG("> CMIDForm::ScrollToFocused");
}

TInt CMIDForm::ScrollDistanceToTappedItem(CMIDControlItem* aPointedControl)
{
    TRect focusedRect = aPointedControl->Rect();
    TInt scroll = 0;

    // Calculate how much we need to scroll to make the tapped item fully/more visible
    if (!RectFullyVisible(focusedRect))
    {
        TInt topMargin = iFormRect.Rect().iTl.iY - iMidpFormRect.Rect().iTl.iY;
        TInt bottomMargin = iMidpFormRect.Rect().iBr.iY - iFormRect.Rect().iBr.iY;

        if (focusedRect.Height() <= Height())
        {
            scroll = (focusedRect.iBr.iY > Height()) ?
                     Height() - focusedRect.iBr.iY : -focusedRect.iTl.iY;
        }
        // Scroll long item so that it fills the whole form height.
        // No scroll in case item already fills the whole screen or if there would
        // be just few pixels (~margins) to scroll.
        else if ((focusedRect.iTl.iY > 0 && focusedRect.iTl.iY > topMargin) ||
                 (focusedRect.iBr.iY < Height() &&
                  Height() - focusedRect.iBr.iY > bottomMargin))
        {
            scroll = (focusedRect.iTl.iY > 0) ?
                     -focusedRect.iTl.iY : -(focusedRect.iBr.iY - Height());
        }
    }
    return scroll;
}

/**
 * Scrolls to the rect corresponding to the row specified by aIdx using
 * the direction indicated by aDirection. @see ScrollToRect()
 **/
void CMIDForm::RawScrollToRow(TInt aIdx, TDirection aDirection)
{
    DEBUG("< CMIDForm::RawScrollToRow");
    if (aIdx >= 0 && aIdx < iRows.Count())
    {
        TRect rect = iRows[aIdx]->Rect();
        DEBUG("CMIDForm::RawScrollToRow - scroll to rect");
        RawScrollToRect(rect, aDirection);
    }
    DEBUG("> CMIDForm::RawScrollToRow");
}

/** */
void CMIDForm::RawScrollToRect(const TRect& aRect, TDirection aDirection, TBool aReset)
{
    DEBUG("< CMIDForm::RawScrollToRect");
    DEBUG_INT4("CMIDForm::RawScrollToRect - rect is %D %D %D %D", aRect.iTl.iX, aRect.iTl.iY,
               aRect.iBr.iX, aRect.iBr.iY);

    if (RectFullyVisible(aRect))
    {
        DEBUG("CMIDForm::RawScrollToRect - return rect fully visible");
        TInt maxScrollValue = Height() - HeightOfAllItems();
        // Avoid scrolling out of the form (i.e. after the view port has lower height)

        if (iScroll < maxScrollValue)
        {
            RawScroll(maxScrollValue);
        }
        return;
    }

    TInt scrollOffset = 0;
    if (aRect.Size().iHeight > Height())
    {//rect higher than screen height
        if (RectPartiallyVisible(aRect) && !aReset)
        {//if already visible, scroll only of KMinScrollAmount (panning)
            scrollOffset = (aDirection == EDown) ? -KMinScrollAmount : KMinScrollAmount;
        }
        else
        {//if not visible and scrolling down scroll to top - KMinScrollAmount
            // or to bottom + KMinScrollAmount if scrolling up
            scrollOffset = (aDirection == EDown) ? (Height() - (aRect.iTl.iY + KMinScrollAmount))
                           : -(aRect.iBr.iY - KMinScrollAmount);
        }
    }
    else
    {//if it fits on screen make sure its BR is at the bottom if scrolling down
        //or its TL is at the top if scrolling up
        scrollOffset = (aDirection == EDown) ? (Height() - aRect.iBr.iY) : (-aRect.iTl.iY);
    }

    RawScroll(iScroll + scrollOffset);
    DEBUG("> CMIDForm::RawScrollToRect");
}

void CMIDForm::ScrollRelative(TInt aScroll)
{
    DEBUG("CMIDForm::ScrollRelative");
    RawScrollRelative(aScroll);
    RawScrollFinalize(EFalse);
}

void CMIDForm::RawScrollRelative(TInt aScroll)
{
    TInt scroll = iScroll + aScroll;
    RawScroll(scroll);
}

void CMIDForm::RawScroll(TInt aScroll)
{
    TInt contentHeight  = HeightOfAllItems();
    TInt viewPortHeight = Height();
    if (aScroll > 0 || contentHeight <= viewPortHeight)
    { // make sure that top does not slide down
        aScroll = 0;
    }
    if (contentHeight > viewPortHeight && ((-aScroll) + viewPortHeight) > contentHeight)
    { // scroll only as long as there is content
        aScroll = -(contentHeight - viewPortHeight);
    }
    iHasScrolled = iScroll != aScroll;
    iScrollDelta = aScroll - iScroll;
    iScroll = aScroll;

    SetRowPositionsWithRespectToScrolledLocation();
}

void CMIDForm::SetRowExtentsWithRespectToScrolledLocation()
{
    iTotalFormHeight = 0;
    TInt xMargin = iFormRect.Rect().iTl.iX;
    TInt yTopMargin = iFormRect.Rect().iTl.iY - iMidpFormRect.Rect().iTl.iY;
    TInt yBottomMargin = iMidpFormRect.Rect().iBr.iY - iFormRect.Rect().iBr.iY;

    iTotalFormHeight = yTopMargin;

    TInt rowCount = iRows.Count();
    for (TInt i=0; i < rowCount; i++)
    {
        // Form left margin is added in the beginning of each row
        iRows[i]->SetExtent(TPoint(xMargin, iScroll + iTotalFormHeight),
                            TSize(Width(), iRows[i]->Size().iHeight));
        iTotalFormHeight += iRows[i]->Size().iHeight;
    }
    // Form bottom margin is added
    iTotalFormHeight += yBottomMargin;
}

void CMIDForm::SetRowPositionsWithRespectToScrolledLocation()
{
    TInt rowCount = iRows.Count();
    for (TInt i=0; i < rowCount; i++)
    {
        // Form left margin is added in the beginning of each row
        TPoint pos = iRows[i]->Position();
        pos.iY += ScrollDelta();
        iRows[i]->SetPosition(pos);
    }
}

// Finalizes scrolling after any RawScrollXxxx method call, e.g. causes repaint.
// Draw can be prevented e.g. because of the performance reasons.
void CMIDForm::RawScrollFinalize(TBool forseAction, TBool aDraw /*= ETrue*/)
{
    if (iHasScrolled || forseAction)
    {
        UpdateHighlightBackgroundPosition();
        HandleItemVisibilityChange();
        UpdateScrollBar();
        if (iFocused != KErrNotFound)
        {
            CMIDControlItem& ci = ControlItem(iFocused);
            if (IsTextFieldItem(ci))
            {
                if (RectPartiallyVisible(ci.Rect()))
                {
                    ((CMIDTextFieldItem&) ci).SetCursorVisibility(ETrue);
                }
                else
                {
                    ((CMIDTextFieldItem&) ci).SetCursorVisibility(EFalse);
                }
            }
            if (IsDateField(ci))
            {
                // Update right cursor position on Datefiled
                ((CMIDDateFieldItem&) ci).CursorUpdate();
            }
        }
        if (aDraw)
        {
            DrawDeferred();
        }
        iHasScrolled = EFalse;
    }
}

/** Returns the client area */
TRect CMIDForm::GetClientArea()
{
    TRect scrolledClientArea = TRect(Position(), TSize(Width(), Height()));
    return scrolledClientArea;
}

// returns ETrue if it moved the focus
// returns EFalse otherwise
TBool CMIDForm::TryMovingFocusHorizontally(TDirection aDirection)
{
    ASSERT(aDirection == ELeft || aDirection == ERight);

    TInt idx = NextFocusableHorizontalIdx(aDirection);
    if (idx != KErrNotFound)
    {
        SetFocusedItem(idx, aDirection);
        return ETrue;
    }

    return EFalse;
}

/**
  Moves the focus vertically.

  We check if there is a focussable item,
  which is on a row totally or partially visible and if there is such
  an item we then select that row. This is done by calling
  NextPartiallyVisibleRowWithFocusableItem(). If no such item was found we pick
  the top or bottom (depending on scrolling direction) row that is partially
  visible or the first not visible if the last one is completely shown.
  This is done by FirstRowWithBottomBelowScreen() and LastRowWithTopAboveScreen().

  At this point if we have a row,  we select a focussable item on it and
  scroll to it if aScroll is true. Calling scroll methods doesn't mean there is going to be
  scrolling of the form,
  if the item is already visible no scrolling is done. We select the
  next focussable item according to the value of the parameter aSelectClosest.
  If this paramerter is true we call SelectClosestFocussableItem() otherwise
  we either select the first or the last selectable item on the row depending
  on the direction.

  If we find a control we scroll to it by calling ScrollToFocused() (if aScroll is true) and move
  the focus to it by calling SetFocusedItem(). Otherwise we
  scroll to the row by calling ScrollToRow() if aScroll is true.

  If we could not find any row with a focusable item on it, we scroll the form
  of a fixed quantity, KMinScrollAmount, if aScroll is true.

  We return ETrue if the focus was moved, EFalse otherwise. Note: Currently the
  return value of this method is ignored.

  @see  NextPartiallyVisibleRowWithFocusableItem(), FirstRowWithBottomBelowScreen(),
              LastRowWithTopAboveScreen(), SelectClosestFocussableItem()
                   ScrollToRow(), SetFocusedItem() and DoScrollRelative().
           Also see ReplaceFocusedItem(), called when the focused item is removed or
           loses focusability - in this case no scrolling will occur.
  */
TBool CMIDForm::TryMovingFocusVertically(TDirection aDirection, TBool aSelectClosest,
        TBool aScroll /*=ETrue*/)
{
    DEBUG("> CMIDForm::TryMovingFocusVertically");
    ASSERT(aDirection == EUp || aDirection == EDown);

    // is VKB opened
    if (iDisplayable.IsVKBOnScreen())
    {
        return ETrue;
    }

    TInt rowIdx = NextPartiallyVisibleRowWithFocusableItem(aDirection);

    if (rowIdx == KErrNotFound)
    {
        DEBUG("CMIDForm::TryMovingFocusVertically - no partially visible focusable control found");
        rowIdx = aDirection == EDown ? FirstRowWithBottomBelowScreen() : LastRowWithTopAboveScreen();
    }

    if (rowIdx != KErrNotFound)
    {
        CMIDControlItem* ci = aSelectClosest ? SelectClosestFocussableItem(rowIdx) :
                              (aDirection == EDown ? iRows[rowIdx]->FirstFocusableItem() :
                               iRows[rowIdx]->LastFocusableItem());


        if (ci)
        {
            DEBUG("CMIDForm::TryMovingFocusVertically - got control");
            TInt index = Index(ci);

            if (index == KErrNotFound && IsLabelContainerItem(*ci))
            {
                CMIDLabelContainerItem* ucsi = static_cast<CMIDLabelContainerItem*>(ci);
                CMIDStringItem& si = ucsi->StringItem();
                index = Index(&si);
            }

            if (index != KErrNotFound)
            {
                TRect rect = GetControlRect(index);

                if (aScroll)
                {
                    SetFocusedItem(index, aDirection, ETrue);
                }
                else if (RectPartiallyVisible(rect))
                {
                    SetFocusedItem(index, aDirection);
                }
                else
                {
                    SetFocusedItem(KErrNotFound);
                }

                DEBUG("CMIDForm::TryMovingFocusVertically - out after scroll to control");
                return ETrue;
            }
        }

        DEBUG("CMIDForm::TryMovingFocusVertically - scrolling to row");
        if (aScroll)
        {
            RawScrollToRow(rowIdx, aDirection);
        }
    }
    else
    {
        DEBUG("CMIDForm::TryMovingFocusVertically - small default scrolling");
        if (aScroll)
        {
            RawScrollRelative((aDirection == EDown) ? -KMinScrollAmount : KMinScrollAmount);
        }
    }

    DEBUG("< CMIDForm::TryMovingFocusVertically");
    return EFalse;
}

/** Given a row index, selects on this row the focusable control whose center on
the x axis is closes to the center of the currently focused item, if there is one.
If no control is found, returns the first selectable item on the row. */
CMIDControlItem* CMIDForm::SelectClosestFocussableItem(TInt aRowIndex) const
{
    ASSERT(aRowIndex >= 0 && aRowIndex < iRows.Count());
    TInt selectedIndex = -1;

    if (iFocused != -1)
    {
        TRect rect = GetControlRect(iFocused);
        TInt focusedCenter = rect.iTl.iX + rect.Width()/2;

        TInt diff = iInnerWidth;

        TInt numItemsOnRow = iRows[aRowIndex]->NumItems();
        for (TInt i = 0; i < numItemsOnRow; i++)
        {
            CMIDControlItem* item = iRows[aRowIndex]->Item(i);
            if (item->IsSelectable())
            {
                TInt center = item->Position().iX + (item->Size().iWidth / 2);
                TInt itemDiff = focusedCenter > center ? (focusedCenter - center) : (center - focusedCenter);

                if (itemDiff < diff)
                {
                    selectedIndex = i;
                    diff = itemDiff;
                }
            }
        }
    }

    return selectedIndex == -1 ? iRows[aRowIndex]->FirstFocusableItem() :
           iRows[aRowIndex]->Item(selectedIndex);
}

TInt CMIDForm::IndexOfFirstFocusableItemOnScreen(TDirection aDirection)
{
    ASSERT(aDirection == EUp || aDirection == EDown);

    TInt startIdx = FirstRowOnScreen(aDirection);

    if (startIdx == KErrNotFound)
    {
        return KErrNotFound;
    }

    CMIDControlItem* ci = NULL;

    if (aDirection == EDown)
    {
        TInt idx = startIdx - 1;
        TInt rowCount = iRows.Count();
        while (++idx < rowCount)
        {
            CMIDFormRow* row = iRows[idx];
            if (row->Position().iY > Height())
            {
                break;
            }
            ci = row->FirstFocusableItemOnScreen();
            if (ci)
            {
                break;
            }
        }
    }
    else
    {
        TInt idx = startIdx + 1;
        while (--idx >= 0)
        {
            CMIDFormRow* row = iRows[idx];
            if ((row->Position().iY + row->Size().iHeight) < 0)
            {
                break;
            }
            ci = row->FirstFocusableItemOnScreen();
            if (ci)
            {
                break;
            }
        }
    }

    if (!ci)
    {
        return KErrNotFound;
    }

    TInt index = Index(ci);
    if (index == KErrNotFound)
    {
        if (IsLabelContainerItem(*ci))
        {
            CMIDLabelContainerItem* ucsi = static_cast<CMIDLabelContainerItem*>(ci);
            CMIDStringItem& si = ucsi->StringItem();
            index = Index(&si);
        }
    }

    return index;
}

/** Return the rect for a control, taking into account if the control is an
unconstrained string item which has been split over more than one label container
*/
TRect CMIDForm::GetControlRect(TInt aIdx) const
{
    CMIDControlItem& ci = ControlItem(aIdx);
    TRect rect;

    if (ItemIsUnconstrained(aIdx))
    {
        rect = GetUnconstrainedStringItemRect(static_cast<CMIDStringItem*>(&ci));
    }
    else
    {
        rect = ci.Rect();
    }

    return rect;
}

/** Collate rects for unconstrained string items that have been split across more
than one lable container */
TRect CMIDForm::GetUnconstrainedStringItemRect(CMIDStringItem* aStringItem) const
{
    TRect rect(0,0,0,0);
    TBool found = EFalse;

    TInt rowIdx = FirstRowOfUnconstrainedStringItem(*aStringItem);
    TInt rowCount = iRows.Count();

    // if the first row of unconstrained string item cannot be found
    // it means that string item is placed as default item
    // so simply return item's rect
    if (rowIdx == KErrNotFound)
    {
        return aStringItem->Rect();
    }

    while (rowIdx != KErrNotFound && rowIdx < rowCount)
    {
        CMIDFormRow* row = iRows[rowIdx];
        for (TInt idxOnRow = 0; idxOnRow < row->NumItems(); idxOnRow++)
        {
            CMIDControlItem* ci = row->Item(idxOnRow);
            if (ci && IsLabelContainerItem(*ci))
            {
                CMIDLabelContainerItem* usil = static_cast<CMIDLabelContainerItem*>(ci);
                CMIDStringItem* si = &(usil->StringItem());

                if (si == aStringItem)
                {
                    if (!found)
                    {
                        rect.iTl = usil->Rect().iTl;
                        found = ETrue;
                    }
                    rect.iBr = usil->Rect().iBr;
                }
                else if (found)
                {
                    return rect;
                }
            }
            else if (found)
            {
                return rect;
            }
        }
        rowIdx++;
    }

    return rect;
}

TBool CMIDForm::RectPartiallyVisible(const TRect& aRect)
{
    return ((aRect.iTl.iY >= 0) && (aRect.iTl.iY < Height())) ||
           ((aRect.iBr.iY >= 0) && (aRect.iBr.iY < Height())) ||
           ((aRect.iTl.iY < 0) && (aRect.iBr.iY >= Height()));
}

TBool CMIDForm::RectFullyVisible(const TRect& aRect)
{
    return (aRect.iTl.iY >= 0) && (aRect.iBr.iY <= Height());
}

TInt CMIDForm::FirstRowWithBottomBelowScreen()
{
    TInt idx = -1;
    TInt rowWithFocus = iFocused == KErrNotFound ? -1 : Row(ControlItem(iFocused));
    TInt rowCount = iRows.Count();
    while (++idx < rowCount)
    {
        CMIDFormRow* row = iRows[idx];
        TInt height = row->Size().iHeight;
        TInt yPos = row->Position().iY;
        if ((height + yPos) > Height() && row->HasNonSpacerItems() && idx != rowWithFocus)
        {
            return idx;
        }
    }

    return KErrNotFound;
}

TInt CMIDForm::LastRowWithTopAboveScreen()
{
    TInt idx = iRows.Count();
    TInt rowWithFocus = iFocused == KErrNotFound ? -1 : Row(ControlItem(iFocused));

    while (--idx >= 0)
    {
        CMIDFormRow* row = iRows[idx];
        TInt yPos = row->Position().iY;
        if (yPos < 0 && row->HasNonSpacerItems() && idx != rowWithFocus)
        {
            return idx;
        }
    }

    return KErrNotFound;
}

// note: inspite of the name of this method, we only consider rows with
// a focusable item that has not already been scrolled off the screen.
// For Example:
//
//  ----
// | A  | This is a
// |    | string item
//  ----  that is placed
//----------------------- <- top of form
//        next to item A.
//        This
//        string
//        item
//        is
//        quite
//        tall
//        and
//----------------------- <- botom of form
//        fills
//        the
//        screen
//
// Assuming that A is focusable, and we are scrolling down, and also assuming
// that the string item is not focusable, we do not consider A when looking
// for focusable items, because we are scrolling down and A is already above
// the screen.
TInt CMIDForm::NextPartiallyVisibleRowWithFocusableItem(TDirection aDirection)
{
    ASSERT(aDirection == EUp || aDirection == EDown);

    TInt startIdx = 0;
    if (iFocused != KErrNotFound)
    {
        //If there is layout request pending, the array of rows is probably
        //empty. So we need to do layout of form, which fills the array of rows.
        //Then row, where current focused item is, will be found.
        if (iRows.Count() == 0)
        {
            TrappedLayoutAndDraw(ETrue);
        }
        TInt row = Row(ControlItem(iFocused));
        if (row == KErrNotFound)
        {
            CMIDControlItem& ci = ControlItem(iFocused);
            if (IsStringItem(ci))
            {
                // find the first row either above or below (depends on direction)
                // this unconstrained stringItem
                CMIDStringItem& stringItem =
                    *(static_cast<CMIDStringItem*>(&ci));
                if (aDirection == EDown)
                {
                    startIdx = FirstRowBelowUnconstrainedStringItem(stringItem);
                }
                else
                {
                    startIdx = FirstRowAboveUnconstrainedStringItem(stringItem);
                }
            }
            else
            {
                // shouldn't get here
                ASSERT(EFalse);
            }
        }
        else
        {
            startIdx = row + ((aDirection == EDown) ? 1 : -1);
        }
    }
    else
    {
        // There is no focused item on Form (iFocused value is KErrNotFound).
        // So we need to try to find another Item which could gain focus.
        startIdx = FirstRowOnScreen(aDirection);
    }

    if (startIdx == KErrNotFound)
    {
        return KErrNotFound;
    }

    if (aDirection == EDown)
    {
        TInt idx = startIdx - 1;
        TInt rowCount = iRows.Count();
        while (++idx < rowCount)
        {
            CMIDFormRow* row = iRows[idx];
            if (row->Position().iY > Height())
            {
                break;
            }
            if (row->HasFocusableItemOnOrBelowScreen())
            {
                return idx;
            }
        }
    }
    else
    {
        TInt idx = startIdx + 1;
        while (--idx >= 0)
        {
            CMIDFormRow* row = iRows[idx];

            if (row->HasFocusableItemOnOrAboveScreen())
            {
                return idx;
            }

            if ((row->Position().iY + row->Size().iHeight) < 0)
            {
                break;
            }
        }
    }

    return KErrNotFound;
}

// if aFromTop is false we look from the bottom up
TInt CMIDForm::FirstRowOnScreen(TDirection aDirection)
{
    ASSERT(aDirection == EUp || aDirection == EDown);

    TInt idx = 0;
    if (aDirection == EDown)
    {
        idx = -1;
        TInt rowCount = iRows.Count();
        while (++idx < rowCount)
        {
            CMIDFormRow* row = iRows[idx];
            TInt bottom = row->Position().iY + row->Size().iHeight;
            if (bottom > 0)
            {
                return idx;
            }
        }
    }
    else
    {
        idx = iRows.Count();
        while (--idx >= 0)
        {
            CMIDFormRow* row = iRows[idx];
            TInt top = row->Position().iY;
            if (top < Height())
            {
                return idx;
            }
        }
    }

    return KErrNotFound;
}

// Returns the item index of the next focusable item in the given direction.
// Note: this may return an object that is not on the current row if there is
//       an unconstrained string item on the current row.
//
//       --------
//       |      |
//       |  A   |
//       |      | --------
//       -------- --------
//       -----------------
//       __________ ------
//                  |    |
//                  | B  |
//                  |    |
//                  ------
// In the above diagram we see a selectable item A, an unconstrained stringItem,
// and another selectable item B. If A has focus and the stringItem is not focusable
// then calling NextFocusableHorizontalIdx( ERight ) should return the index of B.
// If the string item is selectable then its index would get returned and a further
// call to NextFocusableHorizontalIdx( ERight ) would return the index of B.
// If B has focus calling NextFocusableHorizontalIdx( ELeft ) would simply return the
// index of the stringItem if it is selectable or A if not.
TInt CMIDForm::NextFocusableHorizontalIdx(TDirection aDirection)
{
    ASSERT(aDirection == ELeft || aDirection == ERight);

    TInt idx = KErrNotFound;

    if (iFocused != KErrNotFound)
    {
        CMIDFormRow* rowPtr = NULL;
        CMIDControlItem& controlItem = ControlItem(iFocused);

        TInt rowIdx = Row(controlItem, rowPtr);

        if (rowIdx != KErrNotFound)
        {

            idx = rowPtr->Find(&controlItem);

            if (aDirection == ELeft)
            {
                while (--idx >= 0)
                {
                    if (rowPtr->Item(idx)->IsSelectable())
                    {
                        return ItemIndex(*(rowPtr->Item(idx)));
                    }
                }

            }
            else
            {
                // direction is ERight
                while (++idx < rowPtr->NumItems())
                {
                    if (rowPtr->Item(idx)->IsSelectable())
                    {
                        return ItemIndex(*(rowPtr->Item(idx)));
                    }
                }

            }
        }
        else
        {
            // there is something focused but we failed to find a row index for it,
            // so it is probably an unconstrained stringItem
            if (aDirection == ELeft)
            {
                if (IsStringItem(controlItem))
                {
                    CMIDStringItem* si = static_cast<CMIDStringItem*>(&controlItem);
                    idx = FirstFocusableItemBeforeStartOfUnconstrainedStringItemIdx(*si);
                    return idx;
                }
            }
            else
            {
                // direction ie ERight
                if (IsStringItem(controlItem))
                {
                    CMIDStringItem* si = static_cast<CMIDStringItem*>(&controlItem);
                    idx = FirstFocusableItemOnLastLineOfUnconstrainedStringItemIdx(*si);
                    return idx;
                }
            }
        }
    }

    return KErrNotFound;
}

/** Replaces the currently focused item. This is called when the focused item is removed
or it loses focusability, for example a string item that loses all its commands. So this
is always triggered by a java call, never by the user. We call the same methods that are
used by Scroll(), ie TryMovingFocusHorizontally() and TryMovingFocusVertically(). However
in this case we don't want the form to scroll if the focused item is not on screen. So
these two methods will move the focus if a focusable item is either completely or partially
visible on screen following the good direction (right-down for left to right devices and
vice-versa for right to left devices). TryMovingFocusVertically() will set the focused item
to KErrNotFound if no focusable item is currently visible. */
void CMIDForm::ReplaceFocusedItem()
{
    if (LayoutPending())
    {
        DEBUG("CMIDForm::ReplaceFocusedItem - calling layout");
        TRAP_IGNORE(LayoutFormL());
    }

    CMIDForm::TDirection direction = (iInitialAlignment == MMIDItem::ERight) ? ELeft : ERight;

    if (!TryMovingFocusHorizontally(direction))
    {
        if (iInitialAlignment == MMIDItem::ERight)
        {//RIGHT TO LEFT
            if (!TryMovingFocusVertically(direction == ELeft ? EDown : EUp, EFalse, EFalse))
            {
                SetFocusedItem(KErrNotFound);
            }
        }
        else
        {//LEFT TO RIGHT
            if (!TryMovingFocusVertically(direction == ELeft ? EUp : EDown, EFalse, EFalse))
            {
                SetFocusedItem(KErrNotFound);
            }
        }
    }
}

void CMIDForm::SetFocusedItem(TInt aFocusIdx, TDirection aDirection /*= ENone*/,
                              TBool aDoScroll /*= ETrue*/)
{
    TInt oldFocused = iFocused;
    TInt focusIndexDelta = aFocusIdx - iFocused;
    if (iFocused != KErrNotFound)
    { // actions for the item loosing focus
        CMIDControlItem& control = ControlItem(iFocused);
        control.PostFocusTransferEvent(EFalse, aDirection);
        control.SetFocus(EFalse);
        UpdateItemCommands(NULL, NULL);
    }

    iFocused = aFocusIdx;

    //If there is traverse from visible to invisible TextField, Form has to scroll first
    //before actions for the item which gaining focus are done. Form scrolling provides
    //position settings to all visible items in form.
    //When TextField gains focus, cursor values are calculated. Because this step was done before
    //scrolling, position of edwin in TextField was not set (@see CMIDFormRow::SizeChanged())
    //and cursor was drawn to wrong position.
    //So Form has to scroll first -> this action provides proper position settings
    //to item and items which are part of it (e.g. edwin in TextField) and after that
    //changing focus makes proper cursor drawing.
    if (focusIndexDelta != 0 && aDoScroll)
    {
        if (oldFocused != KErrNotFound)
        {
            aDirection = (focusIndexDelta > 0) ? EDown : EUp;
        }

        ScrollToFocused(aDirection, EFalse);
    }

    if (iFocused != KErrNotFound)
    { // actions for the item gaining focus
        CMIDControlItem& control = ControlItem(iFocused);

        SetHighlightBackgroundRects();

        control.PostFocusTransferEvent(ETrue, aDirection);
        control.SetFocus(ETrue);
        // msk: deliver also the possible MSK command to displayable
        UpdateItemCommands(control.CommandList(), control.GetMSKCommand());
    }
}

/** Updates the model of the scrollbar and moves the thumb. Scroll bar must have
already been layouted.*/
void CMIDForm::UpdateScrollBar()
{
    ASSERT(iSBFrame);
    DEBUG_INT("CMIDForm::UpdateScrollBar - iScroll is %D", iScroll);

    TInt heightOfAllItems = HeightOfAllItems();
    TInt height = heightOfAllItems > Height() ? heightOfAllItems : Height();

    TEikScrollBarModel vSbarModel(height, Height(), -iScroll);
    iSBFrame->Tile(&vSbarModel);
    iSBFrame->MoveVertThumbTo(-iScroll);

    iPopupController->HidePopup(); // hide info popup window if visible
}

CMIDForm* CMIDForm::NewL(MMIDEnv& aEnv,MMIDDisplayable& aDisplayable)
{
    CMIDForm* form = new(ELeave) CMIDForm(aEnv, static_cast<CMIDDisplayable&>(aDisplayable));
    CleanupStack::PushL(form);
    form->ConstructL();
    CleanupStack::Pop(form);
    return form;
}

CMIDForm::CMIDForm(MMIDEnv& aEnv, CMIDDisplayable& aDisplayable)
        : iDisplayable(aDisplayable), iEnv(&aEnv), iFocused(KErrNotFound),
        iInitialAlignment(MMIDItem::ELeft), iHasScrolled(EFalse),
        iLastFadeMessage(0)
{
}

CMIDForm::~CMIDForm()
{
    TRAP_IGNORE(DeleteAllItemsL());
    DeleteRows();
    iCurrentRow = NULL;

    delete iSBFrame;
    SetItemsFormPointersToVal(NULL);
    iItems.Close();
    iRows.Close();

    delete iLayoutTimer;
    delete iNoDataText;

    if (iBackgroundControlContext)
    {
        delete iBackgroundControlContext;
        iBackgroundControlContext = NULL;
    }
    if (iHighlightedBackgroundCc)
    {
        delete iHighlightedBackgroundCc;
        iHighlightedBackgroundCc = NULL;
    }

    // Displayable is notified about content control deletion
    iDisplayable.NotifyContentDestroyed();

    delete iPopupController;

    if (iHighlightTimer)
    {
        iHighlightTimer->Cancel();
    }
    delete iHighlightTimer;

    delete iPhysics;
}

#include "javaoslayer.h"

void CMIDForm::Draw(const TRect& aRect) const
{
    if (!iStartUpTraceDone)
    {
        java::util::JavaOsLayer::startUpTrace("MIDP: CMIDForm::Draw starts", -1, -1);
    }
    if (LayoutPending())
    {
        return;
    }

    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background(skin, iBackgroundControlContext, this, gc, aRect);

    // Draw focus highlight
    if (iFocused != KErrNotFound)
    {
        TRect focusedRect = ControlItem(iFocused).Rect();
        if (focusedRect.Intersects(aRect))
        {
            AknsDrawUtils::Background(AknsUtils::SkinInstance(),
                                      iHighlightedBackgroundCc, this,
                                      SystemGc(), focusedRect);
        }
    }

    // If there is no items, "No data" string is printed to the middle of the screen
    if (iItems.Count() == 0)
    {
        TRAP_IGNORE(DrawNoDataStringL());
    }
    if (!iStartUpTraceDone)
    {
        java::util::JavaOsLayer::startUpTrace("MIDP: CMIDForm::Draw done", -1, -1);
        iStartUpTraceDone = ETrue;
    }
}

void CMIDForm::DrawNoDataStringL() const
{
    CWindowGc& gc = SystemGc();

    // Actual skin instance
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // If there is not the color fot the particular skin and id found, default (black) is used
    TRgb color;
    // Try to receive the color by skin and color Id, if not successful, black is used
    TInt err = AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
    if (err != KErrNone)
    {
        color = AKN_LAF_COLOR(AknLayout::Title_pane_texts_Line_1(0, 0).iC);
    }

    const CFont* font = CMIDFont::DefaultFont(CMIDFont::EDefaultTextId);
    TInt textWidth = font->TextWidthInPixels(iNoDataText->Des());

    // Rectangle for computing the center
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);

    // Centers the string to the middle of the screen
    TInt x = (screenRect.Width() - textWidth) / 2;
    TInt y = (iInnerHeight - font->HeightInPixels()) / 2;
    TPoint point(x, y);
    //Reserve extra space for biditext formatting
    HBufC* bidiTextBuffer = HBufC::NewLC(iNoDataText->Size() + KAknBidiExtraSpacePerLine);
    TPtr bidiText = bidiTextBuffer->Des();
    TPtr caption = iNoDataText->Des();
    bidiText = caption;
    // Handle (bidi)text formatting for all languages
    AknBidiTextUtils::ConvertToVisualAndClipL(bidiText, *font, textWidth, textWidth);

    gc.SetPenColor(color);
    gc.UseFont(font);
    gc.DrawText(bidiText, point);
    CleanupStack::PopAndDestroy(bidiTextBuffer);
    gc.DiscardFont();
}


/** Request a layout operaiton. We must delete the rows if postponing layout
in case java side some of the controls have been deleted. Otherwise because those
controls may be on existing rows we would get a panic in a draw request outside
our control. By deleting the rows in the worse scenario we'll get a blank form
drawn temporarily until the next layout operation. */
void CMIDForm::RequestLayoutL(TBool aImmediately /*= EFalse*/)
{
    if (aImmediately)
    {
        iLayoutTimer->Cancel();
        LayoutAndDrawL();
    }
    else
    {
        if (LayoutPending())
        {
            TTime now;
            now.HomeTime();

            if (now.MicroSecondsFrom(iLayoutRequestTime) > KLayoutCap)
            {
                LayoutAndDrawL();
            }
            else
            {
                iLayoutTimer->Cancel();
            }
        }
        else
        {
            iLayoutRequestTime.HomeTime();
            DeleteRows();
        }

        if (iLayoutRequestTime != -1)
        {
            iLayoutTimer->After(KLayoutTimeout);
        }
    }
}

/** Return true if a layout request has been made. */
TBool CMIDForm::LayoutPending() const
{
    return (iLayoutRequestTime != -1) && (iLayoutTimer->IsActive());
}

/**
Layout the form, do scrolling and draw. Reset any previous layout request.

@see RequestLayout(), LayoutPending(), LayoutFormL() and DoScroll()
*/
void CMIDForm::LayoutAndDrawL()
{
    DEBUG("CMIDForm::LayoutAndDrawL");

    LayoutFormL();

    HandleItemVisibilityChange();
    UpdateScrollBar();
    SetHighlightBackgroundRects();
    DrawDeferred();

    UpdatePhysics();
}

/** Call LayoutAndDrawL() inside a trap harness and log an error message if it
levaes.

    @param aOnlyIfPending Call LayoutAndDrawL() only if there is one pending,
           @see LayoutPending(). By default this parameter is EFalse, i.e. always do layout

    @return The leave code returned by LayoutAndDrawL()
*/
TInt CMIDForm::TrappedLayoutAndDraw(TBool aOnlyIfPending /* = EFalse */)
{
    if (aOnlyIfPending  && !LayoutPending())
    {
        return KErrNone;
    }

    TRAPD(err, LayoutAndDrawL());
    return err;
}

/* Called when the form becomes current and after the focus was set
 * @see CMIDDisplayable::HandleCurrentL()
 */
void CMIDForm::HandleCurrentL(TBool aCurrent)
{
    if (iFocused != KErrNotFound)
    {
        CMIDControlItem& control = ControlItem(iFocused);
        control.HandleCurrentL(aCurrent);
    }

    if (!aCurrent)
    {
        iPopupController->HidePopup();
    }

#ifdef RD_SCALABLE_UI_V2
    iCanDragFocus = EFalse;
#endif
}

void CMIDForm::ConstructL()
{
    UpdateMemberVariables();

    SetContainerWindowL(iDisplayable);

    iPopupController = CMIDPopupNoteController::NewL();

    // Scroll initialisations
#ifdef RD_SCALABLE_UI_V2
    iSBFrame = new(ELeave) CEikScrollBarFrame(this, this, ETrue);
    Window().SetPointerGrab(ETrue);
#else
    iSBFrame = new(ELeave) CEikScrollBarFrame(this, NULL, ETrue);
#endif //RD_SCALABLE_UI_V2

    iSBFrame->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iSBFrame->CreateDoubleSpanScrollBarsL(ETrue, EFalse);

    // Background initialisation
    iBackgroundControlContext = CAknsListBoxBackgroundControlContext::NewL(
                                    KAknsIIDQsnBgAreaMainListGene, Rect(), ETrue,
                                    KAknsIIDQsnBgColumnAB, Rect())  ;  // Rect parameters are only place-holders

    // Background for highlighted item, frame rects are set later
    iHighlightedBackgroundCc = CAknsFrameBackgroundControlContext::NewL(
                                   KAknsIIDQsnFrInput,
                                   TRect(), TRect(), ETrue);

    iDisplayable.SetComponentL(*this);

    // get the layout from shared data so we can tell what our
    // initial alignment should be
    TInt layoutId = 0;

    CRepository* repository = CRepository::NewL(KCRUidAvkon);
    repository->Get(KAknLayoutId, layoutId);
    delete repository;

    if (layoutId == EAknLayoutIdABRW)
    {
        iInitialAlignment = MMIDItem::ERight;
    }

    iLayoutTimer = CTimeOutTimer::NewL(CActive::EPriorityIdle, *this);
    iNoDataText =  iEikonEnv->AllocReadResourceL(R_AVKON_NO_DATA);

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    UpdateTactileFeedbackDensity();
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#endif // RD_TACTILE_FEEDBACK

    //index of pointed control is set to not active
    iIndexPointedControl=-1;

    if (CMIDFormPhysics::FeatureEnabled())
    {
        iPhysics = CMIDFormPhysics::NewL(*this);

        // Destroy iPhysics immediately if physics is not allowed.
        // This should happen in error situation only, when implementation
        // dll couldn't be loaded although the feature was enabled.
        if (iPhysics && !iPhysics->PhysicsAllowed())
        {
            delete iPhysics;
            iPhysics = NULL;
        }
        else
        {
            iHighlightTimer = CPeriodic::NewL(CActive::EPriorityStandard);
        }
    }

    LayoutAndDrawL();
}

void CMIDForm::UpdateMemberVariables()
{
    TAknLayoutRect mainMidpPane;

#ifdef RD_JAVA_S60_RELEASE_9_2
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainMidpPane.LayoutRect(mainPaneRect,
                            AknLayoutScalable_Avkon::main_midp_pane().LayoutLine());
#else
    mainMidpPane.LayoutRect(iEikonEnv->ScreenDevice()->SizeInPixels(),
                            AknLayoutScalable_Avkon::main_midp_pane().LayoutLine());
#endif // RD_JAVA_S60_RELEASE_9_2

    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    iMidpFormRect.LayoutRect(mainMidpPane.Rect(),
                             AknLayoutScalable_Avkon::midp_form_pane(variety).LayoutLine());

    TAknLayoutRect listForm2MidpPane;
    listForm2MidpPane.LayoutRect(iMidpFormRect.Rect(),
                                 AknLayoutScalable_Avkon::list_form2_midp_pane().LayoutLine());

    iFormRect.LayoutRect(listForm2MidpPane.Rect(),
                         AknLayoutScalable_Avkon::form2_midp_field_pane().LayoutLine());

    iInnerWidth = iFormRect.Rect().Width();

    iInnerHeight = iFormRect.Rect().Height();
    iFormHeight = iMidpFormRect.Rect().Height();
}

MMIDItem* CMIDForm::CurrentItem() const
{
    return iFocused == KErrNotFound ? NULL : iItems[iFocused];
}

CMIDItem& CMIDForm::Item(TInt aIndex) const
{
    MMIDItem* item = iItems[aIndex];
    return ControlItem(item->Type());
}

CMIDControlItem& CMIDForm::ControlItem(TInt aIndex) const
{
    return ControlItem(iItems[aIndex]);
}

CMIDControlItem& CMIDForm::ControlItem(MMIDItem* aMMidItem)
{
    CMIDControlItem* base = NULL;
    TInt type  = (TInt) aMMidItem->Type();
    switch (type)
    {
    case MMIDComponent::EImageItem:
        base = static_cast<CMIDImageItem*>(aMMidItem);
        break;
    case MMIDComponent::EStringItem:
        base = static_cast<CMIDStringItem*>(aMMidItem);
        break;
    case MMIDComponent::ESpacer:
        base = static_cast<CMIDSpacer*>(aMMidItem);
        break;
    case MMIDComponent::EGauge:
        base = static_cast<CMIDGaugeItem*>(aMMidItem);
        break;
    case MMIDComponent::ETextField:
        base = static_cast<CMIDTextFieldItem*>(aMMidItem);
        break;
    case MMIDComponent::EChoiceGroup:
        base = static_cast<CMIDChoiceGroupItem*>(aMMidItem);
        break;
    case MMIDComponent::EDateField:
        base = static_cast<CMIDDateFieldItem*>(aMMidItem);
        break;
    case MMIDComponent::ECustomItem:
        base = static_cast<CMIDCustomItem*>(aMMidItem);
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    return *base;
}

TInt CMIDForm::Index(CCoeControl* aControl) const
{
    TInt itemCount = iItems.Count();
    for (TInt i=0; i < itemCount; i++)
    {
        CCoeControl* control = static_cast<CCoeControl*>(&(ControlItem(i)));
        if (control == aControl)
        {
            return i;
        }
    }
    return KErrNotFound;
}

TInt CMIDForm::FormWidth()
{
    return CMIDForm::StaticFormRect().Width();
}

/*static*/ TRect CMIDForm::StaticFormRect()
{
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TRect mainPaneRect;

#ifdef RD_JAVA_S60_RELEASE_9_2
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
#else
    mainPaneRect = iAvkonAppUi->ApplicationRect();
#endif // RD_JAVA_S60_RELEASE_9_2

    TAknLayoutRect mainMidpPane;
    mainMidpPane.LayoutRect(mainPaneRect,
                            AknLayoutScalable_Avkon::main_midp_pane().LayoutLine());

    TAknLayoutRect midpFormRect;
    midpFormRect.LayoutRect(mainMidpPane.Rect(),
                            AknLayoutScalable_Avkon::midp_form_pane(variety).LayoutLine());

    TAknLayoutRect listForm2MidpPane;
    listForm2MidpPane.LayoutRect(midpFormRect.Rect(),
                                 AknLayoutScalable_Avkon::list_form2_midp_pane().LayoutLine());

    TAknLayoutRect formRect;
    formRect.LayoutRect(listForm2MidpPane.Rect(),
                        AknLayoutScalable_Avkon::form2_midp_field_pane().LayoutLine());

    return formRect.Rect();
}


TBool CMIDForm::IsCurrentItem(CMIDItem* aItem) const
{
    return (iFocused != KErrNotFound) &&
           (iFocused == Index(static_cast<CCoeControl*>(aItem)));
}


void CMIDForm::HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
{
    switch (aEventType)
    {
    case MCoeControlObserver::EEventStateChanged:
    {
        TInt index = KErrNotFound;
        User::LeaveIfError(index = Index(aControl));
        MMIDComponent::TType type = iItems[index]->Type();
        if (type == EStringItem || type == EImageItem || type == ECustomItem)
        {
            // These don't change state in the MIDP sense, so it must be a button
            // press so we need to post a default command event, if there is one

            // msk: this is needed if msk is not enabled
            PostDefaultItemCommandEvent(index);
        }
        else
        {
            iEnv->PostJavaEvent(*this,EDisplayable,index);
        }
    }
    break;
    case MCoeControlObserver::EEventRequestFocus:
        if (!aControl->IsNonFocusing())
        {
            SetFocusedItem(Index(aControl));
        }
        break;
    default:
        break;
    }
}


/*
 * Pointer events from different Items are are routed via this function to private HandleControlEventL.
 * Focus is not accepted if Item is not focusable.
 */
void CMIDForm::HandleTouchControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
{

    CMIDControlItem& ci = ControlItem(Index(aControl));
    if (ci.IsSelectable())
    {
        HandleControlEventL(aControl, aEventType);
    }
}


void CMIDForm::TimerExpired()
{
    TrappedLayoutAndDraw();
}

/*
 * Called whenever something's happened which could change the visibility of an item
 * controls. e.g. when made current, when update Items, when scroll.
 */
void CMIDForm::HandleItemVisibilityChange()
{

    // CUSTOM ITEM ONLY
    TInt count = iItems.Count();
    for (TInt i=0; i<count; i++)
    {
        if (iItems[i]->Type() == MMIDComponent::ECustomItem)
        {
            // this will get the CustomItem's FocusChanged method called which will cause
            // some visibility events to be sent
            CMIDControlItem& ci = ControlItem(i);
            ((CMIDCustomItem&) ci).HandleVisibilityChange();
        }
    }
    // END CUSTOM ITEM

}

/** Handles an item size change event. This is called by within the plugin, not by java
side unlike RefreshItemL(TInt).

First of all we check if there is a layout pending operation.
If there is one we don't do anything. This is because this method is called during
control creation when items are inserted in the form. However, it is also called later
on when the control is already displaying and its size changes. During control creation
we should do a delayed layout (for performance reasons) wheras if we delay layout in the
second case we get an unpleasant redraw effect because the item control usually draws
immediately. So we should try and layout+draw immediately in the second case.
We distinguish between the two cases by checking if there is a layout
operation (which is requested by InsertItemL()). Also, in the second case, instead of
doing a full LayoutAndDrawL() we only resize elements already assigned to rows and
update the scroll bars. And then we draw. This is a bit more efficient than recreating
the rows. Because this method is only called by items that fill an entire row anyway,
we shouln't need to re-create rows.
*/
void CMIDForm::RefreshItemL(MMIDItem* aItem)
{
    TInt index = iItems.Find(aItem);

    if (!LayoutPending() && index != KErrNotFound)
    {
        SizeItemsInFormRowsL();
        SetRowExtentsWithRespectToScrolledLocation();

        if ((aItem->Type() == MMIDComponent::ETextField) && (index == iFocused))
        {
            CMIDTextFieldItem* tf = static_cast<CMIDTextFieldItem*>(aItem);
            TPoint p;
            TBool success = tf->GetCursorPosL(p);
            if (success)
            {
                if (p.iY < 0)
                {
                    RawScrollRelative(p.iY + 10);
                }
                else if (p.iY > Height())
                {
                    RawScrollRelative(Height() - (p.iY + 10));
                }
            }
        }
        RawScrollFinalize(ETrue);
    }
}

/**
 * If the item at aIndex has a default command set for it, then this will be invoked and True
 * will be returned. False if a command has not been posted
 */
// msk: this method is needed if msk is not enabled
TBool CMIDForm::PostDefaultItemCommandEvent(TInt aIndex)
{
    if (aIndex != KErrNotFound)
    {
        CMIDCommand* defaultCommand = ControlItem(iItems[aIndex]).DefaultCommand();
        if (defaultCommand)
        {
            iEnv->PostJavaEvent(*iItems[aIndex],EItem,ECommand,defaultCommand->Id());
            return ETrue;
        }
    }
    return EFalse;
}

void CMIDForm::AddItemToFormL(CMIDControlItem& aControlItem)
{
    aControlItem.SetForm(this);
    aControlItem.SetContainerWindowL(*this);
    aControlItem.ItemAddedToFormL();
}

void CMIDForm::RemoveItemFromForm(CMIDControlItem& aControlItem)
{
    aControlItem.MakeVisible(EFalse);
    aControlItem.SetPosition(TPoint(-Width(), 0));
    // Item is not allowed to access container window from now.
    aControlItem.ItemRemovedFromForm();
    aControlItem.SetForm(NULL);
}

void CMIDForm::UpdateItemCommands(CMIDCommandList* aCommandList, CMIDCommand* aMSKCommand)
{
    iDisplayable.SetItemCommandList(aCommandList, aMSKCommand);
}

// msk: this method is needed if msk is not enabled
TBool CMIDForm::DoDefaultCommand()
{
    if (iFocused != KErrNotFound)
    {
        CMIDControlItem& ci = ControlItem(iFocused);

        CMIDCommand* defaultCommand = ci.DefaultCommand();
        if (defaultCommand)
        {
            iEnv->PostJavaEvent(*(iItems[iFocused]), EItem, ECommand, defaultCommand->Id());
            return ETrue;
        }
    }

    return EFalse;
}


TInt CMIDForm::Row(const CMIDControlItem& aItem) const
{
    CMIDFormRow* rowPtr = NULL;
    return Row(aItem, rowPtr);
}

TInt CMIDForm::Row(const CMIDControlItem& aItem, CMIDFormRow*& aRowPtr) const
{
    aRowPtr = NULL;
    TInt rowCount = iRows.Count();
    for (TInt i=0; i < rowCount; i++)
    {
        TInt idx = iRows[i]->Find(&aItem);
        if (idx != KErrNotFound)
        {
            aRowPtr = iRows[i];
            return i;
        }
    }
    return KErrNotFound;
}

CMIDForm::TDirection CMIDForm::ConvertKeyToDirection(TUint aCode)
{
    switch (aCode)
    {
    case EKeyDownArrow:
        return EDown;
    case EKeyUpArrow:
        return EUp;
    case EKeyLeftArrow:
        return ELeft;
    case EKeyRightArrow:
        return ERight;
    default:
        return ENone;
    }
}

// returns the index of a CMIDControlItem in the iItems array. If the
// CMIDControlItem is a CMIDLabelContainerItem, then the index
// of the stringItem associated with it is returned.
TInt CMIDForm::ItemIndex(CMIDControlItem& aItem)
{
    MMIDItem* item = aItem.iMMidItem;

    TInt idx = 0;
    TInt itemCount = iItems.Count();
    while (idx < itemCount)
    {
        if (iItems[idx] == item)
        {
            return idx;
        }
        idx++;
    }

    // OK, so we didn't find it. Maybe it is a CMIDLabelContainerItem
    if (IsLabelContainerItem(aItem))
    {
        CMIDLabelContainerItem* usil = static_cast<CMIDLabelContainerItem*>(&aItem);
        return ItemIndex(usil->StringItem());
    }

    return KErrNotFound;
}

// this method will return the index of the row ABOVE the indicated unconstrained string item
// if the string item was on the top row then KErrNotFound is returned, also if the string item
// could not be found or was not an unconstrained string item.
TInt CMIDForm::FirstRowAboveUnconstrainedStringItem(CMIDStringItem& aStringItem) const
{
    TInt idx = FirstRowOfUnconstrainedStringItem(aStringItem);
    return (idx == KErrNotFound) ? KErrNotFound : idx - 1;
}

// returns the index of the first row of an unconstrained string item.
// Returns KErrNotFound if the string item could not be found (or if it was not
// an unconstrained string item).
TInt CMIDForm::FirstRowOfUnconstrainedStringItem(CMIDStringItem& aStringItem) const
{
    TInt idx = -1;
    TInt rowCount = iRows.Count();
    while (++idx < rowCount)
    {
        CMIDFormRow* row = iRows[idx];
        TInt innerIdx = -1;
        while (++innerIdx < row->NumItems())
        {
            CMIDControlItem* ci = row->Item(innerIdx);
            if (IsLabelContainerItem(*ci))
            {
                CMIDLabelContainerItem* ucsi = static_cast<CMIDLabelContainerItem*>(ci);
                if (&(ucsi->StringItem()) == &aStringItem)
                {
                    return idx;
                }
            }
        }
    }
    return KErrNotFound;
}

TInt CMIDForm::FirstRowBelowUnconstrainedStringItem(CMIDStringItem& aStringItem) const
{
    TInt idx = iRows.Count();
    TInt rowCount = iRows.Count();
    while (--idx < rowCount)
    {
        CMIDFormRow* row = iRows[idx];
        TInt innerIdx = -1;
        while (++innerIdx < row->NumItems())
        {
            CMIDControlItem* ci = row->Item(innerIdx);
            if (IsLabelContainerItem(*ci))
            {
                CMIDLabelContainerItem* ucsi = static_cast<CMIDLabelContainerItem*>(ci);
                if (&(ucsi->StringItem()) == &aStringItem)
                {
                    return ((idx + 1) >= rowCount) ? KErrNotFound : idx + 1;
                }
            }
        }
    }
    return KErrNotFound;
}

TInt CMIDForm::FirstFocusableItemBeforeStartOfUnconstrainedStringItemIdx(CMIDStringItem& aStringItem)
{
    TInt rowIdx = FirstRowOfUnconstrainedStringItem(aStringItem);
    if (rowIdx != KErrNotFound)
    {
        CMIDFormRow* row = iRows[rowIdx];
        TInt idxOnRow = row->NumItems();
        while (--idxOnRow >= 0)  // search backward along the row for the unconstrained string item
            // we probably could assume it is the last element
        {
            CMIDControlItem* ci = row->Item(idxOnRow);
            if (IsLabelContainerItem(*ci))
            {
                CMIDLabelContainerItem* usil = static_cast<CMIDLabelContainerItem*>(ci);
                CMIDStringItem* si = &(usil->StringItem());
                if (si == &aStringItem)
                {
                    break;
                }
            }
        }
        if (idxOnRow > 0)
        {
            // there are some items on the row before the start of the unconstrained string item
            while (--idxOnRow >= 0)  // search backward along the row for the first focusable item
            {
                CMIDControlItem* ci = row->Item(idxOnRow);
                if (ci->IsSelectable())
                {
                    return ItemIndex(*ci);
                }
            }
        }
    }
    return KErrNotFound;
}

TInt CMIDForm::FirstFocusableItemOnLastLineOfUnconstrainedStringItemIdx(CMIDStringItem& aStringItem)
{
    TInt rowIdx = FirstRowBelowUnconstrainedStringItem(aStringItem) - 1;

    if (rowIdx == -2)
    {
        rowIdx = iRows.Count() -1;
    }

    if (rowIdx > KErrNotFound)  // if FirstRowBelowUnconstrainedStringItem returned KErrNotFound then we have -2 so check with '>'
    {
        CMIDFormRow* row = iRows[rowIdx];
        TInt idxOnRow = -1;

        TBool found = EFalse;
        while (++idxOnRow < row->NumItems())
        {
            CMIDControlItem* ci = row->Item(idxOnRow);

            if (ControlBelongsToSameUnconstrainedItem(ci,aStringItem))
            {//ok we found our item, if the next one is selectable we return
                found = ETrue;
            }
            else if (found && ci->IsSelectable())
            {
                return ItemIndex(*ci);
            }
        }
    }
    return KErrNotFound;
}

/** Return true if aCi belongs to the same unconstrained string item (aStringItem) */
TBool CMIDForm::ControlBelongsToSameUnconstrainedItem(CMIDControlItem* aCi, CMIDStringItem& aStringItem) const
{
    if (IsLabelContainerItem(*aCi))
    {
        CMIDLabelContainerItem* usil = static_cast<CMIDLabelContainerItem*>(aCi);
        if (&(usil->StringItem()) == &aStringItem)
        {
            return ETrue;
        }
    }

    return EFalse;
}

/** */
void CMIDForm::RemoveDeletedItem(CMIDControlItem& aItem)
{
    iFocused = KErrNotFound;
    TInt i=-1;
    TInt rowCount = iRows.Count();
    while (++i < rowCount)
    {
        CMIDFormRow* row = iRows[i];
        if (row->RemoveItemIfExists(aItem))
        {
            break;
        }
    }

    i = -1;
    TInt itemCount = iItems.Count();
    while (++i < itemCount)
    {
        if (aItem.iMMidItem)
        {
            if (aItem.iMMidItem == iItems[i])
            {
                iItems.Remove(i);
                break;
            }
        }
    }

}


/////////////////////////////////////////////////////////////////////////////////////////
// new layout methods
/////////////////////////////////////////////////////////////////////////////////////////

// does the layout for the entire form
void CMIDForm::LayoutFormL()
{
    DEBUG("CMIDForm::LayoutFormL");

    if (LayoutPending())
    {
        iLayoutRequestTime = -1;
        iLayoutTimer->Cancel();
    }

    TReal prevHeight = HeightOfAllItems() - Height();
    TInt prevFocusedYPos = 0; // by default it is on top of screen
    TInt focusedItem = iFocused;

    if (focusedItem != KErrNotFound)
    {
        TRect rect = GetControlRect(focusedItem);
        prevFocusedYPos    = rect.iTl.iY;
    }


    // scroll bar layouting
    AknLayoutUtils::LayoutVerticalScrollBar(iSBFrame, Rect(),
                                            AknLayoutScalable_Avkon::scroll_pane_cp51().LayoutLine());
    ResetItemsPreferredSizes();

    // this prevents items from calling layout recursively
    SetItemsFormPointersToVal(NULL);

    AssignItemsToRowsL();
    SizeItemsInFormRowsL();

    SetItemsFormPointersToVal(this);

    SetRowExtentsWithRespectToScrolledLocation();

    TReal newHeight = HeightOfAllItems() - Height();

    if (newHeight != prevHeight)
    {
        // layout has changed significantly, but we need to keep focused component scrolled so that
        // user sees it at the same level in viewport
        TInt newFocusedYPos = 0;
        if (focusedItem != KErrNotFound)
        {
            TRect rect = GetControlRect(focusedItem);
            newFocusedYPos    = rect.iTl.iY;
            TInt delta = prevFocusedYPos - newFocusedYPos;
            if (iScroll != 0)
            {
                // correct only if not on top of window
                ScrollRelative(delta);
            }
        }
        else
        {
            if (newHeight < prevHeight)
            {
                // no focused item, just try to keep same component visible
                iScroll  = newHeight > 0 ? ((TReal) iScroll * (newHeight / prevHeight)) : 0;
            }
        }

        SetRowExtentsWithRespectToScrolledLocation();
    }
}

void CMIDForm::ResetItemsPreferredSizes()
{
    TInt itemCount = iItems.Count();
    for (TInt i=0; i < itemCount; i++)
    {
        CMIDControlItem& ci = ControlItem(i);
        ci.ResetPreferredSize();
    }
}

void CMIDForm::SetItemsFormPointersToVal(CMIDForm* aVal)
{
    TInt itemCount = iItems.Count();
    for (TInt i=0; i < itemCount; i++)
    {
        CMIDControlItem& ci = ControlItem(i);
        ci.SetForm(aVal);
    }
}

/** If an items preferred size is zero
we shouldn't add it to the form row and so we skip it */
TBool CMIDForm::SkipNullItem(TInt aIndex)
{
    CMIDControlItem& ci = ControlItem(iItems[aIndex]);
    if (ci.PreferredSize() == TSize(0, 0))
    {
        if (iFocused == aIndex)
        {
            SetFocusedItem(KErrNotFound);
        }

        return ETrue;
    }

    return EFalse;
}

/**
    If the current row index is the same as the row count,
    create and append to iRows a new row and store it into
    iCurrentRow. Increment the current row index. Also reset
    the current alignment.
*/
void CMIDForm::CreateNewRowAndSetAsCurrentL()
{
    CMIDFormRow* row = CMIDFormRow::NewL(*this);
    CleanupStack::PushL(row);

    User::LeaveIfError(iRows.Append(row));
    CleanupStack::Pop(row);

    iCurrentRow = row;
    iCurrentRow->SetAlignment(iCurrentAlignment);
}

/** Verify the current alignment and change it if the current row
is empty. If the row is not empty and the item alignement is different
and the item requires a row break, then add a new row. */
void CMIDForm::VerifyRowAlignmentL(TInt aIndex)
{
    TInt leftRightMask = MMIDItem::ELeft | MMIDItem::ERight;
    CMIDControlItem& ci = ControlItem(iItems[aIndex]);

    if (iCurrentRow->NumItems() == 0)
    {
        // this is the first item in the row
        if (!LayoutDefault(ci))
        {
            // we take the items alignment for the row
            iCurrentAlignment = MMIDItem::TLayout(ci.Layout() & leftRightMask);
            iCurrentRow->SetAlignment(iCurrentAlignment);
        }
    }
    else if (!LayoutMatchesCurrentAlignment(ci, iCurrentAlignment))
    {
        // the new layout doesn't match the current so go to the next row
        CreateNewRowAndSetAsCurrentL();
        iCurrentAlignment = MMIDItem::TLayout(ci.Layout() & leftRightMask);
        iCurrentRow->SetAlignment(iCurrentAlignment);
    }
}

/**
     Inserts the specified number of new rows (aNumNewLines). If there is a control
    item passed as a parameter it means the lines to be inserted are empty lines
    preceeding the control and therefore should have their empty size set to the
    size of the control.

     @param aControlItem The control item used to set the size of empty rows.

    @param aNumNewLines    The number of rows to be added, if equal to zero this method does
                        nothing
 */
void CMIDForm::InsertNewlinesL(TInt aNumNewlines, CMIDControlItem* aControlItem)
{
    for (TInt idx = 0; idx < aNumNewlines; idx++)
    {
        if (aControlItem && aControlItem->LabelControl() && iCurrentRow && iCurrentRow->NumItems() == 0)
        {
            iCurrentRow->SetEmptyRowSize(TSize(iInnerWidth,aControlItem->OneLineLabelHeight()));
        }

        CreateNewRowAndSetAsCurrentL();
    }
}


/**
 * Creates a new string item, using special type CMIDLabelContainerItem.
 * This is a special string item which has no label and is added to wrap
 * content of a previous string item across several rows.
 *
 *  @see CMIDLabelContainerItem
 */
void CMIDForm::CreateAndAddLabelContainerItemL(CMIDStringItem& aStringItem,
        CEikLabel& aLabel, TBool aIsStringItemContent)
{
    // Label container items items have an horizontal margin only
    // when at the beginning of the row
    TBool hasHMargin = (iCurrentRow->NumItems() == 0);

    CMIDLabelContainerItem* usil =
        CMIDLabelContainerItem::NewL(aStringItem, aLabel, hasHMargin);

    CleanupStack::PushL(usil);
    usil->SetContainerWindowL(*this);
    // sets flag for detecting if label container item is created
    // from string item label or content
    usil->SetIsStringItemContent(aIsStringItemContent);

    iCurrentRow->AppendL(usil);
    CleanupStack::Pop(usil);
}

/**
 *  Inserts a CMIDLabelContainerItem for each line in aTextControl.
 *
 *  @see CreateAndAddLabelContainerItemL()
 */
void CMIDForm::InsertContainerItemsL(CMIDStringItem& aStringItem,
                                     CMIDItemLabel& aTextControl, TBool aIsStringItemContent)
{
    TInt numLines = aTextControl.NumLines();
    for (TInt j = 0; j < numLines; j++)
    { // insert every line as a CMIDLabelContainerItem
        CreateAndAddLabelContainerItemL(
            aStringItem, *(aTextControl.LabelAtIdx(j)), aIsStringItemContent);

        if (j != (numLines-1))
        { // insert a row break except for the last line
            CreateNewRowAndSetAsCurrentL();
        }
    }
}

/**
    Adds an unconstrained string item to one or more rows. If the item
    has a label always insert a line break (new row) before and if it
    is followed by constrained item also after the label. Crop the label
    to the full line width and insert is as a CMIDLabelContainerItem.

    If the item has no label we can start anywhere in the current row.
    Split its content across the remaining width for the first line
    and then across the full line width. Insert a CMIDLabelContainerItem
    for each line and insert a row break after each CMIDLabelContainerItem
    except the last one.

    Item with label can be inserted as default item when one of this
    situation occurs:
    - it is the last item in form
    - following item is constrained
    - following item is unconstrained but has a label

    Item without label can be inserted as default item when:
    - it is the only item in form
    - it is the first item in form and following item is
    unconstrained but has a label (it starts in a new row)

    @see CMILabelContainerItem, InsertContainerItemsL(),
         CreateNewRowAndSetAsCurrentL(), InsertNewlinesL()
*/
void CMIDForm::AddUnconstrainedStringItemL(TInt& aIndex)
{
    CMIDStringItem* si = (CMIDStringItem*)iItems[aIndex];

    TInt widthForText = Width();

    // check if next item is unconstrained and has label
    TBool nextUnconstrainedHasLabel(EFalse);
    if (aIndex < iItems.Count()-1 && ItemIsUnconstrained(aIndex + 1))
    {
        CMIDStringItem* nextItem = (CMIDStringItem*)iItems[aIndex+1];
        if (nextItem->LabelControl() &&
                (nextItem->LabelControl()->Text()->Length() > 0))
        {
            nextUnconstrainedHasLabel = ETrue;
        }
    }

    if (si->LabelControl() && (si->LabelControl()->Text()->Length() > 0))
    {//item has a label - it should be placed in a new row
        if (iCurrentRow->NumItems() > 0)
        {
            CreateNewRowAndSetAsCurrentL();
        }

        // If the item is last or next item is constrained or next item
        // is unconstrained but has a label (it will be placed in a new row)
        // this item is placed as default item.
        if ((aIndex == iItems.Count()-1) || !ItemIsUnconstrained(aIndex + 1)
                || nextUnconstrainedHasLabel)
        {
            AddDefaultItemL(aIndex);
            return;
        }

        // otherwise place the label in one row and then insert the text
        si->LabelControl()->SetWidthL(widthForText);
        // label of string item is inserted as label container item
        // EFalse identifies that label container item is created from string item label
        InsertContainerItemsL(*si, *(si->LabelControl()), EFalse);
        // string items is divided to label container items due to concatenation of contents
        // contained in adjacent string items
        si->SetIsDivided(ETrue);
        CreateNewRowAndSetAsCurrentL();
    }
    else
    { // item has no label

        // If the item is the only item in form or it is the first item in form
        // and the next item is unconstrained with label (it starts at a new row)
        // this item is placed as default item
        if ((iItems.Count() == 1) || (aIndex == 0 && nextUnconstrainedHasLabel))
        {
            AddDefaultItemL(aIndex);
            return;
        }
    }

    // this is either the remaining width on the current row or the full
    // line width if we are on a new row
    TInt remainingLineWidth = widthForText - iCurrentRow->CurrentWidth();

    if ((remainingLineWidth < si->StringContentControl()->FirstWordWidth())
            && (iCurrentRow->NumItems() != 0))
    { // if not even one word fits then go to the next line unless
        // we are already on a new line
        CreateNewRowAndSetAsCurrentL();
        remainingLineWidth = widthForText;
    }

    // wrap the content to the remaining width for the first line and to
    // the full width for any other line
    si->StringContentControl()->SetVariableWidthL(remainingLineWidth, widthForText);
    // content of string item is inserted as label container item
    // ETrue identifies that label container item is created from string item content
    InsertContainerItemsL(*si, *(si->StringContentControl()), ETrue);
}

/**
 * Adds a standard case item to the current row. It verifies if a new
 * row should be added and adds one if so. Then it appends the item
 * to the current row.
 *
 * @see CMIDControlItem, OkToMoveToNextRow()
 */
void CMIDForm::AddDefaultItemL(TInt& aIndex)
{
    CMIDControlItem& ci = ControlItem(iItems[aIndex]);

    TSize itemSize = LayoutShrink(ci) ? ci.MinimumSize() : ci.PreferredSize();
    ci.SetSizeQuiet(itemSize);

    if (OkToMoveToNextRow(itemSize))
    {
        CreateNewRowAndSetAsCurrentL();
    }

    iCurrentRow->AppendL(&ci);
}

/**
    Return true if the given size does not fit into the current
    row and the row is not empty and the size is not bigger than
    an empty row.
*/
TBool CMIDForm::OkToMoveToNextRow(const TSize& aItemSize) const
{
    return ((iCurrentRow->CurrentWidth() + aItemSize.iWidth) > iInnerWidth) &&
           !((aItemSize.iWidth > iInnerWidth) && (iCurrentRow->NumItems() == 0));
}

/**
    Assigns the items to the rows, which are re-created.
*/
void CMIDForm::AssignItemsToRowsL()
{
    DeleteRows();

    //reset current alignment
    iCurrentAlignment = InitialAlignment();

    //Create a row, the form must have at least an empty row
    CreateNewRowAndSetAsCurrentL();

    for (TInt i = 0;  i < iItems.Count(); i++)
    {
        if (SkipNullItem(i))
        { //zero size items are ignored
            continue;
        }

        // if the current alignment changes we may need to change row
        VerifyRowAlignmentL(i);

        CMIDControlItem& ci = ControlItem(iItems[i]);

        // add any extra rows before the current item if needed
        InsertNewlinesL(NumNewLinesBefore(ci), &ci);

        if (ItemIsUnconstrained(i))
        { // unconstrained string items are processed specially
            AddUnconstrainedStringItemL(i);
        }
        else
        { // any other item is processed in here
            AddDefaultItemL(i);
        }

        // add any additional row breaks if needed
        InsertNewlinesL(NumNewLinesAfter(ci), &ci);
    }
}

/** Return true if the item is an unconstrained string item, false otherwise */
TBool CMIDForm::ItemIsUnconstrained(TInt aIndex) const
{
    if (aIndex >= 0 && aIndex < iItems.Count())
    {
        CMIDControlItem& ci = ControlItem(iItems[aIndex]);
        return (IsStringItem(ci) && ((CMIDStringItem*)&ci)->IsUnconstrainedStringItem());
    }

    return EFalse;
}

TBool CMIDForm::LayoutMatchesCurrentAlignment(CMIDControlItem& aControlItem,
        MMIDItem::TLayout aCurrentAlignment)
{
    TInt leftRightMask = MMIDItem::ELeft | MMIDItem::ERight;

    return ((aControlItem.Layout() & leftRightMask) == MMIDItem::EDefault) ||
           ((aControlItem.Layout() & leftRightMask) == aCurrentAlignment);
}

TBool CMIDForm::LayoutDefault(CMIDControlItem& aControlItem)
{
    TInt leftRightMask = MMIDItem::ELeft | MMIDItem::ERight;

    return ((aControlItem.Layout() & leftRightMask) == MMIDItem::EDefault);
}

TBool CMIDForm::LayoutNewLineBefore(CMIDControlItem& aControlItem)
{
    return (aControlItem.Layout() & MMIDItem::ENewLineBefore);
}

TBool CMIDForm::LayoutNewLineAfter(CMIDControlItem& aControlItem)
{
    return (aControlItem.Layout() & MMIDItem::ENewLineAfter);
}

/** Return true if the LAYOUT_SHRINK directive has been set. For
string items which have one or both dimensions of the preferred
size set the layout shrink directive is ignored. */
TBool CMIDForm::LayoutShrink(CMIDControlItem& aControlItem)
{
    if (IsStringItem(aControlItem))
    {
        CMIDStringItem* si = (CMIDStringItem*)(&aControlItem);

        if (si->WidthOrHeightSpecified())
        {
            return EFalse;
        }
    }

    return (aControlItem.Layout() & MMIDItem::EShrink);
}

/** Return true if the LAYOUT_EXPAND directive has been set. For
string items which have one or both dimensions of the preferred
size set the layout expand directive is ignored. */
TBool CMIDForm::LayoutExpand(CMIDControlItem& aControlItem)
{
    if (IsStringItem(aControlItem))
    {
        CMIDStringItem* si = (CMIDStringItem*)(&aControlItem);

        if (si->WidthOrHeightSpecified())
        {
            return EFalse;
        }
    }

    return (aControlItem.Layout() & MMIDItem::EExpand);
}

/** Return true if the LAYOUT_VSHIRNK directive has been set. For
string items which have one or both dimensions of the preferred
size set the layout vertical shrink directive is ignored. */
TBool CMIDForm::LayoutVerticalShrink(CMIDControlItem& aControlItem)
{
    if (IsStringItem(aControlItem))
    {
        CMIDStringItem* si = (CMIDStringItem*)(&aControlItem);

        if (si->WidthOrHeightSpecified())
        {
            return EFalse;
        }
    }

    return (aControlItem.Layout() & MMIDItem::EVerticalShrink);
}

/** Return true if the LAYOUT_VEXPAND directive has been set. For
string items which have one or both dimensions of the preferred
size set the layout vertical expand directive is ignored. */
TBool CMIDForm::LayoutVerticalExpand(CMIDControlItem& aControlItem)
{
    if (IsStringItem(aControlItem))
    {
        CMIDStringItem* si = (CMIDStringItem*)(&aControlItem);

        if (si->WidthOrHeightSpecified())
        {
            return EFalse;
        }
    }

    return (aControlItem.Layout() & MMIDItem::EVerticalExpand);
}

TBool CMIDForm::LayoutVerticalCenter(CMIDControlItem& aControlItem)
{
    TInt layout = aControlItem.Layout();
    layout &= MMIDItem::EVerticalCenter;
    return (layout == MMIDItem::EVerticalCenter);
}

TBool CMIDForm::LayoutBottom(CMIDControlItem& aControlItem)
{
    TInt layout = aControlItem.Layout();
    layout &= MMIDItem::EVerticalCenter;
    return (layout == MMIDItem::EBottom) || (layout == 0);
}

TBool CMIDForm::IsStringItem(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDComponent::EStringItem);
}

TBool CMIDForm::IsStringItemHyperlink(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    if (aControlItem.iMMidItem->Type() == MMIDComponent::EStringItem)
    {
        return (((CMIDStringItem&) aControlItem).Appearance() == MMIDItem::EHyperLink);
    }
    else
    {
        return EFalse;
    }
}

TBool CMIDForm::IsStringItemButton(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    if (aControlItem.iMMidItem->Type() == MMIDComponent::EStringItem)
    {
        return (((CMIDStringItem&) aControlItem).Appearance() == MMIDItem::EButton);
    }
    else
    {
        return EFalse;
    }
}

TBool CMIDForm::IsCustomItem(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDComponent::ECustomItem);
}

TBool CMIDForm::IsGaugeItem(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDComponent::EGauge);
}

TBool CMIDForm::IsTextFieldItem(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDComponent::ETextField);
}

TBool CMIDForm::IsChoiceGroup(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDComponent::EChoiceGroup);
}

TBool CMIDForm::IsDateField(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDComponent::EDateField);
}

TBool CMIDForm::IsLabelContainerItem(CMIDControlItem& aControlItem)
{
    return (aControlItem.Type() == KLabelContainerItem);
}

TBool CMIDForm::IsSpacerUsedForFormatting(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    if (aControlItem.iMMidItem->Type() == MMIDComponent::ESpacer)
    {
        CMIDSpacer* spacer = static_cast<CMIDSpacer*>(&aControlItem);
        return spacer->UsedForStringFormatting();
    }
    return EFalse;
}

TBool CMIDForm::IsImageItem(CMIDControlItem& aControlItem)
{
    if (!aControlItem.iMMidItem)
    {
        return EFalse;
    }
    return (aControlItem.iMMidItem->Type() == MMIDItem::EImageItem);
}

#ifdef RD_JAVA_S60_RELEASE_9_2
void CMIDForm::PostPendingUpEventL()
{
    MMIDItem* curItem = CurrentItem();
    if (curItem)
    {
        CMIDControlItem& item = ControlItem(curItem);

        if (IsChoiceGroup(item))
        {
            CMIDChoiceGroupItem* cgItem = static_cast<CMIDChoiceGroupItem*>(&item);
            cgItem->PostPendingUpEventL();
        }
    }
}
#endif // RD_JAVA_S60_RELEASE_9_2

// returns: The number of new lines before an item.
//          This only takes into account new lines that come from
//          preceeding a string item with new line characters.
TInt CMIDForm::NumNewLinesBefore(CMIDControlItem& aControlItem)
{
    if (IsStringItem(aControlItem))
    {
        CMIDStringItem* si = (CMIDStringItem*)(&aControlItem);
        TInt nLinesBefore = si->NumNewlinesBefore();
        if (nLinesBefore > 0)
        {
            return nLinesBefore;
        }
    }
    if (iCurrentRow && iCurrentRow->NumItems() == 0)
    {
        return 0;
    }
    return LayoutNewLineBefore(aControlItem) ? 1 : 0;
}

// returns: The number of new lines that should appear after an item
//          This takes into account the new lines that can occour from
//          newlines at the end of a string item, and if there are none
//          of these, then it returns 1 if the Layout NewLineAfter
//          directive has been used and 0 otherwise.
TInt CMIDForm::NumNewLinesAfter(CMIDControlItem& aControlItem)
{
    if (IsStringItem(aControlItem))
    {
        CMIDStringItem* si = (CMIDStringItem*)(&aControlItem);
        if (si->NumNewlinesAfter() > 0)
        {
            TInt nLinesAfter = si->NumNewlinesAfter();
            if (nLinesAfter > 0)
            {
                return nLinesAfter;
            }
        }
    }
    return LayoutNewLineAfter(aControlItem) ? 1 : 0;
}

// effects:  Sizes the items within the rows based on their layout characteristics
// modifies: CMIDFormRow
void CMIDForm::SizeItemsInFormRowsL()
{
    TInt rowCount = iRows.Count();
    for (TInt i=0; i < rowCount; i++)
    {
        iRows[i]->SizeItemsL();
    }
}

// effects:  Deletes all rows in the Form. It doesn't affect the items that have
//           been added to the form.
// modifies: CMIDForm, CMIDFormRow
void CMIDForm::DeleteRows()
{
    // By deleting rows we are losing pointers to original items.
    // To avoid any problems with using iPointedControl, which
    // could points to invalid object (e.g. used in
    // CMIDForm::HandleHighlightTimer or
    // in CMIDForm::HandlePhysicsPointerEventL),
    // iPointedControl must be set to NULL.
    iPointedControl = NULL;

    for (TInt i=0; i < iRows.Count(); i++)
    {
        delete iRows[i];
    }

    iRows.Reset();
    iCurrentRow = NULL;
}

TInt CMIDForm::HeightOfAllItems()
{
    return iTotalFormHeight;
}

void CMIDForm::UpdateHeightOfAllItems()
{
    TInt yTopMargin = iFormRect.Rect().iTl.iY - iMidpFormRect.Rect().iTl.iY;
    TInt yBottomMargin = iMidpFormRect.Rect().iBr.iY - iFormRect.Rect().iBr.iY;

    iTotalFormHeight = yTopMargin;

    TInt rowCount = iRows.Count();
    for (TInt i=0; i < rowCount; i++)
    {
        iTotalFormHeight += iRows[i]->Size().iHeight;
    }
    // Form bottom margin is added
    iTotalFormHeight += yBottomMargin;
}

/* HandleResourceChange
 *
 * This method is called after a resource change event, for example after
 * screen dynamic resolution change.
 *
 * In the case of a screen dynamic resolution change we notify all items and
 * repeat the entire form layout.
 *
 * We calculate the % of form that has been scrolled so far and we
 * modify iScroll so that we have scrolled the same % of form after resolution
 * change. Note that the max extent of iScroll is HeightOfAllItems() - Height()
 * and not simply HeightOfAllItems() as one may think, see
 * SetRowExtentsWithRespectToScrolledLocation(). Also note that applying a %
 * scrolling does not necessarily mean the same items will be on screen after a
 * resolution change.
 * Especially when changing from portrait to landscape or vice-versa the
 * number of items on a row vary greatly and as a consequence the number of
 * rows may vary. Therefore, if there is a focused item on screen before
 * resolution change (iFocused not KErrNotFound) we make sure this item is
 * visible and focused after res change by scrolling to it, see SetFocusedItem()
 * and ScrollToFocused().
 */
void CMIDForm::HandleResourceChange(TInt aType)
{
    TInt maxScrollExtentOld = HeightOfAllItems() - Height(); //max extent of iScroll
    CCoeControl::HandleResourceChange(aType);

    if (aType == KEikDynamicLayoutVariantSwitch)
    { // dynamic resolution change
        TInt focusedItem = iFocused;

        UpdateMemberVariables();
        UpdateHeightOfAllItems();

        TInt itemCount = iItems.Count();
        for (TInt i=0; i < itemCount; i++)
        {
            CMIDControlItem& controlItem = ControlItem(i);
            controlItem.ResolutionChange(aType);

            if (controlItem.LabelControl())
            {
                controlItem.LabelControl()->ResolutionChange();
            }
        }

        TInt maxScrollExtentNew = HeightOfAllItems() - Height(); //max extent of iScroll

        TRAPD(err,LayoutFormL());

        if (err == KErrNone)
        {
            if ((maxScrollExtentNew <= 0) && (maxScrollExtentOld > 0))
            {
                // scroll on top
                RawScroll(0);
                if (focusedItem != KErrNotFound)
                {
                    // restore the form item focus
                    CMIDControlItem& control = ControlItem(focusedItem);
                    control.RestoreInnerFocus();
                }
            }
            else
            {
                if ((maxScrollExtentNew > 0) || (maxScrollExtentOld > 0))
                {
                    // if there is something focused
                    if (focusedItem != KErrNotFound)
                    {
                        // restore the form item focus
                        CMIDControlItem& control = ControlItem(focusedItem);
                        ScrollToFocused();
                        control.RestoreInnerFocus();
                    }
                    // if scroll out of maxScrollExtent
                    if (maxScrollExtentNew+iScroll < 0)
                    {
                        // scroll to bottom
                        RawScroll(-maxScrollExtentNew);
                    }

                }
            }
        }

        SetHighlightBackgroundRects();
        HandleItemVisibilityChange();
        UpdateScrollBar();
        DrawDeferred();
    }
    else if (aType == KEikColorResourceChange || aType == KAknsMessageSkinChange ||
             aType == KUidValueCoeColorSchemeChangeEvent)
    {//skin or color scheme change
        TInt itemCount = iItems.Count();
        for (TInt i=0; i < itemCount; i++)
        {
            CMIDControlItem& controlItem = ControlItem(i);
            controlItem.ColorChange(aType);
        }

        // send this event also to form scrollbar
        CEikScrollBar* sb = iSBFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
        if (sb)
        {
            sb->HandleResourceChange(aType);
        }
    }
    else if ((aType == KEikMessageUnfadeWindows) ||
             (aType == KEikMessageFadeAllWindows))
    {
        iLastFadeMessage = aType;
    }
    else if ((aType == KEikMessageWindowsFadeChange) &&
             ((iLastFadeMessage == KEikMessageUnfadeWindows) ||
              (iLastFadeMessage == KEikMessageFadeAllWindows)))
    {
        TInt itemCount = iItems.Count();
        for (TInt i=0; i < itemCount; i++)
        {
            CMIDControlItem& controlItem = ControlItem(i);
            controlItem.HandleWindowFade(iLastFadeMessage == KEikMessageFadeAllWindows);
        }
        iLastFadeMessage = 0;
    }

    UpdatePhysics();

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    UpdateTactileFeedbackDensity();
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

TBool CMIDForm::TryDetectLongTapL(const TPointerEvent &aPointerEvent)
{
    return iDisplayable.TryDetectLongTapL(aPointerEvent);
}

#ifdef RD_SCALABLE_UI_V2
void CMIDForm::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    if (AknLayoutUtils::PenEnabled())
    {

        if (LayoutPending())
        {
            LayoutFormL();
        }

        TInt pixelIndex;

        switch (aEventType)
        {
        case EEikScrollUp:
        {
            pixelIndex = iScroll + (Height() / KScrollAmountDivider);
            break;
        }

        case EEikScrollDown:
        {
            pixelIndex = iScroll - (Height() / KScrollAmountDivider);
            break;
        }

        case EEikScrollPageUp:
        {
            pixelIndex = iScroll + Height();
            break;
        }

        case EEikScrollPageDown:
        {
            pixelIndex = iScroll - Height();
            break;
        }

        default://thumb drag
        {
            //scroll form if thumb position has moved more than 1/10 of the screen height either up or down
            //or thumb position is at top or at bottom of the scrollbar.
            if (-aScrollBar->ThumbPosition() < (iScroll - (Height() / KScrollAmountDividerForThumbDrag))
                    || -aScrollBar->ThumbPosition() > (iScroll + (Height() / KScrollAmountDividerForThumbDrag))
                    || aScrollBar->ThumbPosition() == 0
                    || aScrollBar->ThumbPosition() == aScrollBar->Model()->MaxThumbPos())
            {
                aScrollBar->DrawNow();
                pixelIndex = - aScrollBar->ThumbPosition();
            }
            else
            {
                return;
            }
            break;
        }
        }
        RawScroll(pixelIndex);
        RawScrollFinalize(ETrue);
        UpdatePhysics();
    }
}


TBool CMIDForm::StringItemContainsPoint(CMIDStringItem* aStringItem, const TPoint& aPoint) const
{
    CMIDControlItem* ci = ControlItemAtPoint(aPoint);

    if (ci)
    {
        if (IsLabelContainerItem(*ci))
        {
            CMIDLabelContainerItem *lci = static_cast<CMIDLabelContainerItem*>(ci);
            CMIDStringItem& si = lci->StringItem();
            if (&si == aStringItem)
            { // LabelContainer item is (part of) StringItem
                if (si.IsDivided())
                { // StringItem was divided due to concatenation of contents
                    if (lci->IsStringItemContent())
                    { // return ETrue if is pointed to StringItem content
                        return ETrue;
                    }
                    else
                    { // return EFasle if is pointed to StringItem label
                        return EFalse;
                    }
                }
                else
                { // StringItem was not divided, pointed directly to unconstrained StringItem
                    return ETrue;
                }
            }
        }
        else
        {
            return (ci == aStringItem && aStringItem->TappingActionRect().Contains(aPoint));
        }
    }

    return EFalse;
}

TBool CMIDForm::IsFocusChangingWithPen() const
{
    return iFocusChangingWithPen;
}

#endif //RD_SCALABLE_UI_V2


void CMIDForm::HandleForegroundL(TBool aForeground)
{
    // send HandleForegroundL notification to gauge items
    TInt itemsCount = iItems.Count();
    for (TInt i=0; i < itemsCount; i++)
    {
        if (iItems[i]->Type() == MMIDComponent::EGauge)
        {
            CMIDGaugeItem* gauge = static_cast<CMIDGaugeItem*>(iItems[i]);
            ASSERT(gauge);
            gauge->HandleForegroundL(aForeground);
        }
    }
}

CMIDPopupNoteController* CMIDForm::GetPopupNoteController() const
{
    return  iPopupController;
}

TInt CMIDForm::GetMidpNaviPos()
{
    // get main pane size from CEikAppU
    TRect mainPane = iAvkonAppUi->ApplicationRect();

    // get screen size in pixels
    TAknLayoutRect mainMidpPane;
    mainMidpPane.LayoutRect(iEikonEnv->ScreenDevice()->SizeInPixels(),
                            AknLayoutScalable_Avkon::main_midp_pane().LayoutLine());

    // getting form size depends on screen orientation
    TInt variety = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    iMidpFormRect.LayoutRect(mainMidpPane.Rect(),
                             AknLayoutScalable_Avkon::midp_form_pane(variety).LayoutLine());

    // different between screen size and form size
    TInt xFormDiff = mainMidpPane.Rect().iBr.iX - iMidpFormRect.Rect().iBr.iX;
    // width of navi bar position only for ( iAlignment == MMIDItem::ERight )
    TInt xNaviPos  = iMidpFormRect.Rect().iTl.iX - xFormDiff;

    return xNaviPos;
}

void CMIDForm::UpdatePhysics()
{
    if (iPhysics)
    {
        TSize worldSize(Width(), HeightOfAllItems());
        TSize viewSize(Width(), Height());
        TPoint viewCenter(Width() / 2, -iScroll + Height() / 2);
        TRAP_IGNORE(iPhysics->InitPhysicsL(worldSize, viewSize, viewCenter));
    }
}

TInt CMIDForm::HighlightTimerCallback(TAny* aPtr)
{
    CMIDForm* me = static_cast<CMIDForm*>(aPtr);
    me->HandleHighlightTimer();
    return 0;
}

void CMIDForm::HandleHighlightTimer()
{
    iHighlightTimer->Cancel();

    if (iPointedControl && iPointedControl->IsSelectable())
    {
        // If tactile feedback has not been given already
        // give basic tactile feedback when focus changes.
        // Don't give tactile feedback for button because it is handled
        // by button implementation.
#ifdef RD_TACTILE_FEEDBACK
        if (!iFeedbackExecutedInPointerDown)
        {
            DoFeedbackOnFocusChange(*iPointedControl);
            iFeedbackExecutedInPointerDown = ETrue;
        }
#endif // RD_TACTILE_FEEDBACK

        TInt itemIndex = ItemIndex(*iPointedControl);

        if (itemIndex != iFocused)
        {
            SetFocusedItem(itemIndex, ENone, EFalse);
            DrawDeferred();
        }

        if (!PhysicsScrolling() && !iUpEventSent)
        {
            TRAP_IGNORE(ForwardPointerEventToItemL(iLastPointerDownEvent));
        }
    }
}

void CMIDForm::SetHighlightBackgroundRects()
{
    if (iFocused == KErrNotFound)
    {
        return;
    }

    CMIDControlItem& ci = ControlItem(iFocused);

    // Limit rect size to screen size. If Rect() is way larger
    // than screen, the svg engine will not cope handling that.
    TRect r(ci.Size());
    TRect s;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, s);

    if (r.Intersects(s))
    {
        s.Intersection(r);
    }

    TRect rect(s);
    rect.Normalize(); // ensure positive w & h, just in case

    TAknLayoutRect topLeft ;
    topLeft.LayoutRect(rect, SkinLayout::Input_field_skin_placing__general__Line_2());

    TAknLayoutRect bottomRight;
    bottomRight.LayoutRect(rect, SkinLayout::Input_field_skin_placing__general__Line_5());

    TRect outerRect = TRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
    TRect innerRect = TRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);

    iHighlightedBackgroundCc->SetParentContext(iDisplayable.BackGroundControlContext());
    iHighlightedBackgroundCc->SetFrameRects(outerRect, innerRect);

    UpdateHighlightBackgroundPosition();
}

void CMIDForm::UpdateHighlightBackgroundPosition()
{

    if (iFocused != KErrNotFound)
    {
        CMIDControlItem& ci = ControlItem(iFocused);
        TPoint highlightPosition = ci.PositionRelativeToScreen();
        TRect focusedRect = ci.Rect();

        // Max size of the higlight background is limited to screen size,
        // see SetHighlightBackgroundRects().
        // Need to adjust the highlight position if focused item is partially visible
        // on top of the form and it is larger than screen.
        if (focusedRect.iTl.iY < 0 && focusedRect.iBr.iY > 0)
        {
            TRect screen;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screen);
            if (focusedRect.Height() > screen.Height())
            {
                if (focusedRect.iBr.iY > iSize.iHeight)
                {
                    // Focused item fills the visible form area fully
                    highlightPosition.iY = 0;
                }
                else
                {
                    // Bottom of focused item is in the visible form area
                    highlightPosition.iY += focusedRect.Height() - screen.Height();
                }
            }
        }

        iHighlightedBackgroundCc->SetParentPos(highlightPosition);
    }
}

void CMIDForm::HandlePhysicsPointerEventL(const TPointerEvent& aPointerEvent)
{
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iLastPointerEventType = aPointerEvent.iType;
#endif

    if (iPhysics && iPhysics->CanBeStopped())
    {
        switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
        {
            iPreventPhysicsScrolling = EFalse;
            iFlickStoppedOnDownEvent = iFlickOngoing;
            iFeedbackExecutedInPointerDown = EFalse;
            iPhysics->Stop();
            CMIDControlItem* ci = ControlItemAtPoint(aPointerEvent.iPosition);

            // Physics scrolling was not ongoing and tap hit already selected item.
            // Then forward pointer event to the item.
            if (ci && IsCurrentItem((CMIDItem*)ci) && !iFlickStoppedOnDownEvent)
            {
                iPointedControl = ci;
                ForwardPointerEventToItemL(aPointerEvent);
            }
            else
            {
                // Tactile fedback is executed when kinetic scrolling is stopped on down event.
                if (iFlickStoppedOnDownEvent)
                {
#ifdef RD_TACTILE_FEEDBACK
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                    iFeedback->InstantFeedback(ETouchFeedbackList);
#else
                    iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                    iFeedbackExecutedInPointerDown = ETrue;
#endif // RD_TACTILE_FEEDBACK
                }

                // Pointed control changes.
                if (ci && ci->IsSelectable() && (MMIDItem*)ci != CurrentItem())
                {
                    iFocusChangingWithPen = ETrue;
                }

                // Start highlight timer
                iPointedControl = ci;
                iHighlightTimer->Cancel();
                // Start highlight timer if some any control is tapped.
                if (iPointedControl)
                {
                    TInt highlightTimeout = iPhysics->HighlightDelay() * 1000;
                    iHighlightTimer->Start(TTimeIntervalMicroSeconds32(highlightTimeout),
                                           TTimeIntervalMicroSeconds32(highlightTimeout),
                                           TCallBack(HighlightTimerCallback, this));
                }
            }
            // Setup members
            iStartPosition = aPointerEvent.iPosition;
            iLastPointerDownEvent = aPointerEvent;
            iUpEventSent = EFalse;
            iStartTime.HomeTime();
            iLastDragPosition = aPointerEvent.iPosition;

            break;
        }
        // EDrag
        case TPointerEvent::EDrag:
        {
            TInt dragY = iStartPosition.iY - aPointerEvent.iPosition.iY;
            TInt dragX = iStartPosition.iX - aPointerEvent.iPosition.iX;
            iDisplayable.TryDetectLongTapL(aPointerEvent);

            // Override triggering of physicsScrolling in case of doing text painting in TextField-item
            // or moving slider in Gauge-item.
            if (!iPreventPhysicsScrolling && iPointedControl && iPointedControl->iMMidItem &&
                    iPointedControl->iMMidItem->Type() == MMIDComponent::ETextField)
            {
                // If physics scrolling is not ongoing (abs(dragY) < DragTreshold)
                // then stop flicking and forward event to the item (iPreventPhysicsScrolling = ETrue).
                if ((Abs(dragX) > iPhysics->DragThreshold()) && (Abs(dragY) < iPhysics->DragThreshold()
                        && !iPanningOngoing))
                {
                    iPreventPhysicsScrolling = ETrue;
                }
            }

            // Check whether DragTreshold for panning is exceeded. If yes, then cancel timer and longtap.
            if (!iPanningOngoing && Abs(dragY) > iPhysics->DragThreshold() && !iPreventPhysicsScrolling)
            {
                iPanningOngoing = ETrue;
                iHighlightTimer->Cancel();
                // Start panning from aPointerEvent.iPosition with iPhysics->DragThreshold() margin
                iLastDragPosition = aPointerEvent.iPosition;
                iPhysics->SetPanningPosition(iLastDragPosition - aPointerEvent.iPosition);
                //Forward one drag event to the focused textField or Gauge when panning is triggered
                if ((iFocused != KErrNotFound)
                        && IsTextFieldItem(ControlItem(iFocused)))
                {
                    ControlItem(iFocused).HandlePointerEventL(aPointerEvent);
                }
            }
            // If panning is already ongoing. Update panning position. Also we redraw entire Form
            // to prevent unexpected behavior(artifacts on the screen) during flick scrolling and
            // to make scrolling smoother.
            else if (iPanningOngoing)
            {
                iPhysics->SetPanningPosition(iLastDragPosition - aPointerEvent.iPosition);
                iLastDragPosition = aPointerEvent.iPosition;
                DrawDeferred();
            }
            // If dragged outside of StringItem area or panning starts, then up event is sent to the item.
            if (!iUpEventSent && iPointedControl && IsStringItem(*iPointedControl)  && (iPanningOngoing ||
                    (iPointedControl && !iPointedControl->Rect().Contains(aPointerEvent.iPosition)))
               )
            {
                TPointerEvent pointerEvent = aPointerEvent;
                pointerEvent.iType = TPointerEvent::EButton1Up;
                ForwardPointerEventToItemL(pointerEvent);
                iPointedControl = ControlItemAtPoint(aPointerEvent.iPosition);
                iUpEventSent = ETrue;
            }
            // If physics scrolling is not ongoing forward event to the item.
            else if (!iHighlightTimer->IsActive() && !PhysicsScrolling() &&
                     (iPointedControl && iPointedControl->Rect().Contains(aPointerEvent.iPosition)))
            {
                ForwardPointerEventToItemL(aPointerEvent);
            }
#ifdef RD_JAVA_S60_RELEASE_9_2
            // ChoiceGroup gets drag events while physics scrolling.
            // Enables internal highlight disappearing when panning begins.
            // We have control if there is any Item on Form.
            else if (!iHighlightTimer->IsActive()
                     && (iFocused > KErrNotFound)
                     && IsChoiceGroup(ControlItem(iFocused))
                     && PhysicsScrolling())
            {
                ForwardPointerEventToItemL(aPointerEvent);
            }

#endif // RD_JAVA_S60_RELEASE_9_2             
            break;
        }

        // EButton1Up
        case TPointerEvent::EButton1Up:
        {
            // If physics scrolling is ongoing then start flicking
            // and forward event to the item.
            if (iPanningOngoing && !iPreventPhysicsScrolling)
            {
                TPoint distance = iStartPosition - aPointerEvent.iPosition;
                iFlickOngoing = iPhysics->StartFlick(distance, iStartTime);
            }

            // If physics scrolling is not ongoing then only
            // forward event to the item.
            if (!iUpEventSent)
            {
                ForwardPointerEventToItemL(aPointerEvent);
                iUpEventSent = ETrue;
            }

            iPanningOngoing = EFalse;
            iCanDragFocus = EFalse;
            break;
        }
        default:
        {
            ForwardPointerEventToItemL(aPointerEvent);
            break;
        }
        }
    }
    else
    {
        IgnoreEventsUntilNextPointerUp();
        return;
    }
}

void CMIDForm::ForwardPointerEventToItemL(const TPointerEvent& aPointerEvent)
{
    if (iPointedControl)
    {
        // Drag events are not forwarded to CustomItem
        if (!iFlickStoppedOnDownEvent &&
                !(IsCustomItem(*iPointedControl) && aPointerEvent.iType == TPointerEvent::EDrag))
        {
            if (iFocused != KErrNotFound)
            {
                ControlItem(iFocused).HandlePointerEventL(aPointerEvent);
            }

            if (LayoutPending())
            {
                LayoutFormL();
                DrawDeferred();
            }
        }

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            iFocusChangingWithPen = EFalse;
        }
    }
}

void CMIDForm::HandlePhysicsScroll(TInt aScroll, TBool aDrawNow, TUint /*aFlags*/)
{
    iScroll = iScroll + aScroll;
    if (aScroll)
    {
        iHasScrolled = ETrue;
    }
    iScrollDelta = aScroll;
    SetRowPositionsWithRespectToScrolledLocation();

    if (aDrawNow)
    {
        RawScrollFinalize(ETrue, ETrue);
        DrawNow();
    }
    else
    {
        RawScrollFinalize(ETrue, EFalse);
    }

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    //Tactile feedback on dragging/flicking.
    DoScrollingFeedback();
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

void CMIDForm::PhysicsScrollingEnd()
{
    iFlickOngoing = EFalse;
    iPanningOngoing = EFalse;
}

TBool CMIDForm::PhysicsEnabled()
{
    if (iPhysics)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

TBool CMIDForm::PhysicsScrolling()
{
    return iFlickOngoing | iPanningOngoing;
}

TInt CMIDForm::ScrollDelta()
{
    return iScrollDelta;
}

void CMIDForm::DoFeedbackOnFocusChange(CMIDControlItem& aControlItem)
{
    //Tapping on StringItem BUTTON should not do feedback, here.
    //Native class CAknButton does it by itself
    if (!IsStringItemButton(aControlItem))
    {
        //Other items should do some feedback
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        if (IsStringItemHyperlink(aControlItem))
        {
            //if StringItem is HYPERLIK it should give sensitive button feedback
            iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
        }
        else if (IsImageItem(aControlItem))
        {
            //if pointed control is ImageItem, its focusable, it has to be either
            //HYPERLIK ot BUTTON, so it should give sensitive button feedback
            iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
        }
        else if (IsChoiceGroup(aControlItem))
        {
            //tapping on unfocused ChoiceGroup makes also activation, so it should
            //give basic list feedback
            iFeedback->InstantFeedback(ETouchFeedbackList);
        }
        else
        {
            //changing focus to other items should give sensitive list feedback
            iFeedback->InstantFeedback(ETouchFeedbackSensitiveList);
        }
#else
        iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK                
    }
}

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
void CMIDForm::UpdateTactileFeedbackDensity()
{
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(TRect(), AknLayoutScalable_Avkon::
                          list_single_heading_pane(0).LayoutLine());
    iTactileFeedbackDensity = layoutRect.Rect().Height();
}

void CMIDForm::DoScrollingFeedback()
{
    TRect formRect(Rect());
    TInt topEdgePos = iScroll;
    TInt bottomEdgePos = iScroll + HeightOfAllItems();
    // Calculate distance between current Form content position
    // and previous position, where last feedback happened.
    TInt lastFeedbackDistanceY = Abs(iLastTactileFeedbackPos - iScroll);
    TTouchLogicalFeedback feedbackType(ETouchFeedbackNone);
    if (lastFeedbackDistanceY >= iTactileFeedbackDensity)
    {
        if ((topEdgePos < formRect.iTl.iY)
                && (bottomEdgePos > formRect.iBr.iY))
        {
            // If top (or bottom) edge of the content is outside
            // of visible area of Form, do feedback every time.
            feedbackType = ETouchFeedbackSensitiveList;
        }
        else if (iLastPointerEventType != TPointerEvent::EButton1Up)
        {
            // Begining or end of the content reached (i.e. top or bottom
            // egde of the contont is now visible): feedback is given
            // only if user do panning. No feedback, if touch release
            // happened.
            feedbackType = ETouchFeedbackSensitiveList;
        }
        // Store the position of the content.
        iLastTactileFeedbackPos = iScroll;
    }

    if (iFeedback && feedbackType != ETouchFeedbackNone)
    {
        if (iLastPointerEventType != TPointerEvent::EButton1Up)
        {
            iFeedback->EnableFeedbackForControl(this, ETrue, ETrue);
        }
        else
        {
            // Disable audion feedback on flicking (i.e. user don't pan by
            // touch input).
            iFeedback->EnableFeedbackForControl(this, ETrue, EFalse);
        }
        // Do the feedback if needed.
        iFeedback->InstantFeedback(this, feedbackType);
    }
}
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

