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

#include <e32std.h>
#include <gdi.h>
#include <graphicsaccelerator.h>
#include <j2me/jdebug.h>

#include "lcdgdrvif.h"
#include "lcdbitblt.h"
#include "lcd16bpp.h"
#include "lcdtransform.h"
#include "lcdgdrvutil.h"

#ifdef _DEBUG
inline TInt BPP(const TAcceleratedBitmapInfo* aBitmap)
{
    if (aBitmap->iDisplayMode == EColor16M)
    {
        return 24;
    }
    return (1<<aBitmap->iPixelShift);
}

inline TBool VALID(const TAcceleratedBitmapInfo* aDstBitmap, const TAcceleratedBitmapInfo* aSrcBitmap)
{
    if (aDstBitmap && aSrcBitmap)
    {
        return (NULL != aDstBitmap->iAddress) &&
               (NULL != aSrcBitmap->iAddress) &&
               (aDstBitmap->iDisplayMode == aSrcBitmap->iDisplayMode) &&
               (BPP(aDstBitmap)==16);
    }
    return EFalse;
}
#endif


extern void DrawRegion16Bpp1BppTo16BppOpaque
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,      // must be 16bpp mode
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,             // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,      // must match aDstColorBitmap mode
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // must be EGray2
    const TLcdTransform&          aTransform            // includes anchor
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);
    ASSERT(aSrcAlphaBitmap);
    ASSERT(aDstColorBitmap->iDisplayMode == aSrcColorBitmap->iDisplayMode);
    ASSERT(BPP(aDstColorBitmap) == 16);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray2);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;

    const TInt width = aDstRect.Width();

    const TInt srcLinePitch = aSrcColorBitmap->iLinePitch;
    const TInt mskLinePitch = aSrcAlphaBitmap->iLinePitch;
    const TInt dstLinePitch = aDstColorBitmap->iLinePitch;

    TUint8* srcAddress = aSrcColorBitmap->iAddress;
    TUint8* mskAddress = aSrcAlphaBitmap->iAddress; // 1 bpp
    TUint8* dstAddress = aDstColorBitmap->iAddress;

    TInt u;
    TInt v;
    TInt x = aDstRect.iTl.iX;
    TInt y = aDstRect.iTl.iY;
    TInt h = aDstRect.Height();

    //
    // Iterate over destination pixels.
    //
    dstAddress += (y*dstLinePitch + x*sizeof(TUint16));
    for (; h>0; --h)
    {
        TUint16* dst = (TUint16*)(dstAddress);
        TUint16* end = dst + width;
        u=u0;
        v=v0;
        while (dst < end)
        {
            TInt m = *(mskAddress + v*mskLinePitch + (u>>3));
            if (m & (1<<(u&0x7)))
            {
                TUint16 srcColor = *(TUint16*)(srcAddress + v*srcLinePitch + u*sizeof(TUint16));
                *dst = srcColor;
            }
            ++dst;
            u+=dudx;
            v+=dvdx;
        }
        u0+=dudy;
        v0+=dvdy;
        dstAddress += dstLinePitch;
    }
}


#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
LOCAL_C void BlitLine16Bpp16BppTo16BppOpaque
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint16* dst = (TUint16*)(aDstAddress);
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* maskAddr = aMaskAddress;

    while (dst < end)
    {
        TUint16 srcMask = *(TUint16*)maskAddr;
        TUint16 srcColor = *(TUint16*)colorAddr;
        TUint16 dstColor = *dst;

        dstColor = (srcColor & srcMask) | (dstColor & ~srcMask);

        *dst++ = dstColor;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}


extern void DrawRegion16Bpp16BppTo16BppOpaque
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,             // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // mask
    const TLcdTransform&          aTransform            // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == aSrcColorBitmap->iDisplayMode);
    ASSERT(aSrcColorBitmap->iDisplayMode == aSrcAlphaBitmap->iDisplayMode);
    ASSERT(BPP(aDstColorBitmap) == 16);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLine16Bpp16BppTo16BppOpaque);
}
#endif


