/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The CMIDChoiceGroup control container. Owns and manipulates
*                the choicegroup model and creates / deletes the actual
*                control as needed.
*
*/


#include <e32def.h>
#include <e32err.h>
#include <w32std.h>
#include <gdi.h>
#include <gulutil.h>

#include <eiktxlbm.h>
// CColumnListBoxData API
#include <eikclbd.h>

#include <aknenv.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <aknPopup.h>
#include <AknLayoutDef.h>
#include <AknBidiTextUtils.h>
#include <gulcolor.h>

#include <skinlayout.cdl.h>
#include <AknLayoutFont.h>
#include <aknlayoutscalable_avkon.cdl.h> // LAF
#include <AknsDrawUtils.h>// skin
#include <AknsBasicBackgroundControlContext.h> //skin

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <akntranseffect.h>
#include <akntransitionutils.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// using MMIDBitmapImage API in NewElement and SetElement functions - setting icon to element
#include <lcdgr.h>
// macros definitions for resources
#include <lcdui.rsg>
#ifndef RD_SCALABLE_UI_V2
#include "CMIDForm.h"
#endif

// API used in HandlePointerEventL function
#include "CMIDDisplayable.h"

#include "CMIDFont.h"
// using CMIDUtils::CopyBitmapL in NewElement and SetElement functions - setting icon to element
#include "CMIDUtils.h"
#include "CMIDChoiceGroupControl.h"
// needed for CMIDChoiceGroupItem API in several places
#include "CMIDChoiceGroupItem.h"
// using SetTitleL in DoPopupL function - setting popup title
#include "CMIDItemLabel.h"
// using GetPopupNoteController in ShowInfoPopup function - showing popup with text
#include "CMIDPopupNoteController.h"


#include <j2me/jdebug.h>

// For disabling transition effects
#include <aknlistloadertfx.h>
#include <aknlistboxtfx.h>


/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDChoiceGroupControl.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


// Constant used for calculation of ChoiceGroup, which is not in Form yet
// (from eiklbx.cpp)
const TInt KEikListBoxItemVGap = 6;


CMIDChoiceGroupPopupList* CMIDChoiceGroupPopupList::NewL(CEikListBox* aListBox, TInt aCbaResource,
        AknPopupLayouts::TAknPopupLayouts aType)
{
    CMIDChoiceGroupPopupList* self = new(ELeave) CMIDChoiceGroupPopupList;
    CleanupStack::PushL(self);
    self->ConstructL(aListBox, aCbaResource, aType);
    CleanupStack::Pop(self);
    return self;
}

CMIDChoiceGroupPopupList::~CMIDChoiceGroupPopupList()
{
}

void CMIDChoiceGroupPopupList::ResizeList()
{
    CAknPopupList::SetupWindowLayout(AknPopupLayouts::EMenuUnknownColumnWindow);
}

#ifdef RD_SCALABLE_UI_V2
void CMIDChoiceGroupPopupList::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (aPointerEvent.iType == TPointerEvent::EButton1Down &&
            !Rect().Contains(aPointerEvent.iPosition))
    {
        AttemptExitL(EFalse);
    }
    else
    {
        CAknPopupList::HandlePointerEventL(aPointerEvent);
    }
}
#endif //RD_SCALABLE_UI_V2

CMIDChoiceGroupPopupList::CMIDChoiceGroupPopupList()
{
}

CMIDChoiceGroupControl::CMIDChoiceGroupControl(
    MMIDChoiceGroup::TChoiceType aType,
    CMIDChoiceGroupItem* aItem)
        : iType(aType),
        iItem(aItem)
{
}


CMIDChoiceGroupControl::~CMIDChoiceGroupControl()
{
    // Do not allow the listbox data to delete the icon
    // array. It will be deleted by the model.
    DeleteListBox(iListBox);

    // No need to delete iPopupList here. The popup list is created and deleted in DoPopupL().
    delete iModel;
}


// Constructs the model and adds any given elements into it.
// NOTE that the actual control is not yet created, it comes
// to life when the item is added to a form (that has a window
// for the control to draw in). Constructing and storing the model
// outside the actual control enables elements to be manipulated
// (in Java code) even when the ChoiceGroup is not (yet) added to a form.
void CMIDChoiceGroupControl::ConstructL(
    RArray<TPtrC>& aStringArray,
    RArray<MMIDImage*>& aImageArray)
{
    // Create the listbox model
    iModel = new(ELeave) CMIDChoiceGroupModel(iType);
    iModel->ConstructL(iEikonEnv);

    // Start observing model events
    iModel->SetObserver(this);

    // Stuff any given arrays in the model
    PopulateModelL(aStringArray, aImageArray);

    UpdateMargins();

    SetComponentsToInheritVisibility(ETrue);

#ifdef RD_SCALABLE_UI_V2
    SetAllowStrayPointers();
#endif

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
    // Disabling default tactile feedback to prevent a 'double feedback' while tapping on focused item
    iFeedback->EnableFeedbackForControl(this, FALSE);
#endif

}


// Creates the actual choicegroup control, sets all kinds of
// properties and associates the model with the created control
void CMIDChoiceGroupControl::CreateControlL(
    const CCoeControl* aParent,
    const TRect& aRect)
{
    // Model should be created before the control (this is true for
    // popup as well)
    ASSERT(iModel);

    // In dynamic resolution case store the current item index, so that it can be
    // restored once the listbox is recreated
    TInt currentItem = KErrNotFound;
    if (iListBox)
    {
        currentItem = iListBox->View()->CurrentItemIndex();
    }

    DeleteControl();
    ASSERT(!iListBox);

    // Set parent to the existing (form) window and use the rect given
    SetContainerWindowL(*aParent);
    SetRect(aRect);

    UpdateMargins();

    // If this is a popup, there's nothing to else create really
    if (IsPopup())
    {
        return;
    }

    //in case skin or resolution has changed
    iModel->ReConstructSelectionIconsL();
    iModel->UpdateIconArrayL();

    iListBox = CreateListBoxL(aParent);

    // Disable transition effects for iListBox, this is needed to get
    // rid of artifacts caused by transition effects when list box controls
    // are scrolled up/down.
    CWindowGc* gc = iListBox->View()->ItemDrawer()->Gc();
    MAknListBoxTfx* transApi = CAknListLoader::TfxApi(gc);
    if (transApi)
    {
        transApi->EnableEffects(EFalse);
    }

    // setup listbox
    iListBox->SetListBoxObserver(this);
    iListBox->SetRect(ListBoxRect(Rect()));

    if (currentItem != KErrNotFound)
    {
        iListBox->View()->SetCurrentItemIndex(currentItem);
    }

    // Ready to draw
    ActivateL();
}


// Deletes the actual listbox control (when item is removed from a form)
// and disassociates the model from the control.
// Maintains the model so that elements can be accessed even
// if the item is not attached to a form
void CMIDChoiceGroupControl::DeleteControl()
{
    ClosePopup();

    // Gets rid of the listbox when choicegroup is removed from
    // a form, but does not yet kill the model (ie elements that
    // may have been added)
    DeleteListBox(iListBox);

    // Set to null, otherwise destructor will double-delete
    iListBox = NULL;
}


