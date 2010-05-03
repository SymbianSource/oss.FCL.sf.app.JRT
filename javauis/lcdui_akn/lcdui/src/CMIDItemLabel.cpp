/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used to draw the item labels and some string items. Provides
*                word wrapping and other functionality.
*
*/


#include <coemain.h>
// API for CEikLabel used in several places
#include <eikcapc.h>
// API for text wrapping
#include <AknBidiTextUtils.h>
// using CAknLayoutFont for retrieving height in pixels of each line
#include <AknLayoutFont.h>
// used in constructor for obtaining label color from skin
#include <AknsDrawUtils.h>

#include "CMIDControlItem.h"
#include "CMIDGc.h"
// using CMIDUtils::IsLineSeparator
#include "CMIDUtils.h"
#include "CMIDCommand.h"
#include "CMIDForm.h"
#include "CMIDCommandList.h"
#include "CMIDItemLabel.h"
#include "CMIDFont.h"

// using TAknWindowLineLayout (in UpdateMargins function)
#include <applayout.cdl.h>
// LAF - AknLayoutScalable_Avkon::form2_midp_label_pane_cp (in UpdateMargins function)
#include <aknlayoutscalable_avkon.cdl.h>


CMIDItemLabel* CMIDItemLabel::NewL(TInt aMaxWidth, TBool aLabelBeforeContent,
                                   TInt aMaxNumberOfLines, const CMIDFont::TDefaultId& aDefaultFontId, TBool aIsContent)
{
    CMIDItemLabel* self = new(ELeave) CMIDItemLabel(aMaxWidth,
            aLabelBeforeContent, aMaxNumberOfLines, aDefaultFontId, aIsContent);

    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

CMIDItemLabel::~CMIDItemLabel()
{
    delete iText;
    iText = NULL;
    delete iWrappedText;
    iWrappedText = NULL;

    delete iWrappedArray;
    iWrappedArray = NULL;

    delete iLineWidthArray;
    iLineWidthArray = NULL;

    ResetLabelArray();
    delete iLabelArray;
    iLabelArray = NULL;

    delete iPictographInterface;
    iPictographInterface = NULL;
}

/** Returns the width of the ellipsis truncation character
    and the average line height */
TSize CMIDItemLabel::MinimumSize()
{
    if (iLabelBeforeContent)
    {
        return TSize(Font()->CharWidthInPixels(KEllipsis) + iLabelMargins.iLeft
                     + iLabelMargins.iRight,
                     LineHeight() + iLabelMargins.iTop);
    }
    else
    {
        return TSize(Font()->CharWidthInPixels(KEllipsis) + iLabelMargins.iLeft
                     + iLabelMargins.iRight,
                     LineHeight() + iLabelMargins.iBottom);
    }

}

TInt CMIDItemLabel::CountComponentControls() const
{
    return iLabelArray->Count();
}

CCoeControl* CMIDItemLabel::ComponentControl(TInt aIndex) const
{
    return (*iLabelArray)[aIndex];
}

/**
Returns the preferred size, this is not the one given by the
application but the size that allows the text to be displayed
without wrapping at word level but only wrapping at line level.
*/
TSize CMIDItemLabel::PreferredSize() const
{
    if (iLabelBeforeContent)
    {
        return TSize(PreferredWidth(), LineHeight() + iLabelMargins.iTop);
    }
    else
    {
        return TSize(PreferredWidth(), LineHeight() + iLabelMargins.iBottom);
    }
}

// it is up to the component changing the text to tell the form to update. This will lead to
// the label being re-wrapped
void CMIDItemLabel::SetTextL(const TDesC& aText)
{
    delete iText;
    iText = NULL;

    iText = aText.AllocL();

    iWrappedArray->Reset();
    ResetLabelArray();

    delete iWrappedText;
    iWrappedText = NULL;

}

/**
    Return the width of the longest line amongst those
    allowed by iMaxNumberOfLines
*/
TInt CMIDItemLabel::PreferredWidth() const
{
    return Min((LongestLineWidth() + iLabelMargins.iLeft + iLabelMargins.iRight), iMaxWidth);
}

/** Return the width of the longest line as long as this line
    is not beyond iMaxNumberOfLines. If we haven't exausted
    all the text we add the width of the ellipses truncation indicator.
    */
TInt CMIDItemLabel::LongestLineWidth() const
{
    if (!iText)
    {
        return Font()->CharWidthInPixels(KEllipsis);
    }

    TInt length = iText->Length();
    TInt width = 0;
    TBool inLine = EFalse;
    TInt lineStartIdx = 0;
    TInt numLines = 0;

    for (TInt i=0; i < length; i++)
    {
        if (!IsLineSeparator((*iText)[i]) && !inLine)
        {
            inLine = ETrue;
            lineStartIdx = i;
        }
        else if (IsLineSeparator((*iText)[i]) && inLine)
        {
            inLine = EFalse;
            numLines++;

            TPtrC ptr = iText->Mid(lineStartIdx, i - lineStartIdx);
            TInt w = Font()->TextWidthInPixels(ptr);
            if (w > width)
            {
                width = w;
            }
        }
        else if (inLine && (i == (length - 1)))
        {
            inLine = EFalse;
            TPtrC ptr = iText->Mid(lineStartIdx, (i - lineStartIdx) + 1);
            TInt w = Font()->TextWidthInPixels(ptr);
            if (w > width)
            {
                width = w;
            }
        }

        if ((iMaxNumberOfLines > 0) && (numLines >= iMaxNumberOfLines))
        {
            if (i != (length - 1))
            {//if there is still text add space for ellipsis truncation
                width += Font()->CharWidthInPixels(KEllipsis);
            }
            break;
        }
    }

    return width;
}

/**
 *  Returns the height in pixels of each line
 */
TInt CMIDItemLabel::LineHeight() const
{
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull(Font());

    TInt height = 0;
    if (layoutFont)
    {
        // Calculate height of one line
        height += layoutFont->TextPaneHeight();
    }
    else
    {
        height += Font()->HeightInPixels();
    }
    return height;
}

/**
 * Prepare the line width array to the case of all lines having the same
 * width and then call WrapTextAndSetSizeL()
 *
 * @see WrapTextAndSetSizeL()
 */
void CMIDItemLabel::SetWidthL(TInt aLineWidth)
{
    iLineWidthArray->Reset();
    iLineWidthArray->AppendL(aLineWidth - iLabelMargins.iLeft - iLabelMargins.iRight);

    WrapTextAndSetSizeL();
}

/**
 *  Wrap the text and set the size of the control to the longest line width and to the
 *  sum of the height of the lines of text.
 */
void CMIDItemLabel::WrapTextAndSetSizeL()
{

    iWrappedArray->Reset();
    ResetLabelArray();

    delete iWrappedText;
    iWrappedText = NULL;

    if (iMaxNumberOfLines > 0)
    {
        iWrappedText = HBufC::NewL(iText->Length()
                                   + (iMaxNumberOfLines * KAknBidiExtraSpacePerLine));

        TPtr wrappedTextPtr = iWrappedText->Des();
        wrappedTextPtr.Append(*iText);

        CArrayFixFlat<TInt>* lineWidthArray = new(ELeave) CArrayFixFlat<TInt>(iMaxNumberOfLines);
        CleanupStack::PushL(lineWidthArray);

        TInt numWidthsAvailable = iLineWidthArray->Count();
        ASSERT(numWidthsAvailable > 0);

        for (TInt i = 0; i < iMaxNumberOfLines; i++)
        {
            TInt width = i < numWidthsAvailable ?
                         (*iLineWidthArray)[i] :
                         (*iLineWidthArray)[numWidthsAvailable - 1];
            lineWidthArray->AppendL(width);
        }

        AknBidiTextUtils::ConvertToVisualAndWrapToArrayL
        (wrappedTextPtr, *lineWidthArray, *Font(), *iWrappedArray, ETrue);

        CleanupStack::PopAndDestroy(lineWidthArray);
    }
    else if (iMaxNumberOfLines == -1)
    { //unlimited wrapping
        iWrappedText= AknBidiTextUtils::ConvertToVisualAndWrapToArrayWholeTextL
                      (*iText, *iLineWidthArray, *Font(), *iWrappedArray);
    }

    TInt width = 0;
    TInt numOfLabels = iWrappedArray->Count();
    for (TInt i=0; i < numOfLabels; i++)
    {
        CEikLabel* tmp = new(ELeave) CEikLabel();
        CleanupStack::PushL(tmp);
        tmp->UseLogicalToVisualConversion(EFalse);
        tmp->OverrideColorL(EColorLabelText, iColor);
        tmp->SetFont(Font());
        tmp->SetTextL((*iWrappedArray)[i]);
        tmp->SetSize(CMIDItemLabel::PropperEikLabelMinumumSize(*tmp));
        tmp->EnablePictographsL(*iPictographInterface);
        iLabelArray->AppendL(tmp);
        CleanupStack::Pop(tmp);
        TSize lineSize = tmp->Size();
        width = width < lineSize.iWidth ? lineSize.iWidth : width;
        tmp->ActivateL();
    }

    SetUnderlined(iUnderlined);

    // we rely on set extent being called at some later stage to trigger SizeChanged()
    // Label Top margin is added to label height

    width += iLabelMargins.iLeft + iLabelMargins.iRight;
    TInt height = LineHeight() * NumLines();
    if (iIsContent)
    {
        // adds margin to bottom of string item content
        height += iLabelMargins.iBottom;
    }
    if (iLabelBeforeContent)
    {
        iSize = TSize(width, height + iLabelMargins.iTop);
    }
    else
    {
        iSize = TSize(width, height + iLabelMargins.iBottom);
    }
}

/**
 *  Prepare the line width array for the case in which the first line has
 *  a different width or the remaining lines and then call WrapTextAndSetSizeL()
 *
 * @see WrapTextAndSetSizeL()
 */
void CMIDItemLabel::SetVariableWidthL(TInt aFirstWidth, TInt aSecondWidth)
{
    iLineWidthArray->Reset();

    iLineWidthArray->AppendL(aFirstWidth - iLabelMargins.iLeft - iLabelMargins.iRight);
    iLineWidthArray->AppendL(aSecondWidth - iLabelMargins.iLeft - iLabelMargins.iRight);

    WrapTextAndSetSizeL();
}


/** Returns the width in pixels of the first word */
TInt CMIDItemLabel::FirstWordWidth() const
{
    if (!iText)
    {
        return Font()->CharWidthInPixels(KEllipsis);
    }

    TInt pos = iText->Length();
    for (TInt i = 0; i < iText->Length(); i++)
    {
        if (TChar((*iText)[i]).IsSpace() || IsLineSeparator((*iText)[i]))
        {
            pos = i;
            break;
        }
    }

    return Font()->TextWidthInPixels(iText->Left(pos));
}

void CMIDItemLabel::SetCentered(TBool aValue)
{
    if (aValue != iCentered)
    {
        iCentered = aValue;

        // re-layout the lables
        SizeChanged();
    }
}

void CMIDItemLabel::SetUnderlined(TBool aUnderlined)
{
    iUnderlined = aUnderlined;
    TInt i;
    for (i=0; i < iLabelArray->Count(); i++)
    {
        CEikLabel* label = (CEikLabel*)((*iLabelArray)[i]);
        label->SetUnderlining(iUnderlined);
    }
}

void CMIDItemLabel::SetColorL(TRgb aColor)
{
    iColor = aColor;
    AknLayoutUtils::OverrideControlColorL(*this, EColorLabelText, iColor);
    TInt i;
    for (i=0; i < iLabelArray->Count(); i++)
    {
        CEikLabel* label = (CEikLabel*)((*iLabelArray)[i]);
        label->OverrideColorL(EColorLabelText, iColor);
    }
}

void CMIDItemLabel::SetEmphasisL(TBool aEmphasis)
{
    TInt i;
    CEikLabel::TTextEmphasis emphasis = aEmphasis ?
                                        CEikLabel::EPartialEmphasis :
                                        CEikLabel::ENoEmphasis;

    for (i=0; i < iLabelArray->Count(); i++)
    {
        CEikLabel* label = (CEikLabel*)((*iLabelArray)[i]);
        label->OverrideColorL(EColorLabelHighlightPartialEmphasis, TRgb(0x32, 0x99, 0xCC));
        label->SetEmphasis(emphasis);
    }
}

TRgb CMIDItemLabel::GetDefaultColor()
{
    return iDefaultColor;
}

void CMIDItemLabel::SetFont(const MMIDFont* aFont)
{
    iFont = aFont;
    PrepareFont();

    for (TInt i=0; i < iLabelArray->Count(); i++)
    {
        CEikLabel* label = (CEikLabel*)((*iLabelArray)[i]);
        label->SetFont(Font());
    }
}

const CFont* CMIDItemLabel::Font() const
{
    return iFont ? const_cast<MMIDFont*>(iFont)->Font(ETrue) : CMIDFont::DefaultFont(iDefaultFontId);
}

CEikLabel* CMIDItemLabel::LabelAtIdx(TInt aIdx)
{
    if (aIdx > -1 && aIdx < iLabelArray->Count())
    {
        return (*iLabelArray)[aIdx];
    }
    return NULL;
}

void CMIDItemLabel::SetMaxWidth(TInt aMaxWidth)
{
    iMaxWidth = aMaxWidth;
}

CMIDItemLabel::CMIDItemLabel(TInt aMaxWidth, TBool aLabelBeforeContent, TInt aMaxNumberOfLines,
                             const CMIDFont::TDefaultId& aDefaultFontId, TBool aIsContent) :
        iDefaultFontId(aDefaultFontId), iMaxWidth(aMaxWidth),
        iMaxNumberOfLines(aMaxNumberOfLines), iLabelBeforeContent(aLabelBeforeContent), iIsContent(aIsContent)
{
}

void CMIDItemLabel::ConstructL()
{
    iWrappedArray = new(ELeave) CArrayFixFlat<TPtrC>(2);
    iLabelArray = new(ELeave) CArrayFixFlat<CEikLabel*>(2);

    iLineWidthArray = new(ELeave) CArrayFixFlat<TInt>(2);

    // Label should have proper color. This color is obtained from skin.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if (skin)
    {
        AknsUtils::GetCachedColor(skin, iDefaultColor, KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG8);
        iColor = iDefaultColor;
    }

    iPictographInterface = CAknPictographInterface::NewL(*this, *this);
    UpdateMargins();
    PrepareFont();
}

void CMIDItemLabel::UpdateMargins()
{
    TAknWindowLineLayout layout =
        AknLayoutScalable_Avkon::form2_midp_label_pane_cp(0).LayoutLine();

    iLabelMargins.iTop    = layout.it;
    iLabelMargins.iLeft   = layout.il;
    iLabelMargins.iRight  = layout.ir;

    // bottom margin is defined only for ImageItem, so it's read from imageitem's label pane
    layout = AknLayoutScalable_Avkon::form2_midp_label_pane_cp(2).LayoutLine();

    iLabelMargins.iBottom = layout.ib;

}

/**
* Set font size for later using in labels layouting (@see SizeChanged())
*/
void CMIDItemLabel::PrepareFont()
{
    const TInt KJavaFontSmall  = 16; // Java-side small font
    const TInt KJavaFontMedium = 18; // Java-side medium font

    iFontSize = 1; //if ItemLabel is label part of StringItem (or some other item which has label)
    if (iIsContent)
    {
        //if ItemLabel is content part of StringItem font can have three different heights
        //according to java side font
        TInt fontHeight = Font()->HeightInPixels();
        if (fontHeight <= KJavaFontSmall)   // Java-side small font
        {
            iFontSize = 0;
        }
        else if (fontHeight <= KJavaFontMedium)   // Java-side medium font
        {
            iFontSize = 1;
        }
        else // Java-side large font
        {
            iFontSize = 2;
        }
    }
}

void CMIDItemLabel::SizeChanged()
{
    TAknLayoutRect layoutRect;
    CEikLabel* label;
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull(Font());

    if (iIsContent)   // This is a StringItem content
    {
        // get label rect
        TRect rect = Rect();

        // layout rect
        layoutRect.LayoutRect(rect,
                              AknLayoutScalable_Avkon::form2_midp_string_pane(0).LayoutLine());
        rect = layoutRect.Rect();

        // determine maximum rows based on layout data available
        TAknLayoutScalableParameterLimits textLimits =
            AknLayoutScalable_Avkon::form2_mdip_string_pane_t1_ParamLimits(iFontSize);

        TInt maxRows = textLimits.LastRow();

        // layout the lines
        TInt numOfLabels = iLabelArray->Count();
        TAknTextLineLayout lineLayout;

        for (TInt i = 0; i < numOfLabels; i++)
        {
            label = (*iLabelArray)[i];

            if (i < maxRows)
            {
                // get proper layout
                lineLayout = AknLayoutScalable_Avkon::form2_mdip_string_pane_t1(
                                 iFontSize, i).LayoutLine();
            }

            // do the layouting of the label using the prepared layout
            AknLayoutUtils::LayoutLabel(label, rect, lineLayout);

            if (i > 0)
            { // because layout function adds small gap between lines, we must
                // set proper position
                label->SetPosition(TPoint(label->Position().iX ,
                                          (*iLabelArray)[i-1]->Position().iY + (*iLabelArray)[i-1]->Size().iHeight));
            }

            // while LayoutLabel doesn't produce the right color we have to
            // override it with correct value
            TRAP_IGNORE(label->OverrideColorL(EColorLabelText, iColor));

            // as font in layout data is not correct for non-default fonts
            // we need to change labels font directly
            label->SetFont(Font());
            //
            label->SetSize(TSize(label->Size().iWidth, LineHeight()));

            if (iCentered)
            {
                label->SetLabelAlignment(ELayoutAlignCenter);
            }

        } //for
    }
    else // This is a label
    {
        // get label rect
        TRect rect = Rect();

        // layout rect
        layoutRect.LayoutRect(rect,
                              AknLayoutScalable_Avkon::form2_midp_label_pane(0).LayoutLine());
        rect = layoutRect.Rect();

        // determine maximum rows based on layout data available
        TAknLayoutScalableParameterLimits textLimits =
            AknLayoutScalable_Avkon::form2_midp_label_pane_t1_ParamLimits();

        TInt maxRows = textLimits.LastRow();

        // layout the lines
        TInt numOfLabels = iLabelArray->Count();
        TAknTextLineLayout lineLayout;

        for (TInt i = 0; i < numOfLabels; i++)
        {
            label = (*iLabelArray)[i];

            if (i < maxRows)
            {
                // get proper layout
                lineLayout = AknLayoutScalable_Avkon::form2_midp_label_pane_t1(i, 0).LayoutLine();
            }

            // do the layouting of the label using the prepared layout
            AknLayoutUtils::LayoutLabel(label, rect, lineLayout);

            if (i > 0)
            { // because layout function adds small gap between lines, we must
                // set proper position
                label->SetPosition(TPoint(label->Position().iX ,
                                          (*iLabelArray)[i-1]->Position().iY + (*iLabelArray)[i-1]->Size().iHeight));
            }

            // while LayoutLabel doesn't produce the right color we have to make workaround
            // to override it with correct value
            TRAP_IGNORE(label->OverrideColorL(EColorLabelText, iColor));

            if (iCentered)
            {
                label->SetLabelAlignment(ELayoutAlignCenter);
            }

        } //for
    } //else
}

void CMIDItemLabel::ResetLabelArray()
{
    for (TInt i=0; i < iLabelArray->Count(); i++)
    {
        delete(*iLabelArray)[i];
    }
    iLabelArray->Reset();
}

TBool CMIDItemLabel::IsLineSeparator(const TText aChar) const
{
    return CMIDUtils::IsLineSeparator(aChar);
}

TSize CMIDItemLabel::PropperEikLabelMinumumSize(CEikLabel& aLabel) const
{
    TSize size = aLabel.MinimumSize();
    size.iHeight = LineHeight();

    if (aLabel.LogicalToVisualConversionUsed())
    {
        return size;
    }

    const TDesC* text = aLabel.Text();

    TInt textLength = aLabel.Font()->TextWidthInPixels(*text);

    // Calculate correct width, because label calculates it wrong.
    size.iWidth = textLength + aLabel.iMargin.iLeft + aLabel.iMargin.iRight;

    return size;
}

void CMIDItemLabel::DrawPictographArea()
{
    if (IsVisible())
    {
        RDrawableWindow* rDrawableWindow = DrawableWindow();
        if (rDrawableWindow)
        {
            DrawDeferred();
        }
    }
}

void CMIDItemLabel::ResolutionChange()
{
    UpdateMargins();
}

TInt CMIDItemLabel::ItemLabelMargin()
{
    return iLabelBeforeContent ? iLabelMargins.iTop : iLabelMargins.iBottom;
}

void CMIDItemLabel::AdjustToSizeL(const TSize& aSize)
{
    if (iSize.iWidth != aSize.iWidth || iSize.iHeight != aSize.iHeight)
    {
        TInt oldNumLabelLines = iMaxNumberOfLines;
        iMaxNumberOfLines = Min(iMaxNumberOfLines,
                                (aSize.iHeight - ItemLabelMargin()) / LineHeight());

        SetWidthL(aSize.iWidth);
        iMaxNumberOfLines = oldNumLabelLines;
    }
}

//end of File
