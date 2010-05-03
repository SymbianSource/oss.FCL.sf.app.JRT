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


#include <lcdgdrv.h>
#include "lcdgdrvif.h"
#include "calctransform.h"
#include "lcdgdev.h"

CLcdGraphicsDeviceImpl::CLcdGraphicsDeviceImpl
(
    CLcdGraphicsDriver& aDriver,
    const TImageType& aTargetType,
    CRenderFunctions* aRenderers,
    const TColorMap& aColorMap,
    const TDrawFunctions& aDrawFunctions
)
        : iDriver(aDriver)
        , iRenderers(aRenderers)
        , iColorMap(aColorMap)
        , iDrawFunctions(aDrawFunctions)
{
    iRenderKey.iTargetType = aTargetType;
    ASSERT(iDrawFunctions.iDisplayMode == aTargetType.iColorMode);
}

CLcdGraphicsDeviceImpl::~CLcdGraphicsDeviceImpl()
{
    delete iRenderers;
}


TUint32 CLcdGraphicsDeviceImpl::DrawingCaps() const
{
    const TInt renderCaps = (CLcdGraphicsDevice::ECapDrawRegion | CLcdGraphicsDevice::ECapCopyRegion);
    return iDrawFunctions.iDrawCaps | renderCaps;
}

TUint32 CLcdGraphicsDeviceImpl::Quantize(TUint32 aRGB) const
{
    return (*iColorMap.iQuantize)(aRGB);
}

/**
 * Transforming biblt from aColorBitmap/aAlphaBitmap to target surface
 * Composites source image over destination image (either alpha blending
 * or masking as appropriate to <CODE>aSrcTransparency</CODE>).
 */
TInt CLcdGraphicsDeviceImpl::DrawRegion
(
    const TAcceleratedBitmapInfo*   aDstBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    TTransparency                   aSrcTransparency,
    const TRect&                    aSrcRect,
    TTransformType                  aSrcTransform,
    const TRect&                    aClipRect
)
{
    TInt err = KErrNotSupported;

    TImageType sourceType;
    sourceType.iColorMode = aSrcColorBitmap->iDisplayMode;
    sourceType.iAlphaMode = aSrcAlphaBitmap ? aSrcAlphaBitmap->iDisplayMode : ENone;
    sourceType.iTransparency = aSrcTransparency;

    iRenderKey.iSourceType = TCompactImageType(sourceType);
    iRenderKey.iTransform = (1<<aSrcTransform);
    iRenderKey.iComposite  = ECompositeSrcOver;

    const TImageRenderer* renderer = iRenderers->Get(iRenderKey);
    if (renderer)
    {
        TImageRenderFunction drawRegion = renderer->iFunction;

        // calc source to target transform
        TLcdTransform transform = CalcTransform(aDstRect, aSrcRect, aSrcTransform);

        TRect dstRect(aDstRect);
        TRect srcRect(aSrcRect);
        TRect srcClipRect(aSrcColorBitmap->iSize);
        TRect dstClipRect(aDstBitmap->iSize);

        // clip cliprect to device rect
        dstClipRect.Intersection(aClipRect);

        // calculate source and target rects clipped to src and target bounds.
        ClipTransformRect(dstRect, srcRect, dstClipRect, srcClipRect, transform);

        dstRect.Intersection(dstClipRect);

        if (!dstRect.IsEmpty())
        {
            // calc target to source transform.
            transform = transform.Inverse();

            ASSERT(CheckBounds(aDstBitmap->iSize, aSrcColorBitmap->iSize, dstRect, transform));

            (*drawRegion)(aDstBitmap, NULL, dstRect, aSrcColorBitmap, aSrcAlphaBitmap, transform);
        }

        err = KErrNone;
    }

    return err;
}

/**
 * Transforming biblt from <CODE>aSrcColorBitmap,aSrcAlphaBitmap<CODE>
 * to <CODE>aDstColorBitmap,aDstAlphaBitmap</CODE>.
 * Copies source image pixels to destination image converting color
 * and transparency pixels to the destination format. Supports translation
 * and symmetry transformation of source image region, specified by
 * <CODE>aSrcTransform</CODE>.
 */
