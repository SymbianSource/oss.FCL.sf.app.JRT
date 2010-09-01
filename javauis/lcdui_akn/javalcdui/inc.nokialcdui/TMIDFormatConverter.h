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
* Description:  Abstract base class for format converters.
*
*/



#ifndef TMIDFORMATCONVERTER_H
#define TMIDFORMATCONVERTER_H

// INCLUDES
#include <gdi.h> //TDisplayMode
#include "TMIDInternalARGB.h"
#include "TMIDBitmapParameters.h"
#include "TMIDTransformer.h"
#include "nativecolors.h"

// CONSTANTS
const TInt KErrIllegalArgumentException = -11;
const TInt KErrArrayIndexOutOfBoundsException = -12;

//  CLASS DEFINITION
/**
 * Abstract base class for format converters.
 *
 */
NONSHARABLE_CLASS(TMIDFormatConverter)
{
public: // Constructor
    TMIDFormatConverter();

public: //  initialization
    virtual void InitializeL(const TMIDBitmapParameters& aParameters);

public: // abstract methods
    // Convert from internal to int
    virtual TUint32 ConvertInternal(const TMIDInternalARGB& aInternal) = 0;

    // Convert from int to internal
    virtual void Convert(TMIDInternalARGB& aResult, TUint32) const = 0;

    // Get pixel from current position
    virtual void GetPixel(TMIDInternalARGB& aResult) const = 0;

    // Get alpha which is in same format as bitmap
    virtual TUint8 GetAlpha() const = 0;

    // Get pixel using alpha bitmap
    virtual void GetPixelWithAlpha(TMIDInternalARGB& aResult) const;

    // Plot pixel to current position
    virtual void PlotPixel(const TMIDInternalARGB& aInternal) = 0;

    // Get pixel using alpha bitmap
    virtual void PlotPixelWithAlpha(const TMIDInternalARGB& aInternal);

    // Checks is alpha calculation needed
    virtual TBool IsAlpha();

    // Set bitmap
    virtual void SetBitmap(TUint32* aBitmap) = 0;

    // Get bitmap
    virtual TUint32* Bitmap() = 0;

public: // new methods
    // Advance position in x-axis, returns EFalse if advancing is in the end
    inline TBool AdvanceX();

    // Advance position in y-axis, returns EFalse if advancing is in the end
    inline TBool AdvanceY();

    // set alpha bitmap
    void SetAlpha(TUint32* aAlphaBitmap, const TDisplayMode& aAlphaMode)
    {
        iAlphaBitmap = aAlphaBitmap;
        iAlphaMode = aAlphaMode;
    }

protected: // abstract methods
    // checks how many pixels is occupied and is last pixel in that limit
    virtual TBool CheckSize(TInt aPixelSize, TInt aLastDrawnPixelOffset) = 0;

protected: // new methods
    void CalculateAlpha(TMIDInternalARGB& aTargetColor,
                        const TMIDInternalARGB& aBelowColor);
public:
    TInt iOffset;
    TRect iBitmapRect;

protected: // data
    TMIDTransformer iTransformer;
    TInt iScanlength;
    TInt iAlphaScanLength; // in 32 bit words
    TInt iAnchor;
    TUint32* iAlphaBitmap;
    TDisplayMode iAlphaMode;
    TBool iTransparency;
};

inline TBool TMIDFormatConverter::AdvanceX()
{
    iOffset += iTransformer.iXMove.iX;
    return ETrue;
}

inline TBool TMIDFormatConverter::AdvanceY()
{
    iOffset += iTransformer.iYMove.iX;
    return ETrue;
}

#endif // TMIDFORMATCONVERTER_H
