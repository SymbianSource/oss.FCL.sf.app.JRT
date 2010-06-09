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
* Description:  Implementation class for TextField
*
*/


#include <lafmain.h>
// LAF
// use in GetLayou and DoLayout functions
#include <aknlayoutscalable_avkon.cdl.h>  // LAF

#include "CMIDEdwin.h"
// using API for iTextFieldItem
#include "CMIDTextFieldItem.h"
#include "CMIDTextFieldEdwin.h"

// class CMIDTextFieldEdwin

CMIDTextFieldEdwin* CMIDTextFieldEdwin::NewL(TInt aConstraints,
        const TDesC& aText,
        TInt aMaxSize,
        MMIDDisplayable* aDisplayable,
        CMIDTextFieldItem* aTextFieldItem)
{
    ASSERT(aTextFieldItem); // we expect that it always exist for TextField
    CMIDTextFieldEdwin* textBox = new(ELeave)CMIDTextFieldEdwin(aConstraints, aDisplayable, aTextFieldItem);
    CleanupStack::PushL(textBox);
    textBox->ConstructL(aText,aMaxSize);
    CleanupStack::Pop(textBox);
    return textBox;
}

void CMIDTextFieldEdwin::ConstructL(const TDesC& aText,TInt aMaxSize)
{
    // just call superclass, for now
    CMIDEdwin::ConstructL(aText, aMaxSize);
}


// ----------------------------------------------------------------------------
// CMIDTextFieldEdwin::GetLayout()
// ----------------------------------------------------------------------------
TAknMultiLineTextLayout CMIDTextFieldEdwin::GetLayout(
    TRect& aRect, TInt aNumLines) const
{
    TAknMultiLineTextLayout         edwinLayout;
    TAknTextComponentLayout         textLayout;
    RArray<TAknTextComponentLayout> edwinLayoutLines;

    // leave margins first
    TAknWindowComponentLayout  textPaneLayout =
        AknLayoutScalable_Avkon::form2_midp_text_pane(
            AknLayoutScalable_Avkon::form2_midp_text_pane_ParamLimits().LastRow());

    TAknLayoutRect layRect;
    layRect.LayoutRect(aRect, textPaneLayout.LayoutLine());

    aRect = layRect.Rect();

    for (TInt i = 0; i < aNumLines; i++)
    {
        textLayout = AknLayoutScalable_Avkon::form2_mdip_text_pane_t1(i);

        TRAP_IGNORE(
            edwinLayoutLines.AppendL(textLayout));
    }

    edwinLayout = TAknTextComponentLayout::Multiline(edwinLayoutLines);
    edwinLayoutLines.Reset();

    return edwinLayout;
}


CMIDTextFieldEdwin::CMIDTextFieldEdwin(TUint aConstraints, MMIDDisplayable* aDisplayable, CMIDTextFieldItem* aTextFieldItem)
        : CMIDEdwin(aConstraints, aDisplayable, aTextFieldItem)
{
}

CMIDTextFieldEdwin::~CMIDTextFieldEdwin()
{
    // just calls superclass destructor
}

/**
 * Layouts a TextField.
 *
 * The Edwin control is laid-out in 2 steps:
 * <p>
 * Firstly, we calculate how many lines of text are formatted by
 * layout-ing the Edwin with the maximum lines of text that could be visible on screen.
 * <p>
 * Secondly, we layout the Edwin according to the number of formatted lines.
 * In case there are more lines that could fit on screen, we limit the number of displayed lines
 * to the maximum that can fit on screen.
 *
 * In case of of background formatting mode (e.g. TextLength() >= 1900),
 * the number of formatted lines is unpredictable.
 * We ensure that the number of lines shown is maximum.
 */