void CMIDChoiceGroupControl::HiliteFirstElement(TBool aEnableHighlight)
{
    if (iListBox)
    {
        iListBox->View()->SetCurrentItemIndex(0);
        EnableListHighlight(aEnableHighlight);
    }
}

void CMIDChoiceGroupControl::HiliteLastElement(TBool aEnableHighlight)
{
    if (iListBox)
    {
        iListBox->View()->SetCurrentItemIndex(iListBox->View()->BottomItemIndex());
        EnableListHighlight(aEnableHighlight);
    }
}

void CMIDChoiceGroupControl::EnableListHighlight(TBool aEnable)
{
    if (iListBox)
    {
        CListItemDrawer* drawer = iListBox->View()->ItemDrawer();

        if (aEnable)
        {
            drawer->ClearFlags(CListItemDrawer::EDisableHighlight);
        }
        else
        {
            drawer->SetFlags(CListItemDrawer::EDisableHighlight);

#ifdef RD_SCALABLE_UI_V2
            // If the focus is dragged to another form item, the highlight is
            // disabled in this cg. Set the flag to true to be sure that selection is not
            // changed if the pointer up happens on same cg element where the drag started.
            iPointerDragHasStarted = ETrue;
#endif
        }

        TRAP_IGNORE(iListBox->View()->UpdateSelectionL(CListBoxView::ESingleSelection));
    }
}

#ifdef RD_JAVA_S60_RELEASE_9_2
/**
 * Fuction sets ESingleClickDisabledHighlight - item drawer flags
 * and reports current highlight element change event
 */
void CMIDChoiceGroupControl::SingleClickDisableHighlightL(TBool aDisable)
{
    // Highlight should be disabled when listbox element taped
    if (iListBox)
    {
        iListBox->SingleClickDisableHighlightL(aDisable);
    }
}
#endif

// msk
void CMIDChoiceGroupControl::CurrentItemChanged()
{
    if (iItem)
    {
        // delegate the information to the item; commands may need to be updated
        iItem->UpdateCommands();
    }
    // redraw
    DrawNow();
}

// msk
TBool CMIDChoiceGroupControl::IsCurrentItemSelected()
{
    if (iListBox)
    {
        return IsSelected(iListBox->View()->CurrentItemIndex());
    }

    return EFalse;
}

// msk
// Posts always the java event
void CMIDChoiceGroupControl::ToggleCurrentItemSelectionL()
{
    if (iListBox)
    {
        // SelectElementL must not be called if the unselection of the selected element is
        // attempted in exclusive choice group because it would cause the itemStateChanged event.
        if (iType != MMIDChoiceGroup::EExclusive || !IsCurrentItemSelected())
        {
#ifdef RD_JAVA_S60_RELEASE_9_2
            if (Form() && !Form()->PhysicsScrolling())
            {
#endif // RD_JAVA_S60_RELEASE_9_2             
                TInt currentIndex = iListBox->View()->CurrentItemIndex();
                SelectElementL(currentIndex, !IsSelected(currentIndex), ETrue);
#ifdef RD_JAVA_S60_RELEASE_9_2
            }
#endif // RD_JAVA_S60_RELEASE_9_2
        }
    }
}

// returns the numbers of elements in this choice group
TInt CMIDChoiceGroupControl::NumberOfElements() const
{
    ASSERT(iModel);
    return iModel->NumberOfItems();
}

// --- CMIDChoiceGroupItem delegated methods (java Choice interface)


// Inserts a new element in the choice
void CMIDChoiceGroupControl::InsertElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    ASSERT(iModel);

    CMIDChoiceGroupElement* pElement = NewElementLC(aText, aImage);

    // Reserve space for the new element
    iModel->SetReserveL(iModel->NumberOfItems() + 1);

    // Stuff it in
    iModel->InsertElementL(aIndex, pElement);

    CListBoxView* view = NULL;

    if (iListBox)
    {
        view = iListBox->View();
    }
    else if (iPopupList)
    {
        view = iPopupList->ListBox()->View();
    }

    if (view)
    {
        TInt current = view->CurrentItemIndex();

        if (current >= aIndex && current < (iModel->NumberOfItems() - 1))
        {
            // If element was inserted before the highlighted element,
            // move the highlight so that it stays in the same element
            // as before insertion.
            view->SetCurrentItemIndex(++current);
        }
    }

    CleanupStack::Pop(pElement);
}


// Deletes an element from the choice
void CMIDChoiceGroupControl::DeleteElementL(TInt aIndex)
{
    ASSERT(iModel);
    TInt current = 0;
    if (iListBox)
    {
        current = iListBox->View()->CurrentItemIndex();
    }
    iModel->DeleteElementL(aIndex);

    if (iListBox && current != KErrNotFound &&
            iModel->NumberOfItems() > 0 &&
            aIndex <= current)
    {
        if (current < iModel->NumberOfItems())
        {
            if (current > 0 && aIndex < current)
            {
                // Element was deleted before the current item, decrease
                // current so that highlight stays in the same element
                // as before the deletion
                --current;
            }
            iListBox->SetCurrentItemIndex(current);
        }
        else
        {
            iListBox->SetCurrentItemIndex(iModel->NumberOfItems()-1);
        }
    }

}


// Deletes all elements
void CMIDChoiceGroupControl::DeleteAllL()
{
    ASSERT(iModel);
    if (iListBox)
    {
        iListBox->SetCurrentItemIndex(0);
    }
    iModel->DeleteAllL();
}


// Sets properties of element at <aIndex>
void CMIDChoiceGroupControl::SetElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    ASSERT(iModel);

    if (aImage)
    {
        MMIDBitmapImage* bitmapImage = aImage->BitmapImage();

        CFbsBitmap* bitmap = CMIDUtils::CopyBitmapL(bitmapImage->ColorBitmap());
        CleanupStack::PushL(bitmap);

        CFbsBitmap* mask = NULL;
        if (bitmapImage->AlphaBitmap())
        {
            TDisplayMode alphaDisplayMode = bitmapImage->AlphaBitmap()->DisplayMode();
            TBool invert = alphaDisplayMode == EGray2 ? ETrue : EFalse;
            mask = bitmapImage->CreateAlphaBitmapL(alphaDisplayMode, invert);
        }

        CleanupStack::PushL(mask);

        iModel->SetElementL(aIndex, aText, bitmap, mask);

        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);

        bitmapImage->RemoveRef();
    }
    else
    {
        iModel->SetElementL(aIndex, aText, NULL, NULL);
    }
}


// Sets selection state of element at <aIndex>
// This method is also declared in MMIDChoiceGroup. It's implementation
// in CMIDChoiceGroupItem just simply calls this method.
void CMIDChoiceGroupControl::SelectElementL(TInt aIndex, TBool aSelected)
{
    SelectElementL(aIndex, aSelected, EFalse);
}


// Returns selection state of element at <aIndex>
TBool CMIDChoiceGroupControl::IsSelected(TInt aIndex)
{
    ASSERT(iModel);
    CMIDChoiceGroupElement* pElement = iModel->ElementAt(aIndex);

    if (pElement)
    {
        return pElement->IsSelected();
    }

    return EFalse;
}


