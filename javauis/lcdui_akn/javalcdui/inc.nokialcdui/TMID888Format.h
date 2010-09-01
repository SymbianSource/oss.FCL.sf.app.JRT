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
* Description:  8 bits for red, green and blue component in a pixel.
*
*/


#ifndef TMID888FORMAT_H
#define TMID888FORMAT_H

//  INCLUDES
#include "TMIDFormatConverter.h"

//  CONSTANTS
const TInt KBytesPerPixel888 = 4;
const TUint32 K888RMax = 0x00FF0000;
const TUint32 K888GMax = 0x0000FF00;
const TUint32 K888BMax = 0x000000FF;


//  CLASS DEFINITION
/**
 * 8 bits for red, green and blue component in a pixel (0x00RRGGBB).
 * The high order byte (alpha) is ignored by the implementation.
 *
 */
NONSHARABLE_CLASS(TMID888Format): public TMIDFormatConverter
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

    // Set bitmap
    void SetBitmap(TUint32* aBitmap)
    {
        iBitmap = aBitmap;
    }

    // Get bitmap
    TUint32* Bitmap()
    {
        return iBitmap;
    }

protected: // from TMIDFormatConverter
    // checks how many pixels is occupied and is last pixel in that limit
    TBool CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset);

protected: // data
    TUint32* iBitmap;

private: // data
    TUint8* iAlphaBitmap;
};

#endif // TMID888FORMAT_H
