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
#include <implementationproxy.h>

#include "lcdgdrvif.h"
#include "lcdtransform.h"
#include "lcdgdrvutil.h"
#include "lcdgd.hrh"
#include "collision.h"
#include "lcdbitblt.h"
#include "lcd32bpp.h"


inline TInt Color16MURed(TInt aColor16MU)
{
    return (aColor16MU >> 16) & 0xff;
}

inline TInt Color16MUGreen(TInt aColor16MU)
{
    return (aColor16MU >> 8) & 0xff;
}

inline TInt Color16MUBlue(TInt aColor16MU)
{
    return (aColor16MU & 0xff);
}

inline TUint32 ARGB8888ToColor16MU(TUint32 aARGB)
{
    return aARGB & 0xffffff;
}

inline TUint32 Color16MU(TInt aRed, TInt aGreen, TInt aBlue)
{
    return TUint32((aRed << 16) | (aGreen << 8) | aBlue);
}

LOCAL_C TUint32 Color16MUForward(TUint32 aRGB)
{
    return ARGB8888ToColor16MU(aRGB);
}

LOCAL_C TUint32 Color16MUReverse(TUint32 aColor16MU)
{
    return aColor16MU;  // NB: no transparency bits.
}

LOCAL_C TUint32 Color16MUQuantize(TUint32 aRGB)
{
    return aRGB & 0xffffff;
}

//
// DrawRegion support
//


#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
LOCAL_C void DrawRegionColor16MUMaskToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&            aTransform      // includes anchor
);
LOCAL_C void DrawImageColor16MUMaskToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&            aTransform      // includes anchor
);
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
LOCAL_C void DrawRegionColor16MUColor16MUToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&            aTransform      // includes anchor
);

#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void DrawRegionColor16MUAlphaToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);
#endif


LOCAL_C void DrawRegionARGB8888AlphaChannelToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);

//
// BitBlt support
//

LOCAL_C void BitBltColor16MUToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor16MU
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
LOCAL_C void BitBltColor16MUGray2ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor16MUGray2
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
LOCAL_C void BitBltColor16MUColor16MUToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor16MUColor16MU
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BitBltColor16MUGray256ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor16MUGray256
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
#endif

//
// Image types supported by this DLL.
//
#define COLOR16MU_OPAQUE IMAGE_TYPE(EColor16MU,ENone,ETransparencyNone)
#define COLOR16MU_MASK_1BPP IMAGE_TYPE(EColor16MU,EGray2,ETransparencyMaskBitmap)
#define COLOR16MU_MASK_MATCHED IMAGE_TYPE(EColor16MU,EColor16MU,ETransparencyMaskBitmap)
#define COLOR16MU_ALPHA IMAGE_TYPE(EColor16MU,EGray256,ETransparencyAlphaBitmap)

const TImageRenderer ImageRendererArray[] =
{
    //
    // SrcOver
    //
    /*           target,          source,          transform mask,      function */
    RENDER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_OPAQUE, KTransSimpleMask, BitBltThirtyTwoBppSimple),
    RENDER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_OPAQUE, KTransAllMask, BitBltThirtyTwoBpp),
#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
    RENDER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_MASK_1BPP, KTransNoneMask, DrawImageColor16MUMaskToColor16MU),
    RENDER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_MASK_1BPP, KTransAllMask, DrawRegionColor16MUMaskToColor16MU),
