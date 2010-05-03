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
#include <j2me/jdebug.h>


#include "lcdgdrvif.h"
#include "lcdtransform.h"
#include "lcd16bpp.h"
#include "lcdgdrvutil.h"
#include "lcdgd.hrh"
#include "lcdbitblt.h"
#include "collision.h"

const TInt KColor4KRedShift   = 8;
const TInt KColor4KRedMax     = 0xf;
const TInt KColor4KGreenShift = 4;
const TInt KColor4KGreenMax   = 0xf;
const TInt KColor4KBlueShift  = 0;
const TInt KColor4KBlueMax     = 0xf;

INLINE TInt Color4KRed(TInt aColor4K)
{
    TUint32 red = (aColor4K>>KColor4KRedShift) & KColor4KRedMax;
    return (red << 4) + red;
}

INLINE TInt Color4KGreen(TInt aColor4K)
{
    TUint32 green = (aColor4K>>KColor4KGreenShift) & KColor4KGreenMax;
    return (green << 4) + green;
}

INLINE TInt Color4KBlue(TInt aColor4K)
{
    TUint32 blue = (aColor4K>>KColor4KBlueShift) & KColor4KBlueMax;
    return (blue << 4) + blue;
}

INLINE TInt Color4K(TInt aRed, TInt aGreen, TInt aBlue)
{
    return ((aRed & 0xf0)<<4) | (aGreen & 0xf0) | ((aBlue & 0xf0)>>4);
}

INLINE TUint16 ARGB8888ToColor4K(TUint32 aPixel)
{
    return ((aPixel & 0xf00000) >> 12) |
           ((aPixel & 0x00f000) >> 8)  |
           ((aPixel & 0x0000f0) >> 4);
}

INLINE TUint32 Color4KToARGB8888(TUint16 aColor4K)
{
    return (Color4KRed(aColor4K)<<16)|(Color4KGreen(aColor4K)<<8)|(Color4KBlue(aColor4K));
}

LOCAL_C INLINE TUint32 Color4KForward(TUint32 aRGB)
{
    return ARGB8888ToColor4K(aRGB);
}

LOCAL_C INLINE TUint32 Color4KReverse(TUint32 aColor4K)
{
    return Color4KToARGB8888(aColor4K);
}

LOCAL_C INLINE TUint32 Color4KQuantize(TUint32 aRGB)
{
    return Color4KReverse(Color4KForward(aRGB));
}


#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void DrawImageColor4KAlphaToColor4K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* aDstAlphaBitmap,  //ignored
    const TRect&                  aDstRect,         // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
    const TLcdTransform&          aTransform        // includes anchor
);
LOCAL_C void DrawRegionColor4KAlphaToColor4K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* aDstAlphaBitmap,  //ignored
    const TRect&                  aDstRect,         // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
    const TLcdTransform&          aTransform        // includes anchor
);
#endif


LOCAL_C void DrawRegionARGB8888AlphaToColor4K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* aDstAlphaBitmap,  // ignored
    const TRect&                  aDstRect,         // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,  // ARGB8888
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
    const TLcdTransform&          aTransform        // includes anchor
);

//
// Blitters
//
LOCAL_C void BitBltColor4KToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

LOCAL_C void BitBltARGB8888ToColor4K
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // ignored
    const TLcdTransform&            aTransform
);

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
LOCAL_C void BitBltColor4KGray2ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor4KGray2
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,    // ignored
    const TLcdTransform&            aTransform
);
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
LOCAL_C void BitBltColor4KColor4KToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,    // ignored
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor4KColor4K
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
LOCAL_C void BitBltColor4KGray256ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor4KGray256
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
// Supported Image Types
//
#define COLOR4K_OPAQUE IMAGE_TYPE(EColor4K,ENone,ETransparencyNone)

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
#define COLOR4K_MASK_1BPP IMAGE_TYPE(EColor4K,EGray2,ETransparencyMaskBitmap)
#endif

