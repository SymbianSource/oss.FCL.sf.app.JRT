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
#include <eiksbfrm.h>
#include "swtscrollablebase.h"
#include "swtscrollbar.h"


// ======== MEMBER FUNCTIONS ========


TRect ASwtScrollableBase::SbInnerRect(const TRect& aOuterRect) const
{
    TRect innerRect(aOuterRect);
    CEikScrollBarFrame* sbFrame = SbFrame();

    if (sbFrame)
    {
        const CEikScrollBar* hsb = iHScroll ?
                                   &iHScroll->EikScrollBar() :
                                   sbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
        if (hsb && hsb->IsVisible())
        {
            TInt height = iDisplay.UiUtils().ScrollBarBreadth(hsb);
            innerRect.iBr.iY = Max(innerRect.iBr.iY-height, innerRect.iTl.iY);
        }

        const CEikScrollBar* vsb = iVScroll ?
                                   &iVScroll->EikScrollBar() :
                                   sbFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
        if (vsb && vsb->IsVisible())
        {
            TInt width = iDisplay.UiUtils().ScrollBarBreadth(vsb);
            if (AknLayoutUtils::LayoutMirrored())
            {
                innerRect.iTl.iX = Min(innerRect.iTl.iX+width, innerRect.iBr.iX);
            }
            else
            {
                innerRect.iBr.iX = Max(innerRect.iBr.iX-width, innerRect.iTl.iX);
            }
        }
    }

    return innerRect;
}

TRect ASwtScrollableBase::SbOuterRect(const TRect& aInnerRect) const
{
    TRect outerRect(aInnerRect);
    CEikScrollBarFrame* sbFrame = SbFrame();

    if (sbFrame)
    {
        const CEikScrollBar* hsb = iHScroll ?
                                   &iHScroll->EikScrollBar() :
                                   sbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
        if (hsb && hsb->IsVisible())
        {
            TInt height = iDisplay.UiUtils().ScrollBarBreadth(hsb);
            outerRect.iBr.iY += height;
        }

        const CEikScrollBar* vsb = iVScroll ?
                                   &iVScroll->EikScrollBar() :
                                   sbFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
        if (vsb && vsb->IsVisible())
        {
            TInt width = iDisplay.UiUtils().ScrollBarBreadth(vsb);
            if (AknLayoutUtils::LayoutMirrored())
            {
                outerRect.iTl.iX -= width;
            }
            else
            {
                outerRect.iBr.iX += width;
            }
        }
    }

    return outerRect;
}

// From MSwtScrollable
MSwtControl* ASwtScrollableBase::Control()
{
    return this;
}

// ASwtScrollableBase::CreateScrollBarL
MSwtScrollBar* ASwtScrollableBase::CreateScrollBarL(TSwtPeer aPeer, TInt aStyle)
{
    // Get frame
    CEikScrollBarFrame* sbFrame = SbFrame();
    if (!sbFrame)
    {
        return NULL;
    }

    // Asserts
    ASSERT(aStyle == KSwtStyleHScroll || aStyle == KSwtStyleVScroll);
    ASSERT((iStyle & aStyle) != 0);

    // Get scrollbar reference and orientation
    CEikScrollBar::TOrientation orientation;
    CSwtScrollBar** scrollbar;
    if (aStyle == KSwtStyleHScroll)
    {
        orientation = CEikScrollBar::EHorizontal;
        scrollbar = &iHScroll;
    }
    else
    {
        orientation = CEikScrollBar::EVertical;
        scrollbar = &iVScroll;
    }

    // Tile and return scrollbar
    TRect clientRect(Control()->ClientRect());
    TRect inclusiveRect(SbOuterRect(ClientRect()));
    TEikScrollBarModel hModel(DefaultSbModel(KSwtStyleHScroll));
    TEikScrollBarModel vModel(DefaultSbModel(KSwtStyleVScroll));
    TEikScrollBarFrameLayout layout;
    layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
    layout.SetClientMargin(0);
    layout.SetInclusiveMargin(0);

    // Cannot use non double span models for double span scrollbars! Avoiding panics.
    TBool doubleSpan = sbFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan;
    if (doubleSpan)
    {
        TAknDoubleSpanScrollBarModel hDsSbarModel(hModel);
        TAknDoubleSpanScrollBarModel vDsSbarModel(vModel);
        sbFrame->TileL(&hDsSbarModel, &vDsSbarModel, clientRect, inclusiveRect, layout);
    }
    else
    {
        sbFrame->TileL(&hModel, &vModel, clientRect, inclusiveRect, layout);
    }

    CEikScrollBar* sb = sbFrame->GetScrollBarHandle(orientation);
    if (sb)
    {
        *scrollbar = new(ELeave) CSwtScrollBar(iDisplay, aPeer, aStyle,
                                               *sb, *this, 0, CSwtLafFacade::KSwtDefaultScrollBarModelScrollSpan,
                                               CSwtLafFacade::KSwtDefaultScrollBarModelThumbposition,
                                               CSwtLafFacade::KSwtDefaultScrollBarModelThumbSpan, doubleSpan,
                                               orientation);
        sbFrame->SetScrollBarFrameObserver(this);
        return *scrollbar;
    }

    return NULL;
}