// Sets element text font
void CMIDChoiceGroupControl::SetFontL(TInt /*aIndex*/, MMIDFont* /*aFont*/)
{
    ASSERT(iModel);
}


// Sets element text fit policy
void CMIDChoiceGroupControl::SetFitPolicyL(TInt /*aFitPolicy*/)
{
    ASSERT(iModel);
}


// --- from MMIDChoiceGroupModelObserver ---

// Called when the model has changed. Based on the change, will cause
// just a redraw of the control, or size recalculation
void CMIDChoiceGroupControl::HandleChoiceGroupModelEventL(
    CMIDChoiceGroupModel* aModel,
    TChoiceGroupModelEvent aEvent)
{
    ASSERT(iModel);

    // If not from our model, ignore
    if (aModel != iModel)
    {
        return;
    }

    // Assume everything needs to be done (makes the switch shorter)
    TBool bRedraw = ETrue;
    TBool bResize = ETrue;

    // Act upon the event, resize and redraw control as necessary
    switch (aEvent)
    {
    case EElementAdded:
        if (iListBox)
        {
            iListBox->HandleItemAdditionL();
        }
        break;
    case EElementDeleted:
        if (iListBox)
        {
            iListBox->HandleItemRemovalL();
        }
        break;
    case EUpdateEnded:
        if (iListBox)
        {
            iListBox->Reset();
        }
        break;
    case EElementModified:
        // after the LAF changes the icon column is dynamic, so redo the setup for the listbox
        // the columns by calling SizeChanged(). bResize is set to false because the relayout of
        // of the whole form is not needed.
        SizeChanged();
        bResize = EFalse;
        break;
    case EElementSelected:
        // Just a redraw will do
        bResize = EFalse;
        break;
    case EUpdateStarted:
    default:
        // No redraw, no resize. Do nothing.
        bRedraw = EFalse;
        bResize = EFalse;
        break;
    }

    // If popup is opened, it is handled separately
    if (iPopupList)
    {
        UpdatePopupListAppearanceL(aEvent);
    }

    // Reporting ESizeChanged event to iItem is not needed since the request
    // to update the form comes from java side (CMIDForm::RefreshItem(TInt) is called)
    // when items are added to or removed from choice group. See ChoiceGroup.java.
    // Redraw if needed and size has not changed, size change will
    // cause a form layout and redraw anyway.
    if (bRedraw && !bResize)
    {
        // Do safe redraw
        if (!IsPopup() && iListBox && iListBox->DrawableWindow())
        {
            iListBox->DrawDeferred();
        }
        else
        {
            // Seems to be a popup - redraw myself
            if (DrawableWindow())
            {
                DrawDeferred();
            }
        }
    }
}


// --- from MEikListBoxObserver ---
// Handles choice listbox events (as in selection)
void CMIDChoiceGroupControl::HandleListBoxEventL(CEikListBox* aControl, TListBoxEvent aEventType)
{
    if (aControl != iListBox)
    {
        return;
    }

    ASSERT(iModel);

    switch (aEventType)
    {
        // These should all cause a selection change
    case MEikListBoxObserver::EEventEnterKeyPressed:
    case MEikListBoxObserver::EEventItemActioned:
        ToggleCurrentItemSelectionL();
        break;
#ifdef RD_JAVA_S60_RELEASE_9_2
    case MEikListBoxObserver::EEventPenDownOnItem:
        if (iListBox)
        {
            iListBox->SetHighlight(EFalse);
        }
        break;
#endif

    default:
        break;
    }
}


// Offers key events to the listbox, or handles popup events here
TKeyResponse CMIDChoiceGroupControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    TKeyResponse resp = EKeyWasNotConsumed;

    // Pop popup if needed, otherwise do nothing (if popup)
    if (IsPopup())
    {
        if ((aKeyEvent.iCode == EKeyOK) || (aKeyEvent.iCode == EKeyEnter))
        {
            // It's a popup, and the OK key was pressed on it - start poppin'
            DoPopupL();
            resp = EKeyWasConsumed;
        }

        return resp;
    }

    // Not a popup then, handle some keys in clever ways

    // Save the currently selected item (if exclusive choice)
    TInt oldSelected = iModel->SelectedElement();
    TInt oldCurrentIndex = -1;

    if (iListBox)
    {
        oldCurrentIndex = iListBox->View()->CurrentItemIndex();
        // Let the listbox take a shot at the key
        resp = iListBox->OfferKeyEventL(aKeyEvent, aType);
    }

    // If click (enter) on an already selected item in an exclusive choice,
    // do not consume the key. This allows the form to display a context menu
    if ((iType == MMIDChoiceGroup::EExclusive) &&
            ((aKeyEvent.iCode == EKeyOK) || (aKeyEvent.iCode == EKeyEnter)) &&
            ((oldSelected != -1) || (oldSelected == -1 && oldCurrentIndex == -1)) &&
            (oldSelected == iModel->SelectedElement()))
    {
        // Do not consume the key, so that form can pop a menu
        CMIDDisplayable& displayable = iItem->Form()->CurrentDisplayable();
        TInt cntOpt = displayable.NumCommandsForOkOptionsMenu();

        // set first command from command list,
        // if command list <= 0 then command sets NULL
        const CMIDCommand* command = (iItem->CommandList()->Count() > 0 ?
                                      iItem->CommandList()->At(0).iCommand :
                                      NULL);

        // for default command will run ProcessCommandL( KItemCommandIdBase + 1 )
        if (iItem->DefaultCommand())
        {
            displayable.ProcessCommandL(iItem->CommandList()->CommandOffset() +
                                        iItem->CommandList()->FindCommandIndex(iItem->DefaultCommand()));

            resp =  EKeyWasConsumed;
        }
        else
        {
            // if ( cntOpt > 1 ) will run menu, else execute ProcessCommandL( CommandOffset )
            if (cntOpt > 1)
            {
                displayable.MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
                resp = EKeyWasConsumed;
            }
            else if (command && command->CommandType() != MMIDCommand::EBack &&
                     command->CommandType() != MMIDCommand::ECancel)
            {
                displayable.ProcessCommandL(iItem->CommandList()->CommandOffset());
                resp =  EKeyWasConsumed;
            }
            else
            {
                displayable.ProcessCommandL(displayable.MainCommandList()->CommandOffset());
                resp =  EKeyWasConsumed;
            }
        }
    }
    // If the event caused current item change, and the new current item
    // is outside the visible area, should request the form to scroll a bit.
    // Current item has not changed, if the key was not consumed
    if (((aKeyEvent.iCode == EKeyUpArrow) ||
            (aKeyEvent.iCode == EKeyDownArrow)) &&
            (resp != EKeyWasNotConsumed))
    {
        // Calc scroll need and request one, if necessary
        RequestScrollIfNeededL();
    }

    if (iListBox && (oldCurrentIndex != iListBox->View()->CurrentItemIndex()))
    {
        CurrentItemChanged();
#ifdef RD_JAVA_S60_RELEASE_9_2
        ShowInfoPopup();
#endif // RD_JAVA_S60_RELEASE_9_2        
    }
    return resp;
}

