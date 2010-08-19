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


#ifndef SWTBUTTON_H
#define SWTBUTTON_H


#include <AknIconUtils.h>
#include "swtcontrolbase.h"


class CSwtButtonProxy;
class CSwtFont;
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

/**
 * CSwtButton
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtButton)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtButton
        , public MCoeControlObserver
{
// Own methods
public:
    /**
     * Two phase constructor
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     * @return pointer to an object of this class
     */
    static CSwtButton* NewL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle);

protected:
    /**
     * First phase constructor.
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     */
    CSwtButton(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle);

    /**
     * Destructor
     */
    ~CSwtButton();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:
    /**
     * Create button with a text.
     * @param aText the text to set.
     */
    void CreateButtonL(const TDesC& aText);

    /**
     * Change the button state (selected or not).
     * @param aState the new state.
     */
    void ChangeStateL(TBool aState);

    /**
     * Create the button icon (selected or not, inactive)
     * according with its state.
     * @param aState the button state.
     * @param aStyle the button style (Only can be check or radio).
     * @return pointer to icon
     */
    CGulIcon* CreateCheckOrRadioIconL(TBool aState, TInt aStyle) const;

    /**
     * Change the button icon (for check or radio styles) according with its state.
     * @param aState the button state.
     */
    void ChangeCheckOrRadioIconL(TBool aState);

    /**
     * Set size on a icon.
     * @param aIcon The icon.
     * @param aSize The size.
     * @param aScaleMode The applied scale mode.
     */
    void SetIconSize(
        const CGulIcon* aIcon,
        const TSize& aSize,
        TScaleMode aScaleMode = EAspectRatioPreserved) const;

    /**
     * Get the size of icon for CHECK or RADIO style.
     * @param aSize the returned size of icon.
     */
    void GetSizeOfCheckOrRadioIcon(TSize& aSize) const;

    /**
     * Update the control size.
     */
    void UpdateControlSize();

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

    /**
     * Update default font.
     */
    void RetrieveDefaultFontL();

    void ProcessFontUpdate();

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    /**
     * Returns ETrue when tactile feedback is needed on touch down
     */
    TBool IsFeedbackOnTouchDownNeeded(
        const TPointerEvent& aPointerEvent) const;

    /**
     * Returns ETrue when tactile feedback is needed on touch release
     */
    TBool IsFeedbackOnTouchReleaseNeeded(
        const TPointerEvent& aPointerEvent) const;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    void UpdateTextColor();

// From CCoeControl
public:
    CCoeControl* ComponentControl(TInt aIndex) const;
    TInt CountComponentControls() const;
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandleResourceChange(TInt aType);

protected:
    void PositionChanged();
    void SizeChanged();
    void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From ASwtControlBase
public:
    void SetFontL(const MSwtFont* aFont);
    void SetForegroundL(const MSwtColor* aColor);
    HBufC* MSKLabelL() const;
protected:
    const MSwtFont& DefaultFont() const;

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    TBool IsKeyUsed(TUint aKeyCode) const;
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    MSwtButton* ButtonInterface();
    TInt FocusBackgroundPolicy() const;
    TInt PressBackgroundPolicy() const;
    void HandleHighlightChange();

// From MSwtButton
public:
    MSwtControl* Control();
    TBool GetSelection() const;
    void SetSelection(TBool aState);
    TInt GetAlignment() const;
    void SetAlignment(TInt aAlignment);
    void SetImageL(const MSwtImage* aImage);
    void SetTextL(const TDesC& aText);
    void SetDefault(TBool aDefault);
    TBool IsDefault() const;
    void ClickL();

// From MCoeControlObserver
public:
    void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

// Data
private:
    /**
     * The button.
     * Own
     */
    CSwtButtonProxy* iButton;

    /**
     * The button image. May be NULL.
     * Own
     */
    const MSwtImage* iImage;

    /**
     * The default font of text (NULL if no text set).
     * Own
     */
    MSwtFont* iDefaultFont;

    /**
     * Informs if the button is set to default.
     */
    TBool iIsDefault;

    /**
     * Informs if the button is selected.
     */
    TBool iSelected;

    /**
     * Icon for CHECK or RADIO styles.
     */
    CGulIcon* iCustomIcon;
    CSwtFont* iDupFont;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    MTouchFeedback *iFeedback;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    const MSwtColor* iCustomFg;
};

#endif // SWTBUTTON_H