#endif
#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    RENDER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_MASK_MATCHED, KTransAllMask, DrawRegionColor16MUColor16MUToColor16MU),
#endif
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    RENDER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_ALPHA, KTransAllMask, DrawRegionColor16MUAlphaToColor16MU),
#endif
    RENDER_ENTRY(COLOR16MU_OPAQUE, JAVA_OPAQUE_IMAGE, KTransAllMask, BitBltARGB8888ToColor16MU),
    RENDER_ENTRY(COLOR16MU_OPAQUE, JAVA_ALPHA_IMAGE, KTransAllMask, DrawRegionARGB8888AlphaChannelToColor16MU),

    //
    // SrcCopy
    //
    /*            target,          source,              function */
    BLITTER_ENTRY(COLOR16MU_OPAQUE, COLOR16MU_OPAQUE, BitBltARGB8888ToColor16MU),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE,  COLOR16MU_OPAQUE, BitBltColor16MUToARGB8888),
    BLITTER_ENTRY(COLOR16MU_OPAQUE, JAVA_OPAQUE_IMAGE, BitBltARGB8888ToColor16MU),
    BLITTER_ENTRY(JAVA_OPAQUE_IMAGE, COLOR16MU_OPAQUE, BitBltColor16MUToARGB8888),
#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
    BLITTER_ENTRY(COLOR16MU_MASK_1BPP, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor16MUGray2),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR16MU_MASK_1BPP, BitBltColor16MUGray2ToARGB8888),
    BLITTER_ENTRY(COLOR16MU_MASK_1BPP, COLOR16MU_MASK_1BPP, BitBltThirtyTwoBppOneBpp),
#endif
#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    BLITTER_ENTRY(COLOR16MU_MASK_MATCHED, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor16MUColor16MU),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR16MU_MASK_MATCHED, BitBltColor16MUColor16MUToARGB8888),
    BLITTER_ENTRY(COLOR16MU_MASK_MATCHED, COLOR16MU_MASK_MATCHED, BitBltThirtyTwoBppThirtyTwoBpp),
#endif
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    BLITTER_ENTRY(COLOR16MU_ALPHA, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor16MUGray256),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR16MU_ALPHA, BitBltColor16MUGray256ToARGB8888),
    BLITTER_ENTRY(COLOR16MU_ALPHA, COLOR16MU_ALPHA, BitBltThirtyTwoBppEightBpp),
#endif
};
const TInt ImageRendererCount = sizeof(ImageRendererArray)/sizeof(ImageRendererArray[0]);

const TCollisionDetector CollisionDetectorArray[] =
{
//
//  Supported collision detectors.
//
    COLLISION_DETECTOR_ENTRY(ENoneBit,ETransparencyNoneBit,        EGray2Bit,ETransparencyMaskBitmapBit,   DetectCollisionGray2Gray2),
    COLLISION_DETECTOR_ENTRY(EGray2Bit,ETransparencyMaskBitmapBit, ENoneBit,ETransparencyNoneBit,          DetectCollisionGray2Gray2),
    COLLISION_DETECTOR_ENTRY(EGray2Bit,ETransparencyMaskBitmapBit, EGray2Bit,ETransparencyMaskBitmapBit,   DetectCollisionGray2Gray2),

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    COLLISION_DETECTOR_ENTRY(ENoneBit,ETransparencyNoneBit,            EGray256Bit,ETransparencyAlphaBitmapBit,    DetectCollisionGray256Gray256),
    COLLISION_DETECTOR_ENTRY(EGray2Bit,ETransparencyMaskBitmapBit,     EGray256Bit,ETransparencyAlphaBitmapBit,    DetectCollisionGray2Gray256),
    COLLISION_DETECTOR_ENTRY(EGray256Bit,ETransparencyAlphaBitmapBit,  EGray2Bit,ETransparencyMaskBitmapBit,       DetectCollisionGray256Gray2),
    COLLISION_DETECTOR_ENTRY(EGray256Bit,ETransparencyAlphaBitmapBit,  ENoneBit,ETransparencyNoneBit,              DetectCollisionGray256Gray256),
    COLLISION_DETECTOR_ENTRY(EGray256Bit,ETransparencyAlphaBitmapBit,  EGray256Bit,ETransparencyAlphaBitmapBit,    DetectCollisionGray256Gray256),
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    COLLISION_DETECTOR_ENTRY(KGenericModesMask,KGenericTransMask,  KGenericModesMask,KGenericTransMask,    GenericDetectCollision),
#endif
};
const TInt CollisionDetectorCount = sizeof(CollisionDetectorArray)/sizeof(CollisionDetectorArray[0]);

