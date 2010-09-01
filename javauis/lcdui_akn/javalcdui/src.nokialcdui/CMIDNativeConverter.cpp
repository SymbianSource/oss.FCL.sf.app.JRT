/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper for converter used for drawing to Canvas or Image.
*
*/


//  INCLUDE FILES
#include "CMIDNativeConverter.h"
#include "CMIDConvertFactory.h"

const TInt KAlphaShift = 8;

CMIDNativeConverter::CMIDNativeConverter()
{
    iDefaultConverter = TMID8888Format();
    iAlphaMode = ENone;
}

CMIDNativeConverter::~CMIDNativeConverter()
{
    delete iNativeFormat;
}

CMIDNativeConverter* CMIDNativeConverter::NewL(const TMIDBitmapParameters& aParameters)
{
    CMIDNativeConverter* self = new(ELeave)CMIDNativeConverter;
    CleanupStack::PushL(self);
    self->ConstructL(aParameters);
    CleanupStack::Pop(self);
    return self;
}

void CMIDNativeConverter::ConstructL(const TMIDBitmapParameters& aParameters)
{
    iAlphaBitmap = aParameters.iAlpha;
    iAlphaMode = aParameters.iAlphaMode;
    iScanlength = aParameters.iScanLength;

    iNativeFormat = CMIDConvertFactory::NewConverterL(aParameters.iFormat);
    iNativeFormat->InitializeL(aParameters);
}

void CMIDNativeConverter::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    iDefaultConverter.Convert(aResult, aColor);
}

void CMIDNativeConverter::GetPixel(TMIDInternalARGB& aResult) const
{
    if (iNativeFormat->IsAlpha())
    {
        iNativeFormat->GetPixelWithAlpha(aResult);
    }
    iNativeFormat->GetPixel(aResult);
}

void CMIDNativeConverter::PlotPixel(const TMIDInternalARGB& aInternal)
{
    iNativeFormat->PlotPixel(aInternal);
}

void CMIDNativeConverter::PlotPixelWithAlpha(const TMIDInternalARGB& aInternal)
{
    if (aInternal.iA == KAlphaFullTransparent)
    {
        // totally transparent, no need to do anything
        return;
    }

    if (aInternal.iA == KAlphaFullOpaque)
    {
        iNativeFormat->PlotPixel(aInternal);
    }
    else
    {
        // calculating partial transparency
        TMIDInternalARGB alphaPixel = aInternal;
        TMIDInternalARGB belowPixel;
        iNativeFormat->GetPixel(belowPixel);
        CalculateAlpha(alphaPixel, belowPixel);
        iNativeFormat->PlotPixel(alphaPixel);
    }
}

void CMIDNativeConverter::PlotPixelWithAlphaBitmap(const TMIDInternalARGB& aInternal)
{
    if (aInternal.iA == KAlphaFullTransparent)
    {
        // totally transparent, no need to do anything
        return;
    }
    if (aInternal.iA == KAlphaFullOpaque)
    {
        iNativeFormat->PlotPixelWithAlpha(aInternal);
    }
    else
    {
        // calculating partial transparency
        TMIDInternalARGB alphaPixel = aInternal;
        TMIDInternalARGB belowPixel;
        iNativeFormat->GetPixel(belowPixel);
        CalculateAlpha(alphaPixel, belowPixel);
        iNativeFormat->PlotPixelWithAlpha(alphaPixel);
    }
}

void CMIDNativeConverter::SetBitmaps(TUint32* aBitmap)
{
    iNativeFormat->SetBitmap(aBitmap);
}

void CMIDNativeConverter::SetDrawRect(TRect aDrawRect)
{
    iDrawRect = aDrawRect;
    iPosition = aDrawRect.iTl;
    iNativeFormat->iOffset = iDrawRect.iTl.iX + iDrawRect.iTl.iY * iScanlength;
}

void CMIDNativeConverter::PlotPixel(const TPoint& aPos,
                                    const TMIDInternalARGB& aInternal, const TRect& aClip)
{
    // is in clip rect
    if (aClip.Contains(aPos))
    {
        iNativeFormat->iOffset = aPos.iX + aPos.iY * iScanlength;
        PlotPixelWithAlphaBitmap(aInternal);
    }
}

void CMIDNativeConverter::DrawScanLine(TPoint aPos, TInt aLength,
                                       const TMIDInternalARGB& aInternal, const TRect& aClip)
{

    if ((aPos.iY < aClip.iTl.iY) ||
            (aPos.iY >= aClip.iBr.iY)||
            (aPos.iX > aClip.iBr.iX))
    {
        return;
    }
    if (aPos.iX > aClip.iBr.iX)
    {
        return;
    }
    if (aPos.iX < aClip.iTl.iX)
    {
        aLength -= aClip.iTl.iX - aPos.iX;
        aPos.iX = aClip.iTl.iX;
    }
    if (aPos.iX + aLength > aClip.iBr.iX)
    {
        aLength = aClip.iBr.iX - aPos.iX;
    }

    iNativeFormat->iOffset = aPos.iX + aPos.iY * iScanlength;
    for (TInt i = 0; i < aLength; i++)
    {
        PlotPixelWithAlphaBitmap(aInternal);
        ++(iNativeFormat->iOffset);
    }
}

void CMIDNativeConverter::CalculateAlpha(TMIDInternalARGB& aTargetColor,
        const TMIDInternalARGB& aBelowColor)
{
    TInt alphaBelow(KAlphaFullOpaque - aTargetColor.iA + 1);
    TInt alpha(aTargetColor.iA);

    if (aBelowColor.iA + aTargetColor.iA > KAlphaFullOpaque)
    {
        aTargetColor.iA = KAlphaFullOpaque;
    }
    else
    {
        aTargetColor.iA = (TUint8)(aBelowColor.iA + aTargetColor.iA);
    }

    aTargetColor.iR = (TUint8)((aBelowColor.iR * alphaBelow +
                                aTargetColor.iR * alpha) >> KAlphaShift);

    aTargetColor.iG = (TUint8)((aBelowColor.iG * alphaBelow +
                                aTargetColor.iG * alpha) >> KAlphaShift);

    aTargetColor.iB = (TUint8)((aBelowColor.iB * alphaBelow +
                                aTargetColor.iB * alpha) >> KAlphaShift);

}

TBool CMIDNativeConverter::HasAlphaBitmap()
{
    return iNativeFormat->IsAlpha();
}
