/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDGC_H
#define CMIDGC_H

#include <gdi.h>

//
// This is necessary for CCoeControl's to draw in CPicture's
//

class CMIDGc : public CWindowGc
{
public:
    inline CMIDGc(CWsScreenDevice* aSd);
    //
    // From CWindowGc
    //
    inline void Activate(RDrawableWindow& aWindow);
    inline void Deactivate();
    inline CGraphicsDevice* Device() const;
    inline void SetOrigin(const TPoint& aPos=TPoint(0,0));
    inline void SetDrawMode(TDrawMode aDrawingMode);
    inline void SetClippingRect(const TRect& aRect);
    inline void CancelClippingRect();
    inline void Reset();
    inline void UseFont(const CFont* aFont);
    inline void DiscardFont();
    inline void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
    inline void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
    inline void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
    inline void SetCharJustification(TInt aExcessWidth,TInt aNumChars);
    inline void SetPenColor(const TRgb& aColor);
    inline void SetPenStyle(TPenStyle aPenStyle);
    inline void SetPenSize(const TSize& aSize);
    inline void SetBrushColor(const TRgb& aColor);
    inline void SetBrushStyle(TBrushStyle aBrushStyle);
    inline void SetBrushOrigin(const TPoint& aOrigin);
    inline void UseBrushPattern(const CWsBitmap* aBitmap);
    inline void UseBrushPattern(const CFbsBitmap* aBitmap);
    inline void DiscardBrushPattern();
    inline void MoveTo(const TPoint& aPoint);
    inline void MoveBy(const TPoint& aVector);
    inline void Plot(const TPoint& aPoint);
    inline void DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
    inline void DrawLine(const TPoint& aPoint1,const TPoint& aPoint2);
    inline void DrawLineTo(const TPoint& aPoint);
    inline void DrawLineBy(const TPoint& aVector);
    inline void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
    inline void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
    inline void DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
    inline void DrawEllipse(const TRect& aRect);
    inline void DrawRect(const TRect& aRect);
    inline void DrawRoundRect(const TRect& aRect,const TSize& aEllipse);
    inline TInt DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule=EAlternate);
    inline TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule=EAlternate);
    inline void DrawBitmap(const TPoint& aTopLeft,const CWsBitmap* aSource);
    inline void DrawBitmap(const TRect& aDestRect,const CWsBitmap* aSource);
    inline void DrawBitmap(const TRect& aDestRect,const CWsBitmap* aSource,TRect& aSourceRect);
    inline void DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource);
    inline void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource);
    inline void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,TRect& aSourceRect);
    inline void DrawText(const TDesC& aString,const TPoint& aPosition);
    inline void DrawText(const TDesC& aString,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);
    inline void Clear();
    inline void Clear(const TRect& aRect);
    inline void CopyRect(const TPoint& aOffset,const TRect& aRect);
    inline void BitBlt(const TPoint& aPos,const CWsBitmap* aDevice);
    inline void BitBlt(const TPoint& aPos,const CWsBitmap* aDevice,const TRect& aRect);
    inline void BitBltMasked(const TPoint& aPoint,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask);
    inline void BitBlt(const TPoint& aPos,const CFbsBitmap* aDevice);
    inline void BitBlt(const TPoint& aPos,const CFbsBitmap* aDevice,const TRect& aRect);
    inline void BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
    inline void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs=2,TBool aMapForwards=ETrue);
    inline void DrawTextVertical(const TDesC& aText,const TPoint& aPosition,TBool aUp);
    inline void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
    inline void SetDitherOrigin(const TPoint& aPoint);
    inline TInt SetClippingRegion(const TRegion& aClippingRegion);
    inline void CancelClippingRegion();
    inline void SetGcOrigin(TPoint aOrigin);
    inline void SetGraphicsContext(CBitmapContext* aGc);
private:
    inline void ResetOrigin();
private:
    CBitmapContext* iGc;
    TPoint iGcOrigin;
};


inline CMIDGc::CMIDGc(CWsScreenDevice* aSd)
        : CWindowGc(aSd)
{}

inline void CMIDGc::Activate(RDrawableWindow& /*aWindow*/)
{}

inline void CMIDGc::Deactivate()
{}