extern void DrawImage16Bpp1BppTo16BppOpaque
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,             // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // mask
    const TLcdTransform&          aTransform            // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == aSrcColorBitmap->iDisplayMode);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray2);
    ASSERT(BPP(aDstColorBitmap) == 16);

    ASSERT(aTransform.iDuDx == 1);
    ASSERT(aTransform.iDuDy == 0);
    ASSERT(aTransform.iDvDx == 0);
    ASSERT(aTransform.iDvDy == 1);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    ASSERT(srcPoint.iX == (aDstRect.iTl.iX + aTransform.iU0));
    ASSERT(srcPoint.iY == (aDstRect.iTl.iY + aTransform.iV0));

    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u = u0;
    TInt v = v0;
    TInt mu;    // mask u coord
    TInt m = 0; // mask byte

    const TInt width = aDstRect.Width();

    const TInt srcLinePitch = aSrcColorBitmap->iLinePitch;
    const TInt mskLinePitch = aSrcAlphaBitmap->iLinePitch;
    const TInt dstLinePitch = aDstColorBitmap->iLinePitch;

    TUint8* srcAddress = aSrcColorBitmap->iAddress;
    TUint8* mskAddress = aSrcAlphaBitmap->iAddress; // 1 bpp
    TUint8* dstAddress = aDstColorBitmap->iAddress;

    TInt x = aDstRect.iTl.iX;
    TInt y = aDstRect.iTl.iY;
    TInt h = aDstRect.Height();

#ifdef _DEBUG
    const TSize   srcSize  = aSrcColorBitmap->iSize;
    const TUint16* srcStart = (TUint16*)srcAddress;
    const TUint16* srcLimit = (TUint16*)(srcAddress + (srcSize.iHeight-1)*srcLinePitch + srcSize.iWidth*sizeof(TUint16));
#endif

    //
    // Start address of destination pixels.
    //
    dstAddress += ((y*dstLinePitch) + (x*sizeof(TUint16)));

    //
    // Start address of source pixels
    //
    srcAddress += ((v*srcLinePitch) + (u*sizeof(TUint16)));
    mskAddress += ((v*mskLinePitch) + (u>>3));

    for (; h>0; --h)
    {
        TUint16* src = (TUint16*)srcAddress;
        TUint8*  msk = mskAddress;
        TUint16* dst = (TUint16*)dstAddress;
        TUint16* end = dst + width;
        u=u0;
        mu=-1;
        while (dst < end)
        {
#ifdef _DEBUG
            ASSERT((srcStart <= src) && (src <= srcLimit));
#endif
            if (mu != (u>>3))
            {
                mu = (u>>3);
                m  = *msk++;
            }
            if (m & (1<<(u&0x7)))
            {
                *dst = *src;
            }
            ++src;
            ++dst;
            ++u;
        }
        dstAddress += dstLinePitch;
        srcAddress += srcLinePitch;
        mskAddress += mskLinePitch;
    }
}