TInt CLcdGraphicsDeviceImpl::CopyRegion
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    TTransparency                   aSrcTransparency,
    const TRect&                    aSrcRect,
    TTransformType                  aSrcTransform,
    const TRect&                    aClipRect
)
{
    TInt err = KErrNotSupported;
    ASSERT(aDstBitmap->iAddress);

    TImageType sourceType;

    sourceType.iColorMode = aSrcColorBitmap->iDisplayMode;
    sourceType.iAlphaMode = aSrcAlphaBitmap ? aSrcAlphaBitmap->iDisplayMode : ENone;
    sourceType.iTransparency = aSrcTransparency;

    iRenderKey.iSourceType = TCompactImageType(sourceType);
    iRenderKey.iTransform = (1<<aSrcTransform);
    iRenderKey.iComposite  = ECompositeSrcCopy;

    const TImageRenderer* renderer = iRenderers->Get(iRenderKey);
    if (renderer)
    {
        TImageRenderFunction copyRegion = renderer->iFunction;

        // calc source to target transform
        TLcdTransform transform = CalcTransform(aDstRect, aSrcRect, aSrcTransform);

        TRect dstRect(aDstRect);
        TRect srcRect(aSrcRect);
        TRect srcClipRect(aSrcColorBitmap->iSize);
        TRect dstClipRect(aDstBitmap->iSize);

        // clip cliprect to device rect
        dstClipRect.Intersection(aClipRect);

        // clip source and target rects
        ClipTransformRect(dstRect, srcRect, dstClipRect, srcClipRect, transform);

        // check src and dst rects still correspond
        ASSERT(CheckTransform(dstRect, srcRect, transform));

        dstRect.Intersection(dstClipRect);

        if (!dstRect.IsEmpty())
        {
            // calc target to source transform.
            transform = transform.Inverse();

            // check source and dst rects lie within bounds
            ASSERT(CheckBounds(aDstBitmap->iSize, aSrcColorBitmap->iSize, dstRect, transform));

            (*copyRegion)(aDstBitmap, NULL, dstRect, aSrcColorBitmap, aSrcAlphaBitmap, transform);
        }

        err = KErrNone;
    }

    return err;
}


/*
 * Draw line from aStart to aEnd including both end points and
 * using line style TStrokeStyle.
 */
TInt CLcdGraphicsDeviceImpl::DrawLine
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TPoint& aStart,
    const TPoint& aEnd,
    TUint32 aRGB,
    TStrokeStyle aStyle,
    const TRect& aClipRect
)
{
    TInt caps = ECapDrawLine;
    if (aStyle == EStrokeDotted)
    {
        caps |= ECapStrokeDotted;
    }
    if ((iDrawFunctions.iDrawCaps & caps) != caps)
    {
        return KErrNotSupported;
    }
    ASSERT(iDrawFunctions.iDrawLine);
    (*iDrawFunctions.iDrawLine)(aDstBitmap, aStart, aEnd, (*iColorMap.iForward)(aRGB), aStyle, aClipRect);
    return KErrNone;
}

/**
 * Draw outline of <CODE>aRect</CODE>
 */
TInt CLcdGraphicsDeviceImpl::DrawRect
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TRect& aRect,
    TUint32      aRGB,
    TStrokeStyle aStyle,
    const TRect& aClipRect
)
{
    TInt caps = ECapDrawRect;
    if (aStyle == EStrokeDotted)
    {
        caps |= ECapStrokeDotted;
    }
    if ((iDrawFunctions.iDrawCaps & caps) != caps)
    {
        return KErrNotSupported;
    }
    ASSERT(iDrawFunctions.iDrawRect);
    (*iDrawFunctions.iDrawRect)(aDstBitmap, aRect, (*iColorMap.iForward)(aRGB), aStyle, aClipRect);
    return KErrNone;
}

/**
 * Fill interior of <CODE>aRect</CODE> with color <CODE>aRGB</CODE>
 */
TInt CLcdGraphicsDeviceImpl::FillRect
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TRect& aRect,
    TUint32      aRGB,
    const TRect& aClipRect
)
{
    if (!(iDrawFunctions.iDrawCaps & ECapFillRect))
    {
        return KErrNotSupported;
    }
    ASSERT(iDrawFunctions.iFillRect);
    (*iDrawFunctions.iFillRect)(aDstBitmap, aRect, (*iColorMap.iForward)(aRGB), aClipRect);
    return KErrNone;
}

/**
 * Draw the arc of an ellipse bounded by aBoundingRect in device coordinates,
 * starting the arc at aStartAngle from the ellipse horizontal axis and
 * extending for aArcAngle degrees anticlockwise. Draw with color aRGB and
 * clip to aClipRect in device coords.
 */
TInt CLcdGraphicsDeviceImpl::DrawArc
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TRect& aBoundingRect,
    const TInt aStartAngle,
    const TInt aArcAngle,
    TUint32 aRGB,
    TStrokeStyle aStyle,
    const TRect& aClipRect
)
{
    TInt caps = ECapDrawRect;
    if (aStyle == EStrokeDotted)
    {
        caps |= ECapStrokeDotted;
    }
    if ((iDrawFunctions.iDrawCaps & caps) != caps)
    {
        return KErrNotSupported;
    }
    ASSERT(iDrawFunctions.iDrawArc);
    (*iDrawFunctions.iDrawArc)(aDstBitmap, aBoundingRect, aStartAngle, aArcAngle, (*iColorMap.iForward)(aRGB), aStyle, aClipRect);
    return KErrNone;
}

