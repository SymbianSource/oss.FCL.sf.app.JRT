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


#include <swtlaffacade.h>
#include <swtbrowserschemehandler.h>
#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK
#include "swtfont.h"
#include "swtlink.h"

const TInt KBorderMargin = 1;

/**
 * Link fragment descriptor.
 * Holds information about unformatted link text fragments.
 */
NONSHARABLE_CLASS(CSwtLinkFragmentDescriptor)
        : public CBase
{
public:

    static CSwtLinkFragmentDescriptor* NewL(TInt aOffset, TInt aLength);
    static CSwtLinkFragmentDescriptor* NewL(TInt aOffset,
                                            TInt aLength, const TDesC& aTarget);

    TInt Offset() const;
    TInt Length() const;
    const TDesC* Target() const;

    virtual ~CSwtLinkFragmentDescriptor();

protected:
    CSwtLinkFragmentDescriptor(TInt aOffset, TInt aLength);
    void ConstructL(const TDesC& aTarget);

private:
    /**
     * Offset from beginning of the string
     */
    TInt iOffset;
    /**
     * Length of the fragment in characters
     */
    TInt iLength;
    /**
     * Fragment target. Either string returned by selection listener,
     * or NULL if fragment is non-active.
     * Own.
     */
    HBufC* iTarget;
};

/**
 * Drawable link fragment information container.
 * Holds information about formatted fragments created for drawing.
 */
NONSHARABLE_CLASS(CSwtDrawableLinkFragment)
        : public CBase
{
public:
    static CSwtDrawableLinkFragment* NewL(
        const TPtrC& aText,
        const TRect& aRect,
        const CFont& aFont,
        const CSwtLinkFragmentDescriptor* aFragmentDescriptor);

    virtual ~CSwtDrawableLinkFragment();

protected:
    CSwtDrawableLinkFragment(
        const TRect& aRect,
        const CSwtLinkFragmentDescriptor* aFragmentDescriptor);

    void ConstructL(const TPtrC& aText, const CFont& aFont);

public:
    const TDes& Text() const;
    const TRect& Rect() const;
    const CSwtLinkFragmentDescriptor* FragmentDescriptor() const;

private:
    /**
     * Rectangle into which fragment should be drawn.
     *
     */
    TRect iRect;
    /**
     * Pointer to fragment descriptor to which this instance belongs.
     * Not own.
     */
    const CSwtLinkFragmentDescriptor* iFragmentDescriptor;

    /**
     * Formatted text.
     */
    RBuf iFormattedText;
};



// ======== MEMBER FUNCTIONS ========

/**
 * Creates new instance of CSwtLink
 */
CSwtLink* CSwtLink::NewL(
    MSwtDisplay&   aDisplay,
    TSwtPeer       aPeer,
    MSwtComposite& aParent,
    TInt           aStyle)
{
    CSwtLink* self = new(ELeave) CSwtLink(aDisplay,
                                          aPeer,
                                          aParent,
                                          aStyle);

    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

/**
 * Destructor
 */
CSwtLink::~CSwtLink()
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif // RD_TACTILE_FEEDBACK
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
    iText.Close();
    iFormattedText.Close();
    iDrawableLinkFragments.ResetAndDestroy();
    iDrawableLinkFragments.Close();
    iLinkFragmentsDescriptors.ResetAndDestroy();
    iLinkFragmentsDescriptors.Close();
}


/**
 * Constructor
 */
CSwtLink::CSwtLink(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
        , iTextLineCount(0)
        , iLineGap(0)
        , iLineHeight(0)
{
    // Orientation needs to be coerced if it wasn't given explicitly
    if (!(aStyle & KSwtOrientationMask))
    {
        iStyle &= ~KSwtOrientationMask;
    }
}


/**
 * Constructs current instnace
 */
void CSwtLink::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();

    SetContainerWindowL(coeParent);
    CAknControl::MakeVisible(coeParent.IsVisible());
    CAknControl::SetDimmed(coeParent.IsDimmed());

    UpdateDefaultFontL();
    DoSetFont(&iDefaultFont->Font());
    UpdateMarginValues();
    UpdateSkinColor();
    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif // RD_TACTILE_FEEDBACK

    ActivateL();
}

/**
 * Handles resource change
 * @param aType Type of resource change.
 */
void CSwtLink::SwtHandleResourceChangeL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        UpdateDefaultFontL();
        DoSetFont(&iDefaultFont->Font());
        UpdateMarginValues();
        BuildDrawableFragmentsListL(TextRect());
    }
    else if (aType == KAknsMessageSkinChange)
    {
        if (!iCustomTextColor)
        {
            UpdateSkinColor();
        }
    }
}

/**
 * Updates skin colors
 */
void CSwtLink::UpdateSkinColor()
{
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iTextColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG6);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iLinkColor,
                              KAknsIIDQsnHighlightColors,
                              EAknsCIQsnHighlightColorsCG3);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iHighlightColor,
                              KAknsIIDQsnHighlightColors,
                              EAknsCIQsnHighlightColorsCG2);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iHighlightedLinkColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG24);
}


/**
 * Updates default font
 */
void CSwtLink::UpdateDefaultFontL()
{
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(),
                                  iDisplay.Device().GetSystemFont()->Font());
}

/**
 * Updates margins
 */
void CSwtLink::UpdateMarginValues()
{
    TMargins8 padding = iDisplay.UiUtils().InlineReferencePadding();
    iHorizontalMargin = padding.iLeft;
    iVerticalMargin = padding.iTop;
}

