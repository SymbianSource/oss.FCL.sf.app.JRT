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
* Description:  Wrapper for converter used for drawing to Canvas or Image.
*
*/


#ifndef CMIDNATIVECONVERTER_H
#define CMIDNATIVECONVERTER_H

//  INCLUDES
#include "TMIDFormatConverter.h"
#include "TMID8888Format.h"

//  CONSTANTS
const TUint16 K565RMax = 0xF800;
const TUint16 K565GMax = 0x07E0;
const TUint16 K565BMax = 0x001F;
const TInt KBytesPerPixel565 = 2;

// FORWARD DECLARATIONS
class CMIDConvertFactory;

//  CLASS DEFINITION
/**
 * Wrapper for converter that is used for drawing to either Canvas or Image
 *
 */
NONSHARABLE_CLASS(CMIDNativeConverter): public CBase
{
public: // Destructor
    ~CMIDNativeConverter();

public: // Initialization
    static CMIDNativeConverter* NewL(const TMIDBitmapParameters& aParameters);

public: // new methods

    // Convert using default converter
    void Convert(TMIDInternalARGB& aResult, TUint32 aColor) const;

    // Get pixel from current position (includes alpha)
    void GetPixel(TMIDInternalARGB& aResult) const;

    // Plot pixel to current position
    void PlotPixel(const TMIDInternalARGB& aInternal);

    // Plot pixel with alpha calcultion
    void PlotPixelWithAlpha(const TMIDInternalARGB& aInternal);

    // Plot pixel with alpha calcultion from bitmap
    void PlotPixelWithAlphaBitmap(const TMIDInternalARGB& aInternal);

    // Advance position in x-axis, returns EFalse if advancing is in the end
    inline TBool AdvanceX();

    // Advance position in y-axis, returns EFalse if advancing is in the end
    inline TBool AdvanceY();

    // Set bitmaps to native converter.
    void SetBitmaps(TUint32* aBitmap);

    // Start point setting
    void SetDrawRect(TRect aDrawRect);

    // Draw pixel to specified point (checks clipping rect)
    void PlotPixel(const TPoint& aPos, const TMIDInternalARGB& aInternal,
                   const TRect& aClip);

    // Draws scanline from aPos to aLenght with aColor
    void DrawScanLine(TPoint aPos, TInt aLength,
                      const TMIDInternalARGB& aInternal, const TRect& aClip);

    // True if native converter has alpha bitmap
    TBool HasAlphaBitmap();

protected: // Construction
    virtual void ConstructL(const TMIDBitmapParameters& aParameters);
    CMIDNativeConverter();

private: // new methods
    void CalculateAlpha(TMIDInternalARGB& aTargetColor,
                        const TMIDInternalARGB& aBelowColor);

private: // data
    TRect iDrawRect;
    TPoint iPosition;

    TInt iScanlength;

    TUint32* iAlphaBitmap;
    TDisplayMode iAlphaMode;

    // default converter
    TMID8888Format iDefaultConverter;

    TMIDFormatConverter* iNativeFormat;

};

inline TBool CMIDNativeConverter::AdvanceX()
{
    ++iPosition.iX;
    ++iNativeFormat->iOffset;
    if (iPosition.iX >= iDrawRect.iBr.iX)
    {
        iPosition.iX = iDrawRect.iTl.iX;
        return EFalse;
    }
    return ETrue;
}

inline TBool CMIDNativeConverter::AdvanceY()
{
    ++iPosition.iY;
    iNativeFormat->iOffset += iScanlength - iDrawRect.Width();
    return(iPosition.iY < iDrawRect.iBr.iY);
}
#endif // CMIDNATIVECONVERTER_H
