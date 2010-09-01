/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#include <eiklabel.h>
// AknsUtils used for label and content colors settings
#include <AknsDrawUtils.h>
// skins for drawing
#include <skinlayout.cdl.h>
#include <AknUtils.h>
#include <applayout.cdl.h>
// LAF
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknconsts.h>
#include <avkon.mbg>

#include "CMIDStringItem.h"
// API for iCommandList
#include "CMIDCommandList.h"
// some API for text formating before label settings
#include "CMIDUtils.h"
// CMIDItemLabel* iContentControl
#include "CMIDItemLabel.h"

#include <j2me/jdebug.h>


/** This constant determines the maximum number
of lines for button labels and content */
const TInt KMaxNumLinesForButtons = 10;

/** Content can have unlimited lines */
const TInt KMaxNumLinesForContent = -1;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MMIDStringItem* CMIDStringItem::NewL(
    const TDesC& aLabel, const TDesC& aText, TAppearance aAppearance, CMIDUIManager* aUIManager)
{
    CMIDStringItem* item=new(ELeave) CMIDStringItem(aAppearance, aUIManager);
    CleanupStack::PushL(item);
    item->ConstructL(aLabel,aText);
    CleanupStack::Pop(item);
    return item;
}

/**
 * Assumes label has been set before in order to work out if a prefix is
 * required
 */
void CMIDStringItem::SetTextL(const TDesC& aText)
{
    DEBUG("CMIDStringItem::SetTextL - begin");

    delete iText;
    iText = NULL;

    // count the newlines at the beginning and the end of the content
    CountNewlinesBeforeAndAfter(aText);

    // create a TPtrC of the string without the beginning and training newlines
    TPtrC ptr;
    if (iNumNewlinesBefore < aText.Length())
    {
        ptr.Set(aText.Mid(iNumNewlinesBefore, aText.Length() -
                          (iNumNewlinesBefore + iNumNewlinesAfter)));
    }

    iText = ptr.AllocL();
    if (iText->Length() != 0)
    {
        CMIDUtils::MapJavaToETextChars(iText);
    }

    iContentControl->SetTextL(*iText);
    if (iAppearance == MMIDItem::EButton)
    {
        // leave only text before line separator, if any
        TInt i = 0;
        while ((i < iText->Length()) && (!CMIDUtils::IsLineSeparator((*iText)[i])))
        {
            i++;
        }
        ptr.Set(iText->Mid(0, i));

        // if there was line separator add ellipis in the end
        HBufC* textWithEllipsis = HBufC::NewLC(i + 1);
        textWithEllipsis->Des().Append(ptr); // text without line separators
        if (i < iText->Length())
        {
            // if there was line separator add ellipsis in the end
            textWithEllipsis->Des().Append(KEllipsis);
        }

        iButton->State()->SetTextL(textWithEllipsis->Des());

        CleanupStack::PopAndDestroy(textWithEllipsis);
    }

    if (iForm)
    {
        iForm->RequestLayoutL();
    }
    DEBUG("CMIDStringItem::SetTextL - end");
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDStringItem::CMIDStringItem(TAppearance aAppearance, CMIDUIManager* aUIManager)
        : CMIDControlItem(EDefault, aUIManager),
        iAppearance(aAppearance),
        iIsDivided(EFalse)
{
    iMMidItem = this;
}

void CMIDStringItem::ConstructL(const TDesC& aLabel,const TDesC& aText)
{
    DEBUG("CMIDStringItem::ConstructL - begin");
    // Initializes the label control
    CMIDControlItem::ConstructL();

    if (iAppearance == MMIDItem::EButton)
    { //buttons can only have one line

        iButton = CAknButton::NewL(NULL,   // aIcon
                                   NULL,  // aDimmedIcon
                                   NULL,  // aPressedIcon
                                   NULL,  // aHoverIcon
                                   aText,
                                   KNullDesC,
                                   KAknButtonSizeFitText, // iFlags set to respect text width
                                   0);
        iLabelControl->SetMaxNumLines(KMaxNumLinesForButtons);
    }
    // iContentControl is always created because RenderedAppearance() can be PLAIN
    iContentControl = CMIDItemLabel::NewL(FormClientAreaWidth(), ETrue,
                                          KMaxNumLinesForContent, CMIDFont::EDefaultTextId, ETrue);


    SetLabelL(aLabel);
    SetTextL(aText); // Sets CMIDStringitem::iText
    iContentControl->SetFont(Font());
    if (iAppearance != MMIDItem::EButton)
    {
        MMIDFont* font = Font();
        if (font)
        {
            iButton->SetTextFont(font->Font());
        }
    }
    SetColorL();

    if (iAppearance == MMIDItem::EButton)
    {
        CalculateButtonEllipsedSizeL();
    }
    ResetPreferredSize();

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif

    DEBUG("CMIDStringItem::ConstructL - end");
}

CMIDStringItem::~CMIDStringItem()
{
    delete iText;
    delete iContentControl;
    delete iButton;
}

void CMIDStringItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    CMIDControlItem::SetContainerWindowL(aContainer);
    iContentControl->SetContainerWindowL(*this);
    if (iAppearance == MMIDItem::EButton)
    {
        iButton->SetContainerWindowL(*this);
    }
    ActivateL();
}