LOCAL_C void FillTriangle16Bpp64
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor16Bpp,
    const TRect& aClipRect
)
{
    TRect clipRect(aBitmap->iSize);
    clipRect.Intersection(aClipRect);
    const TInt64 clipX1 = (TInt64)(clipRect.iTl.iX);
    const TInt64 clipY1 = (TInt64)(clipRect.iTl.iY);
    const TInt64 clipX2 = (TInt64)(clipRect.iBr.iX);
    const TInt64 clipY2 = (TInt64)(clipRect.iBr.iY);

    TInt i;

    TInt ymin=0;
    TInt ymax=0;
    TInt xmin=0;
    TInt xmax=0;

    for (i=1; i<3; i++)
    {
        if (aPoints[i].iX < aPoints[xmin].iX) xmin = i;
        if (aPoints[i].iX > aPoints[xmax].iX) xmax = i;
        if (aPoints[i].iY < aPoints[ymin].iY) ymin = i;
        if (aPoints[i].iY > aPoints[ymax].iY) ymax = i;
    }
    const TInt ymid = 3 - (ymax + ymin);

//  const TInt64 minX = aPoints[xmin].iX;
//  const TInt64 maxX = aPoints[xmax].iX;

    TEdge64 edges[3];

    //
    // Long edge top to bottom.
    //
    edges[0].iDy = (TInt64)(aPoints[ymax].iY) - (TInt64)(aPoints[ymin].iY);
    if (edges[0].iDy == 0)
    {
        return;
    }
    edges[0].iDx = (TInt64)(aPoints[ymax].iX) - (TInt64)(aPoints[ymin].iX);
    edges[0].iX  = (TInt64)(aPoints[ymin].iX);
    edges[0].iF  = 0;
    edges[0].iD  = edges[0].iDy;
    edges[0].iDe = Sign(edges[0].iDx);
    Increment(edges[0].iIx, edges[0].iIf, edges[0].iDx, edges[0].iDy);
    ASSERT(edges[0].iDy >= 0);

    //
    // Short top edge, or flat top
    //
    edges[1].iDx = (TInt64)(aPoints[ymid].iX) - (TInt64)(aPoints[ymin].iX);
    edges[1].iDy = (TInt64)(aPoints[ymid].iY) - (TInt64)(aPoints[ymin].iY);
    edges[1].iX  = (TInt64)(aPoints[ymin].iX);
    edges[1].iF  = 0;
    edges[1].iD  = edges[1].iDy;
    edges[1].iDe = Sign(edges[1].iDx);
    ASSERT(edges[1].iDy >= 0);

    //
    // Short bottom edge or flat bottom
    //
    edges[2].iDx = (TInt64)(aPoints[ymax].iX) - (TInt64)(aPoints[ymid].iX);
    edges[2].iDy = (TInt64)(aPoints[ymax].iY) - (TInt64)(aPoints[ymid].iY);
    edges[2].iX  = (TInt64)(aPoints[ymid].iX);
    edges[2].iF  = 0;
    edges[2].iD  = edges[2].iDy;
    edges[2].iDe = Sign(edges[2].iDx);
    ASSERT(edges[2].iDy >= 0);

    const TBool flatTop    = (edges[1].iDy == 0);
    const TBool flatBottom = (edges[2].iDy == 0);

    //
    // work out whether long edge is left most or right most.
    //
    // compare slopes of long edge and top edge.
    //
    TBool longEdgeRight;
    if (flatTop)
    {
        longEdgeRight = (aPoints[ymid].iX < aPoints[ymin].iX);
    }
    else
    {
        longEdgeRight = (edges[0].iDx*edges[1].iDy) > (edges[1].iDx*edges[0].iDy);
    }

    TInt64   y    = (TInt64)aPoints[ymin].iY;
    TInt64   yEnd = Min(aPoints[ymid].iY, clipY2);
    TEdge64* el   = NULL;
    TEdge64* er   = NULL;
    TInt64   m;     // number of lines to skip
    TInt64   x1;    // start of clipped scan (inclusive)
    TInt64   x2;    // end of clipped scan (exclusive), exclusive overrides inclusive

    TInt64 eliX;    // Locals used to speed access to edge information in loops.
    TInt64 eliF;
    TInt64 eliD;
    TInt64 eriX;
    TInt64 eriF;
    TInt64 eriD;

    TUint8* base = aBitmap->iAddress;
    TUint8* line = NULL;
    TInt linePitch = aBitmap->iLinePitch;
    TUint16* dst;
    TUint16* end;
    TUint32 color16 = (TUint32)aColor16Bpp;

    //
    // render top half.
    //
    if (!flatTop)
    {
        Increment(edges[1].iIx, edges[1].iIf, edges[1].iDx, edges[1].iDy);
        if (longEdgeRight)
        {
            er = &edges[0];
            el = &edges[1];
        }
        else
        {
            er = &edges[1];
            el = &edges[0];
        }
        eliX = el->iX;
        eliF = el->iF;
        eliD = el->iD;
        eriX = er->iX;
        eriF = er->iF;
        eriD = er->iD;

        //
        // Clip to top of clip rect.
        //
        if (y < clipY1)
        {
            TInt64 yTop = Min(clipY1, yEnd);

            m = (yTop - y);

            double leftSlope = (double)(el->iDx)/(double)(el->iDy);

            eliX += (leftSlope * ((double)m));

            double rightSlope = (double)(er->iDx)/(double)(er->iDy);

            eriX += (rightSlope * ((double)m));
            y = yTop;
        }

        ASSERT(((1 - eriD) <= eriF) && (eriF <= 0));
        ASSERT(((1 - eliD) <= eliF) && (eliF <= 0));

#ifdef PLOT_TOP_VERTEX
        // special case top vertex - can produce pimple so prob best avoided.
        if (aClipRect.Contains(aPoints[ymin]))
        {
            line = base + aPoints[ymin].iY*linePitch;
            ((TUint16*)line)[aPoints[ymin].iX] = color16;
        }
#endif

        line = base + y*linePitch;
        for (; y<yEnd; y++)
        {
            ASSERT(eliX <= eriX);
            if ((eliX < clipX2) && (eriX > clipX1))
            {
                x1 = Max(eliX, clipX1); // inclusive (ceil)
                x2 = Min(eriX, clipX2); // exclusive (floor)

                dst = (TUint16*)(line + x1*sizeof(TUint16));
                end = (TUint16*)(line + x2*sizeof(TUint16));
                while (dst < end)
                {
                    *dst++ = color16;
                }
            }
            eliX += el->iIx;
            eliF += el->iIf;
            if (eliF > 0)
            {
                eliX ++;
                eliF -= eliD;
            }

            eriX += er->iIx;
            eriF += er->iIf;
            if (eriF > 0)
            {
                eriX ++;
                eriF -= eriD;
            }

            line += linePitch;
        }

        el->iX = eliX;
        el->iF = eliF;
        el->iD = eliD;
        er->iX = eriX;
        er->iF = eriF;
        er->iD = eriD;
    }

    if (!flatBottom)
    {
        Increment(edges[2].iIx, edges[2].iIf, edges[2].iDx, edges[2].iDy);
        if (longEdgeRight)
        {
            // initialize short left edge
            er = &edges[0];
            el = &edges[2];
        }
        else
        {
            // initialize short right edge
            er = &edges[2];
            el = &edges[0];
        }
        eliX = el->iX;
        eliF = el->iF;
        eliD = el->iD;
        eriX = er->iX;
        eriF = er->iF;
        eriD = er->iD;

        //
        // clip to top edge of clip rect.
        //
        //
        // Clip to top of clip rect.
        //
        if (y < clipY1)
        {
            m = (clipY1 - y);

            double leftSlope = (double)(el->iDx)/(double)(el->iDy);

            eliX += (leftSlope * m);

            double rightSlope = (double)(er->iDx)/(double)(er->iDy);

            eriX += (rightSlope * m);

            y = clipY1;
        }

        line = base + y*linePitch;
        yEnd = Min(aPoints[ymax].iY, clipY2);
        for (; y<yEnd; y++)     // ignore last line - as pixel will be on bottom edge.
        {
            ASSERT(eliX <= eriX);
            if ((eliX < clipX2) && (eriX > clipX1))
            {
                x1 = Max(eliX, clipX1); // inclusive (ceil)
                x2 = Min(eriX, clipX2); // exclusive (floor)

                dst = (TUint16*)(line + x1*sizeof(TUint16));
                end = (TUint16*)(line + x2*sizeof(TUint16));
                while (dst < end)
                {
                    *dst++ = color16;
                }
            }

            eliX += el->iIx;
            eliF += el->iIf;
            if (eliF > 0)
            {
                eliX ++;
                eliF -= eliD;
            }

            eriX += er->iIx;
            eriF += er->iIf;
            if (eriF > 0)
            {
                eriX ++;
                eriF -= eriD;
            }

            line += linePitch;
        }

        el->iX = eliX;
        el->iF = eliF;
        el->iD = eliD;
        er->iX = eriX;
        er->iF = eriF;
        er->iD = eriD;
    }
}

