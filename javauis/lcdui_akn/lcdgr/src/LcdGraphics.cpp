/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include <fbs.h>
#include <biditext.h>
#include <bitdev.h>
#include <gdi.h>
#include <e32math.h>

#include "LcdGraphics.h"
#include "LcdImage.h"
#include <lcdgdrv.h>
#include "LcdBitmapSurface.h"

#include "DebugArg.h"

#include "LcdWindowSurface.h"

TPoint AnchorPoint(const TPoint& aPoint, TInt aAnchor, const TSize& aSize);
void   ArcVectors(TPoint& aStart, TPoint& aEnd, const TRect& aRect, TInt aStartAngle, TInt aArcAngle);
TPoint Vector(const TReal& aAngle, const TSize& aSize);

#ifdef _DEBUG
class MyBitmapLockCountHack
{
public:
    TInt iCount;
};
#define CHECK_BITMAP_LOCK() ASSERT( ((MyBitmapLockCountHack*)(&iCount))->iCount == 0 )
#else
#define CHECK_BITMAP_LOCK()
#endif

const TUint32 KARGBBlack = 0xff000000;

/**
 * Map lcdui stroke style to GDI pen style.
 */
inline CGraphicsContext::TPenStyle PenStyle(TStrokeStyle aStrokeStyle)
{
    return (aStrokeStyle == EStrokeDotted ? CGraphicsContext::EDottedPen : CGraphicsContext::ESolidPen);
}

//
// CLcdGraphics inline methods
//
inline void CLcdGraphics::SetPenOn()
{
    if (0 == (iState & EPenOn))
    {
        iContext->SetPenStyle(PenStyle(iStrokeStyle));
        iState |= EPenOn;
    }
    if (0 == (iState & EPenColor))
    {
        iContext->SetPenColor(TRgb::Color16MA(iColor));
        iState |= EPenColor;
    }
}

inline void CLcdGraphics::SetPenOff()
{
    if (iState & EPenOn)
    {
        iContext->SetPenStyle(CGraphicsContext::ENullPen);
        iState ^= EPenOn;
    }
}

inline void CLcdGraphics::SetBrushOn()
{
    if (0 == (iState & EBrushOn))
    {
        iContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
        iState |= EBrushOn;
    }
    if (0 == (iState & EBrushColor))
    {
        iContext->SetBrushColor(TRgb::Color16MA(iColor));
        iState |= EBrushColor;
    }
}

inline void CLcdGraphics::SetBrushOff()
{
    if (iState & EBrushOn)
    {
        iContext->SetBrushStyle(CGraphicsContext::ENullBrush);
        iState ^= EBrushOn;
    }
}

inline void CLcdGraphics::LineMode()
{
    SetPenOn();
}

inline void CLcdGraphics::FillMode()
{
    SetPenOff();
    SetBrushOn();
}

inline void CLcdGraphics::OutlineMode()
{
    SetPenOn();
    SetBrushOff();
}

CLcdGraphics* CLcdGraphics::NewL(CLcdGraphicsDriver& aDriver, CCoeControl& aWindow, MDirectContainer& aContainer, TBool aUpdate)
{
    return CLcdGraphics::NewL(aDriver, CLcdWindowSurface::NewL(aWindow, aContainer, aUpdate));
}

CLcdGraphics* CLcdGraphics::NewL(CLcdGraphicsDriver& aDriver, const CFbsBitmap* aBitmap)
{
    return CLcdGraphics::NewL(aDriver, CLcdBitmapSurface::NewL(aBitmap));
}

//
// aSurface is NOT on cleanup stack.
//
CLcdGraphics* CLcdGraphics::NewL(CLcdGraphicsDriver& aDriver, CLcdSurface* aSurface)
{
    CleanupStack::PushL(aSurface);
    CLcdGraphics* graphics = new(ELeave) CLcdGraphics(aDriver, aSurface);
    CleanupStack::Pop(aSurface);
    CleanupStack::PushL(graphics);
    graphics->ConstructL();
    CleanupStack::Pop(graphics);
    return graphics;
}

//
//
//
CLcdGraphics::CLcdGraphics(CLcdGraphicsDriver& aDriver, CLcdSurface* aSurface)
        : iLcdDriver(aDriver)
        , iSurface(aSurface)
        , iIsCanvasGraphicsItemTarget(EFalse)
{
    iStrokeStyle = EStrokeSolid;
    iColor    = KARGBBlack;
    iFont     = NULL;
    iState    = EPenOn|EPenColor;   // Initial state of context
}

