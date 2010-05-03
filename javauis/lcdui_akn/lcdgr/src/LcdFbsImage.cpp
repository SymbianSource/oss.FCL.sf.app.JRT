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

#include <bitdev.h>
#include <bitstd.h>
#include <lcdgdrv.h>
#include "LcdFbsImage.h"

#ifdef LCDGR_ACCELERATED_BITGDI
const TUid KJvmExeUid = { 0x102033E6 };
#endif

/**
 * Copy bitmap data from (aColorBitmap,aAlphaBitmap) to aFbsImage.
 */
extern void CopyBitmapsL
(
    CLcdGraphicsDriver& /* aDriver */,  // not used yet
    CLcdFbsImage&       aFbsImage,
    CFbsBitmap*         aColorBitmap,
    CFbsBitmap*         aAlphaBitmap,
    TBool               aInvertMask
);

/**
 * Create a CFbsBitmap of size aSize in displaymode aMode
 */
LOCAL_C CFbsBitmap* CreateBitmapL(const TSize& aSize, TDisplayMode aMode);

/**
 * Set alpha channel in EColor16MA mode aBitmap from aAlphaBitmap. If aAlphaBitmap is
 * in a mode other than EGray256 this procedure will create a temporary bitmap in
 * EGray256 mode and copy pixels from aAlphaBitmap into that first. It is expected
 * that aAlphaBitmap would be a binary mask.
 */
LOCAL_C void SetAlphaChannelL(CFbsBitmap* aBitmap, CFbsBitmap* aAlphaBitmap, TBool aInvert);

/**
 * Set the alpha channel in an EColor16MA mode aBitmap from an EGray256 mode
 * aGray256Bitmap.
 */
LOCAL_C void SetAlphaChannel(CFbsBitmap* aBitmap, CFbsBitmap* aGray256Bitmap, TBool aInvert);

/**
 * Copy alpha information from EColor16MA bitmap aSource, into EGray2 or EGray256
 * bitmap aTarget.
 */
LOCAL_C void GetAlphaChannelL(CFbsBitmap* aTarget, CFbsBitmap* aSource, TBool aInvert);

/**
 * Copy alpha channel from EColor16MA bitmap into an EGray256 bitmap
 */
LOCAL_C void GetAlphaChannel(CFbsBitmap* aTargetGray256, CFbsBitmap* aSourceColor16MA, TBool aInvert);

#ifdef _DEBUG
//
// debug only - check that EColor16MA aBitmap is fully opaque
//
LOCAL_C TBool CheckOpaque(CFbsBitmap* aBitmap);
#endif


/*************************************************************
 *
 * CLcdFbsImage potentially public bitmapped image class.
 *
 ************************************************************/

CLcdFbsImage* CLcdFbsImage::NewL(const TSize& aSize, TDisplayMode aColorMode, TDisplayMode aAlphaMode)
{
    CLcdFbsImage* image = new(ELeave) CLcdFbsImage;
    CleanupStack::PushL(image);
    image->iColorBitmap = CreateBitmapL(aSize, aColorMode);
    if (aAlphaMode)
    {
        image->iAlphaBitmap = CreateBitmapL(aSize, aAlphaMode);
    }
    CleanupStack::Pop(image);
    return image;
}

CLcdFbsImage* CLcdFbsImage::NewL(CFbsBitmap* aColorBitmap, CFbsBitmap* aAlphaBitmap)
{
    CLcdFbsImage* image = new(ELeave) CLcdFbsImage;

    CleanupStack::PushL(image);
    image->iColorBitmap = DuplicateBitmapL(aColorBitmap);
    if (aAlphaBitmap)
    {
        image->iAlphaBitmap = DuplicateBitmapL(aAlphaBitmap);
    }
    CleanupStack::Pop(image);

    return image;
}

CLcdFbsImage::~CLcdFbsImage()
{
    ASSERT(0 == iRefCount);
    delete iAlphaBitmap;
    delete iColorBitmap;
}

