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


#include <eiklbi.h>
#include <eiktxlbm.h>
#include <AknBidiTextUtils.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <aknappui.h>
#include <avkon.hrh>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK
#include <eswtcore.rsg>
#include <swtlaffacade.h>
#include "eswt.h"
#include "swttablelistbox.h"
#include "swttableitemdrawer.h"
#include "swttablelistboxview.h"
#include "swtimagetextitem.h"
#include "swtfont.h"


// These are only used as a fallback in case skin color is not available
const TInt KSwtGridColorRed(200);
const TInt KSwtGridColorGreen(200);
const TInt KSwtGridColorBlue(200);

#ifdef RD_TACTILE_FEEDBACK
// Id for the only feedback area
const TInt KTouchFeedbackRectId1(0);
#endif // RD_TACTILE_FEEDBACK


// ======== MEMBER FUNCTIONS ========


CSwtTableListBox* CSwtTableListBox::NewL(MSwtDisplay& aDisplay, CSwtTable &aTable)
{
    CSwtTableListBox* self = new(ELeave) CSwtTableListBox(aDisplay, aTable);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


CSwtTableListBox::~CSwtTableListBox()
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif // RD_TACTILE_FEEDBACK
    delete iOldSelectionArray;
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
    }
}


void CSwtTableListBox::DeselectItems(const TInt* aIndices, TInt aCount) const
{
    if (!aIndices ||
            aCount == 0 ||
            iModel->NumberOfItems() == 0 ||
            SelectionIndexes()->Count() == 0)
    {
        return; // The array is empty
    }

    for (TInt i = 0; i < aCount; i++)
    {
        iView->DeselectItem(aIndices[ i ]);
    }
}


void CSwtTableListBox::DeselectRange(TInt aStart, TInt aEnd)
{
    TInt count = iModel->NumberOfItems();
    if (count == 0 || aStart >= count || SelectionIndexes()->Count() == 0)
    {
        return;
    }

    if (aStart < 0)
    {
        aStart = 0;
    }

    if (aEnd >= count)
    {
        aEnd = count - 1;
    }

    if (aStart == 0 && aEnd == count - 1)
    {
        ClearSelection();
    }
    else
    {
        for (TInt i = aStart; i <= aEnd; ++i)
        {
            iView->DeselectItem(i);
        }
    }
}


const MSwtFont& CSwtTableListBox::GetDefaultFont() const
{
    return *iDefaultFont;
}


TRect CSwtTableListBox::GetItemRect(TInt aRowIndex) const
{
    TRect itemRect(iView->ItemPos(aRowIndex), iView->ItemSize(aRowIndex));
    itemRect.Move(0, iTable.GetHeaderHeight());
    return itemRect;
}


TInt CSwtTableListBox::GetSelectionCount() const
{
    return SelectionIndexes()->Count();
}

void CSwtTableListBox::SelectAllL() const
{
    ASSERT(iView);

    for (TInt index = 0; index < iModel->NumberOfItems(); ++index)
    {
        iView->SelectItemL(index);
    }
}

void CSwtTableListBox::SetFocusIndex(TInt aIndex) const
{
    ASSERT(iView);

    TInt old = CurrentItemIndex();
    if (old != aIndex)
    {
        SetCurrentItemIndex(aIndex);
        iView->DrawItem(aIndex);
        if (old != -1)
        {
            iView->DrawItem(old);
        }
    }
}

void CSwtTableListBox::SelectItemL(TInt aIndex, TBool aScroll) const
{
    ASSERT(aIndex >= 0);

    if (aIndex >= iModel->NumberOfItems() ||
            iView->ItemIsSelected(aIndex))
    {
        return;
    }

    if (!(iTable.Style() & KSwtStyleMulti))
    {
        if (aScroll && iView->ViewRect().Height() >= ItemHeight())
        {
            iView->VerticalMoveToItemL(aIndex, CListBoxView::ESingleSelection);
        }
        else
        {
            TInt old = CurrentItemIndex();

            // We do not call CEikListBox::SetCurrentItemIndex, because we do not want to scroll
            iView->SetCurrentItemIndex(aIndex);

            iView->UpdateSelectionL(CListBoxView::ESingleSelection);
            if (old != -1)
            {
                iView->DrawItem(old);
            }
        }
    }
    else
    {
        iView->SelectItemL(aIndex);
        if (aScroll)
        {
            ScrollToMakeItemVisible(aIndex);
        }
    }
}


