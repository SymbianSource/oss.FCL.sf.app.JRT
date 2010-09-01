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

#include "LcdImage.h"
#include <lcdgdrv.h>
#include "fbslcdgraphics.h"
#include "LcdFbsImage.h"

/**
 * Copy bitmap data from (aColorBitmap,aAlphaBitmap) to aFbsImage.
 */
extern void CopyBitmapsL
(
    CLcdGraphicsDriver& aDriver,
    CLcdFbsImage&       aFbsImage,
    CFbsBitmap*         aColorBitmap,
    CFbsBitmap*         aAlphaBitmap,
    TBool               aInvertMask
);

CLcdImage::CLcdImage(CLcdGraphicsDriver& aDriver, TBool aMutable)
        : iDriver(aDriver)
        , iFbsImage(NULL)
        , iMutable(aMutable)
{
}

CLcdImage::~CLcdImage()
{
    if (iFbsImage)
    {
        iFbsImage->RemoveRef();
    }
}

void CLcdImage::ConstructL(const TSize& aSize, const TImageType& aImageType)
{
    ASSERT(NULL == iFbsImage);
    ASSERT(aSize.iWidth  > 0);
    ASSERT(aSize.iHeight > 0);

    iFbsImage = CLcdFbsImage::NewL(aSize, aImageType.iColorMode, aImageType.iAlphaMode);
    iFbsImage->AddRef();
    iTransparency = aImageType.iTransparency;
}

/**
 * 2nd phase constructor from bitmap(s)
 *
 * Where possible, make a shallow copy (Duplicate) of aColorBitmap and optional aAlphaBitmap,
 * if that is not possible because the bitmaps do not match the desired image type, make a
 * deep copy.
 */
void CLcdImage::ConstructL(CFbsBitmap* aColorBitmap, CFbsBitmap* aAlphaBitmap, const TImageType& aImageType)
{
    ASSERT(NULL == iFbsImage);

    TDisplayMode colorMode = aColorBitmap->DisplayMode();
    TDisplayMode alphaMode = aAlphaBitmap ? aAlphaBitmap->DisplayMode() : ENone;

    if ((colorMode == aImageType.iColorMode) && (alphaMode == aImageType.iAlphaMode))
    {
        // modes match - just duplicate.
        iFbsImage = CLcdFbsImage::NewL(aColorBitmap, aAlphaBitmap);
        iFbsImage->AddRef();
    }
    else
    {
        iFbsImage = CLcdFbsImage::NewL(aColorBitmap->SizeInPixels(), aImageType.iColorMode, aImageType.iAlphaMode);
        iFbsImage->AddRef();
        CopyBitmapsL(iDriver, *iFbsImage, aColorBitmap, aAlphaBitmap, EFalse);
    }

    iTransparency = aImageType.iTransparency;
}

TTransparency CLcdImage::Transparency() const
{
    return iTransparency;
}

TSize CLcdImage::Size() const
{
    return iFbsImage->Size();
}

void CLcdImage::CopyRegion(CLcdImage& aSource, const TDrawRegion& aRegion, const TRect& aClipRect)
{
    TTransparency dstTransparency = Transparency();
    TTransparency srcTransparency = aSource.Transparency();

    TInt error = TFbsLcdGraphics::CopyRegion
                 (
                     iDriver,
                     iFbsImage->ColorBitmap(),
                     iFbsImage->AlphaBitmap(),
                     dstTransparency,
                     aSource.iFbsImage->ColorBitmap(),
                     aSource.iFbsImage->AlphaBitmap(),
                     srcTransparency,
                     aRegion,
                     aClipRect
                 );

    ASSERT(error==KErrNone);
}

void CLcdImage::SetPixels
(
    const TRect&    aRect,
    const TUint32*  aPixels,
    TInt            aLength,
    TInt            aOffset,
    TInt            aScanLength,
    const TSize&    aSize,
    TBool           aProcessAlpha
)
{
    TInt error = TFbsLcdGraphics::SetPixels
                 (
                     iDriver,
                     iFbsImage->ColorBitmap(),
                     iFbsImage->AlphaBitmap(),
                     iTransparency,
                     aRect,
                     aPixels,
                     aLength,
                     aOffset,
                     aScanLength,
                     aSize,
                     aProcessAlpha
                 );

    ASSERT(error == KErrNone);
}