void CMIDStringItem::CountNewlinesBeforeAndAfter(const TDesC& aText)
{
    TInt i = 0;

    iNumNewlinesBefore = 0;
    iNumNewlinesAfter = 0;
    while ((i < aText.Length()) && (aText[i] == '\n'))
    {
        iNumNewlinesBefore++;
        i++;
    }

    if (iNumNewlinesBefore == aText.Length())
    {
        return;
    }

    // we get here if the string isn't all newlines
    i = aText.Length() - 1;
    while ((i >= 0) && (aText[i] == '\n'))
    {
        iNumNewlinesAfter++;
        i--;
    }
}

TBool CMIDStringItem::IsSelectable() const
{
    return (iCommandList->Count() > 0);
}

/**
 * Sets the preferred width and height for this Item. If the width is between zero and
 * the minimum width, inclusive, the minimum width is used. If the height is between
 * zero and the minimum height, inclusive, the minimum height is used.
 * By default, the preferred width and height are based entirely on the Item's contents.
 * If both width and height parameters are -1, the default values for width and height
 * are established. If neither width nor height is -1, the values specified become the
 * Item's preferred size.
 *
 * If width is -1 and height is a legal value, the width will be computed based on the
 * item's contents and the given height. If height is -1 and width is a legal value,
 * the height will be computed based on the item's contents and the given width.
 *
 * If a StringItem has been assigned a preferred width or height by
 * the application, it is wrapped to fit that width and height and
 * is treated as a rectangle whose minimum and preferred width and height
 * are the width and height of this rectangle
 *
 * iPreferredSize includes margins. Margins are added in CMIDItemLabel class.
 */
void CMIDStringItem::SetPreferredSizeL(const TSize& aSize)
{

    iRequestedPreferredSize = CheckRequestedSize(aSize);
    iPreferredSize = iRequestedPreferredSize;

    if (iPreferredSize.iWidth < 0)
    { // width is not specified
        if (RenderedAppearance() != MMIDItem::EButton)
        {
            iPreferredSize.iWidth = Max(iLabelControl->PreferredWidth(),
                                        iContentControl->PreferredWidth());
        }
        else
        {
            iPreferredSize.iWidth = Max(iLabelControl->PreferredWidth(),
                                        iButton->MinimumSize().iWidth);
            // add margins
            TAknWindowLineLayout layout =
                AknLayoutScalable_Avkon::form2_midp_label_pane_cp(0).LayoutLine();
            TInt leftMargin = layout.il;
            TInt rightMargin = layout.ir;
            iPreferredSize.iWidth += leftMargin + rightMargin; // add left and right margins
        }
    }

    TSize minimumSize = MinimumSize();

    // make sure the width doesn't get bigger than the form width
    iPreferredSize.iWidth = Min(iPreferredSize.iWidth, FormClientAreaWidth());

    // make sure the width doesn't get smaller than the minimum width
    iPreferredSize.iWidth = Max(iPreferredSize.iWidth, minimumSize.iWidth);

    // preferred width is set to label and content
    if (iLabelControl && iLabelControl->Text()->Length() > 0)
    {
        iLabelControl->SetWidthL(iPreferredSize.iWidth);
    }
    if (RenderedAppearance() != MMIDItem::EButton)
    {
        iContentControl->SetWidthL(iPreferredSize.iWidth);
    }
    else
    {
        SetButtonWidth(iPreferredSize.iWidth);
    }

    if (iPreferredSize.iHeight < 0)
    {  // height is not specified, do text wrapping (call SetWidthL)
        // so that we can then calculate the height
        iPreferredSize.iHeight = LabelHeight() + ItemPreferredHeightWithoutLabel();
    }

    //make sure the preferred size is not smaller than the minimum size
    iPreferredSize.iHeight = Max(iPreferredSize.iHeight, minimumSize.iHeight);

    if ((minimumSize.iWidth == 0) && (minimumSize.iHeight == 0))
    { //it means there is no control and no label, so set our size to null
        iPreferredSize.iWidth = 0;
        iPreferredSize.iHeight = 0;
    }

    AdjustToSizeL(iPreferredSize);
}

