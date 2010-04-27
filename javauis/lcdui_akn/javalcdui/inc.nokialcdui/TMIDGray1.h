/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TMIDGRAY1_H
#define TMIDGRAY1_H


//  INCLUDES
#include "TMIDFormatConverter.h"

//  CONSTANTS
const TInt KPixelsInByteGray1 = 8;
const TInt KGray1Shift = 3;

//  CLASS DEFINITION
/**
 * 1 bit format, 2 distinct color values (on/off), stored as
 * a byte. 8 pixel values in a single byte, packed as close
 * as possible.
 */

NONSHARABLE_CLASS(TMIDGray1): public TMIDFormatConverter
{
public: //  initialization
    void InitializeL(const TMIDBitmapParameters& aParameters);

public: // from TMIDFormatConverter
    // Convert from internal to int
    TUint32 ConvertInternal(const TMIDInternalARGB& aInternal);

    // Convert from int to internal
    void Convert(TMIDInternalARGB& aResult, TUint32 aColor) const;

    // Get pixel from current position
    void GetPixel(TMIDInternalARGB& aResult) const;

    // Get alpha which is in same format as bitmap
    TUint8 GetAlpha() const;

    // Plot pixel to current position
    void PlotPixel(const TMIDInternalARGB& aInternal);

    // Plot pixel with alpha
    void PlotPixelWithAlpha(const TMIDInternalARGB& aInternal);

    // Set bitmap
    void SetBitmap(TUint32* aBitmap)
    {
        iBitmap = (TUint8*)aBitmap;
    }

    // Get bitmap
    TUint32* Bitmap()
    {
        return (TUint32*)iBitmap;
    }

protected: // from TMIDFormatConverter
    // checks how many pixels is occupied and is last pixel in that limit
    TBool CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset);

private:

    TUint8* iBitmap;
};

#endif // TMIDGRAY1_H
