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
#include <graphicsaccelerator.h>
#include <j2me/jdebug.h>

#include "lcd32bpp.h"
#include "lcdgdrvutil.h"
#include "lcdtransform.h"
#include "lcdbitblt.h"

LOCAL_C void FillTriangle32Bpp32
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor32Bpp,
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
    TUint32* dst;
    TUint32* end;
    TUint32 color32 = (TUint32)aColor32Bpp;

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
            ((TUint32*)line)[aPoints[ymin].iX] = color32;
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

                dst = (TUint32*)(line + x1*sizeof(TUint32));
                end = (TUint32*)(line + x2*sizeof(TUint32));
                while (dst < end)
                {
                    *dst++ = color32;
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

                dst = (TUint32*)(line + x1*sizeof(TUint32));
                end = (TUint32*)(line + x2*sizeof(TUint32));
                while (dst < end)
                {
                    *dst++ = color32;
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

LOCAL_C void FillTriangle32Bpp64
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor32Bpp,
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

    // const TInt64 minX = aPoints[xmin].iX;
    // const TInt64 maxX = aPoints[xmax].iX;

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
    TUint32* dst;
    TUint32* end;
    TUint32 color32 = (TUint32)aColor32Bpp;

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
            ((TUint32*)line)[aPoints[ymin].iX] = color32;
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

                dst = (TUint32*)(line + x1*sizeof(TUint32));
                end = (TUint32*)(line + x2*sizeof(TUint32));
                while (dst < end)
                {
                    *dst++ = color32;
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

                dst = (TUint32*)(line + x1*sizeof(TUint32));
                end = (TUint32*)(line + x2*sizeof(TUint32));
                while (dst < end)
                {
                    *dst++ = color32;
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
    if (minY > 0)
    {
        if ((maxY == 0x7FFFFFFF) && (minX == 0))
        {
            return ETrue;
        }
        if (minY > 0xFFFFF)
        {
            return ETrue;
        }
    }
    return EFalse;
}

extern void FillTriangle32Bpp
(
    const TAcceleratedBitmapInfo* aBitmap,
    const TPoint aPoints[],
    TUint32 aColor32Bpp,
    const TRect& aClipRect
)
{
    if (!IsPathologicalTriangle(aPoints))
    {
        FillTriangle32Bpp32(aBitmap, aPoints, aColor32Bpp, aClipRect);
    }
    else
    {
        FillTriangle32Bpp64(aBitmap, aPoints, aColor32Bpp, aClipRect);
    }
}

LOCAL_C void BlitLineThirtyTwoBppSimple
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    DEBUG_ONLY(aColorPixelPitch)
)
{
    ASSERT(aColorPixelPitch == sizeof(TUint32));
    Mem::Move(aDstAddress, aColorAddress, aWidth * sizeof(TUint32));
}


extern void BitBltThirtyTwoBppSimple
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
                BlitLineThirtyTwoBppSimple);
}


LOCAL_C void BlitLineThirtyTwoBpp
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint32* dst = (TUint32*)aDstAddress;
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    while (dst < end)
    {
        *dst++ = *(TUint32*)colorAddr;
        colorAddr += aColorPixelPitch;
    }
}


extern void BitBltThirtyTwoBpp
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
                BlitLineThirtyTwoBpp);
}

extern void BitBltThirtyTwoBppEightBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    BitBltThirtyTwoBpp(aDstColorBitmap, NULL, aDstRect, aSrcColorBitmap, NULL, aTransform);
    BitBltEightBpp(aDstAlphaBitmap, NULL, aDstRect, aSrcAlphaBitmap, NULL, aTransform);
}

extern void BitBltThirtyTwoBppOneBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    BitBltThirtyTwoBpp(aDstColorBitmap, NULL, aDstRect, aSrcColorBitmap, NULL, aTransform);
    BitBltOneBpp(aDstAlphaBitmap, NULL, aDstRect, aSrcAlphaBitmap, NULL, aTransform);
}

extern void BitBltThirtyTwoBppThirtyTwoBpp
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    BitBltThirtyTwoBpp(aDstColorBitmap, NULL, aDstRect, aSrcColorBitmap, NULL, aTransform);
    BitBltThirtyTwoBpp(aDstAlphaBitmap, NULL, aDstRect, aSrcAlphaBitmap, NULL, aTransform);
}
