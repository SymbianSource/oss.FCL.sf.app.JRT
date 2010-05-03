/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTLINK_H
#define SWTLINK_H

#include "swtcontrolbase.h"

class MTouchFeedback;
class CSwtLinkFragmentDescriptor;
class CSwtDrawableLinkFragment;

/**
 *  The class encapsulating the S60 native implementation of Link.
 *
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtLink)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtLink
{
// New functions
public:
    /**
     * The constructor.
     * @param aDisplay The Display object
     * @param aPeer    The Java peer handle
     * @param aParent  The parent Composite
     * @param aStyle   Styles passed to the Java constructor (not the format)
     * @return         New CSwtLink instance
     */
    static CSwtLink* NewL(MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle);
    /**
     * The destructor
     */
    virtual ~CSwtLink();


private:
    CSwtLink(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent,
             TInt aStyle);
    void ConstructL();
    void SwtHandleResourceChangeL(TInt aType);
    void UpdateSkinColor();
    void UpdateDefaultFontL();
    void UpdateMarginValues();
    TRect TextRect() const;
    TInt BuildFragmentListL(const CDesCArray* aTextFragments,
                            const CDesCArray* aLinkTargets);
    void AssembleLinkTextL(const CDesCArray* aTextFragments,
                           TInt aTextLength);
    void BuildDrawableFragmentsListL(const TRect& aRect);
    void WrapTextL(const CFont& aFont, const TRect& aRect,
                   CArrayFixFlat<TPtrC>& aWrappedArray);
    TSize ComputeTextSizeL(TInt aWHint, TInt aHHint);
    TInt CalcWrappedTextHeightL(TInt aWidth);
    TInt CalcWrappedTextWidth(TInt aHeight);
    TSize MinimumSize() const;
    TInt MinimumHeight() const;
    TInt MinimumHeight(TInt aLineCount) const;
    TInt MinimumWidth() const;
    CGraphicsContext::TTextAlign GetAligment() const;
    TInt GetLineXOffset(const CGraphicsContext::TTextAlign aAligment,
                        const TInt aLineWidth, const TInt aAvailableWidth) const;
    TInt GetLineYOffset(TInt aLineCount,
                        const TRect& aRect) const;
    CFont::TMeasureTextInput::TFlags GetVisualOrder() const;
    void DoSetFont(const CFont* aFont);
    const CSwtLinkFragmentDescriptor* FindFragmentOnPosition(
        const TPoint& aPoint) const;
    const CSwtLinkFragmentDescriptor* FindFirstActiveFragment() const;
    const CSwtLinkFragmentDescriptor* FindNextActiveFragment(
        const CSwtLinkFragmentDescriptor* aFragmentDescriptor) const;
    const CSwtLinkFragmentDescriptor* FindLastActiveFragment() const;
    const CSwtLinkFragmentDescriptor* FindPreviousActiveFragment(
        const CSwtLinkFragmentDescriptor* aFragmentDescriptor) const;
    const CSwtLinkFragmentDescriptor* FindActiveFragment(
        const CSwtLinkFragmentDescriptor* aFragmentDescriptor,
        const TInt aFrom, TInt aTo) const;
    const CSwtLinkFragmentDescriptor* FragmentDescOrNullIfNotVisible(
        const CSwtLinkFragmentDescriptor* aFragmentDescriptor) const;
    TInt DoTrailingWhitespaceCorrection(
        const TInt aCurrentTextPosition) const;
    TRect GetDrawableFragmentRectangle(const CFont& aFont,
           const TDesC& aText, const TInt aLinePosition, const TInt aLength,
           const TInt aAvailableWidth, const TInt aXOffset, const TInt aYOffset,
           const TInt aCurrentLineIndex) const;
    void SetFocusedFragment();
    TBool IsRtl() const;


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
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
public:
    void SetForegroundL(const MSwtColor* aColor);
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void  SetFontL(const MSwtFont* aFont);
    TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);

// From base class ASwtControlBase
protected:
    const MSwtFont& DefaultFont() const;
    HBufC* MSKLabelL() const;
    TBool IsKeyUsed(TUint aKeyCode) const;

// From base class MSwtLink
public:
    MSwtControl* Control();
    void SetLinkDataL(const CDesCArray* aTextFragments,
                      const CDesCArray* aLinkTargets);

// Data
private:
    /**
     * Plain link text.
     */
    RBuf iText;

    /**
     * Formatted text used for drawing text.
     */
    RBuf iFormattedText;

    /**
     * Number of text lines
     */
    TInt iTextLineCount;

    /**
     * The default font.
     * Own.
     */
    MSwtFont* iDefaultFont;

    /**
     * Gap between two lines of text
     */
    TInt iLineGap;

    /**
     * Line height in pixels
     */
    TInt iLineHeight;

    /**
     * Plain text fragment text color.
     */
    TRgb iTextColor;

    /**
     * Link fragment text color.
     */
    TRgb iLinkColor;
    
    /**
     * Link fragment text color when highlighted.
     */
    TRgb iHighlightedLinkColor;

    /**
     * Link fragment back highlight color.
     */
    TRgb iHighlightColor;

    /**
     * True if currently used text color is a color set by the Java application.
     */
    TBool iCustomTextColor;

    /**
     * The margins used for text positioning when drawing.
     */
    TInt iHorizontalMargin;
    TInt iVerticalMargin;

    /**
     * Feedback instance.
     * Not own.
     */
    MTouchFeedback* iFeedback;

    /**
     * Collection of CLinkFragmentDescriptors
     */
    RPointerArray<CSwtLinkFragmentDescriptor> iLinkFragmentsDescriptors;

    /**
     * Collection of drawable link elements
     */
    RPointerArray<CSwtDrawableLinkFragment> iDrawableLinkFragments;

    /**
     * Points to link fragment which is currently focused
     * Own.
     */
    const CSwtLinkFragmentDescriptor* iFocusedFragment;
    
    /**
     * True for as long as pressed by pointer.
     * Eclipses ASwtControlBase::iPressed!
     */
    TBool iPressed;
    
    /**
     * True if 2 or more targets.
     */
    TBool iMultipleTargets;
};

#endif // SWTLINK_H



