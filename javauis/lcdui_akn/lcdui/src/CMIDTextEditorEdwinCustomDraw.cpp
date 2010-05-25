
/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines custom drawing for text editor edwin.
*
*/


// INTERNAL INCLUDES
#include "CMIDTextEditorEdwinCustomDraw.h"

// EXTERNAL INCLUDES
#include <j2me/jdebug.h>

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::CMIDTextEditorEdwinCustomDraw
// ---------------------------------------------------------------------------
//
CMIDTextEditorEdwinCustomDraw::CMIDTextEditorEdwinCustomDraw(
    const MLafEnv& aLafEnv,
    const MFormCustomDraw& aParentDraw,
    const CMIDTextEditorEdwin& aEdwin) :
    CLafEdwinCustomDrawBase(aLafEnv, aEdwin),
    iEdwin(aEdwin), iParentDraw(aParentDraw)
{
    DEBUG("CMIDTextEditorEdwinCustomDraw::CMIDTextEditorEdwinCustomDraw");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::~CMIDTextEditorEdwinCustomDraw
// ---------------------------------------------------------------------------
//
CMIDTextEditorEdwinCustomDraw::~CMIDTextEditorEdwinCustomDraw()
{
    DEBUG("CMIDTextEditorEdwinCustomDraw::~CMIDTextEditorEdwinCustomDraw");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::DrawBackground
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwinCustomDraw::DrawBackground(
    const TParam& aParam,
    const TRgb& aBackground,
    TRect& aDrawn) const
{
    // Background drawing is not done by custom draw.

    // Check if CCoeControl::Draw() has been invoked for the editor window.
    // This guarantees that draw is not invoked all the time. Redraw
    // removes the issue with background transparency because if FEP tries
    // to draw the text without the CONE control framework, the editor gets
    // redrawn which guarantees that the text is not messed up due to
    // the editor's transparency.

    if (iEdwin.IsTransparent())
    {
        iEdwin.Redraw();
    }
    else
    {
        // If scaling is on, it clips drawing to canvas size. After it
        // draws its background.
        // Only drawing otherwise.
        if (iEdwin.IsScalingOn())
        {
            aParam.iGc.SetClippingRect(iEdwin.GetOnScreenCanvasRect());

            iParentDraw.DrawBackground(aParam, aBackground, aDrawn);

            aParam.iGc.CancelClippingRect();
        }
        else
        {
            iParentDraw.DrawBackground(aParam, aBackground, aDrawn);
        }
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::DrawLineGraphics
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwinCustomDraw::DrawLineGraphics(
    const TParam& aParam,
    const TLineInfo& aLineInfo) const
{
    // If scaling is on, it clips drawing to canvas size. After it
    // draws its lines.
    // Only drawing otherwise.
    if (iEdwin.IsScalingOn())
    {
        aParam.iGc.SetClippingRect(iEdwin.GetOnScreenCanvasRect());

        iParentDraw.DrawLineGraphics(aParam, aLineInfo);

        aParam.iGc.CancelClippingRect();
    }
    else
    {
        iParentDraw.DrawLineGraphics(aParam, aLineInfo);
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::DrawText
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwinCustomDraw::DrawText(
    const TParam& aParam,
    const TLineInfo& aLineInfo,
    const TCharFormat& aFormat,
    const TDesC& aText,
    const TPoint& aTextOrigin,
    TInt aExtraPixels) const
{
    // If transparency is enabled, check that if draw has not been invoked
    // yet and issue redraw in that case. This removes the issue with
    // transparency so that the old content does not cause corruption to
    // the new text drawn on top of the old content. Otherwise it is ok
    // to draw the text because opaque background draws on top of the old
    // content in the editor.

    if (iEdwin.IsTransparent() && !iEdwin.DrawOngoing())
    {
        iEdwin.Redraw();
        return;
    }

    // If scaling is on, it clips drawing to canvas size. After it
    // draws its text.
    // Only drawing otherwise.
    if (iEdwin.IsScalingOn())
    {
        aParam.iGc.SetClippingRect(iEdwin.GetOnScreenCanvasRect());

        iParentDraw.DrawText(
            aParam,
            aLineInfo,
            aFormat,
            aText,
            aTextOrigin,
            aExtraPixels);

        aParam.iGc.CancelClippingRect();
    }
    else
    {
        iParentDraw.DrawText(
            aParam,
            aLineInfo,
            aFormat,
            aText,
            aTextOrigin,
            aExtraPixels);
    }
}

#ifdef RD_JAVA_S60_RELEASE_9_2
// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::DrawText
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwinCustomDraw::DrawText(
    const TParam& aParam,
    const TLineInfo& aLineInfo,
    const TCharFormat& aFormat,
    const TDesC& aText,
    const TInt aStart,
    const TInt aEnd,
    const TPoint& aTextOrigin,
    TInt aExtraPixels) const
{
    // If transparency is enabled, check that if draw has not been invoked
    // yet and issue redraw in that case. This removes the issue with
    // transparency so that the old content does not cause corruption to
    // the new text drawn on top of the old content. Otherwise it is ok
    // to draw the text because opaque background draws on top of the old
    // content in the editor.

    if (iEdwin.IsTransparent() && !iEdwin.DrawOngoing())
    {
        iEdwin.Redraw();
        return;
    }

    // If scaling is on, it clips drawing to canvas size. After it
    // draws its text.
    // Only drawing otherwise.
    if (iEdwin.IsScalingOn())
    {
        aParam.iGc.SetClippingRect(iEdwin.GetOnScreenCanvasRect());

        iParentDraw.DrawText(
            aParam,
            aLineInfo,
            aFormat,
            aText,
            aStart,
            aEnd,
            aTextOrigin,
            aExtraPixels);

        aParam.iGc.CancelClippingRect();
    }
    else
    {
        iParentDraw.DrawText(
            aParam,
            aLineInfo,
            aFormat,
            aText,
            aStart,
            aEnd,
            aTextOrigin,
            aExtraPixels);
    }
}
#endif

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwinCustomDraw::SystemColor
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TRgb CMIDTextEditorEdwinCustomDraw::SystemColor(
    TUint aColorIndex,
    TRgb aDefaultColor) const
{
    TRgb ret;

    // Return custom highlight color if retrieved by the system.
    if (aColorIndex == TLogicalRgb::ESystemSelectionBackgroundIndex)
    {
        iEdwin.GetColor(EColorControlHighlightBackground, ret);
    }
    else
    {
        // Other colors use parent's SystemColor.
        ret = iParentDraw.SystemColor(aColorIndex, aDefaultColor);
    }

    return ret;
}

// End of file
