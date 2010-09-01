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
#include "swttablelistboxview.h"
#include "swttable.h"
#include "swtimagetextitem.h"
#include "swttableitemdrawer.h"


// ======== MEMBER FUNCTIONS ========


CSwtTableListBoxView::CSwtTableListBoxView(CSwtTable &aTable)
        : CListBoxView()
        , iTable(aTable)
{
}

TBool CSwtTableListBoxView::CanScrollLeft()
{
    if (iHScrollOffset > 0)
    {
        return ETrue;
    }
    return EFalse;
}


TBool CSwtTableListBoxView::CanScrollRight()
{
    TInt maxScrollInPixels = DataWidth() - VisibleWidth(iViewRect);
    if (maxScrollInPixels <= 0)
    {
        return EFalse;
    }
    if (iHScrollOffset < maxScrollInPixels)
    {
        return ETrue;
    }
    return EFalse;
}


void CSwtTableListBoxView::ValidateHScrollOffset()
{
    if (AknLayoutUtils::LayoutMirrored())
    {
        HScrollToLeadingEdge();
    }
    else
    {
        TInt viewWidth(iViewRect.Width());
        TInt maxHScrollOffset(iDataWidth - viewWidth);
        if (iDataWidth > viewWidth)
        {
            if (HScrollOffset() > maxHScrollOffset)
            {
                SetHScrollOffset(maxHScrollOffset);
            }
        }
        else
        {
            HScrollToLeadingEdge();
        }
    }
}


void CSwtTableListBoxView::HScrollToLeadingEdge()
{
    if (AknLayoutUtils::LayoutMirrored())
    {
        TInt viewWidth(iViewRect.Width());
        TInt maxHScrollOffset(iDataWidth - viewWidth);
        if (iDataWidth > viewWidth)
        {
            SetHScrollOffset(maxHScrollOffset);
        }
        else
        {
            SetHScrollOffset(0);
        }
    }
    else
    {
        SetHScrollOffset(0);
    }
}


void CSwtTableListBoxView::ScrollToMakeColumnVisible(const TInt& aColumnIndex)
{
    if (iDataWidth <= iViewRect.Width())
    {
        // All the columns fit in the view, no need to do anything.
        return;
    }
    const TBool layoutMirrored(AknLayoutUtils::LayoutMirrored());
    if (aColumnIndex == 0)
    {
        // When showing the 1st column then scroll all the way to the beginning
        // so that the checkboxes are visible and no further scrolling
        // is possible. Some apps might be expecting this behavior.
        HScrollToLeadingEdge();
        iTable.Redraw();
        return;
    }

    // Otherwise just scroll to the columns edge.

    // Find out how much space before the first column starts
    TRect rect(TPoint(0, 0), TSize(iDataWidth, 0));
    CSwtTableItemDrawer* itemDrawer = static_cast<CSwtTableItemDrawer*>(iItemDrawer);
    TRect columnArea(itemDrawer->ColumnRectFromWholeRect(rect));

    const RArray<TInt>& columnWidths = iTable.GetColumnWidths();
    TInt scrollAmountToLeftViewEdge(columnArea.iTl.iX);
    TInt scrollAmountToRightViewEdge(0);
    if (layoutMirrored)
    {
        for (TInt i = columnWidths.Count() - 1; i > aColumnIndex; --i)
        {
            scrollAmountToLeftViewEdge += columnWidths[ i ];
        }
    }
    else
    {
        for (TInt i = 0; i < aColumnIndex; ++i)
        {
            scrollAmountToLeftViewEdge += columnWidths[ i ];
        }
    }
    scrollAmountToRightViewEdge = scrollAmountToLeftViewEdge;
    scrollAmountToRightViewEdge -= (iViewRect.Width() - columnWidths[ aColumnIndex ]);
    HScrollMin(scrollAmountToLeftViewEdge, scrollAmountToRightViewEdge);
}