TSize CMIDStringItem::MinimumSize() const
{
    TInt numNewLines = iNumNewlinesBefore + iNumNewlinesAfter;
    TBool labelNotEmpty = (iLabelControl->Text()->Length() > 0);

    TInt width;
    TInt height;
    if (RenderedAppearance() != MMIDItem::EButton)
    {
        // if no label and no content then size is zero
        if (!labelNotEmpty  &&
                ((iContentControl->Text()->Length() == 0)) && (numNewLines == 0))
        {
            return TSize(0, 0);
        }

        width = Max(iLabelControl->MinimumSize().iWidth, iContentControl->MinimumSize().iWidth);
        height = (
                     (iLabelControl && iLabelControl->Text()->Length() > 0) ?
                     OneLineLabelHeight() : 0) +
                 (iContentControl ?
                  iContentControl->LineHeight() + iContentControl->ItemLabelMargin() : 0) +
                 ItemContentBottomMargin();

        if (width > FormClientAreaWidth())
        {
            width = FormClientAreaWidth();
        }
    }
    else
    {
        // if no label and no content then size is zero
        if (!labelNotEmpty
                &&((iButton->State()->Text().Length() == 0))
                && (numNewLines == 0))
        {
            return TSize(0, 0);
        }

        TInt width = iEllipsedButtonSize.iWidth;
        TInt height = iEllipsedButtonSize.iHeight;

        // add margins
        TAknWindowLineLayout layout =
            AknLayoutScalable_Avkon::form2_midp_label_pane_cp(0).LayoutLine();
        TInt leftMargin = layout.il;
        TInt rightMargin = layout.ir;

        width = Max(iLabelControl->MinimumSize().iWidth, width);
        width += leftMargin + rightMargin; // add left and right margins

        if (labelNotEmpty)
        {
            height += OneLineLabelHeight() + ItemContentBottomMargin(); // add label and bottom margin
        }
        else
        {
            height += ItemContentBottomMargin(); // use the value for top margin
        }

        if (width > FormClientAreaWidth())
        {
            width = FormClientAreaWidth();
        }
        return TSize(width, height);
    }
    return TSize(width, height);
}

#ifdef RD_SCALABLE_UI_V2
void CMIDStringItem::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        // check if string item is a hyperlink and colour the link according to activation/deactivation with pointer
        // note that want to colour the link and have tactile feedback even though have long tap
        if (RenderedAppearance() == EHyperLink)
        {
            if (aPointerEvent.iType == TPointerEvent::EButton1Down)
            {
                if (!HasLabel() || TappingActionRect().Contains(aPointerEvent.iPosition) ||
                        iForm->StringItemContainsPoint(this, aPointerEvent.iPosition))
                {
                    iHyperLinkActivated = ETrue;
#ifdef RD_TACTILE_FEEDBACK
                    // RenderedAppearance returns EHyperlink for initially hyperlink-created StringItem,
                    // or for plain-created StringItem with commands added later.
                    // if focus is changing, tactile feedback is given already in Form
                    if (!iForm->IsFocusChangingWithPen())
                    {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                        //Feedback for HYPERLINK only on touch down
                        iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
#else
                        iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                    }
#endif // RD_TACTILE_FEEDBACK
                }
                else // tap on label
                {
                    iHyperLinkActivated = EFalse;
                }
                FocusChanged(EDrawNow);
            }
        }

        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            iPointerDownOnContentArea = iForm->StringItemContainsPoint(
                                            this, aPointerEvent.iPosition);
        }

        // pass the event to the button
        if (iButton)
        {

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            if (aPointerEvent.iType == TPointerEvent::EButton1Down &&
                    iForm->IsFocusChangingWithPen() &&
                    !(!HasLabel() || TappingActionRect().Contains(aPointerEvent.iPosition) ||
                      iForm->StringItemContainsPoint(this, aPointerEvent.iPosition)))
            {
                //On touch down, when focus is changing to this item and user
                //tapped to label (if exists), it should give sesitive list feedback
                iFeedback->InstantFeedback(ETouchFeedbackSensitiveList);
            }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

            CCoeControl::HandlePointerEventL(aPointerEvent);
        }

        TBool consumed = iForm->TryDetectLongTapL(aPointerEvent);
        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            // always remove hyperlink activated colour
            iHyperLinkActivated = EFalse;
            if (!consumed)
            {
                if (!iButton)
                {
                    CMIDControlItem::HandlePointerEventL(aPointerEvent);
                }
                // check if the pen was dragged out of button and that pointer down has happened on content area,
                // i.e. dragging pointer from label to content area should not invoke any action.
                if (!iForm->PhysicsScrolling() && (iForm->StringItemContainsPoint(this, aPointerEvent.iPosition) &&
                                                   iPointerDownOnContentArea))
                {
                    iForm->HandleTouchControlEventL(this, MCoeControlObserver::EEventStateChanged);
                }
            }
            FocusChanged(EDrawNow);
        }
    }
}
#endif // RD_SCALABLE_UI_V2

