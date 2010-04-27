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

inline TUint32 ARGB8888ToColor16MA(TUint32 aARGB)
{
    return aARGB;
}

inline TUint32 Color16MA(TInt aRed, TInt aGreen, TInt aBlue)
{
    return TUint32((0xff<<24) | (aRed << 16) | (aGreen << 8) | aBlue);
}

LOCAL_C TUint32 Color16MAForward(TUint32 aRGB)
{
    return ARGB8888ToColor16MA(aRGB);
}

LOCAL_C TUint32 Color16MAReverse(TUint32 aColor16MA)
{
    return aColor16MA;
}

LOCAL_C TUint32 Color16MAQuantize(TUint32 aRGB)
{
    return (aRGB & 0x00FFFFFF);     // must ensure we only return RGB.
}


//
// DrawRegion support
//
LOCAL_C void DrawRegionColor16MAMaskToColor16MAOpaque
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);


LOCAL_C void DrawRegionColor16MAAlphaToColor16MAOpaque
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
LOCAL_C void BitBltJavaOpaqueToColor16MAOpaque
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   /*aDstAlphaBitmap*/,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   /*aSrcAlphaBitmap*/,
    const TLcdTransform&            aTransform
);


LOCAL_C void BitBltColor16MAAlphaToColor16MAMask
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform
);


#ifdef RD_JAVA_NGA_ENABLED
/**
 *
 */
LOCAL_C void DrawRegionColor16MAMaskToColor16MAPre
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);

/**
 *
 */
LOCAL_C void DrawRegionColor16MAAlphaToColor16MAPre
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* aSrcAlphaBitmap,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
);

#endif // RD_JAVA_NGA_ENABLED            