TSize CLcdFbsImage::Size() const
{
    return iColorBitmap->SizeInPixels();
}

void CLcdFbsImage::AddRef()
{
    ++iRefCount;
}

void CLcdFbsImage::RemoveRef()
{
    --iRefCount;
    if (iRefCount == 0)
    {
        delete this;
    }
}

/**
 *@return the color bitmap
 */
CFbsBitmap* CLcdFbsImage::ColorBitmap() const
{
    return iColorBitmap;
}

/**
 *@return the alpha bitmap if any
 */
CFbsBitmap* CLcdFbsImage::AlphaBitmap() const
{
    return iAlphaBitmap;
}

void CLcdFbsImage::CreateAlphaBitmapL()
{
    if (iAlphaBitmap)
    {
        User::Leave(KErrAlreadyExists);
    }

    iAlphaBitmap = CreateBitmapL(Size(), EGray256);
}

/**
 * Create a bitmap of size aSize in display mode aMode.
 */
LOCAL_C CFbsBitmap* CreateBitmapL(const TSize& aSize, TDisplayMode aMode)
{
    TInt err = KErrNotSupported;

    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);

#ifdef LCDGR_ACCELERATED_BITGDI
    //
    // Attempt to create hardware bitmap which can be used with a
    // hardware graphics accelerator. Very few if any devices
    // appear to support RHardwareBitmap so this is very unlikely
    // to be used.
    //
    err = bitmap->CreateHardwareBitmap(aSize, aMode, KJvmExeUid);
#endif

    if (err != KErrNone)
    {
        //
        // Fallback to software only bitmap if hardware unavailable/not supported.
        //
        err = bitmap->Create(aSize, aMode);
    }

    User::LeaveIfError(err);

    CleanupStack::Pop(bitmap);

    return bitmap;
}

/**
 * Create a new bitmap in a given mode and copy the data.
 * Slowish closest match copy. We could use lcd graphics driver for some
 * of these copies but is good enough for now.
 */
CFbsBitmap* CLcdFbsImage::CopyBitmapL(CFbsBitmap* aBitmap, TDisplayMode aMode, TBool aInvert)
{
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Create(aBitmap->SizeInPixels(), aMode));
    CopyBitmapL(bitmap, aBitmap, aInvert);
    CleanupStack::Pop(bitmap);
    return bitmap;
}

/**
 * Copy bitmap content from aSource to aTarget optionally inverting.
 */
void CLcdFbsImage::CopyBitmapL(CFbsBitmap* aTarget, CFbsBitmap* aSource, TBool aInvert)
{
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(aTarget);
    CleanupStack::PushL(device);
    CFbsBitGc* gc = NULL;
    User::LeaveIfError(device->CreateContext(gc));

    // Don't alpha blend (Either copy the alpha component or write opaque pixels)
    TInt drawMode = CGraphicsContext::EDrawModeWriteAlpha;
    if (aInvert)
    {
        // Invert pixel values - only really makes sense for binary masks
        // For EColor16MA images the alpha component is not inverted
        drawMode |= CGraphicsContext::EInvertPen;
    }

    gc->SetDrawMode(CGraphicsContext::TDrawMode(drawMode));
    gc->BitBlt(TPoint(), aSource);
    delete gc;
    CleanupStack::PopAndDestroy(device);
}

/**
 * Dup the handle. This requires an FBSERV ipc.
 */
CFbsBitmap* CLcdFbsImage::DuplicateBitmapL(CFbsBitmap* aBitmap)
{
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(aBitmap->Handle()));
    CleanupStack::Pop(bitmap);
    return bitmap;
}

/**
 * Copy bitmap data from (aColorBitmap,aAlphaBitmap) to aFbsImage.
 */