#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
#define COLOR4K_MASK_MATCHED IMAGE_TYPE(EColor4K,EColor4K,ETransparencyMaskBitmap)
#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
#define COLOR4K_ALPHA IMAGE_TYPE(EColor4K,EGray256,ETransparencyAlphaBitmap)
#endif

//
// Supportd blitters
//
const TImageRenderer ImageRendererArray[] =
{
    //
    // SrcOver
    //
    /*           target,          source,        transform mask,        function */
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_ALPHA, KTransSimpleMask, DrawImageColor4KAlphaToColor4K),
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_ALPHA, KTransAllMask, DrawRegionColor4KAlphaToColor4K),
#endif
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_OPAQUE, KTransSimpleMask, BitBltSixteenBppSimple),
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_OPAQUE, KTransAllMask, BitBltSixteenBpp),
#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_MASK_1BPP, KTransNoneMask, DrawImage16Bpp1BppTo16BppOpaque),
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_MASK_1BPP, KTransAllMask, DrawRegion16Bpp1BppTo16BppOpaque),
#endif
#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    RENDER_ENTRY(COLOR4K_OPAQUE, COLOR4K_MASK_MATCHED, KTransAllMask, DrawRegion16Bpp16BppTo16BppOpaque),
#endif
    RENDER_ENTRY(COLOR4K_OPAQUE, JAVA_OPAQUE_IMAGE, KTransAllMask, BitBltARGB8888ToColor4K),
    RENDER_ENTRY(COLOR4K_OPAQUE, JAVA_ALPHA_IMAGE, KTransAllMask, DrawRegionARGB8888AlphaToColor4K),

    //
    // SrcCopy
    //
    /*            target,          source,      function */
    SIMPLE_BLITTER_ENTRY(COLOR4K_OPAQUE, COLOR4K_OPAQUE, BitBltSixteenBppSimple),
    BLITTER_ENTRY(COLOR4K_OPAQUE, COLOR4K_OPAQUE, BitBltSixteenBpp),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE,  COLOR4K_OPAQUE, BitBltColor4KToARGB8888),
    BLITTER_ENTRY(COLOR4K_OPAQUE, JAVA_OPAQUE_IMAGE, BitBltARGB8888ToColor4K),
    BLITTER_ENTRY(JAVA_OPAQUE_IMAGE, COLOR4K_OPAQUE, BitBltColor4KToARGB8888),
#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
    BLITTER_ENTRY(COLOR4K_MASK_1BPP, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor4KGray2),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR4K_MASK_1BPP, BitBltColor4KGray2ToARGB8888),
    BLITTER_ENTRY(COLOR4K_MASK_1BPP, COLOR4K_MASK_1BPP, BitBltSixteenBppOneBpp),
#endif
#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    BLITTER_ENTRY(COLOR4K_MASK_MATCHED, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor4KColor4K),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR4K_MASK_MATCHED, BitBltColor4KColor4KToARGB8888),
    BLITTER_ENTRY(COLOR4K_MASK_MATCHED, COLOR4K_MASK_MATCHED, BitBltSixteenBppSixteenBpp),
#endif
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    BLITTER_ENTRY(COLOR4K_ALPHA, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor4KGray256),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR4K_ALPHA, BitBltColor4KGray256ToARGB8888),
    BLITTER_ENTRY(COLOR4K_ALPHA, COLOR4K_ALPHA, BitBltSixteenBppEightBpp),
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
    { EColor4K, Color4KForward, Color4KReverse, Color4KQuantize }
};
const TInt ColorMapCount = sizeof(ColorMapArray)/sizeof(ColorMapArray[0]);

const TDrawFunctions DrawFunctionsArray[] =
{
    {
        EColor4K,
        CLcdGraphicsDevice::ECapFillTriangle,
        NULL,   // drawLine
        NULL,   // drawRect
        NULL,   // fillRect
        NULL,   // drawArc
        NULL,   // fillArc
        &FillTriangle16Bpp,
        NULL    // drawText
    }
};
const TInt DrawFunctionsCount = sizeof(DrawFunctionsArray)/sizeof(DrawFunctionsArray[0]);