/**
 * Builds list of fragment descriptors from array of text fragments and
 * array of their targets.
 *
 * @param aTextFragments Array of text fragments
 * @param aLinkTargets Array of strings which should be passed to
 *                     selection listener.
 * @returns Sum of all text fragments legths (ie. expected plain text
 *          length).
 *
 * @remark When aLinkTargets item is empty string, the aTextFragments
 *         item is considered to be non-active fragment.
 *         So active fragments are only those which have non-empty
 *         string in aLinkTargets array.
 */
TInt CSwtLink::BuildFragmentListL(
    const CDesCArray* aTextFragments,
    const CDesCArray* aLinkTargets)
{
    TInt actualTextLength = 0;
    iLinkFragmentsDescriptors.ResetAndDestroy();
    TInt fragmentCount = aTextFragments->Count();
    TInt targetCount = 0;
    for (TInt i = 0; i < fragmentCount; ++i)
    {
        if ((*aLinkTargets)[i].Length())
        {
            iLinkFragmentsDescriptors.AppendL(
                CSwtLinkFragmentDescriptor::NewL(
                    actualTextLength,
                    (*aTextFragments)[i].Length(),
                    (*aLinkTargets)[i]));
            targetCount++;
        }
        else
        {
            iLinkFragmentsDescriptors.AppendL(
                CSwtLinkFragmentDescriptor::NewL(
                    actualTextLength,
                    (*aTextFragments)[i].Length()));
        }
        actualTextLength += (*aTextFragments)[i].Length();
    }

    if (targetCount > 1)
    {
        iMultipleTargets = ETrue;
    }
    else
    {
        iMultipleTargets = EFalse;
    }

    return actualTextLength;
}

/**
 * Assembles control's plain text from text fragments.
 *
 * @param aTextFragments Array of string which will be concatenated to form
 *                      control's plain text.
 * @param aTextLength Expected length of the resulting string.
 */
void CSwtLink::AssembleLinkTextL(
    const CDesCArray* aTextFragments,
    TInt aTextLength)
{
    iText.Close();
    iText.CreateL(aTextLength);
    TInt numberOfLines = 1;
    for (TInt i = 0; i < aTextFragments->Count(); ++i)
    {
        TPtrC fragment = (*aTextFragments)[i];
        iText.Append(fragment);
        const TInt offset = fragment.Locate('\n');
        if (KErrNotFound != offset)
        {
            numberOfLines++;
        }
    }
    iTextLineCount = numberOfLines;
}

/**
 * Builds drawable fragment list from control's text and given
 * rectangle.
 *
 */
void CSwtLink::BuildDrawableFragmentsListL(const TRect& aRect)
{
    iDrawableLinkFragments.ResetAndDestroy();
    if (!iText.Length() || aRect.IsEmpty())
    {
        return;
    }

    const CFont& font(GetFont()->Font());
    const CGraphicsContext::TTextAlign alignment = GetAligment();
    const CFont::TMeasureTextInput::TFlags order = GetVisualOrder();

    // Calculate the number of lines when wrapping text to the given rectangle
    const TInt ARRAY_GRANULARITY = 4;
    CArrayFixFlat<TPtrC>* linesArray =
        new(ELeave) CArrayFixFlat<TPtrC>(ARRAY_GRANULARITY);
    CleanupStack::PushL(linesArray);

    //Wrap text to lines
    WrapTextL(font, aRect, *linesArray);

    //If there are no lines or no formatted text, return
    if (!linesArray->Count() || !iFormattedText.Length())
    {
        return;
    }

    TInt currentLineIndex = 0;
    TInt currentLinePosition = 0;
    TInt currentTextPosition = 0;
    //Get Y offset so the text is centered vertically in the control.
    const TInt lineYOffset = GetLineYOffset(linesArray->Count(), aRect);
    //Here we will map fragment descriptors to wrapped text and create
    //drawable fragments which then will be drawn.
    for (TInt fragmentIndex = 0;
            fragmentIndex < iLinkFragmentsDescriptors.Count();
            ++fragmentIndex)
    {
        //get length of the text in current fragment descriptor
        TInt remainingFragmentLength =
            iLinkFragmentsDescriptors[fragmentIndex]->Length();
        while (remainingFragmentLength > 0 &&
                currentLineIndex < linesArray->Count())
        {
            TPtrC currentLine = linesArray->At(currentLineIndex);
            TInt remainingLineLength =
                currentLine.Length() - currentLinePosition;

            //Get width of the line in pixels and calculate X offset
            //for current aligment
            const TInt wholeLineWidth =
                AknBidiTextUtils::MeasureTextBoundsWidth(font,
                        currentLine, order);
            const TInt lineXOffset = GetLineXOffset(alignment,
                                                    wholeLineWidth, aRect.Width());

            //Determine how many characters will be used from
            //current text line
            TInt charsConsumed = remainingFragmentLength;
            if (remainingFragmentLength > remainingLineLength)
            {
                //Some chracters will be left for next text line
                charsConsumed = remainingLineLength;
            }

            //Calculate position to which current portion of text
            //should be drawn.
            TRect currentRect = GetDrawableFragmentRectangle(font,
                                currentLine, currentLinePosition,
                                charsConsumed, aRect.Width(),
                                lineXOffset, lineYOffset,
                                currentLineIndex);

            //Add new drawable fragment
            iDrawableLinkFragments.AppendL(
                CSwtDrawableLinkFragment::NewL(
                    currentLine.Mid(currentLinePosition, charsConsumed),
                    currentRect,
                    font,
                    iLinkFragmentsDescriptors[fragmentIndex]));

            if (remainingFragmentLength > remainingLineLength)
            {
                //Move to the next line
                ++currentLineIndex;
                currentTextPosition += remainingLineLength;
                currentLinePosition = 0;
                //Wrapping text strips trailing spces at the end of each line
                //Get correction for this.
                TInt correction = DoTrailingWhitespaceCorrection(
                                      currentTextPosition);
                currentTextPosition += correction;
                //Update number of characters remaining in current
                //fragment descriptor
                remainingFragmentLength -= charsConsumed + correction;
            }
            else
            {
                //Stay on current line
                currentTextPosition += charsConsumed;
                currentLinePosition += charsConsumed;
                remainingLineLength -= charsConsumed;
                remainingFragmentLength = 0;
            }
        }
    }

    linesArray->Reset();
    CleanupStack::PopAndDestroy(linesArray);
}

