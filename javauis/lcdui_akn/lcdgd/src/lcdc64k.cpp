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


#include "lcdgd.hrh"
#include "lcdgdrvif.h"
#include "lcdgdrvutil.h"
#include "lcdtransform.h"
#include "lcdbitblt.h"
#include "lcd16bpp.h"
#include "collision.h"

const TInt KColor64KRedBits    = 5;
const TInt KColor64KRedShift   = 11;
const TInt KColor64KRedMax     = (1<<KColor64KRedBits)-1;

const TInt KColor64KGreenBits  = 6;
const TInt KColor64KGreenShift = 5;
const TInt KColor64KGreenMax   = (1<<KColor64KGreenBits)-1;

const TInt KColor64KBlueBits   = 5;
const TInt KColor64KBlueShift  = 0;
const TInt KColor64KBlueMax    = (1<<KColor64KBlueBits)-1;

#ifdef _DEBUG
const TInt KColor64KRedMask    = KColor64KRedMax<<KColor64KRedShift;
const TInt KColor64KGreenMask  = KColor64KGreenMax<<KColor64KGreenShift;
const TInt KColor64KBlueMask   = KColor64KBlueMax<<KColor64KBlueShift;
#endif

INLINE TInt Color64KRed(TInt aColor64K)
{
    TUint32 red = (aColor64K>>KColor64KRedShift) & KColor64KRedMax;

    red = (red << 3) + (red >> 2);

    ASSERT(red <= 255);

    return red;
}

INLINE TInt Color64KGreen(TInt aColor64K)
{
    TUint32 green = (aColor64K>>KColor64KGreenShift) & KColor64KGreenMax;

    green = (green << 2) + (green >> 4);

    ASSERT(green <= 255);

    return green;
}

INLINE TInt Color64KBlue(TInt aColor64K)
{
    TUint32 blue = (aColor64K>>KColor64KBlueShift) & KColor64KBlueMax;

    blue = (blue << 3) + (blue >> 2);

    ASSERT(blue <= 255);

    return blue;
}

INLINE TUint16 Color64K(TInt aRed, TInt aGreen, TInt aBlue)
{
    TUint16 r = (aRed   & 0xf8) << 8;
    TUint16 g = (aGreen & 0xfc) << 3;
    TUint16 b = (aBlue  & 0xf8) >> 3;

    ASSERT((r & (KColor64KGreenMask|KColor64KBlueMask)) == 0);
    ASSERT((g & (KColor64KRedMask  |KColor64KBlueMask)) == 0);
    ASSERT((b & (KColor64KRedMask  |KColor64KGreenMask)) == 0);

    return (r | g | b);
}

INLINE TUint16 ARGB8888ToColor64K(TUint32 aPixel)
{
    return ((aPixel >> 8)  & 0xf800)|   // 5        // 23,16 -> 15,11
           ((aPixel >> 5)  & 0x07E0)|  // 6        // 15,8  -> 10,5
           ((aPixel >> 3)  & 0x001F);  // 5
}

LOCAL_C INLINE TUint32 Color64KForward(TUint32 aRGB)
{
    return ARGB8888ToColor64K(aRGB);
}

LOCAL_C INLINE TUint32 Color64KReverse(TUint32 aColor64K)
{
    return (Color64KRed(aColor64K)<<16)|(Color64KGreen(aColor64K)<<8)|(Color64KBlue(aColor64K));
}

LOCAL_C INLINE TUint32 Color64KQuantize(TUint32 aRGB)
{
    return Color64KReverse(Color64KForward(aRGB));
}

//
// Renderers
//
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void DrawImageColor64KAlphaToColor64K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,         // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,  // alpha
    const TLcdTransform&          aTransform        // includes anchor
);
LOCAL_C void DrawRegionColor64KAlphaToColor64K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* aDstAlphaBitmap,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);
#endif


LOCAL_C void DrawRegionARGB8888AlphaToColor64K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* aDstAlphaBitmap,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,      // ARGB8888
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);

//
// Blitters
//

LOCAL_C void BitBltColor64KToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

LOCAL_C void BitBltARGB8888ToColor64K
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
LOCAL_C void BitBltARGB8888ToColor64KGray2
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

LOCAL_C void BitBltColor64KGray2ToARGB8888
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
LOCAL_C void BitBltColor64KColor64KToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);

