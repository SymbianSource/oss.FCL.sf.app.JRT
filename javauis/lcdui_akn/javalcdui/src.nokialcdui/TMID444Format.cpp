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
* Description:  4 bits for red, green and blue component in a pixel.
*
*/


//  INCLUDE FILES
#include "TMID444Format.h"


inline TUint16 ConvertInternalInline444(const TMIDInternalARGB& aInternal)
{
    // no transparency, so alpha is 0
    return (TUint16)(((aInternal.iR & KFourLeftBits) << KMIDShift4Bits) +
                     (aInternal.iG & KFourLeftBits) +
                     ((aInternal.iB & KFourLeftBits) >> KMIDShift4Bits));
}

inline void ConvertInline444(TMIDInternalARGB& aResult, TUint32 aColor)
{
    aResult.iA = KAlphaFullOpaque;
    aResult.iR = KNativeColors[(aColor & K444RMax) >> KMIDShift8Bits ];
    aResult.iG = KNativeColors[(aColor & K444GMax) >> KMIDShift4Bits ];
    aResult.iB = KNativeColors[(aColor & K444BMax)];
}

void TMID444Format::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = (TUint16*)aParameters.iPixels;
}

TUint32 TMID444Format::ConvertInternal(const TMIDInternalARGB& aInternal)
{
    return ConvertInternalInline444(aInternal);
}

void TMID444Format::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    ConvertInline444(aResult, aColor);
}

void TMID444Format::GetPixel(TMIDInternalARGB& aResult) const
{
    ConvertInline444(aResult, iBitmap[ iOffset ]);    // CSI: 2 Wrong index means implementation error #
}

TUint8 TMID444Format::GetAlpha() const
{
    // we need only last 8 bits since it is either white or black
    return (TUint8)(((TUint16*)iAlphaBitmap)[ iOffset ] & KMIDEightRightBits);   // CSI: 2 Wrong index means implementation error #
}

void TMID444Format::PlotPixel(const TMIDInternalARGB& aInternal)
{
    iBitmap[ iOffset ] = ConvertInternalInline444(aInternal);    // CSI: 2 Wrong index means implementation error #
}


TBool TMID444Format::CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset)
{
    return((aLastDrawnPixelOffset * KBytesInPixel444) <= aPixelSize);
}

