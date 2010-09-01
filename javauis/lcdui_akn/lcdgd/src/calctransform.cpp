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

#include <lcdgdrv.h>
#include "lcdtransform.h"
#include "calctransform.h"

//
// Compute TLcdTransform mapping aSrcRect to aDstRect with aSrcTransform
//
// returns source to target transform
//
EXPORT_C TLcdTransform CalcTransform
(
    const TRect& aDstRect,          // unclipped
    const TRect& aSrcRect,          // unclipped
    TTransformType aSrcTransform
)
{
    //
    // Transform calculates the TLcdTransform which transforms a rectangle of
    // size aSrcRect.Size(), positioned with its top left corner at the origin,
    // such that the top left corner of the transformed rectangle also lies at
    // the origin.
    //
    TLcdTransform transform = ::Transform(aSrcTransform, aSrcRect.Size());

    //
    // compose transform at origin with src and dst translations.
    //
    transform = Translate(aDstRect.iTl) * transform * Translate(-aSrcRect.iTl);

    //
    // Currently do not support scaling transforms. The following check will
    // catch an attempt to specify a destination rectangle that is not the
    // same size as the transformed source rectangle.
    //
    ASSERT(CheckTransform(aDstRect, aSrcRect, transform));

    return transform;
}


LOCAL_C TRect TransformRect(const TLcdTransform& aTransform, const TRect& aRect)
{
    TRect rect;
    if ((aRect.iBr.iX >= aRect.iTl.iX) && (aRect.iBr.iY >= aRect.iTl.iY))
    {
        //
        // Forward propagate clipped rectangle corners to destination coords.
        // We have to use inclusive-inclusive coords and sort the transformed
        // points to produce a destination rectangle.
        //
        TPoint topLeft     = aRect.iTl;
        TPoint bottomRight = aRect.iBr;
        bottomRight.iX--;   // convert to inclusive coords
        bottomRight.iY--;

        //
        // Transform the points.
        //
        topLeft = aTransform(topLeft);
        bottomRight = aTransform(bottomRight);

        //
        // Now find min and max coords - still inclusive
        //
        TInt xMin = Min(topLeft.iX, bottomRight.iX);
        TInt xMax = Max(topLeft.iX, bottomRight.iX);
        TInt yMin = Min(topLeft.iY, bottomRight.iY);
        TInt yMax = Max(topLeft.iY, bottomRight.iY);

        rect.iTl.iX = xMin;
        rect.iTl.iY = yMin;
        rect.iBr.iX = xMax + 1; // exclusive
        rect.iBr.iY = yMax + 1; // exclusive
    }
    else
    {
        rect.iTl = aTransform(aRect.iTl);
        rect.iBr = rect.iTl;    // exclusive
        ASSERT(rect.IsEmpty());
    }

    return rect;
}

/**
 * Calculate clipped source and destination rectangles for
 * src to dst transform aSrcTransform. This function calculates
 * the largest corresponding subrectangles of aDstRect and aSrcRect
 * that also lie within aDstClipRect and aSrcClipRect respectively.
 */
EXPORT_C void ClipTransformRect
(
    TRect& aDstRect,                    // in/out
    TRect& aSrcRect,                    // in/out
    const TRect& aDstClipRect,          // input
    const TRect& aSrcClipRect,          // input
    TLcdTransform& aSrcTransform        // src to target transform
)
{
    TRect transRect;

    TLcdTransform& forward = aSrcTransform;
    TLcdTransform  reverse = forward.Inverse();

    //
    // Clip source rectangle.
    //
    aSrcRect.Intersection(aSrcClipRect);

    //
    // Clip destination rectangle.
    //
    aDstRect.Intersection(aDstClipRect);

    //
    // Transform clipped source rectangle to destination coords.
    //
    transRect = TransformRect(forward, aSrcRect);

    //
    // Clip againts destination rectangle.
    //
    aDstRect.Intersection(transRect);

    //
    // Reverse transform rectangle clipped against destination clip rect to source coords
    //
    aSrcRect = TransformRect(reverse, aDstRect);
}

