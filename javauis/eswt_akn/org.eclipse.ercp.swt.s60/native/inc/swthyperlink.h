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
// SwtHyperLink.h


#ifndef SWTHYPERLINK_H
#define SWTHYPERLINK_H


#include "eswtmobileextensions.h"
#include "swtcontrolbase.h"


class MTouchFeedback;


/**
 *  The class encapsulating the S60 native implementation of HyperLink.
 *
 *  It is a custom control that is a focusable text-only label which,
 *  when being selected, launches an external handler application according
 *  to the format construction parameter.
 *
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtHyperLink)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtHyperLink
{

// New functions
public:
    /**
     * The constructor.  Note that format parameter is passed in by calling SetFormat
     * right after the construction.
     * @param aDisplay The Display object
     * @param aPeer    The Java peer handle
     * @param aParent  The parent Composite
     * @param aStyle   Styles passed to the Java constructor (not the format)
     * @return         New CSwtHyperLink instance
     */
    static CSwtHyperLink* NewL(MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle);
    /**
     * The destructor
     */
    virtual ~CSwtHyperLink();

private:
    CSwtHyperLink(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle);
    void ConstructL();
    void SwtHandleResourceChangeL(TInt aType);
    void DrawText(CWindowGc& aGc,
                  const CFont* aFont,
                  const TDesC& aText,
                  const TRect& aTextRect,
                  const CGraphicsContext::TTextAlign& aAlignment) const;
    void UpdateDefaultFontL();
    void LaunchHandler();
    TPtrC Scheme();
    TRgb LinkColor() const;
    TRgb LinkBgColor() const;
    TRgb PressedLinkColor() const;

// From base class CCoeControl

public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void HandleResourceChange(TInt aType);
protected:
    TTypeUid::Ptr   MopSupplyObject(TTypeUid aId);
    void SizeChanged();
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);
private:
    void Draw(const TRect& aRect) const;

// From base class MSwtControl

protected:
    CCoeControl&       CoeControl();
    const CCoeControl& CoeControl() const;
    void               ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void               HandlePointerEventL(const TPointerEvent& aPointerEvent);
public:
    void               SetForegroundL(const MSwtColor* aColor);
    TSize              ComputeSizeL(TInt aWHint, TInt aHHint);
    TInt               PressBackgroundPolicy() const;

// From base class ASwtControlBase

protected:
    const MSwtFont& DefaultFont() const;
    HBufC* MSKLabelL() const;
    TBool IsKeyUsed(TUint aKeyCode) const;

// From base class MSwtHyperLink

public:
    MSwtControl*    Control();
    void            SetTextL(const TDesC& aText);
    const TDesC*    GetText();
    void            SetFormat(TInt aFormat);

// Data
private:
    /**
     * The hyperlink text set by HyperLink.setText in the Java API.
     * This is returned from getText.
     * Own.
     */
    RBuf iOriginalText;

    /**
     * The hyperlink text that has been formatted.
     * Own.
     */
    RBuf iFormattedText;

    /**
     * The default font.
     * Own.
     */
    MSwtFont* iDefaultFont;

    /**
     * Custom foreground color used by the non pressed link.
     * Not own.
     */
    const MSwtColor* iCustomFg;

    /**
     * The format of the hyperlink. Textual formatting done and the handler
     * to launch depends on this.
     */
    MSwtHyperLink::TFormat iFormat;

    /**
     * Feedback instance.
     * Not own.
     */
    MTouchFeedback* iFeedback;
};


#endif // SWTHYPERLINK_H



