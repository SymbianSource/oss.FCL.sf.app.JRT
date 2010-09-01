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

#ifndef LCDGRAPHICS_H
#define LCDGRAPHICS_H

#include <lcdui.h>
#include <reflcdui.h>   // for direct container/content
#include <lcdgr.h>
#include <lcdgdrv.h>
#include "LcdSurface.h"

#ifdef __SERIES60_
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>

class CMIDPictograph;
#endif

class CCoeControl;
class CFbsBitmap;
class CLcdImage;
class CLcdGraphicsDriver;

TBool IsStronglyRightToLeft(const TDesC& aText);
void  ArcVectors(TPoint& aStart, TPoint& aEnd, const TRect& aRect, TInt aStartAngle, TInt aArcAngle);

/**
 * Native peer to javax.microedition.lcdui.Graphics.
 */
NONSHARABLE_CLASS(CLcdGraphics) : public CBase
{
public:
    static CLcdGraphics* NewL(CLcdGraphicsDriver& aDriver, CCoeControl& aWindow, MDirectContainer& aContainer, TBool aUpdate);
    static CLcdGraphics* NewL(CLcdGraphicsDriver& aDriver, const CFbsBitmap* aBitmap);

    /**
     * function takes ownership of aSurface - do not push aSurface onto cleanup stack
     */
    static CLcdGraphics* NewL(CLcdGraphicsDriver& aDriver, CLcdSurface* aSurface);

    ~CLcdGraphics();

    /**
     * Prepare to draw
     */
    void Begin();

    /**
     * drawing complete.
     */
    void End();

    /**
     * flush drawing in rect at <CODE>aPosition</CODE> of size <CODE>aSize</CODE>
     * (in device coords) to surface.
     */
    void Flush(const TPoint& aPosition, const TSize& aSize);

    TUint32 QuantizeColor(TUint32 aColor);

    void Reset(const TSize& aSize);

    void SetColor(TUint32 aColor);
    TUint32 Color() const;

    void SetStrokeStyle(TStrokeStyle aStyle);
    TStrokeStyle StrokeStyle() const;

    void SetFont(TUint32 aFontID);

    void SetClipRect(const TPoint& aPosition, const TSize& aSize);
    const TRect& ClipRect() const;

    void Translate(const TPoint& aPoint);
    const TPoint& Origin() const;

    void CopyArea(const TPoint& aPosition, const TSize& aSize, const TPoint& aPoint, TInt aAnchor);
    void DrawLine(const TPoint& aStart, const TPoint& aEnd);
    void DrawRect(const TPoint& aPosition, const TSize& aSize);
    void FillRect(const TPoint& aPosition, const TSize& aSize);
    void DrawRoundRect(const TPoint& aPosition, const TSize& aSize, const TSize& aCornerDiameter);
    void FillRoundRect(const TPoint& aPosition, const TSize& aSize, const TSize& aCornerDiameter);
    void DrawArc(const TPoint& aPosition, const TSize& aSize, TInt aStartAngle, TInt aAngle);
    void FillArc(const TPoint& aPosition, const TSize& aSize, TInt aStartAngle, TInt aAngle);
    void DrawText(const TDesC& aText, const TPoint& aPoint, TInt aAnchor);
    void FillTriangle(const TPoint* aPoints);
    TInt DrawPixels
    (
        TInt aType,
        const TUint8* aAddress,
        TInt    aLength,
        TInt    aScanLength,
        TBool   aAlpha,
        const TPoint& aPosition,
        const TSize& aSize,
        TInt aTransform,
        TBool aPremultiplied
    );

    /**
     * Renders image
     * @param aImage Source image
     * @param aPoint Render point
     * @param aAnchor Maps the render point, source image and render surface
     * @param aPremultiplied If true then a target color channel is premultiplied with alpha value.
     */
    void DrawImage(const CLcdImage& aImage, const TPoint& aPoint, TInt aAnchor,
                   TBool aPremultiplied = EFalse);
    /**
     * Renders image region
     * @param aImage Source image
     * @param aPoint Source point
     * @param aRegion Source width and height
     * @param aTransform Defines transform mode
     * @param aPoint Render point
     * @param aAnchor Maps the render point, region and render surface
     * @param aPremultiplied If true then a target color channel is premultiplied with alpha value.
     */
    TInt DrawRegion(
        const CLcdImage& aImage, const TPoint& aRegionPos, const TSize& aRegionSize,
        TInt aTransform, const TPoint& aPoint, TInt aAnchor, TBool aPremultiplied = EFalse);

    /**
     * @see MMIDCanvas::DrawBackground()
     * @since S60 5.0
     */
    void DrawBackground(MMIDCanvas* aCanvas, const TPoint& aPosition, const TSize& aSize);

    /**
     * Support for MMIDGraphics::Bitmap()/MMIDGraphics:Window().
     *
     *@return the drawing surface for this graphics instance.
     */
    CLcdSurface* Surface();

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Copies graphics content
     * @param aSrcGraphics The source graphics
     * @return KErrNone if succeeded.
     * @since S60 9.2
     */
    TInt CopyGraphics(CLcdGraphics* aSrcGraphics);

    /**
     * Gets current clip rectangle.
     * @return The clip rectangle in user coordinates
     * @since S60 9.2
     */
    inline TRect  CurrentClipRectInTargetCoords() const;

    /**
     * Gets current origin.
     * @return The origin in user coordinates
     * @since S60 9.2
     */
    inline TPoint CurrentOriginInTargetCoords() const;

    /**
     * Gets current font.
     * @return The current font
     * @since S60 9.2
     */
    inline CFont* CurrentFont() const;

#endif // RD_JAVA_NGA_ENABLED

    /**
     * Sets the flag, which indicates that rendering target is framebuffer
     * of CanavsGraphicsItem.
     */
    void SetCanvasGraphicsItemtarget(TBool aIsCanvasGraphicsItemTarget);

private:
    CLcdGraphics(CLcdGraphicsDriver& aDriver, CLcdSurface* aSurface);
    void ConstructL();

private:
    enum TState
    {
        EPenOn      = 1,
        EBrushOn    = 2,
        EPenColor   = 4,
        EBrushColor = 8
    };

private:
    inline void SetPenOn();
    inline void SetPenOff();
    inline void SetBrushOn();
    inline void SetBrushOff();
    inline void LineMode();
    inline void FillMode();
    inline void OutlineMode();

private:
    CLcdGraphicsDriver& iLcdDriver;
    CLcdGraphicsDevice* iLcdDevice;
    TUint32             iLcdDrawCaps;

#ifdef __SERIES60_
    CMIDPictograph* iMIDPictograph;
#endif

    CLcdSurface*            iSurface;
    TAcceleratedBitmapInfo  iBitmap;    // data owned by surface
    TBitmapLockCount        iCount;
    CBitmapContext*         iContext;   // owned by surface

    TRect                   iDeviceBounds;      // surface drawable area in device coords
    TRect                   iDeviceClipRect;    // clip rect in device coords
    TPoint                  iDeviceOrigin;      // origin in device coords.

    TInt                iState;
    TStrokeStyle        iStrokeStyle;
    TUint32             iColor;
    CFont*              iFont;
    TFontUnderline      iUnderline;
    TRect               iClipRect;
    TPoint              iOrigin;
    TInt                iHeight;

    // this flag indicates that rendering target is framebuffer of CanavsGraphicsItem
    TBool  iIsCanvasGraphicsItemTarget;
};