#ifdef RD_SCALABLE_UI_V2
void CMIDChoiceGroupControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (!AknLayoutUtils::PenEnabled() || !iListBox)
    {

#ifdef RD_TACTILE_FEEDBACK
        // this is to give tactile feedback when a POPUP type CG is focused
        // and user taps it to open the popup list
        // if focus is changing, tactile feedback is given already in Form
        if (IsPopup() &&
                aPointerEvent.iType == TPointerEvent::EButton1Down &&
                !iItem->Form()->IsFocusChangingWithPen())
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iFeedback->InstantFeedback(ETouchFeedbackList);
#else
            iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        }
#endif // RD_TACTILE_FEEDBACK

        return;
    }

    TInt nOldCurrent = iListBox->View()->CurrentItemIndex();
    TBool oldSelected = IsSelected(nOldCurrent);

    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EDrag:
        // In order to enable highlight moving inside the choice group when the focus is dragged
        // on the form, forward drag events directly to listbox. This is needed beacuse it is not
        // always possible to set the wanted grabbing state for CMIDChoiceGroupListBox.
        // See also CCoeControl implementation of pointer event handling.
        iListBox->HandlePointerEventL(aPointerEvent);
        break;
    case TPointerEvent::EButtonRepeat:
        // EButtonRepeat events are requested by form. These events come only when pointer
        // is dragged out of main pane area.
        TimedScroll(aPointerEvent.iPosition.iY < 0 ? CMIDForm::EUp : CMIDForm::EDown);
        break;
    case TPointerEvent::EButton1Down:
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        // Hide pop-up note if visible.
        CMIDForm* form = iItem->Form();
        if (form)
        {
            form->GetPopupNoteController()->HidePopup();
        }
#endif // RD_JAVA_S60_RELEASE_9_2

        iPointerDragHasStarted = EFalse;

#ifdef RD_TACTILE_FEEDBACK
        // give tactile feedback here on ListBox only if it is already focused
        // if focus is changing, tactile feedback is given already in Form
        if (!iItem->Form()->IsFocusChangingWithPen())
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iFeedback->InstantFeedback(ETouchFeedbackList);
#else
            iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        }
#endif // RD_TACTILE_FEEDBACK
        CCoeControl::HandlePointerEventL(aPointerEvent);

        if (iListBox->Rect().Contains(aPointerEvent.iPosition) &&
                !iItem->Form()->IsScrolledOnPointerDown())
        {
            TRect mainPane;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPane);

            TRect rect = iListBox->HighlightRect(); // highlight rect is in propotion to screen,
            rect.Move(0, -mainPane.iTl.iY);         // move it so that it's in propotion to main pane

            // Scroll the form so that highlighted list element becomes fully visible
            TInt scroll = 0;
            if (rect.iBr.iY > mainPane.Height())
            {
                scroll = mainPane.Height() - rect.iBr.iY;
            }
            else if (rect.iTl.iY < 0)
            {
                scroll = -rect.iTl.iY;
            }

            if (scroll != 0)
            {
                ReportEventL(MMIDChoiceGroupControlObserver::EScrollRequest,
                             (TAny*)scroll);
            }
        }
        break;
    }
    case TPointerEvent::EButton1Up:
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        // If up event comes when long tap has been detected, then
        // just forward the pointer event. In that case we do not
        // want to change selection, but update the listbox visually.
        // This is performed when pop-up menu is closed and we want to
        // hide listbox highlight.
        if (iItem->LongTapDetected())
        {
            CCoeControl::HandlePointerEventL(aPointerEvent);
            return;
        }
#endif // RD_JAVA_S60_RELEASE_9_2
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        switch (iType)
        {
        case MMIDChoiceGroup::EMultiple:
            iFeedback->InstantFeedback(ETouchFeedbackCheckbox);
            break;
        default:
            iFeedback->InstantFeedback(ETouchFeedbackList);
            break;
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

        CCoeControl::HandlePointerEventL(aPointerEvent);

        TInt idx;

        // Allow toggling the selection also in a case when form has been scrolled on pointer down event,
        // if the pointer up happens still on cg item area.
        TBool toggleAfterScroll = iItem->Form()->IsScrolledOnPointerDown() &&
                                  iItem->Rect().Contains(aPointerEvent.iPosition);

        if (!iPointerDragHasStarted &&
                (iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, idx) ||
                 toggleAfterScroll))
        {
            ToggleCurrentItemSelectionL();
        }

        // If selected item is tapped in the Exclusive ChoiceGroup
        // then possible default Command is executed.
        if (iItem->DefaultCommand() && !iItem->Form()->PhysicsScrolling())
        {
            if (iType == MMIDChoiceGroup::EExclusive && oldSelected
                    && !iFocusChangingWithPen && !iItem->Form()->IsFocusChangingWithPen())
            {
                iItem->Form()->CurrentDisplayable().ProcessCommandL(iItem->DefaultCommand());
            }
        }

        iFocusChangingWithPen = EFalse;
        break;
    }
    default:
        CCoeControl::HandlePointerEventL(aPointerEvent);
        break;
    }

    // msk: notify if the current index has changed
    if (nOldCurrent != iListBox->View()->CurrentItemIndex())
    {
        iFocusChangingWithPen = ETrue;
        CurrentItemChanged();
        if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
            iPointerDragHasStarted = ETrue;
        }
    }
}

#endif //RD_SCALABLE_UI_V2

// Count component controls in this compound control
TInt CMIDChoiceGroupControl::CountComponentControls() const
{
    if (iListBox)
        return 1;

    return 0;
}


// Return requested component control
CCoeControl* CMIDChoiceGroupControl::ComponentControl(TInt /*aIndex*/) const
{
    return iListBox;
}


/** Called when the size adjustment need is detected, the item notified and
    the item caused the form to relayout. The form will calc the new
    position of the item and cause sizechanged to be called - this is
    this controls chance to update its position.

*/
void CMIDChoiceGroupControl::SizeChanged()
{
    if (iListBox)
    {
        iListBox->SetRect(ListBoxRect(Rect()));
        TRAP_IGNORE(SetupColumnsL(iListBox->ItemDrawer()->ColumnData()));
    }

    if (IsPopup())
    {
        TAknLayoutRect layoutRect; // used only temporarily for getting the layout for popup item's icons and text

        // Rect() includes also the margins so it cannot be used directly when layouting the popup icons and text.
        // popupRect is the rect that is available for the closed popup item (i.e. it does not include the margins).
        // popupRect is used as a parent rect when updating the rects of the popup icons and text.
        layoutRect.LayoutRect(Rect(),
                              AknLayoutScalable_Avkon::form2_midp_field_choice_group_pane().LayoutLine());
        TRect popupRect = layoutRect.Rect();

        // Layout for the element icon
        layoutRect.LayoutRect(popupRect,
                              AknLayoutScalable_Avkon::list_single_midp_graphic_pane_g4_cp(1).LayoutLine());
        iPopupIconRect = layoutRect.Rect();

        // Layout for the text

        iPopupTextLayout.LayoutText(popupRect,
                                    AknLayoutScalable_Avkon::list_single_midp_graphic_pane_t1_cp(1).LayoutLine());
    }
}