extern void CopyBitmapsL
(
    CLcdGraphicsDriver& /* aDriver */,  // not used yet
    CLcdFbsImage&       aFbsImage,
    CFbsBitmap*         aColorBitmap,
    CFbsBitmap*         aAlphaBitmap,
    TBool               aInvertMask
)
{
    //
    // First copy the the pixel color data.
    //
    CLcdFbsImage::CopyBitmapL(aFbsImage.ColorBitmap(), aColorBitmap, EFalse);

    //
    // If target colour bitmap contains alpha channel, copy any alpha data
    // into it.
    //
    if (aFbsImage.ColorBitmap()->DisplayMode() == EColor16MA)
    {
        ASSERT(NULL == aFbsImage.AlphaBitmap());
        if (aAlphaBitmap)
        {
            SetAlphaChannelL(aFbsImage.ColorBitmap(), aAlphaBitmap, aInvertMask);
        }
#ifdef _DEBUG
        else
        {
            ASSERT(CheckOpaque(aFbsImage.ColorBitmap()));
        }
#endif
    }
    else if (aFbsImage.AlphaBitmap())
    {
        if (aAlphaBitmap)
        {
            CLcdFbsImage::CopyBitmapL(aFbsImage.AlphaBitmap(), aAlphaBitmap, EFalse);
        }
        else if (aColorBitmap->DisplayMode() == EColor16MA)
        {
            GetAlphaChannelL(aFbsImage.AlphaBitmap(), aColorBitmap, aInvertMask);
        }
    }
}


//
// Set the alpha channel in a bitmap in a mode that contains alpha
// information from an alpha bitmap.
//
// Not the cheapest, but this is hopefully an edge case.
//
// EColor16M,Gray256 is the most likely format, so a dedicated blitter
// could help.
//
LOCAL_C void SetAlphaChannelL(CFbsBitmap* aBitmap, CFbsBitmap* aAlphaBitmap, TBool aInvert)
{
    ASSERT(aBitmap->DisplayMode() == EColor16MA);
    if (aAlphaBitmap->DisplayMode() != EGray256)
    {
        CFbsBitmap* alpha = CLcdFbsImage::CopyBitmapL(aAlphaBitmap, EGray256, EFalse);
        SetAlphaChannel(aBitmap, alpha, aInvert);
        delete alpha;
    }
    else
    {
        SetAlphaChannel(aBitmap, aAlphaBitmap, aInvert);
    }
}

//
// Set the alpha channel in an EColor16MA bitmap from an EGray256
// bitmap.
//
LOCAL_C void SetAlphaChannel(CFbsBitmap* aBitmap, CFbsBitmap* aGray256Bitmap, TBool aInvert)
{
    ASSERT(aBitmap->DisplayMode()        == EColor16MA);
    ASSERT(aGray256Bitmap->DisplayMode() == EGray256);

    TAcceleratedBitmapSpec targetSpec(aBitmap);
    TAcceleratedBitmapInfo target;
    TAcceleratedBitmapSpec sourceSpec(aGray256Bitmap);
    TAcceleratedBitmapInfo source;
    TBitmapLockCount count;

    targetSpec.Lock(count);
    sourceSpec.Lock(count);

    targetSpec.GetInfo(target);
    sourceSpec.GetInfo(source);

    ASSERT(target.iSize == source.iSize);

    TUint8* tgtAddress   = target.iAddress;
    TInt    tgtLinePitch = target.iLinePitch;
    TUint8* srcAddress   = source.iAddress;
    TInt    srcLinePitch = source.iLinePitch;

    TUint32 mask = aInvert ? 0xff:0x00;
    for (TInt h=target.iSize.iHeight; --h>=0;)
    {
        TUint8*  src = srcAddress;
        TUint32* tgt = (TUint32*)tgtAddress;
        TUint32* end = tgt + target.iSize.iWidth;
        while (tgt < end)
        {
            TUint32 alpha = (*src) ^ mask;
            TUint32 pixel = *tgt;
            pixel &= 0x00ffffff;
            pixel |= (alpha << 24);
            *tgt = pixel;
            ++src;
            ++tgt;
        }
        tgtAddress += tgtLinePitch;
        srcAddress += srcLinePitch;
    }

    sourceSpec.Unlock(count);
    targetSpec.Unlock(count);
}


