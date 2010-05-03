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
#include "lcdtransform.h"
#include "collision.h"

/**
Return ETrue if the mask pixel is fully opaque.
*/
TBool IsOpaque(TDisplayMode aDisplayMode, TUint8* aAddress, TInt aLinePitch , TInt aX, TInt aY)
{
    switch (aDisplayMode)
    {
        // binary transparent
    case EGray2:
    {
        TUint8 m1 = *(aAddress + aY*aLinePitch + (aX>>3));
        TUint8 srcMask1 = m1 >> (aX&0x7);
        return srcMask1 & 0x1;
    }

    // alpha transparent
    case EGray256:
    {
        TUint8 srcMask2 = *(TUint8*)(aAddress + aY*aLinePitch + aX*sizeof(TUint8));
        // semi-transparent is treated as transparent.
        return srcMask2 == 0xff;
    }

    // mask matched
    case EColor64K:
    {
        TUint16 srcMask3 = *(TUint16*)(aAddress + aY*aLinePitch + aX*sizeof(TUint16));
        return srcMask3 == 0xffff;
    }
    case EColor4K:
    {
        TUint16 srcMask4 = *(TUint16*)(aAddress + aY*aLinePitch + aX*sizeof(TUint16));
        return srcMask4 == 0xfff;
    }
    case EColor16MU:
    {
        TUint32 srcMask5  = *(TUint32*)(aAddress + aY*aLinePitch + aX*sizeof(TUint32));
        return srcMask5 == 0xffffff;
    }

    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    }
    return EFalse;
}

TBool GenericDetectCollision
(
    const TAcceleratedBitmapInfo*   aBitmap1,
    const TAcceleratedBitmapInfo*   aBitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
)
{

    // if only one mask, just need to check if it has any opaque pixels
    if (aBitmap1 == NULL)
    {
        return ::HasOpaquePixel(aBitmap2, aRect2);
    }
    if (aBitmap2 == NULL)
    {
        return ::HasOpaquePixel(aBitmap1, aRect1);
    }

    TLcdTransform transform = aTransform1 * aTransform2.Inverse();
    TRect rect = aRect2;

    TInt x1 = rect.iTl.iX;
    TInt x2 = rect.iBr.iX-1;    // inclusive!
    TInt y  = rect.iTl.iY;
    TInt h  = rect.Height();

    TPoint srcPoint = transform(rect.iTl);
    TInt dudx = transform.iDuDx;
    TInt dudy = transform.iDuDy;
    TInt dvdx = transform.iDvDx;
    TInt dvdy = transform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

    const TInt linePitch1   = aBitmap1->iLinePitch;
    const TInt linePitch2   = aBitmap2->iLinePitch;
    TUint8* address1        = aBitmap1->iAddress;
    TUint8* address2        = aBitmap2->iAddress;

    for (; h>0; --h)
    {
        u=u0;
        v=v0;

        for (TInt x=x1; x<=x2; x++)
        {
            TBool o1 = IsOpaque(aBitmap2->iDisplayMode, address2, linePitch2, x, y);
            TBool o2 = IsOpaque(aBitmap1->iDisplayMode, address1, linePitch1, u, v);

            if (o1 && o2)
            {
                return ETrue;
            }

            u+=dudx;
            v+=dvdx;
        }

        u0+=dudy;
        v0+=dvdy;
        y++;
    }

    return EFalse;
}