/**
 * Calculates rectangle for drawable fragment.
 * @param aFont Font.
 * @param aText Text to be drawn.
 * @param aLinePosition Position in iFormattedText.
 * @param aLength Length the text to be drawn.
 * @param aAvailableWidth Width available for drawing.
 * @param aXOffset X offset of the rectangle.
 * @param aYOffset Y offset of the rectangle.
 * @param aCurrentLineIndex Index of current line.
 * @returns Rectnagle into which the drawable fragment should be drawn.
 */
TRect CSwtLink::GetDrawableFragmentRectangle(const CFont& aFont,
        const TDesC& aText, const TInt aLinePosition,
        const TInt aLength, const TInt aAvailableWidth,
        const TInt aXOffset, const TInt aYOffset,
        const TInt aCurrentLineIndex) const
{
    CFont::TMeasureTextInput::TFlags order = GetVisualOrder();

    // Try to determine text directionality
    TBool found = EFalse;
    TInt directionality = TBidiText::TextDirectionality(aText, &found);
    if (found)
    {
        switch (directionality)
        {
        case TBidiText::ERightToLeft:
            order = CFont::TMeasureTextInput::EFVisualOrderRightToLeft;
            break;
        default:
            order = CFont::TMeasureTextInput::EFVisualOrder;
            break;
        }
    }

    const TInt leftX = AknBidiTextUtils::MeasureTextBoundsWidth(
                           aFont, aText.Left(aLinePosition), order);

    const TInt rightX = AknBidiTextUtils::MeasureTextBoundsWidth(
                            aFont, aText.Left(aLinePosition + aLength), order);

    TInt tlX = 0;
    TInt brX = 0;
    if (IsRtl())
    {
        tlX = aAvailableWidth - rightX;
        brX = aAvailableWidth - leftX;
    }
    else
    {
        tlX = aXOffset + leftX;
        brX = aXOffset + rightX;
    }

    TRect currentRect(
        tlX,
        aYOffset + aCurrentLineIndex *(iLineHeight + iLineGap),
        brX,
        aYOffset + (aCurrentLineIndex + 1) *(iLineHeight + iLineGap));

    return currentRect;
}

/**
 * Returns number of white spaces from the given position to
 * next non-white space character in formatted control's text.
 *
 * @param aCurrentTextPosition Current position in the formatted text
 * @returns Number of white spaces between aCurrentTextPosition and next
 *          non-white space character.
 * @remark This method is used to coerce position in iText while
 *          moving to the next line, because text wrapping strips
 *          trailing line spaces in the lines.
 */
TInt CSwtLink::DoTrailingWhitespaceCorrection(
    const TInt aCurrentTextPosition) const
{
    TInt correction = 0;
    const TInt textLen = iText.Length();
    if (aCurrentTextPosition < textLen)
    {
        TChar c(iText[aCurrentTextPosition]);
        while (c.IsSpace()
                && (aCurrentTextPosition + correction + 1) < textLen)
        {
            ++correction;

            // Stop on EOL to prevent overcounting for leading spaces
            if (c == '\n' || c == '\r')
            {
                break;
            }

            c = iText[aCurrentTextPosition + correction];
        }
    }

    return correction;
}

/**
 * Returns alignment depending on control's style and current layout
 * settings.
 */
CGraphicsContext::TTextAlign CSwtLink::GetAligment() const
{
    CGraphicsContext::TTextAlign alignment;

    if (iStyle & KSwtStyleCenter)
    {
        alignment = CGraphicsContext::ECenter;
    }
    else if (iStyle & KSwtStyleTrail)
    {
        if (IsRtl())
        {
            alignment = CGraphicsContext::ELeft;
        }
        else
        {
            alignment = CGraphicsContext::ERight;
        }
    }
    else // default is left
    {
        if (IsRtl())
        {
            alignment = CGraphicsContext::ERight;
        }
        else
        {
            alignment = CGraphicsContext::ELeft;
        }
    }

    return alignment;
}

/**
 * Calculates X offset for the drawn line so the text is on the right
 * position for given alignment.
 * @param aAlignment Alignment of the line.
 * @param aLineWidth Width of the text to be drawn in pixels.
 * @param aAvailableWidth Width available for the line in pixels.
 * @returns X offset
 */
TInt CSwtLink::GetLineXOffset(
    const CGraphicsContext::TTextAlign aAlignment,
    const TInt aLineWidth,
    const TInt aAvailableWidth) const
{
    TInt offset = 0;
    switch (aAlignment)
    {
    case CGraphicsContext::ELeft:
        //Do nothing, 0 is already there
        break;
    case CGraphicsContext::ERight:
        offset = aAvailableWidth - aLineWidth;
        break;
    case CGraphicsContext::ECenter:
        offset = (aAvailableWidth - aLineWidth) / 2;
        break;
    default:
        break;
    }

    return offset;
}

/**
 * Calculates Y offset for the first drawn line so the
 * text is vertically centered in the client area.
 * @param aLineCount Number of lines to draw.
 * @param aRect Rectangle into which text will be drawn.
 * @returns Y offset
 */