LOCAL_C void BitBltARGB8888ToColor64KColor64K
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
LOCAL_C void BitBltColor64KGray256ToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);
LOCAL_C void BitBltARGB8888ToColor64KGray256
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
// Supported image types.
//
#define COLOR64K_OPAQUE IMAGE_TYPE(EColor64K,ENone,ETransparencyNone)
#define COLOR64K_MASK_1BPP IMAGE_TYPE(EColor64K,EGray2,ETransparencyMaskBitmap)
#define COLOR64K_MASK_MATCHED IMAGE_TYPE(EColor64K,EColor64K,ETransparencyMaskBitmap)
#define COLOR64K_ALPHA IMAGE_TYPE(EColor64K,EGray256,ETransparencyAlphaBitmap)

//
// General transforming blitters.
//
const TImageRenderer ImageRendererArray[] =
{
    //
    // SrcOver
    //
    /*           target,          source,          transform mask,      function */
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_ALPHA, KTransSimpleMask, DrawImageColor64KAlphaToColor64K),
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_ALPHA, KTransAllMask, DrawRegionColor64KAlphaToColor64K),
#endif
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_OPAQUE, KTransSimpleMask, BitBltSixteenBppSimple),
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_OPAQUE, KTransAllMask, BitBltSixteenBpp),
#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_MASK_1BPP, KTransNoneMask, DrawImage16Bpp1BppTo16BppOpaque),
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_MASK_1BPP, KTransAllMask, DrawRegion16Bpp1BppTo16BppOpaque),
#endif
#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    RENDER_ENTRY(COLOR64K_OPAQUE, COLOR64K_MASK_MATCHED, KTransAllMask, DrawRegion16Bpp16BppTo16BppOpaque),
#endif
    RENDER_ENTRY(COLOR64K_OPAQUE, JAVA_OPAQUE_IMAGE, KTransAllMask, BitBltARGB8888ToColor64K),
    RENDER_ENTRY(COLOR64K_OPAQUE, JAVA_ALPHA_IMAGE, KTransAllMask, DrawRegionARGB8888AlphaToColor64K),

    //
    // SrcCopy
    //
    /*            target,          source,              function */
    SIMPLE_BLITTER_ENTRY(COLOR64K_OPAQUE, COLOR64K_OPAQUE, BitBltSixteenBppSimple),
    BLITTER_ENTRY(COLOR64K_OPAQUE, COLOR64K_OPAQUE, BitBltSixteenBpp),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE,  COLOR64K_OPAQUE, BitBltColor64KToARGB8888),
    BLITTER_ENTRY(COLOR64K_OPAQUE,     JAVA_OPAQUE_IMAGE, BitBltARGB8888ToColor64K),
    BLITTER_ENTRY(JAVA_OPAQUE_IMAGE, COLOR64K_OPAQUE, BitBltColor64KToARGB8888),
#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
    BLITTER_ENTRY(COLOR64K_MASK_1BPP, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor64KGray2),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR64K_MASK_1BPP, BitBltColor64KGray2ToARGB8888),
    BLITTER_ENTRY(COLOR64K_MASK_1BPP, COLOR64K_MASK_1BPP, BitBltSixteenBppOneBpp),
#endif
#ifdef LCDGD_SUPPORT_MATCHED_MASK_BITMAP
    BLITTER_ENTRY(COLOR64K_MASK_MATCHED, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor64KColor64K),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR64K_MASK_MATCHED, BitBltColor64KColor64KToARGB8888),
    BLITTER_ENTRY(COLOR64K_MASK_MATCHED, COLOR64K_MASK_MATCHED, BitBltSixteenBppSixteenBpp),
#endif
#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
    BLITTER_ENTRY(COLOR64K_ALPHA, JAVA_ALPHA_IMAGE, BitBltARGB8888ToColor64KGray256),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR64K_ALPHA, BitBltColor64KGray256ToARGB8888),
    BLITTER_ENTRY(COLOR64K_ALPHA, COLOR64K_ALPHA, BitBltSixteenBppEightBpp),
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
    { EColor64K, Color64KForward, Color64KReverse, Color64KQuantize }
};
const TInt ColorMapCount = sizeof(ColorMapArray)/sizeof(ColorMapArray[0]);