void CLcdGraphics::ConstructL()
{
    //
    // We require a CLcdGraphicsDevice for transforming bitblts which are
    // not supported by BITGDI. If the driver cannot create a device for
    // this surface type we will fail here. A line buffer could be used to
    // implement a slow version of the transforming blitters to avoid this
    // failure mode.
    //
    iLcdDevice = iLcdDriver.CreateDeviceL(iSurface->ImageType());
    iLcdDrawCaps = iLcdDevice->DrawingCaps();

    iDeviceBounds = iSurface->Bounds();
    iDeviceOrigin = iDeviceBounds.iTl;
    iDeviceClipRect = iDeviceBounds;

    iOrigin   = TPoint();
    iClipRect = TRect(iDeviceBounds.Size());

#ifdef __SERIES60_
    iMIDPictograph  = CMIDPictograph::NewL();
#endif
}

CLcdGraphics::~CLcdGraphics()
{
    delete iLcdDevice;
    delete iSurface;
#ifdef __SERIES60_
    delete iMIDPictograph;
#endif
}

void CLcdGraphics::Begin()
{
    CHECK_BITMAP_LOCK();

    TRect bounds = iSurface->Bounds();
    if (bounds != iDeviceBounds)
    {
        iDeviceBounds = bounds;
        iDeviceOrigin = iDeviceBounds.iTl + iOrigin;
        iDeviceClipRect = iClipRect;
        iDeviceClipRect.Move(iDeviceOrigin);
        iDeviceClipRect.Intersection(bounds);
    }

    CBitmapContext* context = iSurface->Context();
    if (context != iContext)
    {
        iContext = context;
        ASSERT(iContext);
        iState = 0;
        iContext->SetPenStyle(CGraphicsContext::ENullPen);
        iContext->SetBrushStyle(CGraphicsContext::ENullBrush);
        iContext->SetOrigin(iOrigin);
        iContext->SetClippingRect(iClipRect);
        if (iFont)
        {
            iContext->UseFont(iFont);
            iContext->SetUnderlineStyle(iUnderline);
        }
    }
}

void CLcdGraphics::End()
{
    //
    // This may not be required if we have CLcdGraphics::Flush(rect)
    //
    //iSurface->Update();

    CHECK_BITMAP_LOCK();
}

/**
Maps a color provided as 32bit rgb triple to the closest color representable
in the target devices display mode. returns the result as a 32bit rgb triple.
*/
TUint32 CLcdGraphics::QuantizeColor(TUint32 aRGB32)
{
    if (iLcdDevice)
    {
        return iLcdDevice->Quantize(aRGB32);
    }
    return aRGB32; // iLcdDriver.Quantize( iFbsBitmap->DisplayMode(), aRGB32 );
}

CLcdSurface* CLcdGraphics::Surface()
{
    return iSurface;
}

void CLcdGraphics::SetColor(TUint32 aColor)
{
    if (iColor != aColor)
    {
        iColor = aColor;
        iState &= ~(EPenColor|EBrushColor);
    }
}

TUint32 CLcdGraphics::Color() const
{
    return iColor;
}

void CLcdGraphics::SetClipRect(const TPoint& aPosition, const TSize& aSize)
{
    //
    // We keep a copy of the clipping rectangle for the
    // blitters. Clip rect is supplied in translated
    // coordinates, we map to device coords.
    //
    iClipRect = TRect(aPosition, aSize);
    iContext->SetClippingRect(iClipRect);
    iDeviceClipRect = iClipRect;
    iDeviceClipRect.Move(iDeviceOrigin);
    iDeviceClipRect.Intersection(iDeviceBounds);
}

const TRect& CLcdGraphics::ClipRect() const
{
    return iClipRect;
}

void CLcdGraphics::Translate(const TPoint& aPoint)
{
    //
    // Context stores clip rect in translated coords so there
    // is no need to update the clipping rect.
    //
    iContext->SetOrigin(aPoint);
    iClipRect.Move(iOrigin - aPoint);
    iOrigin = aPoint;
    iDeviceOrigin = aPoint + iDeviceBounds.iTl;
}