TInt CSwtLink::GetLineYOffset(TInt aLineCount,
                              const TRect& aRect) const
{
    const TInt offset = (aRect.Height()
                         - aLineCount * (GetFont()->Font().FontMaxHeight() + iLineGap)
                         + (aLineCount > 0 ? iLineGap : 0)) / 2;
    return offset;
}

/**
 * Returns current visual order
 */
CFont::TMeasureTextInput::TFlags CSwtLink::GetVisualOrder() const
{
    CFont::TMeasureTextInput::TFlags order =
        CFont::TMeasureTextInput::EFVisualOrder;
    if (IsRtl())
    {
        order = CFont::TMeasureTextInput::EFVisualOrderRightToLeft;
    }

    return order;
}


/**
 * Wraps control's text to fit in given rectangle.
 *
 * @param aRect Target rectangle
 * @param aWrappedArray Array of wrapped lines.
 * @remark If there is not enough space for the text to fit in the
 *          rectangle, last line is clipped.
 *         This method updates iFormattedText to contain clipped text.
 */
void CSwtLink::WrapTextL(
    const CFont& aFont,
    const TRect& aRect,
    CArrayFixFlat<TPtrC>& aWrappedArray)
{
    // The last row in label does not add a gap after it. So...
    // aHHint = rows * textPaneHeight + (rows - 1) * gap; and therefore...
    TInt rows = (aRect.Height()  + iLineGap) /
                (aFont.FontMaxHeight() + iLineGap);
    if (rows < 1)
    {
        // It happens when hint height < Char's height
        // here just set nBStrings to 1
        // so something can be visible.
        rows = 1;
    }

    // Setup width arrray
    CArrayFixFlat< TInt >* widthsArray =
        new(ELeave) CArrayFixFlat< TInt >(rows);
    CleanupStack::PushL(widthsArray);
    widthsArray->SetReserveL(rows);
    TInt i;
    for (i = 0; i < rows; ++i)
    {
        widthsArray->AppendL(aRect.Width());
    }

    // If formatted text doesn't have enough space we need to
    // resize it.
    if (iFormattedText.Length() < rows)
    {
        iFormattedText.Close();
        iFormattedText.CreateL(iText.Length() + rows);
    }

    // AknTextUtils::WrapToArrayAndClipL can change the string when
    // adding ellipsis, therefore we work with copy here.
    iFormattedText.Copy(iText);

    AknTextUtils::WrapToArrayAndClipL(iFormattedText, *widthsArray,
                                      aFont, aWrappedArray);
    widthsArray->Reset();
    CleanupStack::PopAndDestroy(widthsArray);
}

/**
 * Returns rectangle into which text should be drawn.
 */
TRect CSwtLink::TextRect() const
{
    TRect textRect = Rect();
    textRect.Shrink(iHorizontalMargin + KBorderMargin,
                    iVerticalMargin + KBorderMargin);
    return textRect;
}

/**
 * Finds fragment descriptor at given position.
 *
 * @param aPoint Point relative to control.
 * @returns Fragment descriptor drawn under aPoint.
 * If there is no fragment desriptor under the point,
 *      returns NULL.
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FindFragmentOnPosition(
    const TPoint& aPoint) const
{
    const CSwtLinkFragmentDescriptor* fragment = NULL;
    const TPoint offsetPoint(aPoint - TextRect().iTl);
    for (TInt i = 0; i < iDrawableLinkFragments.Count(); ++i)
    {
        CSwtDrawableLinkFragment* currentDrawableFragment =
            iDrawableLinkFragments[i];
        if (currentDrawableFragment->Rect().Contains(offsetPoint))
        {
            fragment = iDrawableLinkFragments[i]->FragmentDescriptor();
            break;
        }
    }
    return fragment;
}

/**
 * Finds first active fragment descriptor and verifies it is drawn.
 *
 * @returns First active fragment descriptor.
 * If there are no active fragments,
 *      returns NULL.
 * If first fragment descriptor is found, but it is not drawn,
 *      returns NULL.
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FindFirstActiveFragment() const
{
    return FindNextActiveFragment(NULL);
}

/**
 * Finds next active fragment descriptor and verifies it is drawn.
 *
 * @param aFragmentDescriptor Current fragment descriptor
 * @returns Next active fragment descriptor.
 * If aFragmentDescriptor is NULL,
 *      returns first active fragment descriptor.
 * If aFragmentDescriptor points to the last active fragment descriptor,
 *      returns NULL.
 * If next fragment descriptor is found, but it is not drawn,
 *      returns NULL.
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FindNextActiveFragment(
    const CSwtLinkFragmentDescriptor* aFragmentDescriptor) const
{
    return FindActiveFragment(aFragmentDescriptor,
                              0, iLinkFragmentsDescriptors.Count() - 1);
}

/**
 * Finds last active fragment descriptor and verifies it is drawn.
 *
 * @returns Last active fragment descriptor.
 * If there are no active fragments,
 *      returns NULL.
 * If last fragment descriptor is found, but it is not drawn,
 *      returns NULL.
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FindLastActiveFragment() const
{
    return FindPreviousActiveFragment(NULL);
}

/**
 * Finds previous active fragment descriptor and verifies it is drawn.
 *
 * @param aFragmentDescriptor Current fragment descriptor
 * @returns Previous active fragment descriptor.
 * If aFragmentDescriptor is NULL,
 *      returns last active fragment descriptor.
 * If aFragmentDescriptor points to first active fragment descriptor,
 *      returns NULL.
 * If previous fragment descriptor is found, but it is not drawn,
 *      returns NULL.
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FindPreviousActiveFragment(
    const CSwtLinkFragmentDescriptor* aFragmentDescriptor) const
{
    return FindActiveFragment(aFragmentDescriptor,
                              iLinkFragmentsDescriptors.Count() - 1, 0);
}

/**
 * Finds active fragment next to given fragment in forward or
 * backward order depending on given aFrom an aTo values.
 * @param aFragmentDescriptor Current fragment descriptor
 * @param aFrom Current fragment descriptor
 * @param aTo Current fragment descriptor
 * @returns Fragment descriptor next to the aFragmentDescriptor in
 *          direction defined by aFrom and aTo
 * If aFragmentDescriptor is NULL,
 *      returns first active fragment descriptor found.
 * If aFragmentDescriptor points to first active fragment descriptor,
 *      returns NULL.
 * If previous fragment descriptor is found, but it is not drawn,
 *      returns NULL.
 *
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FindActiveFragment(
    const CSwtLinkFragmentDescriptor* aFragmentDescriptor,
    const TInt aFrom,
    const TInt aTo) const
{
    if (aFrom < 0 || aTo < 0)
    {
        return NULL;
    }

    TBool currentFound = NULL == aFragmentDescriptor;
    const CSwtLinkFragmentDescriptor* fragment = NULL;
    const TInt change = aFrom < aTo ? 1 : -1;
    const TBool upDownDirection = aFrom < aTo;
    for (TInt i = aFrom; (upDownDirection && i <= aTo)
            || (!upDownDirection && i >= aTo); i += change)
    {
        if (currentFound)
        {
            if (iLinkFragmentsDescriptors[i]->Target())
            {
                fragment = iLinkFragmentsDescriptors[i];
                break;
            }
        }
        else if (iLinkFragmentsDescriptors[i] == aFragmentDescriptor)
        {
            currentFound = ETrue;
        }
    }
    return FragmentDescOrNullIfNotVisible(fragment);
}


/**
 * Verifies that particular fragment descriptor is displayed.
 * @param aFragmentDescriptor Fragment decriptor to be verified.
 * @returns aFragmentDescriptor if at least part of it is drawn;
 *          NULL otherwise.
 */