const TColorMap ColorMapArray[] =
{
    { EColor16MU, Color16MUForward, Color16MUReverse, Color16MUQuantize }
};
const TInt ColorMapCount = sizeof(ColorMapArray)/sizeof(ColorMapArray[0]);

const TDrawFunctions DrawFunctionsArray[] =
{
    {
        EColor16MU,
        CLcdGraphicsDevice::ECapFillTriangle,
        NULL,   // drawLine
        NULL,   // drawRect
        NULL,   // fillRect
        NULL,   // drawArc
        NULL,   // fillArc
        &FillTriangle32Bpp,
        NULL    // drawText
    }
};
const TInt DrawFunctionsCount = sizeof(DrawFunctionsArray)/sizeof(DrawFunctionsArray[0]);

const TInt KVersionBuild = 0;

_LIT(KProviderSymbianSoftwareLtd, "Symbian Software Ltd");

LOCAL_C CLcdGraphicsDriverImpl* CreateColor16MUDriverL();

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(LCDGD_COLOR16MU_IMPLEMENTATION_UID, CreateColor16MUDriverL)
};

/**
 * Ordinal 1
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

CLcdGraphicsDriverImpl* CreateColor16MUDriverL()
{
    CLcdGraphicsDriver::TDriverInfo driverInfo;

    driverInfo.iVersion      = TVersion(KLcdImgVersionMajor, KLcdImgVersionMinor, KVersionBuild);
    driverInfo.iProvider     = KProviderSymbianSoftwareLtd;
    driverInfo.iTransparencyCaps = CAP_1BPP_MASK_BITMAP|CAP_MATCHED_MASK_BITMAP|CAP_ALPHA_BITMAP;
    driverInfo.iARGB8888Mode = EColorARGB8888;
    driverInfo.iDisplayMode = EColor16MU;

    return new(ELeave) CLcdGraphicsDriverImpl
           (
               driverInfo,
               ImageRendererArray,
               ImageRendererCount,
               ColorMapArray,
               ColorMapCount,
               CollisionDetectorArray,
               CollisionDetectorCount,
               DrawFunctionsArray,
               DrawFunctionsCount
           );
}

// ***************************************************************
// Render Functions
// ***************************************************************

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
LOCAL_C void DrawRegionColor16MUMaskToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // mask
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray2);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

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

    //
    // Iterate over destination pixels.
    //
    dstAddress += (y*dstLinePitch + x*sizeof(TUint32));
    for (; h>0; --h)
    {
        TUint32* dst = (TUint32*)(dstAddress);
        TUint32* end = dst + width;
        u=u0;
        v=v0;
        while (dst < end)
        {
            TInt m = *(mskAddress + v*mskLinePitch + (u>>3));
            if (m & (1<<(u&0x7)))
            {
                TUint32 srcColor = *(TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));
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

LOCAL_C void DrawImageColor16MUMaskToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // mask
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray2);

    ASSERT(aTransform.iDuDx == 1);
    ASSERT(aTransform.iDuDy == 0);
    ASSERT(aTransform.iDvDx == 0);
    ASSERT(aTransform.iDvDy == 1);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    ASSERT(srcPoint.iX == (aDstRect.iTl.iX + aTransform.iU0));
    ASSERT(srcPoint.iY == (aDstRect.iTl.iY + aTransform.iV0));

    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;
    TInt mu;        // mask u coord
    TInt m = 0;         // mask byte

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
    const TUint32* srcStart = (TUint32*)srcAddress;
    const TUint32* srcLimit = (TUint32*)(srcAddress + (srcSize.iHeight-1)*dstLinePitch + srcSize.iWidth*sizeof(TUint32));
#endif

    //
    // Start address of destination pixels.
    //
    dstAddress += ((y*dstLinePitch) + (x*sizeof(TUint32)));

    //
    // Start address of source pixels
    //
    srcAddress += ((v*srcLinePitch) + (u*sizeof(TUint32)));
    mskAddress += ((v*mskLinePitch) + (u>>3));

    for (; h>0; --h)
    {
        TUint32* src = (TUint32*)srcAddress;
        TUint8*  msk = mskAddress;
        TUint32* dst = (TUint32*)dstAddress;
        TUint32* end = dst + width;
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
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
LOCAL_C void BlitLineColor16MUColor16MUToColor16MU
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* maskAddr = aMaskAddress;

    while (dst < end)
    {
        TUint32 dstColor = *dst;
        TUint32 srcColor = *(TUint32*)colorAddr;
        TUint32 srcMask = *(TUint32*)maskAddr;

        dstColor = (dstColor & ~srcMask) | (srcColor & srcMask);

        *dst++ = dstColor;
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}


//
//
//
LOCAL_C void DrawRegionColor16MUColor16MUToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // mask
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EColor16MU);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor16MUColor16MUToColor16MU);
}
#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BlitLineColor16MUAlphaToColor16MU
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aAlphaAddress,
    TInt    aAlphaPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* alphaAddr = aAlphaAddress;

    while (dst < end)
    {
        TUint alpha = *alphaAddr;
        if (alpha == 0xFF)
        {
            *dst = *(TUint32*)colorAddr;
        }
        else
        {
            if (alpha)
            {
                TUint32 dstColor = *dst;
                TUint32 srcColor = *(TUint32*)colorAddr;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend32(alpha, Color16MURed(srcColor), Color16MURed(dstColor));
                TUint32 dg = Blend32(alpha, Color16MUGreen(srcColor), Color16MUGreen(dstColor));
                TUint32 db = Blend32(alpha, Color16MUBlue(srcColor), Color16MUBlue(dstColor));

                ASSERT(dr < 256);
                ASSERT(dg < 256);
                ASSERT(db < 256);

                // map back to Color16MU
                *dst = Color16MU(dr, dg, db);
            }
        }

        // Advance to next pixel in the line in each bitmap.
        dst++;
        colorAddr += aColorPixelPitch;
        alphaAddr += aAlphaPixelPitch;
    }
}

//
//
//
LOCAL_C void DrawRegionColor16MUAlphaToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // alpha
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor16MUAlphaToColor16MU);
}
#endif


LOCAL_C void BlitLineARGB8888AlphaChannelToColor16MU
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;

    while (dst < end)
    {
        TUint32 argb = *(TUint32*)colorAddr;
        TUint32 alpha = argb >> 24;

        if (alpha == 0xFF)
        {
            *dst = ARGB8888ToColor16MU(argb);
        }
        else
        {
            if (alpha)
            {
                TUint32 dstColor = *dst;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend32(alpha, Color16MURed(argb), Color16MURed(dstColor));
                TUint32 dg = Blend32(alpha, Color16MUGreen(argb), Color16MUGreen(dstColor));
                TUint32 db = Blend32(alpha, Color16MUBlue(argb), Color16MUBlue(dstColor));

                ASSERT(dr < 256);
                ASSERT(dg < 256);
                ASSERT(db < 256);

                // map back to Color16MU
                *dst = Color16MU(dr, dg, db);
            }
        }

        // Advance to next pixel in the line in each bitmap.
        dst++;
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void DrawRegionARGB8888AlphaChannelToColor16MU
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineARGB8888AlphaChannelToColor16MU);
}


// ***************************************************************
// BitBlt Functions
// ***************************************************************

LOCAL_C void BlitLineColor16MUToARGB8888
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;

    while (dst < end)
    {
        // Force alpha to opaque, because top bits in Java are 'don't care'.
        *dst++ = *(TUint32*)colorAddr | 0xFF000000;
        colorAddr += aColorPixelPitch;
    }
}

//
// Color16MU -> ARGB8888
//
LOCAL_C void BitBltColor16MUToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);

    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);

    ASSERT(aDstColorBitmap->iDisplayMode == EColorARGB8888);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor16MUToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor16MU
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;

    while (dst < end)
    {
        *dst++ = ARGB8888ToColor16MU(*(TUint32*)colorAddr);
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void BitBltARGB8888ToColor16MU
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);

    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);

    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888 || aSrcColorBitmap->iDisplayMode == EColor16MU);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineARGB8888ToColor16MU);
}

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
//
// Color16MU, Gray2 -> ARGB8888
//
LOCAL_C void BitBltColor16MUGray2ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,    // ARGB32 array
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);
    ASSERT(aSrcAlphaBitmap);
    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);
    ASSERT(aSrcAlphaBitmap->iAddress != NULL);
    ASSERT(aDstColorBitmap->iDisplayMode == EColorARGB8888);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray2);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

    TUint8* dstAddress = aDstColorBitmap->iAddress;
    TUint8* srcAddress = aSrcColorBitmap->iAddress;
    TUint8* mskAddress = aSrcAlphaBitmap->iAddress;

    TInt dstLinePitch = aDstColorBitmap->iLinePitch;
    TInt srcLinePitch = aSrcColorBitmap->iLinePitch;
    TInt mskLinePitch = aSrcAlphaBitmap->iLinePitch;

#ifdef _DEBUG
    const TSize   srcSize = aSrcColorBitmap->iSize;
    const TUint8* srcBeg = srcAddress;
    const TUint8* srcEnd = srcAddress + srcLinePitch*(srcSize.iHeight - 1) + (srcSize.iWidth-1)*sizeof(TUint32);

    const TUint8* mskBeg = mskAddress;
    const TUint8* mskEnd = mskAddress + mskLinePitch*(srcSize.iHeight - 1) + ((srcSize.iWidth-1)>>3);

    const TSize   dstSize = aDstColorBitmap->iSize;
    const TUint32* dstBeg = (TUint32*)dstAddress;
    const TUint32* dstEnd = (TUint32*)(dstAddress + dstLinePitch*(dstSize.iHeight - 1) + (dstSize.iWidth-1)*sizeof(TUint32));
#endif

    dstAddress += dstLinePitch*aDstRect.iTl.iY;
    dstAddress += aDstRect.iTl.iX*sizeof(TUint32);

    TInt height = aDstRect.Height();
    TInt width  = aDstRect.Width();
    for (; height>0; --height)
    {
        TUint32* dst = (TUint32*)dstAddress;
        TUint32* end = dst + width;

        u=u0;
        v=v0;
        while (dst < end)
        {
            TUint8* src = srcAddress + v*srcLinePitch + u*sizeof(TUint32);
            TUint8* msk = mskAddress + v*mskLinePitch + (u>>3);

#ifdef _DEBUG
            ASSERT(srcBeg <= src && src <= srcEnd);
            ASSERT(mskBeg <= msk && msk <= mskEnd);
            ASSERT(dstBeg <= dst && dst <= dstEnd);
#endif

            TUint8  smsk = *msk;

            TUint32 dpix = *(TUint32*)src;
            if (smsk & 1<<(u&0x7))
            {
                dpix |= 0xff000000;
            }
            else
            {
                dpix &= 0x00ffffff;
            }

            *dst++ = dpix;
            u+=dudx;
            v+=dvdx;
        }
        u0+=dudy;
        v0+=dvdy;
        dstAddress += dstLinePitch;
    }
}

//
// ARGB8888 -> Color16MU, Gray2
//
LOCAL_C void BitBltARGB8888ToColor16MUGray2
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aDstAlphaBitmap);
    ASSERT(aSrcColorBitmap);

    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aDstAlphaBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);

    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EGray2);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

    TUint8* srcAddress = aSrcColorBitmap->iAddress;
    TUint8* dstAddress = aDstColorBitmap->iAddress;
    TUint8* mskAddress = aDstAlphaBitmap->iAddress;

    TInt srcLinePitch = aSrcColorBitmap->iLinePitch;
    TInt dstLinePitch = aDstColorBitmap->iLinePitch;
    TInt mskLinePitch = aDstAlphaBitmap->iLinePitch;

#ifdef _DEBUG
    const TSize   srcSize = aSrcColorBitmap->iSize;
    const TUint32* srcBeg = (TUint32*)srcAddress;
    const TUint32* srcEnd = (TUint32*)(srcAddress + srcLinePitch*(srcSize.iHeight - 1) + (srcSize.iWidth-1)*sizeof(TUint32));

    const TSize   dstSize = aDstColorBitmap->iSize;
    const TUint32* dstBeg = (TUint32*)dstAddress;
    const TUint32* dstEnd = (TUint32*)(dstAddress + dstLinePitch*(dstSize.iHeight - 1) + (dstSize.iWidth-1)*sizeof(TUint32));

    const TSize mskSize = aDstAlphaBitmap->iSize;
    const TUint8* mskBeg = mskAddress;
    const TUint8* mskEnd = mskAddress + mskLinePitch*(mskSize.iHeight-1) + ((mskSize.iWidth-1)>>3);
#endif


    dstAddress += dstLinePitch*aDstRect.iTl.iY;
    dstAddress += aDstRect.iTl.iX*sizeof(TUint32);

    mskAddress += mskLinePitch*aDstRect.iTl.iY;
    mskAddress += (aDstRect.iTl.iX>>3);

    TInt height = aDstRect.Height();
    TInt width  = aDstRect.Width();

    const TInt x0 = aDstRect.iTl.iX;
    const TInt x1 = Min((aDstRect.iTl.iX | 0x7)+1, aDstRect.iBr.iX);
    const TInt x2 = (aDstRect.iBr.iX & ~0x7);
    const TInt x3 = aDstRect.iBr.iX;

    for (; height>0; --height)
    {
        TUint32* dst = (TUint32*)dstAddress;
        TUint8*  msk = mskAddress;

#ifdef _DEBUG
        ASSERT(mskBeg <= msk && msk <= mskEnd);
        ASSERT(dstBeg <= dst && dst <= dstEnd);
#endif

        TInt x;

        u=u0;
        v=v0;

        //
        // Leading mask byte
        //
        for (x = x0; x<x1; x++)
        {
            TUint32* src = (TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));

#ifdef _DEBUG
            ASSERT(srcBeg <= src && src <= srcEnd);
            ASSERT(mskBeg <= msk && msk <= mskEnd);
            ASSERT(dstBeg <= dst && dst <= dstEnd);
#endif

            TUint32 pixel = *src;
            TUint8  alpha = pixel>>24;

            TInt xBit = (1<<(x&0x7));
            if (alpha == 0xff)
            {
                *msk |= xBit;
            }
            else
            {
                *msk &= ~xBit;
            }

            *dst++ = ARGB8888ToColor16MU(pixel);
            u+=dudx;
            v+=dvdx;
        }
        ++msk;

        //
        // Middle section
        //
        for (ASSERT(x==x1); x<x2; x+=8)
        {
            TUint8 mask = 0;

            TUint32* end = dst + 8;
            TInt mbit = 1;
            while (dst < end)
            {
                TUint32* src = (TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));

#ifdef _DEBUG
                ASSERT(srcBeg <= src && src <= srcEnd);
                ASSERT(dstBeg <= dst && dst <= dstEnd);
#endif

                TUint32 pixel = *src;
                if ((pixel >> 24)==0xff)
                {
                    mask |= mbit;
                }
                *dst++ = ARGB8888ToColor16MU(pixel);
                mbit <<= 1;
                u+=dudx;
                v+=dvdx;
            }

            ASSERT(mskBeg <= msk && msk <= mskEnd);

            *msk++ = mask;
        }

        //
        // Trailing mask byte
        //
        for (ASSERT((x>=x2)&&(x<=x3)); x<x3; x++)
        {
            TUint32* src = (TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));

#ifdef _DEBUG
            ASSERT(srcBeg <= src && src <= srcEnd);
            ASSERT(dstBeg <= dst && dst <= dstEnd);
            ASSERT(mskBeg <= msk && msk <= mskEnd);
#endif

            TUint32 pixel = *src;
            TUint8  alpha = pixel>>24;

            TInt xBit = (1<<(x&0x7));
            if (alpha == 0xff)
            {
                *msk |= xBit;
            }
            else
            {
                *msk &= ~xBit;
            }

            *dst++ = ARGB8888ToColor16MU(pixel);
            u+=dudx;
            v+=dvdx;
        }

        u0+=dudy;
        v0+=dvdy;
        dstAddress += dstLinePitch;
        mskAddress += mskLinePitch;
    }
}
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
LOCAL_C void BlitLineColor16MUColor16MUToARGB8888
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* maskAddr = aMaskAddress;

    while (dst < end)
    {
        TUint32 srcColor = *(TUint32*)colorAddr;
        TUint32 srcMask = *(TUint32*)maskAddr;

        ASSERT((srcMask == 0) || (srcMask == 0xffffff));

        *dst++ = (srcMask << 24) | (srcColor & 0xffffff);
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}


//
// Color16MU, Color16MU -> ARGB8888
//
LOCAL_C void BitBltColor16MUColor16MUToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,    // ARGB32 array
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);
    ASSERT(aSrcAlphaBitmap);
    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);
    ASSERT(aSrcAlphaBitmap->iAddress != NULL);
    ASSERT(aDstColorBitmap->iDisplayMode == EColorARGB8888);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EColor16MU);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor16MUColor16MUToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor16MUColor16MU
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint32* maskAddr = aMaskAddress;

    while (dst < end)
    {
        TUint32 srcColor = *(TUint32*)colorAddr;

        *dst++ = ARGB8888ToColor16MU(srcColor);
        *maskAddr++ = ((srcColor >> 24) == 0xff) ? 0xffffff : 0;

        colorAddr += aColorPixelPitch;
    }
}


//
// ARGB8888 -> Color16MUColor16MU
//
LOCAL_C void BitBltARGB8888ToColor16MUColor16MU
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,    // ARGB32 array
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aDstAlphaBitmap);
    ASSERT(aSrcColorBitmap);

    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aDstAlphaBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);

    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aDstAlphaBitmap, EFalse, BlitLineARGB8888ToColor16MUColor16MU);
}
#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BlitLineColor16MUGray256ToARGB8888
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* maskAddr = aMaskAddress;

    while (dst < end)
    {
        TUint32 srcColor = *(TUint32*)colorAddr;

        *dst++ = (*maskAddr << 24) | (srcColor & 0xffffff);

        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}


//
// Color16MU, Gray256 -> ARGB8888
//
LOCAL_C void BitBltColor16MUGray256ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);
    ASSERT(aSrcAlphaBitmap);
    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);
    ASSERT(aSrcAlphaBitmap->iAddress != NULL);
    ASSERT(aDstColorBitmap->iDisplayMode == EColorARGB8888);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor16MUGray256ToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor16MUGray256
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint32* dst = (TUint32*)(aDstAddress);
    TUint32* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* maskAddr = aMaskAddress;

    ASSERT(aMaskPixelPitch == sizeof(TUint8));
    UNUSED(aMaskPixelPitch);    // ASSERT is the only use otherwise.

    while (dst < end)
    {
        TUint32 srcColor = *(TUint32*)colorAddr;

        *dst++ = ARGB8888ToColor16MU(srcColor);
        *maskAddr++ = (srcColor >> 24);

        colorAddr += aColorPixelPitch;
    }
}


//
// ARGB8888 -> Color16MU, Gray256
//
LOCAL_C void BitBltARGB8888ToColor16MUGray256
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aDstAlphaBitmap);
    ASSERT(aSrcColorBitmap);

    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aDstAlphaBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);

    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MU);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EGray256);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aDstAlphaBitmap, EFalse, BlitLineARGB8888ToColor16MUGray256);
}

#endif
