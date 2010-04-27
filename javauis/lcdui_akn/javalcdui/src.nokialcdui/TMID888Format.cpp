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
* Description:  8 bits for red, green and blue component in a pixel.
*
*/


//  INCLUDE FILES
#include "TMID888Format.h"


inline TUint32 ConvertInternalInline888(const TMIDInternalARGB& aInternal)
{
    // fully opaque always, alpha is 0
    return ((aInternal.iR << KMIDShift16Bits) & K888RMax) +
           ((aInternal.iG << KMIDShift8Bits) & K888GMax) +
           ((aInternal.iB) & K888BMax);
}

void ConvertInline888(TMIDInternalARGB& aResult, TUint32 aColor)
{
    aResult.iA = KAlphaFullOpaque;
    aResult.iR = (TUint8)((aColor & K888RMax) >> KMIDShift16Bits);
    aResult.iG = (TUint8)((aColor & K888GMax) >> KMIDShift8Bits);
    aResult.iB = (TUint8)(aColor & K888BMax);
}

void TMID888Format::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = aParameters.iPixels;
    iAlphaBitmap = (TUint8*)aParameters.iAlpha;
}

TUint32 TMID888Format::ConvertInternal(const TMIDInternalARGB& aInternal)
{
    return ConvertInternalInline888(aInternal);
}

void TMID888Format::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    ConvertInline888(aResult, aColor);
}

void TMID888Format::GetPixel(TMIDInternalARGB& aResult) const
{
    ConvertInline888(aResult, iBitmap[ iOffset ]);    // CSI: 2 Wrong offset means implementation error #
}

void TMID888Format::PlotPixel(const TMIDInternalARGB& aInternal)
{
    iBitmap[ iOffset ] = ConvertInternalInline888(aInternal);   // CSI: 2 Wrong offset means implementation error #
}

TUint8 TMID888Format::GetAlpha() const
{
    // we need only last 8 bits since it is either white or black
    return (TUint8)(iAlphaBitmap[ iOffset ] & KMIDEightRightBits);  // CSI: 2 Wrong offset means implementation error #
}

TBool TMID888Format::CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset)
{
    return ((aLastDrawnPixelOffset * KBytesPerPixel888) <= aPixelSize);
}
