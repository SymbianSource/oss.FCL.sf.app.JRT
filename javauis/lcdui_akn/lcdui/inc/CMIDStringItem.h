/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDSTRINGITEM_H
#define CMIDSTRINGITEM_H

#include "CMIDControlItem.h"
#include "lcdui.h"
// using CAknButton API when appearance is MMIDItem::EButton
#include <aknbutton.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif


class CMIDStringButton;

/**
 *  A form string item as defined in MMIDStringItem. Contains a CMIDItemLabel
 *  inherited from CMIDControlItem (the label) and a second CMIDItemLabel
 *  to display the content of the string item.
 */
NONSHARABLE_CLASS(CMIDStringItem) : public CMIDControlItem, public MMIDStringItem
{
public:
    /**
     * Double ctor
     * @param aLabel Label
     * @return MMIDStringItem instance
     *
     * @since s60
     */
    static MMIDStringItem* NewL(
        const TDesC& aLabel, const TDesC& aText, TAppearance aAppearance, CMIDUIManager* aUIManager);

    const TDesC& Text() const;
    MMIDFont* Font() const;
    //
    // From CMIDItem
    //
    TBool IsSelectable() const;
    TPtrC Label() const;
    //
    // From MMIDStringItem
    //
    void SetTextL(const TDesC& aText);
    void SetFontL(MMIDFont* aFont);
    //
    // From MMIDItem
    //
    void SetLabelL(const TDesC& aLabel);
    void SetLayoutL(TLayout aLayout);
    void SetPreferredSizeL(const TSize& aSize);

    TSize MinimumSize() const;
    TSize PreferredSize() const;
    MMIDItem::TAppearance Appearance() const;

    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    void SetDefaultCommand(MMIDCommand* aCommand);
    //
    // From MMIDComponent
    //
    void Dispose();
    //
    // From CCoeControl
    //
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    void ResolutionChange(TInt aType);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void SetContainerWindowL(const CCoeControl& aContainer);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2

    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    //
    // From CMIDControlItem
    //
    void FocusChanged(TDrawNow aDrawNow);
    TRect FocusableRect();
    //
    //
    CMIDItemLabel* StringContentControl() const;
    TBool IsUnconstrainedStringItem();
    TBool WidthOrHeightSpecified() const;
    TBool WidthSpecified() const;
    TBool HeightSpecified() const;
    TSize ResetPreferredSize() const;
    void AdjustToNewWidthL(TInt aWidth);
    TInt NumNewlinesBefore();
    TInt NumNewlinesAfter();
    TSize PreferredSize();
    void AdjustToSizeL(const TSize& aSize);

    /**
     * Gives appearance after taking into account whether the item has commands or not.
     * Focusable items (e.g hyperlinks) are rendered as plain if there are no commands.
     * NOTE: EPlain is dealed as a EHyperlink if it contains command(s)
     * @return Appearance mode
     */
    TAppearance RenderedAppearance() const;

    /**
     * Returns area that is sensitive to tapping actions
     */
    TRect TappingActionRect();

    /**
     * Returns ETrue if CMIDStringItem was didided to several CMIDLabelContainerItem
     * objects due to concatenation.
     * Returns EFalse if CMIDStringItem was added to Form as default item
     */
    TBool IsDivided();
    /**
     * Sets flag iIsDivided to ETrue if CMIDStringItem was didided to several CMIDLabelContainerItem
     * objects due to concatenation.
     * Sets flag iIsDivided to EFalse if CMIDStringItem was added to Form as default item.
     */
    void SetIsDivided(TBool aIsDivided);

private:

    /**
     * Ctor
     * @param aAppearance Appearance
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDStringItem(TAppearance aAppearance, CMIDUIManager* aUIManager);

    void ConstructL(const TDesC& aLabel,const TDesC& aText);
    ~CMIDStringItem();
    //
    void CountNewlinesBeforeAndAfter(const TDesC& aText);

    /**
     * Sets label and content colors according to the appearance mode and focus info
     * NOTE: focusable items (e.g hyperlinks) are rendered as plain if there are no commands.
     * PLAIN:
     * label & content = EAknsCIQsnTextColorsCG6 (text #6 main area, main area texts #215)
     * HYPERLINK - FOCUSED:
     * label & content = EAknsCIQsnTextColorsCG10 (text #10 list highlight, list texts #215)
     * HYPERLINK - NON FOCUSED:
     * label & content = EAknsCIQsnHighlightColorsCG3 (highlight #3 link indication, link text and underline indication #210)
     */
    void SetColorL();

    TRgb BorderColor() const;

    /**
     * Update button size. If buttonSize was not set before (height or width == 0) then first size defaults to minimumSize
     * @param aNewWidth a new width
     */
    void CMIDStringItem::SetButtonWidth(TInt aNewWidth);

    /**
     * Calculates the minimum button size with ellipsis used as text
     */
    void CalculateButtonEllipsedSizeL();

#ifdef RD_TACTILE_FEEDBACK
private:
    MTouchFeedback* iFeedback;
#endif

private:
    TBool iHyperLinkActivated;
    HBufC* iText;
    TAppearance iAppearance;
    MMIDFont* iFont;
    CMIDItemLabel* iContentControl;
    TInt iNumNewlinesBefore;
    TInt iNumNewlinesAfter;
    TGulBorder::TColors iBorderColors;
    CAknButton* iButton;    // used when appearance is MMIDItem::EButton
    TSize iSavedButtonSize; // the latest set button size is kept separately in the variable
    TSize iEllipsedButtonSize; // minimum button size if ellipsis is used instead of text
#ifdef RD_SCALABLE_UI_V2
    TBool iPointerDownOnContentArea; // ETrue, if EButton1Down happened on item content area
#endif //RD_SCALABLE_UI_V2
    // flag detects if string item was divided to label container items
    // due to concatenation of contents contained in adjacent string items
    TBool iIsDivided;

};

#endif // CMIDSTRINGITEM_H