TBool DetectCollisionGray2Gray2
(
    const TAcceleratedBitmapInfo*   aGray2Bitmap1,
    const TAcceleratedBitmapInfo*   aGray2Bitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
)
{

    // if only one mask, just need to check if it has any opaque pixels
    if (aGray2Bitmap1 == NULL)
    {
        return ::HasOpaquePixelGray2(aGray2Bitmap2, aRect2);
    }
    if (aGray2Bitmap2 == NULL)
    {
        return ::HasOpaquePixelGray2(aGray2Bitmap1, aRect1);
    }

    ASSERT(aGray2Bitmap1->iDisplayMode == EGray2);
    ASSERT(aGray2Bitmap2->iDisplayMode == EGray2);

    TLcdTransform transform = aTransform1 * aTransform2.Inverse();
    TRect rect = aRect2;

    TInt x1 = rect.iTl.iX;
    TInt x2 = rect.iBr.iX-1;    // inclusive!
    TInt y  = rect.iTl.iY;
    TInt h  = rect.Height();

    TPoint srcPoint = transform(rect.iTl);
    TInt dudx = transform.iDuDx;
    TInt dudy = transform.iDuDy;
    TInt dvdx = transform.iDvDx;
    TInt dvdy = transform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

    const TInt linePitch1   = aGray2Bitmap1->iLinePitch;
    const TInt linePitch2   = aGray2Bitmap2->iLinePitch;
    TUint8* address1        = aGray2Bitmap1->iAddress;
    TUint8* address2        = aGray2Bitmap2->iAddress;

    address2 += y*linePitch2;
    for (; h>0; --h)
    {
        u=u0;
        v=v0;

        for (TInt x=x1; x<=x2; x++)
        {
            TUint8 m2 = address2[(x>>3)];
            TUint8 p2 = m2 >> (x&0x7);

            if (p2&0x1)
            {
                TUint8 m1 = *(address1 + v*linePitch1 + (u>>3));
                TUint8 p1 = m1 >> (u&0x7);

                if (p1&0x1)
                {
                    return ETrue;
                }
            }

            u+=dudx;
            v+=dvdx;
        }

        u0+=dudy;
        v0+=dvdy;

        address2 += linePitch2;
    }

    return EFalse;
}

TBool DetectCollisionGray256Gray256
(
    const TAcceleratedBitmapInfo*   aGray256Bitmap1,
    const TAcceleratedBitmapInfo*   aGray256Bitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
)
{
    // if only one mask, just need to check if it has any opaque pixels
    if (aGray256Bitmap1 == NULL)
    {
        return ::HasOpaquePixelGray256(aGray256Bitmap2, aRect2);
    }
    if (aGray256Bitmap2 == NULL)
    {
        return ::HasOpaquePixelGray256(aGray256Bitmap1, aRect1);
    }

    ASSERT(aGray256Bitmap1->iDisplayMode == EGray256);
    ASSERT(aGray256Bitmap2->iDisplayMode == EGray256);

    TLcdTransform transform = aTransform1 * aTransform2.Inverse();
    TRect rect = aRect2;

    const TInt x = rect.iTl.iX;
    const TInt y = rect.iTl.iY;
    const TInt w = rect.Width();
    TInt h = rect.Height();

    TPoint srcPoint = transform(rect.iTl);
    TInt dudx = transform.iDuDx;
    TInt dudy = transform.iDuDy;
    TInt dvdx = transform.iDvDx;
    TInt dvdy = transform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;


    const TInt linePitch1   = aGray256Bitmap1->iLinePitch;
    const TInt linePitch2   = aGray256Bitmap2->iLinePitch;
    TUint8* address1        = aGray256Bitmap1->iAddress;
    TUint8* address2        = aGray256Bitmap2->iAddress;

    address2 += y*linePitch2 + x*sizeof(TUint8);
    for (; h>0; --h)
    {
        TInt u = u0;
        TInt v = v0;

        TUint8* ptr2 = address2;
        const TUint8* end = ptr2 + w;

        while (ptr2 < end)
        {
            TUint8 m2 = *ptr2++;
            if (m2 == 255)
            {
                TUint8 m1 = *(TUint8*)(address1 + v*linePitch1 + u*sizeof(TUint8));
                if (m1 == 255)
                {
                    return ETrue;
                }
            }

            u+=dudx;
            v+=dvdx;
        }

        u0+=dudy;
        v0+=dvdy;
        address2 += linePitch2;
    }

    return EFalse;
}

