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


#include <swtlaffacade.h>
#include "swttasktip.h"


const TInt KVariationFullScreen = 3;
const TInt KVariationWindow = 0;
const TInt KVariationWindowBarOrTextOnly = 1;
const TInt KVariationBar = 0;
const TInt KVariationBarOnly = 1;
const TInt KVariationLabel = 0;
const TInt KVariationLabelOnly = 1;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtTaskTip::NewL
// ---------------------------------------------------------------------------
//
CSwtTaskTip* CSwtTaskTip::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle)
{
    CSwtTaskTip* self = new(ELeave) CSwtTaskTip(aDisplay, aPeer, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::Dispose
// From MSwtTaskTip
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTaskTip::Dispose()
{
    TSwtPeer peer(JavaPeer());
    delete this;
    return peer;
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::JavaPeer
// From MSwtTaskTip
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTaskTip::JavaPeer() const
{
    return iPeer;
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::GetBarDefaultBounds
// From MSwtTaskTip
// ---------------------------------------------------------------------------
//
TRect CSwtTaskTip::GetBarDefaultBounds(TBool aHasText) const
{
    if (aHasText
            && (((iStyle & KSwtStyleIndeterminate) != 0)
                || ((iStyle & KSwtStyleSmooth) != 0)))
    {
        return iRectBar;
    }
    else
    {
        return iRectBarOnly;
    }
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::GetLabelDefaultBounds
// From MSwtTaskTip
// ---------------------------------------------------------------------------
//
TRect CSwtTaskTip::GetLabelDefaultBounds(TBool aHasText) const
{
    if (aHasText
            && (((iStyle & KSwtStyleIndeterminate) != 0)
                || ((iStyle & KSwtStyleSmooth) != 0)))
    {
        return iRectLabel;
    }
    else
    {
        return iRectLabelOnly;
    }
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::GetShellDefaultBounds
// From MSwtTaskTip
// ---------------------------------------------------------------------------
//
TRect CSwtTaskTip::GetShellDefaultBounds(TBool aHasText) const
{
    if (aHasText
            && (((iStyle & KSwtStyleIndeterminate) != 0)
                || ((iStyle & KSwtStyleSmooth) != 0)))
    {
        return iRectShell;
    }
    else
    {
        return iRectShellBarOrLabelOnly;
    }
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::OfferResourceChangeL
// From MSwtResourceChangeObserver
// ---------------------------------------------------------------------------
//
void CSwtTaskTip::OfferResourceChangeL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        UpdateLayout();
    }
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::CSwtTaskTip
// ---------------------------------------------------------------------------
//
CSwtTaskTip::CSwtTaskTip(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle)
        : iDisplay(aDisplay)
        , iPeer(aPeer)
        , iStyle(aStyle)
{
    iRectShell = TRect(TRect::EUninitialized);
    iRectShellBarOrLabelOnly = TRect(TRect::EUninitialized);
    iRectLabel = TRect(TRect::EUninitialized);
    iRectLabelOnly = TRect(TRect::EUninitialized);
    iRectBar = TRect(TRect::EUninitialized);
    iRectBarOnly = TRect(TRect::EUninitialized);
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::CSwtTaskTip
// ---------------------------------------------------------------------------
//
CSwtTaskTip::~CSwtTaskTip()
{
    iDisplay.RemoveResourceChangeObserver(this);
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtTaskTip::ConstructL()
{
    iDisplay.AddResourceChangeObserverL(this);
    UpdateLayout();
}

// ---------------------------------------------------------------------------
// CSwtTaskTip::UpdateLayout
// ---------------------------------------------------------------------------
//
void CSwtTaskTip::UpdateLayout()
{
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
    TAknLayoutRect layoutMainPane = CSwtLafFacade::GetLayoutRect(
                                        CSwtLafFacade::EMainEswtPane,
                                        screenRect,
                                        KVariationFullScreen);
    TRect mainPaneRect = layoutMainPane.Rect();

    // Shell bounds
    TAknLayoutRect layoutShell = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EPopupEswtTasktipWindow,
                                     mainPaneRect,
                                     KVariationWindow);
    iRectShell = layoutShell.Rect();
    layoutShell = CSwtLafFacade::GetLayoutRect(
                      CSwtLafFacade::EPopupEswtTasktipWindow,
                      mainPaneRect,
                      KVariationWindowBarOrTextOnly);
    iRectShellBarOrLabelOnly = layoutShell.Rect();

    // Label bounds
    TAknLayoutText layoutLabel = CSwtLafFacade::GetLayoutText(
                                     CSwtLafFacade::EPopupEswtTasktipWindowT1,
                                     iRectShell,
                                     KVariationLabel);
    iRectLabel = layoutLabel.TextRect();
    layoutLabel = CSwtLafFacade::GetLayoutText(
                      CSwtLafFacade::EPopupEswtTasktipWindowT1,
                      iRectShellBarOrLabelOnly,
                      KVariationLabelOnly);
    iRectLabelOnly = layoutLabel.TextRect();

    // Progress bar bounds
    if (((iStyle & KSwtStyleIndeterminate) != 0)
            || ((iStyle & KSwtStyleSmooth) != 0))
    {
        TAknLayoutRect layoutBar = CSwtLafFacade::GetLayoutRect(
                                       CSwtLafFacade::EWaitBarPaneCp71,
                                       iRectShell,
                                       KVariationBar);
        iRectBar = layoutBar.Rect();
        layoutBar = CSwtLafFacade::GetLayoutRect(
                        CSwtLafFacade::EWaitBarPaneCp71,
                        iRectShellBarOrLabelOnly,
                        KVariationBarOnly);
        iRectBarOnly = layoutBar.Rect();
    }
}