extern void FillTriangle16Bpp32
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor16Bpp,
    const TRect& aClipRect
)
{
    TRect clipRect(aBitmap->iSize);
    clipRect.Intersection(aClipRect);
    const TInt clipX1 = clipRect.iTl.iX;
    const TInt clipY1 = clipRect.iTl.iY;
    const TInt clipX2 = clipRect.iBr.iX;
    const TInt clipY2 = clipRect.iBr.iY;

    TInt i;

    TInt ymin=0;
    TInt ymax=0;
    TInt xmin=0;
    TInt xmax=0;

    for (i=1; i<3; i++)
    {
        if (aPoints[i].iX < aPoints[xmin].iX) xmin = i;
        if (aPoints[i].iX > aPoints[xmax].iX) xmax = i;
        if (aPoints[i].iY < aPoints[ymin].iY) ymin = i;
        if (aPoints[i].iY > aPoints[ymax].iY) ymax = i;
    }
    const TInt ymid = 3 - (ymax + ymin);


    TEdge edges[3];

    //
    // Long edge top to bottom.
    //
    edges[0].iDy = aPoints[ymax].iY - aPoints[ymin].iY;
    if (edges[0].iDy == 0)
    {
        return;
    }
    edges[0].iDx = aPoints[ymax].iX - aPoints[ymin].iX;
    edges[0].iX  = aPoints[ymin].iX;
    edges[0].iF  = 0;
    edges[0].iD  = edges[0].iDy;
    edges[0].iDe = Sign(edges[0].iDx);
    Increment(edges[0].iIx, edges[0].iIf, edges[0].iDx, edges[0].iDy);
    ASSERT(edges[0].iDy >= 0);

    //
    // Short top edge, or flat top
    //
    edges[1].iDx = aPoints[ymid].iX - aPoints[ymin].iX;
    edges[1].iDy = aPoints[ymid].iY - aPoints[ymin].iY;
    edges[1].iX  = aPoints[ymin].iX;
    edges[1].iF  = 0;
    edges[1].iD  = edges[1].iDy;
    edges[1].iDe = Sign(edges[1].iDx);
    ASSERT(edges[1].iDy >= 0);

    //
    // Short bottom edge or flat bottom
    //
    edges[2].iDx = aPoints[ymax].iX - aPoints[ymid].iX;
    edges[2].iDy = aPoints[ymax].iY - aPoints[ymid].iY;
    edges[2].iX  = aPoints[ymid].iX;
    edges[2].iF  = 0;
    edges[2].iD  = edges[2].iDy;
    edges[2].iDe = Sign(edges[2].iDx);
    ASSERT(edges[2].iDy >= 0);

    const TBool flatTop    = (edges[1].iDy == 0);
    const TBool flatBottom = (edges[2].iDy == 0);

    //
    // work out whether long edge is left most or right most.
    //
    // compare slopes of long edge and top edge.
    //
    TBool longEdgeRight;
    if (flatTop)
    {
        longEdgeRight = (aPoints[ymid].iX < aPoints[ymin].iX);
    }
    else
    {
        longEdgeRight = (edges[0].iDx*edges[1].iDy) > (edges[1].iDx*edges[0].iDy);
    }

    TInt y    = aPoints[ymin].iY;
    TInt yEnd = Min(aPoints[ymid].iY, clipY2);
    TEdge* el = NULL;
    TEdge* er = NULL;
    TInt m;     // number of lines to skip
    TInt x1;    // start of clipped scan (inclusive)
    TInt x2;    // end of clipped scan (exclusive), exclusive overrides inclusive

    TInt eliX;  // Locals used to speed access to edge information in loops.
    TInt eliF;
    TInt eliD;
    TInt eriX;
    TInt eriF;
    TInt eriD;


    TUint8* base = aBitmap->iAddress;
    TUint8* line = NULL;
    TInt linePitch = aBitmap->iLinePitch;
    TUint16* dst;
    TUint16* end;
    TUint16 color16 = (TUint16)aColor16Bpp;

    //
    // render top half.
    //
    if (!flatTop)
    {
        Increment(edges[1].iIx, edges[1].iIf, edges[1].iDx, edges[1].iDy);
        if (longEdgeRight)
        {
            er = &edges[0];
            el = &edges[1];
        }
        else
        {
            er = &edges[1];
            el = &edges[0];
        }
        eliX = el->iX;
        eliF = el->iF;
        eliD = el->iD;
        eriX = er->iX;
        eriF = er->iF;
        eriD = er->iD;

        //
        // Clip to top of clip rect.
        //
        if (y < clipY1)
        {
            TInt yTop = Min(clipY1, yEnd);

            m = (yTop - y);

            eliX += m*el->iIx;
            eliF += m*el->iIf;
            while (eliF > 0)
            {
                eliX ++;
                eliF -= eliD;
            }

            eriX += m*er->iIx;
            eriF += m*er->iIf;
            while (eriF > 0)
            {
                eriX ++;
                eriF -= eriD;
            }

            y = yTop;
        }

        ASSERT(((1 - eriD) <= eriF) && (eriF <= 0));
        ASSERT(((1 - eliD) <= eliF) && (eliF <= 0));

#ifdef PLOT_TOP_VERTEX
        // special case top vertex - can produce pimple so prob best avoided.
        if (aClipRect.Contains(aPoints[ymin]))
        {
            line = base + aPoints[ymin].iY*linePitch;
            ((TUint16*)line)[aPoints[ymin].iX] = color16;
        }
#endif

        line = base + y*linePitch;
        for (; y<yEnd; y++)
        {
            ASSERT(eliX <= eriX);
            if ((eliX < clipX2) && (eriX > clipX1))
            {
                x1 = Max(eliX, clipX1); // inclusive (ceil)
                x2 = Min(eriX, clipX2); // exclusive (floor)

                dst = (TUint16*)(line + x1*sizeof(TUint16));
                end = (TUint16*)(line + x2*sizeof(TUint16));
                while (dst < end)
                {
                    *dst++ = color16;
                }
            }

            eliX += el->iIx;
            eliF += el->iIf;
            if (eliF > 0)
            {
                eliX ++;
                eliF -= eliD;
            }

            eriX += er->iIx;
            eriF += er->iIf;
            if (eriF > 0)
            {
                eriX ++;
                eriF -= eriD;
            }

            line += linePitch;
        }

        el->iX = eliX;
        el->iF = eliF;
        el->iD = eliD;
        er->iX = eriX;
        er->iF = eriF;
        er->iD = eriD;
    }

    if (!flatBottom)
    {
        Increment(edges[2].iIx, edges[2].iIf, edges[2].iDx, edges[2].iDy);
        if (longEdgeRight)
        {
            // initialize short left edge
            er = &edges[0];
            el = &edges[2];
        }
        else
        {
            // initialize short right edge
            er = &edges[2];
            el = &edges[0];
        }
        eliX = el->iX;
        eliF = el->iF;
        eliD = el->iD;
        eriX = er->iX;
        eriF = er->iF;
        eriD = er->iD;

        //
        // clip to top edge of clip rect.
        //
        //
        // Clip to top of clip rect.
        //
        if (y < clipY1)
        {
            m = (clipY1 - y);

            eliX += m*el->iIx;
            eliF += m*el->iIf;
            while (eliF > 0)
            {
                eliX ++;
                eliF -= eliD;
            }

            eriX += m*er->iIx;
            eriF += m*er->iIf;
            while (eriF > 0)
            {
                eriX ++;
                eriF -= eriD;
            }
            y = clipY1;
        }

        line = base + y*linePitch;
        yEnd = Min(aPoints[ymax].iY, clipY2);
        for (; y<yEnd; y++)     // ignore last line - as pixel will be on bottom edge.
        {
            ASSERT(eliX <= eriX);
            if ((eliX < clipX2) && (eriX > clipX1))
            {
                x1 = Max(eliX, clipX1); // inclusive (ceil)
                x2 = Min(eriX, clipX2); // exclusive (floor)

                dst = (TUint16*)(line + x1*sizeof(TUint16));
                end = (TUint16*)(line + x2*sizeof(TUint16));
                while (dst < end)
                {
                    *dst++ = color16;
                }
            }

            eliX += el->iIx;
            eliF += el->iIf;
            if (eliF > 0)
            {
                eliX ++;
                eliF -= eliD;
            }

            eriX += er->iIx;
            eriF += er->iIf;
            if (eriF > 0)
            {
                eriX ++;
                eriF -= eriD;
            }

            line += linePitch;
        }

        el->iX = eliX;
        el->iF = eliF;
        el->iD = eliD;
        er->iX = eriX;
        er->iF = eriF;
        er->iD = eriD;
    }
}