void CSwtTableListBox::SelectRangeL(TInt aStart, TInt aEnd) const
{
    ASSERT(iListBoxFlags & EMultipleSelection);
    ASSERT(aStart >= 0);

    TInt count = iModel->NumberOfItems();
    if (count == 0 || aStart >= count)
    {
        return;
    }

    if (aEnd >= count)
    {
        aEnd = count - 1;
    }

    for (TInt index = aStart; index <= aEnd; ++index)
    {
        iView->SelectItemL(index);
    }
}


void CSwtTableListBox::ShowSelection() const
{
    ASSERT(iListBoxFlags & EMultipleSelection);

    if (GetSelectionCount() == 0)
    {
        return;
    }
    TInt smallestIndex = (*SelectionIndexes())[ 0 ];
    for (TInt i = 1; i < SelectionIndexes()->Count(); ++i)
    {
        smallestIndex = Min(smallestIndex, (*SelectionIndexes())[ i ]);
    }

    ScrollToMakeItemVisible(smallestIndex);
}


void CSwtTableListBox::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // Horizontal scrolling
    TInt scrollAmount = HorizontalScrollPixelsPerArrowEvent();
    if (aKeyEvent.iCode == EKeyLeftArrow)
    {
        iView->HScroll(-scrollAmount);
        UpdateScrollBarThumbs();
        return;
    }
    else if (aKeyEvent.iCode == EKeyRightArrow)
    {
        iView->HScroll(scrollAmount);
        UpdateScrollBarThumbs();
        return;
    }

    // No items or not a normal key event
    if (iModel->NumberOfItems() == 0 || aType != EEventKey)
    {
        CEikTextListBox::OfferKeyEventL(aKeyEvent, aType);
        return;
    }

    TBool scroll = aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow
                   || aKeyEvent.iCode == EKeyNext || aKeyEvent.iCode == EKeyPrevious;
    TBool submit = aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK;

    // For performance considerations, assuming that Enter or Select always
    // does change the multi selection. If the assumption is wrong, we must
    // store the multi selection before the key is handled and then compare:
    // if ( ( iListBoxFlags & EMultipleSelection ) && submit )
    // iView->GetSelectionIndexesL( iOldSelectionArray );

    // Store selection before letting the list handle the key.
    if (!(iListBoxFlags & EMultipleSelection) && scroll)
    {
        iPrevFocusIndex = CurrentItemIndex();
    }

    CEikTextListBox::OfferKeyEventL(aKeyEvent, aType);

    if (scroll || submit)
    {
        if (!(iListBoxFlags & EMultipleSelection))
        {
            // Single lists
            if (submit)
            {
                // Item submited, send default selection event.
                iTable.PostDefaultSelectionEventL(CurrentItemIndex());
            }
            else
            {
                if (CurrentItemIndex() != iPrevFocusIndex)
                {
                    // Ensure the selection and focus index are in sync.
                    View()->UpdateSelectionL(CListBoxView::ESingleSelection);

                    // Item focus highlight moved, send selection event.
                    iTable.PostSelectionEventL(KSwtNone, CurrentItemIndex());
                }
            }
        }
        else
        {
            // Multi lists
            // For performance considerations, assuming that Enter or Select always
            // does change the multi selection. If the assumption is wrong, here's
            // how we must check if the selection really changed:
            // if ( submit && iOldSelectionArray && iOldSelectionArray->Count()
            // != iView->SelectionIndexes()->Count() )
            if (submit)
            {
                // Multi selection changed, send selection event.
                iTable.PostSelectionEventL(KSwtStyleCheck, CurrentItemIndex());
            }
        }
    }
}


void CSwtTableListBox::HandleDataSizeChangeL()
{
    iView->CalcDataWidth();
    SetViewRectFromClientRect(ClientRect());
    TableView()->ValidateHScrollOffset();// Set the offset before updating the scrollbars
    HandleItemAdditionL();               // This will update the scrollbars
}


