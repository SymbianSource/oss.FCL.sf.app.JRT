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
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <lcdgdrv.h>
#include "lcdgdrvif.h"
#include "lcdtransform.h"
#include "calctransform.h"
#include "collision.h"
#include "lcdgdev.h"

#ifdef _DEBUG
TBool DbgValidateImageType(const TImageType& aType);
TBool DbgValidateRendererArray(const TImageRenderer aRendererArray[], TInt aCount);
#endif

/**
 * Used in blitter/renderer searches
 */
inline TBool operator == (const TImageType& aLhs, const TImageType& aRhs)
{
    return (aLhs.iTransparency == aRhs.iTransparency) &&
           (aLhs.iAlphaMode == aRhs.iAlphaMode) &&
           (aLhs.iColorMode == aRhs.iColorMode);
}

inline void Swap(TInt& aX, TInt& aY)
{
    TInt tmp = aX;
    aX = aY;
    aY = tmp;
}

#define ASCIITEXT(x) (const TUint8*)x
#define UCS16TEXT(x) (const TText*)L##x

struct TLcdDriverType
{
    TDisplayMode    iMode;
    const TUint8*   iDataType;      // ASCII/UTF8
    const TText*    iLibName;       // UNICODE
    TInt            iInterfaceUid;
};
const TLcdDriverType LcdDriverTypeTab[] =
{
    { EColor64K,  ASCIITEXT(LCDGD_COLOR64K_DRIVER_ID_STR),  UCS16TEXT("LCDC64K"),  LCDGD_COLOR64K_IMPLEMENTATION_UID },
    { EColor4K,   ASCIITEXT(LCDGD_COLOR4K_DRIVER_ID_STR),   UCS16TEXT("LCDC4K"),   LCDGD_COLOR4K_IMPLEMENTATION_UID  },
    { EColor16MU, ASCIITEXT(LCDGD_COLOR16MU_DRIVER_ID_STR), UCS16TEXT("LCDC16MU"), LCDGD_COLOR16MU_IMPLEMENTATION_UID },
    { EColor16MA, ASCIITEXT(LCDGD_COLOR16MA_DRIVER_ID_STR), UCS16TEXT("LCDC16MA"), LCDGD_COLOR16MA_IMPLEMENTATION_UID }
};
const TInt KLcdDriverTypeTabSize = sizeof(LcdDriverTypeTab)/sizeof(LcdDriverTypeTab[0]);

// *****************************************************************
// Driver implementation ECOM interface.
//
// *****************************************************************
EXPORT_C CLcdGraphicsDriverImpl* CLcdGraphicsDriverImpl::NewL(TDisplayMode aDisplayMode)
{
    const TLcdDriverType* type = &LcdDriverTypeTab[0];
    const TLcdDriverType* end  = type + KLcdDriverTypeTabSize;
    while (type < end)
    {
        if (type->iMode == aDisplayMode)
        {
            break;
        }
        type++;
    }

    if (type == end)
    {
        User::Leave(KErrNotSupported);
    }

    TEComResolverParams resolverParams;

    TPtrC8 dataType(type->iDataType);
    resolverParams.SetDataType(dataType);

    TUid    interfaceUid  = KLcdDriverInterfaceUid;
    TUint32 dtorKeyOffset = _FOFF(CLcdGraphicsDriverImpl, iDtor_ID_Key);

    //
    // Try to load via ECOM
    //
    CLcdGraphicsDriverImpl* impl = NULL;
    impl = (CLcdGraphicsDriverImpl*)REComSession::CreateImplementationL(interfaceUid, dtorKeyOffset, resolverParams);

    return impl;
}

EXPORT_C CLcdGraphicsDriverImpl::CLcdGraphicsDriverImpl
(
    const TDriverInfo& aDriverInfo,
    const TImageRenderer* aRendererArray,
    TInt aRendererCount,
    const TColorMap* aColorMapArray,
    TInt aColorMapCount,
    const TCollisionDetector* aCollisionDetectorArray,
    TInt aCollisionDetectorCount,
    const TDrawFunctions* aDrawFunctionsArray,
    TInt aDrawFunctionsCount
)
        : iDriverInfo(aDriverInfo)
        , iRendererArray(aRendererArray)
        , iRendererCount(aRendererCount)
        , iColorMapArray(aColorMapArray)
        , iColorMapCount(aColorMapCount)
        , iCollisionDetectorArray(aCollisionDetectorArray)
        , iCollisionDetectorCount(aCollisionDetectorCount)
        , iDrawFunctionsArray(aDrawFunctionsArray)
        , iDrawFunctionsCount(aDrawFunctionsCount)
{
}