//
// Clip source and dst rects against source and dst clip rects, maintaining aspect ratio
//
EXPORT_C void ClipRects(TRect& aDstRect, const TRect& aDstClip, TRect& aSrcRect, const TRect& aSrcClip)
{
    TPoint vector = aDstRect.iTl - aSrcRect.iTl;

    aDstRect.Intersection(aDstClip);
    aSrcRect.Intersection(aSrcClip);

    TRect srcRect(aDstRect.iTl - vector, aDstRect.Size());
    aSrcRect.Intersection(srcRect);

    aDstRect = aSrcRect;
    aDstRect.Move(vector);

    ASSERT(aDstRect.Size() == aSrcRect.Size());
}


#ifdef _DEBUG
void TestDbgCheckTransform()
{
    TLcdTransform transform = Translate(TPoint(-1,1));

    TRect srcRect(TPoint(1,0), TSize(1,2));
    TRect dstRect(TPoint(0,0), TSize(1,2));

    ASSERT(CheckTransform(dstRect, srcRect, transform));
}
#endif

EXPORT_C TBool CheckTransform(const TRect& aDstRect, const TRect& aSrcRect, const TLcdTransform aTransform)
{
    TPoint dst[4];
    TPoint src[4];
    const TPoint* dstPtr[4];

    TInt x1,y1,x2,y2;

    x1=aDstRect.iTl.iX;
    y1=aDstRect.iTl.iY;
    x2=aDstRect.iBr.iX-1;
    y2=aDstRect.iBr.iY-1;

    dst[0].iX = x1;
    dst[0].iY = y1;

    dst[1].iX = x2;
    dst[1].iY = y1;

    dst[2].iX = x1;
    dst[2].iY = y2;

    dst[3].iX = x2;
    dst[3].iY = y2;

    x1=aSrcRect.iTl.iX;
    y1=aSrcRect.iTl.iY;
    x2=aSrcRect.iBr.iX-1;
    y2=aSrcRect.iBr.iY-1;

    src[0].iX = x1;
    src[0].iY = y1;

    src[1].iX = x2;
    src[1].iY = y1;

    src[2].iX = x1;
    src[2].iY = y2;

    src[3].iX = x2;
    src[3].iY = y2;

    dstPtr[0] = &dst[0];
    dstPtr[1] = &dst[1];
    dstPtr[2] = &dst[2];
    dstPtr[3] = &dst[3];

    TPoint p;

    TBool f=ETrue;
    for (TInt i=0; i<4 && f; i++)
    {
        f = EFalse;
        p = aTransform(src[i]);
        for (TInt j=4; --j>=0;)
        {
            if (dstPtr[j] && (*dstPtr[j] == p))
            {
                dstPtr[j] = NULL;
                f = ETrue;
                break;
            }
        }
        ASSERT(f);
    }

    return f;
}

EXPORT_C TBool CheckBounds(const TSize& aDstSize, const TSize& aSrcSize, const TRect& aDstRect, const TLcdTransform& aTransform)
{
    if (!aDstRect.IsEmpty())
    {
        TPoint p1 = aDstRect.iTl;
        TPoint p2 = aDstRect.iBr;

        p2.iX--;
        p2.iY--;

        TRect dstClipRect(aDstSize);

        ASSERT(dstClipRect.Contains(p1));
        if (!dstClipRect.Contains(p1))
        {
            return EFalse;
        }

        ASSERT(dstClipRect.Contains(p2));
        if (!dstClipRect.Contains(p2))
        {
            return EFalse;
        }

        p1 = aTransform(p1);
        p2 = aTransform(p2);

        TRect srcClipRect(aSrcSize);

        ASSERT(srcClipRect.Contains(p1));
        if (!srcClipRect.Contains(p1))
        {
            return EFalse;
        }

        ASSERT(srcClipRect.Contains(p2));
        if (!srcClipRect.Contains(p2))
        {
            return EFalse;
        }
    }
    return ETrue;
}
