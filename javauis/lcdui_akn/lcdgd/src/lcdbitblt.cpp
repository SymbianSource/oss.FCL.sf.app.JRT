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


#include <graphicsaccelerator.h>
#include <gdi.h>
#include "lcdgdrvif.h"
#include "lcdgdrvutil.h"
#include "lcdtransform.h"
#include "lcdbitblt.h"

LOCAL_C INLINE TInt PixelPitch(const TAcceleratedBitmapInfo* aBitmap)
{
    switch (aBitmap->iDisplayMode)
    {
    case EColor64K:
    case EColor4K:
        return 2;
    case EColorARGB8888:
    case EColor16MU:
        return 4;
    case EGray256:
        return 1;
    }

    // Any other display mode is either invalid, or has a fractional number of
    // bytes per pixel, and cannot be handled by this routine.
    ASSERT(EFalse);
    return 0;   // Pacify the compiler
}

extern void GenericBlit
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TRect&                    aDstRect,       // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TLcdTransform&            aTransform,     // includes anchor
    TBlitLineFunction               aBlitLineFn     // function to call on each line
)
{
    ASSERT(aDstColorBitmap != NULL);
    ASSERT(aSrcColorBitmap != NULL);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;

    //
    // For each bitmap, calculate the starting address and byte offsets to the
    // next address for one line down and one pixel right.
    //
    const TInt dstLinePitch = aDstColorBitmap->iLinePitch;
    const TInt dstPixelPitch = PixelPitch(aDstColorBitmap);
    TUint8* dstAddress = aDstColorBitmap->iAddress;
    dstAddress += aDstRect.iTl.iY * dstLinePitch + aDstRect.iTl.iX * dstPixelPitch;

    const TInt colorLinePitch = aSrcColorBitmap->iLinePitch;
    const TInt colorPixelPitch = PixelPitch(aSrcColorBitmap);
    TUint8* colorAddress = aSrcColorBitmap->iAddress;
    colorAddress += srcPoint.iY * colorLinePitch + srcPoint.iX * colorPixelPitch;

    // For the source bitmap, also calculate the pitch to the next address for
    // one line down and one pixel right in the destination bitmap.
    const TInt colorDstLinePitch = colorLinePitch * dvdy + colorPixelPitch * dudy;
    const TInt colorDstPixelPitch = colorLinePitch * dvdx + colorPixelPitch * dudx;

    //
    // Iterate over destination pixels.
    //
    const TInt width = aDstRect.Width();
    TInt h = aDstRect.Height();
    while (h-- > 0)
    {
        (*aBlitLineFn)(dstAddress, width, colorAddress, colorDstPixelPitch);
        dstAddress += dstLinePitch;
        colorAddress += colorDstLinePitch;
    }
}


extern void GenericMaskBlit
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TRect&                    aDstRect,       // must be clipped to destination
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TLcdTransform&            aTransform,     // includes anchor
    const TAcceleratedBitmapInfo*   aAlphaBitmap,   // source or destination alpha
    TBool                           aIsSrcAlpha,    // ETrue if alpha is source
    TMaskBlitLineFunction           aBlitLineFn     // function to call on each line
)
{
    ASSERT(aDstColorBitmap != NULL);
    ASSERT(aSrcColorBitmap != NULL);
    ASSERT(aAlphaBitmap != NULL);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;

    //
    // For each bitmap, calculate the starting address and byte offsets to the
    // next address for one line down and one pixel right.
    //
    const TInt dstLinePitch = aDstColorBitmap->iLinePitch;
    const TInt dstPixelPitch = PixelPitch(aDstColorBitmap);
    TUint8* dstAddress = aDstColorBitmap->iAddress;
    dstAddress += aDstRect.iTl.iY * dstLinePitch + aDstRect.iTl.iX * dstPixelPitch;

    const TInt colorLinePitch = aSrcColorBitmap->iLinePitch;
    const TInt colorPixelPitch = PixelPitch(aSrcColorBitmap);
    TUint8* colorAddress = aSrcColorBitmap->iAddress;
    colorAddress += srcPoint.iY * colorLinePitch + srcPoint.iX * colorPixelPitch;

    const TInt alphaLinePitch = aAlphaBitmap->iLinePitch;
    const TInt alphaPixelPitch = PixelPitch(aAlphaBitmap);
    TUint8* alphaAddress = aAlphaBitmap->iAddress;

    // Choose the start according to whether alpha/mask is source or destination
    TPoint alphaTl(aIsSrcAlpha ? srcPoint : aDstRect.iTl);
    alphaAddress += alphaTl.iY * alphaLinePitch + alphaTl.iX * alphaPixelPitch;

    // For the source and alpha bitmaps, also calculate the pitch to the next
    // address for one line down and one pixel right in the destination bitmap.
    const TInt colorDstLinePitch = colorLinePitch * dvdy + colorPixelPitch * dudy;
    const TInt colorDstPixelPitch = colorLinePitch * dvdx + colorPixelPitch * dudx;

    TInt alphaDstLinePitch;
    TInt alphaDstPixelPitch;
    if (aIsSrcAlpha)
    {
        alphaDstLinePitch = alphaLinePitch * dvdy + alphaPixelPitch * dudy;
        alphaDstPixelPitch = alphaLinePitch * dvdx + alphaPixelPitch * dudx;
    }
    else
    {
        alphaDstLinePitch = alphaLinePitch;
        alphaDstPixelPitch = alphaPixelPitch;
    }

    //
    // Iterate over destination pixels.
    //
    const TInt width = aDstRect.Width();
    TInt h = aDstRect.Height();
    while (h-- > 0)
    {
        (*aBlitLineFn)(dstAddress, width, colorAddress, colorDstPixelPitch,
                       alphaAddress, alphaDstPixelPitch);
        dstAddress += dstLinePitch;
        colorAddress += colorDstLinePitch;
        alphaAddress += alphaDstLinePitch;
    }
}


