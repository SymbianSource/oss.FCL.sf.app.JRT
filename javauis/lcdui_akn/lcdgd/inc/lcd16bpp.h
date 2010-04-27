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

#ifndef LCD16BPP_H
#define LCD16BPP_H

class TRect;
class TAcceleratedBitmapInfo;
class TLcdTransform;


extern void DrawRegion16Bpp1BppTo16BppOpaque
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,    // must be 16bpp mode
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,           // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,    // must match aDestination mode
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // must be EGray2
    const TLcdTransform&            aSrcTransform       // includes anchor
);

extern void DrawImage16Bpp1BppTo16BppOpaque
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,    // must be 16bpp mode
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,           // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,    // must be 16bpp mode
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // must be EGray2
    const TLcdTransform&            aSrcTransform       // includes anchor
);

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
extern void DrawRegion16Bpp16BppTo16BppOpaque
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,    // must be 16bpp mode
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,           // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,    // must match aDestination mode
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // must be 16bpp mode
    const TLcdTransform&            aSrcTransform       // includes anchor
);
#endif

/**
 * Opaque 16bpp to opaque 16bpp bitblt with simple transform.
 */
extern void BitBltSixteenBppSimple
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // ignored
    const TLcdTransform&            aTransform
);

/**
 * Opaque 16bpp to opaque 16bpp bitblt with transform.
 */
extern void BitBltSixteenBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // ignored
    const TLcdTransform&            aTransform
);

/**
 * 16bpp color 1bpp mask to 16bpp color 1bpp mask bitblt with transform.
 */
extern void BitBltSixteenBppOneBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

/**
 * 16bpp color 8bpp mask to 16bpp color 8bpp mask bitblt with transform.
 */
extern void BitBltSixteenBppEightBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

/**
 * 16bpp color 16bpp mask to 16bpp color 16bpp mask bitblt with transform.
 */
extern void BitBltSixteenBppSixteenBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

extern void FillTriangle16Bpp
(
    const TAcceleratedBitmapInfo*   aBitmap,
    const TPoint                    aPoints[],
    TUint32                         aColor16Bpp,
    const TRect&                    aClipRect
);

#endif // LCD16BPP_H
