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


#ifndef SWTIMAGEDATA_H
#define SWTIMAGEDATA_H


#include <jni.h>
#include "eswtgraphics.h"


class CSwtPaletteData;


/**
 * CSwtImageData
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtImageData)
        : public CBase
        , public MSwtImageData
{
// Own Methods
public:
    static CSwtImageData* NewL(JNIEnv* aJniEnv, jobject aImageData);
    ~CSwtImageData();

private:
    inline CSwtImageData();
    void ConstructL(JNIEnv* aJniEnv, jobject aImageData, TBool& aFailed);

// From MSwtImageData
public:
    const TInfo&           Info() const;
    const MSwtPaletteData& Palette() const;
    const HBufC8&          PixelBuffer() const;
    const HBufC8*          MaskBuffer() const;
    const HBufC8*          AlphaBuffer() const;

// Data
private:
    TInfo            iInfo;         // Information about the image
    CSwtPaletteData* iPalette;      // The color table for the image.
    HBufC8*          iPixelBuffer;  // The pixel data of the image.
    HBufC8*          iMaskBuffer;   // An icon-specific field containing the data from the icon mask.
    HBufC8*          iAlphaBuffer;  // The alpha data of the image.
};


#endif // SWTIMAGEDATA_H