/**
 * Fill the region bounded by an arc and the radii of its end points of an ellipse bounded
 * by aBoundingRect in device coordinates. The first radius lies at aStartAngle from the
 * ellipse horizontal axis and the second radies lies aArcAngle degrees anticlockwise
 * from the first. Fill with color aRGB and clip to aClipRect in device coords.
 */
TInt CLcdGraphicsDeviceImpl::FillArc
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TRect& aBoundingRect,
    const TInt aStartAngle,
    const TInt aArcAngle,
    TUint32 aRGB,
    const TRect& aClipRect
)
{
    if (!(iDrawFunctions.iDrawCaps & ECapFillArc))
    {
        return KErrNotSupported;
    }
    ASSERT(iDrawFunctions.iFillArc);
    (*iDrawFunctions.iFillArc)(aDstBitmap, aBoundingRect, aStartAngle, aArcAngle, (*iColorMap.iForward)(aRGB), aClipRect);
    return KErrNone;
}

/**
 * Fill a triangle in device coordinates with color aRGB,
 * clipping to aClipRect in device coordinates.
 */
TInt CLcdGraphicsDeviceImpl::FillTriangle
(
    const TAcceleratedBitmapInfo* aDstBitmap,
    const TPoint aPoints[3],
    TUint32 aRGB,
    const TRect& aClipRect
)
{
    if (!(iDrawFunctions.iDrawCaps & ECapFillTriangle))
    {
        return KErrNotSupported;
    }
    ASSERT(iDrawFunctions.iFillTriangle);
    (*iDrawFunctions.iFillTriangle)(aDstBitmap, aPoints, (*iColorMap.iForward)(aRGB), aClipRect);
    return KErrNone;
}

TInt CLcdGraphicsDeviceImpl::DrawText
(
    const TAcceleratedBitmapInfo* /*aDstBitmap*/,
    const TDesC& /*aText*/,
    const TPoint& /*aPoint*/,
    const CFont* /*aFont*/,
    TUint32 /*aColor*/,
    const TRect& /*aClipRect*/
)
{
    return KErrNotSupported;
}

/**
 * This function is used, when image is drawn and rendering
 * target is framebuffer of CanavsGraphicsItem.
 */
TInt CLcdGraphicsDeviceImpl::DrawRegionForCanvasGraphicsItem
(
    const TAcceleratedBitmapInfo*               aDstBitmap,
    const TRect&                                aDstRect,
    const TAcceleratedBitmapInfo*               aSrcColorBitmap,
    const TAcceleratedBitmapInfo*             /*aSrcAlphaBitmap*/,
    TTransparency                             /*aSrcTransparency*/,
    const TRect&                                aSrcRect,
    TTransformType                              aSrcTransform,
    const TRect&                                aClipRect,
    const TCanvasGraphicsItemOperationsType&    aOperation
)
{
    TInt err = KErrNotSupported;

    // calc source to target transform
    TLcdTransform transform = CalcTransform(aDstRect, aSrcRect, aSrcTransform);

    TRect dstRect(aDstRect);
    TRect srcRect(aSrcRect);
    TRect srcClipRect(aSrcColorBitmap->iSize);
    TRect dstClipRect(aDstBitmap->iSize);

    // clip cliprect to device rect
    dstClipRect.Intersection(aClipRect);

    // calculate source and target rects clipped to src and target bounds.
    ClipTransformRect(dstRect, srcRect, dstClipRect, srcClipRect, transform);

    dstRect.Intersection(dstClipRect);

    if (!dstRect.IsEmpty())
    {
        // calc target to source transform.
        transform = transform.Inverse();

        ASSERT(CheckBounds(aDstBitmap->iSize, aSrcColorBitmap->iSize, dstRect, transform));

        ASSERT(aDstBitmap->iDisplayMode == EColor16MA);
        ASSERT(aSrcColorBitmap->iDisplayMode == EColor16MA);

        DoBlit(aDstBitmap, dstRect, aSrcColorBitmap, transform, aOperation);
    }

    err = KErrNone;

    return err;
}

// support for rendering image on CanavsGraphicsItem frame buffer
TInt CLcdGraphicsDeviceImpl::PixelPitch(const TAcceleratedBitmapInfo* aBitmap)
{
    switch (aBitmap->iDisplayMode)
    {
    case EColor64K:
    case EColor4K:
        return 2;
    case EColorARGB8888:
    case EColor16MU:
        return 4;
    case EGray256:
        return 1;
    }

    // Any other display mode is either invalid, or has a fractional number of
    // bytes per pixel, and cannot be handled by this routine.
    ASSERT(EFalse);
    return 0;   // Pacify the compiler
}

