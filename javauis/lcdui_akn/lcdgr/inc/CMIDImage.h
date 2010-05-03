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

#ifndef CMIDIMAGE_H
#define CMIDIMAGE_H

#include <e32base.h>
#include <fbs.h>
#include <lcdgr.h>
#include <lcdgdrv.h>
#include "LcdImage.h"

class CLcdFbsImageCache;

class MImageTypeMap
{
public:
    virtual TImageType GetImageType(MMIDImage::TTransparencyType aType) = 0;
};

/**
 * MIDP Image peer proxy exported via MMIDImage interface.
 */
NONSHARABLE_CLASS(CMIDImage) : public CBase, public MMIDImage
{
public:
    enum { EType = MMIDComponent::EImage };

public:
    CMIDImage(CLcdFbsImageCache& aCache, MImageTypeMap& aTypeMap, CLcdImage* aImage);
    void ConstructL();
    ~CMIDImage();

    CLcdImage&  Image() const;

    /**
     * From MMIDImage
     */
    void Dispose();

    TType Type() const;

    TSize Size() const;

    /**
     *@returns type of transparecy channel for this image.
     * ENone indicates that TransparencyChannel() will return NULL
     * EMask indicates that TransparencyChannel() will return a bitmap
     * of the same displaymode as ColorChannel(), which is used as a
     * mask. Each pixel will be either white (indicating opaque) or black
     * (indicating transparent).
     *
     */
    TTransparencyType TransparencyType() const;

    /**
     * transparent mutable image support.
     */
    TInt SetTransparencyType(TTransparencyType aType);

    TInt SetPixels(const TUint32* aPixelBuffer, TInt aLength, TInt aOffset, TInt aScanLength, const TRect& aRect, TBool aProcessAlpha);

    TInt GetPixels(TUint32* aPixelBuffer, TInt aLength, TInt aOffset, TInt aScanLength, const TRect& aRect);

    /**
     * Copy a region of pixels from aSourceImage applying a transformation.
     */
    TInt CopyRegion(MMIDImage* aSourceImage, const TRect& aRect, TInt aTransform);

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
                          MMIDImage* aImage2,
                          const TRect& aRect2, TInt aTransform2, const TPoint& aPoint2);

    /* return primary bitmap representation of this image */
    MMIDBitmapImage* BitmapImage();

    MMIDBitmapImage* BitmapImage(TInt aColorMode, TInt aAlphaMode, TBool aInvert);

    MMIDBitmapImage* BitmapImageL(TInt aColorMode, TInt aAlphaMode, TBool aInvert);

private:
    void RegisterL();

private:
    CLcdFbsImageCache&  iCache;
    MImageTypeMap&      iTypeMap;
    CLcdImage*          iImage;
};

#endif // CMIDIMAGE_H