void CMIDTextFieldEdwin::DoLayout(const TRect& aRect)
{
    TAknMultiLineTextLayout layout;

    /** The skin text color overrides the text color in LAF. */
    TAknsQsnTextColorsIndex skinTextColor;

    if (iConstraints & MMIDTextField::EUneditable)
    { // skinning colour groups for uneditable text
        skinTextColor = EAknsCIQsnTextColorsCG53;
    }
    else
    { // skinning colour groups for editable text
        skinTextColor = IsFocused() ? EAknsCIQsnTextColorsCG8 : EAknsCIQsnTextColorsCG6;
    }

    TRect parentRect = aRect;

    TAknLayoutScalableParameterLimits paneLimits =
        AknLayoutScalable_Avkon::form2_mdip_text_pane_t1_ParamLimits();

    TInt maxHeightInLines = paneLimits.LastRow();

    SetMaximumHeightInLines(maxHeightInLines);

    // layout edwin with the maximum number of lines first
    // to see how many number of lines will be formatted

    layout = GetLayout(parentRect, maxHeightInLines);   // changes aRect
    AknLayoutUtils::LayoutEdwin(this, parentRect, layout, skinTextColor);

    TInt numLayoutLines;

    if (!IsPartialFormatting())
    {
        // check how many lines are formatted
        // to prepare the multi line layout structure
        TInt numFormattedLines = TextLayout()->NumFormattedLines();

        if (numFormattedLines > maxHeightInLines)
        {
            numLayoutLines = maxHeightInLines; // restrict the number of visible lines

#ifdef RD_SCALABLE_UI_V2
            // set the correct scrolling rect for line by line scrolling
            // when text is longer than displayed
            SetThisScrollRect(this, parentRect, layout, numLayoutLines);
#endif // RD_SCALABLE_UI_V2
        }
        else
        {
            numLayoutLines = numFormattedLines;
        }
    }
    else
    {
        // partial formatting mode
        // the number of formatted lines is unpredictable
        // ensure that the number of lines shown is maximum
        numLayoutLines = maxHeightInLines;
    }

    // previously, the control was laid out showing the maximum number of lines
    // lay out again only if the control should be smaller in height
    if (numLayoutLines < maxHeightInLines)
    {
        // parentRect was changed in the previous GetLayout()
        parentRect = aRect;

        layout = GetLayout(parentRect, numLayoutLines);
        AknLayoutUtils::LayoutEdwin(this, parentRect, layout, skinTextColor);
    }
}

#ifdef RD_SCALABLE_UI_V2
TRect CMIDTextFieldEdwin::LineRect(TRect aParentRect, TInt aLine, TAknMultiLineTextLayout aLayout)
{
    TAknLayoutText text;
    text.LayoutText(aParentRect, aLayout);
    TRect rect = text.TextRect();
    rect.iTl.iY += aLayout.iBaselineSkip * aLine;
    rect.iBr.iY += aLayout.iBaselineSkip * aLine;
    return rect;
}
#endif // RD_SCALABLE_UI_V2

#ifdef RD_SCALABLE_UI_V2
void CMIDTextFieldEdwin::SetThisScrollRect(CMIDTextFieldEdwin *aEdwin, TRect aParentRect, TAknMultiLineTextLayout aLayout, TInt aNumberOfLines)
{
    TRect top = LineRect(aParentRect, 1, aLayout);
    TRect bottom = LineRect(aParentRect, aNumberOfLines-2, aLayout);
    TRect area = TRect(top.iTl, bottom.iBr);
    aEdwin->CEikEdwin::SetScrollRect(area);
}
#endif // RD_SCALABLE_UI_V2

void CMIDTextFieldEdwin::HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
{
    switch (aEventType)
    {
    case MCoeControlObserver::EEventStateChanged:
        iTextFieldItem->HandleControlEventL(aControl, aEventType);
        break;
    case MCoeControlObserver::EEventRequestFocus:
        iTextFieldItem->HandleControlEventL(this, MCoeControlObserver::EEventRequestFocus);
        break;
    default:
        break;
    }

    CMIDEdwin::HandleControlEventL(aControl, aEventType);   // will update iStateChanged if needed
}

void CMIDTextFieldEdwin::PositionChanged()
{
    if (iTextFieldItem->Form())
    {
        TInt newYPos = iTextView->ViewRect().iTl.iY + iTextFieldItem->Form()->ScrollDelta();

        TRect rect;
        rect.SetRect(TPoint(iTextView->ViewRect().iTl.iX, newYPos), iTextView->ViewRect().Size());
        iTextView->SetViewRect(rect);
    }
}

/**
 * Need to recreate TextView here such that it uses the right window
 */
void CMIDTextFieldEdwin::SetContainerWindowL(const CCoeControl& aContainer)
{
    CMIDEdwin::SetContainerWindowL(aContainer);

    if (iTextView)
    {
        delete iTextView;
        iTextView = NULL;

        CreateTextViewL();
    }
}


void CMIDTextFieldEdwin::AddCommandToEdwinL(MMIDCommand& aCommand)
{
    STATIC_CAST(CMIDTextFieldItem*, iTextFieldItem)->AddCommandL(&aCommand);
}