void CSwtTableListBox::DrawGrid(CWindowGc& aGc, const TRect& aClippingRect) const
{
    TInt lineWidth(iTable.GetGridLineWidth());
    if (lineWidth == 0)
    {
        return;
    }

    aGc.SetClippingRect(aClippingRect);

    TRgb gridLineColor(TRgb(KSwtGridColorRed, KSwtGridColorGreen, KSwtGridColorBlue));
    TInt error = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                                           gridLineColor,
                                           KAknsIIDQsnLineColors,
#ifdef RD_JAVA_S60_RELEASE_9_2
                                           EAknsCIQsnLineColorsCG15
#else
                                           EAknsCIQsnLineColorsCG6
#endif
                                           );
    aGc.SetPenColor(gridLineColor);
    aGc.SetPenSize(TSize(lineWidth, lineWidth));
    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

    // Set-up a drawing rectangle to position the grid correctly relative
    // to the view
    TRect drawRect(iTable.ClientRect());
    TRect viewRect = iView->ViewRect();
    const TInt dataWidth(iView->DataWidth());
    const TInt viewWidth(iView->ViewRect().Width());
    const TInt hScrollOffset(iView->HScrollOffset());
    drawRect.iTl.iY = iView->ItemPos(iView->TopItemIndex()).iY;
    drawRect.Move(-hScrollOffset, 0/*TableView()->VOffset()*/);
    drawRect.iBr.iX = drawRect.iTl.iX + dataWidth;
    if (drawRect.iBr.iX < iView->ViewRect().iBr.iX)
    {
        drawRect.iBr.iX = iView->ViewRect().iBr.iX;
    }

    // Last index means also last index partially visible, 
    // therefore intentionally we do not substract 1 here. 
    TInt lastIndex = iView->TopItemIndex() 
        + iView->NumberOfItemsThatFitInRect(iView->ViewRect());
    TInt lastPotentialItemIndex = Min(iModel->NumberOfItems() - 1, lastIndex);
    drawRect.iBr.iY = iView->ItemPos(lastPotentialItemIndex).iY 
        + iView->ItemSize(lastPotentialItemIndex).iHeight;

    if (drawRect.iBr.iY > viewRect.iBr.iY) drawRect.iBr.iY = viewRect.iBr.iY;

    TInt headerHeight(iTable.GetHeaderHeight());

    // First draw the horizontal lines, if any

    TPoint p1(drawRect.iTl);
    TPoint p2(drawRect.iBr.iX, drawRect.iTl.iY);

    // Draw a line below each item, but stop if we are getting out of
    // clipping rectangle
    TInt itemHeight = ItemHeight();
    while (itemHeight > 0 && p1.iY <= drawRect.iBr.iY && p1.iY <= aClippingRect.iBr.iY)
    {
        aGc.DrawLine(p1, p2);
        p1.iY += itemHeight;
        p2.iY += itemHeight;
    }
    
    // Then draw the vertical lines

    // Compute the first column edge location
    TSwtTableMargins margins(iTable.GetMargins());
    TInt directionFactor;
    if (AknLayoutUtils::LayoutMirrored())
    {
        directionFactor = -1;
        p1.SetXY(drawRect.iBr.iX, drawRect.iTl.iY);
        p2.SetXY(drawRect.iBr.iX, drawRect.iBr.iY);
    }
    else
    {
        directionFactor = 1;
        p1.SetXY(drawRect.iTl.iX, drawRect.iTl.iY);
        p2.SetXY(drawRect.iTl.iX, drawRect.iBr.iY);
    }
    p1.iX += directionFactor * margins.iLeft;
    p2.iX += directionFactor * margins.iLeft;
    if (iTable.Style() & KSwtStyleMulti ||
            iTable.Style() & KSwtStyleCheck)
    {
        TInt checkBoxWidth(iTable.GetCheckBoxSize().iWidth);
        p1.iX += directionFactor * checkBoxWidth;
        p2.iX += directionFactor * checkBoxWidth;
    }

    // Draw a line after each column
    const RArray<TInt>& columnWidths = iTable.GetColumnWidths();
    TInt columnCount(iTable.GetColumnCount());
    for (TInt i = 0; i < columnCount; ++i)
    {
        TInt linePosDelta(directionFactor * columnWidths[ i ]);
        p1.iX += linePosDelta;
        p2.iX += linePosDelta;
        if (linePosDelta != 0)
        {
            aGc.DrawLine(p1, p2);
        }
    }
}


