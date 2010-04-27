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


#ifndef LCDBITBLT_H
#define LCDBITBLT_H

class TAcceleratedBitmapInfo;
class TRect;
class TLcdTransform;

typedef void (*TBlitLineFunction)(TUint8*,TInt,TUint8*,TInt);
typedef void (*TMaskBlitLineFunction)(TUint8*,TInt,TUint8*,TInt,TUint8*,TInt);

/**
 * Generic image copying routine, with no separate mask or alpha bitmap. The
 * work is carried out by calls to aBlitLineFn for each line in the destination
 * bitmap.
 */
extern void GenericBlit
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TRect&                    aDstRect,       // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TLcdTransform&            aTransform,     // includes anchor
    TBlitLineFunction               aBlitLineFn     // function to call on each line
);

/**
 * Generic image copying routine, with separate mask or alpha bitmap. The
 * work is carried out by calls to aBlitLineFn for each line in the destination
 * bitmap. The aAlphaBitmap is treated as corresponding to the source bitmap,
 * unless aIsSrcAlpha is set to EFalse.
 */
extern void GenericMaskBlit
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TRect&                    aDstRect,       // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TLcdTransform&            aTransform,     // includes anchor
    const TAcceleratedBitmapInfo*   aAlphaBitmap,   // source or destination alpha
    TBool                           aIsSrcAlpha,    // ETrue if alpha is source
    TMaskBlitLineFunction           aBlitLineFn     // function to call on each line
);

/**
 * Opaque 1bpp to opaque 1bpp bitblt with transform.
 */
extern void BitBltOneBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
);

/**
 * Opaque 8bpp to opaque 8bpp bitblt with simple transform.
 */
extern void BitBltEightBppSimple
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
);

/**
 * Opaque 8bpp to opaque 8bpp bitblt with transform.
 */
extern void BitBltEightBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
);


#endif // LCDBITBLT_H
