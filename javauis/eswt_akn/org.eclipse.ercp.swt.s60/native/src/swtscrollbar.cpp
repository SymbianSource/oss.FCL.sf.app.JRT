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


#include <eikscrlb.h>
#include <AknUtils.h>
#include "swtscrollbar.h"


// ======== MEMBER FUNCTIONS ========


CSwtScrollBar::CSwtScrollBar(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle,
                             CEikScrollBar& aScrollBar, MSwtScrollable& aScrollable, TInt aMinimum,
                             TInt aMaximum, TInt aPosition, TInt aThumbSpan, TBool aDoubleSpan,
                             CEikScrollBar::TOrientation aOrientation)
        : iDisplay(aDisplay)
        , iPeer(aPeer)
        , iStyle(aStyle)
        , iScrollBar(aScrollBar)
        , iScrollable(aScrollable)
        , iMinimum(aMinimum)
        , iMaximum(aMaximum)
        , iThumbPosition(aPosition)
        , iThumbSpan(aThumbSpan)
        , iDoubleSpan(aDoubleSpan)
        , iOrientation(aOrientation)
{
}

CSwtScrollBar::~CSwtScrollBar()
{
    iScrollable.ScrollBarDestroyed(this);
}

void CSwtScrollBar::SetModelL(TInt aPosition, TInt aMinimum, TInt aMaximum, TInt aThumbSpan)
{
    TEikScrollBarModel model;
    model.iScrollSpan    = aMaximum - aMinimum;
    model.iThumbSpan     = Min(model.iScrollSpan, aThumbSpan);
    model.iThumbPosition = Min(model.iScrollSpan - aThumbSpan, Max(0, aPosition - aMinimum));
    // Cannot use non double span models for double span scrollbars! Avoiding panics.
    if (iDoubleSpan)
    {
        TAknDoubleSpanScrollBarModel dsModel(model);
        iScrollBar.SetModelL(&dsModel);
    }
    else
    {
        iScrollBar.SetModelL(&model);
    }
}

