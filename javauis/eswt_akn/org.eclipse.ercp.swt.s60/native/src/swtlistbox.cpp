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


#include <aknlists.h>
#include <gulicon.h>
#include <avkon.hrh>
#include <aknselectionlist.h>
#include <eikclbd.h>
#include <avkon.mbg>
#include <e32math.h>
#include <aknconsts.h>
#include <AknUtils.h>
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfx.h>
#endif // RD_UI_TRANSITION_EFFECTS_LIST
#include <swtlaffacade.h>
#include "eswtgraphics.h"
#include "swtlistboxlists.h"
#include "swtcontrolhelper.h"
#include "swtlistbox.h"
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

_LIT(KTxtTab, "\t");
_LIT(KTxtSpace, " ");

const TInt KImgIdxNull = -1;
const TInt KImgIdxOn = 0;
const TInt KImgIdxOff = 1;
const TInt KMaxIdxDigitCount = 4;
const TInt KInitImgArrLength = 2;
const TInt KInitTxtArrLength = 2;
const TInt KInitSelArrLength = 1;
const TInt KVBorderSides = 2;


// ======== MEMBER FUNCTIONS ========


//
// CSwtLbItem
//

// ---------------------------------------------------------------------------
// CSwtLbItem::CSwtLbItem
// ---------------------------------------------------------------------------
//
CSwtLbItem::CSwtLbItem()
        : CSwtListBoxItem(NULL, NULL, NULL, NULL)
        , iHimgIdx(KImgIdxNull)
        , iDimgIdx(KImgIdxNull)
{
}

// ---------------------------------------------------------------------------
// CSwtLbItem::CSwtLbItem
// ---------------------------------------------------------------------------
//
CSwtLbItem::CSwtLbItem(
    HBufC* aDtxt,
    const MSwtImage* aDimg,
    HBufC* aHtxt,
    const MSwtImage* aHimg)
        : CSwtListBoxItem(aDtxt, aDimg, aHtxt, aHimg)
        , iHimgIdx(KImgIdxNull)
        , iDimgIdx(KImgIdxNull)
{
}

// ---------------------------------------------------------------------------
// CSwtLbItem::CSwtLbItem
// ---------------------------------------------------------------------------
//
CSwtLbItem::CSwtLbItem(
    HBufC* aDtxt,
    const MSwtImage* aDimg,
    const TSize& aDimgSize,
    HBufC* aHtxt,
    const MSwtImage* aHimg,
    const TSize& aHimgSize)
        : CSwtListBoxItem(aDtxt, aDimg, aDimgSize, aHtxt, aHimg, aHimgSize)
        , iHimgIdx(KImgIdxNull)
        , iDimgIdx(KImgIdxNull)
{
}

// ---------------------------------------------------------------------------
// CSwtLbItem::~CSwtLbItem
// ---------------------------------------------------------------------------
//
CSwtLbItem::~CSwtLbItem()
{
}


//
// CSwtListBox
//

