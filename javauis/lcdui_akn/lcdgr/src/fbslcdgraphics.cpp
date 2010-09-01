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

#include <fbs.h>
#include "fbslcdgraphics.h"
#include <lcdgdrv.h>

#include "DebugArg.h"

#ifdef _LCDGDRV_FBSBITMAP_SUPPORT_


/**
 * Copy a region of a bitmap - used when creating immutable images from mutable ones, or when creating
 * transformed immutable images from other immutable images.
 */
TInt TFbsLcdGraphics::CopyRegion
(
    CLcdGraphicsDriver& aDriver,
    CFbsBitmap* aDstColorBitmap,
    CFbsBitmap* aDstAlphaBitmap,
    TTransparency aDstTransparency,
    CFbsBitmap* aSrcColorBitmap,
    CFbsBitmap* aSrcAlphaBitmap,
    TTransparency aSrcTransparency,
    const TDrawRegion& aRegion,
    const TRect& aClipRect
)
{
    TAcceleratedBitmapSpec dstColorSpec(aDstColorBitmap);
    TAcceleratedBitmapSpec dstAlphaSpec;
    TAcceleratedBitmapSpec srcColorSpec(aSrcColorBitmap);
    TAcceleratedBitmapSpec srcAlphaSpec;
    TAcceleratedBitmapInfo dstColor;
    TAcceleratedBitmapInfo dstAlpha;
    TAcceleratedBitmapInfo srcColor;
    TAcceleratedBitmapInfo srcAlpha;
    TAcceleratedBitmapInfo* dstAlphaPtr = NULL;
    TAcceleratedBitmapInfo* srcAlphaPtr = NULL;
    TBitmapLockCount count;
    TInt err;

    if (aDstAlphaBitmap)
    {
        new(&dstAlphaSpec) TAcceleratedBitmapSpec(aDstAlphaBitmap);
    }

    if (aSrcAlphaBitmap)
    {
        new(&srcAlphaSpec) TAcceleratedBitmapSpec(aSrcAlphaBitmap);
    }

    //
    // Lock and copy color bitmaps.
    //
    dstColorSpec.Lock(count);
    srcColorSpec.Lock(count);

    if (aDstAlphaBitmap)
    {
        dstAlphaSpec.Lock(count);
    }

    if (aSrcAlphaBitmap)
    {
        srcAlphaSpec.Lock(count);
    }

    dstColorSpec.GetInfo(dstColor);
    srcColorSpec.GetInfo(srcColor);

    if (aDstAlphaBitmap)
    {
        dstAlphaSpec.GetInfo(dstAlpha);
        dstAlphaPtr = &dstAlpha;
    }

    if (aSrcAlphaBitmap)
    {
        srcAlphaSpec.GetInfo(srcAlpha);
        srcAlphaPtr = &srcAlpha;
    }

    TTransformType transform = (TTransformType)aRegion.iTransform;
    TAnchor anchor = (TAnchor)aRegion.iAnchor;

    TRect srcRect(aRegion.iSrcRect);
    TRect clipRect(dstColor.iSize);
    clipRect.Intersection(aClipRect);

    //
    // Compute destination rectangle with anchor point at aRegion.iDstPoint.
    //
    // Destination rectangle need not lie within destination image.
    //
    TRect dstRect = CalcDstRect(aRegion.iDstPoint, srcRect.Size(), transform, anchor);

    err = aDriver.CopyRegion(
              &dstColor,
              dstAlphaPtr,
              aDstTransparency,
              dstRect,
              &srcColor,
              srcAlphaPtr,
              aSrcTransparency,
              srcRect,
              transform,
              clipRect
          );

    if (aSrcAlphaBitmap)
    {
        srcAlphaSpec.Unlock(count);
    }
    if (aDstAlphaBitmap)
    {
        dstAlphaSpec.Unlock(count);
    }

    srcColorSpec.Unlock(count);
    dstColorSpec.Unlock(count);

    return err;
}

