/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTGRIMAGEDATA_H
#define SWTGRIMAGEDATA_H


#include "eswtgraphics.h"


class CSwtGrPaletteData;


/**
 * CSwtGrImageData
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtGrImageData)
        : public CBase
        , public MSwtImageData
{
// Methods
public:
    static CSwtGrImageData* NewL(const TInfo& aInfo, CSwtGrPaletteData* aPalette,
    HBufC8* aPixelBuffer, HBufC8* aMaskBuffer, HBufC8* aAlphaBuffer);
    ~CSwtGrImageData();

private:
    CSwtGrImageData(const TInfo& aInfo, CSwtGrPaletteData* aPalette,
                    HBufC8* aPixelBuffer, HBufC8* aMaskBuffer, HBufC8* aAlphaBuffer);

// From MSwtImageData
public:
    const TInfo&           Info()        const;
    const MSwtPaletteData& Palette()     const;
    const HBufC8&          PixelBuffer() const;
    const HBufC8*          MaskBuffer()  const;
    const HBufC8*          AlphaBuffer() const;

// Data
private:
    TInfo              iInfo;         // Information about the image
    CSwtGrPaletteData* iPalette;      // The color table for the image.
    HBufC8*            iPixelBuffer;  // The pixel data of the image.
    HBufC8*            iMaskBuffer;   // An icon-specific field containing the data from the icon mask.
    HBufC8*            iAlphaBuffer;  // The alpha data of the image.
};


#endif // SWTGRIMAGEDATA_H