void CMIDChoiceGroupControl::PositionChanged()
{
    if (IsPopup())
    {
        SizeChanged();
    }
}

// Calculates and returns the minimum size required by the control
// The size is based on the number of items in the listbox, so that
// the listbox does not need to be scrolled. In popup mode the height
// is fixed.
TSize CMIDChoiceGroupControl::MinimumSize() const
{
    // Start with the current size
    TSize size = Size();

    TInt height = 0;

    // Get height of list box, if not popup, and there is one
    if (!IsPopup())
    {
        // Get current number of list box items.
        TInt numberOfItems = iModel->NumberOfItems();

        if (iListBox)
        {
            // Use item count to calculate listbox height
            height += iListBox->CalcHeightBasedOnNumOfItems(numberOfItems);
        }
        else
        {
            // If there is no iListBox, this cg is not added to form yet.
            // In this case calculate the best estimate for the list box height
            // here.
            // Because none of layout seems to be suitable for this
            // calculation, we have to do it in the same way as CEikLisBox
            // (see CEikListBox::CalcHeightBasedOnNumOfItems in eiklbx.cpp).
            height += (iEikonEnv->NormalFont()->HeightInPixels()
                       + KEikListBoxItemVGap) * numberOfItems;
        }

        height += (numberOfItems > 0)
                  ? (iContentMargins.iTop + iContentMargins.iBottom)
                  : iContentMargins.iBottom;
    }
    else // It is a popup
    {
        TAknLayoutRect layoutRect;
        // layout in respect of the screen as only the height is needed
        layoutRect.LayoutRect(iEikonEnv->ScreenDevice()->SizeInPixels(),
                              AknLayoutScalable_Avkon::
                              list_single_midp_graphic_pane_cp(1, 0).LayoutLine());
        height += layoutRect.Rect().Height()
                  + iContentMargins.iTop + iContentMargins.iBottom;
    }

    // Adjust the size (height only) accordingly
    size.iHeight = height;

    return size;
}

void CMIDChoiceGroupControl::Draw(const TRect& /*aRect*/) const
{
    if (IsPopup())
    {
        CMIDChoiceGroupElement* pElement = iModel->ElementAt(iModel->SelectedElement());

        if (pElement)
        {
            DrawIcon(pElement->Icon());
            DrawText(pElement->Text());
        }
    }
}

// Responds to focus change.
// From CCoeControl.
void CMIDChoiceGroupControl::FocusChanged(TDrawNow aDrawNow)
{
    // call default CCoeControl's implementation
    CCoeControl::FocusChanged(aDrawNow);

    // update focus state of listbox regarding to current focus state
    if (iListBox)
    {
        iListBox->SetFocus(IsFocused(), aDrawNow);
    }
}

// Handles resource change.
// From CCoeControl.
void CMIDChoiceGroupControl::HandleResourceChange(TInt aType)
{
    // call default CCoeControl's implementation
    CCoeControl::HandleResourceChange(aType);

    // update listbox focus state when layout is switched
    if (aType == KEikDynamicLayoutVariantSwitch && iListBox)
    {
        iListBox->SetFocus(IsFocused());
    }
}

TTypeUid::Ptr CMIDChoiceGroupControl::MopSupplyObject(TTypeUid aId)
{
    return iItem->MopSupplyObject(aId);
}

// Closes the popup if it is open
void CMIDChoiceGroupControl::ClosePopup()
{
    if (iPopupList)
    {
        iPopupList->CancelPopup();
    }
}

// --- private functions ---

// Draws an icon in the popup control using LAF as much as possible
void CMIDChoiceGroupControl::DrawIcon(CGulIcon* aIcon) const
{
    // Sanity check
    if (!aIcon)
    {
        return;
    }

    // Extract parts - they're needed later anyway
    CFbsBitmap* bmp = aIcon->Bitmap();
    CFbsBitmap* mask = aIcon->Mask();

    CWindowGc& gc = SystemGc();
    TRect iconRect = iPopupIconRect;

    // Draw the icon centered in its rect

    // Position in icon is (0,0), if the image fits. Else there's a
    // need for a bit of recalc
    TPoint posInIcon = TPoint(0, 0) + (bmp->SizeInPixels() - iconRect.Size());
    posInIcon.iX /= 2;
    posInIcon.iY /= 2;

    // If the thing is negative, the icon fits. Need to move the drawing
    // rect a bit to make the icon draw in the middle
    if (posInIcon.iX < 0)
    {
        // Shift right by amount left over
        iconRect.iTl.iX -= posInIcon.iX;
        posInIcon.iX = 0;
    }

    if (posInIcon.iY < 0)
    {
        // Shift down by amount left over
        iconRect.iTl.iY -= posInIcon.iY;
        posInIcon.iY = 0;
    }

    // Set the part of icon that is to be drawn
    TRect drawRect(posInIcon, iconRect.Size());

    // Draw it
    if (mask)
    {
        gc.BitBltMasked(iconRect.iTl, bmp, drawRect, mask, ETrue);
    }
    else
    {
        gc.BitBlt(iconRect.iTl, bmp, drawRect);
    }
}


/** Draws the selected item text in the popup control */
void CMIDChoiceGroupControl::DrawText(const TDesC& aText) const
{
    if (aText.Length())
    {
        // Get color from skin
        TRgb rgb = AKN_LAF_COLOR(215);
        TInt textColor;

        // Set color for text according to item highlight
        // (logical color constants are defined in lcdui.h)
        if (iItem && iItem->IsHighlighted())
        {
            textColor = KHighlightedItemTextColor;
        }
        else
        {
            textColor = KNonHighlightedItemTextColor;
        }

        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                                  rgb, KAknsIIDQsnTextColors, textColor);
        iPopupTextLayout.DrawText(SystemGc(), aText, ETrue, rgb);
    }
}


// Populates the listbox model from given arrays
void CMIDChoiceGroupControl::PopulateModelL(
    RArray<TPtrC>& aStringArray,
    RArray<MMIDImage*>& aImageArray)
{
    if (!iModel)
    {
        // Should not happen.
        ASSERT(EFalse);
        return;
    }

    // Begin model update. The model defers updating
    // some of its internal stuff until EndUpdate()
    iModel->BeginUpdate();

    // Loop through the string & image arrays and add elements,
    // pass model from here so that it doesn't need to be retrieved
    // from the box all the time
    TInt nCount = aStringArray.Count();
    for (TInt i = 0; i < nCount; i++)
    {
        iModel->AppendElementL(NewElementLC(aStringArray[i], aImageArray[i]));
        CleanupStack::Pop();  // NewElementLC
    }

    // End model update
    iModel->EndUpdate();
}

