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
* Description:  The CMIDChoiceGroup listbox
*
*/


#include "CMIDChoiceGroupListBox.h"
// API for iChoiceControl
#include "CMIDChoiceGroupControl.h"
#include "CMIDChoiceGroupModel.h"

#include <e32def.h>
// using for CColumnListBoxData related to colors
// and in HandleResourceChange function - setting icon array
#include <eikclbd.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <skinlayout.cdl.h>
#include <layoutmetadata.cdl.h>

// LAF
// AknLayoutScalable_Avkon::list_single_pane_cp2
#include <aknlayoutscalable_avkon.cdl.h> // LAF

#include <j2me/jdebug.h>

#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDChoiceGroupListBox.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


CMIDChoiceGroupListBox::CMIDChoiceGroupListBox(CMIDChoiceGroupControl* aChoiceControl)
        : CAknColumnListBox()
        , iChoiceControl(aChoiceControl)
#ifdef RD_JAVA_S60_RELEASE_9_2
        , iHighlight(EFalse)
        , iTopVisibleItemIndex(KErrNotFound)
#endif // RD_JAVA_S60_RELEASE_9_2
{
    ASSERT(iChoiceControl);
}

CMIDChoiceGroupListBox::~CMIDChoiceGroupListBox()
{
}

// Constructs the listbox with the model created and owned outside.
// Makes sure the EKeepModel flag is set, in order to not destroy
// the model when listbox is destroyed
void CMIDChoiceGroupListBox::ConstructL(
    const CCoeControl* aParent,
    TInt aFlags,
    CMIDChoiceGroupModel* aModel)
{
    // Model is owned outside, set it here, so that when
    // CCAknColumnListBox::ConstructL() calls CreateModelL(),
    // it is already in place. Absolutely make sure that the
    // model owned outside flag is set
    iModel = aModel;
    aFlags |= EKeepModel;

    CAknColumnListBox::ConstructL(aParent, aFlags);
    iListBoxFlags &= ~ELoopScrolling;

    if (iChoiceControl->ChoiceType() == MMIDChoiceGroup::EPopup)
    {
        TAknLayoutRect layoutRect;
        // layout in respect of the screen as only the height is needed
        layoutRect.LayoutRect(iEikonEnv->ScreenDevice()->SizeInPixels(), AknLayoutScalable_Avkon::list_single_pane_cp2(0).LayoutLine());
        SetItemHeightL(layoutRect.Rect().Height());
    }
#ifndef RD_JAVA_FOR_S60_5_0_w49_2008
    else
    {
        // Disable flick/panning functionality
        // Disbled in OMJ DisableScrolling( ETrue );
    }
#endif // RD_JAVA_FOR_S60_5_0_w49_2008

#ifdef RD_SCALABLE_UI_V2
    SetAllowStrayPointers();
#endif

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iFeedback->EnableFeedbackForControl(this, FALSE);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#endif //RD_TACTILE_FEEDBACK

}

// Overridden, because CEikListBox returns EKeyWasConsumed when
// up is pressed on the first item, or down on last. We need to
// return not consumed in order to make the form move focus
// to other items
// Offers key events to the listbox
TKeyResponse CMIDChoiceGroupListBox::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    // First save the currently selected item
    TInt oldCurrent = iView->CurrentItemIndex();

    TKeyResponse resp = EKeyWasNotConsumed;

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iChoiceControl->ChoiceType() != MMIDChoiceGroup::EPopup)
    {
        if (aType != EEventKey || iHighlight)
        {
            // Do not pass the EEventKey to ListBox in case of first
            // HW key action after highlight was disabled.
            resp = CAknColumnListBox::OfferKeyEventL(aKeyEvent, aType);
        }
        else
        {
            // This is first HW key action after highlight was disabled:
            // highlight should be enabled
            UpdateTopVisibleItemIndex();
            if (iTopVisibleItemIndex > KErrNotFound)
            {
                // First top visible element should be highlighted
                iView->SetCurrentItemIndex(iTopVisibleItemIndex);
                // Item drawer must know, that highlight should be enabled
                // from now.
                CColumnListBoxItemDrawer* drawer = ItemDrawer();
                if (drawer)
                {
                    drawer->ClearFlags(
                        CListItemDrawer::ESingleClickDisabledHighlight);
                }
                // Report, that current highlighted element changed
                ReportEventL(MCoeControlObserver::EEventStateChanged);
                resp = EKeyWasConsumed;
                DrawNow();
                iTopVisibleItemIndex = KErrNotFound;
            }
            // Enable highlight flag for this ChoiceGroup, even if top visible
            // element wasn't found (ChoiceGroup is now fully invisible).
            iHighlight = ETrue;
        }
    }
    else
    {
        // For popup ChoiceGroup, handle key events in usual way
        resp = CAknColumnListBox::OfferKeyEventL(aKeyEvent, aType);
    }
