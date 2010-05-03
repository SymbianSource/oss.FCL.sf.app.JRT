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

#ifndef FBSLCDGRAPHICS_H
#define FBSLCDGRAPHICS_H

#include <e32std.h>
#include <gdi.h>
#include <lcdgdrv.h>

/**
 * Structure containing sufficient information to construct
 * a transform for the DrawRegion primitive.
 */
struct TDrawRegion
{
    TRect   iSrcRect;
    TPoint  iDstPoint;
    TInt    iAnchor;
    TInt    iTransform;
};

/**
 * Utility function to compute the destination rectangle given a source rectangle, an identifier for the
 * anchor point to translate to the destination point and a transform to apply to the source rectangle.
 */
TRect CalcDstRect(const TPoint& aDstPoint, const TSize& aSrcSize, TTransformType aSrcTransform, TAnchor aSrcAnchor);

//
// FBSERV bitmap support, may be withdrawn later.
//
#define _LCDGDRV_FBSBITMAP_SUPPORT_

#ifdef _LCDGDRV_FBSBITMAP_SUPPORT_

class CFbsBitmap;

NONSHARABLE_CLASS(TFbsLcdGraphics)
{
public:

    /**
     * Copy region of image represented by <aSrcColorBitmap, aSrcAlphaBitmap> pair
     * onto destination image represented by <aDstColorBitmap, aDstAlphaBitmap> pair.
     * The region to be drawn, the destination position and the transformation to
     * apply are supplied by aDrawRegion.
     */
    static TInt CopyRegion
    (
        CLcdGraphicsDriver& aDriver,
        CFbsBitmap*         aDstColorBitmap,
        CFbsBitmap*         aDstAlphaBitmap,
        TTransparency       aDstTransparency,
        CFbsBitmap*         aSrcColorBitmap,
        CFbsBitmap*         aSrcAlphaBitmap,
        TTransparency       aSrcTransparency,
        const TDrawRegion&  aRegion,
        const TRect&        aClipRect
    );

    /**
     * Copies ARGB8888 pixels into image represented by <aDstColorBitmap, aDstAlphaBitmap>
     */
    static TInt SetPixels
    (
        CLcdGraphicsDriver& aDriver,
        CFbsBitmap*     aDstColorBitmap,
        CFbsBitmap*     aDstAlphaBitmap,
        TTransparency   aDstTransparency,
        const TRect&    aDstRect,
        const TUint32*  aSrcARGB32Pixels,
        TInt            aSrcLength,
        TInt            aSrcOffset,
        TInt            aSrcLinePitch,
        const TSize&    aSrcSize,
        TBool           aAlpha
    );

    /**
     * Copies pixels from image represented by <aDstColorBitmap, aDstAlphaBitmap>
     * into an array in ARGB8888 format.
     */
    static TInt GetPixels
    (
        CLcdGraphicsDriver& aDriver,
        CFbsBitmap*     aSrcColorBitmap,
        CFbsBitmap*     aSrcAlphaBitmap,
        TTransparency   aSrcTransparency,
        const TRect&    aSrcRect,
        TUint32*        aARGB32Pixels,
        TInt            aLength,
        TInt            aOffset,
        TInt            aLinePitch,
        const TSize&    aSize
    );

    /**
    Check for pixel level collision in area of overlap in two images.
    Locks bitmap.
    @param aMask1           image1 mask
    @param aTransparency1   image1 transparency type
    @param aSize1           image1 size
    @param aRect1           image1 collison rect in image1 co-ordinates
    @param aTransform1      image1 transform
    @param aPoint1          position of image1 collison rect in painter's co-ordinates.

    @param aMask2           image2 mask
    @param aTransparency1   image2 transparency type
    @param aSize2           image2 size
    @param aRect2           image2 collison rect in image2 co-ordinates
    @param aTransform2      image2 transform
    @param aPoint2          position of image2 collison rect in painter's co-ordinates.
    */
    static TBool DetectCollision
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
    );
};

#endif

#endif // FBSLCDGRAPHICS_H