extern void BitBltOneBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

    TUint8* dstAddress = aDstColorBitmap->iAddress;
    TUint8* srcAddress = aSrcColorBitmap->iAddress;
    TInt dstLinePitch  = aDstColorBitmap->iLinePitch;
    TInt srcLinePitch  = aSrcColorBitmap->iLinePitch;

    TInt height = aDstRect.Height();
    TInt width  = aDstRect.Width();

    const TInt x0 = aDstRect.iTl.iX;
    const TInt x3 = aDstRect.iBr.iX;
    const TInt x2 = Max(x0, aDstRect.iBr.iX & ~7);
    const TInt x1 = Min(x3,(aDstRect.iTl.iX + 7) & ~7);

    for (; height>0; --height)
    {
        TUint8 accum;
        TInt x;

        u = u0;
        v = v0;

        x = x0;
        if (x < x1)
        {
            //
            // Left edge.
            //
            accum = dstAddress[x0>>3];
            for (; x<x1; x++)
            {
                TUint8 mb = 1<<(x&0x7);
                TUint8 sb = *(srcAddress + srcLinePitch*v + (u>>3));

                if (sb & (1<<(u&7)))
                {
                    accum |= mb;
                }
                else
                {
                    accum &= ~mb;
                }

                u += dudx;
                v += dvdx;
            }
            dstAddress[x0>>3] = accum;
        }

        //
        // Middle section.
        //
        for (; x<x2; x+=8)
        {
            ASSERT((x & 7) == 0);   // must be byte aligned
            accum = 0;
            for (TInt j=0; j<8; j++)
            {
                TUint8 sb = *(srcAddress + srcLinePitch*v + (u>>3));
                if (sb & (1<<(u&7)))
                {
                    accum |= (1<<j);
                }
                u += dudx;
                v += dvdx;
            }
            dstAddress[x>>3] = accum;
        }

        //
        // Right edge
        //
        if (x < x3)
        {
            accum = dstAddress[x>>3];
            for (; x<=x3; x++)
            {
                TUint8 mb = 1<<(x&0x7);
                TUint8 sb = *(srcAddress + srcLinePitch*v + (u>>3));

                if (sb & (1<<(u&7)))
                {
                    accum |= mb;
                }
                else
                {
                    accum &= ~mb;
                }

                u += dudx;
                v += dvdx;
            }
            dstAddress[x>>3] = accum;
        }
        u0 += dudy;
        v0 += dvdy;
        dstAddress += dstLinePitch;
    }
}


LOCAL_C void BlitLineEightBppSimple
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    ASSERT(aColorPixelPitch == sizeof(TUint8));
    UNUSED(aColorPixelPitch);   // ASSERT is the only use otherwise.

    Mem::Copy(aDstAddress, aColorAddress, aWidth);
}


extern void BitBltEightBppSimple
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineEightBppSimple);
}


LOCAL_C void BlitLineEightBpp
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint8* dst = aDstAddress;
    TUint8* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    while (dst < end)
    {
        *dst++ = *colorAddr;
        colorAddr += aColorPixelPitch;
    }
}


extern void BitBltEightBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineEightBpp);
}