#else
    // Let the ListBox handle the key
    resp = CAknColumnListBox::OfferKeyEventL(aKeyEvent, aType);
#endif // RD_JAVA_S60_RELEASE_9_2

    if (iChoiceControl->ChoiceType() != MMIDChoiceGroup::EPopup)
    {
        // If the key was up or down, and the current item did not change,
        // we were apparently already on the first or last item.
        // Return  EKeyWasNotConsumed so that the form knows to transfer focus
        // BUT ONLY if not used in a popup (otherwise the form will get it and
        // move focus under the popup)
        TInt newCurrent = iView->CurrentItemIndex();
        TBool change = oldCurrent != newCurrent;
        TInt lastItemIdx = KErrNotFound;
        if (iModel)
        {
            lastItemIdx = iModel->NumberOfItems() - 1;
        }
        if ((aKeyEvent.iCode == EKeyUpArrow) && !change && (newCurrent == 0))
        {
            resp = EKeyWasNotConsumed;
        }
        if ((aKeyEvent.iCode == EKeyDownArrow) && !change &&
                (newCurrent == lastItemIdx))
        {
            resp = EKeyWasNotConsumed;
        }
    }

    //For Popup ChoiceGroup in landscape mode we have to redraw listbox
    //to display background properly
    if (iChoiceControl->ChoiceType() == MMIDChoiceGroup::EPopup &&
            Layout_Meta_Data::IsLandscapeOrientation() &&
            (aKeyEvent.iCode == EKeyUpArrow ||
             aKeyEvent.iCode == EKeyDownArrow))
    {
        DrawDeferred();
    }

    return resp;
}


// Overridden in order to prevent the listbox from creating its
// own model.
void CMIDChoiceGroupListBox::CreateModelL()
{
    // Model should've already been given
    ASSERT(iModel);
    if (!iModel)
    {
        User::Leave(KErrArgument);
    }
}

void CMIDChoiceGroupListBox::SizeChanged()
{
    // Because SizeChanged() is the only method called by CAknPopupList during a screen
    // resolution change, we must call these two in here. If we don't,
    // open popup dialogs do not update their content during a screen resolution change
    // @see CMIDChoiceGroupListBox::HandleResolutionChange() and CAknPopupList::HandleResolutionChange().
    if (iChoiceControl->ChoiceType() == MMIDChoiceGroup::EPopup)
    {
        TRAP_IGNORE(
            iChoiceControl->SetupColumnsL(ItemDrawer()->ColumnData());
            TAknLayoutRect layoutRect;
            // layout in respect of the screen as only the height is needed
            layoutRect.LayoutRect(iEikonEnv->ScreenDevice()->SizeInPixels(), AknLayoutScalable_Avkon::list_single_pane_cp2(0).LayoutLine());
            SetItemHeightL(layoutRect.Rect().Height());
        );
    }

    CAknColumnListBox::SizeChanged();

    // Assure that non-popup choicegroup is updated correctly visually
    // after dynamic resolution change.
    TInt lbHeight = Rect().Height();
    TInt lbItemHeight = iItemHeight;

    // check that listbox's size is already set
    if ((lbHeight > 0) && (lbItemHeight > 0) && (lbHeight >= lbItemHeight) &&
            iChoiceControl &&
            (iChoiceControl->ChoiceType() != MMIDChoiceGroup::EPopup))
    {
        TInt topItemIndex = TopItemIndex();
        TInt bottomItemIndex = BottomItemIndex();
        TInt currentItemIndex = CurrentItemIndex();
        TInt numOfItems = (Model()) ? Model()->NumberOfItems() : 0;

        // check that listbox-view is set correctly
        if ((topItemIndex >= 0) && (bottomItemIndex >= topItemIndex) &&
                (bottomItemIndex < numOfItems) &&
                (currentItemIndex >= topItemIndex) &&
                (currentItemIndex <= bottomItemIndex))
        {
            // number of actually/possible visible items
            TInt numOfActuallyVisibleItems = bottomItemIndex - topItemIndex + 1;
            TInt numOfPossiblyVisibleItems = lbHeight / lbItemHeight;

            // detect if it is possible to display more items than it is
            // currently displayed (this indicates that listbox-view is not
            // updated correctly after dynamic resolution change) and
            // eventually update listbox-view's top-item-index
            if ((numOfPossiblyVisibleItems > numOfActuallyVisibleItems) &&
                    (topItemIndex >= numOfPossiblyVisibleItems -
                     numOfActuallyVisibleItems))
            {
                topItemIndex -= numOfPossiblyVisibleItems -
                                numOfActuallyVisibleItems;
                SetTopItemIndex(topItemIndex);
            }
        }
    }
}