const CSwtLinkFragmentDescriptor* CSwtLink::FragmentDescOrNullIfNotVisible(
    const CSwtLinkFragmentDescriptor* aFragmentDescriptor) const
{
    const CSwtLinkFragmentDescriptor* fragment = NULL;

    for (TInt fragmentIndex = 0;
            fragmentIndex < iDrawableLinkFragments.Count();
            ++fragmentIndex)
    {
        if (iDrawableLinkFragments[fragmentIndex]
                ->FragmentDescriptor() == aFragmentDescriptor)
        {
            fragment = aFragmentDescriptor;
            break;
        }
    }

    return fragment;
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtLink::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                      TEventCode       aType)
{
    TBool traversalDoIt(ETrue);
    if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
    {
        traversalDoIt = EFalse;
    }
    else if (aKeyEvent.iCode == EKeyLeftArrow)
    {
        iFocusedFragment = FindPreviousActiveFragment(iFocusedFragment);
        traversalDoIt = (NULL == iFocusedFragment) &&
                        GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this);
    }
    else if (aKeyEvent.iCode == EKeyRightArrow)
    {
        iFocusedFragment = FindNextActiveFragment(iFocusedFragment);
        traversalDoIt = (NULL == iFocusedFragment)  &&
                        GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this);
    }
    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::HandleResourceChange(TInt aType)
{
    CAknControl::HandleResourceChange(aType);
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtLink::MopSupplyObject(TTypeUid aId)
{
    return ASwtControlBase::SwtMopSupplyObject(aId);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::SizeChanged()
{
    TRAP_IGNORE(BuildDrawableFragmentsListL(TextRect()));
    SetFocusedFragment();
    HandleSizeChanged();
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::PositionChanged()
{
    HandlePositionChanged();
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::FocusChanged(TDrawNow aDrawNow)
{
    SetFocusedFragment();
    HandleFocusChanged(aDrawNow);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
    const CFont* font(&GetFont()->Font());
    const TRect textRect(TextRect());
    for (TInt i = 0; i < iDrawableLinkFragments.Count(); ++i)
    {
        CSwtDrawableLinkFragment* fragment = iDrawableLinkFragments[i];
        TRect rect = fragment->Rect();
        rect.Move(textRect.iTl);

        TRgb textColor = iTextColor;
        if (fragment->FragmentDescriptor()->Target())
        {
            textColor = iLinkColor;
        }

        // Same background highlight as that of HyperLink
        if (fragment->FragmentDescriptor() == iFocusedFragment
                && (iPressed || iDisplay.UiUtils().NaviKeyInput()))
        {
            textColor = iHighlightedLinkColor;
            gc.SetPenStyle(CGraphicsContext::ENullPen);
            gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
            gc.SetBrushColor(iHighlightColor);
            gc.DrawRect(rect);
        }

        gc.UseFont(font);
        gc.SetPenStyle(CGraphicsContext::ESolidPen);
        gc.SetBrushStyle(CGraphicsContext::ENullBrush);
        gc.SetUnderlineStyle(fragment->FragmentDescriptor()->Target()
                             ? EUnderlineOn : EUnderlineOff);
        gc.SetPenColor(textColor);

        const TInt baseLineY = font->FontMaxAscent();
        TPoint drawPos(rect.iTl.iX, rect.iTl.iY + baseLineY);
        gc.DrawText(fragment->Text(), drawPos);
    }
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtLink::CoeControl()
{
    return *this;
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtLink::CoeControl() const
{
    return *this;
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::ProcessKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    if (aType == EEventKey)
    {
        if ((aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
                && NULL != iFocusedFragment)
        {
            iDisplay.PostSelectionEventL(iPeer, *iFocusedFragment->Target());
        }
        else if (aKeyEvent.iCode == EKeyLeftArrow
                 || aKeyEvent.iCode == EKeyRightArrow)
        {
            Redraw();
        }
    }
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    const CSwtLinkFragmentDescriptor* tappedFragment = NULL;
    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
    {
        if (iMultipleTargets)
        {
            iFocusedFragment = NULL;
            tappedFragment = FindFragmentOnPosition(aPointerEvent.iPosition);
        }
        else
        {
            // If single target, the focused fragment does not change
            // with pointer
            tappedFragment = iFocusedFragment;
        }
        if (tappedFragment && tappedFragment->Target())
        {
            iPressed = ETrue;
            iFocusedFragment = tappedFragment;
#ifdef RD_TACTILE_FEEDBACK
            if (iFeedback)
            {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
#else
                iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            }
#endif //RD_TACTILE_FEEDBACK
        }
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        else if (iFocusChanged)
        {
            iFeedback->InstantFeedback(ETouchFeedbackSensitiveList);
            iFocusChanged = EFalse;
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

        if (iPressed)
        {
            Redraw();
        }
        break;
    }

    case TPointerEvent::EDrag:
    {
        TBool pressed = iPressed;

        if (iMultipleTargets)
        {
            tappedFragment = FindFragmentOnPosition(aPointerEvent.iPosition);
        }
        else
        {
            if (Rect().Contains(aPointerEvent.iPosition))
            {
                // If single target, the focused fragment does not change
                // with pointer
                tappedFragment = iFocusedFragment;
            }
        }

        if (tappedFragment && tappedFragment == iFocusedFragment)
        {
            iPressed = ETrue;
        }
        else
        {
            iPressed = EFalse;
        }

        if (pressed != iPressed)
        {
            Redraw();
        }
        break;
    }

    case TPointerEvent::EButton1Up:
    {
        TBool pressed = iPressed;
        iPressed = EFalse;

        if (iMultipleTargets)
        {
            tappedFragment = FindFragmentOnPosition(aPointerEvent.iPosition);
        }
        else
        {
            if (Rect().Contains(aPointerEvent.iPosition))
            {
                // If single target, the focused fragment does not change with pointer
                tappedFragment = iFocusedFragment;
            }
        }

        if (!iDisplay.RevertPointerEvent() && tappedFragment
                && tappedFragment == iFocusedFragment && tappedFragment->Target())
        {
            iDisplay.PostSelectionEventL(iPeer, *iFocusedFragment->Target());
        }

        if (pressed != iPressed)
        {
            Redraw();
        }
        break;
    }
    }

    PostMouseEventL(aPointerEvent);
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtLink::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);
    aColor ? iCustomTextColor = ETrue : iCustomTextColor = EFalse;
    if (iCustomTextColor)
    {
        TRgb rgb;
        TBool overrideColorSet(GetColor(EColorControlText, rgb));
        ASSERT(overrideColorSet);
        iTextColor = rgb;
        iLinkColor = rgb;
    }
    else
    {
        UpdateSkinColor();
    }
    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
TSize CSwtLink::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    return ComputeTextSizeL(aWHint, aHHint);
}


// ---------------------------------------------------------------------------
// CSwtLink::ComputeTextSizeL
// ---------------------------------------------------------------------------
//
TSize CSwtLink::ComputeTextSizeL(TInt aWHint, TInt aHHint)
{
    // Return non default hints
    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    // Compute size
    TSize prefSize(TSize::EUninitialized);
    const TInt vertMargin = 2 * (iVerticalMargin + KBorderMargin);
    const TInt horizMargin = 2 * (iHorizontalMargin + KBorderMargin);
    if (!(aWHint == KSwtDefault && aHHint == KSwtDefault))
    {
        // Wrap text with a default hint
        if (aWHint != KSwtDefault)
        {
            prefSize.iWidth = aWHint;
            prefSize.iHeight = CalcWrappedTextHeightL(aWHint - horizMargin)
                               + vertMargin;
        }
        else
        {
            prefSize.iHeight = aHHint;
            prefSize.iWidth = CalcWrappedTextWidth(aHHint - vertMargin)
                              + horizMargin;
        }
    }
    else
    {
        // This will return the width of the longest line and height that
        // fits all lines
        prefSize = MinimumSize();
        prefSize += TPoint(horizMargin, vertMargin);
    }

    if (aHHint != KSwtDefault)
    {
        prefSize.iHeight = aHHint;
    }

    if (aWHint != KSwtDefault)
    {
        prefSize.iWidth = aWHint;
    }

    return prefSize;
}

// ---------------------------------------------------------------------------
// CSwtLink::CalcWrappedTextHeightL
// ---------------------------------------------------------------------------
//
TInt CSwtLink::CalcWrappedTextHeightL(TInt aWidth)
{
    if (iText.Length() == 0 || aWidth <= 0)
    {
        return 0;
    }

    // Calculate the number of lines when wrapping text to the given rectangle
    const TInt ARRAY_GRANULARITY = 4;
    CArrayFixFlat<TPtrC>* wrappedArray = new(ELeave) CArrayFixFlat<TPtrC>(
        ARRAY_GRANULARITY);
    CleanupStack::PushL(wrappedArray);
    const CFont* font(&GetFont()->Font());

    AknTextUtils::WrapToArrayL(iText, aWidth, *font, *wrappedArray);

    TInt nbStrings = wrappedArray->Count();
    if (nbStrings < 1)
    {
        // It happens when aWidth < Char's width
        // here just set nBStrings to 1
        // so something can be visible.
        nbStrings = 1;
    }

    // Calculate text height
    const TInt result(MinimumHeight(nbStrings));

    wrappedArray->Reset();
    CleanupStack::PopAndDestroy(wrappedArray);
    return result;
}

// ---------------------------------------------------------------------------
// CSwtLink::CalcWrappedTextWidth
// ---------------------------------------------------------------------------
//
TInt CSwtLink::CalcWrappedTextWidth(TInt aHeight)
{
    if (iText.Length() == 0 || aHeight <= 0)
    {
        return 0;
    }
    // This will return the width of the longest line and height that fits
    // all lines
    return MinimumWidth();
}

// ---------------------------------------------------------------------------
// CSwtLink::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CSwtLink::MinimumSize() const
{
    const TInt height = MinimumHeight();
    const TInt width = MinimumWidth();
    return TSize(width, height);
}

// ---------------------------------------------------------------------------
// CSwtLink::MinimumHeight
// ---------------------------------------------------------------------------
//
TInt CSwtLink::MinimumHeight() const
{
    return(MinimumHeight(iTextLineCount));
}

// ---------------------------------------------------------------------------
// CSwtLink::MinimumHeight
// ---------------------------------------------------------------------------
//
TInt CSwtLink::MinimumHeight(TInt aLineCount) const
{
    const MSwtFont* font = GetFont();
    TInt res = aLineCount *
               (iLineGap + font->Font().FontMaxHeight())
               - (aLineCount > 0 ? iLineGap : 0);

    // Help the inline case. Makes sense if there is only one line of text
    if (aLineCount == 1 && font == &DefaultFont())
    {
        res = Max(iDisplay.UiUtils().InlineReferenceFontHeight(), res);
    }

    return res;
}

// ---------------------------------------------------------------------------
// CSwtLink::MinimumWidth
// ---------------------------------------------------------------------------
//
TInt CSwtLink::MinimumWidth() const
{
    if (iText.Length() == 0)
    {
        return 0;
    }

    TInt result = 0;
    TPtrC ptr(iText);
    const CFont* font(&GetFont()->Font());
    CFont::TMeasureTextInput::TFlags order =
        CFont::TMeasureTextInput::EFVisualOrder;
    if (IsRtl())
    {
        order = CFont::TMeasureTextInput::EFVisualOrderRightToLeft;
    }

    FOREVER
    {
        const TInt pos = ptr.Locate('\n');
        TPtrC left = ptr;
        if (pos >= 0)
        {
            left.Set(ptr.Left(pos));
        }
        TInt lineW = AknBidiTextUtils::MeasureTextBoundsWidth(*font, left,
                     order);
        if (lineW > result)
        {
            result = lineW;
        }
        if (pos < 0 || pos == ptr.Length() - 1)
        {
            break;
        }
        ptr.Set(ptr.Mid(pos + 1));
    }

    return result;
}


// ---------------------------------------------------------------------------
// CSwtLink::IsRtl
// ---------------------------------------------------------------------------
//
TBool CSwtLink::IsRtl() const
{
    TBool result = AknLayoutUtils::LayoutMirrored();
    if (iStyle & KSwtStyleLeftToRight)
    {
        result = EFalse;
    }
    else if (iStyle & KSwtStyleRightToLeft)
    {
        result = ETrue;
    }
    return result;
}

// ---------------------------------------------------------------------------
// CSwtLink::DefaultFont
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtLink::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);
    DoSetFont(&GetFont()->Font());
    Redraw();
}


void CSwtLink::DoSetFont(const CFont* aFont)
{
    iLineGap = Max(aFont->FontLineGap() - aFont->FontMaxHeight(),
                   KSwtMinLinePadding);
    iLineHeight = aFont->FontMaxHeight();

    TRAP_IGNORE(BuildDrawableFragmentsListL(TextRect()));
}

// ---------------------------------------------------------------------------
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtLink::DefaultFont() const
{
    return *iDefaultFont;
}


// ---------------------------------------------------------------------------
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
HBufC* CSwtLink::MSKLabelL() const
{
    return iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
}


// ---------------------------------------------------------------------------
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
TBool CSwtLink::IsKeyUsed(TUint aKeyCode) const
{
    TBool keyUsed(EFalse);
    if (aKeyCode == EKeyOK ||
            aKeyCode == EKeyEnter ||
            aKeyCode == EKeyLeftArrow ||
            aKeyCode == EKeyRightArrow ||
            aKeyCode == EKeyUpArrow ||
            aKeyCode == EKeyDownArrow
       )
    {
        keyUsed = ETrue;
    }
    return keyUsed;
}

// ---------------------------------------------------------------------------
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtLink::Control()
{
    return this;
}


// ---------------------------------------------------------------------------
// From class MSwtLink.
// ---------------------------------------------------------------------------
//
void CSwtLink::SetLinkDataL(
    const CDesCArray* aTextFragments,
    const CDesCArray* aLinkTargets)
{
    ASSERT(aTextFragments);
    ASSERT(aLinkTargets);
    ASSERT(aTextFragments->Count() == aLinkTargets->Count());

    const TInt textLength = BuildFragmentListL(aTextFragments, aLinkTargets);
    AssembleLinkTextL(aTextFragments, textLength);

    //Prepare formatted text container
    iFormattedText.Close();
    iFormattedText.CreateL(textLength);

    BuildDrawableFragmentsListL(TextRect());

    SetFocusedFragment();

    Redraw();
}

/**
 * If control has focus, this method finds first active
 * link fragment and sets focus to it.
 */
void CSwtLink::SetFocusedFragment()
{
    if (IsFocused())
    {
        iFocusedFragment = FindFirstActiveFragment();
    }
    else
    {
        iFocusedFragment = NULL;
    }
}

TBool CSwtLink::SetSwtFocus(TInt aReason /*= KSwtFocusByApi*/)
{
    TBool prevFocused = IsFocusControl();
    TBool res = ASwtControlBase::SetSwtFocus(aReason);

    // Gaines focus by pointer
    if (IsFocusControl() && !prevFocused)
    {
        iFocusChanged = ETrue;
    }

    return res;
}

// ---------------------------------------------------------------------------
// CSwtLinkFragmentDescriptor
// ---------------------------------------------------------------------------
//
/**
 * Creates new instance of non-active link fragment descriptor.
 * @param aOffset Fragment text offset from the beggining of the link
 *                plain text.
 * @param aLength Fragment text length
 */
CSwtLinkFragmentDescriptor* CSwtLinkFragmentDescriptor::NewL(
    TInt aOffset,
    TInt aLength)
{
    CSwtLinkFragmentDescriptor* self =
        new(ELeave) CSwtLinkFragmentDescriptor(aOffset, aLength);
    return self;
}

/**
 * Creates new instance of active link fragment descriptor.
 * @param aOffset Fragment text offset from the beggining of the link
 *                plain text.
 * @param aLength Fragment text length
 * @param aTarget Text passed to selection listner.
 */
CSwtLinkFragmentDescriptor* CSwtLinkFragmentDescriptor::NewL(
    TInt aOffset,
    TInt aLength,
    const TDesC& aTarget)
{
    CSwtLinkFragmentDescriptor* self =
        new(ELeave) CSwtLinkFragmentDescriptor(aOffset, aLength);
    CleanupStack::PushL(self);
    self->ConstructL(aTarget);
    CleanupStack::Pop(self);
    return self;
}

/**
 * Constructor.
 * @param aOffset Fragment text offset from the beggining of the link
 *                plain text.
 * @param aLength Fragment text length
 */
CSwtLinkFragmentDescriptor::CSwtLinkFragmentDescriptor(
    TInt aOffset,
    TInt aLength)
        : iOffset(aOffset)
        , iLength(aLength)
        , iTarget(NULL)
{
}

/**
 * Constructs current insance.
 * @param aTarget String to be passed to selection listener.
 */
void CSwtLinkFragmentDescriptor::ConstructL(const TDesC& aTarget)
{
    iTarget = aTarget.AllocL();
}

/**
 * Destructor.
 */
CSwtLinkFragmentDescriptor::~CSwtLinkFragmentDescriptor()
{
    if (iTarget)
    {
        delete iTarget;
        iTarget = NULL;
    }
}

/**
 * Returns offset from the beggining of the link text where the fragment
 * begins.
 */
TInt CSwtLinkFragmentDescriptor::Offset() const
{
    return iOffset;
}

/**
 * Returns the fragment text length.
 */
TInt CSwtLinkFragmentDescriptor::Length() const
{
    return iLength;
}

/**
 * Returns string passed to selection listenr for active link fragments
 * or NULL for non-active fragments.
 */
const TDesC* CSwtLinkFragmentDescriptor::Target() const
{
    return iTarget;
}


// ---------------------------------------------------------------------------
// CSwtDrawableLinkFragment
// ---------------------------------------------------------------------------
//

/**
 * Creates new instance
 * @param aText Text asociated to drawable fragment.
 * @param aRect Rectangle into which fragment should be drawn.
 * @param aFragmentDescriptor Fragment descriptor to which this drawable
 *                            fragment belongs.
 */
CSwtDrawableLinkFragment* CSwtDrawableLinkFragment::NewL(
    const TPtrC& aText,
    const TRect& aRect,
    const CFont& aFont,
    const CSwtLinkFragmentDescriptor* aFragmentDescriptor)
{
    CSwtDrawableLinkFragment* self =
        new(ELeave) CSwtDrawableLinkFragment(aRect,
                                             aFragmentDescriptor);
    CleanupStack::PushL(self);
    self->ConstructL(aText, aFont);
    CleanupStack::Pop(self);
    return self;
}

/**
 * Destructor.
 */
CSwtDrawableLinkFragment::~CSwtDrawableLinkFragment()
{
    iFormattedText.Close();
}

/**
 * Constructor
 * @param aText Text asociated to drawable fragment.
 * @param aRect Rectangle into which fragment should be drawn.
 * @param aFragmentDescriptor Fragment descriptor to which this drawable
 *                            fragment belongs.
 */
CSwtDrawableLinkFragment::CSwtDrawableLinkFragment(
    const TRect& aRect,
    const CSwtLinkFragmentDescriptor* aFragmentDescriptor)
        : iRect(aRect)
        , iFragmentDescriptor(aFragmentDescriptor)
{
}

/**
 * Constructs the instance.
 */
void CSwtDrawableLinkFragment::ConstructL(const TPtrC& aText,
        const CFont& aFont)
{
    iFormattedText.CreateL(aText.Length() + KAknBidiExtraSpacePerLine);
    const TInt width = iRect.Width();
    AknBidiTextUtils::ConvertToVisualAndClip(aText, iFormattedText, aFont,
            width, width);
}

/**
 * Return fragment descriptor to which this drawable fragment belongs.
 */
const CSwtLinkFragmentDescriptor*
CSwtDrawableLinkFragment::FragmentDescriptor() const
{
    return iFragmentDescriptor;
}

/**
 * Returns rectangle into which drawable fragment should be drawn.
 */
const TRect& CSwtDrawableLinkFragment::Rect() const
{
    return iRect;
}

/**
 * Returns text belonging to the drawable fragment.
 */
const TDes& CSwtDrawableLinkFragment::Text() const
{
    return iFormattedText;
}