const TPoint& CLcdGraphics::Origin() const
{
    return iOrigin;
}

void CLcdGraphics::SetStrokeStyle(TStrokeStyle aStyle)
{
    if (iStrokeStyle != aStyle)
    {
        iStrokeStyle = aStyle;
        if (iState & EPenOn)
        {
            iContext->SetPenStyle(PenStyle(iStrokeStyle));
        }
    }
}

TStrokeStyle CLcdGraphics::StrokeStyle() const
{
    return iStrokeStyle;
}

void CLcdGraphics::SetFont(TUint32 aFontID)
{
    MMIDFont* font = reinterpret_cast<MMIDFont*>(aFontID << 2);
    iFont = font->Font();
    iUnderline = font->IsUnderlined()  ? EUnderlineOn : EUnderlineOff;
    iHeight = font->Height();
    iContext->UseFont(iFont);
    iContext->SetUnderlineStyle(iUnderline);
}

void CLcdGraphics::Reset(const TSize& aSize)
{
    TRect deviceBounds(iDeviceBounds.iTl, aSize);
    iDeviceBounds = deviceBounds;
    iState = 0;
    iColor = KARGBBlack;
    iStrokeStyle = EStrokeSolid;
    iOrigin = TPoint(0,0);
    iClipRect = TRect(iDeviceBounds.Size());
    iDeviceClipRect = iDeviceBounds;
    iDeviceOrigin = iDeviceBounds.iTl;
    iContext->SetPenStyle(CGraphicsContext::ENullPen);
    iContext->SetBrushStyle(CGraphicsContext::ENullBrush);
    iContext->SetOrigin(iOrigin);
    iContext->SetClippingRect(iClipRect);
}

void CLcdGraphics::DrawLine(const TPoint& aPoint1, const TPoint& aPoint2)
{
    TInt caps = CLcdGraphicsDevice::ECapDrawLine;
    if (iStrokeStyle == EStrokeDotted)
    {
        caps |= CLcdGraphicsDevice::ECapStrokeDotted;
    }
    if ((iLcdDrawCaps & caps) == caps)
    {
        ASSERT(iLcdDevice);

        TPoint p1(aPoint1);
        TPoint p2(aPoint2);

        p1 += iDeviceOrigin;
        p2 += iDeviceOrigin;

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);

        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);
                iLcdDevice->DrawLine(&iBitmap, p1, p2, iColor, (TStrokeStyle)iStrokeStyle, clip);
            }
        }

        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    else
    {
        LineMode();
        const TPoint& p1 = aPoint1;
        TPoint p2 = aPoint2;
        if (p1.iY == p2.iY)
        {
            /**
            Horizontal special case - adjust coords to include endpoint
            */
            if (p2.iX > p1.iX)
            {
                ++ p2.iX;
            }
            else
            {
                -- p2.iX;
            }
            iContext->DrawLine(p1,p2);
        }
        else if (p1.iX == p2.iX)
        {
            /**
            Vertical special case - adjust coords to include endpoint
            */
            if (p2.iY > p1.iY)
            {
                ++ p2.iY;
            }
            else
            {
                -- p2.iY;
            }
            iContext->DrawLine(p1, p2);
        }
        else
        {
            /**
            General slope - have to plot last point since SymbianOS GDI does
            not draw the last point on a line.
            */
            iContext->DrawLine(p1, p2);
            iContext->Plot(p2);
        }
    }
}

void CLcdGraphics::DrawRect(const TPoint& aPosition, const TSize& aSize)
{
    //
    // adjust coords for stroked primitives
    //
    TRect rect(aPosition, aSize);
    rect.iBr.iX++;
    rect.iBr.iY++;

    TInt caps = CLcdGraphicsDevice::ECapDrawRect;
    if (iStrokeStyle == EStrokeDotted)
    {
        caps |= CLcdGraphicsDevice::ECapStrokeDotted;
    }
    if ((iLcdDrawCaps & caps) == caps)
    {
        ASSERT(iLcdDevice);

        rect.Move(iDeviceOrigin);

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);

        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);
                iLcdDevice->DrawRect(&iBitmap, rect, iColor, (TStrokeStyle)iStrokeStyle, clip);
            }
        }

        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    else
    {
        OutlineMode();
        iContext->DrawRect(rect);
    }
}