// Always remember to update the offset in the header too when setting it.
// Note that this is not a virtual function.
void CSwtTableListBoxView::SetHScrollOffset(const TInt& aHorizontalOffset)
{
    CListBoxView::SetHScrollOffset(aHorizontalOffset);
    iTable.UpdateHeaderHScrollOffset(aHorizontalOffset);
}


void CSwtTableListBoxView::DoSetFlags(TInt aMask)
{
    CListBoxView::SetFlags(aMask);
}


void CSwtTableListBoxView::DoClearFlags(TInt aMask)
{
    CListBoxView::ClearFlags(aMask);
}


// ---------------------------------------------------------------------------
// From CListBoxView.
// This function implements the horizontal scrolling for the view.
// It simply calculates and sets the horizontal offset which is then
// used when drawing the view.
// ---------------------------------------------------------------------------
//
void CSwtTableListBoxView::HScroll(TInt aHScrollAmount)
{
    if (aHScrollAmount == 0)
    {
        return;
    }

    TInt oldHScrollOffset = iHScrollOffset;

    TInt maxScrollInPixels = DataWidth() - VisibleWidth(iViewRect);
    if (maxScrollInPixels <= 0)
    {
        return;
    }

    // Scroll the list.
    iHScrollOffset += aHScrollAmount;
    iHScrollOffset = Max(0, Min(iHScrollOffset, maxScrollInPixels));

    // Scroll the header.
    iTable.UpdateHeaderHScrollOffset(iHScrollOffset);

    // Because the wallpaper background has to stay in place
    // we have to redraw the entire area.
    if ((oldHScrollOffset != iHScrollOffset) && !RedrawDisabled())
    {
        iTable.Redraw();
    }
}


// ---------------------------------------------------------------------------
// From CListBoxView.
// Calculates the pixel width of the data in the table.
// ---------------------------------------------------------------------------
//
void CSwtTableListBoxView::CalcDataWidth()
{
    // Recalculate the width of the data from the column widths
    // and from the items we know are outside the columns
    const RArray<TInt>& columnWidths = iTable.GetColumnWidths();
    TInt columnCount = iTable.GetColumnCount();
    iDataWidth = 0;
    for (TInt i = 0; i < columnCount; ++i)
    {
        iDataWidth += columnWidths[ i ];
    }
    if (iTable.Style() & KSwtStyleCheck || iTable.Style() & KSwtStyleMulti)
    {
        iDataWidth += iTable.GetCheckBoxSize().iWidth;
    }
    TSwtTableMargins margins = iTable.GetMargins();
    iDataWidth += margins.iLeft;
    iDataWidth += margins.iRight;

    // Item size needs to be updated in the drawer in case
    // the column widths have changed. Otherwise it will
    // draw with the old width.
    iItemDrawer->SetItemCellSize(ItemSize());
}


// ---------------------------------------------------------------------------
// From CListBoxView.
// Draws the visible items. Must be overridden to prevent unnecessary
// clearing of the background that could cause flicker.
// ---------------------------------------------------------------------------
//
void CSwtTableListBoxView::Draw(const TRect* /*aClipRect*/) const
{
    if (RedrawDisabled() || !IsVisible())
    {
        return;
    }
    TInt firstPotentialItemIndex = iTopItemIndex;
    TInt lastPotentialItemIndex = iTopItemIndex + NumberOfItemsThatFitInRect(iViewRect);
    TInt nItems = iModel->NumberOfItems();
    for (TInt i = firstPotentialItemIndex; i <= lastPotentialItemIndex; i++)
    {
        if (((i) > -1) && ((i) < nItems))
        {
            DrawItem(i);
        }
        else
        {
            break;
        }
    }
}


void CSwtTableListBoxView::HScrollMin(const TInt& aHScrollOffset1, const TInt& aHScrollOffset2)
{
    if (Abs(aHScrollOffset1 - iHScrollOffset) < Abs(aHScrollOffset2 - iHScrollOffset))
    {
        HScroll(aHScrollOffset1 - iHScrollOffset);
    }
    else
    {
        HScroll(aHScrollOffset2 - iHScrollOffset);
    }
}
