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
* Description:   8 bits for alpha, red, green and blue component in a pixel.
*
*/


//  INCLUDE FILES
#include "TMID8888Format.h"

// CONSTANTS
const TInt KMIDAlphaShift = 24;
const TInt KMIDRedShift   = 16;
const TInt KMIDGreenShift =  8;

inline TUint32 ConvertInternalInline8888(const TMIDInternalARGB& aInternal)
{
    return ((aInternal.iA << KMIDAlphaShift) & K8888AMax) |
           ((aInternal.iR << KMIDRedShift) & K8888RMax) |
           ((aInternal.iG << KMIDGreenShift) & K8888GMax) |
           ((aInternal.iB) & K8888BMax);
}

inline void ConvertInline8888(TMIDInternalARGB& aResult, TUint32 aColor, TBool aTransparency)
{
    // set alpha only if a bitmap is transparent, otherwise use full opaque
    aResult.iA = aTransparency
                 ? (TUint8)((aColor & K8888AMax) >> KMIDAlphaShift)
                 : KAlphaFullOpaque;

    aResult.iR = (TUint8)((aColor & K8888RMax) >> KMIDRedShift);
    aResult.iG = (TUint8)((aColor & K8888GMax) >> KMIDGreenShift);
    aResult.iB = (TUint8)(aColor & K8888BMax);
}

void TMID8888Format::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    __ASSERT_DEBUG(!iAlphaBitmap, User::Invariant());
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = aParameters.iPixels;
}

TUint32 TMID8888Format::ConvertInternal(const TMIDInternalARGB& aInternal)
{
    return ConvertInternalInline8888(aInternal);
}

void TMID8888Format::Convert(TMIDInternalARGB& aResult, TUint32 aColor) const
{
    ConvertInline8888(aResult, aColor, iTransparency);
}

void TMID8888Format::GetPixel(TMIDInternalARGB& aResult) const
{
    ConvertInline8888(aResult, iBitmap[ iOffset ], iTransparency);    // CSI: 2 Wrong index means implementation error #
}

void TMID8888Format::PlotPixel(const TMIDInternalARGB& aInternal)
{
    iBitmap[ iOffset ] = ConvertInternalInline8888(aInternal);    // CSI: 2 Wrong index means implementation error #
}

TUint8 TMID8888Format::GetAlpha() const
{
    // we need only last 8 bits since it is either white or black
    __ASSERT_DEBUG(!iAlphaBitmap, User::Invariant());
    return (TUint8)(iBitmap[ iOffset ] >> KMIDAlphaShift);    // CSI: 2 Wrong index means implementation error #
}

void TMID8888Format::GetPixelWithAlpha(TMIDInternalARGB& aResult) const
{
    __ASSERT_DEBUG(!iAlphaBitmap, User::Invariant());
    GetPixel(aResult);
}

TBool TMID8888Format::CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset)
{
    return ((aLastDrawnPixelOffset * KBytesPerPixel8888) <= aPixelSize);
}