const TDrawFunctions DrawFunctionsArray[] =
{
    {
        EColor64K,
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

//
// Build number of this DLL.
//
const TInt KVersionBuild = 0;

_LIT(KProviderSymbianSoftwareLtd, "Symbian Software Ltd");

LOCAL_C CLcdGraphicsDriverImpl* CreateColor64KDriverL();

//
// Could support more than one implementation
//
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(LCDGD_COLOR64K_IMPLEMENTATION_UID, CreateColor64KDriverL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

CLcdGraphicsDriverImpl* CreateColor64KDriverL()
{
    CLcdGraphicsDriver::TDriverInfo driverInfo;

    driverInfo.iVersion  = TVersion(KLcdImgVersionMajor, KLcdImgVersionMinor, KVersionBuild);
    driverInfo.iProvider = KProviderSymbianSoftwareLtd;
    driverInfo.iTransparencyCaps = CAP_1BPP_MASK_BITMAP|CAP_MATCHED_MASK_BITMAP|CAP_ALPHA_BITMAP;
    driverInfo.iARGB8888Mode = EColorARGB8888;
    driverInfo.iDisplayMode = EColor64K;

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

// ***********************************************************
// Render (SrcOver) routines
// ***********************************************************

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BlitLineColor64KAlphaToColor64KNoTrans
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

                TUint32 dr = Blend16(alpha, Color64KRed(srcColor), Color64KRed(dstColor));
                TUint32 dg = Blend16(alpha, Color64KGreen(srcColor), Color64KGreen(dstColor));
                TUint32 db = Blend16(alpha, Color64KBlue(srcColor), Color64KBlue(dstColor));

                ASSERT(dr <= 255);
                ASSERT(dg <= 255);
                ASSERT(db <= 255);

                // map back to Color64K
                *dst = Color64K(dr, dg, db);
            }
        }
        dst++;
        colorAddr++;
    }
}

LOCAL_C void DrawImageColor64KAlphaToColor64K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,         // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,  // alpha
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor64KAlphaToColor64KNoTrans);
}

LOCAL_C void BlitLineColor64KAlphaToColor64K
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

                TUint32 dr = Blend16(alpha, Color64KRed(srcColor), Color64KRed(dstColor));
                TUint32 dg = Blend16(alpha, Color64KGreen(srcColor), Color64KGreen(dstColor));
                TUint32 db = Blend16(alpha, Color64KBlue(srcColor), Color64KBlue(dstColor));

                ASSERT(dr <= 255);
                ASSERT(dg <= 255);
                ASSERT(db <= 255);

                // map back to Color64K
                *dst = Color64K(dr, dg, db);
            }
        }

        // Advance to next pixel in the line in each bitmap.
        ++dst;  // TUint16* so adds two to the address.
        colorAddr += aColorPixelPitch;
        alphaAddr += aAlphaPixelPitch;
    }
}

LOCAL_C void DrawRegionColor64KAlphaToColor64K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,         // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,  // alpha
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor64KAlphaToColor64K);
}
#endif


LOCAL_C void BlitLineARGB8888AlphaToColor64K
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
            *dst = ARGB8888ToColor64K(argb);
        }
        else
        {
            // Check for the other trivial case: clear
            if (alpha)
            {
                TUint16 dstColor = *dst;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend16(alpha, ((argb>>16)&0xff), Color64KRed(dstColor));
                TUint32 dg = Blend16(alpha, ((argb>>8)&0xff), Color64KGreen(dstColor));
                TUint32 db = Blend16(alpha, (argb&0xff), Color64KBlue(dstColor));

                ASSERT(dr <= 255);
                ASSERT(dg <= 255);
                ASSERT(db <= 255);

                *dst = Color64K(dr, dg, db);
            }
        }
        dst++;
        colorAddr += aColorPixelPitch;
    }
}

LOCAL_C void DrawRegionARGB8888AlphaToColor64K
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,      //
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,             // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,      // ARGB8888
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,  // ignored
    const TLcdTransform&          aTransform            // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineARGB8888AlphaToColor64K);
}

// ***********************************************************
// Blitter (SrcCopy) functions.
// ***********************************************************

LOCAL_C void BlitLineColor64KToARGB8888
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
        *dst++ = Color64KReverse(*(TUint16*)colorAddr) | 0xFF000000;
        colorAddr += aColorPixelPitch;
    }
}

//
// Color64K -> ARGB8888
//
LOCAL_C void BitBltColor64KToARGB8888
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,            // ARGB32 array
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,        // NULL
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,        // NULL
    const TLcdTransform&            aTransform
)
{
    ASSERT(aDstColorBitmap);
    ASSERT(aSrcColorBitmap);

    ASSERT(aDstColorBitmap->iAddress != NULL);
    ASSERT(aSrcColorBitmap->iAddress != NULL);

    ASSERT(aDstColorBitmap->iDisplayMode == EColorARGB8888);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor64K);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor64KToARGB8888);
}

LOCAL_C void BlitLineARGB8888OpaqueToColor64K
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
        *dst++ = ARGB8888ToColor64K(*(TUint32*)colorAddr);
        colorAddr += aColorPixelPitch;
    }
}

//
// ARGB8888 -> Color64K
//
LOCAL_C void BitBltARGB8888ToColor64K
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineARGB8888OpaqueToColor64K);
}

