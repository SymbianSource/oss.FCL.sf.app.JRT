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


#ifndef COLLISION_H
#define COLLISION_H

#include <gdi.h>
class TAcceleratedBitmapInfo;
class TLcdTransform;
class TRect;

enum TDisplayModeBit
{
    ENoneBit        = (1<<ENone),
    EGray2Bit       = (1<<EGray2),
    EGray256Bit     = (1<<EGray256),
    EColor64KBit    = (1<<EColor64K),
    EColor4KBit     = (1<<EColor4K),
    EColor16MUBit   = (1<<EColor16MU),
    EColor16MABit   = (1<<EColor16MA)
};

enum TTransparencyBit
{
    ETransparencyNoneBit        = 1<<ETransparencyNone,
    ETransparencyMaskBitmapBit  = 1<<ETransparencyMaskBitmap,
    ETransparencyAlphaBitmapBit = 1<<ETransparencyAlphaBitmap,
    ETransparencyAlphaChannelBit= 1<<ETransparencyAlphaChannel,
    ETransparencyMaskChannelBit = 1<<ETransparencyMaskChannel,
};

// Display modes currently supported by GenericDetectCollision
const TUint KGenericModesMask = ENoneBit
                                | EGray2Bit
                                | EGray256Bit
                                | EColor64KBit
                                | EColor4KBit
                                | EColor16MUBit
                                | EColor16MABit;

// Transparency types currently supported by GenericDetectCollision
const TUint KGenericTransMask = ETransparencyNoneBit
                                | ETransparencyMaskBitmapBit
                                | ETransparencyAlphaBitmapBit;

TBool IsOpaque(TDisplayMode aDisplayMode, TUint8* aAddress, TInt aLinePitch , TInt aX, TInt aY);

TBool GenericDetectCollision
(
    const TAcceleratedBitmapInfo*   aBitmap1,
    const TAcceleratedBitmapInfo*   aBitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

TBool DetectCollisionGray2Gray2
(
    const TAcceleratedBitmapInfo*   aGray2Bitmap1,
    const TAcceleratedBitmapInfo*   aGray2Bitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

TBool DetectCollisionGray256Gray256
(
    const TAcceleratedBitmapInfo*   aGray256Bitmap1,
    const TAcceleratedBitmapInfo*   aGray256Bitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

TBool DetectCollisionGray2Gray256
(
    const TAcceleratedBitmapInfo*   aGray2Bitmap1,
    const TAcceleratedBitmapInfo*   aGray256Bitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

TBool DetectCollisionGray256Gray2
(
    const TAcceleratedBitmapInfo*   aGray256Bitmap1,
    const TAcceleratedBitmapInfo*   aGray2Bitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

TBool HasOpaquePixel
(
    const TAcceleratedBitmapInfo*   aBitmap,
    const TRect&                    aRect
);

TBool HasOpaquePixelGray2
(
    const TAcceleratedBitmapInfo*   aGray2Bitmap,
    const TRect&                    aRect
);

TBool HasOpaquePixelGray256
(
    const TAcceleratedBitmapInfo*   aGray256Bitmap,
    const TRect&                    aRect
);

#endif // COLLISION_H
