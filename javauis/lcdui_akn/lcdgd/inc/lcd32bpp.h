/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef LCD32BPP_H
#define LCD32BPP_H

class TRect;
class TAcceleratedBitmapInfo;
class TLcdTransform;

extern void FillTriangle32Bpp
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor32Bpp,
    const TRect& aClipRect
);


/**
 * Opaque 32bpp to opaque 32bpp bitblt with simple transform.
 */
extern void BitBltThirtyTwoBppSimple
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // ignored
    const TLcdTransform&            aTransform
);

/**
 * Opaque 32bpp to opaque 32bpp bitblt with transform.
 */
extern void BitBltThirtyTwoBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // ignored
    const TLcdTransform&            aTransform
);

/**
 * 32bpp color 8bpp mask to 32bpp color 8bpp mask bitblt with transform.
 */
extern void BitBltThirtyTwoBppEightBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

/**
 * 32bpp color 1bpp mask to 32bpp color 1bpp mask bitblt with transform.
 */
extern void BitBltThirtyTwoBppOneBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

/**
 * 32bpp color 32bpp mask to 32bpp color 32bpp mask bitblt with transform.
 */
extern void BitBltThirtyTwoBppThirtyTwoBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

#endif // LCD32BPP_H
