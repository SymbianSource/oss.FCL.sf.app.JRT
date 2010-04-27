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
* Description:  1 bit format, 2 distinct color values, stored as a byte.
*
*/


//  INCLUDE FILES
#include "TMIDGray1.h"

namespace
{
const TUint8 KWhite = 0;
const TUint8 KBlack = 1;
const TInt KMIDOffSet = 7;
}

void TMIDGray1::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = (TUint8*)aParameters.iPixels;
}

TUint32 TMIDGray1::ConvertInternal(const TMIDInternalARGB& aInternal)
{
    if ((aInternal.iR + aInternal.iG + aInternal.iB)  <
            KWhiteBlackSumRGBMidValue)
    {
        // return 'on' (black)
        return KBlack;
    }
    else
    {
        // return 'off' (white)
        return KWhite;
    }
}

void TMIDGray1::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    aResult.iA = KAlphaFullOpaque;
    if (aColor == KWhite)
    {
        aResult.iR = KAlphaFullOpaque;
        aResult.iG = KAlphaFullOpaque;
        aResult.iB = KAlphaFullOpaque;
    }
    else
    {
        aResult.iR = 0;
        aResult.iG = 0;
        aResult.iB = 0;
    }
}


void TMIDGray1::GetPixel(TMIDInternalARGB& aResult) const
{
    Convert(aResult, (iBitmap[ iOffset >> KGray1Shift ] >>     // CSI: 2 Wrong index means implementation error #
                      (KMIDOffSet - (iOffset % KPixelsInByteGray1))) & 1);
}

TUint8 TMIDGray1::GetAlpha() const
{
    return (TUint8)(((((TUint8*)iAlphaBitmap)[ iOffset >> KGray1Shift ] >>          // CSI: 2 Wrong index means implementation error #
                      (KMIDOffSet - (iOffset % KPixelsInByteGray1))) & 1) *
                    KAlphaFullOpaque);
}

void TMIDGray1::PlotPixel(const TMIDInternalARGB& aInternal)
{
    if (ConvertInternal(aInternal) == KBlack)
    {
        // black
        iBitmap[ iOffset >> KGray1Shift ] = // CSI: 2 Wrong index means implementation error #
            (TUint8)(iBitmap[ iOffset >> KGray1Shift ] |     // CSI: 2 Wrong index means implementation error #
                     KBits[ iOffset % KPixelsInByteGray1 ]);  // CSI: 2 Wrong index means implementation error #
    }
    else
    {
        // white
        iBitmap[ iOffset >> KGray1Shift ] =   // CSI: 2 Wrong index means implementation error #
            (TUint8)(iBitmap[ iOffset >> KGray1Shift ] &     // CSI: 2 Wrong index means implementation error #
                     ~KBits[ iOffset % KPixelsInByteGray1 ]);   // CSI: 2 Wrong index means implementation error #
    }
}

void TMIDGray1::PlotPixelWithAlpha(const TMIDInternalARGB& aInternal)
{
    PlotPixel(aInternal);

    // modifying alpha channel
    if (iAlphaBitmap)
    {
        if (iAlphaMode == EGray256)
        {
            ((TUint8*)iAlphaBitmap)[ iOffset ] = aInternal.iA;     // CSI: 2 Wrong index means implementation error #
        }
        else
        {
            // otherwise we have same format as normal bitmap
            // getting real bitmap from converter
            TUint32* tempBitmap = Bitmap();
            SetBitmap(iAlphaBitmap);

            // It must be either white or black
            // NOTE: This is just opposite way than
            // normal alpha since 0 (black) is 'on'
            // and 1 (white) is 'off'
            TMIDInternalARGB alphaIn;
            if (aInternal.iA != KAlphaFullOpaque)
            {
                alphaIn = KOpaque;
            }
            PlotPixel(alphaIn);

            // reverting to original bitmap
            SetBitmap(tempBitmap);
        }
    }
}

TBool TMIDGray1::CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset)
{
    if (aLastDrawnPixelOffset <= aPixelSize * KPixelsInByteGray1)
    {
        return ETrue; // there's enough space
    }
    return EFalse; // there's not enough space
}