/** Recalculate the preferred size.*/
TSize CMIDStringItem::ResetPreferredSize() const
{
    CMIDStringItem* self = const_cast<CMIDStringItem*>(this);
    TRAP_IGNORE(self->SetPreferredSizeL(iRequestedPreferredSize));
    return iPreferredSize;
}

/** @see CMIDControlItem::AdjustToNewWidthL()
* Margins are added in CMIDItemLabel class */
void CMIDStringItem::AdjustToNewWidthL(TInt aWidth)
{
    ASSERT(aWidth >= 0);

    if (aWidth != iPreferredSize.iWidth)
    {
        if (RenderedAppearance() != MMIDItem::EButton)
        {
            iContentControl->SetWidthL(aWidth);
        }
        else
        {
            SetButtonWidth(aWidth);
        }
    }
    CMIDControlItem::AdjustToNewWidthL(aWidth);
}


/** This method is never called by C++ side, only java side. CMIDItem::PreferredSize() is
called instead, which returns the minimum between iPreferredSize and the minimum size.
*/
TSize CMIDStringItem::PreferredSize() const
{
    return iPreferredSize;
}

// return Item appearance
MMIDItem::TAppearance CMIDStringItem::Appearance() const
{
    return iAppearance;
}

/** This method is never called by C++ side, only java side. CMIDItem::PreferredSize() is
called instead, which returns the minimum between iPreferredSize and the minimum size.
*/
TSize CMIDStringItem::PreferredSize()
{
    return iPreferredSize;
}

// We display a string as a hyperlink or a button depending on both its appearance and if it has any commands
void CMIDStringItem::AddCommandL(MMIDCommand* aCommand)
{
    DEBUG("CMIDStringItem::AddCommandL - begin");
    CMIDItem::AddCommandL(aCommand);

    TBool underlined = (iFont ? iFont->IsUnderlined() : EFalse) ||
                       (RenderedAppearance() == EHyperLink);

    iContentControl->SetUnderlined(underlined);
    if (iForm)
    {
        iForm->RequestLayoutL();
    }
    if (RenderedAppearance() == MMIDItem::EButton)
    {
        iButton->SetTextUnderlineStyle((underlined?EUnderlineOn:EUnderlineOff));
    }
    DEBUG("CMIDStringItem::AddCommandL - end");
}

// We display a string as a hyperlink or a button depending on both its appearance and if it has any commands
void CMIDStringItem::RemoveCommand(MMIDCommand* aCommand)
{
    DEBUG("CMIDStringItem::RemoveCommand - begin");

    CMIDItem::RemoveCommand(aCommand);

    TBool underlined = (iFont ? iFont->IsUnderlined() : EFalse) ||
                       (RenderedAppearance() == EHyperLink);

    iContentControl->SetUnderlined(underlined);
    if (iForm)
    {
        TRAP_IGNORE(iForm->RequestLayoutL());
    }
    if (RenderedAppearance() == MMIDItem::EButton)
    {
        iButton->SetTextUnderlineStyle((underlined?EUnderlineOn:EUnderlineOff));
    }
    DEBUG("CMIDStringItem::RemoveCommand - end");
}

void CMIDStringItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    DEBUG("CMIDStringItem::SetDefaultCommand - begin");
    CMIDItem::SetDefaultCommand(aCommand);
    DEBUG("CMIDStringItem::SetDefaultCommand - end");
}

void CMIDStringItem::SetFontL(MMIDFont* aFont)
{
    iFont = aFont;

    iContentControl->SetFont(iFont);

    TBool underlined = (iFont ? iFont->IsUnderlined() : EFalse) ||
                       (RenderedAppearance() == EHyperLink);

    iContentControl->SetUnderlined(underlined);

    if (iAppearance == MMIDItem::EButton)
    {
        iButton->SetTextFont(aFont->Font());
        iButton->SetTextUnderlineStyle((underlined?EUnderlineOn:EUnderlineOff));
        CalculateButtonEllipsedSizeL();
    }
    ResetPreferredSize();
}

/**
 * Returns the label WITH the prefix if it's not a control, and WITHOUT if it is.
 */
TPtrC CMIDStringItem::Label() const
{
    return *iLabel;
}

const TDesC& CMIDStringItem::Text() const
{
    return *iText;
}

/**
    Because MMIDItem declares a pure virtual const method called MinimumSize()
    whilst CCoeControl has a virtual non const method called MinimumSize()
    we are forced to make sure these two methods behave in the same way
    in every item and hence the non cost method calls the const method.

    We cannot do this once and for all in CMIDControlItem because only
    concrete items inherit from the MMID LCDUI framework classes:

        CCoeControl                 MMIDItem
            |                           |
        CMIDItem                  MMIDStringItem
            |                           |
      CMIDControlItem                   |
            |                           |
       CMISStringItem--------------------

    This is true for every other item.
*/
TSize CMIDStringItem::MinimumSize()
{
    const CMIDStringItem* self = const_cast<const CMIDStringItem*>(this);

    return self->MinimumSize();
}

TInt CMIDStringItem::CountComponentControls() const
{
    return 2;
}

CCoeControl* CMIDStringItem::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iLabelControl;
    case 1:
        if (RenderedAppearance() != MMIDItem::EButton)
        {
            return iContentControl;
        }
        else
        {
            return iButton;
        }
    }
    return NULL;
}

/**
 * Sets label and content colors according to the appearance mode and focusing info
 */