void CLcdGraphics::FillRect(const TPoint& aPosition, const TSize& aSize)
{
    TRect rect(aPosition, aSize);
    if (iLcdDrawCaps & CLcdGraphicsDevice::ECapFillRect)
    {
        ASSERT(iLcdDevice);
        rect.Move(iDeviceOrigin);

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);

        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);
                iLcdDevice->FillRect(&iBitmap, rect, iColor, clip);
            }
        }

        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    else
    {
        FillMode();
        iContext->DrawRect(rect);
    }
}

void CLcdGraphics::DrawArc(const TPoint& aPosition, const TSize& aSize, TInt aStartAngle, TInt aArcAngle)
{
    if (!aArcAngle)
    {
        return;
    }

    TRect rect(aPosition, aSize);
    rect.iBr.iX++;
    rect.iBr.iY++;

    TInt caps = CLcdGraphicsDevice::ECapDrawArc;
    if (iStrokeStyle == EStrokeDotted)
    {
        caps |= CLcdGraphicsDevice::ECapStrokeDotted;
    }
    if ((iLcdDrawCaps & caps) == caps)
    {
        ASSERT(iLcdDevice);

        rect.Move(iDeviceOrigin);

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);

        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);
                iLcdDevice->DrawArc(&iBitmap, rect, aStartAngle, aArcAngle, iColor, iStrokeStyle, clip);
            }
        }

        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    else
    {
        OutlineMode();
        TPoint start;
        TPoint end;
        ArcVectors(start, end, rect, aStartAngle, aArcAngle);
        iContext->DrawArc(rect,start,end);
    }
}

void CLcdGraphics::FillArc(const TPoint& aPosition, const TSize& aSize, TInt aStartAngle, TInt aArcAngle)
{
    if (!aArcAngle)
    {
        return;
    }
    TRect rect(aPosition, aSize);
    if (iLcdDrawCaps & CLcdGraphicsDevice::ECapFillArc)
    {
        ASSERT(iLcdDevice);
        rect.Move(iDeviceOrigin);

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);

        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);
                iLcdDevice->FillArc(&iBitmap, rect, aStartAngle, aArcAngle, iColor, clip);
            }
        }

        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    else
    {
        // Temporarily turn on the pen and set the pen color.
        SetPenOff();
        SetBrushOn();

        // Grow the bounding rect by a 1 pixel border.
        rect.Grow(1,1);

        TPoint start;
        TPoint end;
        ArcVectors(start, end, rect, aStartAngle, aArcAngle);
        iContext->DrawPie(rect,start,end);
    }
}

void CLcdGraphics::CopyArea(const TPoint& aAreaPosition, const TSize& aAreaSize, const TPoint& aPoint, TInt aAnchor)
{
    TRect  rect(aAreaPosition, aAreaSize);
    TPoint vector = AnchorPoint(aPoint, aAnchor, aAreaSize) - aAreaPosition;
    iContext->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    iContext->CopyRect(vector, rect);
    iContext->SetDrawMode(CGraphicsContext::EDrawModePEN);
}

void CLcdGraphics::DrawRoundRect(const TPoint& aPosition, const TSize& aSize, const TSize& aCornerDiameter)
{
    TRect rect(aPosition, aSize);
    rect.iBr.iX++;
    rect.iBr.iY++;
    TSize cornerRadii;
    cornerRadii.iWidth  = aCornerDiameter.iWidth/2;
    cornerRadii.iHeight = aCornerDiameter.iHeight/2;
    OutlineMode();
    iContext->DrawRoundRect(rect, cornerRadii);
}

void CLcdGraphics::FillRoundRect(const TPoint& aPosition, const TSize& aSize, const TSize& aCornerDiameter)
{
    TRect rect(aPosition, aSize);
    TSize cornerRadii;
    cornerRadii.iWidth  = aCornerDiameter.iWidth/2;
    cornerRadii.iHeight = aCornerDiameter.iHeight/2;

    // Temporarily turn on the pen and set the pen color.
    SetBrushOn();
    iContext->SetPenColor(TRgb::Color16MA(iColor));
    iContext->SetPenStyle(CGraphicsContext::ESolidPen);
    iContext->DrawRoundRect(rect, cornerRadii);
    iState |= EPenOn;
    SetPenOff();    // keep in sync with stroke style
}