void CSwtTableListBox::HandleResourceChangeL(TInt aType)
{
    CCoeControl::HandleResourceChange(aType);

    // Update the default font
    if (aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange)
    {
        iDefaultFont->RemoveRef();
        TableItemDrawer()->SetFont(LayoutFont());
        ResetDefaultFontFromItemDrawerL();
        iTable.ListBoxDefaultFontChanged();
        HandleDataSizeChangeL();
        if (iTable.ListMode())
        {
            iTable.PackColumn(0);
        }
    }
}


TRect CSwtTableListBox::ClientRect() const
{
    TRect rect(Rect());
    if (iSBFrame)
    {
        if (iSBFrame->ScrollBarVisibility(CEikScrollBar::EVertical)
                != CEikScrollBarFrame::EOff)
        {
            if (AknLayoutUtils::LayoutMirrored())
            {
                rect.iTl.iX += iDisplay.UiUtils().ScrollBarBreadth(
                                   iSBFrame->VerticalScrollBar());
            }
            else
            {
                rect.iBr.iX -= iDisplay.UiUtils().ScrollBarBreadth(
                                   iSBFrame->VerticalScrollBar());
            }
        }
        if (iSBFrame->ScrollBarVisibility(CEikScrollBar::EHorizontal)
                != CEikScrollBarFrame::EOff)
        {
            rect.iBr.iY -= iDisplay.UiUtils().ScrollBarBreadth(
                               iSBFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal));
        }
    }
    return rect;
}


void CSwtTableListBox::SetRequiredHeightInNumOfItems(const TInt& aRequiredHeightInNumOfItems)
{
    iRequiredHeightInNumOfItems= aRequiredHeightInNumOfItems;
}


CSwtTableItemDrawer* CSwtTableListBox::TableItemDrawer() const
{
    return static_cast< CSwtTableItemDrawer* >(iItemDrawer);
} //lint !e1763


CSwtTableListBoxView* CSwtTableListBox::TableView() const
{
    return static_cast< CSwtTableListBoxView* >(iView);
} //lint !e1763


void CSwtTableListBox::ResetDefaultFontFromItemDrawerL()
{
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), *TableItemDrawer()->Font(0));
}


const CFont* CSwtTableListBox::LayoutFont()
{
    const CFont* font = AknLayoutUtils::FontFromId(
                            CSwtLafFacade::GetFontId(CSwtLafFacade::EListSingleGraphicPaneT1Font, 0));
    ASSERT(font);
    return font;
}


// ---------------------------------------------------------------------------
// This is needed only due to problems with the base class implementation of
// updating the scrollbars. Without doing this the scrollbars won't get
// correctly updated when the control is moved around.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::InvalidateScrollBars()
{
    if (iSBFrame)
    {
        CEikScrollBar* vBar = iSBFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
        CEikScrollBar* hBar = iSBFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
        if (vBar)
        {
            vBar->SetExtent(TPoint(0, 0), TSize(0,0));
        }
        if (hBar)
        {
            hBar->SetExtent(TPoint(0, 0), TSize(0,0));
        }
    }
}


// ---------------------------------------------------------------------------
// This is needed in order to handle item additions in a way that redraws
// can be controlled. Note that CEikListBox::HandleItemAdditionL is not
// a virtual function.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::HandleItemAdditionL()
{
    TableView()->DoSetFlags(CListBoxView::EItemCountModified);
    TableView()->CalcBottomItemIndex();
    UpdateScrollBarsL();
    UpdateScrollBarThumbs();
    TableView()->DoClearFlags(CListBoxView::EItemCountModified);
    FireItemChange();
    iTable.Redraw();
}


// ---------------------------------------------------------------------------
// UpdateScrollBarsL updates the scrollbars and calls drawDeferred.
// To prevent redraw it can't be called. Scrollbars have to be then updated
// when redraws are enabled again.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::UpdateScrollBarsL()
{
    if (iTable.HasDoNotDrawFlag() || !iSBFrame)
    {
        return;
    }

    CEikListBox::UpdateScrollBarsL();
}


// ---------------------------------------------------------------------------
// Tactile feedback setter needed by the drawer.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::SetTouchFeedbackRect(const TRect& aRect) const
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->SetFeedbackArea(this, KTouchFeedbackRectId1, aRect,
                                   ETouchFeedbackBasic, ETouchEventStylusDown);
    }