void CMIDStringItem::SetColorL()
{
    DEBUG("CMIDStringItem::SetColorL - begin");
    // Create & initialize a color variable according to the focus & appearance info
    TBool focused = IsFocused();
    TRgb labelColor;
    TRgb contentColor;
    TRgb color;
    // Set colors in case of plain
    // NOTE: All items (e.g hyperlinks) are rendered as plain if there are no commands.
    if (RenderedAppearance() == MMIDItem::EPlain)
    {
        // The appearance mode is the EPlain.
        DEBUG("CMIDStringItem::SetColorL - EPlain");
        // Same color - no matter if focused or not
        AknsUtils::GetCachedColor(
            AknsUtils::SkinInstance(), color,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
        labelColor = color;
        contentColor = color;
    }
    else
    {
        // The appearance mode is EHyperLink or EButton
        DEBUG("CMIDStringItem::SetColorL - not EPlain");
        if (focused)
        {
            // Item is focused
            // If item is focused, label is always focused,
            AknsUtils::GetCachedColor(
                AknsUtils::SkinInstance(), labelColor,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10);

            if (RenderedAppearance() == MMIDItem::EHyperLink)
            {
                if (iHyperLinkActivated)
                    // link was activated
                {
                    AknsUtils::GetCachedColor(
                        AknsUtils::SkinInstance(), contentColor,
                        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10);
                }
                else
                    // link was not activated
                {
                    AknsUtils::GetCachedColor(
                        AknsUtils::SkinInstance(), contentColor,
                        KAknsIIDQsnTextColors, EAknsCIQsnHighlightColorsCG3);
                }
            }
        }
        else
        {
            // Item is not focused
            AknsUtils::GetCachedColor(
                AknsUtils::SkinInstance(), labelColor,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
            AknsUtils::GetCachedColor(
                AknsUtils::SkinInstance(), contentColor,
                KAknsIIDQsnHighlightColors, EAknsCIQsnHighlightColorsCG3);
        }

        if (RenderedAppearance() == MMIDItem::EButton)
        {
            // Note: BorderColor() is the color of the border internal background, not the outline
            ColorUtils::GetRgbDerivedBorderColors(
                iBorderColors, BorderColor(), iEikonEnv->DefaultDisplayMode());
        }
    }
    // Set content & label colors
    if (RenderedAppearance() != MMIDItem::EButton)
    {
        iContentControl->SetColorL(contentColor);
    }
    iLabelControl->SetColorL(labelColor);

    // Text color for CAknButton has to be set
    if (RenderedAppearance() == MMIDItem::EButton)
    {
        if (iButton)
        {
            iButton->SetTextColorIds(
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
        }
    }

    DEBUG("CMIDStringItem::SetColorL - end");
}


/** The color for the border background in buttons.
*/
TRgb CMIDStringItem::BorderColor() const
{
    DEBUG("CMIDStringItem::BorderColor - begin");

    TRgb color;
    AknsUtils::GetCachedColor(
        AknsUtils::SkinInstance(), color,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG12);
    DEBUG("CMIDStringItem::BorderColor - end");
    return color;
}

/**  Draw a frame (border) for buttons and then draw the grid highlight frame if
we have focus. Note: only hyperlinks and buttons can have focus.  */
void CMIDStringItem::Draw(const TRect& aRect) const
{
    DEBUG("CMIDStringItem::Draw - begin");

    if (RenderedAppearance() == MMIDItem::EButton)
    {
        // To avoid clipping button's border to the form's client area
        // we pass entire button's rectangle - Rect() - as parameter
        Border().Draw(SystemGc(), Rect(), iBorderColors);
    }

    if (IsFocused())
    { //only buttons or hyperlinks
        DEBUG("CMIDStringItem::Draw - focused");

        // A focused item e.g. hyperlink
        TAknLayoutRect topLeft;
        topLeft.LayoutRect(aRect,
                           SkinLayout::List_highlight_skin_placing__apps_specific__Line_2());

        TAknLayoutRect bottomRight;
        bottomRight.LayoutRect(aRect,
                               SkinLayout::List_highlight_skin_placing__apps_specific__Line_5());

        TRect outerRect = TRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
        TRect innerRect = TRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);

        AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(), SystemGc(),
                                 outerRect, innerRect,
                                 KAknsIIDQsnFrList, KAknsIIDQsnFrListCenter);
    }
    DEBUG("CMIDStringItem::Draw - end");
}

void CMIDStringItem::SizeChanged()
{
    /* Margins are added in CMIDItemLabel class */
    // If rects of label and content are layouted according to parent rect
    // their width should be the same as parent. So text within labels is
    // placed to proper side: to left in left-to right and to right in
    // right-to-left variants.
    iLabelControl->SetExtent(Position(),
                             TSize(Size().iWidth, iLabelControl->Size().iHeight));
    iContentControl->SetExtent(Position() + TPoint(0, LabelHeight()),
                               TSize(Size().iWidth, iContentControl->Size().iHeight));

    if (RenderedAppearance() == MMIDItem::EButton)
    {
        TBool labelEmpty = (iLabelControl->Text()->Length() == 0);

        TAknWindowLineLayout layout =
            AknLayoutScalable_Avkon::form2_midp_label_pane_cp(0).LayoutLine();

        TInt leftMargin = layout.il;
        TInt rightMargin = layout.ir;
        TInt topMargin = ItemContentBottomMargin();

        TPoint buttonPos = Position() + TPoint(0, LabelHeight());
        TSize buttonSize = iSavedButtonSize;

        buttonPos += TPoint(leftMargin, 0);
        buttonSize -= TSize(leftMargin + rightMargin, 0); // these margins were added in MinimumSize()

        if (labelEmpty)
        {
            buttonPos += TPoint(0, topMargin); // shift down, we need some space above
        }

        iButton->SetExtent(buttonPos, buttonSize);
    }

    CMIDControlItem::SizeChanged();
}

TKeyResponse CMIDStringItem::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
    return EKeyWasNotConsumed;
}

// this assumes that the item has been formatted by the form
TInt CMIDStringItem::ItemPreferredHeightWithoutLabel()
{
    if (iAppearance != MMIDItem::EButton)
    {
        return iContentControl->Size().iHeight + ItemContentBottomMargin();
    }
    else
    {
        TInt height =  iButton->Size().iHeight + ItemContentBottomMargin();
        TBool labelEmpty = (iLabelControl->Text()->Length() == 0);
        if (labelEmpty)
        {
            height += ItemContentBottomMargin(); // add top margin
        }
        return height;
    }
}