void CLcdGraphics::FillTriangle(const TPoint aPoints[3])
{
#ifdef RD_JAVA_NGA_ENABLED
    // Lcdgd implementation for fillTriangle does not support alpha blending,
    // use CFbsBitGc in case of non-opaque color.
    if ((iLcdDrawCaps & CLcdGraphicsDevice::ECapFillTriangle) &&
            ((iColor & 0xFF000000) == 0xFF000000))
#else
    if (iLcdDrawCaps & CLcdGraphicsDevice::ECapFillTriangle)
#endif
    {
        ASSERT(iLcdDevice);
        TPoint points[3];
        points[0] = aPoints[0]+iDeviceOrigin;
        points[1] = aPoints[1]+iDeviceOrigin;
        points[2] = aPoints[2]+iDeviceOrigin;

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);
        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);
                iLcdDevice->FillTriangle(&iBitmap, points, iColor, clip);
            }
        }

        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    else
    {
        // Temporarily set solid pen style and the pen color.
        SetBrushOn();
        iContext->SetPenColor(TRgb::Color16MA(iColor));
        iContext->SetPenStyle(CGraphicsContext::ESolidPen);
        iContext->DrawPolygon(aPoints, 3, CGraphicsContext::EAlternate);
        SetPenOff();    // keep in sync with stroke style
    }
}

// ---------------------------------------------------------------------------
// CMIDGraphics::DrawImage()
// ---------------------------------------------------------------------------
//
void CLcdGraphics::DrawImage(
    const CLcdImage& aImage, const TPoint& aPoint, TInt aAnchor, TBool aPremultiplied)
{
    TPoint regionPos;
    TSize  regionSize(aImage.Size());
    (void) DrawRegion(
        aImage, regionPos, regionSize, ETransNone, aPoint, aAnchor, aPremultiplied);
}

// ---------------------------------------------------------------------------
// CMIDGraphics::DrawRegion()
// Returns error if no blitter found.
// ---------------------------------------------------------------------------
//
TInt CLcdGraphics::DrawRegion
(
    const CLcdImage& aImage,
    const TPoint&    aRegionPos,
    const TSize&     aRegionSize,
    TInt             aTransform,
    const TPoint&    aPoint,
    TInt             aAnchor,
    TBool            aPremultiplied
)
{
    ASSERT(iLcdDrawCaps & CLcdGraphicsDevice::ECapDrawRegion);
    ASSERT(iLcdDevice);

    TTransformType transform = (TTransformType)aTransform;
    TAnchor anchor = (TAnchor)aAnchor;

    TRect srcRect(aRegionPos, aRegionSize);
    TPoint dstPoint(iDeviceOrigin + aPoint);

    TAcceleratedBitmapInfo srcColor;
    TAcceleratedBitmapInfo srcAlpha;

    CHECK_BITMAP_LOCK();

    iSurface->Begin(iBitmap, iCount);
    aImage.Lock(iCount, srcColor, srcAlpha);

    //
    // Compute destination rectangle with anchor point at aRegion.iDstPoint.
    //
    TRect dstRect = CalcDstRect(dstPoint, srcRect.Size(), transform, anchor);
    TInt error = KErrNone;

    TTransparency transparency = aImage.Transparency();

#ifdef RD_JAVA_NGA_ENABLED
    // In S60 9.2 and onward the target canvas offscreen is not necessarily
    // fully opaque when the M3G API is using hardware accelerated OpenGL ES API.
    // Note that in this case target pixels are in premultiplied form.
    if (aPremultiplied)
    {
        if (transparency == ETransparencyMaskChannel)
        {
            // Target contains premultiplied pixel components and
            // and source contains transparent pixels
            transparency = ETransparencyMaskChannelPre;
        }
        if (transparency == ETransparencyAlphaChannel)
        {
            // Target contains premultiplied pixel components and
            // and source contains transparent pixels
            transparency = ETransparencyAlphaChannelPre;
        }
    }
#else // !RD_JAVA_NGA_ENABLED
    (void)aPremultiplied;  // Just to remove a compiler warning
#endif // RD_JAVA_NGA_ENABLED

    RRegion* visibleRegion = iSurface->VisibleRegion();
    if (iBitmap.iAddress && visibleRegion)
    {
        const TRect* rects = visibleRegion->RectangleList();
        const TInt   count = visibleRegion->Count();
        for (TInt i=0; i<count; i++)
        {
            TRect clip(iDeviceClipRect);
            clip.Intersection(rects[i]);

            if (iIsCanvasGraphicsItemTarget)
            {
                // Because rendering function, which is used for drawing image,
                // doesn't allow to render on transparent target
                // (@see BlitLineColor16MAMaskToColor16MAOpaque), we have to
                // do it by ourselves.
                error = iLcdDevice->DrawRegionForCanvasGraphicsItem
                        (
                            &iBitmap,
                            dstRect,
                            &srcColor,
                            srcAlpha.iAddress ? &srcAlpha : NULL,
                            aImage.Transparency(),
                            srcRect,
                            transform,
                            clip,
                            ECanvasGraphicsItemImageRendering
                        );
            }
            else
            {
                error = iLcdDevice->DrawRegion
                        (
                            &iBitmap,
                            dstRect,
                            &srcColor,
                            srcAlpha.iAddress ? &srcAlpha : NULL,
                            transparency,
                            srcRect,
                            transform,
                            clip
                        );
            }
        }
    }

    aImage.Unlock(iCount);
    iSurface->End(iCount);

    CHECK_BITMAP_LOCK();

    ASSERT(error == KErrNone);

    return error;
}