LOCAL_C TBool IsPathologicalTriangle(const TPoint aPoints[3])
{
    TInt minX = aPoints[0].iX;
    TInt maxX = aPoints[0].iX;
    TInt minY = aPoints[0].iY;
    TInt maxY = aPoints[0].iY;

    for (TInt i = 1; i <= 2; i++)
    {
        TInt x = aPoints[i].iX;
        TInt y = aPoints[i].iY;

        if (x < minX)
        {
            minX = x;
        }
        if (x > maxX)
        {
            maxX = x;
        }
        if (y < minY)
        {
            minY = y;
        }
        if (y > maxY)
        {
            maxY = y;
        }
    }
    if (minX < 0)
    {
        if (maxX >= (0x7FFFFFFF + minX))
        {
            return ETrue;
        }
        if (-minX > 0xFFFFF)
        {
            return ETrue;
        }
    }
    if (maxX > 0)
    {
        if ((maxX == 0x7FFFFFFF) && (minX == 0))
        {
            return ETrue;
        }
        if (maxX > 0xFFFFF)
        {
            return ETrue;
        }
    }
    if (minY < 0)
    {
        if (maxY >= (0x7FFFFFFF + minY))
        {
            return ETrue;
        }
        if (-minY > 0xFFFFF)
        {
            return ETrue;
        }
    }
    if (maxY > 0)
    {
        if ((maxY == 0x7FFFFFFF) && (minY == 0))
        {
            return ETrue;
        }
        if (maxY > 0xFFFFF)
        {
            return ETrue;
        }
    }
    return EFalse;
}