EXPORT_C CLcdGraphicsDriverImpl::~CLcdGraphicsDriverImpl()
{
    if (iDtor_ID_Key != KNullUid)
    {
        REComSession::DestroyedImplementation(iDtor_ID_Key);
    }
}

EXPORT_C void CLcdGraphicsDriverImpl::GetDriverInfo(CLcdGraphicsDriver::TDriverInfo& aDriverInfo) const
{
    aDriverInfo=iDriverInfo;
}

EXPORT_C TInt CLcdGraphicsDriverImpl::DrawRegion
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TRect&                  aDstRect,
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
    TTransparency                 aSrcTransparency,
    const TRect&                  aSrcRect,
    TTransformType                aSrcTransform,
    const TRect&                  aClipRect
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);

    TInt err = KErrNotSupported;

    TImageType dstType;
    dstType.iColorMode = aDstColorBitmap->iDisplayMode;
    dstType.iAlphaMode = ENone;
    dstType.iTransparency = ETransparencyNone;

    TImageType srcType;
    srcType.iColorMode = aSrcColorBitmap->iDisplayMode;
    srcType.iAlphaMode = aSrcAlphaBitmap ? aSrcAlphaBitmap->iDisplayMode : ENone;
    srcType.iTransparency = aSrcTransparency;

    const TImageRenderer* renderer = GetRenderer(dstType, srcType, aSrcTransform, ECompositeSrcOver);
    if (renderer)
    {
        TImageRenderFunction drawRegion = renderer->iFunction;

        // calc source to target transform
        TLcdTransform transform = CalcTransform(aDstRect, aSrcRect, aSrcTransform);

        TRect dstRect(aDstRect);
        TRect srcRect(aSrcRect);
        TRect srcClipRect(aSrcColorBitmap->iSize);
        TRect dstClipRect(aDstColorBitmap->iSize);

        // clip cliprect to device rect
        dstClipRect.Intersection(aClipRect);

        // calculate source and target rects clipped to src and target bounds.
        ClipTransformRect(dstRect, srcRect, dstClipRect, srcClipRect, transform);

        // clip dst rect to cliprect shouldn't be necessary if
        // ClipTransformRect works
        dstRect.Intersection(dstClipRect);

        if (!dstRect.IsEmpty())
        {
            // calc target to source transform.
            transform = transform.Inverse();

            ASSERT(CheckBounds(aDstColorBitmap->iSize, aSrcColorBitmap->iSize, dstRect, transform));

            (*drawRegion)(aDstColorBitmap, NULL, dstRect, aSrcColorBitmap, aSrcAlphaBitmap, transform);
        }

        err = KErrNone;
    }

    return err;
}

EXPORT_C TInt CLcdGraphicsDriverImpl::CopyRegion
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    TTransparency                   aDstTransparency,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    TTransparency                   aSrcTransparency,
    const TRect&                    aSrcRect,
    TTransformType                  aSrcTransform,
    const TRect&                    aClipRect
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);

    TInt err = KErrNotSupported;
    TImageType dstType;
    TImageType srcType;

    dstType.iColorMode = aDstColorBitmap->iDisplayMode;
    dstType.iAlphaMode = aDstAlphaBitmap ? aDstAlphaBitmap->iDisplayMode : ENone;
    dstType.iTransparency = aDstTransparency;

    srcType.iColorMode = aSrcColorBitmap->iDisplayMode;
    srcType.iAlphaMode = aSrcAlphaBitmap ? aSrcAlphaBitmap->iDisplayMode : ENone;
    srcType.iTransparency = aSrcTransparency;

    const TImageRenderer* renderer = GetRenderer(dstType, srcType, aSrcTransform, ECompositeSrcCopy);
    if (renderer)
    {
        TImageRenderFunction copyRegion = renderer->iFunction;

        // calc source to target transform
        TLcdTransform transform = CalcTransform(aDstRect, aSrcRect, aSrcTransform);

        TRect dstRect(aDstRect);
        TRect srcRect(aSrcRect);
        TRect srcClipRect(aSrcColorBitmap->iSize);
        TRect dstClipRect(aDstColorBitmap->iSize);

        // clip cliprect to device rect
        dstClipRect.Intersection(aClipRect);

        // clip source and target rects
        ClipTransformRect(dstRect, srcRect, dstClipRect, srcClipRect, transform);

        // check src and dst rects still correspond
        ASSERT(CheckTransform(dstRect, srcRect, transform));

        // clip dst rect to cliprect shouldn't be necessary if
        // ClipTransformRect works
        dstRect.Intersection(dstClipRect);

        if (!dstRect.IsEmpty())
        {
            // calc target to source transform.
            transform = transform.Inverse();

            // check source and dst rects lie within bounds
            ASSERT(CheckBounds(aDstColorBitmap->iSize, aSrcColorBitmap->iSize, dstRect, transform));

            (*copyRegion)(aDstColorBitmap, aDstAlphaBitmap, dstRect, aSrcColorBitmap, aSrcAlphaBitmap, transform);
        }

        err = KErrNone;
    }

    return err;
}

