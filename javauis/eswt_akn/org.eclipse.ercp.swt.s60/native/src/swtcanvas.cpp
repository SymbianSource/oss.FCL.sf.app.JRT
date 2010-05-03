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
#include <AknsUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include "swtcanvas.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtCanvas::NewL
// ---------------------------------------------------------------------------
//
CSwtCanvas* CSwtCanvas::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite& aParent, TInt aStyle)
{
    CCoeControl& parentCtrl = aParent.Control()->CoeControl();

    CSwtCanvas* self = new(ELeave) CSwtCanvas(aDisplay, aPeer, &aParent,
            aStyle, parentCtrl.IsVisible(), parentCtrl.IsDimmed());
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtCanvas::Scroll
// From MSwtCanvas
// ---------------------------------------------------------------------------
//
void CSwtCanvas::Scroll(const TPoint& aDest, const TRect& aRect, TBool aAll)
{
    const TRect clientRect(ClientRect());
    const TRect rectInWin(aRect.iTl+clientRect.iTl, aRect.iBr+clientRect.iTl);
    const TPoint offset(aDest - aRect.iTl);
    if (aAll)
    {
        const TInt count = iChildren.Count();
        for (TInt i = 0; i < count; ++i)
        {
            MSwtControl& child = *iChildren[i];
            if (!child.IsShell())
            {
                CCoeControl& coeChild = child.CoeControl();
                if (coeChild.Rect().Intersects(rectInWin))
                {
                    coeChild.SetPosition(coeChild.Position() + offset);
                }
            }
        }
    }

    Window().Scroll(clientRect, offset, rectInWin);

    // Repaint area of the source which is not covered by the destination
    TRect heightNotCoveredByDest(rectInWin);
    if (offset.iY == 0)
    {
        heightNotCoveredByDest.SetSize(TSize(0, 0));
    }
    else if ((offset.iY > 0) && (rectInWin.Height() - offset.iY > 0))
    {
        heightNotCoveredByDest.SetHeight(offset.iY);
    }
    else if ((offset.iY < 0) && (rectInWin.Height() + offset.iY > 0))
    {
        heightNotCoveredByDest.Move(0, rectInWin.Height() + offset.iY);
        heightNotCoveredByDest.SetHeight(-offset.iY);
    }

    if (!heightNotCoveredByDest.IsEmpty())
    {
        RedrawRegion(heightNotCoveredByDest);
    }

    TRect widthNotCoveredByDest(rectInWin);
    if (offset.iX == 0)
    {
        widthNotCoveredByDest.SetSize(TSize(0, 0));
    }
    else if ((offset.iX > 0) && (rectInWin.Width() - offset.iX > 0))
    {
        widthNotCoveredByDest.SetWidth(offset.iX);
    }
    else if ((offset.iX < 0) && (rectInWin.Width() + offset.iX > 0))
    {
        widthNotCoveredByDest.Move(rectInWin.Width() + offset.iX, 0);
        widthNotCoveredByDest.SetWidth(-offset.iX);
    }

    if (!widthNotCoveredByDest.IsEmpty()
            && widthNotCoveredByDest != heightNotCoveredByDest)
    {
        RedrawRegion(widthNotCoveredByDest);
    }
}

TBool CSwtCanvas::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    // Don't want CSwtComposite's focusability behavior.
    return ASwtScrollableBase::IsFocusable(aReason);
}

// ---------------------------------------------------------------------------
// CSwtCanvas::Scroll
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtCanvas::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                        TEventCode aType)
{
    TBool traversalDoIt = ETrue;
    if (aKeyEvent.iCode == EKeyUpArrow
            || aKeyEvent.iCode == EKeyDownArrow
            || aKeyEvent.iCode == EKeyRightArrow
            || aKeyEvent.iCode == EKeyLeftArrow)
    {
        traversalDoIt = EFalse;
    }
    return ASwtControlBase::HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