void CMIDChoiceGroupListBox::Draw(const TRect& aRect) const
{
    //For Popup ChoiceGroup we have to draw its background,
    //because native side doesn't do that properly.
    if (iChoiceControl->ChoiceType() == MMIDChoiceGroup::EPopup)
    {
        CWindowGc& gc = SystemGc();
        MAknsControlContext* cc = NULL;
        //create background context:
        //KAknsIIDQsnFrPopupCenter - id of proper background
        //for popup dialog, background is without borders
        TRAP_IGNORE(cc = CAknsBasicBackgroundControlContext::NewL(
                             KAknsIIDQsnFrPopupCenter,
                             aRect,
                             EFalse));
        if (cc)
        {
            //draw background
            AknsDrawUtils::DrawBackground(
                AknsUtils::SkinInstance(),
                cc,
                NULL,
                gc,
                Position(), //current listobox position on popup dialog
                aRect,
                KAknsDrawParamDefault);
        }
    }

    CAknColumnListBox::Draw(aRect);
}

TTypeUid::Ptr CMIDChoiceGroupListBox::MopSupplyObject(TTypeUid aId)
{
    if (iChoiceControl->ChoiceType() != MMIDChoiceGroup::EPopup)
    {
        return iChoiceControl->MopSupplyObject(aId);
    }
    else // no skinned background in popup list
    {
        return CAknColumnListBox::MopSupplyObject(aId);
    }
}

/** This is currently only used for the open popup choice group items. For the normal
closed items at the moment form won't pass the notification to them. They are instead
recreated on a screen resolution change. See CMIChoiceGroupItem::ResolutionChange() and
CMIChoiceGroupItem::ColorChange(). */
void CMIDChoiceGroupListBox::HandleResourceChange(TInt aType)
{
    if (aType == KEikColorResourceChange ||
            aType == KAknsMessageSkinChange || aType == KUidValueCoeColorSchemeChangeEvent)
    {//skin or color scheme have changed
        TRAP_IGNORE(((CMIDChoiceGroupModel*)Model())->ReConstructSelectionIconsL());
        ItemDrawer()->ColumnData()->SetIconArray(((CMIDChoiceGroupModel*)Model())->IconArray(ETrue));

        SetTextColors();
    }
    else if (aType == KEikDynamicLayoutVariantSwitch)
    {
        // NOTE for the open popup choicegroup items:
        // CAknPopupList never calls HandleResourceChange() on the list box when aType
        // is KEikDynamicLayoutVariantSwitch so do not put any code here,
        // SizeChanged() will be called instead.
    }

    CAknColumnListBox::HandleResourceChange(aType);
}

/** Initialise the listbox text and background colors with values
    from LAF first, and then from the skin if this is supported. */
void CMIDChoiceGroupListBox::SetTextColors()
{
    CColumnListBoxData::TColors colors;

    colors.iText = AKN_LAF_COLOR_STATIC(215);
    colors.iHighlightedText = AKN_LAF_COLOR_STATIC(0);

    if (AknsUtils::AvkonSkinEnabled())
    {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        // icon color, checkbox, radio button
        TInt error = AknsUtils::GetCachedColor(skin,
                                               color,
                                               KAknsIIDQsnIconColors,
                                               EAknsCIQsnIconColorsCG14);

        if (!error)
        {
            colors.iText = color;
        }

        // icon color, setting list highlight   checkbox, radio button
        error = AknsUtils::GetCachedColor(skin,
                                          color,
                                          KAknsIIDQsnIconColors,
                                          EAknsCIQsnIconColorsCG14);

        if (!error)
        {
            colors.iHighlightedText = color;
        }
    }

    View()->ItemDrawer()->SetTextColor(colors.iText);
    View()->ItemDrawer()->SetHighlightedTextColor(colors.iHighlightedText);
}


void CMIDChoiceGroupListBox::PositionChanged()
{
    // Code copied from CEikListBox::SizeChanged()
    TRect clientRect = iBorder.InnerRect(Rect());
    SetViewRectFromClientRect(clientRect);
}