// From MSwtScrollable
void ASwtScrollableBase::SetVisibleScrollBar(TInt aStyle, TBool aVisible)
{
    // Get frame
    CEikScrollBarFrame* sbFrame = SbFrame();
    if (!sbFrame)
    {
        return;
    }

    // Get scrollbars visibility
    CEikScrollBarFrame::TScrollBarVisibility hVis =
        sbFrame->ScrollBarVisibility(CEikScrollBar::EHorizontal);
    CEikScrollBarFrame::TScrollBarVisibility vVis =
        sbFrame->ScrollBarVisibility(CEikScrollBar::EVertical);

    // Calculate new visibility states
    if (aStyle & KSwtStyleHScroll)
    {
        if (aVisible)
        {
            hVis = CEikScrollBarFrame::EOn;
        }
        else
        {
            hVis = CEikScrollBarFrame::EOff;
        }
        iHScroll->SetVisible(aVisible);
    }

    if (aStyle & KSwtStyleVScroll)
    {
        if (aVisible)
        {
            vVis = CEikScrollBarFrame::EOn;
        }
        else
        {
            vVis = CEikScrollBarFrame::EOff;
        }
        iVScroll->SetVisible(aVisible);
    }

    // Apply new visibility states
    TRAP_IGNORE(sbFrame->SetScrollBarVisibilityL(hVis, vVis));

    // Resize Scrollable, as the client area has changed
    CCoeControl& coeCtrl = CoeControl();
    coeCtrl.SetSize(coeCtrl.Size());

    // Force a redraw here as some Scrollables do not update the screen after
    // a scrollbar as been switched on or off.
    Redraw();
}

// From MSwtScrollable
void ASwtScrollableBase::ScrollBarDestroyed(const MSwtScrollBar* aScrollbar)
{
    // Get frame
    CEikScrollBarFrame* sbFrame = SbFrame();
    if (sbFrame == NULL)
    {
        return;
    }

    // Calculate new visibility states, clear scrollbar reference
    CEikScrollBarFrame::TScrollBarVisibility hVis;
    CEikScrollBarFrame::TScrollBarVisibility vVis;
    if (aScrollbar == iHScroll)
    {
        hVis = CEikScrollBarFrame::EOff;
        vVis = sbFrame->ScrollBarVisibility(CEikScrollBar::EVertical);
        iHScroll = NULL;
    }
    else
    {
        ASSERT(aScrollbar == iVScroll);
        hVis = sbFrame->ScrollBarVisibility(CEikScrollBar::EHorizontal);
        vVis = CEikScrollBarFrame::EOff;
        iVScroll = NULL;
    }

    // Apply new visibility states
    TRAP_IGNORE(sbFrame->SetScrollBarVisibilityL(hVis, vVis));

    // Resize Scrollable, as the client area has changed
    CCoeControl& coeCtrl = CoeControl();
    coeCtrl.SetSize(coeCtrl.Size());

    // Force a redraw here as some Scrollables do not update the screen after
    // a scrollbar as been switched on or off.
    Redraw();
}

// From MSwtScrollable
void ASwtScrollableBase::ComputeTrim(TRect& aRect) const
{
    aRect = BorderOuterRect(SbOuterRect(aRect));
}

TBool ASwtScrollableBase::HasScrollBars() const
{
    TBool hasScrollBars = EFalse;
    CEikScrollBarFrame* sbFrame = SbFrame();
    if (sbFrame)
    {
        if (iHScroll || iVScroll)
        {
            hasScrollBars = ETrue;
        }
    }
    return hasScrollBars;
}

// From ASwtControlBase
TRect ASwtScrollableBase::ClientRect() const
{
    return SbInnerRect(BorderInnerRect());
}

#ifdef RD_SCALABLE_UI_V2
// From MEikScrollBarObserver
void ASwtScrollableBase::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    if (iVScroll && (aScrollBar == &(iVScroll->EikScrollBar())))
    {
        iVScroll->HandleScrollEventL(aScrollBar, aEventType);
    }
    else if (iHScroll)
    {
        iHScroll->HandleScrollEventL(aScrollBar, aEventType);
    }
}
#else //RD_SCALABLE_UI_V2
void ASwtScrollableBase::HandleScrollEventL(CEikScrollBar*, TEikScrollEvent)
{
}
#endif //RD_SCALABLE_UI_V2

MSwtScrollable* ASwtScrollableBase::ScrollableInterface()
{
    return this;
}