// Create a new element, initialise with string & image
CMIDChoiceGroupElement* CMIDChoiceGroupControl::NewElementLC(
    const TDesC& aString, MMIDImage* aImage)
{
    CMIDChoiceGroupElement* pElement = CMIDChoiceGroupElement::NewL();
    CleanupStack::PushL(pElement);

    pElement->SetTextL(aString);

    if (aImage)
    {
        MMIDBitmapImage* bitmapImage = aImage->BitmapImage();

        CFbsBitmap* bitmap = CMIDUtils::CopyBitmapL(bitmapImage->ColorBitmap());
        CleanupStack::PushL(bitmap);

        CFbsBitmap* mask = NULL;
        if (bitmapImage->AlphaBitmap())
        {
            TDisplayMode alphaDisplayMode = bitmapImage->AlphaBitmap()->DisplayMode();
            TBool invert = alphaDisplayMode == EGray2 ? ETrue : EFalse;
            mask = bitmapImage->CreateAlphaBitmapL(alphaDisplayMode, invert);
        }

        CleanupStack::PushL(mask);

        pElement->SetIconL(*bitmap, mask);
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);

        bitmapImage->RemoveRef();
    }

    return pElement;
}


// Reports control events to observer
void CMIDChoiceGroupControl::ReportEventL(
    MMIDChoiceGroupControlObserver::TChoiceGroupControlEvent aEvent,
    TAny* aParam /* == NULL */)
{
    if (iObserver)
    {
        iObserver->HandleChoiceGroupControlEventL(this, aEvent, aParam);
    }
}


// Calculates whether scrolling the current lbox item into view
// is in order, and requests a scroll from the form (informs item
// of a svcroll request event) if necessary
void CMIDChoiceGroupControl::RequestScrollIfNeededL()
{
    // Scroll should never be needed in a popup CG, and definitely
    // not, if there is no listbox
    if ((IsPopup()) || (!iListBox))
    {
        return;
    }

    // Start with the form rect
    TRect formRect = iItem->FormRect();

    if (formRect.IsEmpty())
    {
        return;
    }

    TInt currentItem = KErrNotFound;
#ifdef RD_JAVA_S60_RELEASE_9_2
    // First check if top visible item was set (only in case that
    // ChoiceGroup was panned and now it is partially visible and
    // got highlight by starting HW keys interaction).
    // If item was set, do not scroll at all.
    currentItem = iListBox->TopVisibleItemIndex();
    if (currentItem == KErrNotFound)
    {
        // Top visible item was not set, i.e. ChoiceGroup already
        // have highlight or it is fully visible or it is fully invisible.
        currentItem = iListBox->CurrentItemIndex();
    }
#else
    currentItem = iListBox->CurrentItemIndex();
#endif // RD_JAVA_S60_RELEASE_9_2

    // Calculate current listbox item rect
    TRect lbitemRect = TRect(
                           iListBox->View()->ItemPos(currentItem),
                           iListBox->View()->ItemSize(currentItem));

    // CG item rect is also needed
    TRect cgitemRect = iItem->Rect();

    // Amount to be scrolled
    TInt nScroll = 0;

    // If the current item is even partly outside the form, should
    // scroll. Scroll amount is either the selected item border to
    // middle of form rect, or the rest of the CG visible, whichever is
    // smaller.
    if (lbitemRect.iBr.iY > formRect.iBr.iY)
    {
        // Items bottom is below the form bottom - need to scroll up (-)
        nScroll = Min((lbitemRect.iTl.iY - (formRect.Height() / 2)),
                      ((cgitemRect.iBr.iY - formRect.iBr.iY) + 1));
        // Scroll up is negative
        nScroll = -nScroll;
    }
    else if (lbitemRect.iTl.iY < formRect.iTl.iY)
    {
        // Items top is above the form top - need to scroll down (+)
        nScroll = Min((((formRect.Height() / 2) - lbitemRect.iBr.iY) + 1),
                      (formRect.iTl.iY - cgitemRect.iTl.iY));
    }
    else
    {
        // There should be no need to scroll
        return;
    }

    // Report a scrolling request
    ReportEventL(
        MMIDChoiceGroupControlObserver::EScrollRequest,
        (TAny*) nScroll);
}


void CMIDChoiceGroupControl::RestoreFocus()
{
    // The sub-item focus restore is done by scrolling the form
    // Exceptions can be ignored, because failing of RequestScrollIfNeededL
    // may cause only a cosmetic visual problems
    TRAP_IGNORE(RequestScrollIfNeededL());
}


// Pops up the popup choice (creates a listbox, gives it to the
// popup for content, gives the popup in turn as parent to the
// listbox construction, sets title, creates scrollbars and
// pop goes the weasel
void CMIDChoiceGroupControl::DoPopupL()
{
    //in case skin or resolution has changed
    iModel->ReConstructSelectionIconsL();
    iModel->UpdateIconArrayL();

    // In order for the listbox to have popup as parent, it
    // needs to be allocated and constructed separately
    CMIDChoiceGroupListBox* pListBox = AllocateListBoxL();
    CleanupStack::PushL(pListBox);

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Create the popup with the listbox as popup content
    CMIDChoiceGroupPopupList* pPop = CMIDChoiceGroupPopupList::NewL(
                                         pListBox,
                                         R_MIDP_SOFTKEYS_CANCEL,
                                         AknPopupLayouts::EMenuUnknownColumnWindow);
#else

    // Create the popup with the listbox as popup content
    CMIDChoiceGroupPopupList* pPop = CMIDChoiceGroupPopupList::NewL(
                                         pListBox,
                                         R_MIDP_SOFTKEYS_OK_BACK,
                                         AknPopupLayouts::EMenuUnknownColumnWindow);
#endif // RD_JAVA_S60_RELEASE_9_2
    CleanupStack::PushL(pPop);

    // Now actually construct the listbox with the popup as parent
    ConstructListBoxL(pListBox, pPop);

    // We need some internal vertical scroll stuff in popup - set it up
    pListBox->CreateScrollBarFrameL(ETrue);
    pListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto);
    // Drawing of scrollbar background is disabled - avoid improper
    // drawing in landscape mode and appearance is consistent
    // with native side
    pListBox->ScrollBarFrame()->DrawBackground(EFalse, EFalse);

    // Set the popup title to be the same as the choicegroup label
    pPop->SetTitleL(*(iItem->LabelControl()->Text()));

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Show highlight in pop-up ChoiceGroup.
    pListBox->DisableSingleClick(ETrue);
#endif // RD_JAVA_S60_RELEASE_9_2

    // set the current item to the one we currently have selected
    if (iModel->SelectedElement() >= 0)
    {
        pListBox->SetCurrentItemIndex(iModel->SelectedElement());
    }

    iPopupList = pPop;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    //When popup list is opening, do the feedback
    if (CAknTransitionUtils::TransitionsEnabled(
                AknTransEffect::EComponentTransitionsOff))
    {
        iFeedback->InstantFeedback(ETouchFeedbackIncreasingPopUp);
    }
    else
    {
        iFeedback->InstantFeedback(ETouchFeedbackPopUp);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    TInt popOK = pPop->ExecuteLD();
    iPopupList = NULL;

    CleanupStack::Pop(pPop);

    // Examine result
    if (popOK)
    {
        // Retrieve the selection, and set in model
        TInt nSelected = pListBox->CurrentItemIndex();
        if (nSelected != iModel->SelectedElement())
        {
            SelectElementL(nSelected, ETrue, ETrue);
        }
    }
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    else
    {
        //Popup list was closed without any selection:
        //-user canceled it
        //-user tapped outside of choice list dialog
        //Do the closing feedback for these two cases
        if (CAknTransitionUtils::TransitionsEnabled(
                    AknTransEffect::EComponentTransitionsOff))
        {
            iFeedback->InstantFeedback(ETouchFeedbackDecreasingPopUp);
        }
        else
        {
            iFeedback->InstantFeedback(ETouchFeedbackPopUp);
        }
    }
#endif //RD_RD_ADVANCED_TACTILE_FEEDBACK

    // The icon column may need to be added or removed,
    // SizeChanged does the relayout
    SizeChanged();

    // Cleanup
    CleanupStack::Pop(pListBox);
    DeleteListBox(pListBox);

    iItem->DrawDeferred();
}