TInt TFbsLcdGraphics::GetPixels
(
    CLcdGraphicsDriver& aDriver,
    CFbsBitmap*     aSrcColorBitmap,
    CFbsBitmap*     aSrcAlphaBitmap,
    TTransparency   aSrcTransparency,
    const TRect&    aSrcRect,
    TUint32*        aARGB32Pixels,
    TInt            DEBUG_ARG(aLength),
    TInt            aOffset,
    TInt            aLinePitch,
    const TSize&    aSize
)
{
    ASSERT((aOffset >= 0) && (aOffset < aLength));
    ASSERT((aOffset + aLinePitch*(aSize.iHeight-1) + aSize.iWidth - 1) >= 0);
    ASSERT((aOffset + aLinePitch*(aSize.iHeight-1) + aSize.iWidth - 1) < aLength);

    TAcceleratedBitmapSpec srcColorSpec(aSrcColorBitmap);
    TAcceleratedBitmapSpec srcAlphaSpec;
    if (aSrcAlphaBitmap)
    {
        new(&srcAlphaSpec) TAcceleratedBitmapSpec(aSrcAlphaBitmap);
    }

    TAcceleratedBitmapInfo dstColor;
    TAcceleratedBitmapInfo srcColor;
    TAcceleratedBitmapInfo srcAlpha;
    TBitmapLockCount       count;
    TAcceleratedBitmapInfo* srcAlphaPtr = NULL;

    TTransformType transform = ETransNone;
    TUint8* address = (TUint8*)(aARGB32Pixels) + aOffset*sizeof(TUint32);
    if (aLinePitch < 0)
    {
        //
        // compute address of first row of image.
        //
        address += aLinePitch * (aSize.iHeight - 1) * sizeof(TUint32);

        //
        // use vertical reflection blitter.
        //
        transform = ETransMirrorRot180;

        //
        // correct sign of linePitch
        //
        aLinePitch = - aLinePitch;
    }

    CLcdGraphicsDriver::TDriverInfo info;
    aDriver.GetDriverInfo(info);

    dstColor.iAddress     = address;
    dstColor.iDisplayMode = info.iARGB8888Mode;
    dstColor.iPixelShift  = 5;                          // or 32?
    dstColor.iLinePitch   = aLinePitch*sizeof(TUint32); // bytes
    dstColor.iSize        = aSize;                      //
    dstColor.iPhysicalAddress = NULL;                   // software bitmap

    srcColorSpec.Lock(count);
    srcColorSpec.GetInfo(srcColor);
    if (aSrcAlphaBitmap)
    {
        srcAlphaSpec.Lock(count);
        srcAlphaSpec.GetInfo(srcAlpha);
        srcAlphaPtr = &srcAlpha;
    }

    TRect dstRect(aSize);
    TRect clipRect(aSize);

    TInt err = aDriver.CopyRegion
               (
                   &dstColor,
                   NULL,
                   ETransparencyAlphaChannel,
                   dstRect,
                   &srcColor,
                   srcAlphaPtr,
                   aSrcTransparency,
                   aSrcRect,
                   transform,
                   clipRect
               );

    if (aSrcAlphaBitmap)
    {
        srcAlphaSpec.Unlock(count);
    }
    srcColorSpec.Unlock(count);

    return err;
}

/**
 *
 */
TInt TFbsLcdGraphics::SetPixels
(
    CLcdGraphicsDriver& aDriver,
    CFbsBitmap*     aDstColorBitmap,
    CFbsBitmap*     aDstAlphaBitmap,
    TTransparency   aDstTransparency,
    const TRect&    aDstRect,
    const TUint32*  aARGB32Pixels,
    TInt            DEBUG_ARG(aLength),
    TInt            aOffset,
    TInt            aLinePitch,
    const TSize&    aSize,
    TBool           aAlpha
)
{
    ASSERT((aOffset >= 0) && (aOffset < aLength));
    ASSERT((aOffset + aLinePitch*(aSize.iHeight-1) + aSize.iWidth - 1) >= 0);
    ASSERT((aOffset + aLinePitch*(aSize.iHeight-1) + aSize.iWidth - 1) < aLength);

    TAcceleratedBitmapSpec dstColorSpec(aDstColorBitmap);
    TAcceleratedBitmapSpec dstAlphaSpec;
    if (aDstAlphaBitmap)
    {
        new(&dstAlphaSpec) TAcceleratedBitmapSpec(aDstAlphaBitmap);
    }

    TAcceleratedBitmapInfo srcColor;
    TAcceleratedBitmapInfo dstColor;
    TAcceleratedBitmapInfo dstAlpha;
    TAcceleratedBitmapInfo* dstAlphaPtr = NULL;
    TBitmapLockCount count;


    TTransformType transform = ETransNone;
    TUint8* address = (TUint8*)(aARGB32Pixels) + aOffset*sizeof(TUint32);
    if (aLinePitch < 0)
    {
        //
        // compute address of first row of image.
        //
        address += aLinePitch * (aSize.iHeight - 1) * sizeof(TUint32);

        //
        // use vertical reflection blitter.
        //
        transform = ETransMirrorRot180;

        //
        // correct sign of linePitch
        //
        aLinePitch = - aLinePitch;
    }

    CLcdGraphicsDriver::TDriverInfo info;
    aDriver.GetDriverInfo(info);

    srcColor.iAddress     = address;                    // address of top-left pixel in src
    srcColor.iDisplayMode = info.iARGB8888Mode;         // how driver recognises ARGB8888 pixels
    srcColor.iPixelShift  = 5;                          //
    srcColor.iLinePitch   = aLinePitch*sizeof(TUint32); // bytes
    srcColor.iSize        = aSize;                      //
    srcColor.iPhysicalAddress = NULL;                   // software bitmap

    TTransparency srcTransparency = (aAlpha ? ETransparencyAlphaChannel : ETransparencyIgnoreChannel);

    dstColorSpec.Lock(count);
    dstColorSpec.GetInfo(dstColor);
    if (aDstAlphaBitmap)
    {
        dstAlphaSpec.Lock(count);
        dstAlphaSpec.GetInfo(dstAlpha);
        dstAlphaPtr = &dstAlpha;
    }

    TRect srcRect(aSize);
    TRect clipRect(dstColor.iSize);

    TInt err = aDriver.CopyRegion
               (
                   &dstColor,
                   dstAlphaPtr,
                   aDstTransparency,
                   aDstRect,
                   &srcColor,
                   NULL,
                   srcTransparency,
                   srcRect,
                   transform,
                   clipRect
               );

    if (aDstAlphaBitmap)
    {
        dstAlphaSpec.Unlock(count);
    }
    dstColorSpec.Unlock(count);

    return err;
}


