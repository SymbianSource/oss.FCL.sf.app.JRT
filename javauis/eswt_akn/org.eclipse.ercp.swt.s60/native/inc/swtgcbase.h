/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTGCBASE_H
#define SWTGCBASE_H


#include "eswtgraphics.h"
#include "eswtwidgetscore.h"


class CGlobalText;
class CTextLayout;

/**
 * Extension of CFbsBitmap class for M2G support.
 * Needed to delete Mbitmap from server - deletion has to be done on eSWT
 * side since M2G does not know, when the bitmap is drawn.
 */
class CFbsBitmapESWT : public CFbsBitmap
{
public:
    void SetHandles(TInt aHandle, TInt aServerHandle);
};

/**
 * Base class to be used for implementing GCs.
 *
 * CSwtGcBase implements whatever methods of MSwtGc
 * can be implemented using only CBitmapContext, other methods
 * are left to be implemented by derived classes. This makes CSwtGcBase
 * an abstract class.
 *
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtGcBase)
        : public CBase
        , public MSwtGc
        , public MFormCustomDraw
{
// Own Methods
protected:
    CSwtGcBase(MSwtDrawable& aDrawable, CBitmapContext* aGc, TDestructor aDestructor,
    const TRgb& aPenCol, const TRgb& aBrushCol, const MSwtFont& aFont,
    const TRect& aBoundaryRect, const TSize& aSize);
    ~CSwtGcBase();
    void ConstructL();

private:
    void DrawRectangleWorkaround(const TRect& aRect);
    void DoDrawArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle);
    void DrawResizedAlphaImageL(const CFbsBitmap& aBitmap,
                                const CFbsBitmap& aMask, const TRect& aSrceRect, const TRect& aDestRect);
    void UseNullPen();
    void UseSolidPen();
    void RestorePen();
    void UseThinPen();
    void RestorePenWidth();
    void UseNullBrush();
    void RestoreBrush();
    void RestoreClipping();
    inline void NotifyUpdate();
    void RemoveMnemonics(TDes& aText);
    TBidiText* CreateTextLayoutLC(TDes& aText,
                                  TInt aFlags, TInt& aBaseLineSpace, TSize& aViewSize) const;
    static inline TRect InclusiveRect(const TRect& aRect);
    static TPoint AngleToPosition(TInt aAngle, const TSize& aSize);
    static void   ComputeArc(const TRect& aRect,
                             TInt aStartAngle, TInt aArcAngle, TPoint& aStart, TPoint& aEnd);
    static TSize  ScaleSize(const TSize& aSize,
                            const TSize& aMultiplier, const TSize& aDivider);
    static CFbsBitmap* ScaleBitmapPortionLC(
        const CFbsBitmap& aBitmap, const TRect& aRect, const TSize& aNewSize);

// From MSwtGc
public:
    void  Dispose();
    CBitmapContext& BitmapContext();
    void  SetAlpha(TInt aAlpha);
    void  SetBackground(const MSwtColor& aColor);
    void  SetForeground(const MSwtColor& aColor);
    TSwtLineStyle LineStyle() const;
    void  SetLineStyleL(TSwtLineStyle aLineStyle);
    TInt  LineWidth() const;
    void  SetLineWidth(TInt aLineWidth);
    const MSwtFont* Font() const;
    void  SetFont(const MSwtFont* aFont);
    TBool XORMode() const;
    void  SetXORMode(TBool aXor);
    void  CopyArea(const TRect& aSource, const TPoint& aDestination);
#ifdef RD_JAVA_NGA_ENABLED
    TInt FinishOperationL(void);
#endif // RD_JAVA_NGA_ENABLED
    void  DrawM2GImageL(const TInt* aBitmapHandles, const TPoint& aSrcePos,
                        const TSize& aSrceSize, const TPoint& aDestPos, const TSize& aDestSize,
                        const TBool aUseNativeClear);
    void  DrawImage(const MSwtImage& aImage, const TPoint& aSrcePos,
                    const TSize& aSrceSize, const TPoint& aDestPos, const TSize& aDestSize);
    void  DrawImageFromBitmaps(const CFbsBitmap& aBitmap, const CFbsBitmap* aMask,
                               const TPoint& aSrcePos, const TSize& aSrceSize, const TPoint& aDestPos,
                               const TSize& aDestSize, const TBool aUseNativeClear);
    void  DrawPoint(const TPoint& aPoint);
    void  DrawLine(const TPoint& aPoint1, const TPoint& aPoint2);
    void  DrawPolyline(const TPoint* aPoints, TInt aCount);
    void  DrawPolygon(const TPoint* aPoints, TInt aCount);
    void  FillPolygon(const TPoint* aPoints, TInt aCount);
    void  DrawRectangle(const TRect& aRect);
    void  FillRectangle(const TRect& aRect);
    void  FillGradientRectangle(const TRect& aRect, TBool aVertical, TBool aInvertGradient);
    void  DrawRoundRectangle(const TRect& aRect, const TSize& aArcSize);
    void  FillRoundRectangle(const TRect& aRect, const TSize& aArcSize);
    void  DrawOval(const TRect& aRect);
    void  FillOval(const TRect& aRect);
    void  DrawArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle);
    void  FillArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle);
    TInt  GetCharWidth(const TChar& aChar) const;
    TInt  GetAdvanceWidth(const TChar& aChar) const;
    void  GetFontMetrics(TSwtFontMetrics& aMetrics) const;
    TSize TextExtentL(TDes& aText, TInt aFlags) const;
    void  DrawTextL(TDes& aText, const TPoint& aPosition, TInt aFlags);
    TBool IsClipped() const;
    TRect ClippingRect() const;
    void  SetClippingRect(const TRect& aRect);
    void  CancelClipping();
    void  DrawFocusRect(const TRect& aRect, const MSwtUiUtils& aUtils);
    void  SetBrushRgbValue(const TRgb& aRgb);
    void  SetPenRgbValue(const TRgb& aRgb);

// From MFormCustomDraw:
private:
    void DrawBackground(const TParam& aParam,const TRgb& aBackground,TRect& aDrawn) const;

// Data
protected:
    MSwtDrawable&   iDrawable;     // The drawable that created the native GC
    CBitmapContext* iGc;           // The native GC used, owned, cannot be NULL

private:
    const TDestructor             iDestructor;        // iGc's destructor function, cannot be NULL
    CGraphicsContext::TPenStyle   iCurrentPenStyle;   // The currently set pen style
    CGraphicsContext::TPenStyle   iUserPenStyle;      // The user-asked pen style
    TSwtLineStyle                 iLineStyle;
    TInt                          iCurrentPenWidth;   // The currently set pen width
    TInt                          iUserPenWidth;      // The user-asked pen width
    TRgb                          iPenColor;
    CGraphicsContext::TBrushStyle iCurrentBrushStyle; // The currently set brush style
    CGraphicsContext::TBrushStyle iUserBrushStyle;    // The user-asked brush style
    TRgb                          iBrushColor;
    CGraphicsContext::TDrawMode   iDrawMode;
    const MSwtFont&               iDefaultFont;       // The default font to use, ref counted
    const MSwtFont*               iFont;              // The font used, cannot be NULL, ref counted
    const TRect                   iBoundaryRect;      // The rectangle you cannot draw outside of
    const TSize                   iSize;              // The GC's size
    TRect                         iClippingRect;      // The user specified clipping rect
    TInt                          iAlpha;             // The alpha value
};

/**
 * Returns a rectangle suitable for drawing the bottom & right edges of
 * the specified rectangle.
 */
inline TRect CSwtGcBase::InclusiveRect(const TRect& aRect)
{
    return TRect(aRect.iTl, TPoint(aRect.iBr.iX+1, aRect.iBr.iY+1));
}

/**
 * Notifies the Drawable that its content may have been updated.
 */
inline void CSwtGcBase::NotifyUpdate()
{
    iDrawable.HandleUpdate();
}


#endif // SWTGCBASE_H