#else // RD_TACTILE_FEEDBACK
    (void) aRect;
#endif // RD_TACTILE_FEEDBACK
}


// ---------------------------------------------------------------------------
// From class CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::MakeVisible(TBool aVisible)
{
    CEikTextListBox::MakeVisible(aVisible);
}


// ---------------------------------------------------------------------------
// From class CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::SetDimmed(TBool aDimmed)
{
    CEikTextListBox::SetDimmed(aDimmed);
}


// ---------------------------------------------------------------------------
// From class CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::FocusChanged(TDrawNow /*aDrawNow*/)
{
    CEikTextListBox::FocusChanged(ENoDrawNow);
}


// ---------------------------------------------------------------------------
// From class CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    CEikScrollBar* vsb = iSBFrame ? iSBFrame->GetScrollBarHandle(CEikScrollBar::EVertical) : NULL;
    CEikScrollBar* hsb = iSBFrame ? iSBFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal) : NULL;

    // Check if we should start scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        if (!iVScrollBarGrabsPointerEvents)
        {
            if (vsb && vsb->Rect().Contains(aPointerEvent.iPosition))
            {
                iVScrollBarGrabsPointerEvents = ETrue;
            }
        }
        if (!iHScrollBarGrabsPointerEvents)
        {
            if (hsb && hsb->Rect().Contains(aPointerEvent.iPosition))
            {
                iHScrollBarGrabsPointerEvents = ETrue;
            }
        }
    }

    // Deliver event to scrollbar
    if (iVScrollBarGrabsPointerEvents && vsb)
    {
        vsb->HandlePointerEventL(aPointerEvent);
    }
    if (iHScrollBarGrabsPointerEvents && hsb)
    {
        hsb->HandlePointerEventL(aPointerEvent);
    }

    // Deliver event to list
    if (!iVScrollBarGrabsPointerEvents && !iHScrollBarGrabsPointerEvents)
    {
        // Store the selection on first pointer event.
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            if (iListBoxFlags & EMultipleSelection)
            {
                // Multi lists
                ASSERT(iOldSelectionArray);
                iView->GetSelectionIndexesL(iOldSelectionArray);
            }
            iPrevFocusIndex = CurrentItemIndex();
        }

        if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
            TInt deltaX(iDragPos.iX - aPointerEvent.iPosition.iX);
            if (deltaX != 0)
                iView->HScroll(deltaX);
        }
        
        iDragPos = aPointerEvent.iPosition;
        CEikTextListBox::HandlePointerEventL(aPointerEvent);

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            TInt focusIndex = CurrentItemIndex();
            
            if (iListBoxFlags & EMultipleSelection)
            {
                // Multi lists
                if (iOldSelectionArray->Count() != iView->SelectionIndexes()->Count())
                {
                    if (iDisplay.RevertPointerEvent())
                    {
                        // Revert.
                        if (View()->ItemIsSelected(focusIndex))
                        {
                            View()->DeselectItem(focusIndex);
                        }
                        else
                        {
                            View()->SelectItemL(focusIndex);   
                        }
                    }
                    else
                    {
                        iTable.PostSelectionEventL(KSwtStyleCheck, focusIndex);                        
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
                    View()->UpdateSelectionL(CListBoxView::ESingleSelection);

                    // Item focus highlight moved, send selection event.
                    // Changing the focus index is not subject to reverting.
                    iTable.PostSelectionEventL(KSwtNone, focusIndex);
                }

                // The default selection is sent from HandleListBoxEventL!
            }
        }
    }

    // Stop scrollbar grabbing
    if ((iVScrollBarGrabsPointerEvents || iHScrollBarGrabsPointerEvents)
            && aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iVScrollBarGrabsPointerEvents = EFalse;
        iHScrollBarGrabsPointerEvents = EFalse;
    }
}

// ---------------------------------------------------------------------------
// From class CEikListBox
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::HandleViewRectSizeChangeL()
{
    CEikTextListBox::HandleViewRectSizeChangeL();
    TableView()->ValidateHScrollOffset();
}


// ---------------------------------------------------------------------------
// From class CEikListBox.
// Overridden to simply set the item height without side-effects.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::SetItemHeightL(TInt aHeight)
{
    ASSERT(aHeight > 0);
    iItemHeight = aHeight;
    iView->SetItemHeight(aHeight);
}