//
// Copy alpha information from EColor16MA bitmap aSource, into EGray2 or EGray256
// bitmap aTarget.
//
LOCAL_C void GetAlphaChannelL(CFbsBitmap* aTarget, CFbsBitmap* aSource, TBool aInvert)
{
    ASSERT(aTarget->DisplayMode() == EGray2 || aTarget->DisplayMode() == EGray256);
    if (aTarget->DisplayMode() == EGray2)
    {
        TSize size = aSource->SizeInPixels();
        CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmap);
        User::LeaveIfError(bitmap->Create(size, EGray256));
        GetAlphaChannel(bitmap, aSource, aInvert);
        CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(aTarget);
        CleanupStack::PushL(device);
        CFbsBitGc* context = NULL;
        User::LeaveIfError(device->CreateContext(context));
        context->BitBlt(TPoint(), bitmap);
        delete context;
        CleanupStack::PopAndDestroy(device);
        CleanupStack::PopAndDestroy(bitmap);
    }
    else
    {
        GetAlphaChannel(aTarget, aSource, aInvert);
    }
}

//
// Copy alpha channel from EColor16MA bitmap into an EGray256 bitmap
//
LOCAL_C void GetAlphaChannel(CFbsBitmap* aTargetGray256, CFbsBitmap* aSourceColor16MA, TBool aInvert)
{
    ASSERT(aTargetGray256->DisplayMode()   == EGray256);
    ASSERT(aSourceColor16MA->DisplayMode() == EColor16MA);

    TAcceleratedBitmapSpec targetSpec(aTargetGray256);
    TAcceleratedBitmapInfo target;
    TAcceleratedBitmapSpec sourceSpec(aSourceColor16MA);
    TAcceleratedBitmapInfo source;
    TBitmapLockCount count;

    targetSpec.Lock(count);
    sourceSpec.Lock(count);

    targetSpec.GetInfo(target);
    sourceSpec.GetInfo(source);

    ASSERT(target.iSize == source.iSize);

    TUint8* tgtAddress   = target.iAddress;
    TInt    tgtLinePitch = target.iLinePitch;
    TUint8* srcAddress   = source.iAddress;
    TInt    srcLinePitch = source.iLinePitch;

    TUint32 mask = aInvert ? 0xff:0x00;
    for (TInt h=target.iSize.iHeight; --h>=0;)
    {
        TUint32*  src = (TUint32*)srcAddress;
        TUint8*   tgt = (TUint8*)tgtAddress;
        TUint8*   end = tgt + target.iSize.iWidth;
        while (tgt < end)
        {
            *tgt++ = (TUint8)(((*src++)>>24)^mask);
        }
        tgtAddress += tgtLinePitch;
        srcAddress += srcLinePitch;
    }

    sourceSpec.Unlock(count);
    targetSpec.Unlock(count);
}

#ifdef _DEBUG
LOCAL_C TBool CheckOpaque(CFbsBitmap* aBitmap)
{
    ASSERT(aBitmap->DisplayMode() == EColor16MA);
    TAcceleratedBitmapSpec spec(aBitmap);
    TAcceleratedBitmapInfo info;
    TBitmapLockCount count;

    spec.Lock(count);
    spec.GetInfo(info);

    TUint8* address = info.iAddress;
    TInt    linePitch = info.iLinePitch;
    TUint32 accum = 0xff000000;

    for (TInt h=info.iSize.iHeight; --h>=0;)
    {
        TUint32* src = (TUint32*)address;
        TUint32* end = src + info.iSize.iWidth;
        while (src < end)
        {
            accum &= *src++;
        }
        address += linePitch;
    }

    spec.Unlock(count);

    return (accum == 0xff000000);
}
#endif