TBool DetectCollisionGray2Gray256
(
    const TAcceleratedBitmapInfo*   aGray2Bitmap,
    const TAcceleratedBitmapInfo*   aGray256Bitmap,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
)
{
    // if only one mask, just need to check if it has any opaque pixels
    if (aGray2Bitmap == NULL)
    {
        return ::HasOpaquePixelGray256(aGray256Bitmap, aRect2);
    }
    if (aGray256Bitmap == NULL)
    {
        return ::HasOpaquePixelGray2(aGray2Bitmap, aRect1);
    }

    ASSERT(aGray2Bitmap->iDisplayMode == EGray2);
    ASSERT(aGray256Bitmap->iDisplayMode == EGray256);

    TLcdTransform transform = aTransform1 * aTransform2.Inverse();
    TRect rect = aRect2;

    const TInt x = rect.iTl.iX;
    const TInt y = rect.iTl.iY;
    const TInt w = rect.Width();
    TInt h = rect.Height();

    TPoint srcPoint = transform(rect.iTl);
    TInt dudx = transform.iDuDx;
    TInt dudy = transform.iDuDy;
    TInt dvdx = transform.iDvDx;
    TInt dvdy = transform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;

    const TInt linePitch1   = aGray2Bitmap->iLinePitch;
    const TInt linePitch2   = aGray256Bitmap->iLinePitch;
    TUint8* address1        = aGray2Bitmap->iAddress;
    TUint8* address2        = aGray256Bitmap->iAddress;

    address2 += y*linePitch2 + x*sizeof(TUint8);
    for (; h>0; --h)
    {
        TInt u = u0;
        TInt v = v0;

        TUint8* ptr2 = address2;
        const TUint8* end = ptr2 + w;
        while (ptr2 < end)
        {
            TUint8 m2 = *ptr2++;

            if (m2 == 255)
            {
                TUint8 m1 = *(address1 + v*linePitch1 + (u>>3));
                TUint8 p1 = m1 >> (u&0x7);

                if (p1&0x1)
                {
                    return ETrue;
                }
            }

            u+=dudx;
            v+=dvdx;
        }

        u0+=dudy;
        v0+=dvdy;
        address2 += linePitch2;
    }

    return EFalse;
}

TBool DetectCollisionGray256Gray2
(
    const TAcceleratedBitmapInfo*   aGray256Bitmap,
    const TAcceleratedBitmapInfo*   aGray2Bitmap,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
)
{
    return DetectCollisionGray2Gray256
           (
               aGray2Bitmap,
               aGray256Bitmap,
               aRect2,
               aRect1,
               aTransform2,
               aTransform1
           );
}

TBool HasOpaquePixel
(
    const TAcceleratedBitmapInfo*   aBitmap,
    const TRect&                    aRect
)
{
    const TInt linePitch    = aBitmap->iLinePitch;
    TUint8* address         = aBitmap->iAddress;

    TInt x1 = aRect.iTl.iX;
    TInt x2 = aRect.iBr.iX-1;   // inclusive!
    TInt y  = aRect.iTl.iY;
    TInt h  = aRect.Height();

    for (; h>0; --h)
    {
        for (TInt x=x1; x<=x2; x++)
        {
            if (IsOpaque(aBitmap->iDisplayMode, address, linePitch, x, y))
            {
                return ETrue;
            }
        }
        y++;
    }

    return EFalse;
}

TBool HasOpaquePixelGray2
(
    const TAcceleratedBitmapInfo*   aGray2Bitmap,
    const TRect&                    aRect
)
{
    ASSERT(aGray2Bitmap->iDisplayMode == EGray2);

    const TInt linePitch    = aGray2Bitmap->iLinePitch;
    TUint8* address         = aGray2Bitmap->iAddress;

    const TInt x1   = aRect.iTl.iX;
    const TInt x2   = aRect.iBr.iX-1;   // inclusive!
    const TInt y    = aRect.iTl.iY;
    TInt h  = aRect.Height();

    address += y*linePitch;
    for (; h>0; --h)
    {
        for (TInt x=x1; x<=x2; x++)
        {
            TUint8 m = address[(x>>3)];
            TUint8 p = m >> (x&0x7);

            if (p&0x1)
            {
                return ETrue;
            }
        }
        address += linePitch;
    }

    return EFalse;
}

TBool HasOpaquePixelGray256
(
    const TAcceleratedBitmapInfo*   aGray256Bitmap,
    const TRect&                    aRect
)
{
    ASSERT(aGray256Bitmap->iDisplayMode == EGray256);

    const TInt linePitch = aGray256Bitmap->iLinePitch;
    TUint8* address      = aGray256Bitmap->iAddress;

    const TInt x = aRect.iTl.iX;
    const TInt y = aRect.iTl.iY;
    const TInt w = aRect.Width();
    TInt h = aRect.Height();

    address += y*linePitch + x*sizeof(TUint8);
    for (; h>0; --h)
    {
        TUint8* ptr = address;
        const TUint8* end = ptr + w;

        while (ptr < end)
        {
            // semi-transparent is treated as transparent.
            TUint8 m = *ptr++;
            if (m == 255)
            {
                return ETrue;
            }
        }
        address += linePitch;
    }

    return EFalse;
}