// ---------------------------------------------------------------------------
// From class CEikListBox
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::SetTopItemIndex(TInt aItemIndex) const
{
    CEikTextListBox::SetTopItemIndex(aItemIndex);
    AdjustTopItemIndex();
}


// ---------------------------------------------------------------------------
// From class CEikListBox.
// Overridden to prevent redraw when SetCurrentItemIndex is called.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::UpdateCurrentItem(TInt aItemIndex) const
{
    TInt oldCurrentItemIndex = iView->CurrentItemIndex();
    iView->SetCurrentItemIndex(aItemIndex);
    if (IsReadyToDraw())
    {
        iView->DrawItem(oldCurrentItemIndex);
    }
    if (!(iView->ItemIsVisible(aItemIndex)))
    {
        SetTopItemIndex(iView->CalcNewTopItemIndexSoItemIsVisible(aItemIndex));
    }
    DrawItem(aItemIndex);
    UpdateScrollBarThumbs();
}


// ---------------------------------------------------------------------------
// From class CEikListBox.
// Overridden to make the listbox scroll as desired when tapping on the
// horizontal scrollbar.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::HandleScrollEventL(CEikScrollBar* aScrollBar,TEikScrollEvent aEventType)
{
    if ((aEventType & KEikScrollEventBarMask == KEikScrollEventFromHBar) &&
            (aEventType == EEikScrollLeft ||
             aEventType == EEikScrollRight ||
             aEventType == EEikScrollPageLeft ||
             aEventType == EEikScrollPageRight))
    {
        TInt scrollAmount(0);
        if (aEventType == EEikScrollLeft)
        {
            scrollAmount = -HorizontalScrollPixelsPerArrowEvent();
        }
        else if (aEventType == EEikScrollRight)
        {
            scrollAmount = HorizontalScrollPixelsPerArrowEvent();
        }
        else if (aEventType == EEikScrollPageLeft)
        {
            scrollAmount = -HorizontalScrollPixelsPerPageEvent();
        }
        else if (aEventType == EEikScrollPageRight)
        {
            scrollAmount = HorizontalScrollPixelsPerPageEvent();
        }
        iView->HScroll(scrollAmount);
        UpdateScrollBarThumbs();
    }
    else
    {
        CEikListBox::HandleScrollEventL(aScrollBar, aEventType);
    }
}


// ---------------------------------------------------------------------------
// CSwtTableListBox::HandleScrollEventL
// From MEikListBoxObserver
// Handles default selection for touch. Unable to handle the selection events
// here since EEventItemClicked is not always sent for every tap.
// The selection events are handled in HandlePointerEvent.
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtTableListBox::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
{
    if (aListBox != this)
    {
        return;
    }

    switch (aEventType)
    {
        // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventPanningStarted:
        iTable.HandlePhysics(ETrue);
        break;
    case EEventFlickStopped:
        iTable.HandlePhysics(EFalse);
        break;
#endif // RD_JAVA_S60_RELEASE_9_2
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventItemSingleClicked:
#else
    case EEventItemDoubleClicked:
#endif // RD_JAVA_S60_RELEASE_9_2
        if (!(iTable.Style() & KSwtStyleMulti))
        {
            if (!iDisplay.RevertPointerEvent())
                iTable.PostDefaultSelectionEventL(CurrentItemIndex());
        }
        break;
    default:
        // Do nothing
        break;
    }
}
#else //RD_SCALABLE_UI_V2
void CSwtTableListBox::HandleListBoxEventL(CEikListBox*, TListBoxEvent)
{
}
#endif //RD_SCALABLE_UI_V2


