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

#include <eikedwin.h>
#include <lafmain.h>
#include "swtedwincustomdrawer.h"

// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::CSwtEdwinCustomDrawer
// -----------------------------------------------------------------------------
//
CSwtEdwinCustomDrawer::CSwtEdwinCustomDrawer(
    const MLafEnv& aLafEnv,
    const MFormCustomDraw* aParentDraw,
    const CEikEdwin& aEdwin) :
        CLafEdwinCustomDrawBase(aLafEnv, aEdwin),
        iEdwin(aEdwin), iEdWinDrawer(aParentDraw)
{
    ASSERT(iEdWinDrawer);
}

// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::~CSwtEdwinCustomDrawer
// -----------------------------------------------------------------------------
//
CSwtEdwinCustomDrawer::~CSwtEdwinCustomDrawer()
{
}

// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::SetClippingRect
// -----------------------------------------------------------------------------
//
void CSwtEdwinCustomDrawer::SetClippingRect(const TRect& aRect)
{
    iClippingRect = aRect;
}


// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::DrawBackground
// From CLafEdwinCustomDrawBase
// -----------------------------------------------------------------------------
//
void CSwtEdwinCustomDrawer::DrawBackground(
    const TParam& aParam,
    const TRgb& aBackground,
    TRect& aDrawn) const
{
    if (!iEdwin.IsVisible())
    {
        return;
    }
    aParam.iGc.SetClippingRect(iClippingRect);
    iEdWinDrawer->DrawBackground(aParam, aBackground, aDrawn);
}

// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::DrawLineGraphics
// From CLafEdwinCustomDrawBase
// -----------------------------------------------------------------------------
//
void CSwtEdwinCustomDrawer::DrawLineGraphics(
    const TParam& aParam,
    const TLineInfo& aLineInfo) const
{
    if (!iEdwin.IsVisible())
    {
        return;
    }
    aParam.iGc.SetClippingRect(iClippingRect);
    iEdWinDrawer->DrawLineGraphics(aParam, aLineInfo);
}

// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::DrawText
// From CLafEdwinCustomDrawBase
// -----------------------------------------------------------------------------
//
void CSwtEdwinCustomDrawer::DrawText(
    const TParam& aParam,
    const TLineInfo& aLineInfo,
    const TCharFormat& aFormat,
    const TDesC& aText,
    const TPoint& aTextOrigin,
    TInt aExtraPixels) const
{
    if (!iEdwin.IsVisible())
    {
        return;
    }
    aParam.iGc.SetClippingRect(iClippingRect);
    iEdWinDrawer->DrawText(
        aParam,
        aLineInfo,
        aFormat,
        aText,
        aTextOrigin,
        aExtraPixels);
}

// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::DrawText
// From CLafEdwinCustomDrawBase
// -----------------------------------------------------------------------------
//
#ifdef RD_JAVA_S60_RELEASE_9_2
void CSwtEdwinCustomDrawer::DrawText(
    const TParam& aParam,
    const TLineInfo& aLineInfo,
    const TCharFormat& aFormat,
    const TDesC& aText,
    const TInt aStart,
    const TInt aEnd,
    const TPoint& aTextOrigin,
    TInt aExtraPixels) const
{
    if (!iEdwin.IsVisible())
    {
        return;
    }
    aParam.iGc.SetClippingRect(iClippingRect);
    iEdWinDrawer->DrawText(
        aParam,
        aLineInfo,
        aFormat,
        aText,
        aStart,
        aEnd,
        aTextOrigin,
        aExtraPixels);
}
#endif

// -----------------------------------------------------------------------------
// CSwtEdwinCustomDrawer::SystemColor
// From CLafEdwinCustomDrawBase
// -----------------------------------------------------------------------------
//
TRgb CSwtEdwinCustomDrawer::SystemColor(
    TUint aColorIndex,
    TRgb aDefaultColor) const
{
    return iEdWinDrawer->SystemColor(aColorIndex, aDefaultColor);
}