void CLcdGraphics::DrawBackground(MMIDCanvas* aCanvas, const TPoint& aPosition, const TSize& aSize)
{

    CHECK_BITMAP_LOCK();

    iSurface->Begin(iBitmap, iCount);
    aCanvas->DrawBackground(*iContext, aPosition, aSize);
    iSurface->End(iCount);

    CHECK_BITMAP_LOCK();

}

TInt CLcdGraphics::DrawPixels
(
    TInt aType,
    const TUint8* aAddress,
    TInt DEBUG_ARG(aLength),
    TInt aScanLength,
    TBool aAlpha,
    const TPoint& aPosition,
    const TSize& aSize,
    TInt aTransform,
    TBool aPremultiplied
)
{
    ASSERT(iLcdDevice);
    TInt err = KErrNotSupported;
    if (aType == 0)     // we could support others now.
    {
        ASSERT(iLcdDrawCaps & CLcdGraphicsDevice::ECapDrawRegion);
        ASSERT((((TUint32)aAddress) & 0x3) == 0);       // must be word aligned
        ASSERT(aLength >= ((aSize.iHeight-1)*aScanLength + aSize.iWidth*sizeof(TUint32)));
        ASSERT(aScanLength >= 0);

        TTransformType transform  = (TTransformType)aTransform;
        TTransparency transparency = (aAlpha ? ETransparencyAlphaChannel : ETransparencyIgnoreChannel);

        TAcceleratedBitmapInfo srcBitmap;

        CLcdGraphicsDriver::TDriverInfo info;
        iLcdDriver.GetDriverInfo(info);

        srcBitmap.iPhysicalAddress = NULL;
        srcBitmap.iAddress     = const_cast<TUint8*>(aAddress);
        srcBitmap.iDisplayMode = info.iARGB8888Mode;
        srcBitmap.iSize        = aSize;
        srcBitmap.iLinePitch   = aScanLength;
        srcBitmap.iPixelShift  = 5;     // 32bpp


        TRect srcRect(aSize);
        TRect dstRect(aPosition, aSize);
        dstRect.Move(iDeviceOrigin);

        CHECK_BITMAP_LOCK();

        iSurface->Begin(iBitmap, iCount);

#ifdef RD_JAVA_NGA_ENABLED
        // In S60 9.2 and onward the target canvas offscreen is not necessarily
        // fully opaque when the M3G API is using hardware accelerated OpenGL ES API.
        // Note that in this case target pixels are in premultiplied form.
        if (aPremultiplied && transparency == ETransparencyAlphaChannel)
        {
            // Target contains premultiplied pixel components and
            // and source contains transparent pixels
            transparency = ETransparencyAlphaChannelPre;
        }
#else // !RD_JAVA_NGA_ENABLED
        (void)aPremultiplied;  // Just to remove a compiler warning
#endif // RD_JAVA_NGA_ENABLED

        RRegion* visibleRegion = iSurface->VisibleRegion();
        if (iBitmap.iAddress && visibleRegion)
        {
            const TRect* rects = visibleRegion->RectangleList();
            const TInt   count = visibleRegion->Count();
            for (TInt i=0; i<count; i++)
            {
                TRect clip(iDeviceClipRect);
                clip.Intersection(rects[i]);

                if (iIsCanvasGraphicsItemTarget)
                {
                    // Because rendering function, which is used for drawing
                    // RGB data, doesn't blend properly to transparent target
                    // (@see BlitLineColor16MAAlphaToColor16MAOpaque),
                    // we have to do it by ourselves.
                    err = iLcdDevice->DrawRegionForCanvasGraphicsItem
                          (
                              &iBitmap,
                              dstRect,
                              &srcBitmap,
                              NULL,
                              transparency,
                              srcRect,
                              transform,
                              clip,
                              ECanvasGraphicsItemRGBRendering
                          );
                }
                else
                {
                    err = iLcdDevice->DrawRegion
                          (
                              &iBitmap,
                              dstRect,
                              &srcBitmap,
                              NULL,
                              transparency,
                              srcRect,
                              transform,
                              clip
                          );
                }
            }
        }


        iSurface->End(iCount);

        CHECK_BITMAP_LOCK();
    }
    return err;
}