extern void FillTriangle16Bpp
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor16Bpp,
    const TRect& aClipRect
)
{
    if (!IsPathologicalTriangle(aPoints))
    {
        FillTriangle16Bpp32(aBitmap, aPoints, aColor16Bpp, aClipRect);
    }
    else
    {
        FillTriangle16Bpp64(aBitmap, aPoints, aColor16Bpp, aClipRect);
    }
}

LOCAL_C void BlitLineSixteenBppSimple
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    DEBUG_ONLY(aColorPixelPitch)
)
{
    ASSERT(aColorPixelPitch == sizeof(TUint16));
#ifdef USE_MEM_COPY_FOR_SIMPLE_BLIT
    Mem::Copy(aDstAddress, aColorAddress, aWidth * sizeof(TUint16));
#else
    TUint16* dst = (TUint16*)aDstAddress;
    TUint16* src = (TUint16*)aColorAddress;
    TUint16* end = dst + aWidth;
    while (dst < end)
    {
        *dst++ = *src++;
    }
#endif
}


/**
 * Opaque 16bpp to 16bpp bitblt with simple transform.
 */
extern void BitBltSixteenBppSimple
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    //
    // Translation or translation & vertical reflection case.
    //
    ASSERT(aTransform.iDuDx == 1);
    ASSERT(aTransform.iDuDy == 0);
    ASSERT(aTransform.iDvDx == 0);
    ASSERT(aTransform.iDvDy == 1 || aTransform.iDvDy == -1);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineSixteenBppSimple);
}


