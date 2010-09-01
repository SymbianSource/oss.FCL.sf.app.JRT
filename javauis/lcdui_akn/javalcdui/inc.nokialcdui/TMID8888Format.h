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
* Description:   8 bits for alpha, red, green and blue component in a pixel.
*
*/


#ifndef TMID8888FORMAT_H
#define TMID8888FORMAT_H

//  INCLUDES
#include "TMIDFormatConverter.h"

//  CONSTANTS
const TUint32 K8888AMax = 0xFF000000;
const TUint32 K8888RMax = 0x00FF0000;
const TUint32 K8888GMax = 0x0000FF00;
const TUint32 K8888BMax = 0x000000FF;
const TInt KBytesPerPixel8888 = 4;


//  CLASS DEFINITION
/**
 * 8 bits for alpha, red, green and blue component in a pixel (0xAARRGGBB).
 *
 * note that this class does not use external alpha bitmap, use
 * TMID888AFormat for that.
 *
 */

NONSHARABLE_CLASS(TMID8888Format):  public TMIDFormatConverter
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

    // Get pixel with alpha values
    void GetPixelWithAlpha(TMIDInternalARGB& aResult) const;

    // Plot pixel to current position
    void PlotPixel(const TMIDInternalARGB& aInternal);

    // Checks is alpha calculation needed
    TBool IsAlpha()
    {
        return ETrue; /* always alpha */
    }

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

private: // data
    TUint32* iBitmap;
};

#endif // TMID8888FORMAT_H