inline CGraphicsDevice* CMIDGc::Device() const
{
    return iGc->Device();
}

inline void CMIDGc::SetOrigin(const TPoint& aPos)
{
    iGc->SetOrigin(aPos);
}

inline void CMIDGc::SetDrawMode(TDrawMode aDrawingMode)
{
    iGc->SetDrawMode(aDrawingMode);
}

inline void CMIDGc::SetClippingRect(const TRect& aRect)
{
    iGc->SetClippingRect(aRect);
}

inline void CMIDGc::CancelClippingRect()
{
    iGc->CancelClippingRect();
}

inline void CMIDGc::Reset()
{
    iGc->Reset();
}

inline void CMIDGc::UseFont(const CFont* aFont)
{
    iGc->UseFont(aFont);
}

inline void CMIDGc::DiscardFont()
{
    iGc->DiscardFont();
}

inline void CMIDGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
{
    iGc->SetUnderlineStyle(aUnderlineStyle);
}

inline void CMIDGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
{
    iGc->SetStrikethroughStyle(aStrikethroughStyle);
}

inline void CMIDGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
{
    iGc->SetWordJustification(aExcessWidth,aNumGaps);
}

inline void CMIDGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
{
    iGc->SetCharJustification(aExcessWidth,aNumChars);
}

inline void CMIDGc::SetPenColor(const TRgb& aColor)
{
    iGc->SetPenColor(aColor);
}

inline void CMIDGc::SetPenStyle(TPenStyle aPenStyle)
{
    iGc->SetPenStyle(aPenStyle);
}

inline void CMIDGc::SetPenSize(const TSize& aSize)
{
    iGc->SetPenSize(aSize);
}

inline void CMIDGc::SetBrushColor(const TRgb& aColor)
{
    iGc->SetBrushColor(aColor);
}

inline void CMIDGc::SetBrushStyle(TBrushStyle aBrushStyle)
{
    iGc->SetBrushStyle(aBrushStyle);
}

inline void CMIDGc::SetBrushOrigin(const TPoint& aOrigin)
{
    iGc->SetBrushOrigin(aOrigin);
}

inline void CMIDGc::UseBrushPattern(const CWsBitmap* aBitmap)
{
    iGc->UseBrushPattern(aBitmap);
}

inline void CMIDGc::UseBrushPattern(const CFbsBitmap* aBitmap)
{
    iGc->UseBrushPattern(aBitmap);
}

inline void CMIDGc::DiscardBrushPattern()
{
    iGc->DiscardBrushPattern();
}

inline void CMIDGc::MoveTo(const TPoint& aPoint)
{
    iGc->MoveTo(aPoint);
}

inline void CMIDGc::MoveBy(const TPoint& aVector)
{
    iGc->MoveBy(aVector);
}

inline void CMIDGc::Plot(const TPoint& aPoint)
{
    iGc->Plot(aPoint);
}

inline void CMIDGc::DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
{
    iGc->DrawArc(aRect,aStart,aEnd);
}

inline void CMIDGc::DrawLine(const TPoint& aPoint1,const TPoint& aPoint2)
{
    iGc->DrawLine(aPoint1,aPoint2);
}

inline void CMIDGc::DrawLineTo(const TPoint& aPoint)
{
    iGc->DrawLineTo(aPoint);
}

inline void CMIDGc::DrawLineBy(const TPoint& aVector)
{
    iGc->DrawLineBy(aVector);
}

inline void CMIDGc::DrawPolyLine(const CArrayFix<TPoint>* aPointList)
{
    iGc->DrawPolyLine(aPointList);
}

inline void CMIDGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
{
    iGc->DrawPolyLine(aPointList,aNumPoints);
}

inline void CMIDGc::DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
{
    iGc->DrawPie(aRect,aStart,aEnd);
}

inline void CMIDGc::DrawEllipse(const TRect& aRect)
{
    iGc->DrawEllipse(aRect);
}

inline void CMIDGc::DrawRect(const TRect& aRect)
{
    iGc->DrawRect(aRect);
}

inline void CMIDGc::DrawRoundRect(const TRect& aRect,const TSize& aEllipse)
{
    iGc->DrawRoundRect(aRect,aEllipse);
}

inline TInt CMIDGc::DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule)
{
    return iGc->DrawPolygon(aPointList,aFillRule);
}