TBool DetectCollisionColor16MAColor16MA
(
    const TAcceleratedBitmapInfo*   aBitmap1,
    const TAcceleratedBitmapInfo*   aBitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

TBool HasOpaquePixelColor16MA(const TAcceleratedBitmapInfo* aBitmap, const TRect& aRect);

//
// Image types supported by this DLL.
//
#define COLOR16MA_OPAQUE IMAGE_TYPE(EColor16MA, ENone, ETransparencyNone)
#define COLOR16MA_MASK   IMAGE_TYPE(EColor16MA, ENone, ETransparencyMaskChannel)
#define COLOR16MA_ALPHA  IMAGE_TYPE(EColor16MA, ENone, ETransparencyAlphaChannel)

#ifdef RD_JAVA_NGA_ENABLED
#define COLOR16MA_ALPHA_PRE  IMAGE_TYPE(EColor16MA, ENone, ETransparencyAlphaChannelPre)
#define COLOR16MA_MASK_PRE  IMAGE_TYPE(EColor16MA, ENone, ETransparencyMaskChannelPre)
#endif // RD_JAVA_NGA_ENABLED

const TImageRenderer ImageRendererArray[] =
{
    //
    // SrcOver
    //
    /*           target,          source,          transform mask,      function */
    RENDER_ENTRY(COLOR16MA_OPAQUE, COLOR16MA_OPAQUE, KTransSimpleMask,  BitBltThirtyTwoBppSimple),
    RENDER_ENTRY(COLOR16MA_OPAQUE, COLOR16MA_OPAQUE, KTransAllMask,   BitBltThirtyTwoBpp),
    RENDER_ENTRY(COLOR16MA_OPAQUE, COLOR16MA_MASK,   KTransAllMask,   DrawRegionColor16MAMaskToColor16MAOpaque),
    RENDER_ENTRY(COLOR16MA_OPAQUE, COLOR16MA_ALPHA,  KTransAllMask,   DrawRegionColor16MAAlphaToColor16MAOpaque),
    RENDER_ENTRY(COLOR16MA_OPAQUE, JAVA_OPAQUE_IMAGE, KTransAllMask,  BitBltJavaOpaqueToColor16MAOpaque),
    RENDER_ENTRY(COLOR16MA_OPAQUE, JAVA_ALPHA_IMAGE, KTransAllMask,   DrawRegionColor16MAAlphaToColor16MAOpaque),

#ifdef RD_JAVA_NGA_ENABLED
    RENDER_ENTRY(COLOR16MA_OPAQUE, COLOR16MA_MASK_PRE,   KTransAllMask,   DrawRegionColor16MAMaskToColor16MAPre),
    RENDER_ENTRY(COLOR16MA_OPAQUE, COLOR16MA_ALPHA_PRE,  KTransAllMask,   DrawRegionColor16MAAlphaToColor16MAPre),
#endif // RD_JAVA_NGA_ENABLED

    //
    // SrcCopy
    //
    /*            target,          source,              function */
    BLITTER_ENTRY(COLOR16MA_OPAQUE,  JAVA_OPAQUE_IMAGE, BitBltJavaOpaqueToColor16MAOpaque),
    SIMPLE_BLITTER_ENTRY(COLOR16MA_OPAQUE,  COLOR16MA_OPAQUE,   BitBltThirtyTwoBppSimple),
    BLITTER_ENTRY(COLOR16MA_OPAQUE,  COLOR16MA_OPAQUE,  BitBltThirtyTwoBpp),
    SIMPLE_BLITTER_ENTRY(JAVA_OPAQUE_IMAGE, COLOR16MA_OPAQUE,  BitBltThirtyTwoBppSimple),
    BLITTER_ENTRY(JAVA_OPAQUE_IMAGE, COLOR16MA_OPAQUE,  BitBltThirtyTwoBpp),
    BLITTER_ENTRY(COLOR16MA_OPAQUE,  JAVA_ALPHA_IMAGE,  BitBltJavaOpaqueToColor16MAOpaque), // entry shouldn't be needed
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE,  COLOR16MA_OPAQUE,  BitBltThirtyTwoBpp),
    //
    BLITTER_ENTRY(COLOR16MA_MASK,   JAVA_ALPHA_IMAGE,   BitBltColor16MAAlphaToColor16MAMask),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR16MA_MASK,     BitBltThirtyTwoBpp),        // mask(0,255)==alpha
    BLITTER_ENTRY(COLOR16MA_MASK, COLOR16MA_MASK,       BitBltThirtyTwoBpp),
    //
    BLITTER_ENTRY(COLOR16MA_ALPHA,  JAVA_ALPHA_IMAGE,   BitBltThirtyTwoBpp),
    BLITTER_ENTRY(JAVA_ALPHA_IMAGE, COLOR16MA_ALPHA,    BitBltThirtyTwoBpp)
};
const TInt ImageRendererCount = sizeof(ImageRendererArray)/sizeof(ImageRendererArray[0]);