// ---------------------------------------------------------------------------
// Note that this is not a virtual function.
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtTableListBox::CreateScrollBarFrameL()
{
    ASSERT(!iSBFrame);

    iSBFrame = CEikTextListBox::CreateScrollBarFrameL();
    User::LeaveIfNull(iSBFrame);

#ifdef RD_SCALABLE_UI_V2
    // WARNING!!! The expanded touch area does not move correctly togehter with the scrollbars!
    iSBFrame->SetScrollBarFrameFlags(CEikScrollBarFrame::EDisableExpandedTouchArea);
#endif // RD_SCALABLE_UI_V2

    TBool vScroll(iTable.Style() & KSwtStyleVScroll ? ETrue : EFalse);
    TBool hScroll(iTable.Style() & KSwtStyleHScroll ? ETrue : EFalse);

    // In eSWT all scrollbars must be non window owing the main reason being that
    // the scrollbars must draw at exact same time with the parent control. This
    // is especially essential in ScrolledComposite. If the scrollbars would be
    // window owing, they would draw too much ahead of the parents creating an
    // ugly visual effect when flicking the ScrolledComposite.
    // The drawback of having non window owing scrollbars is too many paint events
    // which affects on the speed of kinetic scrolling in lists.
    iSBFrame->CreateDoubleSpanScrollBarsL(EFalse,    // Window-owning
                                          EFalse,   // Remote
                                          vScroll,  // Vertical
                                          hScroll); // Horizontal

    CEikScrollBarFrame::TScrollBarVisibility horizVis = CEikScrollBarFrame::EOn;
    CEikScrollBarFrame::TScrollBarVisibility vertVis = CEikScrollBarFrame::EOn;
    iSBFrame->SetScrollBarVisibilityL(horizVis, vertVis);
    iSBFrame->DrawBackground(EFalse, EFalse);

    return iSBFrame;
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::Draw(const TRect& aRect) const
{
    // Make sure item drawer doesn't draw over it's composite children
    CWindowGc& itemDrawerGc(*iItemDrawer->Gc());
    const MSwtComposite* composite = iTable.CompositeInterface();
    RRegion* childClippingRegion =
        iTable.SetupChildClippingRegion(composite, itemDrawerGc, aRect);

    // Also set the same clipping rect for SystemGc that's used for some of the drawing
    CWindowGc& systemGc = SystemGc();
    TBool systemGcClipRegionSet(EFalse);
    if (childClippingRegion)
    {
        if (systemGc.SetClippingRegion(*childClippingRegion) == KErrNone)
        {
            systemGcClipRegionSet = ETrue;
        }
    }

#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->ChangeFeedbackType(this, KTouchFeedbackRectId1,
                                      ETouchFeedbackNone);
    }
