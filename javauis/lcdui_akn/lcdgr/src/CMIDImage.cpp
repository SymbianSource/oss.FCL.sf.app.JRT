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

#include "CMIDImage.h"
#include "CMIDBitmapImage.h"
#include "LcdImage.h"
#include "LcdFbsImage.h"
#include "LcdFbsImageCache.h"

#ifdef _DEBUG
static const TUint KTransparancyMapCount = 6;
#endif

const MMIDImage::TTransparencyType TransparencyMap[] =
{
    MMIDImage::ENone,       /* ETransparencyNone        */
    MMIDImage::EMask,       /* ETransparencyMaskBitmap  */
    MMIDImage::EAlpha,      /* ETransparencyAlphaBitmap */
    MMIDImage::EAlpha,      /* ETransparencyAlphaChannel*/
    MMIDImage::EMask,       /* ETransparencyMaskChannel */
    MMIDImage::ENone        /* ETransparencyIgnoreChannel */
};


CMIDImage::CMIDImage(CLcdFbsImageCache& aCache, MImageTypeMap& aTypeMap, CLcdImage* aImage)
        : iCache(aCache)
        , iTypeMap(aTypeMap)
        , iImage(aImage)
{
}

void CMIDImage::ConstructL()
{
    RegisterL();
}

void CMIDImage::RegisterL()
{
    CMIDBitmapImage* bitmapImage = new(ELeave) CMIDBitmapImage(*iImage->FbsImage());
    CleanupStack::PushL(bitmapImage);
    iCache.RegisterL(this, EFalse, bitmapImage);
    CleanupStack::Pop(bitmapImage);
}

CMIDImage::~CMIDImage()
{
    iCache.Purge(this);
    delete iImage;
}

MMIDImage::TTransparencyType
CMIDImage::TransparencyType() const
{
    TTransparency transparency = iImage->Transparency();

    ASSERT(0 <= transparency && transparency < KTransparancyMapCount);

    return TransparencyMap[transparency];
}

TInt CMIDImage::SetTransparencyType(TTransparencyType aType)
{
    if (TransparencyType() == aType)
    {
        return KErrNone;
    }

    if (aType != MMIDImage::EAlpha)
    {
        //
        // Only support switch to alpha for transparent mutable images.
        // Fail all other cases.
        //
        return KErrNotSupported;
    }

    //
    // Set new image type and fixup cache entries. Iff any of these steps
    // fail the image may be left in an inconsistent state. It is anticipated
    // that SetTransparencyType will be called once only during transparent
    // mutable image construction (the only supported use case). Hence
    // transactional error handling not required as failure will result in
    // immediate destruction of the image.
    //
    TInt err = iImage->SetImageType(iTypeMap.GetImageType(aType));
    if (0 == err)
    {
        iCache.Purge(this);
        TRAP(err, RegisterL());
    }

    return err;
}

TInt CMIDImage::CopyRegion
(
    MMIDImage*          aSource,
    const TRect&        aRect,
    TInt                aTransform
)
{
    CMIDImage& proxy = *static_cast<CMIDImage*>(aSource);
    CLcdImage& source = proxy.Image();
    CLcdImage& target = Image();

    TDrawRegion region;
    TRect       clipRect(target.Size());    // clip to target, not source.

    region.iAnchor    = EAnchorTop|EAnchorLeft;
    region.iDstPoint  = TPoint();
    region.iSrcRect   = aRect;
    region.iTransform = aTransform;

    target.CopyRegion(source, region, clipRect);

    return KErrNone;
}

TInt CMIDImage::SetPixels
(
    const TUint32*  aPixelBuffer,
    TInt            aLength,
    TInt            aOffset,
    TInt            aScanLength,
    const TRect&    aRect,
    TBool           aProcessAlpha
)
{
    iImage->SetPixels(aRect, aPixelBuffer, aLength, aOffset, aScanLength, aRect.Size(), aProcessAlpha);
    return KErrNone;
}

TInt CMIDImage::GetPixels
(
    TUint32*        aPixelBuffer,
    TInt            aLength,
    TInt            aOffset,        // in words
    TInt            aScanLength,
    const TRect&    aRect
)
{
    iImage->GetPixels(aRect, aPixelBuffer, aLength, aOffset, aScanLength, aRect.Size());
    return KErrNone;
}

void CMIDImage::Dispose()
{
    delete this;
}

MMIDComponent::TType CMIDImage::Type() const
{
    return EImage;
}

TSize CMIDImage::Size() const
{
    return iImage->Size();
}

CLcdImage& CMIDImage::Image() const
{
    return *iImage;
}

TBool CMIDImage::DetectCollision
(
    const TRect& aRect1, TInt aTransform1, const TPoint& aPoint1,
    MMIDImage* aImage2,
    const TRect& aRect2, TInt aTransform2, const TPoint& aPoint2
)
{
    CMIDImage& proxy = *static_cast<CMIDImage*>(aImage2);
    CLcdImage& image2 = proxy.Image();

    return iImage->DetectCollision
           (
               aRect1,
               aTransform1,
               aPoint1,
               image2,
               aRect2,
               aTransform2,
               aPoint2
           );
}

/**
 *@return the primary mode bitmap representation of this image - i.e. the representation
 * that Graphics would render from or to, as opposed to the representation used by
 * high level components such as Form Items.
 *
 * IMPROVEMENT: consider keeping pointer to the CMIDBitmapImage instance constructed in
 * RegisterL() and return that here since it will always be the primary representation.
 */
MMIDBitmapImage* CMIDImage::BitmapImage()
{
    return (MMIDBitmapImage*)iCache.GetBitmapImage(this);
}

MMIDBitmapImage* CMIDImage::BitmapImage(TInt aColorMode, TInt aAlphaMode, TBool aInvert)
{
    return (MMIDBitmapImage*)iCache.GetBitmapImage(this, aColorMode, aAlphaMode, aInvert);
}

MMIDBitmapImage* CMIDImage::BitmapImageL(TInt aColorMode, TInt aAlphaMode, TBool aInvert)
{
    return (MMIDBitmapImage*)iCache.GetBitmapImageL(this, aColorMode, aAlphaMode, aInvert);
}
