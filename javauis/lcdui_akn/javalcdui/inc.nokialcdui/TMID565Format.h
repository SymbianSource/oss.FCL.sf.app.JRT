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
* Description:  5 bits for red, 6 for green and 5 for blue component in a pixel.
*
*/


#ifndef TMID565FORMAT_H
#define TMID565FORMAT_H

//  INCLUDES
#include "TMIDFormatConverter.h"


//  CONSTANTS
const TUint32 K565RMax = 0x0000F800;
const TUint32 K565GMax = 0x000007E0;
const TUint32 K565BMax = 0x0000001F;
const TInt KBytesPerPixel565 = 2;


//  CLASS DEFINITION
/**
 * 5 bits for red, 6 for green and 5 for blue component in a pixel, stored
 * as a short (0xRRRRRGGGGGGBBBBBB).
 *
 */

NONSHARABLE_CLASS(TMID565Format):  public TMIDFormatConverter
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
};

#endif // TMID565FORMAT_H