// ---------------------------------------------------------------------------
// CSwtListBox::NewL
// ---------------------------------------------------------------------------
//
CSwtListBox* CSwtListBox::NewL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
{
    CSwtListBox* self = new(ELeave) CSwtListBox(
        aDisplay,
        aPeer,
        aParent,
        aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CSwtListBox
// ---------------------------------------------------------------------------
//
CSwtListBox::CSwtListBox(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
        : ASwtScrollableBase(aDisplay, aPeer, &aParent, aStyle)
        , iFlickScrollingOngoing(EFalse)

{
}

// ---------------------------------------------------------------------------
// CSwtListBox::~CSwtListBox
// ---------------------------------------------------------------------------
//
CSwtListBox::~CSwtListBox()
{
    // Selection items, own
    delete iPrevSelItems;
    iPrevSelItems = NULL;

    // Contained list, own
    delete iList;
    iList = NULL;

    // List images, not own. The array is reset and destroyed by
    // the destructor of the contained Avkon list
    iImgs = NULL;

    // List texts, own.
    if (iTxts)
    {
        iTxts->Reset();
        delete iTxts;
        iTxts = NULL;
    }

    // Remove all items
    RemoveSubRefs();
    iItems.ResetAndDestroy();

    // Item cells, own.
    iCells.Close();

    // Current and default colors, not own
    iDefColor = NULL;
    iColor = NULL;
}

// ---------------------------------------------------------------------------
// CSwtListBox::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtListBox::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);
    SetComponentsToInheritVisibility(ETrue);
    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());

    // This is indeed necessary, otherwise the background might not be drawn!
    SetBackground(this);
}

// ---------------------------------------------------------------------------
// CSwtListBox::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtListBox::CountComponentControls() const
{
    return iList ? 1 : 0;
}

// ---------------------------------------------------------------------------
// CSwtListBox::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtListBox::ComponentControl(TInt) const
{
    return iList;
}

// ---------------------------------------------------------------------------
// CSwtListBox::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::MakeVisible(TBool aVisible)
{
    CCoeControl::MakeVisible(aVisible);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetDimmed(TBool aDimmed)
{
    CCoeControl::SetDimmed(aDimmed);
    iList->SetDimmed(aDimmed);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBox::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtListBox::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    TBool traversalDoIt = ETrue;

    // No traverse on enter / select
    if ((iStyle & KSwtStyleMulti)
            && (aKeyEvent.iCode == EKeyEnter
                || aKeyEvent.iCode == EKeyOK))
    {
        traversalDoIt = EFalse;
    }

    // No traverse on scroll by default
    if (aKeyEvent.iCode == EKeyDownArrow
            || aKeyEvent.iCode == EKeyUpArrow)
    {
        traversalDoIt = EFalse;

        if (aKeyEvent.iCode == EKeyDownArrow)
        {
            // Traverse if last item is focused and
            // there are other controls to traverse to
            if (iList->View()->CurrentItemIndex()
                    == (iList->Model()->NumberOfItems() - 1))
            {
                if (GetShell().FindTraversalTargetL(
                            ESwtTraverseArrowNext,
                            *this))
                {
                    traversalDoIt = ETrue;
                }
            }
        }
        else
        {
            // Traverse if first item is focused and
            // there are other controls to traverse to
            if (iList->View()->CurrentItemIndex() == 0)
            {
                if (GetShell().FindTraversalTargetL(
                            ESwtTraverseArrowPrevious,
                            *this))
                {
                    traversalDoIt = ETrue;
                }
            }
        }
    }

    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------
// CSwtListBox::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::HandleResourceChange(TInt aType)
{
    if (aType == KAknsMessageSkinChange)
    {
        if (iProps.iSelectionType == ESwtLbMultiSelection)
        {
            TRAP_IGNORE(CreateListSelImgsL(iImgs));
        }
    }
    CAknControl::HandleResourceChange(aType);

    SetMaximumIconSizes();
}

// ---------------------------------------------------------------------------
// CSwtListBox::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    ASSERT(iList);

    CEikScrollBarFrame* sbFrame = iList->ScrollBarFrame();
    CEikScrollBar* vsb = sbFrame ? sbFrame->GetScrollBarHandle(CEikScrollBar::EVertical) : NULL;

    // Check if we should start scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down && !iVScrollBarGrabsPointerEvents)
    {
        if (vsb && vsb->Rect().Contains(aPointerEvent.iPosition))
        {
            iVScrollBarGrabsPointerEvents = ETrue;
        }
    }

    // Deliver event to scrollbar
    if (iVScrollBarGrabsPointerEvents && vsb)
    {
        if (!iFlickScrollingOngoing
                && aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            // Scrollbar was tapped after scrolling stopped
            // by itself, so no need to redirect events
            iScrollbarPointerEventToListbox = EFalse;
        }

        if (iScrollbarPointerEventToListbox)
        {
            // Stops kinetic scrolling when scrollbar is tapped
            iList->HandlePointerEventL(aPointerEvent);
            // Continue delivering events until button up appears to prevent
            // some unexpected behavior in both scrollbar and listbox
            switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Up:
                iScrollbarPointerEventToListbox = EFalse;
                break;
            }
        }
        else
        {
            // Handles scrollbar behavior
            vsb->HandlePointerEventL(aPointerEvent);
        }
    }

    // Deliver event to list
    if (!iVScrollBarGrabsPointerEvents)
    {
        // Store the selection on first pointer event.
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            if (iStyle & KSwtStyleMulti)
            {
                // Multi lists
                ASSERT(iPrevSelItems);
                GetUserSelectionL(iPrevSelItems);
            }
            iPrevFocusIndex = iList->View()->CurrentItemIndex();
        }

        iList->HandlePointerEventL(aPointerEvent);

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            TInt focusIndex = iList->View()->CurrentItemIndex();

            if (iStyle & KSwtStyleMulti)
            {
                // Multi lists
                if ((iList->View()->SelectionIndexes()->Count() != iPrevSelItems->Count()))
                {
                    if (iDisplay.RevertPointerEvent())
                    {
                        // Revert newly checked item.
                        if (iList->View()->ItemIsSelected(focusIndex))
                        {
                            iList->View()->DeselectItem(focusIndex);
                        }
                        else
                        {
                            iList->View()->SelectItemL(focusIndex);
                        }
                    }
                    else
                    {
                        iDisplay.PostSelectionEventL(iPeer);
                    }
                }
            }
            else
            {
                // Single lists
                // Compare the focus index after tap or drag ended
                if (focusIndex != iPrevFocusIndex)
                {
                    // Ensure the selection and focus index are in sync.
                    iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);

                    // Item focus highlight moved, send selection event.
                    // Changing the focus index is not subject to reverting.
                    iDisplay.PostSelectionEventL(iPeer);
                }

                // The default selection is sent from HandleListBoxEventL!
            }
        }
    }

    // Stop scrollbar grabbing
    if (iVScrollBarGrabsPointerEvents
            && aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iVScrollBarGrabsPointerEvents = EFalse;
    }

    PostMouseEventL(aPointerEvent);
}
#else //RD_SCALABLE_UI_V2
void CSwtListBox::HandlePointerEventL(
    const TPointerEvent& /*aPointerEvent*/)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtListBox::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::SizeChanged()
{
    SetMaximumIconSizes();

    if (iList)
    {
        TRect rect = BorderInnerRect();
        iList->SetSize(rect.Size());

        // This is a workaround for the list being drawn in the wrong place if
        // it is placed inside a mobile shell.
        iList->SetPosition(rect.iTl);
    }

    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBox::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::PositionChanged()
{
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBox::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::FocusChanged(TDrawNow aDrawNow)
{
    TBool isFocused = IsFocusControl();
    // This gets called before contained list is created.
    if (iList)
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        EnableFocusHighlight(isFocused);
#endif //RD_JAVA_S60_RELEASE_9_2
        iList->SetFocus(isFocused, aDrawNow);
    }
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtListBox::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtListBox::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);

    if (id.Pointer() == NULL)
    {
        return CAknControl::MopSupplyObject(aId);
    }
    else
    {
        return id;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::Draw
// From CCoeControl
// Overriding Draw just to set correct clipping rect for the item drawer of
// the contained list. The contained list will be drawn immediately after this.
// ---------------------------------------------------------------------------
//
void CSwtListBox::Draw(const TRect& /*aRect*/) const
{
    // Suppress margins - they are drawn over scrollbars
    CSwtListBoxLists::SetMargins(iProps.iListType, iList, 0, 0);

    if (GetShell().UrgentPaintControl() == this)
        return;

    TRect clipRect(ClipToVisibleRect(iList->View()->ViewRect()));
    if (clipRect != iLastViewVisibleRect)
    {
        CSwtListBoxLists::SetItemDrawerClippingRect(iProps.iListType, iList, clipRect);
        iLastViewVisibleRect = clipRect;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::MSKLabelL
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
HBufC* CSwtListBox::MSKLabelL() const
{
    // This gets called before contained list is created.
    if (iList)
    {
        if (!(iList->Model()->NumberOfItems() > 0))
        {
            return ASwtControlBase::MSKLabelL();
        }
        if ((iStyle & KSwtStyleMulti))
        {
            HBufC* label = NULL;
            if (iList->View()->ItemIsSelected(iList->CurrentItemIndex()))
            {
                label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_UNMARK);
            }
            else
            {
                label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_MARK);
            }
            return label;
        }
        else
        {
            return iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
        }
    }
    return ASwtControlBase::MSKLabelL();
}

// ---------------------------------------------------------------------------
// CSwtListBox::SbFrame
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtListBox::SbFrame() const
{
    return iList ? iList->ScrollBarFrame() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtListBox::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtListBox::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtListBox::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtListBox::ComputeSizeL(TInt aWhint, TInt aHhint)
{
    if (!iList)
    {
        return TSize(0, 0);
    }

    TSize prefSize(aWhint, aHhint);

    if (aWhint == KSwtDefault)
    {
        // Add the border width
        prefSize.iWidth = GetBorderWidth() * KVBorderSides;

        // Add the vertical scrollbar space if present
        if (SbFrame()->ScrollBarExists(CEikScrollBar::EVertical)
                && SbFrame()->ScrollBarVisibility(CEikScrollBar::EVertical)
                != CEikScrollBarFrame::EOff)
        {
            prefSize.iWidth += iDisplay.UiUtils().ScrollBarBreadth(
                                   SbFrame()->VerticalScrollBar());
        }

        // Add minimum item width
        prefSize.iWidth += MinimumListItemWidth();
    }
    else
    {
        prefSize.iWidth = aWhint;
    }


    if (aHhint == KSwtDefault)
    {
        // Add the border width
        prefSize.iHeight = GetBorderWidth() * KVBorderSides;

        // Add the height of all items
        prefSize.iHeight += iList->CalcHeightBasedOnNumOfItems(
                                iItems.Count());
    }
    else
    {
        prefSize.iHeight = aHhint;
    }

    return prefSize;
}

// ---------------------------------------------------------------------------
// CSwtListBox::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aType == EEventKeyDown)
    {
        // After panning focus highlight was disabled, so enabling again
        EnableFocusHighlight(ETrue);
    }
#endif //RD_JAVA_S60_RELEASE_9_2

    // No items or not a normal key event
    if (iList->Model()->NumberOfItems() == 0 || aType != EEventKey)
    {
        iList->OfferKeyEventL(aKeyEvent, aType);
        return;
    }

    TBool scroll = aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow
                   || aKeyEvent.iCode == EKeyNext || aKeyEvent.iCode == EKeyPrevious;
    TBool submit = aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK;

    // For performance considerations, assuming that Enter or Select always
    // does change the multi selection. If the assumption is wrong, we must
    // store the multi selection before the key is handled and then compare:
    // if ( ( iStyle & KSwtStyleMulti ) && submit ) GetUserSelectionL( iPrevSelItems );

    // Store selection before letting the list handle the key.
    if ((iStyle & KSwtStyleSingle) && scroll)
    {
        iPrevFocusIndex = iList->View()->CurrentItemIndex();
    }

    iList->OfferKeyEventL(aKeyEvent, aType);

    if (scroll || submit)
    {
        if (iStyle & KSwtStyleSingle)
        {
            // Single lists
            if (submit)
            {
                // Item submited, send default selection event.
                iDisplay.PostDefaultSelectionEventL(iPeer);
            }
            else
            {
                if (iList->View()->CurrentItemIndex() != iPrevFocusIndex)
                {
                    // Ensure the selection and focus index are in sync.
                    iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);

                    // Item focus highlight moved, send selection event.
                    iDisplay.PostSelectionEventL(iPeer);
                }
            }
        }
        else
        {
            // Multi lists
            // Msk could have changed: Mark / Unmark
            UpdateListMskL();

            // For performance considerations, assuming that Enter or Select always
            // does change the multi selection. If the assumption is wrong, here's
            // how we must check if the selection really changed:
            // if ( submit && iPrevSelItems && ( iList->View()->SelectionIndexes()->
            // Count() != iPrevSelItems->Count() ) )
            if (submit)
            {
                // Multi selection changed, send selection event.
                iDisplay.PostSelectionEventL(iPeer);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::IsKeyUsed
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtListBox::IsKeyUsed(TUint aKeyCode) const
{
    if (aKeyCode == EKeyBackspace)
    {
        return EFalse;
    }
    else if (aKeyCode == EKeyOK)
    {
        if (iStyle & KSwtStyleSingle)
        {
            MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
            if (commandArranger)
            {
                if (commandArranger->IsContextSensitiveOperationSet())
                {
                    return EFalse;
                }
            }
            return ETrue;
        }
        else
        {
            return ETrue;
        }
    }
    else
    {
        return ETrue;
    }
}


TBool CSwtListBox::MSKSelCmdEnabled() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtListBox::GetForeground
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRgb CSwtListBox::GetForeground() const
{
    if (CSwtListBoxLists::IsListFormatted(iProps.iListType))
    {
        // Get the text color from the formatted list item drawer
        return (static_cast< CEikFormattedCellListBox* >(iList))->
               ItemDrawer()->TextColor();
    }
    else
    {
        // Get the text color from the column list item drawer
        return (static_cast< CEikColumnListBox* >(iList))->
               ItemDrawer()->TextColor();
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetForegroundL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetForegroundL(const MSwtColor* aColor)
{
    iColor = aColor;
    UpdateListColor();
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SetBackgroundL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetBackgroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetBackgroundL(aColor);
    iList->HandleResourceChange(KEikMessageColorSchemeChange);
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtListBase::IsLongTapAnimationCandidate
// From MSwtControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
TBool CSwtListBox::IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const
{
    ASSERT(iList);
    TInt itemIndex = KErrNotFound;
    TBool pointerOverItem = iList->View()->XYPosToItemIndex(aPointerEvent.iPosition, itemIndex);
    return pointerOverItem && ASwtControlBase::IsLongTapAnimationCandidate(aPointerEvent);
}
#else
TBool CSwtListBox::IsLongTapAnimationCandidate(const TPointerEvent& /*aPointerEvent*/) const
{
    return EFalse;
}
#endif

#ifdef RD_JAVA_S60_RELEASE_9_2
// ---------------------------------------------------------------------------
// CSwtListBox::EnableFocusHighlight
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBox::EnableFocusHighlight(TBool aEnable)
{
    ASSERT(iList);
    ASSERT(iList->View());

    CSwtListBoxLists::EnableFocusHighlight(iList->View()->ItemDrawer(),
                                           aEnable);
}
#endif //RD_JAVA_S60_RELEASE_9_2

TInt CSwtListBox::FocusBackgroundPolicy() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    return ENoFocusBackgroundInCaptionedControl;
#else
    return EDefaultFocusBackground;
#endif // RD_JAVA_S60_RELEASE_9_2
}

// ---------------------------------------------------------------------------
// CSwtListBox::Scrollable
// From MSwtListBox
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtListBox::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetLayoutStyleL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetLayoutStyleL(TInt aLayoutStyle)
{
    ASSERT(!iList);

    // Calculate first the list properties
    iProps = ListProperties(aLayoutStyle, iStyle);

    // Change style to SINGLE if MULTI is not supported
    if ((iProps.iSelectionType == ESwtLbSingleSelection)
            && (iStyle & KSwtStyleMulti))
    {
        iStyle &= ~KSwtStyleMulti;
        iStyle |= KSwtStyleSingle;
    }

    // Create a list based on properties
    iList = CreateListL(iProps.iListType);
    if (!iList)
    {
        User::Leave(ESwtErrorNullArgument);
    }

    // Set image array
    iImgs = CreateListImgArrL(iProps.iSelectionType == ESwtLbMultiSelection);
    if (!iImgs)
    {
        User::Leave(ESwtErrorNullArgument);
    }
    else
    {
        if (CSwtListBoxLists::IsListFormatted(iProps.iListType))
        {
            CFormattedCellListBoxData* cellData =
                (static_cast< CEikFormattedCellListBox* >(iList))->
                ItemDrawer()->FormattedCellData();
            cellData->SetIconArray(iImgs);
            cellData->SetSkinEnabledL(EFalse);
        }
        else
        {
            CColumnListBoxData* columnData =
                (static_cast< CEikColumnListBox* >(iList))->
                ItemDrawer()->ColumnData();
            columnData->SetIconArray(iImgs);
            columnData->SetSkinEnabledL(EFalse);
        }
    }

    // Set text array
    iTxts = new(ELeave) CDesCArrayFlat(KInitTxtArrLength);
    iList->Model()->SetItemTextArray(iTxts);
    iList->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Selected item array
    iPrevSelItems = new(ELeave) CArrayFixFlat<TInt>(KInitSelArrLength);

    // Get item cells
    CSwtListBoxLists::Cells(iProps.iListType, iCells);

    // This is needed for the case where the theme has animated highlights.
    iList->SetFocus(ETrue, ENoDrawNow);

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Disable stretching for two line lists
    switch (iProps.iListType)
    {
    case ESwtLbDouble:
    case ESwtLbDoubleGraphic:
    case ESwtLbDoubleLarge:
        // Disable stretching to keep items double line in landscape
        CSwtListBoxLists::EnableStretching(iProps.iListType, iList, EFalse);
        break;
    default:
        // Do nothing
        break;
    }
#endif

    UpdateListMskL();

    ActivateL();

    // Because the creation of the contained list is delayed
    // we need to ensure that it redraws with correct size.
    // Not doing this would result in empty list in CaptionedControl for instance.
    SizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeselectItemsL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeselectItemsL(const TInt* aIndices, TInt aCount)
{
    if (aIndices)
    {
        for (TInt i = 0; i < aCount; i++)
        {
            SelectListItemL(aIndices[i], EFalse, EFalse);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeselectItemL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeselectItemL(TInt aIdx)
{
    SelectListItemL(aIdx, EFalse, EFalse);
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeselectRangeL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeselectRangeL(TInt aStart, TInt aEnd)
{
    SelectListItemsL(aStart, aEnd, EFalse);
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeselectAllL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeselectAllL()
{
    SelectListItemsL(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtListBox::GetFocusIndex
// From MSwtListBox
// ---------------------------------------------------------------------------
//
TInt CSwtListBox::GetFocusIndex() const
{
    return iList->View()->CurrentItemIndex();
}

// ---------------------------------------------------------------------------
// CSwtListBox::GetSelectionCount
// From MSwtListBox
// ---------------------------------------------------------------------------
//
TInt CSwtListBox::GetSelectionCount() const
{
    return iList->View()->SelectionIndexes()->Count();
}

// ---------------------------------------------------------------------------
// CSwtListBox::GetSelectionIndices
// From MSwtListBox
// ---------------------------------------------------------------------------
//
const CArrayFix<TInt>* CSwtListBox::GetSelectionIndices() const
{
    return iList->View()->SelectionIndexes();
}

// ---------------------------------------------------------------------------
// CSwtListBox::RefreshItemL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::RefreshItemL(TInt aIdx,
                               const CSwtListBoxItem* aRemoteItem)
{
    ASSERT(aIdx >= 0 && aIdx < iItems.Count());
    CSwtLbItem* newItem = NULL;
    if (aRemoteItem)
    {
        newItem = new(ELeave) CSwtLbItem(
            aRemoteItem->String().AllocL(),
            aRemoteItem->Image(),
            aRemoteItem->ImageSize(),
            aRemoteItem->HeadingString().AllocL(),
            aRemoteItem->HeadingIcon(),
            aRemoteItem->HeadingIconSize());
    }
    else
    {
        newItem = new(ELeave) CSwtLbItem();
    }
    DeleteListItem(aIdx);
    newItem->iIdx = aIdx;
    InsertListItemL(newItem);
    iList->HandleItemAdditionL();
}

// ---------------------------------------------------------------------------
// CSwtListBox::RefreshListL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::RefreshListL(
    const RSwtListBoxItemsArray& aRemoteItemsArray)
{
    // Guaranteed from the Java side that the remote item array
    // is in fact the same item array received in SetDataModelL
    TInt count = iItems.Count();
    ASSERT(count == aRemoteItemsArray.Count());

    // Replace current items
    CSwtListBoxItem* remoteItem = NULL;
    CSwtLbItem* newItem = NULL;
    for (TInt i = 0; i < count; i++)
    {
        remoteItem = aRemoteItemsArray[i];
        newItem = NULL;
        if (remoteItem)
        {
            newItem = new(ELeave) CSwtLbItem(
                remoteItem->String().AllocL(),
                remoteItem->Image(),
                remoteItem->ImageSize(),
                remoteItem->HeadingString().AllocL(),
                remoteItem->HeadingIcon(),
                remoteItem->HeadingIconSize());
        }
        else
        {
            newItem = new(ELeave) CSwtLbItem();
        }
        newItem->iIdx = i;
        DeleteListItem(i);
        InsertListItemL(newItem);
    }

    // Inform list of model change
    iList->HandleItemAdditionL();
}

// --------------------------------------------------------------------------
// CSwtListBox::SelectItemL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::SelectItemL(TInt aIdx, TBool aScroll)
{
    SelectListItemL(aIdx, ETrue, aScroll);
}

// ---------------------------------------------------------------------------
// CSwtListBox::SelectRangeL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::SelectRangeL(TInt aStart, TInt aEnd)
{

    TInt count = iList->Model()->NumberOfItems();
    if (count == 0 || aStart >= count)
    {
        return;
    }

    TInt end(aEnd);
    if (end >= count)
    {
        end = count - 1;
    }
    TInt start(aStart);
    if (start < 0)
    {
        start = 0;
    }

    for (TInt index = start; index <= end; ++index)
    {
        iList->View()->SelectItemL(index);
    }

    UpdateListMskL();
}

// --------------------------------------------------------------------------
// CSwtListBox::SelectAllL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::SelectAllL()
{
    SelectListItemsL(ETrue);
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetFocusIndex
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetFocusIndex(TInt aIndex)
{
    ASSERT(iList);

    // By now, we only call this method in multiple selection.
    // In single selection, we assume that setting the focus means selecting.
    ASSERT(iStyle & KSwtStyleMulti);

    if (aIndex < 0 || aIndex >= iList->Model()->NumberOfItems())
    {
        return;
    }

    TInt old = iList->CurrentItemIndex();
    if (old != aIndex)
    {
        iList->SetCurrentItemIndex(aIndex);
        iList->View()->DrawItem(aIndex);
        if (old != -1)
        {
            iList->View()->DrawItem(old);
        }
    }
    TRAP_IGNORE(UpdateListMskL());
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetDataModelL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetDataModelL(
    const RSwtListBoxItemsArray& aRemoteItemsArray)
{
    // Delete current items and insert copies of the received ones
    TInt count = aRemoteItemsArray.Count();
    CSwtListBoxItem* remoteItem = NULL;
    CSwtLbItem* newItem = NULL;
    DeleteAllListItems();
    for (TInt i = 0; i < count; i++)
    {
        remoteItem = aRemoteItemsArray[i];
        newItem = NULL;
        if (remoteItem)
        {
            newItem = new(ELeave) CSwtLbItem(
                remoteItem->String().AllocL(),
                remoteItem->Image(),
                remoteItem->ImageSize(),
                remoteItem->HeadingString().AllocL(),
                remoteItem->HeadingIcon(),
                remoteItem->HeadingIconSize());
        }
        else
        {
            newItem = new(ELeave) CSwtLbItem();
        }
        newItem->iIdx = i;
        InsertListItemL(newItem);
    }

    // Inform list of model change and select first item
    iList->HandleItemAdditionL();
    if (iProps.iSelectionType != ESwtLbMultiSelection)
    {
        SelectListItemL(0, ETrue, EFalse);
    }
    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetHeadingFontL
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetHeadingFontL(const MSwtFont* /*aFont*/)
{
    // Not supported
}

// ---------------------------------------------------------------------------
// CSwtListBox::ShowSelection
// From MSwtListBox
// ---------------------------------------------------------------------------
//
void CSwtListBox::ShowSelection()
{
    if (GetSelectionCount() == 0)
    {
        return;
    }

    TInt idx = KErrNotFound;
    if (iProps.iSelectionType == ESwtLbSingleSelection)
    {
        idx = iList->View()->CurrentItemIndex();
    }
    else
    {
        idx = (*(iList->SelectionIndexes()))[0];
        for (TInt i = 1; i < iList->SelectionIndexes()->Count(); i++)
        {
            idx = Min(idx, (*(iList->SelectionIndexes()))[i]);
        }
    }
    iList->ScrollToMakeItemVisible(idx);
}

// ---------------------------------------------------------------------------
// CSwtListBox::HandleSizeChangedL
// From MSwtListObserver
// ---------------------------------------------------------------------------
//
void CSwtListBox::HandleSizeChangedL()
{
    if (CSwtListBoxLists::IsListFormatted(iProps.iListType))
    {
        CFormattedCellListBoxItemDrawer* itemDrawer = (static_cast<
                CEikFormattedCellListBox* >(iList))->ItemDrawer();
        CFormattedCellListBoxData* cellData = itemDrawer->FormattedCellData();
        cellData->SetSeparatorLinePosition(ENoLine);
        iDefColor = itemDrawer->TextColor();
        itemDrawer->SetBackColor(GetBackground());
    }
    else
    {
        CColumnListBoxItemDrawer* itemDrawer = (static_cast<
                                                CEikColumnListBox* >(iList))->ItemDrawer();
        CColumnListBoxData* cellData = itemDrawer->ColumnData();
        cellData->SetSeparatorLinePosition(ENoLine);
        iDefColor = itemDrawer->TextColor();
        itemDrawer->SetBackColor(GetBackground());
    }

    // The listbox looses the custom color after resize, therefore reset.
    if (iColor)
        UpdateListColor();

    // Suppress margins - they are drawn over scrollbars
    // We need to update margins here one more time, because previous
    // code re-sets them
    CSwtListBoxLists::SetMargins(iProps.iListType, iList, 0, 0);
}

// ---------------------------------------------------------------------------
// CSwtListBox::Utils
// From MSwtListObserver
// ---------------------------------------------------------------------------
//
MSwtUiUtils& CSwtListBox::Utils() const
{
    return iDisplay.UiUtils();
}

// ---------------------------------------------------------------------------
// CSwtListBox::HandleScrollEventL
// From MEikScrollBarObserver
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListBox::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    switch (aEventType)
    {
    case EEikScrollThumbDragVert:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEikScrollThumbReleaseVert:
        GetShell().SetUrgentPaintControl(NULL);
        break;
    default:
        break;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    iList->HandleScrollEventL(aScrollBar, aEventType);
    ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
}
#else // RD_SCALABLE_UI_V2
void CSwtListBox::HandleScrollEventL(CEikScrollBar*, TEikScrollEvent)
{
}
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtListBase::HandleScrollEventL
// From MEikListBoxObserver
// Handles default selection for touch. Unable to handle the selection events
// here since EEventItemClicked is not always sent for every tap.
// The selection events are handled in HandlePointerEvent.
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListBox::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
{
    if (aListBox != iList)
    {
        return;
    }

    UpdateFlickScrollingState(aEventType);

    switch (aEventType)
    {
        // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventPanningStarted:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEventFlickStopped:
        GetShell().SetUrgentPaintControl(NULL);
        break;
#endif // RD_JAVA_S60_RELEASE_9_2
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventItemSingleClicked:
#else
    case EEventItemDoubleClicked:
#endif // RD_JAVA_S60_RELEASE_9_2
        if (!(iStyle & KSwtStyleMulti))
        {
            if (!iDisplay.RevertPointerEvent())
                iDisplay.PostDefaultSelectionEventL(iPeer);
        }
        break;
    default:
        // Do nothing
        break;
    }
}
#else // RD_SCALABLE_UI_V2
void CSwtListBox::HandleListBoxEventL(CEikListBox*, TListBoxEvent)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtListBox::ListProperties
// ---------------------------------------------------------------------------
//
CSwtListBox::TSwtLbProp CSwtListBox::ListProperties(TInt aLayoutStyle,
        TInt aStyle) const
{
    TSwtLbProp lbProp;
    lbProp.iListType = ESwtLbSingle;
    lbProp.iSelectionType = ESwtLbSingleSelection;
    lbProp.iDiconVisible = aLayoutStyle & MSwtListBox::KLbModShowDetailIcons;

    // Heading 2 lines
    if (aLayoutStyle & MSwtListBox::ELbStyle2LineItem)
    {
        if (aLayoutStyle & MSwtListBox::KLbModShowHeadingIcons)
        {
            lbProp.iListType = ESwtLbDoubleLarge;
        }
        else if (aStyle & KSwtStyleMulti)
        {
            lbProp.iSelectionType = ESwtLbMultiSelection;
            lbProp.iListType = ESwtLbDoubleGraphic;
        }
        else
        {
            lbProp.iListType = ESwtLbDouble;
        }
    }
    // Heading 1 line
    else if (aLayoutStyle & MSwtListBox::ELbStyle1LineItem)
    {
        if (aLayoutStyle & MSwtListBox::KLbModShowHeadingIcons)
        {
            lbProp.iListType = ESwtLbSingleHeadingGraphic;
        }
        else if (aStyle & KSwtStyleMulti)
        {
            lbProp.iListType = ESwtLbSingleHeadingGraphic;
            lbProp.iSelectionType = ESwtLbMultiSelection;
        }
        else
        {
            lbProp.iListType = ESwtLbSingleHeading;
        }
    }
    // No heading
    else
    {
        if (aLayoutStyle & MSwtListBox::KLbModShowHeadingIcons)
        {
            lbProp.iListType = ESwtLbSingleLarge;
        }
        else if (aStyle & KSwtStyleMulti)
        {
            lbProp.iListType = ESwtLbSingleGraphic;
            lbProp.iSelectionType = ESwtLbMultiSelection;
        }
        else
        {
            lbProp.iListType = ESwtLbSingle;
        }
    }

    return lbProp;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CreateListL
// ---------------------------------------------------------------------------
//
CEikTextListBox* CSwtListBox::CreateListL(TInt aListType)
{
    CEikTextListBox* list = CSwtListBoxLists::NewListL(aListType);

    if (list)
    {
        CleanupStack::PushL(list);
        if (iStyle & KSwtStyleMulti)
        {
            list->ConstructL(this, EAknListBoxStylusMultiselectionList);
        }
        else
        {
            list->ConstructL(this, EAknListBoxSelectionList);
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        // Effects do not work with non window owning scrollbars.
        CWindowGc* gc = list->View()->ItemDrawer()->Gc();
        MAknListBoxTfx* transApi = CAknListLoader::TfxApi(gc);
        if (transApi)
        {
            transApi->EnableEffects(EFalse);
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

        CSwtListBoxLists::SetListObserver(aListType, list, this);
        list->SetContainerWindowL(*this);
        list->SetCurrentItemIndex(0);
        list->MakeVisible(ETrue);
        list->SetComponentsToInheritVisibility(ETrue);
        CreateScrollBarsL(list);

#ifdef RD_SCALABLE_UI_V2
        list->SetListBoxObserver(this);
#endif // RD_SCALABLE_UI_V2
        CleanupStack::Pop(list);
    }

    return list;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CreateScrollBarsL
// ---------------------------------------------------------------------------
//
void CSwtListBox::CreateScrollBarsL(CEikTextListBox* aList) const
{
    ASSERT(aList);
    // Horizontal scrollbars are always off, the vertical scrollbars
    // are visible if the style contains the vertical scroll bit
    aList->CreateScrollBarFrameL();

#ifdef RD_SCALABLE_UI_V2
    // WARNING!!! The expanded touch area does not move correctly togehter with the scrollbars!
    aList->ScrollBarFrame()->SetScrollBarFrameFlags(CEikScrollBarFrame::EDisableExpandedTouchArea);
#endif // RD_SCALABLE_UI_V2

    // In eSWT all scrollbars must be non window owing the main reason being that
    // the scrollbars must draw at exact same time with the parent control. This
    // is especially essential in ScrolledComposite. If the scrollbars would be
    // window owing, they would draw too much ahead of the parents creating an
    // ugly visual effect when flicking the ScrolledComposite.
    // The drawback of having non window owing scrollbars is too many paint events
    // which affects on the speed of kinetic scrolling in lists.
    aList->ScrollBarFrame()->CreateDoubleSpanScrollBarsL(EFalse, EFalse, ETrue, EFalse);
    aList->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        iStyle & KSwtStyleVScroll ?
        CEikScrollBarFrame::EOn : CEikScrollBarFrame::EOff);
}

// ---------------------------------------------------------------------------
// CSwtListBox::CreateListImgArrL
// ---------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CSwtListBox::CreateListImgArrL(
    TBool aMultiSelImgs) const
{
    CArrayPtr<CGulIcon>* imgArray =
        new(ELeave) CArrayPtrFlat<CGulIcon>(KInitImgArrLength);

    // Selection icons
    CleanupStack::PushL(imgArray);
    if (aMultiSelImgs)
    {
        CreateListSelImgsL(imgArray);
    }
    CleanupStack::Pop(imgArray);

    return imgArray;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CreateListSelImgsL
// ---------------------------------------------------------------------------
//
void CSwtListBox::CreateListSelImgsL(
    CArrayPtr<CGulIcon>* aImgArr) const
{
    ASSERT(aImgArr);

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Check box on
    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    CGulIcon* checkBox = NULL;

    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropCheckboxOn, bmp, mask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_prop_checkbox_on,
                            EMbmAvkonQgn_prop_checkbox_on_mask);

    checkBox = CGulIcon::NewL(bmp, mask);
    CleanupStack::PushL(checkBox);
    if (aImgArr->Count() > KImgIdxOn)
    {
        if (aImgArr->At(KImgIdxOn))
        {
            delete aImgArr->At(KImgIdxOn);
        }
        aImgArr->At(KImgIdxOn) = checkBox;
    }
    else
    {
        aImgArr->AppendL(checkBox);
    }
    CleanupStack::Pop(3);   // bmp, mask, checkbox

    // Check box off
    bmp = NULL;
    mask = NULL;
    checkBox = NULL;

    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropCheckboxOff, bmp, mask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_prop_checkbox_off,
                            EMbmAvkonQgn_prop_checkbox_off_mask);

    checkBox = CGulIcon::NewL(bmp, mask);
    CleanupStack::PushL(checkBox);
    if (aImgArr->Count() > KImgIdxOff)
    {
        if (aImgArr->At(KImgIdxOff))
        {
            delete aImgArr->At(KImgIdxOff);
        }
        aImgArr->At(KImgIdxOff) = checkBox;
    }
    else
    {
        aImgArr->AppendL(checkBox);
    }
    CleanupStack::Pop(3);   // bmp, mask, checkbox
}

// ---------------------------------------------------------------------------
// CSwtListBox::InsertListItemL
// ---------------------------------------------------------------------------
//
void CSwtListBox::InsertListItemL(CSwtLbItem* aItem)
{
    CleanupStack::PushL(aItem);

    // Heading image
    CGulIcon* img = NULL;
    TInt imgIdx = KImgIdxNull;
    TSize bitmapSize(TSize::EUninitialized);
    if (iProps.iListType == ESwtLbSingleGraphic
            || iProps.iListType == ESwtLbSingleLarge
            || iProps.iListType == ESwtLbSingleHeadingGraphic
            || iProps.iListType == ESwtLbDoubleGraphic
            || iProps.iListType == ESwtLbDoubleLarge)
    {
        // Create / allocate image
        img = CreateListImgL(aItem->HeadingIcon(), EFalse, bitmapSize);
        if (img)
        {
            aItem->SetHeadingIconSize(bitmapSize);
            // Insert image
            CleanupStack::PushL(img);
            imgIdx = InsertListImgL(img);
            CleanupStack::Pop(img);
            aItem->iHimgIdx = imgIdx;
        }
    }

    // Detail image
    if (iProps.iDiconVisible)
    {
        // Create / allocate image
        img = CreateListImgL(aItem->Image(), ETrue, bitmapSize);
        if (img)
        {
            aItem->SetImageSize(bitmapSize);
            // Insert image
            CleanupStack::PushL(img);
            imgIdx = InsertListImgL(img);
            CleanupStack::Pop(img);
            aItem->iDimgIdx = imgIdx;
        }
    }

    // Store the item
    iItems.Insert(aItem, aItem->iIdx);

    // Insert string into the list text array
    HBufC* buf = CreateListTxtL(aItem);
    ASSERT(buf);
    CleanupStack::PushL(buf);
    InsertListTxtL(*buf, aItem->iIdx);
    CleanupStack::PopAndDestroy(buf);

    CleanupStack::Pop(aItem);
}

// ---------------------------------------------------------------------------
// CSwtListBox::CreateListImgL
// ---------------------------------------------------------------------------
//
CGulIcon* CSwtListBox::CreateListImgL(const MSwtImage* aSwtImg, const TBool aDetailIcon,
                                      TSize& aBitmapResSize) const
{
    CGulIcon* img = NULL;
    if (aSwtImg)
    {
        CFbsBitmap* bmp = 0;
        CFbsBitmap* mask = 0;
        TSize maxIconSize = aDetailIcon ? iMaxDetailIconSize : iMaxHeadingIconSize;
        TSize bitmapSize = aSwtImg->Bitmap().SizeInPixels();

        if (bitmapSize.iWidth > maxIconSize.iWidth ||
                bitmapSize.iHeight > maxIconSize.iHeight)
        {
            bitmapSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                             bitmapSize, maxIconSize);
        }

        bmp =  const_cast<CFbsBitmap*>(&aSwtImg->SubBitmap(bitmapSize));
        mask = const_cast<CFbsBitmap*>(aSwtImg->SubMaskBitmap(bitmapSize, ETrue));

        aSwtImg->AddSubRef(bitmapSize);
        aBitmapResSize = bitmapSize;

        img = CGulIcon::NewL();
        img->SetBitmapsOwnedExternally(ETrue);   // Freed by CSwtImage
        img->SetBitmap(bmp);
        img->SetMask(mask);
    }
    return img;
}

// ---------------------------------------------------------------------------
// CSwtListBox::InsertListImgL
// ---------------------------------------------------------------------------
//
TInt CSwtListBox::InsertListImgL(CGulIcon* aImg)
{
    ASSERT(aImg);

    TInt imgIdx = KImgIdxNull;
    TInt nrOfImgs = iImgs->Count();

    // Search for a free position and insert if any
    for (TInt i = 0; i < nrOfImgs; i++)
    {
        if (!iImgs->At(i))
        {
            iImgs->At(i) = aImg;
            imgIdx = i;
            break;
        }
    }

    // Append if no position was free
    if (imgIdx == KImgIdxNull)
    {
        iImgs->AppendL(aImg);
        imgIdx = nrOfImgs;
    }

    return imgIdx;
}

// ---------------------------------------------------------------------------
// CSwtListBox::CreateListTxtL
// ---------------------------------------------------------------------------
//
HBufC* CSwtListBox::CreateListTxtL(const CSwtLbItem* aItem) const
{
    if (!aItem)
    {
        return KNullDesC().AllocL();
    }

    // Convert heading image index to string
    TBuf<KMaxIdxDigitCount> hImgIdxTxt;
    if (iProps.iSelectionType == ESwtLbMultiSelection)
    {
        hImgIdxTxt.AppendNum(KImgIdxOff);
    }
    else
    {
        if (aItem->iHimgIdx != KImgIdxNull)
        {
            hImgIdxTxt.AppendNum(aItem->iHimgIdx);
        }
        else
        {
            hImgIdxTxt.Append(KNullDesC);
        }
    }

    // Convert detail image index to string
    TBuf<KMaxIdxDigitCount> dImgIdxTxt;
    if (iProps.iDiconVisible
            && (aItem->iDimgIdx != KImgIdxNull))
    {
        dImgIdxTxt.AppendNum(aItem->iDimgIdx);
    }
    else
    {
        dImgIdxTxt.Append(KNullDesC);
    }

    // Replace tabs in heading string
    HBufC* hBuf = HBufC::NewLC(aItem->HeadingString().Length());
    *hBuf = aItem->HeadingString();
    TabsToSpaces(hBuf);

    // Replace tabs in detail string
    HBufC* dBuf = HBufC::NewLC(aItem->String().Length());
    *dBuf = aItem->String();
    TabsToSpaces(dBuf);

    // Allocate buffer for the new formatted string
    HBufC* buf = HBufC::NewL(dImgIdxTxt.Length()
                             + hImgIdxTxt.Length()
                             + hBuf->Length()
                             + dBuf->Length()
                             + KTxtTab().Length() * KMaxCellCount);
    *buf = KNullDesC;
    TPtr txt = buf->Des();

    // Go trough cells and format the new string
    // depending on the type of each cell
    TInt count = iCells.Count();
    for (TInt i = 0; i < count; i++)
    {
        switch (iCells[i])
        {
        case ECellDtxt:
            txt.Append(*dBuf);
            break;
        case ECellDimg:
            txt.Append(dImgIdxTxt);
            break;
        case ECellHtxt:
            txt.Append(*hBuf);
            break;
        case ECellHimg:
            txt.Append(hImgIdxTxt);
            break;
        case ECellEmpty:
        default:
            // do nothing
            break;
        }
        txt.Append(KTxtTab);
    }

    CleanupStack::PopAndDestroy(dBuf);
    CleanupStack::PopAndDestroy(hBuf);

    return buf;
}

// ---------------------------------------------------------------------------
// CSwtListBox::InsertListTxtL
// ---------------------------------------------------------------------------
//
void CSwtListBox::InsertListTxtL(const TDesC& aTxt, TInt aIdx)
{
    ASSERT(iTxts);
    iTxts->InsertL(aIdx, aTxt);
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeleteListItem
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeleteListItem(TInt aIdx)
{
    // Out of range check
    if (aIdx < 0 || aIdx >= iItems.Count())
    {
        return;
    }

    // Delete images & texts, remove item
    CSwtLbItem* item = iItems[aIdx];
    if (!item)
    {
        return;
    }

    // If we have at some point created a scaled copies for list item images,
    // remove references to the copies here.
    const MSwtImage* dImg = item->Image();
    const MSwtImage* hImg = item->HeadingIcon();
    const TSize dImgSize = item->ImageSize();
    const TSize hImgSize = item->HeadingIconSize();

    if (dImg && dImgSize != TSize(TSize::EUninitialized))
    {
        dImg->RemoveSubRef(item->ImageSize());
    }

    if (hImg && hImgSize != TSize(TSize::EUninitialized))
    {
        hImg->RemoveSubRef(item->HeadingIconSize());
    }

    DeleteListImg(item->iHimgIdx);
    DeleteListImg(item->iDimgIdx);
    DeleteListTxt(aIdx);
    iItems.Remove(aIdx);
    delete item;
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeleteAllListItems
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeleteAllListItems()
{
    // Clear selection
    iList->View()->ClearSelection();

    // Remove all texts
    iTxts->Reset();

    RemoveSubRefs();

    // Remove all images except the selection check box images
    if (iProps.iSelectionType != ESwtLbMultiSelection)
    {
        iImgs->ResetAndDestroy();
    }
    else
    {
        if (iImgs->Count() > (KImgIdxOff + 1))
        {
            for (TInt i = KImgIdxOff + 1; i < iImgs->Count(); i++)
            {
                delete iImgs->At(i);
            }
            iImgs->Delete(
                KImgIdxOff + 1,
                iImgs->Count() - (KImgIdxOff + 1));
        }
    }

    // Remove all items
    iItems.ResetAndDestroy();
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeleteListImg
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeleteListImg(TInt aIdx)
{
    if (aIdx >= 0 && aIdx < iImgs->Count())
    {
        delete iImgs->At(aIdx);
        iImgs->At(aIdx) = NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::DeleteListTxt
// ---------------------------------------------------------------------------
//
void CSwtListBox::DeleteListTxt(TInt aIdx)
{
    ASSERT(iTxts);
    ASSERT(aIdx >= 0 && aIdx < iTxts->Count());
    iTxts->Delete(aIdx);
}

// ---------------------------------------------------------------------------
// CSwtListBox::SelectListItemL
// ---------------------------------------------------------------------------
//
void CSwtListBox::SelectListItemL(TInt aIdx, TBool aSelected, TBool aScroll)
{
    // Ignore call if item is out of range
    if (aIdx < 0 || aIdx >= iItems.Count())
    {
        return;
    }

    if (iProps.iSelectionType == ESwtLbMultiSelection)
    {
        // Multi selection
        if (aSelected)
        {
            // Select
            iList->View()->SelectItemL(aIdx);
            if (aScroll)
            {
                iList->ScrollToMakeItemVisible(aIdx);
            }
        }
        else
        {
            // Deselect
            iList->View()->DeselectItem(aIdx);
        }
        UpdateListMskL();
    }
    else
    {
        // Single selection
        if (aSelected)
        {
            // Select
            if (aScroll && iList->View()->ViewRect().Height() >= iList->ItemHeight())
            {
                iList->View()->VerticalMoveToItemL(aIdx, CListBoxView::ESingleSelection);
            }
            else
            {
                TInt old = iList->CurrentItemIndex();
                iList->View()->SetCurrentItemIndex(aIdx); //we do not call CEikListBox::SetCurrentItemIndex, because we do not want to scroll.
                iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
                if (old != -1)
                {
                    iList->View()->DrawItem(old);
                }
            }
        }
        // Calls to deselect when the style is SINGLE are ignored.
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::SelectListItemL
// ---------------------------------------------------------------------------
//
void CSwtListBox::SelectListItemsL(TInt aStart, TInt aEnd, TBool aSelected)
{
    for (TInt i = aStart; i <= aEnd; i++)
    {
        SelectListItemL(i, aSelected, EFalse);
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::SelectListItemL
// ---------------------------------------------------------------------------
//
void CSwtListBox::SelectListItemsL(TBool aSelected)
{
    SelectListItemsL(0, iItems.Count() - 1, aSelected);
}


// ---------------------------------------------------------------------------
// CSwtListBox::GetUserSelectionL
// ---------------------------------------------------------------------------
//
void CSwtListBox::GetUserSelectionL(CArrayFix<TInt>* aCurrSelItems)
{
    // Multi selected items array
    if ((iStyle & KSwtStyleMulti) && aCurrSelItems)
    {
        iList->View()->GetSelectionIndexesL(aCurrSelItems);
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::UpdateListColor
// ---------------------------------------------------------------------------
//
void CSwtListBox::UpdateListColor()
{
    TRgb color;

    if (iColor)
    {
        // Use current color
        color = iColor->RgbValue();
    }
    else
    {
        // Use default color
        color = iDefColor;
    }

    if (CSwtListBoxLists::IsListFormatted(iProps.iListType))
    {
        // Set text and highlighted text color to the formatted item drawer
        static_cast< CEikFormattedCellListBox* >(iList)->
        ItemDrawer()->SetTextColor(color);
    }
    else
    {
        // Set text and highlighted text color to the column item drawer
        static_cast< CEikColumnListBox* >(iList)->
        ItemDrawer()->SetTextColor(color);
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::UpdateListMSK
// ---------------------------------------------------------------------------
//
void CSwtListBox::UpdateListMskL()
{
    if (AknLayoutUtils::MSKEnabled())
    {
        MSwtCommandArranger* cmdArranger = iDisplay.CommandArranger();
        if (cmdArranger)
        {
            cmdArranger->UpdateMSKLabelL();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::MinimumListItemWidth
// ---------------------------------------------------------------------------
//
TInt CSwtListBox::MinimumListItemWidth() const
{
    // Layout id selection
    CSwtLafFacade::TSwtLafFacadeRectId itemLayoutId =
        CSwtLafFacade::EUndefinedRectId;
    CSwtLafFacade::TSwtLafFacadeTextId detailTextLayoutId =
        CSwtLafFacade::EUndefinedTextId;
    CSwtLafFacade::TSwtLafFacadeTextId headingTextLayoutId =
        CSwtLafFacade::EUndefinedTextId;
    CSwtLafFacade::TSwtLafFacadeRectId detailIconLayoutId =
        CSwtLafFacade::EUndefinedRectId;
    switch (iProps.iListType)
    {
    case ESwtLbSingle:
        itemLayoutId = CSwtLafFacade::EListSinglePane;
        detailTextLayoutId = CSwtLafFacade::EListSinglePaneT1;
        detailIconLayoutId = CSwtLafFacade::EListSinglePaneG1;
        break;
    case ESwtLbSingleGraphic:
        itemLayoutId = CSwtLafFacade::EListSingleGraphicPane;
        detailTextLayoutId = CSwtLafFacade::EListSingleGraphicPaneT1;
        detailIconLayoutId = CSwtLafFacade::EListSingleGraphicPaneG2;
        break;
    case ESwtLbSingleLarge:
        itemLayoutId = CSwtLafFacade::EListSingleLargeGraphicPane;
        detailTextLayoutId = CSwtLafFacade::EListSingleLargeGraphicPaneT1;
        detailIconLayoutId = CSwtLafFacade::EListSingleLargeGraphicPaneG2;
        break;
    case ESwtLbSingleHeading:
        itemLayoutId = CSwtLafFacade::EListSingleHeadingPane;
        detailTextLayoutId = CSwtLafFacade::EListSingleHeadingPaneT1;
        detailIconLayoutId = CSwtLafFacade::EListSingleHeadingPaneG1;
        break;
    case ESwtLbSingleHeadingGraphic:
        itemLayoutId = CSwtLafFacade::EListSingleGraphicHeadingPane;
        detailTextLayoutId = CSwtLafFacade::EListSingleGraphicHeadingPaneT1;
        detailIconLayoutId = CSwtLafFacade::EListSingleGraphicHeadingPaneG4;
        break;
    case ESwtLbDouble:
        itemLayoutId = CSwtLafFacade::EListDoublePane;
        headingTextLayoutId = CSwtLafFacade::EListDoublePaneT1;
        detailTextLayoutId = CSwtLafFacade::EListDoublePaneT2;
        detailIconLayoutId = CSwtLafFacade::EListDoublePaneG1;
        break;
    case ESwtLbDoubleGraphic:
        itemLayoutId = CSwtLafFacade::EListDoubleGraphicPane;
        headingTextLayoutId = CSwtLafFacade::EListDoubleGraphicPaneT1;
        detailTextLayoutId = CSwtLafFacade::EListDoubleGraphicPaneT2;
        detailIconLayoutId = CSwtLafFacade::EListDoubleGraphicPaneG2;
        break;
    case ESwtLbDoubleLarge:
        itemLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPane;
        headingTextLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneT1;
        detailTextLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneT2;
        detailIconLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneG2;
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    TBool isListDouble = (iProps.iListType >= ESwtLbDouble);

    // List item pane
    TAknLayoutRect itemLayout;
    TRect mainRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainRect);
    itemLayout = CSwtLafFacade::GetLayoutRect(
                     CSwtLafFacade::EListScrollGenPane, mainRect, 0);
    itemLayout = CSwtLafFacade::GetLayoutRect(
                     CSwtLafFacade::EListGenPane, itemLayout.Rect(), 0);
    itemLayout = CSwtLafFacade::GetLayoutRect(
                     itemLayoutId, itemLayout.Rect(), 0);

    // Text layouts
    TAknLayoutText detailTextLayout = CSwtLafFacade::GetLayoutText(
                                          detailTextLayoutId, itemLayout.Rect(), 0);
    TAknLayoutText headingTextLayout;
    if (headingTextLayoutId != CSwtLafFacade::EUndefinedRectId)
    {
        headingTextLayout = CSwtLafFacade::GetLayoutText(
                                headingTextLayoutId, itemLayout.Rect(), 0);
    }

    // Text fonts
    const CFont* detailFont = detailTextLayout.Font();
    const CFont* headingFont = NULL;
    if (isListDouble)
    {
        headingFont = headingTextLayout.Font();
    }

    TInt width(0);
    TInt detailX(0);
    TInt headingX(0);
    TInt detailIconX(0);

    // Text max width
    if (detailFont || headingFont)
    {
        TInt count = iItems.Count();
        for (TInt i = 0; i < count; i++)
        {
            if (headingFont)
            {
                headingX = Max(headingX, headingFont->TextWidthInPixels(
                                   iItems[i]->HeadingString()));
            }
            if (detailFont)
            {
                detailX = Max(detailX, detailFont->TextWidthInPixels(
                                  iItems[i]->String()));
            }
        }
    }

    // Detail text fixed left margin & fixed right margin
    if (AknLayoutUtils::LayoutMirrored())
    {
        detailX += mainRect.iBr.iX - detailTextLayout.TextRect().iBr.iX;
        detailX += CSwtLafFacade::TextLineLayout(detailTextLayoutId).il;
    }
    else
    {
        detailX += detailTextLayout.TextRect().iTl.iX;
        detailX += CSwtLafFacade::TextLineLayout(detailTextLayoutId).ir;
    }

    // Heading text fixed left margin & fixed right margin
    if (isListDouble)
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            headingX += mainRect.iBr.iX - headingTextLayout.TextRect().iBr.iX;
            headingX += CSwtLafFacade::TextLineLayout(headingTextLayoutId).il;
        }
        else
        {
            headingX += headingTextLayout.TextRect().iTl.iX;
            headingX += CSwtLafFacade::TextLineLayout(headingTextLayoutId).ir;
        }
    }

    // Icon fixed width & fixed right margin
    if (iProps.iDiconVisible)
    {
        TAknLayoutRect detailIconLayout = CSwtLafFacade::GetLayoutRect(
                                              detailIconLayoutId, itemLayout.Rect(), 0);
        detailIconX += detailIconLayout.Rect().Width();
        if (AknLayoutUtils::LayoutMirrored())
        {
            detailIconX += CSwtLafFacade::WindowLineLayout(
                               detailIconLayoutId).il;
        }
        else
        {
            detailIconX += CSwtLafFacade::WindowLineLayout(
                               detailIconLayoutId).ir;
        }
        if (isListDouble)
        {
            headingX += detailIconX;
        }
        else
        {
            detailX += detailIconX;
        }
    }

    width += Max(headingX, detailX);

    return width;
}

// ---------------------------------------------------------------------------
// CSwtListBox::TabsToSpace
// ---------------------------------------------------------------------------
//
void CSwtListBox::TabsToSpaces(HBufC* aBuf) const
{
    ASSERT(aBuf);

    TPtr ptr = aBuf->Des();
    TInt pos = KErrNotFound;

    // Locate tab, replace with space and start from there new search
    do
    {
        pos = ptr.Find(KTxtTab);
        if (pos != KErrNotFound)
        {
            ptr.Replace(pos, KTxtTab().Length(), KTxtSpace);
            ptr = ptr.MidTPtr(pos + KTxtTab().Length() + 1);
        }
    }
    while (pos != KErrNotFound);
}


// ---------------------------------------------------------------------------
// CSwtListBox::SetMaximumIconSizes
// ---------------------------------------------------------------------------
//
void CSwtListBox::SetMaximumIconSizes()
{
    CSwtLafFacade::TSwtLafFacadeRectId detailIconLayoutId =
        CSwtLafFacade::EUndefinedRectId;
    CSwtLafFacade::TSwtLafFacadeRectId headingIconLayoutId =
        CSwtLafFacade::EUndefinedRectId;

    // Heading icon is not supported for all styles.
    switch (iProps.iListType)
    {
    case ESwtLbSingle:
        detailIconLayoutId = CSwtLafFacade::EListSinglePaneG1;
        break;
    case ESwtLbSingleGraphic:
        detailIconLayoutId = CSwtLafFacade::EListSingleGraphicPaneG2;
        headingIconLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneG1;
        break;
    case ESwtLbSingleLarge:
        detailIconLayoutId = CSwtLafFacade::EListSingleLargeGraphicPaneG2;
        headingIconLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneG1;
        break;
    case ESwtLbSingleHeading:
        detailIconLayoutId = CSwtLafFacade::EListSingleHeadingPaneG1;
        break;
    case ESwtLbSingleHeadingGraphic:
        detailIconLayoutId = CSwtLafFacade::EListSingleGraphicHeadingPaneG4;
        headingIconLayoutId = CSwtLafFacade::EListSingleGraphicHeadingPaneG1;
        break;
    case ESwtLbDouble:
        detailIconLayoutId = CSwtLafFacade::EListDoublePaneG1;
        break;
    case ESwtLbDoubleGraphic:
        detailIconLayoutId = CSwtLafFacade::EListDoubleGraphicPaneG2;
        headingIconLayoutId = CSwtLafFacade::EListDoubleGraphicPaneG1;
        break;
    case ESwtLbDoubleLarge:
        detailIconLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneG2;
        headingIconLayoutId = CSwtLafFacade::EListDoubleLargeGraphicPaneG1;
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    TAknLayoutRect detailIconLayout = CSwtLafFacade::GetLayoutRect(
                                          detailIconLayoutId, TRect(), 0);
    TSize maxDetailIconSize = detailIconLayout.Rect().Size();
    TSize maxHeadingIconSize;

    if (headingIconLayoutId != CSwtLafFacade::EUndefinedRectId)
    {
        TAknLayoutRect headingIconLayout = CSwtLafFacade::GetLayoutRect(
                                               headingIconLayoutId, TRect(), 0);
        maxHeadingIconSize = headingIconLayout.Rect().Size();
    }

    if (maxDetailIconSize != iMaxDetailIconSize ||
            maxHeadingIconSize != iMaxHeadingIconSize)
    {
        UpdateImageSizes(maxDetailIconSize, maxHeadingIconSize);
        iMaxDetailIconSize = maxDetailIconSize;
        iMaxHeadingIconSize = maxHeadingIconSize;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::SetMaximumIconSizes
// ---------------------------------------------------------------------------
//
void CSwtListBox::UpdateImageSizes(const TSize& aNewMaxDetailSize,
                                   const TSize& aNewMaxHeadingSize)
{
    TInt count = iItems.Count();

    for (TInt i = 0; i < count; i++)
    {
        CSwtLbItem* item = iItems[ i ];

        CGulIcon* detailIcon = 0;
        CGulIcon* headingIcon = 0;
        const MSwtImage* headingImage = item->HeadingIcon();
        const MSwtImage* detailImage = item->Image();

        if (item->iDimgIdx != KImgIdxNull)
        {
            detailIcon = iImgs->At(item->iDimgIdx);
        }

        if (item->iHimgIdx != KImgIdxNull)
        {
            headingIcon = iImgs->At(item->iHimgIdx);
        }

        if (detailIcon && detailImage)
        {
            item->SetImageSize(
                ScaleImage(detailImage, detailIcon, iMaxDetailIconSize, aNewMaxDetailSize));

        }

        if (headingIcon && headingImage)
        {
            item->SetHeadingIconSize(
                ScaleImage(headingImage, headingIcon, iMaxHeadingIconSize, aNewMaxHeadingSize));
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBox::ScaleImage
// ---------------------------------------------------------------------------
//
TSize CSwtListBox::ScaleImage(const MSwtImage* aImage, CGulIcon* aIcon,
                              const TSize& aOldMaxSize, const TSize& aNewMaxSize)
{
    TSize imageSize = aImage->Bitmap().SizeInPixels();
    TSize oldSize;
    TBool doScaling = ETrue;

    if (imageSize.iHeight > aOldMaxSize.iHeight ||
            imageSize.iWidth > aOldMaxSize.iWidth)
    {
        // If image size is bigger than the old maximum size, then
        // the image has been also previously scaled.
        oldSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                      imageSize, aOldMaxSize);
    }
    else if (imageSize.iHeight > aNewMaxSize.iHeight ||
             imageSize.iWidth > aNewMaxSize.iWidth)
    {
        // Image is bigger than the new boundaries, but it did fit
        // inside the old boundaries, so the image is default size.
        oldSize = imageSize;
    }
    else
    {
        doScaling = EFalse;
    }

    if (doScaling)
    {
        imageSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                        imageSize, aNewMaxSize);
        CFbsBitmap* bmp = const_cast<CFbsBitmap*>(&aImage->SubBitmap(imageSize));
        CFbsBitmap* mask = const_cast<CFbsBitmap*>(aImage->SubMaskBitmap(imageSize, ETrue));

        aImage->AddSubRef(imageSize);

        // Remove the old reference
        aImage->RemoveSubRef(oldSize);

        aIcon->SetBitmap(bmp);
        aIcon->SetMask(mask);
    }

    return imageSize;
}

// ---------------------------------------------------------------------------
// CSwtListBox::RemoveSubRefs
// ---------------------------------------------------------------------------
//
void CSwtListBox::RemoveSubRefs()
{
    TInt count = iItems.Count();
    for (TInt i = 0; i < count; i++)
    {
        CSwtLbItem* item = iItems[ i ];
        const MSwtImage* dImg = item->Image();
        const MSwtImage* hImg = item->HeadingIcon();
        const TSize dImgSize = item->ImageSize();
        const TSize hImgSize = item->HeadingIconSize();

        if (dImg && dImgSize != TSize(TSize::EUninitialized))
        {
            dImg->RemoveSubRef(item->ImageSize());
        }

        if (hImg && hImgSize != TSize(TSize::EUninitialized))
        {
            hImg->RemoveSubRef(item->HeadingIconSize());
        }
    }
}

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
void CSwtListBox::DoControlSpecificFeedback(
    const TBool& aFirstTap,
    const TBool& aTappedToChildRect,
    const TPointerEvent& aPointerEvent) const
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback && !aTappedToChildRect)
    {
        switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            if (aFirstTap)
            {
                feedback->InstantFeedback(ETouchFeedbackSensitiveList);
            }
            break;
        }
    }
}
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// ---------------------------------------------------------------------------
// CSwtTableListBox::UpdateFlickScrollingState
// Updates flick scrolling status based on received listbox event.
// ---------------------------------------------------------------------------
//
void CSwtListBox::UpdateFlickScrollingState(TListBoxEvent aEventType)
{
    switch (aEventType)
    {
    case EEventFlickStarted:
        iFlickScrollingOngoing = ETrue;
        iScrollbarPointerEventToListbox = ETrue;
        break;
    case EEventFlickStopped:
        iFlickScrollingOngoing = EFalse;
        break;
    }
}