TRect CMIDStringItem::FocusableRect()
{
    TRect rect = Rect();

    if (rect.iTl.iY > rect.iBr.iY)
    {
        TInt tmp = rect.iBr.iY;
        rect.iBr.iY = rect.iTl.iY;
        rect.iTl.iY = tmp;
    }

    return rect;
}



/** Set text colors according to the focus: label color stays
the same as all other items (currently text entry), see CMIDControlItem::SetLabelColor().
Content colors is either NormalColor() or HighlightColor() according to focus.
Note: only hyperlinks and buttons can have focus. Currently NormalColor() is text
entry color (same as labels) whilst HighlightColor() is grid highlight text color. */
void CMIDStringItem::FocusChanged(TDrawNow aDrawNow)
{
    DEBUG("CMIDStringItem::FocusChanged - begin");

    TRAP_IGNORE(SetColorL());

    if (EDrawNow == aDrawNow && DrawableWindow())
    {
        if (!HasLabel() && RenderedAppearance() == EHyperLink)
        {
            // Have to do a call back to Form to draw a hyperlink without label correctly.
            // Note: a hyperlink without label is a CMIDLabelContainerItem and not a CMIDStringItem
            // Hyperlink without label is created by Form and this string item has no knowledge of it.
            // CMIDStringItem FocusChanged() is called when focus changes for drawing all kinds of string items even though
            // they are not necessarily drawn here. See CMIDLabelContainerItem too.
            iForm->DrawNow();
        }
        else
        {
            if (iIsDivided)
            {
                // For correct drawing of StringItem with label which is divided
                // to CMIDLabelContainerItem objects due to concatenation of contents contained
                // in adjacent string items.
                iForm->DrawNow();
            }
            else
            {
                DrawNow();
            }
        }
    }
    DEBUG("CMIDStringItem::FocusChanged - end");
}

MMIDItem::TAppearance CMIDStringItem::RenderedAppearance() const
{
    TBool hasCommands(CommandList()->Count() > 0);
    if (hasCommands)
    {
        return (iAppearance == EButton) ? EButton : EHyperLink;
    }
    return EPlain;
}

CMIDItemLabel* CMIDStringItem::StringContentControl() const
{
    return iContentControl;
}

TBool CMIDStringItem::IsUnconstrainedStringItem()
{
    return !(WidthOrHeightSpecified() || (RenderedAppearance() == EButton));
}

TBool CMIDStringItem::WidthOrHeightSpecified() const
{
    return WidthSpecified() || HeightSpecified();
}

TBool CMIDStringItem::WidthSpecified() const
{
    return (iRequestedPreferredSize.iWidth != -1);
}

TBool CMIDStringItem::HeightSpecified() const
{
    return (iRequestedPreferredSize.iHeight != -1);
}