inline TInt CMIDGc::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
{
    return iGc->DrawPolygon(aPointList,aNumPoints,aFillRule);
}

inline void CMIDGc::DrawBitmap(const TPoint& aTopLeft,const CWsBitmap* aSource)
{
    iGc->DrawBitmap(aTopLeft,aSource);
}

inline void CMIDGc::DrawBitmap(const TRect& aDestRect,const CWsBitmap* aSource)
{
    iGc->DrawBitmap(aDestRect,aSource);
}

inline void CMIDGc::DrawBitmap(const TRect& aDestRect,const CWsBitmap* aSource,TRect& aSourceRect)
{
    iGc->DrawBitmap(aDestRect,aSource,aSourceRect);
}

inline void CMIDGc::DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource)
{
    iGc->DrawBitmap(aTopLeft,aSource);
}

inline void CMIDGc::DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource)
{
    iGc->DrawBitmap(aDestRect,aSource);
}

inline void CMIDGc::DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,TRect& aSourceRect)
{
    iGc->DrawBitmap(aDestRect,aSource,aSourceRect);
}

inline void CMIDGc::DrawText(const TDesC& aString,const TPoint& aPosition)
{
    iGc->DrawText(aString,aPosition);
}

inline void CMIDGc::DrawText(const TDesC& aString,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHoriz,TInt aLeftMrg)
{
    iGc->DrawText(aString,aBox,aBaselineOffset,aHoriz,aLeftMrg);
}

inline void CMIDGc::Clear()
{
    iGc->Clear();
}

inline void CMIDGc::Clear(const TRect& aRect)
{
    iGc->Clear(aRect);
}

inline void CMIDGc::CopyRect(const TPoint& aOffset,const TRect& aRect)
{
    iGc->CopyRect(aOffset,aRect);
}

inline void CMIDGc::BitBlt(const TPoint& aPos,const CWsBitmap* aBitmap)
{
    iGc->BitBlt(aPos,aBitmap);
}

inline void CMIDGc::BitBlt(const TPoint& aPos,const CWsBitmap* aDevice,const TRect& aRect)
{
    iGc->BitBlt(aPos,aDevice,aRect);
}

inline void CMIDGc::BitBltMasked(const TPoint& aPoint,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask)
{
    iGc->BitBltMasked(aPoint,aBitmap,aSourceRect,aMaskBitmap,aInvertMask);
}

inline void CMIDGc::BitBlt(const TPoint& aPos,const CFbsBitmap* aBitmap)
{
    iGc->BitBlt(aPos,aBitmap);
}

inline void CMIDGc::BitBlt(const TPoint& aPos,const CFbsBitmap* aDevice,const TRect& aRect)
{
    iGc->BitBlt(aPos,aDevice,aRect);
}

inline void CMIDGc::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)
{
    iGc->BitBltMasked(aPoint,aBitmap,aSourceRect,aMaskBitmap,aInvertMask) ;
}

inline void CMIDGc::MapColors(const TRect& /*aRect*/,const TRgb* /*aColors*/,TInt /*aNumPairs*/,TBool /*aMapForwards*/)
{}

inline void CMIDGc::DrawTextVertical(const TDesC& /*aText*/,const TPoint& /*aPosition*/,TBool /*aUp*/)
{}

inline void CMIDGc::DrawTextVertical(const TDesC& /*aText*/,const TRect& /*aBox*/,TInt /*aBaselineOffset*/,TBool /*aUp*/,TTextAlign /*aVert*/,TInt /*aMargin*/)
{}

inline void CMIDGc::SetDitherOrigin(const TPoint& /*aPoint*/)
{}

inline TInt CMIDGc::SetClippingRegion(const TRegion& /*aClippingRegion*/)
{
    return 0;
}

inline void CMIDGc::CancelClippingRegion()
{}

inline void CMIDGc::SetGcOrigin(TPoint aOrigin)
{
    iGcOrigin = aOrigin;
    ResetOrigin();
}

inline void CMIDGc::SetGraphicsContext(CBitmapContext* aGc)
{
    iGc = aGc;
}

inline void CMIDGc::ResetOrigin()
{
    SetOrigin(iGcOrigin);
}

#endif // CMIDGC_H



