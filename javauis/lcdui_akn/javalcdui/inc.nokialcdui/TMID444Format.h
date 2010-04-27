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
* Description:  4 bits for red, green and blue component in a pixel.
*
*/


#ifndef TMID444FORMAT_H
#define TMID444FORMAT_H

//  INCLUDES
#include "TMIDFormatConverter.h"

//  CONSTANTS
const TUint16 K444RMax = 0xF00;
const TUint16 K444GMax = 0x0F0;
const TUint16 K444BMax = 0x00F;
const TInt KBytesInPixel444 = 2;


//  CLASS DEFINITION
/**
 * 4 bits for red, green and blue component in a pixel, stored as a
 * short (0x0RGB).
 *
 */

NONSHARABLE_CLASS(TMID444Format): public TMIDFormatConverter
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

    // Set bitmap and alpha bitmap
    void SetBitmap(TUint32* aBitmap)
    {
        iBitmap = (TUint16*)aBitmap;
    }

    // Get bitmap
    TUint32* Bitmap()
    {
        return (TUint32*)iBitmap;
    }
protected: // from TMIDFormatConverter
    // checks how many pixels is occupied and is last pixel in that limit
    TBool CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset);

private: // data
    TUint16* iBitmap;
    TUint8* iAlphaBitmap;
};

#endif // TMID444FORMAT_H