static TBool IsUniformlyLeftToRight(const TDesC& aText)
{
    const TInt length = aText.Length();

    for (TInt i = 0; i < length; i++)
    {
        switch (TChar(aText[i]).GetBdCategory())
        {
        case TChar::ERightToLeft:
        case TChar::ERightToLeftArabic:
        case TChar::ERightToLeftEmbedding:
        case TChar::ERightToLeftOverride:
        case TChar::EArabicNumber:

            return EFalse;

        default:
            ;
        }
    }
    return ETrue;
}

void CLcdGraphics::DrawText(const TDesC& aText, const TPoint& aPoint, TInt aAnchor)
{
    LineMode();

    ASSERT(iFont);
    TPoint pos = aPoint;

    if (aAnchor & EAnchorTop)
    {
        pos.iY += iFont->FontMaxAscent();
    }
    else if (aAnchor & EAnchorBottom)
    {
        pos.iY -= (iHeight - iFont->FontMaxAscent()); // iHeight includes leading
    }

    if (aAnchor & EAnchorHCenter)
    {
        pos.iX -= iFont->TextWidthInPixels(aText)/2;
    }
    else if (aAnchor & EAnchorRight)
    {
        pos.iX -= iFont->TextWidthInPixels(aText);
    }

    if (IsUniformlyLeftToRight(aText))
    {
#ifdef __SERIES60_
        CAknPictographInterface* aknPictographInterface = iMIDPictograph->PictographInterface();

        if (aknPictographInterface)
        {
            MAknPictographDrawer* aknPictographDrawer = aknPictographInterface->Interface();
            aknPictographDrawer->DrawText(*iContext, *iFont, aText, pos);
        }
        else
        {
            iContext->DrawText(aText, pos);
        }

#else
        iContext->DrawText(aText, pos);
#endif
    }
    else
    {
        TBidiText* bidiText = NULL;

        TRAPD(status, bidiText = TBidiText::NewL(aText, 1));
        if (status != KErrNone)
        {
            return;
        }
        bidiText->WrapText(iFont->TextWidthInPixels(aText), *iFont);
#ifdef __SERIES60_
        CAknPictographInterface* aknPictographInterface = iMIDPictograph->PictographInterface();

        if (aknPictographInterface)
        {
            MAknPictographDrawer* aknPictographDrawer = aknPictographInterface->Interface();
            aknPictographDrawer->DrawText(*iContext, *iFont, aText, pos);
        }
        else
        {
            bidiText->DrawText(*iContext, pos);
        }
#else
        bidiText->DrawText(*iContext, pos);
#endif
        delete bidiText;
    }
}

void CLcdGraphics::Flush(const TPoint& aPosition, const TSize& aSize)
{
    TRect deviceRect(aPosition, aSize);
    deviceRect.Move(iDeviceBounds.iTl);
    iSurface->Update(deviceRect);
}