/////////////////////////////////////////////////////////////
// CMIDPictograph is for Pictograpgh in S60
//
////////////////////////////////////////////////////////////
#ifdef __SERIES60_

class CMIDPictograph : public CBase, public MAknPictographAnimatorCallBack
{
public:
    static CMIDPictograph* NewL();

    CAknPictographInterface* PictographInterface();

    // From MAknPictographAnimatorCallBack
    void DrawPictographArea();

    virtual ~CMIDPictograph();
protected:
    CMIDPictograph();
    void ConstructL();

private:
    CAknPictographInterface* iPictographInterface;
};

#endif // end of __SERIES60_

#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// CLcdGraphics::CurrentClipRectInTargetCoords
// ---------------------------------------------------------------------------
//
inline TRect CLcdGraphics::CurrentClipRectInTargetCoords() const
{
    return iDeviceClipRect;
}

// ---------------------------------------------------------------------------
// CLcdGraphics::CurrentOriginInTargetCoords
// ---------------------------------------------------------------------------
//
inline TPoint CLcdGraphics::CurrentOriginInTargetCoords() const
{
    return iDeviceOrigin;
}

// ---------------------------------------------------------------------------
// CLcdGraphics::CurrentFont
// ---------------------------------------------------------------------------
//
inline CFont* CLcdGraphics::CurrentFont() const
{
    return iFont;
}

#endif // RD_JAVA_NGA_ENABLED

#endif // LCDGRAPHICS_H

