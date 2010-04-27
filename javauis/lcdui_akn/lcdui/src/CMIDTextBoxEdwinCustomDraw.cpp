/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for TextBoxEdwinCustomDraw
*
*/



// ========== INCLUDE FILES ================================

// using CGulIcon API for iIcon
#include <gulicon.h>
// constants from TMifAvkon enumeration used when creating iIcon
#include <avkon.mbg>
#include <avkon.hrh>
// CLafEdwinCustomDrawBase
#include <eikedwin.h>
// AknsUtils::CreateGulIconL
// AknIconUtils::SetSize
// MAknsSkinInstance* skin = AknsUtils::SkinInstance()
#include <AknsUtils.h>
// LAF
// used in GetLayoutRectAndSetLineDelta function
#include <aknlayoutscalable_avkon.cdl.h>    // LAF

#include <data_caging_path_literals.hrh>

#include <s60commonutils.h>

#include "CMIDTextBoxEdwinCustomDraw.h"

// ========== CONSTANTS ====================================

_LIT(KAvkonMbmFileName, "avkon2.mbm");


// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::NewL
// ---------------------------------------------------------
//
CMIDTextBoxEdwinCustomDraw* CMIDTextBoxEdwinCustomDraw::NewL(const MLafEnv& aEnv,
        const MFormCustomDraw* aParentCustomDraw,
        const CEikEdwin* aParentControl)
{
    CMIDTextBoxEdwinCustomDraw* self = new(ELeave) CMIDTextBoxEdwinCustomDraw(aEnv,
            aParentCustomDraw,
            aParentControl);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::ConstructL
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::ConstructL()
{
    // Get resource file name
    TFileName fileName;
    fileName.Append(KDC_APP_BITMAP_DIR);
    fileName.Append(KAvkonMbmFileName);
    fileName = java::util::S60CommonUtils::VerifiedFileNameL(fileName);

    iIcon = AknsUtils::CreateGulIconL(AknsUtils::SkinInstance(),
                                      KAknsIIDQgnGrafLinePrimaryHorizontal,
                                      fileName,
                                      EMbmAvkonQgn_graf_line_primary_horizontal,
                                      EMbmAvkonQgn_graf_line_primary_horizontal_mask);
}


// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::CMIDTextBoxEdwinCustomDraw
// ---------------------------------------------------------
//
CMIDTextBoxEdwinCustomDraw::CMIDTextBoxEdwinCustomDraw(const MLafEnv& aEnv,
        const MFormCustomDraw* aParentCustomDraw,
        const CEikEdwin* aParentControl) :
        CLafEdwinCustomDrawBase(aEnv,*aParentControl),
        iParentControl(aParentControl),
        iParentCustomDraw(aParentCustomDraw)
{
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::~CMIDTextBoxEdwinCustomDraw
// ---------------------------------------------------------
//
CMIDTextBoxEdwinCustomDraw::~CMIDTextBoxEdwinCustomDraw()
{
    if (iRects)
    {
        iRects->ResetAndDestroy();
        delete iRects;
    }

    delete iIcon;
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::CreateRectsIfNeededL
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::CreateRectsIfNeededL()
{
    if ((!iRects && iNumberOfLinesInBody > 0)
            || (iRects && (iNumberOfLinesInBody >= iRects->Count())))
    {
        if (!iRects)
        {
            iRects = new(ELeave) CArrayPtrFlat<TRect>(iNumberOfLinesInBody);
        }

        iRects->SetReserveL(iNumberOfLinesInBody);

        TRect* rect = NULL;
        for (TUint i = iRects->Count(); i < iNumberOfLinesInBody; i++)
        {
            rect = new(ELeave) TRect;
            CleanupStack::PushL(rect);
            iRects->InsertL(i, rect);
            CleanupStack::Pop(rect);
        }
    }
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::GetLayoutRectAndSetLineDelta
// ---------------------------------------------------------
//
TAknLayoutRect CMIDTextBoxEdwinCustomDraw::GetLayoutRectAndSetLineDelta(
    TRect& aRect,
    TUint8& aLineTopOffset)
{
    iLineDelta      = 0;
    aLineTopOffset  = 0;

    TAknWindowLineLayout listSinglePaneLayout =
        AknLayoutScalable_Avkon::list_single_midp_pane(0).LayoutLine();

    // get the top margin for list_single_midp_pane:
    // used for offset-ing the iLineDelta
    aLineTopOffset = listSinglePaneLayout.it;

    TAknLayoutRect listSinglePane;
    listSinglePane.LayoutRect(aRect, listSinglePaneLayout);

    TAknLayoutRect horizLinePaneLayout;
    horizLinePaneLayout.LayoutRect(
        listSinglePane.Rect(),
        AknLayoutScalable_Avkon::list_single_midp_pane_g1().LayoutLine());

    iLineDelta = listSinglePane.Rect().Height();
    ASSERT(iLineDelta > 0);

    return horizLinePaneLayout;
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::CalculateLayout
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::CalculateLayout(TRect aParentRect)
{
    TRect mainPane = aParentRect;

    TUint8 lineTopOffset;
    TAknLayoutRect lineLayout = GetLayoutRectAndSetLineDelta(mainPane,
                                lineTopOffset);

    iNumberOfLinesInBody = (mainPane.Height() - lineTopOffset) / iLineDelta;

    TRAPD(err, CreateRectsIfNeededL());
    if (err != KErrNone)
    {
        return;
    }

    TRect lineRect = lineLayout.Rect();

    TUint i;
    for (i = 0; i < iNumberOfLinesInBody; i++)
    {
        *(iRects->At(i)) = lineRect;
        lineRect.Move(0, iLineDelta);
    }

    AknIconUtils::SetSize(iIcon->Bitmap(),
                          lineLayout.Rect().Size(),
                          EAspectRatioNotPreserved);
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::DrawBackground
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::DrawBackground(const TParam& aParam,
        const TRgb& aRgb,
        TRect& aDrawn) const
{
    iParentCustomDraw->DrawBackground(aParam, aRgb, aDrawn);

    DrawEditorLines(aParam, aDrawn);
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::DrawLineGraphics
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::DrawLineGraphics(const TParam& aParam,
        const TLineInfo& aLineInfo) const
{
    iParentCustomDraw->DrawLineGraphics(aParam, aLineInfo);
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::DrawText
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::DrawText(const TParam& aParam,
        const TLineInfo& aLineInfo,
        const TCharFormat& aFormat,
        const TDesC& aText,
        const TPoint& aTextOrigin,
        TInt aExtraPixels) const
{
    iParentCustomDraw->DrawText(aParam,
                                aLineInfo,
                                aFormat,
                                aText,
                                aTextOrigin,
                                aExtraPixels);
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::SystemColor
// ---------------------------------------------------------
//
TRgb CMIDTextBoxEdwinCustomDraw::SystemColor(TUint aColorIndex, TRgb aDefaultColor) const
{
    return iParentCustomDraw->SystemColor(aColorIndex, aDefaultColor);
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::DrawEditorLines
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::DrawEditorLines(const TParam& aParam, TRect& aDrawn) const
{
    if (!iRects)
    {// may happen only in OOM
        return;
    }

    for (TUint i = 0; i < iNumberOfLinesInBody; i++)
    {
        // Find out of TRect where will line drawn
        TRect currentRect = *(iRects->At(i));
        if (aDrawn.Intersects(currentRect))     // draw only if it is inside the area to be re-drawn
            // Drawing of line
            aParam.iGc.DrawBitmapMasked(currentRect,
                                        iIcon->Bitmap(),
                                        TRect(TPoint(0 ,0), currentRect.Size()),
                                        iIcon->Mask(),
                                        ETrue);
    }
}

// ---------------------------------------------------------
// CMIDTextBoxEdwinCustomDraw::UpdateIconL
// ---------------------------------------------------------
//
void CMIDTextBoxEdwinCustomDraw::UpdateIconL()
{
    // Find of actual skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    // Get resource file name
    TFileName fileName;
    fileName.Append(KDC_APP_BITMAP_DIR);
    fileName.Append(KAvkonMbmFileName);
    fileName = java::util::S60CommonUtils::VerifiedFileNameL(fileName);

    // Update of icon.
    iIcon = AknsUtils::CreateGulIconL(skin,
                                      KAknsIIDQgnGrafLinePrimaryHorizontal,
                                      fileName,
                                      EMbmAvkonQgn_graf_line_primary_horizontal,
                                      EMbmAvkonQgn_graf_line_primary_horizontal_mask);
}

//  End of File