/**
  Make sure label and content do not go beyond the available size. This method
  is called after setting the preferred size and when the row needs to size
  the items. If the item height (available height) cannot fit label and content
  then fit the label as much as possible and fit the control with the
  remaining height. However, reserve to the control at least one line. Because
  the minimumsize is one line for label (if available) and one line for control
  then there should always be space at least for one label line and for one control line.
*/
void CMIDStringItem::AdjustToSizeL(const TSize& aSize)
{
    if (iAppearance != MMIDItem::EButton)
    {
        TInt availableHeight = aSize.iHeight;
        TInt requestedHeight = iContentControl->Size().iHeight;

        if (iLabelControl && iLabelControl->Text()->Length() > 0)
        {
            requestedHeight += LabelHeight();
        }

        if (requestedHeight > availableHeight)
        {// label + control do not fit
            if (iLabelControl && iLabelControl->Text()->Length() > 0)
            {
                //reserve one line to the control
                TInt heightForLabel = iContentControl->Text()->Length() > 0 ?
                                      availableHeight - iContentControl->LineHeight() - iContentControl->ItemLabelMargin() :
                                      availableHeight;

                if (iLabelControl->Size().iHeight > heightForLabel)
                { //label does not fit

                    //By setting a temporary max number of lines and then calling
                    //SetWidthL() we limit the number of lines to the temporary max number
                    //However then the max number must be resetted
                    TInt oldMaxNumLabelLines = iLabelControl->MaxNumLines();
                    iLabelControl->SetMaxNumLines(
                        (heightForLabel - iLabelControl->ItemLabelMargin())/ iLabelControl->LineHeight());
                    iLabelControl->SetWidthL(aSize.iWidth);
                    iLabelControl->SetMaxNumLines(oldMaxNumLabelLines);
                }

                //height available for the control
                availableHeight -= iLabelControl->Size().iHeight;
            }

            TInt oldMaxNumContentLines = iContentControl->MaxNumLines();
            iContentControl->SetMaxNumLines(
                (availableHeight - iLabelControl->ItemLabelMargin()) / iContentControl->LineHeight());
            iContentControl->SetWidthL(aSize.iWidth);
            iContentControl->SetMaxNumLines(oldMaxNumContentLines);
        }
    }
    else
    {
        TInt availableHeight = aSize.iHeight;
        TInt requestedHeight = iButton->Size().iHeight;

        if (iLabelControl && iLabelControl->Text()->Length() > 0)
        {
            requestedHeight += LabelHeight();
        }

        if (requestedHeight > availableHeight)
        {// label + control do not fit
            if (iLabelControl && iLabelControl->Text()->Length() > 0)
            {
                //reserve one line to the control
                TInt heightForLabel = iButton->Size().iHeight;

                if (iLabelControl->Size().iHeight > heightForLabel)
                { //label does not fit

                    //By setting a temporary max number of lines and then calling
                    //SetWidthL() we limit the number of lines to the temporary max number
                    //However then the max number must be resetted
                    TInt oldMaxNumLabelLines = iLabelControl->MaxNumLines();
                    iLabelControl->SetMaxNumLines(
                        (heightForLabel - iLabelControl->ItemLabelMargin())/ iLabelControl->LineHeight());
                    iLabelControl->SetWidthL(aSize.iWidth);
                    iLabelControl->SetMaxNumLines(oldMaxNumLabelLines);
                }
            }

            SetButtonWidth(aSize.iWidth);
        }
    }
}


void CMIDStringItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
}

void CMIDStringItem::SetLayoutL(TLayout aLayout)
{
    CMIDItem::SetLayoutL(aLayout);
}

void CMIDStringItem::Dispose()
{
    delete this;
}

MMIDFont* CMIDStringItem::Font() const
{
    return iFont;
}

TInt CMIDStringItem::NumNewlinesBefore()
{
    return iNumNewlinesBefore;
}

TInt CMIDStringItem::NumNewlinesAfter()
{
    return iNumNewlinesAfter;
}

void CMIDStringItem::SetButtonWidth(TInt aNewWidth)
{
    TSize buttonSize = iButton->Size();
    if (!buttonSize.iWidth || !buttonSize.iHeight)
    {
        buttonSize = iButton->MinimumSize();
    }
    buttonSize.iWidth = aNewWidth;
    iButton->SetSize(buttonSize);
    iSavedButtonSize = buttonSize;
}

//
// The area sensitive to actions is contentControl label for hyperlink and button for button appearance
//
TRect CMIDStringItem::TappingActionRect()
{
    if (iAppearance != MMIDItem::EButton)
    {
        return iContentControl->Rect();
    }
    else
    {
        return iButton->Rect();
    }
}

void CMIDStringItem::ResolutionChange(TInt /*aType*/)
{
    iContentControl->SetMaxWidth(FormClientAreaWidth());
    iLabelControl->SetMaxWidth(FormClientAreaWidth());
}

void CMIDStringItem::CalculateButtonEllipsedSizeL()
{
    ASSERT(iAppearance == MMIDItem::EButton);

    TBuf<1> ellipsis;
    ellipsis.Append(KEllipsis);

    CAknButton* tempButton = CAknButton::NewLC(NULL,   // aIcon
                             NULL,  // aDimmedIcon
                             NULL,  // aPressedIcon
                             NULL,  // aHoverIcon
                             ellipsis,
                             KNullDesC,
                             KAknButtonSizeFitText, // iFlags set to respect text width
                             0);

    iEllipsedButtonSize = tempButton->MinimumSize();
    CleanupStack::PopAndDestroy(tempButton);
}

TBool CMIDStringItem::IsDivided()
{
    return iIsDivided;
}

void CMIDStringItem::SetIsDivided(TBool aIsDivided)
{
    iIsDivided = aIsDivided;
}