/**
Detect collision between images. Returns ETrue if the overlapping
region contains corresponding opaque pixels.
*/
EXPORT_C TBool CLcdGraphicsDriverImpl::DetectCollision
(
    const TAcceleratedBitmapInfo* aMask1,
    TTransparency       aTransparency1,
    const TSize&        aSize1,
    const TRect&        aRect1,
    TInt                aTransform1,
    const TPoint&       aPoint1,

    const TAcceleratedBitmapInfo* aMask2,
    TTransparency       aTransparency2,
    const TSize&        aSize2,
    const TRect&        aRect2,
    TInt                aTransform2,
    const TPoint&       aPoint2
)
{
    if ((aMask1 == NULL) && (aMask2 == NULL))
    {
        // if neither have mask all pixels opaque, so collide
        return ETrue;
    }

    ASSERT(aRect1.Intersects(TRect(aSize1)));
    ASSERT(aRect2.Intersects(TRect(aSize2)));

    TDisplayMode displayMode1 = aMask1 ? aMask1->iDisplayMode : ENone;
    TDisplayMode displayMode2 = aMask2 ? aMask2->iDisplayMode : ENone;
    const TCollisionDetector* detector = GetCollisionDetector(displayMode1,aTransparency1,displayMode2,aTransparency2);
    ASSERT(detector != NULL);

    TSize dstSize1=aRect1.Size();
    if (aTransform1 & EReflectDiag)
    {
        Swap(dstSize1.iWidth, dstSize1.iHeight);
    }
    TSize dstSize2=aRect2.Size();
    if (aTransform2 & EReflectDiag)
    {
        Swap(dstSize2.iWidth, dstSize2.iHeight);
    }
    TRect dstRect1(aPoint1, dstSize1);
    TRect dstRect2(aPoint2, dstSize2);

    TLcdTransform forward1 = CalcTransform(dstRect1, aRect1, (TTransformType)aTransform1);
    const TRect srcClipRect1(aSize1);
    TRect srcRect1 = aRect1;
    ClipTransformRect(dstRect1, srcRect1, dstRect2, srcClipRect1, forward1);
    TLcdTransform transform1 = forward1.Inverse();

    TLcdTransform forward2 = CalcTransform(dstRect2, aRect2, (TTransformType)aTransform2);
    const TRect srcClipRect2(aSize2);
    TRect srcRect2 = aRect2;
    ClipTransformRect(dstRect2, srcRect2, dstRect1, srcClipRect2, forward2);
    TLcdTransform transform2 = forward2.Inverse();

    return (*detector->iFunction)(aMask1, aMask2, srcRect1, srcRect2, transform1, transform2);
}

CLcdGraphicsDevice* CLcdGraphicsDriverImpl::CreateDeviceL(const TImageType& aType)
{
    const TDrawFunctions* drawFunctions = GetDrawFunctions(aType.iColorMode);
    const TColorMap* colorMap = GetColorMap(aType.iColorMode);

    if (!drawFunctions || !colorMap)
    {
        User::Leave(KErrNotSupported);
    }

    CRenderFunctions* renderers = new(ELeave) CRenderFunctions;
    CleanupStack::PushL(renderers);

    const TImageRenderer* ptr = iRendererArray;
    const TImageRenderer* end = iRendererArray + iRendererCount;
    while (ptr < end)
    {
        //
        // extract only the drawRegion renderers
        //
        if ((ptr->iTargetType == aType) && (ptr->iCompositeRule == ECompositeSrcOver))
        {
            renderers->AppendL(*ptr);
        }
        ++ptr;
    }

    //
    // We need to find all renderers targeting aType
    //
    CLcdGraphicsDeviceImpl* impl = new(ELeave) CLcdGraphicsDeviceImpl
    (
        *this,
        aType,
        renderers,
        *colorMap,
        *drawFunctions
    );
    CleanupStack::Pop(renderers);
    return impl;
}

const TImageRenderer* CLcdGraphicsDriverImpl::GetRenderer
(
    const TImageType& aTargetType,
    const TImageType& aSourceType,
    TTransformType aTransform,
    TCompositRule aCompositeRule
)
const
{
    const TInt transformBit = 1<<aTransform;

    const TInt            count = iRendererCount;
    const TImageRenderer* array = iRendererArray;

    const TImageRenderer* ptr = array;
    const TImageRenderer* end = array + count;

    while (ptr < end)
    {
        if ((ptr->iCompositeRule == aCompositeRule) &&
                (ptr->iTransformMask & transformBit)  &&
                (ptr->iSourceType == aSourceType)     &&
                (ptr->iTargetType == aTargetType))
        {
            return ptr;
        }
        ++ptr;
    }

    return NULL;
}