const TCollisionDetector CollisionDetectorArray[] =
{
    COLLISION_DETECTOR_ENTRY(EColor16MABit,ETransparencyMaskChannelBit,     EColor16MABit,ETransparencyMaskChannelBit,  DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(EColor16MABit,ETransparencyAlphaChannelBit,    EColor16MABit,ETransparencyAlphaChannelBit, DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(EColor16MABit,ETransparencyMaskChannelBit,     EColor16MABit,ETransparencyAlphaChannelBit, DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(EColor16MABit,ETransparencyAlphaChannelBit,    EColor16MABit,ETransparencyMaskChannelBit,  DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(ENoneBit,ETransparencyNoneBit,                 EColor16MABit,ETransparencyMaskChannelBit,  DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(EColor16MABit,ETransparencyMaskChannelBit,     ENoneBit,ETransparencyNoneBit,              DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(ENoneBit,ETransparencyNoneBit,                 EColor16MABit,ETransparencyAlphaChannelBit, DetectCollisionColor16MAColor16MA),
    COLLISION_DETECTOR_ENTRY(EColor16MABit,ETransparencyAlphaChannelBit,    ENoneBit,ETransparencyNoneBit,              DetectCollisionColor16MAColor16MA)
};
const TInt CollisionDetectorCount = sizeof(CollisionDetectorArray)/sizeof(CollisionDetectorArray[0]);

const TColorMap ColorMapArray[] =
{
    { EColor16MA, Color16MAForward, Color16MAReverse, Color16MAQuantize }
};
const TInt ColorMapCount = sizeof(ColorMapArray)/sizeof(ColorMapArray[0]);

const TDrawFunctions DrawFunctionsArray[] =
{
    {
        EColor16MA,
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

LOCAL_C CLcdGraphicsDriverImpl* CreateColor16MADriverL();

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(LCDGD_COLOR16MA_IMPLEMENTATION_UID, CreateColor16MADriverL)
};

/**
 * Ordinal 1
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

CLcdGraphicsDriverImpl* CreateColor16MADriverL()
{
    CLcdGraphicsDriver::TDriverInfo driverInfo;

    driverInfo.iVersion      = TVersion(KLcdImgVersionMajor, KLcdImgVersionMinor, KVersionBuild);
    driverInfo.iProvider     = KProviderSymbianSoftwareLtd;
    driverInfo.iTransparencyCaps = CAP_1BPP_MASK_BITMAP|CAP_MATCHED_MASK_BITMAP|CAP_ALPHA_BITMAP;
    driverInfo.iARGB8888Mode = EColorARGB8888;
    driverInfo.iDisplayMode = EColor16MA;

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

LOCAL_C void BlitLineColor16MAMaskToColor16MAOpaque
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
        TUint32 dstColor = *dst;
        TUint32 srcColor = *(TUint32*)colorAddr;

        TUint32 mask = (TUint32)(((TInt32)srcColor) >> 24); // Sign extend down.
        ASSERT(mask == 0 || mask == (TUint32)-1);

        dstColor = (dstColor & ~mask) | (srcColor & mask);
        ASSERT((dstColor >> 24) == 0xff);

        *dst++ = dstColor;
        colorAddr += aColorPixelPitch;
    }
}


LOCAL_C void DrawRegionColor16MAMaskToColor16MAOpaque
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,      // mask
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MA);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MA);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor16MAMaskToColor16MAOpaque);
}


LOCAL_C void BlitLineColor16MAAlphaToColor16MAOpaque
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
        TInt alpha = argb >> 24;
        if (alpha == 0xFF)
        {
            *dst = argb;
        }
        else
        {
            if (alpha)
            {
                //
                // alpha blend src over dst.
                //
                TUint32 drgb= *dst;

                alpha = (alpha << 8) | alpha;

                TUint32 dr = Blend32(alpha, ((argb>>16)&0xff), ((drgb>>16)&0xff));
                TUint32 dg = Blend32(alpha, ((argb>>8)&0xff), ((drgb>>8)&0xff));
                TUint32 db = Blend32(alpha, (argb&0xff), (drgb&0xff));

                ASSERT(dr < 256);
                ASSERT(dg < 256);
                ASSERT(db < 256);

                *dst = Color16MA(dr, dg, db);
            }
        }

        dst++;
        colorAddr += aColorPixelPitch;
    }
}

//
// NB destination is fully opaque.
//
LOCAL_C void DrawRegionColor16MAAlphaToColor16MAOpaque
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MA);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor16MAAlphaToColor16MAOpaque);
}


#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// BlitLineColor16MAMaskToColor16MAPre
// Target's color components are premultiplied
// ---------------------------------------------------------------------------
//
LOCAL_C void BlitLineColor16MAMaskToColor16MAPre
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
        TUint32 dstColor = *dst;
        TUint32 srcColor = *(TUint32*)colorAddr;

        TUint32 mask = (TUint32)(((TInt32)srcColor) >> 24); // Sign extend down.
        ASSERT(mask == 0 || mask == (TUint32)-1);

        if (mask)
        {
            // Note that the target is not always opaque anymore
            dstColor = srcColor;
        }
        *dst++ = dstColor;
        colorAddr += aColorPixelPitch;
    }
}


// ---------------------------------------------------------------------------
// DrawRegionColor16MAMaskToColor16MAPre
// ---------------------------------------------------------------------------
//
LOCAL_C void DrawRegionColor16MAMaskToColor16MAPre
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,      // mask
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MA);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MA);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor16MAMaskToColor16MAPre);
}

// ---------------------------------------------------------------------------
// BlitLineColor16MAMaskToColor16MAPre
// Target's color components are premultiplied
// ---------------------------------------------------------------------------
//
LOCAL_C void BlitLineColor16MAAlphaToColor16MAPre
(
    TUint8* aDstAddress,
    TInt    aWidth,
    TUint8* aColorAddress,
    TInt    aColorPixelPitch
)
{

    TUint32* dstAddress = (TUint32*)(aDstAddress);
    TUint32* end = dstAddress + aWidth;

    TUint8* srcAddress = aColorAddress;

    while (dstAddress < end)
    {
        const TUint32 src=*(TUint32*)srcAddress;

        if (src >= 0xFF000000)
        {
            *(TUint32*)dstAddress = src;
        }
        else
        {
            const TUint32 srcAlpha = src >> 24;

            if (srcAlpha)
            {
                TUint32 destA;
                TUint32 destAG;
                TUint32 destRB;
                TUint32 destMultAlpha;

                const TUint32 dst = *(TUint32*)dstAddress;
                const TUint32 dstAlpha = dst >> 24;

                destA = dstAlpha << 16;
                destA = destA * (0x100 - srcAlpha);
                destA += srcAlpha << 24;
                destMultAlpha = 0xFF - srcAlpha;

                const TUint32 srcPixel = *(TUint32*)srcAddress;
                const TUint32 dstPixel = *(TUint32*)dstAddress;

                destAG = (dstPixel & 0xFF00FF00) >> 8;
                destAG = destAG * destMultAlpha;
                TUint32 srcAG = (srcPixel & 0xFF00FF00) >> 8;
                destAG &= 0xFF00FF00;
                TUint32 alphaPlus1 = srcAlpha + 1;
                destAG += srcAG * alphaPlus1;

                destRB = dstPixel & 0x00FF00FF;
                destRB = destRB * destMultAlpha;
                destRB &= 0xFF00FF00;
                TUint32 srcRB = (srcPixel & 0x00FF00FF);
                destRB += srcRB * alphaPlus1;
                destRB >>= 8;

                *(TUint32*)dstAddress = (destAG & 0x0000FF00) |
                                        (destRB & 0x00FF00FF) |
                                        (destA & 0xFF000000);
            }
        }

        dstAddress++;
        srcAddress += aColorPixelPitch;
    } // while( dstAddress < end )
}

// ---------------------------------------------------------------------------
// DrawRegionColor16MAAlphaToColor16MAPre
// ---------------------------------------------------------------------------
//
LOCAL_C void DrawRegionColor16MAAlphaToColor16MAPre
(
    const TAcceleratedBitmapInfo* aDstColorBitmap,
    const TAcceleratedBitmapInfo* /*aDstAlphaBitmap*/,
    const TRect&                  aDstRect,     // must be clipped to destination
    const TAcceleratedBitmapInfo* aSrcColorBitmap,
    const TAcceleratedBitmapInfo* /*aSrcAlphaBitmap*/,      // ignored
    const TLcdTransform&          aTransform        // includes anchor
)
{
    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MA);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColorARGB8888);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor16MAAlphaToColor16MAPre);
}

#endif // RD_JAVA_NGA_ENABLED

// ***************************************************************
// BitBlt Functions
// ***************************************************************

LOCAL_C void BlitLineJavaOpaqueToColor16MAOpaque
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
// XRGB8888 to Color16MA opaque.
//
// We cannot guarantee that the java image has fully opaque pixels
// so we need to ensure that the alpha component is initialized
// in the destination image. Consider using COLOR16MU for java opaque
// images.
//
LOCAL_C void BitBltJavaOpaqueToColor16MAOpaque
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
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MA);

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineJavaOpaqueToColor16MAOpaque);
}