#endif // RD_TACTILE_FEEDBACK

    if (iTable.HasDoNotDrawFlag())
    {
        return;
    }

    // Draw the client area
    const TRect viewRect(iView->ViewRect());
    TRect rect(viewRect);
    rect.Intersection(aRect);
    CEikTextListBox::Draw(rect);   // rect not used

    // Draw the background for the area not covered by the items

    // Compute a rectangle for the total table area needed to be drawn
    // including the area of the scrollbars
    TRect fullRect(Rect());

    // Compute a rectangle for the area already drawn when drawing the items
    TInt heightOfItemArea(0);
    if (iModel->NumberOfItems() > 0)
    {
        heightOfItemArea += iModel->NumberOfItems() *
                            (iView->ItemSize(iModel->NumberOfItems() - 1).iHeight);
    }
    if (heightOfItemArea > viewRect.Height())
    {
        heightOfItemArea = viewRect.Height();
    }

    // Last index means also last index partially visible, 
    // therefore intentionally we do not substract 1 here.
    TInt lastIndex = iView->TopItemIndex() 
        + iView->NumberOfItemsThatFitInRect(iView->ViewRect());
    TInt lastPotentialItemIndex = Min(iModel->NumberOfItems() - 1, lastIndex);

    // Clear the unused portion of the viewing area
    TRect usedPortionOfViewRect(viewRect.iTl.iX, iView->ItemPos(iView->TopItemIndex()).iY,
                                viewRect.iBr.iX, iView->ItemPos(lastPotentialItemIndex).iY
                                + iView->ItemSize(lastPotentialItemIndex).iHeight);
    
    // This is needed as the horizontal scrollbar is transparent. 
    if (usedPortionOfViewRect.iBr.iY > viewRect.iBr.iY)
        usedPortionOfViewRect.iBr.iY = viewRect.iBr.iY;

    if (iTable.BackgroundSet())
    {
        // Note: If table should draw a focused control background,
        // it's not drawn here currently.
        TRgb backgroundRgb = iTable.GetBackground();
        systemGc.SetBrushColor(backgroundRgb);
        systemGc.SetPenStyle(CGraphicsContext::ENullPen);
        systemGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        DrawUtils::DrawBetweenRects(systemGc, fullRect, usedPortionOfViewRect);
    }
    else
    {
        // Draw the background between the two rectangles
        MAknsControlContext* controlContext = iTable.SwtMopSupplyCtrlContext();
        AknsDrawUtils::BackgroundBetweenRects(AknsUtils::SkinInstance(),
                                              controlContext,
                                              &(iTable.CoeControl()),
                                              systemGc,
                                              fullRect,
                                              usedPortionOfViewRect);
    }

    // Draw the grid lines
    if (iTable.GetLinesVisible())
    {
        DrawGrid(systemGc, iTable.ClipToVisibleRect(iView->ViewRect()));
    }

    // Draw the empty list text
    const CFont* font = &iDefaultFont->Font();
    const TDesC* text = iView->EmptyListText();
    if (iModel->NumberOfItems() == 0 &&
            !iTable.GetTableColumnCount() &&
            font &&
            text)
    {
        TableItemDrawer()->DrawTableBidiText(
            systemGc,
            font,
            iTable.GetForeground(),
            *text,
            ClientRect(),
            CGraphicsContext::ECenter);
    }

    // Cancel the clipping region from systemGc
    if (systemGcClipRegionSet)
    {
        systemGc.CancelClippingRegion();
    }
    // Cancel the clipping region from item drawer gc and destroy the RRegion object
    iTable.CancelChildClippingRegion(childClippingRegion, itemDrawerGc);
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::PositionChanged()
{
    CEikTextListBox::PositionChanged();
    SetViewRectFromClientRect(ClientRect());
    InvalidateScrollBars();
    TRAP_IGNORE(UpdateScrollBarsL());
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::SizeChanged()
{
    SetViewRectFromClientRect(ClientRect());
    TRAP_IGNORE(HandleViewRectSizeChangeL());
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(HandleResourceChangeL(aType));
}


// ---------------------------------------------------------------------------
// From class CEikTextListBox.
// ---------------------------------------------------------------------------
//
void CSwtTableListBox::CreateItemDrawerL()
{
    const CFont* font = LayoutFont();
    iItemDrawer = new(ELeave) CSwtTableItemDrawer(
        static_cast< CTextListBoxModel* >(iModel), font, *this, iTable);
}


// ---------------------------------------------------------------------------
// From class CEikListBox
// ---------------------------------------------------------------------------
//
CListBoxView* CSwtTableListBox::MakeViewClassInstanceL()
{
    return new(ELeave) CSwtTableListBoxView(iTable);
}


CSwtTableListBox::CSwtTableListBox(MSwtDisplay& aDisplay, CSwtTable &aTable)
        : iDisplay(aDisplay),
        iTable(aTable)
{
}


void CSwtTableListBox::ConstructL()
{
    SetContainerWindowL(iTable);
    SetComponentsToInheritVisibility(ETrue);

    TInt constructFlags = 0;
    TInt tableStyle = iTable.Composite()->Control()->Style();

    if (tableStyle & KSwtStyleMulti || tableStyle & KSwtStyleCheck)
    {
        constructFlags |= EAknListBoxStylusMultiselectionList;
        iOldSelectionArray = new(ELeave) CArrayFixFlat< TInt >(2);
    }
    else
    {
        constructFlags |= EAknListBoxSelectionList;
    }

    // If this doesn't leave then model, item drawer and
    // view are also succesfully constructed
    CEikTextListBox::ConstructL(&iTable, constructFlags);

    ResetDefaultFontFromItemDrawerL();

    // Construct the scroll-bars
    CreateScrollBarFrameL();

    SetRequiredHeightInNumOfItems(1);

    // Inherit visibility and dimming from the parent
    CCoeControl::SetDimmed(iTable.IsDimmed());
    iView->SetDimmed(iTable.IsDimmed());

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif // RD_TACTILE_FEEDBACK

#ifdef RD_SCALABLE_UI_V2
    SetListBoxObserver(this);
#endif // RD_SCALABLE_UI_V2

    ActivateL();
}


TInt CSwtTableListBox::HorizontalScrollPixelsPerArrowEvent()
{
    return iTable.ClientRect().Width() / 2;
}


TInt CSwtTableListBox::HorizontalScrollPixelsPerPageEvent()
{
    return iTable.ClientRect().Width();
}