// support for rendering image on CanavsGraphicsItem frame buffer
void CLcdGraphicsDeviceImpl::DoBlit
(
    const TAcceleratedBitmapInfo*               aDstColorBitmap,
    const TRect&                                aDstRect,       // must be clipped to destination
    const TAcceleratedBitmapInfo*               aSrcColorBitmap,
    const TLcdTransform&                        aTransform,     // includes anchor
    const TCanvasGraphicsItemOperationsType&    aOperation
)
{
    ASSERT(aDstColorBitmap != NULL);
    ASSERT(aSrcColorBitmap != NULL);

    TPoint srcPoint = aTransform(aDstRect.iTl);

    TInt dudx = aTransform.iDuDx;
    TInt dudy = aTransform.iDuDy;
    TInt dvdx = aTransform.iDvDx;
    TInt dvdy = aTransform.iDvDy;

    //
    // For each bitmap, calculate the starting address and byte offsets to the
    // next address for one line down and one pixel right.
    //
    const TInt dstLinePitch = aDstColorBitmap->iLinePitch;
    const TInt dstPixelPitch = PixelPitch(aDstColorBitmap);
    TUint8* dstAddress = aDstColorBitmap->iAddress;
    dstAddress += aDstRect.iTl.iY * dstLinePitch + aDstRect.iTl.iX * dstPixelPitch;

    const TInt colorLinePitch = aSrcColorBitmap->iLinePitch;
    const TInt colorPixelPitch = PixelPitch(aSrcColorBitmap);
    TUint8* colorAddress = aSrcColorBitmap->iAddress;
    colorAddress += srcPoint.iY * colorLinePitch + srcPoint.iX * colorPixelPitch;

    // For the source bitmap, also calculate the pitch to the next address for
    // one line down and one pixel right in the destination bitmap.
    const TInt colorDstLinePitch = colorLinePitch * dvdy + colorPixelPitch * dudy;
    const TInt colorDstPixelPitch = colorLinePitch * dvdx + colorPixelPitch * dudx;

    //
    // Iterate over destination pixels.
    //
    const TInt width = aDstRect.Width();
    TInt h = aDstRect.Height();
    while (h-- > 0)
    {
        switch (aOperation)
        {
        case ECanvasGraphicsItemImageRendering:
            DoBlitLineForImage(dstAddress, width, colorAddress, colorDstPixelPitch);
            break;
        case ECanvasGraphicsItemRGBRendering:
            DoBlitLineForRgb(dstAddress, width, colorAddress, colorDstPixelPitch);
            break;
        }

        dstAddress += dstLinePitch;
        colorAddress += colorDstLinePitch;
    }
}

// support for rendering image on CanavsGraphicsItem frame buffer
void CLcdGraphicsDeviceImpl::DoBlitLineForImage
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

#ifdef RD_JAVA_NGA_ENABLED
        if (mask)
        {
            // Note that the target is not always opaque anymore
            dstColor = srcColor;
        }
#else // !RD_JAVA_NGA_ENABLED
        dstColor = (dstColor & ~mask) | (srcColor & mask);
#endif // RD_JAVA_NGA_ENABLED

        *dst++ = dstColor;
        colorAddr += aColorPixelPitch;
    }
}

// support for rendering image on CanavsGraphicsItem frame buffer
void CLcdGraphicsDeviceImpl::DoBlitLineForRgb
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
                destMultAlpha = (((0x100 - srcAlpha) * dstAlpha) >> 8) + 1;

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

CRenderFunctions::~CRenderFunctions()
{
    iEntries.Reset();
    iEntries.Close();
}

const TImageRenderer* CRenderFunctions::Get(const TRenderKey& aKey)
{
    if (iLast && (iLast->iKey == aKey))
    {
        return &(iLast->iRenderer);
    }
    const TInt count = iEntries.Count();
    for (TInt index=0; index<count; ++index)
    {
        const TRenderEntry& entry = iEntries[index];
        if (entry.iKey.Match(aKey))
        {
            if (entry.iKey.iTransform == (1<<ETransNone))
            {
                // only cache no-trans variants.
                iLast = &entry;
            }
            return &entry.iRenderer;
        }
    }
    return NULL;
}

void CRenderFunctions::AppendL(const TImageRenderer& aRenderer)
{
    TRenderEntry entry;
    entry.iKey.iTargetType = aRenderer.iTargetType;
    entry.iKey.iSourceType = aRenderer.iSourceType;
    entry.iKey.iTransform  = aRenderer.iTransformMask;
    entry.iKey.iComposite   = aRenderer.iCompositeRule;
    entry.iRenderer = aRenderer;
    iEntries.AppendL(entry);
}