LOCAL_C void BlitLineSixteenBpp
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint16* dst = (TUint16*)aDstAddress;
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    while (dst < end)
    {
        *dst++ = *(TUint16*)colorAddr;
        colorAddr += aColorPixelPitch;
    }
}


/**
 * Opaque 16bpp to 16bpp bitblt with transform.
 */
extern void BitBltSixteenBpp
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
                BlitLineSixteenBpp);
}

extern void BitBltSixteenBppOneBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    BitBltSixteenBpp(aDstColorBitmap, NULL, aDstRect, aSrcColorBitmap, NULL, aTransform);
    BitBltOneBpp(aDstAlphaBitmap, NULL, aDstRect, aSrcAlphaBitmap, NULL, aTransform);
}

extern void BitBltSixteenBppSixteenBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    BitBltSixteenBpp(aDstColorBitmap, NULL, aDstRect, aSrcColorBitmap, NULL, aTransform);
    BitBltSixteenBpp(aDstAlphaBitmap, NULL, aDstRect, aSrcAlphaBitmap, NULL, aTransform);
}

extern void BitBltSixteenBppEightBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    BitBltSixteenBpp(aDstColorBitmap, NULL, aDstRect, aSrcColorBitmap, NULL, aTransform);
    BitBltEightBpp(aDstAlphaBitmap, NULL, aDstRect, aSrcAlphaBitmap, NULL, aTransform);
}

