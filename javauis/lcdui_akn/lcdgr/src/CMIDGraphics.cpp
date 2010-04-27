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

#include <fbs.h>
#include <bitdev.h>
#include <gdi.h>

#ifdef RD_JAVA_NGA_ENABLED
#include <coecntrl.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include <lcdgdrv.h> // for TTransformType
#endif // RD_JAVA_NGA_ENABLED

#include "fbslcdgraphics.h"

#include "CMIDGraphics.h"
#include "CMIDImage.h"

#include "MidProxyMap.h"
#include "LcdGraphics.h"
#include "LcdImage.h"
#include "LcdSurface.h"
#include "LcdBitmapSurface.h"
#include "LcdWindowSurface.h"

#define UnhandAddress(aAddress) (aAddress << 2)

template <class T>
inline T* Unhand(TInt aAddress)
{
    return reinterpret_cast<T*>(UnhandAddress(aAddress));
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
#ifdef RD_JAVA_NGA_ENABLED
CMIDGraphics::CMIDGraphics(
    CMIDProxyMap& aProxyMap, CLcdGraphics* aGraphics,
    TBool aIsImageTarget, MMIDCanvas* aTarget)
        : iCanvasTarget(aTarget)
        , iProxyMap(aProxyMap)
        , iGraphics(aGraphics)
        , iIsImageTarget(aIsImageTarget)
{
}
#else // !RD_JAVA_NGA_ENABLED
CMIDGraphics::CMIDGraphics(
    CMIDProxyMap& aProxyMap, CLcdGraphics* aGraphics, TBool aIsImageTarget)
        : iProxyMap(aProxyMap)
        , iGraphics(aGraphics)
        , iIsImageTarget(aIsImageTarget)
{
}
#endif // RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMIDGraphics::~CMIDGraphics()
{
    delete iGraphics;
}

// ---------------------------------------------------------------------------
// CMIDGraphics::Type()
// ---------------------------------------------------------------------------
//
MMIDComponent::TType CMIDGraphics::Type() const
{
    return EGraphics;
}

// ---------------------------------------------------------------------------
// CMIDGraphics::Dispose()
// ---------------------------------------------------------------------------
//
void CMIDGraphics::Dispose()
{
    delete this;
}

// ---------------------------------------------------------------------------
// From MMIDBufferProcessor
// CMIDGraphics::Processor()
// ---------------------------------------------------------------------------
//
MMIDBufferProcessor* CMIDGraphics::Processor()
{
    return this;
}

// ---------------------------------------------------------------------------
// From MMIDBufferProcessor
// CMIDGraphics::AbortAsync()
// ---------------------------------------------------------------------------
//
void CMIDGraphics::AbortAsync()
{
}

// ---------------------------------------------------------------------------
// From MMIDBufferProcessor
// CMIDGraphics::ProcessL()
// ---------------------------------------------------------------------------
//
#ifdef RD_JAVA_NGA_ENABLED
TBool CMIDGraphics::ProcessL(
    const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
    TInt& /*aCycles*/, java::util::Monitor* /*aMonitor*/)
{
    // Checks if EGL is availabe.
    TBool eglAvailable = EFalse;
    if (iCanvasTarget && iCanvasTarget->IsEglAvailable())
    {
        // Egl is available so bounding boxes of 2D primitives
        // need to be calculated.
        eglAvailable = ETrue;
    }
#else // !RD_JAVA_NGA_ENABLED
TBool CMIDGraphics::ProcessL(
    const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
    TInt& /*aCycles*/, TRequestStatus* /*aStatus*/)
{
#endif // RD_JAVA_NGA_ENABLED   
    CLcdGraphics& graphics = *iGraphics;

    graphics.Begin();
    while (aRead < aEnd)
    {
        TInt opCode = aRead->OpCode();

        switch (opCode)
        {
        case EGrOpSetColor:
        {
            const TLcdGrOpSetColor* setColor = (const TLcdGrOpSetColor*)(aRead);
            graphics.SetColor(setColor->iColor);
        }
        break;

        case EGrOpSetStroke:
        {
            const TLcdGrOpSetStrokeStyle* setStroke =
                (const TLcdGrOpSetStrokeStyle*)(aRead);
            graphics.SetStrokeStyle((TStrokeStyle)setStroke->iStyle);
        }
        break;

        case EGrOpSetFont:
        {
            const TLcdGrOpSetFont* setFont = (const TLcdGrOpSetFont*)(aRead);
            graphics.SetFont(setFont->iFont);
        }
        break;

        case EGrOpSetClipRect:
        {
            const TLcdGrOpSetClipRect* clipRect= (const TLcdGrOpSetClipRect*)(aRead);
            graphics.SetClipRect(clipRect->iPosition, clipRect->iSize);
        }
        break;

        case EGrOpTranslate:
        {
            const TLcdGrOpTranslate* translate = (const TLcdGrOpTranslate*)(aRead);
            graphics.Translate(translate->iVector);
        }
        break;

        case EGrOpReset:
        {
            const TLcdGrOpReset* reset = (const TLcdGrOpReset*)(aRead);
            graphics.Reset(reset->iSize);
        }
        break;

        case EGrOpDrawLine:
        {
            ASSERT(aRead->Size() >= 5);
            const TLcdGrOpDrawLine* line = (const TLcdGrOpDrawLine*)(aRead);
            graphics.DrawLine(line->iPoints[0], line->iPoints[1]);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                // Update bounding box
                TRect rect(line->iPoints[0], line->iPoints[1]);
                rect.Normalize();
                // Move br by one pixel to include the end point
                ++rect.iBr.iX;
                ++rect.iBr.iY;
                UpdateRect(rect);
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpDrawRect:
        {
            const TLcdGrOpDrawRect* drawRect = (const TLcdGrOpDrawRect*)(aRead);
            graphics.DrawRect(drawRect->iPosition, drawRect->iSize);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(
                               drawRect->iPosition,
                               TSize(drawRect->iSize.iWidth + 1, drawRect->iSize.iHeight + 1)));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpFillRect:
        {
            const TLcdGrOpFillRect* fillRect = (const TLcdGrOpFillRect*)(aRead);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                TRect rect(fillRect->iPosition, fillRect->iSize);
                if (!FillEglSurface(rect, TRgb::Color16MA(graphics.Color())))
                {
                    graphics.FillRect(fillRect->iPosition, fillRect->iSize);
                    UpdateRect(TRect(
                                   fillRect->iPosition,
                                   TSize(fillRect->iSize.iWidth + 1,
                                         fillRect->iSize.iHeight + 1)));
                }
            }
            else
            {
                graphics.FillRect(fillRect->iPosition, fillRect->iSize);
            }
#else // !RD_JAVA_NGA_ENABLED
            graphics.FillRect(fillRect->iPosition, fillRect->iSize);
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpDrawRoundRect:
        {
            const TLcdGrOpDrawRoundRect* drawRoundRect =
                (const TLcdGrOpDrawRoundRect*)(aRead);
            graphics.DrawRoundRect(
                drawRoundRect->iPosition, drawRoundRect->iSize,
                drawRoundRect->iCornerSize);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(
                               drawRoundRect->iPosition,
                               TSize(drawRoundRect->iSize.iWidth + 1,
                                     drawRoundRect->iSize.iHeight + 1)));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpFillRoundRect:
        {
            const TLcdGrOpFillRoundRect* fillRoundRect =
                (const TLcdGrOpFillRoundRect*)(aRead);
            graphics.FillRoundRect(
                fillRoundRect->iPosition, fillRoundRect->iSize,
                fillRoundRect->iCornerSize);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(
                               fillRoundRect->iPosition,
                               TSize(fillRoundRect->iSize.iWidth + 1,
                                     fillRoundRect->iSize.iHeight + 1)));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpDrawArc:
        {
            const TLcdGrOpDrawArc* drawArc = (const TLcdGrOpDrawArc*)(aRead);
            graphics.DrawArc(
                drawArc->iPosition, drawArc->iSize, drawArc->iStart,
                drawArc->iAngle);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(
                               drawArc->iPosition,
                               TSize(drawArc->iSize.iWidth + 1, drawArc->iSize.iHeight + 1)));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpFillArc:
        {
            const TLcdGrOpFillArc* fillArc = (const TLcdGrOpFillArc*)(aRead);
            graphics.FillArc(
                fillArc->iPosition, fillArc->iSize, fillArc->iStart,
                fillArc->iAngle);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(
                               fillArc->iPosition,
                               TSize(fillArc->iSize.iWidth + 1, fillArc->iSize.iHeight + 1)));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpDrawText:
        {
            const TLcdGrOpDrawText* drawText = (const TLcdGrOpDrawText*)(aRead);
            TPtrC ptr(&drawText->iText[0], drawText->iLength);
            graphics.DrawText(ptr, drawText->iPoint, drawText->iAnchor);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateTextRect(ptr, drawText->iPoint, drawText->iAnchor);
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpDrawImage:
        {
            const TLcdGrOpDrawImage* drawImage = (const TLcdGrOpDrawImage*)(aRead);
            MMIDComponent* component = Unhand<MMIDComponent>(drawImage->iImage);
            ASSERT(component);
            const CMIDImage& proxy =
                static_cast<const CMIDImage&>(*static_cast<MMIDImage*>(component));
            const CLcdImage& image = proxy.Image();
#ifndef RD_JAVA_NGA_ENABLED
            graphics.DrawImage(image, drawImage->iPoint, drawImage->iAnchor);
#else // RD_JAVA_NGA_ENABLED
            graphics.DrawImage(image, drawImage->iPoint, drawImage->iAnchor, eglAvailable);
            if (eglAvailable)
            {
                UpdateRect(CalcDstRect(
                               drawImage->iPoint, image.Size(), ETransNone,
                               (::TAnchor)drawImage->iAnchor));
            }
#endif // !RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpDrawRegion:
        {
            const TLcdGrOpDrawRegion* drawRegion = (const TLcdGrOpDrawRegion*)(aRead);
            MMIDComponent* component = Unhand<MMIDComponent>(drawRegion->iImage);
            ASSERT(component);
            const CMIDImage& proxy =
                static_cast<const CMIDImage&>(*static_cast<MMIDImage*>(component));
            const CLcdImage& image = proxy.Image();
#ifndef RD_JAVA_NGA_ENABLED
            graphics.DrawRegion(
                image, drawRegion->iSrcPosition, drawRegion->iSrcSize,
                drawRegion->iTransform, drawRegion->iDstPoint,
                drawRegion->iAnchor);
#else // RD_JAVA_NGA_ENABLED
            graphics.DrawRegion(
                image, drawRegion->iSrcPosition, drawRegion->iSrcSize,
                drawRegion->iTransform, drawRegion->iDstPoint,
                drawRegion->iAnchor, eglAvailable);
            if (eglAvailable)
            {
                UpdateRect(CalcDstRect(drawRegion->iDstPoint, drawRegion->iSrcSize,
                                       (TTransformType)drawRegion->iTransform,
                                       (::TAnchor)drawRegion->iAnchor));
            }
#endif // !RD_JAVA_NGA_ENABLED   
        }
        break;

        case EGrOpCopyArea:
        {
            // NGA: EGrOpCopyArea is not valid for canvas
            const TLcdGrOpCopyArea* copyArea = (const TLcdGrOpCopyArea*)(aRead);
            graphics.CopyArea(copyArea->iSrcPosition, copyArea->iSrcSize,
                              copyArea->iPoint, copyArea->iAnchor);
        }
        break;

        case EGrOpFillTriangle:
        {
            ASSERT(aRead->Size() >= 5);
            const TLcdGrOpFillTriangle* triangle = (const TLcdGrOpFillTriangle*)(aRead);
            graphics.FillTriangle(triangle->iPoints);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(
                               Min3(triangle->iPoints[ 0 ].iX, triangle->iPoints[ 1 ].iX,
                                    triangle->iPoints[ 2 ].iX),
                               Min3(triangle->iPoints[ 0 ].iY, triangle->iPoints[ 1 ].iY,
                                    triangle->iPoints[ 2 ].iY),
                               Max3(triangle->iPoints[ 0 ].iX, triangle->iPoints[ 1 ].iX,
                                    triangle->iPoints[ 2 ].iX) + 1,
                               Max3(triangle->iPoints[ 0 ].iY, triangle->iPoints[ 1 ].iY,
                                    triangle->iPoints[ 2 ].iY) + 1));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;
        case EGrOpFlush:
        {
            const TLcdGrOpFlush* flush = (const TLcdGrOpFlush*)(aRead);
            graphics.Flush(flush->iPosition, flush->iSize);
        }
        break;
        case EGrOpDrawBackground:
        {
            const TLcdGrOpDrawBackground* drawBackground =
                (const TLcdGrOpDrawBackground*)(aRead);
            MMIDCanvas* canvas = Unhand<MMIDCanvas>(drawBackground->iCanvas);
            ASSERT(canvas);
            graphics.DrawBackground(canvas, drawBackground->iPosition, drawBackground->iSize);
#ifdef RD_JAVA_NGA_ENABLED
            if (eglAvailable)
            {
                UpdateRect(TRect(drawBackground->iPosition, drawBackground->iSize));
            }
#endif // RD_JAVA_NGA_ENABLED   
        }
        break;
        case EGrOpCopyGraphics:
        {
#ifdef RD_JAVA_NGA_ENABLED
            const TLcdGrOpCopyGraphics* copyGraphics =
                (const TLcdGrOpCopyGraphics*)(aRead);
            MMIDComponent* component = Unhand<MMIDComponent>(copyGraphics->iSrcGraphics);
            ASSERT(component);
            CMIDGraphics* proxy =
                static_cast<CMIDGraphics*>(static_cast<MMIDGraphics*>(component));
            CLcdGraphics* srcGraphics = proxy->iGraphics;
            proxy->UpdateOffScreenBitmapL(ETrue);
            graphics.CopyGraphics(srcGraphics);

#endif // RD_JAVA_NGA_ENABLED   
        }
        break;
        default:
#ifdef _TRACE
            RDebug::Print(_L("CMIDGraphics::ProcessL - ignoring unknown opcode %d"), opCode);
#endif
            break;
        }
        aRead += aRead->Size();
    }
    graphics.End();
    return EFalse;
}


// ---------------------------------------------------------------------------
// CMIDGraphics::DrawPixels()
// ---------------------------------------------------------------------------
//
TInt CMIDGraphics::DrawPixels
(
    TInt    aType,          // pixel data type
    TUint8* aAddress,       // pixel data array
    TInt    aLength,        // pixel array length - bytes
    TInt    aScanLength,    // always positive - vertical reflection handled by point/size
    TBool   aAlpha,         // true if pixel data contains alpha
    const TRect& aRect,
    TInt    aTransform      // transform to apply
)
{
    TInt err;
    TSize size(aRect.Size());
    iGraphics->Begin();

    TBool premultiplied(EFalse);
#ifdef RD_JAVA_NGA_ENABLED
    if (IsEglAvailable())
    {
        premultiplied = ETrue;
        UpdateRect(aRect);
    }
#endif

    err = iGraphics->DrawPixels(aType, aAddress, aLength, aScanLength,
                                aAlpha, aRect.iTl, size, aTransform, premultiplied);
    iGraphics->End();
    return err;
}

// ---------------------------------------------------------------------------
// CMIDGraphics::DisplayColor()
// Maps a color provided as 32bit rgb triple to the closest color representable
// in the target devices display mode. returns the result as a 32bit rgb triple.
// ---------------------------------------------------------------------------
//
TUint32 CMIDGraphics::DisplayColor(TUint32 aRGB32)
{
    return iGraphics->QuantizeColor(aRGB32);
}

// ---------------------------------------------------------------------------
// CMIDGraphics::Bitmap()
// Returns a pointer to the back buffer bitmap in use by the graphics object.
// May return NULL.
// ---------------------------------------------------------------------------
//
CFbsBitmap* CMIDGraphics::Bitmap() const
{
    CLcdSurface* surface = iGraphics->Surface();
    if (surface->Type() == CLcdSurface::EBitmapSurface)
    {
        CLcdBitmapSurface* bitmapSurface = (CLcdBitmapSurface*)surface;
        return bitmapSurface->Bitmap();
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CMIDGraphics::Window()
// ---------------------------------------------------------------------------
//
RWindow* CMIDGraphics::Window() const
{
    CLcdSurface* surface = iGraphics->Surface();
    if (surface->Type() == CLcdSurface::EWindowSurface)
    {
        CLcdWindowSurface* windowSurface = (CLcdWindowSurface*)surface;
        return windowSurface->Window();
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CMIDGraphics::ViewPort()
// @return drawing surface viewport rectangle.
// ---------------------------------------------------------------------------
//
TRect CMIDGraphics::ViewPort() const
{
    return iGraphics->Surface()->Bounds();
}

// ---------------------------------------------------------------------------
// CMIDGraphics::IsImageTarget()
// @return ETrue for mutable off-screen images (not for canvas/GameCanvas framebuffer)
// or EFalse for canvas/GameCanvas framebuffer
// ---------------------------------------------------------------------------
//
TBool CMIDGraphics::IsImageTarget() const
{
    return iIsImageTarget;
}

#ifdef RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDGraphics::UpdateEglContent()
// ---------------------------------------------------------------------------
//
void CMIDGraphics::UpdateEglContent()
{
    if (iCanvasTarget)
    {
        iCanvasTarget->UpdateEglContent();
    }
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDGraphics::IsEglAvailable()
// ---------------------------------------------------------------------------
//
TBool CMIDGraphics::IsEglAvailable()
{
    return (iCanvasTarget ? iCanvasTarget->IsEglAvailable() : EFalse);
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDGraphics::BindEglSurface()
// ---------------------------------------------------------------------------
//
EGLSurface CMIDGraphics::BindEglSurface()
{
    return (iCanvasTarget ? iCanvasTarget->BindEglSurface() : EGL_NO_SURFACE);
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDGraphics::ReleaseEglSurface()
// ---------------------------------------------------------------------------
//
void CMIDGraphics::ReleaseEglSurface()
{
    if (iCanvasTarget)
    {
        iCanvasTarget->ReleaseEglSurface();
    }
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDGraphics::UpdateOffScreenBitmapL()
// ---------------------------------------------------------------------------
//
void CMIDGraphics::UpdateOffScreenBitmapL(TBool aForced)
{
    if (iCanvasTarget)
    {
        iCanvasTarget->UpdateOffScreenBitmapL(aForced);
    }
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDGraphics::FillEglSurface()
// ---------------------------------------------------------------------------
//
TBool CMIDGraphics::FillEglSurface(const TRect& aRect, const TRgb& aColor)
{
    if (iCanvasTarget)
    {
        TRect rect(aRect);
        rect.Move(iGraphics->CurrentOriginInTargetCoords());
        rect.Intersection(iGraphics->CurrentClipRectInTargetCoords());
        TSize canvasSize = iCanvasTarget->Control().Size();
        rect.Intersection(TRect(iCanvasTarget->Control().Size()));
        return iCanvasTarget->FillEglSurface(rect, aColor);
    }
    return EFalse;
}


// ---------------------------------------------------------------------------
// CMIDGraphics::CanvasTargetSize()
// ---------------------------------------------------------------------------
//
TSize CMIDGraphics::CanvasTargetSize() const
{
    if (iCanvasTarget)
    {
        return iCanvasTarget->ContentSize();
    }
    else
    {
        return TSize();
    }
}

// ---------------------------------------------------------------------------
// CMIDGraphics::GetTargetCanvas()
// ---------------------------------------------------------------------------
//
MMIDCanvas* CMIDGraphics::GetTargetCanvas() const
{
    return iCanvasTarget;
}

// ---------------------------------------------------------------------------
// CMIDGraphics::UpdateRect()
// Merges aRect to member TRects that are used for tracking the updated canvas
// area. Current coordinate translation and clip rect are taken into account.
// ---------------------------------------------------------------------------
//
void CMIDGraphics::UpdateRect(const TRect& aRect)
{
    // Translate rect to target (=canvas) coordinates
    TRect rect(aRect);
    rect.Move(iGraphics->CurrentOriginInTargetCoords());

    // Clip with current clip rect and canvas bounds
    rect.Intersection(iGraphics->CurrentClipRectInTargetCoords());

    iCanvasTarget->UpdateRect(rect);
}

// ---------------------------------------------------------------------------
// CMIDGraphics::UpdateTextRect()
// Calculates bounding box for text.
// ---------------------------------------------------------------------------
//
void CMIDGraphics::UpdateTextRect(TPtrC& aTextPtr, const TPoint& aP, TInt aAnchor)
{
    CFont* font = iGraphics->CurrentFont();

    // Create a bounding box for the text
    // TextWidthInPixels() seems to return few pixels too small value.
    // Width of the zero char is added to be sure that bouding box is
    // long enough.
    TRect boundingBox(aP, TSize(font->TextWidthInPixels(aTextPtr) +
                                font->WidthZeroInPixels(), 0));
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull(font);

    if (layoutFont)
    {
        boundingBox.SetHeight(layoutFont->TextPaneHeight());
    }
    else
    {
        boundingBox.SetHeight(font->FontMaxHeight());
    }

    // Moving the bounding box according to the anchor. The EAnchorTop is
    // default vertically and the EAnchorLeft is default horizontally
    if (aAnchor & EAnchorBaseline)
    {
        boundingBox.Move(0, -font->FontMaxAscent());
    }
    else if (aAnchor & EAnchorBottom)
    {
        boundingBox.Move(0, -boundingBox.Height());
    }

    if (aAnchor & EAnchorHCenter)
    {
        boundingBox.Move(-(boundingBox.Width() / 2), 0);
    }
    else if (aAnchor & EAnchorRight)
    {
        boundingBox.Move(-boundingBox.Width(), 0);
    }
    UpdateRect(boundingBox);
}

#endif // RD_JAVA_NGA_ENABLED