#ifdef LCDGD_SUPPORT_1BPP_MASK_BITMAP
//
// Color64K, Gray2 -> ARGB8888
//
// Very slow. Would be better to iterate over source rectangle as this
// would allow multi-pixel mask reads. Alternatively just implemement
// the simple no-transform or vert reflection only case.
//
LOCAL_C void BitBltColor64KGray2ToARGB8888
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor64K);
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
    const TUint8* srcEnd = srcAddress + srcLinePitch*(srcSize.iHeight - 1) + (srcSize.iWidth-1)*sizeof(TUint16);

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
            TUint8* src = srcAddress + v*srcLinePitch + u*sizeof(TUint16);
            TUint8* msk = mskAddress + v*mskLinePitch + (u>>3);

#ifdef _DEBUG
            ASSERT(srcBeg <= src && src <= srcEnd);
            ASSERT(mskBeg <= msk && msk <= mskEnd);
            ASSERT(dstBeg <= dst && dst <= dstEnd);
#endif

            TUint16 spix = *(TUint16*)src;
            TUint8  smsk = *msk;
            TUint32 dpix = (smsk & 1<<(u&0x7))?0xff000000:0;

            dpix |= Color64KRed(spix)   << 16;
            dpix |= Color64KGreen(spix) << 8;
            dpix |= Color64KBlue(spix);
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
// ARGB8888 -> Color64K, Gray2
//
LOCAL_C void BitBltARGB8888ToColor64KGray2
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
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
    const TUint16* dstBeg = (TUint16*)dstAddress;
    const TUint16* dstEnd = (TUint16*)(dstAddress + dstLinePitch*(dstSize.iHeight - 1) + (dstSize.iWidth-1)*sizeof(TUint16));

    const TSize mskSize = aDstAlphaBitmap->iSize;
    const TUint8* mskBeg = mskAddress;
    const TUint8* mskEnd = mskAddress + mskLinePitch*(mskSize.iHeight-1) + ((mskSize.iWidth-1)>>3);
#endif


    dstAddress += dstLinePitch*aDstRect.iTl.iY;
    dstAddress += aDstRect.iTl.iX*sizeof(TUint16);

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
        TUint16* dst = (TUint16*)dstAddress;
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

            *dst++ = ARGB8888ToColor64K(pixel);
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
                *dst++ = ARGB8888ToColor64K(pixel);
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

            *dst++ = ARGB8888ToColor64K(pixel);
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
LOCAL_C void BlitLineColor64KColor64KToARGB8888
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
        TUint16 spix = *(TUint16*)colorAddr;

        TUint32 dpix = (smsk << 24);    // bottom 8 bits of smsk should all be 1 if opaque
        ASSERT((smsk == 0) || (smsk == 0xffff));
        dpix |= Color64KRed(spix)   << 16;
        dpix |= Color64KGreen(spix) << 8;
        dpix |= Color64KBlue(spix);
        *dst++ = dpix;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}

//
//
//
LOCAL_C void BitBltColor64KColor64KToARGB8888
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EColor64K);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor64KColor64KToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor64KColor64K
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
        *dst++ = (TUint16)ARGB8888ToColor64K(pixel);
        *maskAddr++ = (TUint16)(((pixel >> 24) == 0xff) ? -1 : 0);

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
    }
}

//
//
//
LOCAL_C void BitBltARGB8888ToColor64KColor64K
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aDstAlphaBitmap, EFalse, BlitLineARGB8888ToColor64KColor64K);
}
#endif

#ifdef LCDGD_SUPPORT_ALPHA_BITMAP
LOCAL_C void BlitLineColor64KGray256ToARGB8888
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

        dpix |= Color64KRed(spix)   << 16;
        dpix |= Color64KGreen(spix) << 8;
        dpix |= Color64KBlue(spix);
        *dst++ = dpix;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
        maskAddr += aMaskPixelPitch;
    }
}

//
// Color64K, Gray256 -> ARGB8888
//
LOCAL_C void BitBltColor64KGray256ToARGB8888
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aSrcAlphaBitmap->iDisplayMode == EGray256);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aSrcAlphaBitmap, ETrue, BlitLineColor64KGray256ToARGB8888);
}

LOCAL_C void BlitLineARGB8888ToColor64KGray256
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
        *dst++ = (TUint16)ARGB8888ToColor64K(pixel);
        *maskAddr++ = pixel >> 24;

        // Advance to next pixel in the line in each bitmap.
        colorAddr += aColorPixelPitch;
    }
}

//
// ARGB8888 -> Color64K, Gray256
//
LOCAL_C void BitBltARGB8888ToColor64KGray256
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor64K);
    ASSERT(aDstAlphaBitmap->iDisplayMode == EGray256);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericMaskBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                    aDstAlphaBitmap, EFalse, BlitLineARGB8888ToColor64KGray256);
}
#endif

