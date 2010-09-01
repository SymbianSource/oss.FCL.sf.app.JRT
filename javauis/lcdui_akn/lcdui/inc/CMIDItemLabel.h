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


#ifndef CMIDITEMLABEL_H
#define CMIDITEMLABEL_H

// needed for TRgb and TMargins classes
#include <gdi.h>
// CCoeControl inheritance in class declaration
#include <coecntrl.h>
// using CAknPictographInterface for iPictographInterface
#include <AknPictographInterface.h>
// MAknPictographAnimatorCallBack interface in class declaration
#include <AknPictographDrawerInterface.h>

// using CMIDFont::TDefaultId for iDefaultFontId
#include "CMIDFont.h"

class CEikLabel;

/** This class implements the text in form item labels and the
    content in string items. Its name is therefore not the best one
    as it is more than a label in form items.

    It uses and array of CEikLabel to wrap the given text in more than
    one line. The text wrapping is performed via the AVKON Text utilities.
*/
NONSHARABLE_CLASS(CMIDItemLabel) : public CCoeControl, public MAknPictographAnimatorCallBack
{
public:

    static CMIDItemLabel* NewL(TInt aMaxWidth, TBool aLabelBeforeContent,
    TInt aMaxNumberOfLines = -1,
    const CMIDFont::TDefaultId& aDefaultFontId = CMIDFont::EDefaultLabelId,
    TBool aIsContent = EFalse);

    ~CMIDItemLabel();

    //
    // From CCoeControl
    //
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

    void SetTextL(const TDesC& aText);
    inline const TDesC* Text() const;

    TSize PreferredSize() const;
    TInt PreferredWidth() const;
    void SetWidthL(TInt aLineWidth);

    void SetVariableWidthL(TInt aFirstWidth, TInt aSecondWidth);
    TInt FirstWordWidth() const;

    void SetCentered(TBool aValue);
    void SetUnderlined(TBool aUnderlined);
    void SetColorL(TRgb aColor);
    void SetEmphasisL(TBool aEmphasis);
    TRgb GetDefaultColor();
    void SetFont(const MMIDFont* aFont);
    const CFont* Font() const;
    inline TInt NumLines();
    TInt LineHeight() const;
    CEikLabel* LabelAtIdx(TInt aIdx);
    void SetMaxWidth(TInt aMaxWidth);   // this just sets the new max width (usually form width), no reformatting is actually done

    inline TBool IsContent();
    // Pictograph
    void DrawPictographArea();

    inline void SetMaxNumLines(TInt aMaxNumLines);
    inline TInt MaxNumLines() const;

    void ResolutionChange();
    TInt ItemLabelMargin();
    void AdjustToSizeL(const TSize& aSize);
    void LayoutItemLabel();

private:
    CMIDItemLabel(TInt aMaxWidth, TBool aLabelBeforeContent, TInt aMaxNumberOfLines,
                  const CMIDFont::TDefaultId& aDefaultFontId, TBool aIsContent = EFalse);

    void UpdateMargins();
    void ConstructL();

private:
    //
    // From CCoeControl
    //
    void SizeChanged();

private:
    void WrapTextAndSetSizeL();
    void ResetLabelArray();

    TBool IsLineSeparator(const TText aChar) const;
    TInt LongestLineWidth() const;
    TSize PropperEikLabelMinumumSize(CEikLabel& aLabel) const;

    /** The font used by current layout*/
    void PrepareFont();
private:
    TBool iCentered;

    HBufC* iText; /** The text given by the application */
    HBufC* iWrappedText; /** The text given plus newlines for wrapping and eventual ellipsis*/

    CArrayFixFlat<TPtrC>* iWrappedArray;
    CArrayFixFlat<CEikLabel*>* iLabelArray;

    /** A font assigned java side. If this is null we use the default font id */
    const MMIDFont* iFont;

    /** The default font id, to be used if no font has been given */
    CMIDFont::TDefaultId iDefaultFontId;
    TInt iFontSize;

    TRgb iColor;
    TRgb iDefaultColor;
    TBool iUnderlined;
    TInt iMaxWidth;


    //
    CAknPictographInterface* iPictographInterface;

    /** We can have at most this number of lines, -1 means unlimited */
    TInt iMaxNumberOfLines;

    /** The array containing the width of each line. If it contains less that
    iMaxNumberOfLines elements, then the last width is used for the remaining
    lines. So if this array has only one element, it means all lines have the
    same width. */
    CArrayFixFlat<TInt>* iLineWidthArray;

    TBool iLabelBeforeContent; // EFalse with ImageItem, otherwise ETrue
    TBool iIsContent;      // ETrue if this represents the StringItem content, EFalse otherwise
    TMargins iLabelMargins;

};

inline const TDesC* CMIDItemLabel::Text() const
{
    return(iText);
}

inline TInt CMIDItemLabel::NumLines()
{
    if (iLabelArray)
    {
        return iLabelArray->Count();
    }
    return 0;
}

inline void CMIDItemLabel::SetMaxNumLines(TInt aMaxNumLines)
{
    iMaxNumberOfLines = aMaxNumLines;
}

inline TInt CMIDItemLabel::MaxNumLines() const
{
    return iMaxNumberOfLines;
}

inline TBool CMIDItemLabel::IsContent()
{
    return iIsContent;
}
#endif // CMIDITEMLABEL_H
