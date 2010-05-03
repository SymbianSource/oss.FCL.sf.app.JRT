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

#ifndef LCDIMAGE_H
#define LCDIMAGE_H

#include <e32base.h>
#include <fbs.h>

#include <lcdgdrv.h>

#include "fbslcdgraphics.h" // for TDrawRegion.

class TAcceleratedBitmapInfo;
class TBitmapLockCount;
class CLcdFbsImage;

/**
 * MIDP LCDUI Image peer
 */
NONSHARABLE_CLASS(CLcdImage) : public CBase
{
public:
    /**
     *
     */
    CLcdImage(CLcdGraphicsDriver& aDriver, TBool aMutable);

    /**
     *
     */
    ~CLcdImage();

    /**
     * 2nd phase constructor for blank (white) images.
     */
    void ConstructL(const TSize& aSize, const TImageType& aImageType);

    /**
     * 2nd phase constructor from bitmaps.
     *
     * Where possible (bitmaps match aImageType ) makes a shallow copy (Duplicate) of aColorBitmap
     * and optional aAlphaBitmap, otherwise makes a deep copy converting to aImageType.
     */
    void ConstructL(CFbsBitmap* aColorBitmap, CFbsBitmap* aAlphaBitmap, const TImageType& aImageType);

    TSize Size() const;
    TTransparency Transparency() const;

    /**
     *
     */
    void Lock(TBitmapLockCount& aLockCount, TAcceleratedBitmapInfo& aColorBitmap, TAcceleratedBitmapInfo& aAlphaBitmap) const;

    /**
     *
     */
    void Unlock(TBitmapLockCount& aLockCount) const;

    /**
     *
     */
    void SetPixels(const TRect& aRect, const TUint32* aARGBArray, TInt aLength, TInt aOffset, TInt aLinePitch, const TSize& aSize, TBool aProcessAlpha);

    /**
     *
     */
    void GetPixels(const TRect& aRect, TUint32* aARGBArray, TInt aLength, TInt aOffset, TInt aLinePitch, const TSize& aSize);

    /**
     * Copy a region of pixels from aSourceImage applying a transformation.
     */
    void CopyRegion(CLcdImage& aSourceImage, const TDrawRegion&, const TRect& aClipRect);

    /**
    Detect pixel level collision between two Images.
    @param aRect1       image1 collison rect in image1 co-ordinates
    @param aTransform1  image1 transform
    @param aPoint1      position of image1 collison rect in painter's co-ordinates.

    @param aImage2      image2
    @param aRect2       image2 collison rect in image2 co-ordinates
    @param aTransform2  image2 transform
    @param aPoint2      position of image2 collison rect in painter's co-ordinates.
    */
    TBool DetectCollision(const TRect& aRect1, TInt aTransform1, const TPoint& aPoint1,
                          const CLcdImage& aImage2,
                          const TRect& aRect2, TInt aTransform2, const TPoint& aPoint2);

    //
    // Consider drawing surface accessor instead.
    //
    TBool Mutable();

    /**
     * Change image type - may be used to allow mutable images to contain tranparent pixels.
     */
    TInt SetImageType(const TImageType& aType);

    //
    // Temporary.
    //
    CLcdFbsImage* FbsImage();

private:
    CLcdGraphicsDriver& iDriver;
    CLcdFbsImage*       iFbsImage;
    TTransparency       iTransparency;
    TBool               iMutable;
};

#endif // LCDIMAGE_H