/**
 *
 */
const TColorMap* CLcdGraphicsDriverImpl::GetColorMap(TDisplayMode aMode)
{
    const TColorMap* ptr = iColorMapArray;
    const TColorMap* end = iColorMapArray + iColorMapCount;

    while (ptr < end)
    {
        if (ptr->iTargetMode == aMode)
        {
            return ptr;
        }
        ++ptr;
    }

    return NULL;
}


const TCollisionDetector* CLcdGraphicsDriverImpl::GetCollisionDetector
(
    TDisplayMode    aDisplayMode1,
    TTransparency   aTransparency1,
    TDisplayMode    aDisplayMode2,
    TTransparency   aTransparency2
)
const
{
    const TCollisionDetector* ptr = iCollisionDetectorArray;
    const TCollisionDetector* end = iCollisionDetectorArray + iCollisionDetectorCount;

    TUint displayMode1Bit   = 1<<aDisplayMode1;
    TUint transparency1Bit  = 1<<aTransparency1;
    TUint displayMode2Bit   = 1<<aDisplayMode2;
    TUint transparency2Bit  = 1<<aTransparency2;

    while (ptr < end)
    {
        if ((ptr->iDisplayMode1 & displayMode1Bit) &&
                (ptr->iTransparency1 & transparency1Bit) &&
                (ptr->iDisplayMode2 & displayMode2Bit) &&
                (ptr->iTransparency2 & transparency2Bit))
        {
            return ptr;
        }
        ++ptr;
    }

    return NULL;
}


EXPORT_C const TDrawFunctions* CLcdGraphicsDriverImpl::GetDrawFunctions(TDisplayMode aMode) const
{
    const TDrawFunctions* ptr = iDrawFunctionsArray;
    const TDrawFunctions* end = iDrawFunctionsArray + iDrawFunctionsCount;
    while (ptr < end)
    {
        if (ptr->iDisplayMode == aMode)
        {
            return ptr;
        }
        ++ptr;
    }
    return NULL;
}

#ifdef _DEBUG
TBool DbgValidateRendererArray(const TImageRenderer aRendererArray[], TInt aCount)
{
    for (TInt i=0; i<aCount; i++)
    {
        const TImageRenderer& renderer = aRendererArray[i];

        if (!DbgValidateImageType(renderer.iTargetType))
        {
            return EFalse;
        }

        if (!DbgValidateImageType(renderer.iSourceType))
        {
            return EFalse;
        }

        if (NULL == renderer.iFunction)
        {
            return EFalse;
        }
    }
    return ETrue;
}

TBool DbgValidateImageType(const TImageType& aType)
{
    //
    // Validate displaymodes against transparency type.
    //
    switch (aType.iTransparency)
    {
    case ETransparencyNone:
        //
        // Must be none.
        //
        if (aType.iAlphaMode != ENone)
        {
            return EFalse;
        }
        break;

    case ETransparencyMaskBitmap:
        //
        // Must be one of EGray2 or matching display mode
        //
        if ((aType.iAlphaMode != EGray2) && (aType.iAlphaMode != aType.iColorMode))
        {
            return EFalse;
        }
        break;

    case ETransparencyAlphaBitmap:
        //
        // Must be EGray256 - the only supported alpha bitmap type.
        //
        if (aType.iAlphaMode != EGray256)
        {
            return EFalse;
        }
        break;

    case ETransparencyAlphaChannel:
        //
        // Alpha mode must be none
        // Color mode must be the Java ARGB8888 mode (build dependent)
        //
        if ((aType.iAlphaMode != ENone) || (aType.iColorMode != EColorARGB8888))
        {
            return EFalse;
        }
        break;

    case ETransparencyMaskChannel:
        if ((aType.iAlphaMode != ENone) || (aType.iColorMode != EColorARGB8888))
        {
            return EFalse;
        }
        break;

#ifdef RD_JAVA_NGA_ENABLED
    case ETransparencyAlphaChannelPre:
    case ETransparencyMaskChannelPre:
        if ((aType.iAlphaMode != ENone) || (aType.iColorMode != EColorARGB8888))
        {
            return EFalse;
        }
        break;
#endif // RD_JAVA_NGA_ENABLED

    case ETransparencyIgnoreChannel:
        if ((aType.iAlphaMode != ENone) || (aType.iColorMode != EColorARGB8888))
        {
            return EFalse;
        }
        break;
    }

    return ETrue;
}
#endif

