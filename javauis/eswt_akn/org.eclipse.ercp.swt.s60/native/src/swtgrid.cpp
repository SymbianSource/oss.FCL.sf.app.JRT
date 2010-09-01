/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swtgrid.h"
#include "swtlistview.h"
#include "swtlistboxlistsitemdrawer.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtGrid Constructor
// ---------------------------------------------------------------------------
//
CSwtGrid::CSwtGrid(CSwtListView* aCompoundCtrl)
        : CAknGrid()
        , iCompoundCtrl(aCompoundCtrl)
{
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtGrid::PositionChanged()
{
    TRAP_IGNORE(RectChangedL());
}

// ---------------------------------------------------------------------------
// From class CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtGrid::SizeChanged()
{
    TRAP_IGNORE(RectChangedL());
}

// ---------------------------------------------------------------------------
// From class CAknGrid.
// CAknGrid scrollbar handling needs to be overridden in order to draw the
// scrollbars correctly when the control is resized/repositioned. Only the
// vertical scrollbar is supported.
// ---------------------------------------------------------------------------
//
void CSwtGrid::UpdateScrollBarsL()
{
    if (!iSBFrame)
    {
        return;
    }

    if (iCompoundCtrl && iCompoundCtrl->HasDoNotDrawFlag())
    {
        return;
    }

    // Preserve visibility as CAknGrid::DoSetLayout is recreating the scrollbar
    ApplyScrollbarVisibility();

    // Hide the scrollbars while Avkon updates the thumbs
    CAknDoubleSpanScrollBar* vBar = static_cast<CAknDoubleSpanScrollBar*>
                                    (ScrollBarFrame()->VerticalScrollBar());
    if (vBar)
    {
        vBar->SetFixedLayoutRect(TRect(0, 0, 0, 0));
    }
    CAknGrid::UpdateScrollBarsL();

    // Now reposition and resize the scrollbar
    if (vBar)
    {
        TRect clientRect(ClientRect(Rect(), EFalse));
        TRect inclusiveRect(ClientRect(Rect(), ETrue));
        TEikScrollBarFrameLayout layout;
        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
        layout.SetClientMargin(0);
        layout.SetInclusiveMargin(0);
        vBar->SetFixedLayoutRect(TRect(0, 0, 0, 0));
        TAknDoubleSpanScrollBarModel dsModel(*vBar->Model());
        iSBFrame->TileL(NULL, &dsModel, clientRect, inclusiveRect, layout);
    }

    if (Rect().IsEmpty())
    {
        // WORKAROUND: The grid view does not handle correctly the current
        // data index while it's size is set to nothing (0, 0)
        GridView()->SetViewRect(TRect(0, 0, 1, 1));
    }
    else
    {
        GridView()->SetViewRect(ClientRect(Rect(), EFalse));
    }
}

// ---------------------------------------------------------------------------
// From class CAknGrid.
// See the UpdateScrollBarsL comment.
// ---------------------------------------------------------------------------
//
void CSwtGrid::UpdateScrollBarThumbs() const
{
    CAknGrid::UpdateScrollBarThumbs();
}

// ---------------------------------------------------------------------------
// Handling of position and size changes of the grid
// ---------------------------------------------------------------------------
//
void CSwtGrid::RectChangedL()
{
    if (Rect().IsEmpty())
    {
        // WORKAROUND: The grid view does not handle correctly the current
        // data index while it's size is set to nothing (0, 0)
        GridView()->SetViewRect(TRect(0, 0, 1, 1));
    }
    else
    {
        GridView()->SetViewRect(ClientRect(Rect(), EFalse));
    }

    iCompoundCtrl->ScrollViewToShowAsManyItemsAsPossibleL();
}

// ---------------------------------------------------------------------------
// Calculates area capable of displaying client data.
// ---------------------------------------------------------------------------
//
TRect CSwtGrid::ClientRect(const TRect& aRect, const TBool& aExcludeScrollBars) const
{
    TRect clientRect(Border().InnerRect(aRect));

    if (iSBFrame && !aExcludeScrollBars)
    {
        if (iSBFrame->ScrollBarExists(CEikScrollBar::EVertical) &&
                iSBFrame->ScrollBarVisibility(CEikScrollBar::EVertical) != CEikScrollBarFrame::EOff &&
                Model()->NumberOfItems() > 0)
        {
            CEikScrollBar* bar(iSBFrame->VerticalScrollBar());
            if (AknLayoutUtils::LayoutMirrored())
            {
                clientRect.iTl.iX += iCompoundCtrl->ScrollBarBreadth(bar);
            }
            else
            {
                clientRect.iBr.iX -= iCompoundCtrl->ScrollBarBreadth(bar);
            }
        }
    }

    return clientRect;
}

// ---------------------------------------------------------------------------
// CSwtGrid::SetScrollbarVisibility
// ---------------------------------------------------------------------------
//
void CSwtGrid::SetScrollbarVisibility(TInt aStyle, TBool aState)
{
    if (aStyle & KSwtStyleVScroll)
    {
        iVScrollVisible = aState;
    }
    else
    {
        iHScrollVisible = aState;
    }
}

// ---------------------------------------------------------------------------
// CSwtGrid::ApplyScrollbarVisibility
// Used as workaround to enforce visibility of the scrollbars. Everytime the
// size of CAknGrid changes the scrollbars are recreated. Therefore if we
// want invisible scrollbars we have to call this function after each size
// change.
// ---------------------------------------------------------------------------
//
void CSwtGrid::ApplyScrollbarVisibility()
{
    if (!iSBFrame)
    {
        return;
    }

    CEikScrollBarFrame::TScrollBarVisibility hVisibility;
    CEikScrollBarFrame::TScrollBarVisibility vVisibility;
    CEikScrollBarFrame::TScrollBarVisibility hVisibilityPrev;
    CEikScrollBarFrame::TScrollBarVisibility vVisibilityPrev;

    hVisibilityPrev = iSBFrame->ScrollBarVisibility(CEikScrollBar::EHorizontal);
    vVisibilityPrev = iSBFrame->ScrollBarVisibility(CEikScrollBar::EVertical);

    // Enforce scrollbar visibility only when needed
    if ((iHScrollVisible && (hVisibilityPrev == CEikScrollBarFrame::EOff))
            || (!iHScrollVisible && (hVisibilityPrev != CEikScrollBarFrame::EOff))
            || (iVScrollVisible && (vVisibilityPrev == CEikScrollBarFrame::EOff))
            || (!iVScrollVisible && (vVisibilityPrev != CEikScrollBarFrame::EOff)))
    {
        if (iHScrollVisible)
        {
            hVisibility = CEikScrollBarFrame::EOn;
        }
        else
        {
            hVisibility = CEikScrollBarFrame::EOff;
        }

        if (iVScrollVisible)
        {
            vVisibility = CEikScrollBarFrame::EOn;
        }
        else
        {
            vVisibility = CEikScrollBarFrame::EOff;
        }

        TRAP_IGNORE(iSBFrame->SetScrollBarVisibilityL(hVisibility, vVisibility));
    }
}

// ---------------------------------------------------------------------------
// CSwtGrid::SetItemDrawerClippingRect
// ---------------------------------------------------------------------------
//
void CSwtGrid::SetItemDrawerClippingRect(const TRect& aRect)
{
    CSwtFormattedCellListBoxItemDrawer* customItemDrawer =
        static_cast<CSwtFormattedCellListBoxItemDrawer*>(ItemDrawer());

    if (customItemDrawer)
    {
        customItemDrawer->SetCustomClippingRect(aRect);
    }
}

// ---------------------------------------------------------------------------
// CSwtGrid::CreateItemDrawerL
// From CAknGrid
// ---------------------------------------------------------------------------
//
void CSwtGrid::CreateItemDrawerL()
{
    CFormattedCellListBoxData* cellData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL(cellData);
    iItemDrawer = new(ELeave) CSwtFormattedCellListBoxItemDrawer(Model(),
            iEikonEnv->NormalFont(), cellData, this);
    CleanupStack::Pop();
}

// ---------------------------------------------------------------------------
// CSwtGrid::ForwardPointerEvent
// ---------------------------------------------------------------------------
//
void CSwtGrid::ForwardPointerEventL(const TPointerEvent& aPointerEvent)
{
    CEikListBox::HandlePointerEventL(aPointerEvent);
}

// ---------------------------------------------------------------------------
// CSwtGrid::DoHandleScrollEventL
// Wrapper for protected member function HandleScrollEventL of CAknGrid.
// ---------------------------------------------------------------------------
//
#if defined( RD_SCALABLE_UI_V2)
void CSwtGrid::DoHandleScrollEventL(CEikScrollBar* aScrollBar,
                                    TEikScrollEvent aEventType)
{
    HandleScrollEventL(aScrollBar, aEventType);
}
#endif // RD_SCALABLE_UI_V2