// Creates the choice listbox
CMIDChoiceGroupListBox* CMIDChoiceGroupControl::CreateListBoxL(
    const CCoeControl* aParent)
{
    // Allocate
    CMIDChoiceGroupListBox* pListBox = AllocateListBoxL();
    CleanupStack::PushL(pListBox);

    // Construct
    ConstructListBoxL(pListBox, aParent);
    CleanupStack::Pop(pListBox);

    return pListBox;
}


// Allocates the choice listbox.
CMIDChoiceGroupListBox* CMIDChoiceGroupControl::AllocateListBoxL()
{
    return (new(ELeave) CMIDChoiceGroupListBox(this));
}


// Constructs the choice listbox with proper flags and parent
void CMIDChoiceGroupControl::ConstructListBoxL(
    CMIDChoiceGroupListBox* aListBox,
    const CCoeControl* aParent)
{
    TInt flags(CEikListBox::ELeftDownInViewRect |
               CEikListBox::EPaintedSelection |
               CEikListBox::EKeepModel);
#ifdef RD_JAVA_S60_RELEASE_9_2
    flags |= CEikListBox::ENoExtendedSelection;
#endif // RD_JAVA_S60_RELEASE_9_2

    // Construct with the model passed in (do not allow the listbox
    // to create its own model)
    aListBox->ConstructL(aParent, flags, iModel);

    // Grab the column data
    CColumnListBoxData* columnData = aListBox->ItemDrawer()->ColumnData();

    // Set up columns
    SetupColumnsL(columnData);

    // Set icon array
    // NOTE that the listbox (columndata) does not own the
    // icon array. Some things are needed in deletion to make
    // sure that the array is not deleted with the listbox
    columnData->SetIconArray(iModel->IconArray());
}


// Deletes the choice listbox (taking care not to destroy the icon array)
void CMIDChoiceGroupControl::DeleteListBox(CMIDChoiceGroupListBox* aListBox)
{
    // NOTE that the iconarray in the listbox data needs to be
    // set to NULL, otherwise the array contents will be deleted, which
    // is not good, since it is actually the model that owns the icons
    if (aListBox)
    {
        CColumnListBoxData* columnData = aListBox->ItemDrawer()->ColumnData();
        columnData->SetIconArray(NULL);
        
        if (aListBox->IsFocused())
        {
            aListBox->SetFocus(EFalse);
        }
        delete aListBox;
    }
}

/**
* Functions sets and layouts column data (part of list box used for choicegroup):
* Column 0 (ERadioButtonOrCheckBoxColumn) is used for radiobuton/checkbox icons
* Column 1 (EUserDefinedImagesColumn) is used for user defined images
* Column 2 (ETextColumn) is used for text
* Each colum has specific layout.
*
* @param aColumnData pointer to CColumnListBoxData
*/
void CMIDChoiceGroupControl::SetupColumnsL(CColumnListBoxData* aColumnData)
{
    ASSERT(aColumnData);

    if (iModel->HasIcons())
    { //ChoiceGroup has user defined images
        TAknTextLineLayout textlayout;
        TAknWindowLineLayout graphiclayout;

        if (iType != MMIDChoiceGroup::EPopup)
        { //ChoiceGroup
            //Normal ChoiceGroup must have radiobuton/checkbox icons
            //if MULTIPLE or EXCLUSIVE type is set.
            //We set and layout 0. column for those icons.
            aColumnData->SetGraphicSubCellL(ERadioButtonOrCheckBoxColumn,
                                            AknLayoutScalable_Avkon::
                                            list_single_2graphic_pane_g2_cp4().LayoutLine());
            //layout for text
            textlayout = AknLayoutScalable_Avkon::
                         list_single_2graphic_pane_t1_cp4().LayoutLine();
            //layout for user defined image
            graphiclayout = AknLayoutScalable_Avkon::
                            list_single_2graphic_pane_g1_cp4().LayoutLine();
        }
        else
        { //Popup ChoiceGroup - must not have radiobuton/checkbox icons,
            // so that 0. column is not layouted.
            //text layout
            textlayout = AknLayoutScalable_Avkon::
                         list_single_graphic_pane_t1_cp2(0).LayoutLine();
            //layout for user defined image
            graphiclayout = AknLayoutScalable_Avkon::
                            list_single_graphic_pane_g1_cp2(0).LayoutLine();
        }
        //set and layout remaining columns
        aColumnData->SetTextSubCellL(ETextColumn, textlayout);
        aColumnData->SetGraphicSubCellL(EUserDefinedImagesColumn,
                                        graphiclayout);
    }
    else
    { //ChoiceGroup doesn't have user defined images
        if (iType != MMIDChoiceGroup::EPopup)
        { //ChoiceGroup
            //Normal ChoiceGroup must have radiobuton/checkbox icons
            //if MULTIPLE or EXCLUSIVE type is set.
            //We set and layout 0. column for those icons.
            aColumnData->SetGraphicSubCellL(ERadioButtonOrCheckBoxColumn,
                                            AknLayoutScalable_Avkon::
                                            list_single_midp_graphic_pane_g4_cp().LayoutLine());
            // Set and layout text column.
            aColumnData->SetTextSubCellL(ETextColumn,
                                         AknLayoutScalable_Avkon::
                                         list_single_midp_graphic_pane_t1_cp().LayoutLine());
        }
        else
        { //Popup ChoiceGroup - must not have radiobuton/checkbox icons,
            // so that 0. column is not layouted.
            // Set and layout text column.
            aColumnData->SetTextSubCellL(ETextColumn,
                                         AknLayoutScalable_Avkon::
                                         list_single_pane_t1_cp2(0).LayoutLine());
        }
        // Hide the 1. graphic column when there are no user defined images
        TAknWindowLineLayout layout =
            AknLayoutScalable_Avkon::
            list_single_2graphic_pane_g1_cp4().LayoutLine();
        layout.iW = 0;  // zero width
        aColumnData->SetGraphicSubCellL(EUserDefinedImagesColumn, layout);
    }
}

// Calculate the rect to assign to the listbox.
TRect CMIDChoiceGroupControl::ListBoxRect(const TRect& aRect) const
{
    TRect rect(aRect);

    if (iListBox)
    {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(rect, AknLayoutScalable_Avkon::
                              form2_midp_field_choice_group_pane().LayoutLine());
        rect = layoutRect.Rect();
    }

    return rect;
}