const TInt KVersionBuild = 0;

_LIT(KProviderSymbianSoftwareLtd, "Symbian Software Ltd");


LOCAL_C CLcdGraphicsDriverImpl* CreateColor4KDriverL();

//
// Could support more than one implementation
//
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(LCDGD_COLOR4K_IMPLEMENTATION_UID, CreateColor4KDriverL)
};

/**
 * Exported @ Ordinal 1
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

CLcdGraphicsDriverImpl* CreateColor4KDriverL()
{
    CLcdGraphicsDriver::TDriverInfo driverInfo;

    driverInfo.iVersion      = TVersion(KLcdImgVersionMajor, KLcdImgVersionMinor, KVersionBuild);
    driverInfo.iProvider     = KProviderSymbianSoftwareLtd;
    driverInfo.iTransparencyCaps = CAP_1BPP_MASK_BITMAP|CAP_MATCHED_MASK_BITMAP|CAP_ALPHA_BITMAP;
    driverInfo.iDisplayMode = EColor4K;
    driverInfo.iARGB8888Mode = EColorARGB8888;

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

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BlitLineColor4KAlphaToColor4KNoTrans
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    DEBUG_ONLY(aColorPixelPitch),
    TUint8* aAlphaAddress,
    TInt    DEBUG_ONLY(aAlphaPixelPitch)
)
{
    ASSERT(aColorPixelPitch == sizeof(TUint16));
    ASSERT(aAlphaPixelPitch == sizeof(TUint8));

    TUint16* dst = (TUint16*)aDstAddress;
    TUint16* end = dst + aWidth;

    TUint16* colorAddr = (TUint16*)aColorAddress;
    TUint8* alphaAddr = aAlphaAddress;

    while (dst < end)
    {
        TUint alpha = *alphaAddr++;
        if (alpha == 0xFF)
        {
            *dst = *colorAddr;
        }
        else
        {
            if (alpha)
            {
                TUint16 dstColor = *dst;
                TUint16 srcColor = *(TUint16*)colorAddr;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend16(alpha, Color4KRed(srcColor), Color4KRed(dstColor));
                TUint32 dg = Blend16(alpha, Color4KGreen(srcColor), Color4KGreen(dstColor));
                TUint32 db = Blend16(alpha, Color4KBlue(srcColor), Color4KBlue(dstColor));

                ASSERT(dr <= 255);
                ASSERT(dg <= 255);
                ASSERT(db <= 255);

                // map back to Color4K
                *dst = Color4K(dr, dg, db);
            }
        }
        dst++;
        colorAddr++;
    }
}

LOCAL_C void DrawImageColor4KAlphaToColor4K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,             // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
    const TLcdTransform&          aTransform            // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor4KAlphaToColor4KNoTrans);
}

LOCAL_C void BlitLineColor4KAlphaToColor4K
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aAlphaAddress,
    TInt    aAlphaPixelPitch
)
{
    TUint16* dst = (TUint16*)(aDstAddress);
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* alphaAddr = aAlphaAddress;

    while (dst < end)
    {
        TUint alpha = *alphaAddr;
        if (alpha == 0xFF)
        {
            *dst = *(TUint16*)colorAddr;
        }
        else
        {
            if (alpha)
            {
                TUint16 dstColor = *dst;
                TUint16 srcColor = *(TUint16*)colorAddr;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend16(alpha, Color4KRed(srcColor), Color4KRed(dstColor));
                TUint32 dg = Blend16(alpha, Color4KGreen(srcColor), Color4KGreen(dstColor));
                TUint32 db = Blend16(alpha, Color4KBlue(srcColor), Color4KBlue(dstColor));

                ASSERT(dr <= 255);
                ASSERT(dg <= 255);
                ASSERT(db <= 255);

                // map back to Color4K
                *dst = Color4K(dr, dg, db);
            }
        }

        // Advance to next pixel in the line in each bitmap.
        ++dst;  // TUint16* so adds two to the address.
        colorAddr += aColorPixelPitch;
        alphaAddr += aAlphaPixelPitch;
    }
}

LOCAL_C void DrawRegionColor4KAlphaToColor4K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,             // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
    const TLcdTransform&          aTransform            // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor4KAlphaToColor4K);
}
#endif


LOCAL_C void BlitLineARGB8888AlphaToColor4K
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint16* dst = (TUint16*)(aDstAddress);
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;

    while (dst < end)
    {
        TUint32 argb = *(TUint32*)colorAddr;
        TInt alpha = argb>>24;
        if (alpha == 0xFF)
        {
            // Trivial case: opaque
            *dst = ARGB8888ToColor4K(argb);
        }
        else
        {
            // Check for the other trivial case: clear
            if (alpha)
            {
                TUint16 dstColor = *dst;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend16(alpha, ((argb>>16)&0xff), Color4KRed(dstColor));
                TUint32 dg = Blend16(alpha, ((argb>>8)&0xff), Color4KGreen(dstColor));
                TUint32 db = Blend16(alpha, (argb&0xff), Color4KBlue(dstColor));

                ASSERT(dr <= 255);
                ASSERT(dg <= 255);
                ASSERT(db <= 255);

                *dst = Color4K(dr, dg, db);
            }
        }
        dst++;
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void DrawRegionARGB8888AlphaToColor4K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,      // ARGB8888
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineARGB8888AlphaToColor4K);
}

// *************************************************
// Blitters.
// *************************************************

LOCAL_C void BlitLineColor4KToARGB8888
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
        *dst++ = Color4KReverse(*(TUint16*)colorAddr) | 0xFF000000;
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void BitBltColor4KToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aDstColorBitmap->iDisplayMode == EColorARGB8888);
    ASSERT(aSrcColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor4K);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor4KToARGB8888);
}

LOCAL_C void BlitLineARGB8888OpaqueToColor4K
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{
    TUint16* dst = (TUint16*)(aDstAddress);
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;

    while (dst < end)
    {
        *dst++ = ARGB8888ToColor4K(*(TUint32*)colorAddr);
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void BitBltARGB8888ToColor4K
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineARGB8888OpaqueToColor4K);
}


#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
LOCAL_C void BitBltColor4KGray2ToARGB8888
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor4K);
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
            TUint8* src = srcAddress + v*srcLinePitch + u*sizeof(TUint16);
            TUint8* msk = mskAddress + v*mskLinePitch + (u>>3);

            TUint16 spix = *(TUint16*)src;
            TUint8  smsk = *msk;
            TUint32 dpix = (smsk & 1<<(u&0x7))?0xff000000:0;

            dpix |= Color4KToARGB8888(spix);

            *dst++ = dpix;
            u+=dudx;
            v+=dvdx;
        }
        u0+=dudy;
        v0+=dvdy;
        dstAddress += dstLinePitch;
    }
}

LOCAL_C void BitBltARGB8888ToColor4KGray2
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
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

    dstAddress += dstLinePitch*aDstRect.iTl.iY;
    dstAddress += aDstRect.iTl.iX*sizeof(TUint16);

    mskAddress += mskLinePitch*aDstRect.iTl.iY;
    mskAddress += (aDstRect.iTl.iX>>3);

    TInt height = aDstRect.Height();
    TInt width  = aDstRect.Width();

    for (; height>0; --height)
    {
        TUint16* dst = (TUint16*)dstAddress;
        TUint8*  msk = mskAddress;

        TInt x0 = aDstRect.iTl.iX;
        TInt x1 = Min((aDstRect.iTl.iX | 0x7)+1, aDstRect.iBr.iX);
        TInt x2 = (aDstRect.iBr.iX & ~0x7);
        TInt x3 = aDstRect.iBr.iX;
        TInt x;

        u=u0;
        v=v0;

        //
        // Leading mask byte
        //
        for (x = x0; x<x1; x++)
        {
            TUint32 pixel = *(TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));
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

            *dst++ = ARGB8888ToColor4K(pixel);
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

            TUint16* end = dst + 8;
            TInt mbit = 1;
            while (dst < end)
            {
                TUint32 pixel = *(TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));
                if ((pixel >> 24)==0xff)
                {
                    mask |= mbit;
                }
                *dst++ = ARGB8888ToColor4K(pixel);
                mbit <<= 1;
                u+=dudx;
                v+=dvdx;
            }

            *msk++ = mask;
        }

        //
        // Trailing mask byte
        //
        for (ASSERT((x>=x2)&&(x<=x3)); x<x3; x++)
        {
            TUint32 pixel = *(TUint32*)(srcAddress + v*srcLinePitch + u*sizeof(TUint32));
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

            *dst++ = ARGB8888ToColor4K(pixel);
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
LOCAL_C void BlitLineColor4KColor4KToARGB8888
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
        TUint16 smsk = *(TUint16*)maskAddr;
        TUint16 spix = *(TUint16*)colorAddr);

        TUint32 dpix = (smsk << 24);    // bottom 8 bits of smsk should all be 1 if opaque
        ASSERT((smsk == 0) || (smsk == 0x0fff));
        dpix |= Color4KRed(spix)   << 16;
        dpix |= Color4KGreen(spix) << 8;
        dpix |= Color4KBlue(spix);
        *dst++ = dpix;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}

LOCAL_C void BitBltColor4KColor4KToARGB8888
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EColor4K);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor4KColor4KToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor4KColor4K
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint16* dst = (TUint16*)(aDstAddress);
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint16* maskAddr = aMaskAddress;

    ASSERT(aMaskPixelPitch == sizeof(TUint16));
    UNUSED(aMaskPixelPitch);    // ASSERT is the only use otherwise.

    while (dst < end)
    {
        TUint32 pixel = *(TUint32*)colorAddr;
        *dst++ = ARGB8888ToColor4K(pixel);
        *maskAddr++ = (TUint16)(((pixel >> 24) == 0xff) ? 0x0fff : 0);

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void BitBltARGB8888ToColor4KColor4K
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aDstAlphaBitmap, EFalse, BlitLineARGB8888ToColor4KColor4K);
}
#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BlitLineColor4KGray256ToARGB8888
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
        TUint16 spix = *(TUint16*)colorAddr;
        TUint32 dpix = *maskAddr << 24;

        dpix |= Color4KRed(spix)   << 16;
        dpix |= Color4KGreen(spix) << 8;
        dpix |= Color4KBlue(spix);
        *dst++ = dpix;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}

//
// Color4K, Gray256 -> ARGB8888
//
LOCAL_C void BitBltColor4KGray256ToARGB8888
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor4KGray256ToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor4KGray256
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch,
    TUint8* aMaskAddress,
    TInt    aMaskPixelPitch
)
{
    TUint16* dst = (TUint16*)(aDstAddress);
    TUint16* end = dst + aWidth;

    TUint8* colorAddr = aColorAddress;
    TUint8* maskAddr = aMaskAddress;

    ASSERT(aMaskPixelPitch == sizeof(TUint8));
    UNUSED(aMaskPixelPitch);    // ASSERT is the only use otherwise.

    while (dst < end)
    {
        TUint32 pixel = *(TUint32*)colorAddr;
        *dst++ = ARGB8888ToColor4K(pixel);
        *maskAddr++ = pixel >> 24;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void BitBltARGB8888ToColor4KGray256
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor4K);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EGray256);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aDstAlphaBitmap, EFalse, BlitLineARGB8888ToColor4KGray256);
}
#endif