TSwtPeer CSwtScrollBar::Dispose()
{
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

TSize CSwtScrollBar::GetSize() const
{
    TSize scrollBarSize = iScrollBar.Size();

    // Avkon returns negative value, if scrollbar is not initialized
    if (scrollBarSize.iHeight < 0)
    {
        scrollBarSize.iHeight = 0;
    }
    if (scrollBarSize.iWidth < 0)
    {
        scrollBarSize.iWidth = 0;
    }

    // If the size is not initialized, we can at least return correct
    // scrollbar breadth
    if (scrollBarSize == TSize(0, 0))
    {
        if (iOrientation == CEikScrollBar::EHorizontal)
        {
            scrollBarSize.SetSize(0, iDisplay.UiUtils().ScrollBarBreadth(&iScrollBar));
        }
        if (iOrientation == CEikScrollBar::EVertical)
        {
            scrollBarSize.SetSize(iDisplay.UiUtils().ScrollBarBreadth(&iScrollBar), 0);
        }
    }

    return scrollBarSize;
}

void CSwtScrollBar::SetEnabled(TBool aEnabled)
{
    iScrollBar.SetFocusing(aEnabled);
    iScrollBar.SetDimmed(!aEnabled);
}

void CSwtScrollBar::SetVisible(TBool aVisible)
{
    iScrollBar.MakeVisible(aVisible);
}

TInt CSwtScrollBar::GetThumbPosition() const
{
    TInt thumbPos = iScrollBar.ThumbPosition();

    if ((iStyle & KSwtStyleHScroll) && AknLayoutUtils::LayoutMirrored())
    {
        TInt thumbSpan(0);
        TInt scrollSpan(0);
        if (iDoubleSpan)
        {
            const TAknDoubleSpanScrollBarModel* dsModel =
                static_cast<const TAknDoubleSpanScrollBarModel*>(iScrollBar.Model());
            thumbSpan = dsModel->WindowSize();
            scrollSpan = dsModel->ScrollSpan();
        }
        else
        {
            thumbSpan = iScrollBar.Model()->iThumbSpan;
            scrollSpan = iScrollBar.Model()->iScrollSpan;
        }

        // Get thumb span right position
        thumbPos += thumbSpan;
        if (thumbPos > scrollSpan)
        {
            thumbPos = scrollSpan;
        }

        // Convert thumb position into left based position
        thumbPos = scrollSpan - thumbPos;
    }

    return thumbPos + iMinimum;
}

void CSwtScrollBar::SetThumbPositionL(TInt aPosition)
{
    MSwtComposite* comp = iScrollable.Control()->CompositeInterface();
    if (comp && comp->ScrolledCompositePysicsAction() != KSwtPhysicsNoAction)
    {
        return;
    }
    DoSetThumbPositionL(aPosition);
}

void CSwtScrollBar::DoSetThumbPositionL(TInt aPosition)
{
    iThumbPosition = Min(iMaximum, Max(iMinimum, aPosition));
    TInt thumbPos = iThumbPosition;

    if ((iStyle & KSwtStyleHScroll) && AknLayoutUtils::LayoutMirrored())
    {
        // Convert thumb position into right based position
        thumbPos = iMaximum - thumbPos;

        // Get thumb span left position
        TInt thumbSpan(0);
        if (iDoubleSpan)
        {
            const TAknDoubleSpanScrollBarModel* dsModel =
                static_cast<const TAknDoubleSpanScrollBarModel*>(iScrollBar.Model());
            thumbSpan = dsModel->WindowSize();
        }
        else
        {
            thumbSpan = iScrollBar.Model()->iThumbSpan;
        }

        thumbPos = Max(iMinimum, thumbPos - thumbSpan);
    }

    SetModelL(thumbPos, iMinimum, iMaximum, iThumbSpan);
}

TInt CSwtScrollBar::GetThumbSpan() const
{
    if (iDoubleSpan)
    {
        const TAknDoubleSpanScrollBarModel* dsModel =
            static_cast<const TAknDoubleSpanScrollBarModel*>(iScrollBar.Model());
        return dsModel->WindowSize();
    }
    else
    {
        return iScrollBar.Model()->iThumbSpan;
    }
}

void CSwtScrollBar::SetThumbSpanL(TInt aThumbSpan)
{
    ASSERT(aThumbSpan>=1);

    iThumbSpan = Min(aThumbSpan, (iMaximum - iMinimum));

    if (iThumbPosition > (iMaximum - iThumbSpan))
    {
        iThumbPosition = iMaximum - iThumbSpan;
    }

    SetModelL(iThumbPosition, iMinimum, iMaximum, iThumbSpan);
}

TInt CSwtScrollBar::GetMinimum() const
{
    return iMinimum;
}

void CSwtScrollBar::SetMinimumL(TInt aMinimum)
{
    ASSERT(aMinimum>=0);

    if (aMinimum < iMaximum)
    {
        iMinimum = aMinimum;

        if (iThumbSpan > (iMaximum - iMinimum))
        {
            iThumbSpan = (iMaximum - iMinimum);
        }

        if (iThumbPosition < iMinimum)
        {
            iThumbPosition = iMinimum;
        }

        SetModelL(iThumbPosition, iMinimum, iMaximum, iThumbSpan);
    }
}

TInt CSwtScrollBar::GetMaximum() const
{
    TInt scrollSpan(0);
    if (iDoubleSpan)
    {
        const TAknDoubleSpanScrollBarModel* dsModel =
            static_cast<const TAknDoubleSpanScrollBarModel*>(iScrollBar.Model());
        scrollSpan = dsModel->ScrollSpan();
    }
    else
    {
        scrollSpan = iScrollBar.Model()->iScrollSpan;
    }

    if (scrollSpan < 0)
    {
        return iMaximum;
    }
    else
    {
        return iMinimum + scrollSpan;
    }
}

void CSwtScrollBar::SetMaximumL(TInt aMaximum)
{
    ASSERT(aMaximum>=0);

    if (aMaximum > iMinimum)
    {
        iMaximum = aMaximum;

        if (iThumbSpan > (iMaximum - iMinimum))
        {
            iThumbSpan = (iMaximum - iMinimum);
        }

        if ((iThumbPosition + iThumbSpan) > iMaximum)
        {
            iThumbPosition = iMaximum - iThumbSpan;
        }

        SetModelL(iThumbPosition, iMinimum, iMaximum, iThumbSpan);
    }
}

TInt CSwtScrollBar::GetIncrement() const
{
    return iIncrement;
}

void CSwtScrollBar::SetIncrement(TInt aIncrement)
{
    ASSERT(aIncrement>=1);
    iIncrement = aIncrement;
}

TInt CSwtScrollBar::GetPageIncrement() const
{
    return iPageIncrement;
}

void CSwtScrollBar::SetPageIncrement(TInt aPageIncrement)
{
    ASSERT(aPageIncrement>=1);
    iPageIncrement = aPageIncrement;
}

void CSwtScrollBar::SetValuesL(TInt aPosition, TInt aMinimum, TInt aMaximum,
                               TInt aThumbSpan, TInt aIncrement, TInt aPageIncrement)
{
    iIncrement      = aIncrement;
    iPageIncrement  = aPageIncrement;
    iThumbPosition  = aPosition;
    iMinimum        = aMinimum;
    iMaximum        = aMaximum;
    iThumbSpan      = aThumbSpan;

    TInt thumbPos = iThumbPosition;
    if ((iStyle & KSwtStyleHScroll) && AknLayoutUtils::LayoutMirrored())
    {
        // convert thumb position into right based position
        thumbPos = iMaximum - thumbPos;
        // get thumb span left position
        thumbPos = Max(iMinimum, thumbPos - iThumbSpan);
    }

    SetModelL(thumbPos, aMinimum, aMaximum, aThumbSpan);
}

#ifdef RD_SCALABLE_UI_V2
// From MEikScrollBarObserver
void CSwtScrollBar::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    if (aScrollBar != &iScrollBar)
    {
        ASSERT(EFalse); // debug only
        return;
    }

    TInt detail;
    switch (aEventType)
    {
    case EEikScrollLeft:
    case EEikScrollUp:
        detail = ESwtKeyArrowUp;
        break;
    case EEikScrollRight:
    case EEikScrollDown:
        detail = ESwtKeyArrowDown;
        break;
    case EEikScrollPageLeft:
    case EEikScrollPageUp:
        detail = ESwtKeyPageUp;
        break;
    case EEikScrollPageRight:
    case EEikScrollPageDown:
        detail = ESwtKeyPageDown;
        break;
    case EEikScrollHome:
    case EEikScrollTop:
        detail = ESwtKeyHome;
        break;
    case EEikScrollEnd:
    case EEikScrollBottom:
        detail = ESwtKeyEnd;
        break;
    case EEikScrollThumbDragHoriz:
    case EEikScrollThumbDragVert:
        detail = KSwtDrag;
        break;
    case EEikScrollThumbReleaseHoriz:
    case EEikScrollThumbReleaseVert:
        detail = 0;
        break;
    default:
        return;
    }
    iDisplay.PostScrollEventL(iPeer, detail);
}
#endif // RD_SCALABLE_UI_V2
