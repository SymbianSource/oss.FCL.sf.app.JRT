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
* Description:  5 bits for red, 6 for green and 5 for blue component in a pixel.
*
*/



//  INCLUDE FILES
#include "TMID565Format.h"

inline TUint16 ConvertInternalInline565(const TMIDInternalARGB& aInternal)
{
    return (TUint16)(((aInternal.iR << KMIDShift8Bits) & K565RMax) +
                     ((aInternal.iG << KMIDShift3Bits) & K565GMax) +
                     ((aInternal.iB >> KMIDShift3Bits) & K565BMax));
}

inline void ConvertInline565(TMIDInternalARGB& aResult, TUint32 aColor)
{
    aResult.iA = KAlphaFullOpaque;
    aResult.iR = (TUint8)((aColor & K565RMax) >> KMIDShift8Bits);
    aResult.iG = (TUint8)((aColor & K565GMax) >> KMIDShift3Bits);
    aResult.iB = (TUint8)((aColor & K565BMax) << KMIDShift3Bits);
}

void TMID565Format::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = (TUint16*)aParameters.iPixels;
}

TUint32 TMID565Format::ConvertInternal(const TMIDInternalARGB& aInternal)
{
    return ConvertInternalInline565(aInternal);
}

void TMID565Format::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    ConvertInline565(aResult, aColor);
}

void TMID565Format::GetPixel(TMIDInternalARGB& aResult) const
{
    ConvertInline565(aResult, iBitmap[ iOffset ]);    // CSI: 2 Wrong index means implementation error #
}

TUint8 TMID565Format::GetAlpha() const
{
    // we need only last 8 bits since it is either white or black
    return (TUint8)(((TUint16*)iAlphaBitmap)[ iOffset ] & KMIDEightRightBits);          // CSI: 2 Wrong index means implementation error #
}

void TMID565Format::PlotPixel(const TMIDInternalARGB& aInternal)
{
    iBitmap[ iOffset ] = ConvertInternalInline565(aInternal);    // CSI: 2 Wrong index means implementation error #
}


TBool TMID565Format::CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset)
{
    return((aLastDrawnPixelOffset * KBytesPerPixel565) <= aPixelSize);
}