void CLcdImage::GetPixels
(
    const TRect&    aRect,
    TUint32*        aPixels,
    TInt            aLength,
    TInt            aOffset,
    TInt            aScanLength,
    const TSize&    aSize
)
{
    TInt error = TFbsLcdGraphics::GetPixels
                 (
                     iDriver,
                     iFbsImage->ColorBitmap(),
                     iFbsImage->AlphaBitmap(),
                     iTransparency,
                     aRect,
                     aPixels,
                     aLength,
                     aOffset,
                     aScanLength,
                     aSize
                 );

    ASSERT(error == KErrNone);
}

TBool CLcdImage::DetectCollision
(
    const TRect&        aRect1,
    TInt                aTransform1,
    const TPoint&       aPoint1,
    const CLcdImage&    aImage2,
    const TRect&        aRect2,
    TInt                aTransform2,
    const TPoint&       aPoint2
)
{
    TTransparency transparency1 = iTransparency;
    CFbsBitmap* alphaBitmap1 = iFbsImage->AlphaBitmap();
    if (transparency1 == ETransparencyAlphaChannel || transparency1 == ETransparencyMaskChannel)
    {
        alphaBitmap1 = iFbsImage->ColorBitmap();
    }

    TTransparency transparency2 = aImage2.iTransparency;
    CFbsBitmap* alphaBitmap2 = aImage2.iFbsImage->AlphaBitmap();
    if (transparency2 == ETransparencyAlphaChannel || transparency2 == ETransparencyMaskChannel)
    {
        alphaBitmap2 = aImage2.iFbsImage->ColorBitmap();
    }

    return TFbsLcdGraphics::DetectCollision
           (
               iDriver,
               alphaBitmap1,
               iTransparency,
               Size(),
               aRect1,
               aTransform1,
               aPoint1,
               alphaBitmap2,
               aImage2.Transparency(),
               aImage2.Size(),
               aRect2,
               aTransform2,
               aPoint2
           );
}


void CLcdImage::Lock(TBitmapLockCount& aCount, TAcceleratedBitmapInfo& aColorBitmap, TAcceleratedBitmapInfo& aAlphaBitmap) const
{
    CFbsBitmap* colorBitmap = iFbsImage->ColorBitmap();
    CFbsBitmap* alphaBitmap = iFbsImage->AlphaBitmap();

    TAcceleratedBitmapSpec color(colorBitmap);
    TAcceleratedBitmapSpec alpha;
    if (alphaBitmap)
    {
        alpha = TAcceleratedBitmapSpec(alphaBitmap);
    }

    color.Lock(aCount);
    color.GetInfo(aColorBitmap);
    if (alphaBitmap)
    {
        alpha.Lock(aCount);
        alpha.GetInfo(aAlphaBitmap);
    }
    else
    {
        Mem::FillZ(&aAlphaBitmap, sizeof(TAcceleratedBitmapInfo));
    }
}

void CLcdImage::Unlock(TBitmapLockCount& aCount) const
{
    CFbsBitmap* colorBitmap = iFbsImage->ColorBitmap();
    CFbsBitmap* alphaBitmap = iFbsImage->AlphaBitmap();

    if (alphaBitmap)
    {
        TAcceleratedBitmapSpec alpha(alphaBitmap);
        alpha.Unlock(aCount);
    }

    TAcceleratedBitmapSpec color(colorBitmap);
    color.Unlock(aCount);
}

TBool CLcdImage::Mutable()
{
    return iMutable;
}

/**
 * Temporary
 */
CLcdFbsImage* CLcdImage::FbsImage()
{
    return iFbsImage;
}

TInt CLcdImage::SetImageType(const TImageType& aType)
{
    //
    // We only support changing from no-transparency images.
    //
    if (iTransparency != ETransparencyNone)
    {
        return KErrNotSupported;
    }

    ASSERT(NULL == iFbsImage->AlphaBitmap());

    CFbsBitmap* colorBitmap = iFbsImage->ColorBitmap();
    TDisplayMode colorMode  = colorBitmap->DisplayMode();

    TInt err = KErrNotSupported;
    if ((colorMode == aType.iColorMode) && (aType.iTransparency == ETransparencyAlphaBitmap))
    {
        //
        // We are just adding an alpha bitmap.
        //
        TRAP(err, iFbsImage->CreateAlphaBitmapL());
    }
    else if ((colorMode == EColor16MU) && (aType.iColorMode == EColor16MA) && (aType.iTransparency == ETransparencyAlphaChannel))
    {
        //
        // Switch from EColor16MU to EColor16MA mode.
        //
        err = colorBitmap->SetDisplayMode(EColor16MA);
    }

    if (0 == err)
    {
        iTransparency = aType.iTransparency;
    }

    return err;
}