TBool TFbsLcdGraphics::DetectCollision
(
    CLcdGraphicsDriver& aDriver,

    CFbsBitmap*         aMask1,
    TTransparency       aTransparency1,
    const TSize&        aSize1,
    const TRect&        aRect1,
    TInt                aTransform1,
    const TPoint&       aPoint1,

    CFbsBitmap*         aMask2,
    TTransparency       aTransparency2,
    const TSize&        aSize2,
    const TRect&        aRect2,
    TInt                aTransform2,
    const TPoint&       aPoint2
)
{
    TAcceleratedBitmapSpec bitmapSpec1;
    TAcceleratedBitmapInfo bitmapInfo1;
    TAcceleratedBitmapInfo* bitmapPtr1 = NULL;
    TAcceleratedBitmapSpec bitmapSpec2;
    TAcceleratedBitmapInfo bitmapInfo2;
    TAcceleratedBitmapInfo* bitmapPtr2 = NULL;

    TBitmapLockCount count;

    if (aMask1)
    {
        bitmapSpec1 = TAcceleratedBitmapSpec(aMask1);
    }

    if (aMask2)
    {
        bitmapSpec2 = TAcceleratedBitmapSpec(aMask2);
    }

    if (aMask1)
    {
        bitmapSpec1.Lock(count);
        bitmapSpec1.GetInfo(bitmapInfo1);
        bitmapPtr1 = &bitmapInfo1;
    }

    if (aMask2)
    {
        bitmapSpec2.Lock(count);
        bitmapSpec2.GetInfo(bitmapInfo2);
        bitmapPtr2 = &bitmapInfo2;
    }

    TBool collision =  aDriver.DetectCollision(
                           bitmapPtr1,
                           aTransparency1,
                           aSize1,
                           aRect1,
                           aTransform1,
                           aPoint1,

                           bitmapPtr2,
                           aTransparency2,
                           aSize2,
                           aRect2,
                           aTransform2,
                           aPoint2
                       );

    if (aMask2)
    {
        bitmapSpec2.Unlock(count);
    }

    if (aMask1)
    {
        bitmapSpec1.Unlock(count);
    }

    return collision;
}
#endif

/**
 * Compute destination rectangle of rectangle of size aSrcSize, transformed by aSrcTransform
 * and then positioned with a reference point specified by aSrcAnchor at aDstPoint.
 */
TRect CalcDstRect(const TPoint& aDstPoint, const TSize& aSrcSize, TTransformType aSrcTransform, TAnchor aSrcAnchor)
{
    TPoint point = aDstPoint;
    TSize  size  = aSrcSize;

    if (aSrcTransform & EReflectDiag)
    {
        TInt tmp = size.iWidth;
        size.iWidth  = size.iHeight;
        size.iHeight = tmp;
    }

    if (aSrcAnchor & EAnchorHCenter)
    {
        point.iX -= size.iWidth/2;
    }
    else if (aSrcAnchor & EAnchorRight)
    {
        point.iX -= size.iWidth;
    }

    if (aSrcAnchor & EAnchorVCenter)
    {
        point.iY -= size.iHeight/2;
    }
    else if (aSrcAnchor & EAnchorBottom)
    {
        point.iY -= size.iHeight;
    }

    return TRect(point, size);
}
