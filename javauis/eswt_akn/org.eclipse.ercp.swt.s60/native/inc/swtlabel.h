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


#ifndef SWTLABEL_H
#define SWTLABEL_H


#include <eiklabel.h>
#include <eikspace.h>
#include "swtcontrolbase.h"
#include "eswtwidgetscore.h"


/**
 * CSwtLabel
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtLabel)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtLabel
{
// New methods
public:
    static CSwtLabel* NewL(MSwtDisplay& aDisplay,
    TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle);

protected:
    CSwtLabel(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle);
    ~CSwtLabel();
    void ConstructL();

private:
    void  CreateImageL();
    void  CreateTextL();
    void  CreateSeparatorL();
    void  ClipTextL(TInt aWHint);
    TSize ComputeTextSizeL(TInt aWHint, TInt aHHint);
    TInt  CalcWrappedTextHeightL(TInt aWidth);
    TInt  CalcWrappedTextWidth(TInt aHeight);
    void  WrapTextL(TInt aWHint, TInt aHHint);
    void  WrapTextL(TSize aSize);
    void  RetrieveDefaultFontL();
    TSize MinimumSize() const;
    TInt  MinimumWidth() const;
    TInt  MinimumHeight() const;
    TInt  MinimumHeight(TInt aLineCount) const;
    void  DoSetFontL(const CFont* aFont);
    void  SwtHandleResourceChangeL(TInt aType);

// From CCoeControl
public:
    void  SetDimmed(TBool aDimmed);
    CCoeControl* ComponentControl(TInt aIndex) const;
    TInt  CountComponentControls() const;
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void  HandleResourceChange(TInt aType);
protected:
    void  Draw(const TRect& aRect) const;
    void  FocusChanged(TDrawNow aDrawNow);
    void  PositionChanged();
    void  SizeChanged();
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    void  DrawSeparator(CWindowGc& aGc) const;
    TRect CalcAndUpdateSeparatorRect();
    void  ResizeSeparatorImages();

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl&  CoeControl() const;
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;
    void  ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void  SetForegroundL(const MSwtColor* aColor);
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void  SetFontL(const MSwtFont* aFont);

// From ASwtControlbase
protected:
    const MSwtFont& DefaultFont() const;

// From MSwtLabel
public:
    MSwtControl* Control();
    TInt GetAlignment() const;
    void SetAlignment(TInt aAlignment);
    void SetImageL(const MSwtImage* aImage);
    void SetTextL(const TDesC& aText);

// Data
private:
    CFbsBitmap*      iSeparatorHead;
    CFbsBitmap*      iSeparatorHeadMask;
    CFbsBitmap*      iSeparatorCenter;
    CFbsBitmap*      iSeparatorCenterMask;
    CFbsBitmap*      iSeparatorTail;
    CFbsBitmap*      iSeparatorTailMask;
    TRect            iSeparatorHeadRect;
    TRect            iSeparatorCenterRect;
    TRect            iSeparatorTailRect;
    TInt             iSeparatorThickness;
    CEikLabel*       iEikLabel;             // Symbian label, owned, may be NULL
    HBufC*           iOriginalText;         // The original text set with the method SetText().
    TInt             iOriginalTextLineCount;// Count how many lines are in the text ('\n')
    MSwtFont*        iDefaultFont;          // The text's default font, owned, NULL only if iText is NULL
    const MSwtImage* iImage;                // The image to draw, may be NULL, ref counted
    const MSwtColor* iForegroundColor;      // The set foreground color
    TBool            iDashedLineSeparatorInUse;
};


#endif // SWTLABEL_H