#ifdef RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// CLcdGraphics::CopyGraphics()
// Copies source graphics content
// ---------------------------------------------------------------------------
//
TInt CLcdGraphics::CopyGraphics(CLcdGraphics* aSrcGraphics)
{
    ASSERT(iLcdDevice);
    CLcdSurface* srcSurface = aSrcGraphics->Surface();
    TInt error = KErrNone;
    CHECK_BITMAP_LOCK();
    iSurface->Begin(iBitmap, iCount);
    srcSurface->Begin(aSrcGraphics->iBitmap, iCount);

    RRegion* visibleRegion = iSurface->VisibleRegion();
    if (iBitmap.iAddress && visibleRegion)
    {
        const TRect* rects = visibleRegion->RectangleList();
        const TInt   count = visibleRegion->Count();
        for (TInt i=0; i<count; i++)
        {
            TRect clip(iDeviceClipRect);
            clip.Intersection(rects[i]);

            error = iLcdDevice->DrawRegion(
                        &iBitmap,
                        iSurface->Bounds(),
                        &aSrcGraphics->iBitmap,
                        NULL,
                        ETransparencyIgnoreChannel,
                        iSurface->Bounds(),
                        ETransNone,
                        clip);
        }
    }
    srcSurface->End(iCount);
    iSurface->End(iCount);
    CHECK_BITMAP_LOCK();
    ASSERT(error == KErrNone);
    return error;
}

#endif // RD_JAVA_NGA_ENABLED

void ArcVectors(TPoint& aStart, TPoint& aEnd, const TRect& aRect, TInt aStartAngle, TInt aArcAngle)
{
    TPoint  point = aRect.iTl;
    TSize   size  = aRect.Size();

    //
    // Center.
    //
    point.iX += size.iWidth/2;
    point.iY += size.iHeight/2;

    if (aArcAngle>=360 || aArcAngle <=-360)
    {
        aStart = point;
        aStart.iX += size.iWidth/2;
        aEnd   = aStart;
    }
    else
    {
        TInt begAngle = aStartAngle;
        TInt endAngle = aStartAngle+aArcAngle;
        if (aArcAngle < 0)
        {
            begAngle = endAngle;
            endAngle = aStartAngle;
        }
        aEnd   = point + Vector(endAngle, size);
        aStart = point + Vector(begAngle, size);
    }
}

TPoint Vector(const TReal& aAngle, const TSize& aSize)
{
    TReal  angle(aAngle * KDegToRad);

    TReal  x;
    Math::Cos(x, angle);

    TReal  y;
    Math::Sin(y, angle);

    x *= aSize.iWidth;
    y *= -aSize.iHeight;

    return TPoint((TInt)x, (TInt)y);
}

TPoint AnchorPoint(const TPoint& aPoint, TInt aAnchor, const TSize& aSize)
{
    TPoint point = aPoint;

    if (aAnchor & MMIDGraphics::EAnchorRight)
    {
        point.iX -= aSize.iWidth;
    }
    else if (aAnchor & MMIDGraphics::EAnchorHCenter)
    {
        point.iX -= aSize.iWidth/2;
    }

    if (aAnchor & MMIDGraphics::EAnchorVCenter)
    {
        point.iY -= aSize.iHeight/2;
    }
    else if (aAnchor & MMIDGraphics::EAnchorBottom)
    {
        point.iY -= aSize.iHeight;
    }

    return point;
}

void CLcdGraphics::SetCanvasGraphicsItemtarget(TBool aIsCanvasGraphicsItemTarget)
{
    iIsCanvasGraphicsItemTarget = aIsCanvasGraphicsItemTarget;
}

/////////////////////////////////////////////////////////////
// CMIDPictograph is for Pictograpgh in S60
//
////////////////////////////////////////////////////////////
#ifdef __SERIES60_

CMIDPictograph* CMIDPictograph::NewL()
{
    CMIDPictograph* self = new(ELeave) CMIDPictograph;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}
//
CMIDPictograph::CMIDPictograph() {}
void CMIDPictograph::ConstructL()
{
    CCoeControl *dummyControl = NULL;
    iPictographInterface = CAknPictographInterface::NewL(*dummyControl, *this);
}
//Not doing any thing, Canvas has to draw the pictograph
//by calling draw again and again.
void CMIDPictograph::DrawPictographArea() {}
CAknPictographInterface* CMIDPictograph::PictographInterface()
{
    return iPictographInterface;
}

CMIDPictograph::~CMIDPictograph()
{
    delete iPictographInterface;
}

#endif
