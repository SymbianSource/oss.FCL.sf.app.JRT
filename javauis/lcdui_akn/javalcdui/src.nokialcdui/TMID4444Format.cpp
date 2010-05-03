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
* Description:  4 bits for alpha, red, green and blue component in a pixel.
*
*/


//  INCLUDE FILES
#include "TMID4444Format.h"

inline TUint16 ConvertInternalInline4444(const TMIDInternalARGB& aInternal)
{
    return (TUint16)(((aInternal.iA & KFourLeftBits) << KMIDShift8Bits) |
                     ((aInternal.iR & KFourLeftBits) << KMIDShift4Bits) |
                     ((aInternal.iG & KFourLeftBits)) |
                     ((aInternal.iB & KFourLeftBits) >> KMIDShift4Bits));
}

inline void ConvertInline4444(TMIDInternalARGB& aResult, TUint32 aColor, TBool aTransparency)
{
    // set alpha only if bitmap is transparent, otherwise use full opaque
    aResult.iA = aTransparency
                 ? KNativeColors[(aColor & K4444AMax) >> KMIDShift12Bits ]
                 : KAlphaFullOpaque;

    aResult.iR = KNativeColors[(aColor & K4444RMax) >> KMIDShift8Bits ];
    aResult.iG = KNativeColors[(aColor & K4444GMax) >> KMIDShift4Bits ];
    aResult.iB = KNativeColors[(aColor & K4444BMax)];
}

void TMID4444Format::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = (TUint16*)aParameters.iPixels;
}

TUint32 TMID4444Format::ConvertInternal(const TMIDInternalARGB& aInternal)
{
    return ConvertInternalInline4444(aInternal);
}

void TMID4444Format::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    ConvertInline4444(aResult, aColor, iTransparency);
}


void TMID4444Format::GetPixel(TMIDInternalARGB& aResult) const
{
    ConvertInline4444(aResult, iBitmap[ iOffset ], iTransparency);    // CSI: 2 Wrong index means implementation error #
}

TUint8 TMID4444Format::GetAlpha() const
{
    // we need only last 8 bits since it is either white or black
    return (TUint8)(((TUint16*)iAlphaBitmap)[ iOffset ] & KMIDEightRightBits);         // CSI: 2 Wrong index means implementation error #
}

void TMID4444Format::PlotPixel(const TMIDInternalARGB& aInternal)
{
    iBitmap[ iOffset ] = ConvertInternalInline4444(aInternal);    // CSI: 2 Wrong index means implementation error #
}


TBool TMID4444Format::CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset)
{
    // returns is there enough space
    return ((aLastDrawnPixelOffset * KBytesPerPixel4444) <= aPixelSize);
}