// Sets selection state of element at <aIndex>
// This a private method. There is also a public method
// CMIDChoiceGroupControl::SelectElementL(TInt aIndex, TBool aSelected)
// which calls this one.
void CMIDChoiceGroupControl::SelectElementL(TInt aIndex, TBool aSelected, TBool aPostEvent)
{
    ASSERT(iModel);
    iModel->SelectElementL(aIndex, aSelected);
    iItem->UpdateCommands();

    if (aPostEvent)
    {
        iItem->PostItemStateChangedEventL();
    }
}


void CMIDChoiceGroupControl::UpdateMargins()
{
    TAknLayoutRect layoutRect;
    // use the screen as a parent only the margins are calculated
    TRect screenRect(iEikonEnv->ScreenDevice()->SizeInPixels());
    layoutRect.LayoutRect(screenRect, AknLayoutScalable_Avkon::
                          form2_midp_field_choice_group_pane().LayoutLine());

    TRect contentRect = layoutRect.Rect();

    iContentMargins.iTop    = contentRect.iTl.iY;
    iContentMargins.iBottom = screenRect.iBr.iY - contentRect.iBr.iY;
    iContentMargins.iLeft   = contentRect.iTl.iX;
    iContentMargins.iRight  = screenRect.iBr.iX - contentRect.iBr.iX;
}

void CMIDChoiceGroupControl::UpdatePopupListAppearanceL(TChoiceGroupModelEvent aEvent)
{
    if (!iPopupList)
    {
        return;
    }

    // resize is done by default
    TBool bResize = ETrue;

    // Act upon the event, resize and redraw the popup list as necessary
    // EUpdateEnded and EUpdateStarted events are not possible in this
    // function as they occur only when choice group is constructed
    switch (aEvent)
    {
    case EElementAdded:
        iPopupList->ListBox()->HandleItemAdditionL();
        break;
    case EElementDeleted:
        iPopupList->ListBox()->HandleItemRemovalL();

        // Popup list loses focus if the highlighted is in the last element and
        // an element is removed. Set focus to last element in this case.
        if (iPopupList->ListBox()->View()->CurrentItemIndex() == KErrNotFound &&
                iModel->NumberOfItems() > 0)
        {
            iPopupList->ListBox()->View()->SetCurrentItemIndex(iModel->NumberOfItems() - 1);
        }
        break;
    case EElementModified:
        // after the LAF changes the icon column is dynamic in popup list, so resize
        // may be needed after image has been added to or removed from element
        break;
    case EElementSelected:
        // Just a redraw will do
        bResize = EFalse;
        break;
    default:
        bResize = EFalse;
        break;
    }

    if (bResize)
    {
        // cast is safe as CMIDChoiceGroupListBox is inherited from CAknColumnListBox
        SetupColumnsL(static_cast<CEikColumnListBox*>(
                          iPopupList->ListBox())->ItemDrawer()->ColumnData());
        iPopupList->ResizeList();
    }

    iPopupList->DrawDeferred();
}

void CMIDChoiceGroupControl::ShowInfoPopup()
{
    if (!iListBox || !IsVisible())
    {
        return;
    }

    TInt index = iListBox->View()->CurrentItemIndex();
    if (index >= 0)
    {
        CMIDForm* form = iItem->Form();
        if (form)
        {
            form->GetPopupNoteController()->HidePopup();

            if (static_cast<CEikColumnListBox*>(iListBox)->ItemDrawer()->ColumnData()->
                    CurrentItemTextWasClipped())
            {
                CMIDChoiceGroupElement* pElement = iModel->ElementAt(index);
                if (pElement)
                {
                    TRect mainPane;
                    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPane);

                    // HighLight rect returns the rect in propotion to whole screen,
                    // move it in propotion to the main pane to check if its visible.
                    TRect rect = iListBox->HighlightRect();
                    TRect temp = rect;
                    temp.Move(0, -mainPane.iTl.iY);
                    if (form->RectPartiallyVisible(temp))
                    {
                        TRAP_IGNORE(form->GetPopupNoteController()->ShowPopupL(pElement->Text(),
                                    rect));
                    }
                }
            }
            else
            {
                DEBUG("CMIDChoiceGroupControl::ShowInfoPopup(): text not clipped");
            }
        }
    }
}

#ifdef RD_SCALABLE_UI_V2
void CMIDChoiceGroupControl::TimedScroll(CMIDForm::TDirection aDirection)
{
    ASSERT(iListBox);

    TRect itemRect = iItem->Rect();
    TInt formHeight = iItem->FormRect().Height();

    // There must be something to scroll
    if ((aDirection == CMIDForm::EDown && itemRect.iBr.iY <= formHeight) ||
            (aDirection == CMIDForm::EUp   && itemRect.iTl.iY >= 0))
    {
        return;
    }

    iPointerDragHasStarted = ETrue;

    TInt scroll = 0;
    TInt elementHeight = iListBox->ItemHeight();
    TInt index = FirstInvisibleElement(aDirection);

    // If valid index was found, adjust scroll so that the element at index will be fully visible.
    // Otherwise scroll height of one element or to the bottom/top of the item.
    if (aDirection == CMIDForm::EDown)
    {
        if (index != KErrNotFound)
        {
            scroll = -(iListBox->View()->ItemPos(index).iY + elementHeight - formHeight);
        }
        else
        {
            scroll = Max(-elementHeight, -(itemRect.iBr.iY - formHeight));
        }
    }
    else if (aDirection == CMIDForm::EUp)
    {
        if (index != KErrNotFound)
        {
            scroll = -iListBox->View()->ItemPos(index).iY;
        }
        else
        {
            scroll = Min(elementHeight, -itemRect.iTl.iY);
        }
    }

    if (index != KErrNotFound)
    {
        iListBox->View()->SetCurrentItemIndex(index);
    }

    TRAP_IGNORE(ReportEventL(MMIDChoiceGroupControlObserver::EScrollRequest, (TAny*)scroll));
}

TInt CMIDChoiceGroupControl::FirstInvisibleElement(CMIDForm::TDirection aScrollDirection) const
{
    TInt index = KErrNotFound;
    TInt formHeight = iItem->FormRect().Height();

    // First find which element is currently right in the top or bottom edge of the form
    TInt yPos = (aScrollDirection == CMIDForm::EDown) ? formHeight : 0;

    TBool found = iListBox->View()->XYPosToItemIndex(
                      TPoint(iListBox->Rect().Center().iX, yPos), index);

    // Check if more than half of the element at index is already visible, move to next item or
    // if already in the last/first element, return KErrNotFound
    if (found && aScrollDirection == CMIDForm::EDown)
    {
        if (formHeight - iListBox->View()->ItemPos(index).iY > (iListBox->ItemHeight() / 2))
        {
            index = (index < (iModel->NumberOfItems() - 1)) ? index + 1 : KErrNotFound;
        }
    }
    else if (found && aScrollDirection == CMIDForm::EUp)
    {
        if (-iListBox->View()->ItemPos(index).iY < (iListBox->ItemHeight() / 2))
        {
            index = (index > 0) ? index - 1 : KErrNotFound;
        }
    }

    return index;
}

#endif

#ifdef RD_JAVA_S60_RELEASE_9_2
CMIDForm* CMIDChoiceGroupControl::Form() const
{
    return iItem->Form();
}
#endif // RD_JAVA_S60_RELEASE_9_2    