#ifdef RD_SCALABLE_UI_V2
void CMIDChoiceGroupListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        iUpEventSent = EFalse;
        iHighlight = EFalse;
        UpdateTopVisibleItemIndex();
    }
#endif // RD_JAVA_S60_RELEASE_9_2   

    // In non-popup case, don't forward to base class the drag events
    // that occur outside the listbox

#ifdef RD_TACTILE_FEEDBACK
    if ((aPointerEvent.iType == TPointerEvent::EButton1Down ||
            aPointerEvent.iType == TPointerEvent::EButton1Up) &&
            iChoiceControl->ChoiceType() == MMIDChoiceGroup::EPopup)
    {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        iFeedback->InstantFeedback(ETouchFeedbackList);
#else
        iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    }
#endif //RD_TACTILE_FEEDBACK

#ifndef RD_JAVA_S60_RELEASE_9_2
    if (iChoiceControl->ChoiceType() != MMIDChoiceGroup::EPopup &&
            aPointerEvent.iType == TPointerEvent::EDrag)
    {

        if (!View()->ViewRect().Contains(aPointerEvent.iPosition))
        {
            return;
        }
        else
        {

            // this flag must be set in order that drag events can change
            // the focused listbox item
            iListBoxFlags |= ELeftDownInViewRect;
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aPointerEvent.iType == TPointerEvent::EDrag && !iUpEventSent && iChoiceControl->Form()->PhysicsScrolling())
    {
        TPointerEvent pointerEvent = aPointerEvent;
        pointerEvent.iType = TPointerEvent::EButton1Up;
        CAknColumnListBox::HandlePointerEventL(pointerEvent);
        iUpEventSent = ETrue;
    }
    else if (!iUpEventSent)
    {
#endif // RD_JAVA_S60_RELEASE_9_2         
        CAknColumnListBox::HandlePointerEventL(aPointerEvent);
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            iUpEventSent = ETrue;
        }
    }

#endif // RD_JAVA_S60_RELEASE_9_2    

    //For Popup ChoiceGroup in landscape mode we have to redraw listbox
    //to display background properly
    if (iChoiceControl->ChoiceType() == MMIDChoiceGroup::EPopup &&
            Layout_Meta_Data::IsLandscapeOrientation())
    {
        DrawDeferred();
    }
}
#endif

#ifdef RD_JAVA_S60_RELEASE_9_2
/**
 * Returns index of ChoiceGroup element, which is the first
 * visible element in ChoiceGroup from top of Form.
 * Used in CMIDChoiceGroupControl, which needs to know
 * current element for possible scrolling
 * (@see CMIDChoiceGroupControl::RequestScrollIfNeededL()).
 */
TInt CMIDChoiceGroupListBox::TopVisibleItemIndex()
{
    return iTopVisibleItemIndex;
}

/**
 * Fuction calculates index of element, which is the first
 * visible element in ChoiceGroup from top of Form.
 */
void CMIDChoiceGroupListBox::UpdateTopVisibleItemIndex()
{
    if (!iHighlight)
    {
        // There is touch interaction, so we need index for element,
        // which will be highlighted, when user starts using HW keys
        if (iPosition.iY < 0)
        {
            // ChoiceGroup is now partially visible and its top edge is
            // above the Form top edge.

            // Calculate index of first (partially) visible element
            iTopVisibleItemIndex = Abs(iPosition.iY) / iView->ItemHeight();
            TRect lbitemRect = TRect(
                                   iView->ItemPos(iTopVisibleItemIndex),
                                   iView->ItemSize(iTopVisibleItemIndex));
            if (lbitemRect.iTl.iY < 0)
            {
                // Item is really partially visible.
                // Because we don't want index of partially visible item,
                // move index to next item
                iTopVisibleItemIndex++;
            }
            if (iTopVisibleItemIndex >= iModel->NumberOfItems())
            {
                // ChoiceGroup is fully invisible (whole rect is above
                // the Form top edge): no element is visible now
                iTopVisibleItemIndex = KErrNotFound;
            }
        }
        else
        {
            // Top item idge is lower than top edge of Form - is visible
            iTopVisibleItemIndex = 0;
        }
    }
    else
    {
        // In case of HW keys interaction, there is no highlight,
        //  so we don't need highlight any element
        iTopVisibleItemIndex = KErrNotFound;
    }
}
#endif // RD_JAVA_S60_RELEASE_9_2