LOCAL_C void BlitLineColor16MAAlphaToColor16MAMask
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

        TUint32 alpha = argb & 0xff000000;
        argb ^= alpha;

        // Anything other than full alpha is set to zero alpha
        alpha *= !(alpha + 0x01000000);

        argb |= alpha;
        *dst++ = argb;
        colorAddr += aColorPixelPitch;
    }
}

//
// ARGB8888 -> Color16MA, Gray2
//
LOCAL_C void BitBltColor16MAAlphaToColor16MAMask
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

    ASSERT(aDstColorBitmap->iDisplayMode == EColor16MA);
    ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MA);        // EColorARGB8888 on SymbianOS < 9.1

    GenericBlit(aDstColorBitmap, aDstRect, aSrcColorBitmap, aTransform,
                BlitLineColor16MAAlphaToColor16MAMask);
}

TBool DetectCollisionColor16MAColor16MA
(
    const TAcceleratedBitmapInfo*   aBitmap1,
    const TAcceleratedBitmapInfo*   aBitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
)
{
    // if only one mask, just need to check if it has any opaque pixels
    if (aBitmap1 == NULL)
    {
        return ::HasOpaquePixelColor16MA(aBitmap2, aRect2);
    }

    if (aBitmap2 == NULL)
    {
        return ::HasOpaquePixelColor16MA(aBitmap1, aRect1);
    }

    ASSERT(aBitmap1->iDisplayMode == EColor16MA);
    ASSERT(aBitmap2->iDisplayMode == EColor16MA);

    TLcdTransform transform = aTransform1 * aTransform2.Inverse();
    TRect rect = aRect2;

    TInt h= rect.Height();
    TInt w = rect.Width();

    TPoint srcPoint = transform(rect.iTl);
    TInt dudx = transform.iDuDx;
    TInt dudy = transform.iDuDy;
    TInt dvdx = transform.iDvDx;
    TInt dvdy = transform.iDvDy;
    TInt u0   = srcPoint.iX;
    TInt v0   = srcPoint.iY;
    TInt u    = u0;
    TInt v    = v0;

    const TInt linePitch1   = aBitmap1->iLinePitch;
    const TInt linePitch2   = aBitmap2->iLinePitch;

    TUint8* address1        = aBitmap1->iAddress;
    TUint8* address2        = aBitmap2->iAddress;

    address2 += rect.iTl.iY*linePitch2 + rect.iTl.iX*sizeof(TUint32);

    if ((dudx == 1) && (dvdx == 0) && (dudy == 0) && (dvdy == 1))
    {
        address1 += v0*linePitch1 + u0*sizeof(TUint32);
        for (; h>0; --h)
        {
            TUint32* pixel1 = (TUint32*)address1;
            TUint32* pixel2 = (TUint32*)address2;
            TUint32* end = pixel2 + w;

            while (pixel2 < end)
            {
                TUint32 m2 = *pixel2++;
                TUint32 m1 = *pixel1++;
                if ((m2>>24) == 255 && (m1>>24) == 255)
                {
                    return ETrue;
                }
            }

            address1 += linePitch1;
            address2 += linePitch2;
        }
    }
    else
    {
        for (; h>0; --h)
        {
            u=u0;
            v=v0;

            TUint32* pixel2 = (TUint32*)address2;
            TUint32* end = pixel2 + w;

            while (pixel2 < end)
            {
                TUint32 m2 = *pixel2++;
                TUint32 m1 = *(TUint32*)(address1 + v*linePitch1 + u*sizeof(TUint32));

                if ((m2>>24) == 255 && (m1>>24) == 255)
                {
                    return ETrue;
                }

                u+=dudx;
                v+=dvdx;
            }

            u0+=dudy;
            v0+=dvdy;
            address2 += linePitch2;
        }
    }

    return EFalse;
}


TBool HasOpaquePixelColor16MA
(
    const TAcceleratedBitmapInfo*   aBitmap,
    const TRect&                    aRect
)
{
    const TInt linePitch    = aBitmap->iLinePitch;
    TUint8* address         = aBitmap->iAddress;

    TInt h  = aRect.Height();
    TInt w  = aRect.Width();

    address += aRect.iTl.iY*linePitch + aRect.iTl.iX*sizeof(TUint32);
    for (; h>0; --h)
    {
        TUint32* pix = (TUint32*)address;
        TUint32* end = pix + w;
        while (pix < end)
        {
            if (((*pix) >> 24) == 255)
            {
                return ETrue;
            }
            ++pix;
        }
        address += linePitch;
    }

    return EFalse;
}

