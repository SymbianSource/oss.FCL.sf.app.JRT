/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTBUTTONPROXY_H
#define SWTBUTTONPROXY_H


#include <AknControl.h>
#include <coecobs.h>


class CAknButton;
class CAknButtonState;
class CGulIcon;


/**
 * CSwtButtonStateProxy
 * eSWT's CSwtButton access to S60's CAknButtonState
 * @lib eswtapifacade
 */
class CSwtButtonStateProxy
        : public CBase
{
// Own methods
public:
    IMPORT_C static CSwtButtonStateProxy* NewL(CAknButtonState& aAknButtonState);
    IMPORT_C virtual ~CSwtButtonStateProxy();

// From CAknButtonState
public:
    IMPORT_C const TDesC& Text() const;
    IMPORT_C void SetTextL(const TDesC& aText);
    IMPORT_C const CGulIcon* Icon() const;
    IMPORT_C void SetIcon(CGulIcon* aIcon);
    IMPORT_C TInt Flags() const;
    IMPORT_C void SetFlags(const TInt aFlags);

// Own methods
private:
    CSwtButtonStateProxy(CAknButtonState& aAknButtonState);

// Data
private:
    CAknButtonState& iDelegate;
};


/**
 * CSwtButtonProxy
 * eSWT's CSwtButton access to S60's CAknButton
 * @lib eswtapifacade
 */
class CSwtButtonProxy
        : public CAknControl
        , public MCoeControlObserver
{
// Types
public:
    enum TAlignment
    {
        ECenter,
        ETop,
        EBottom,
        ERight,
        ELeft
    };

    enum TTextAndIconAlignment
    {
        EIconBeforeText,
        EIconAfterText,
        EIconUnderText,
        EIconOverText,
        EOverlay
    };

// Own methods
public:
    IMPORT_C static CSwtButtonProxy* NewL(
        CCoeControl* aParent,
        CGulIcon* aIcon,
        CGulIcon* aDimmedIcon,
        CGulIcon* aPressedIcon,
        CGulIcon* aHoverIcon,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags);

    IMPORT_C static CSwtButtonProxy* NewL(
        CCoeControl* aParent,
        const TDesC& aFilePath,
        const TInt aBmpId,
        const TInt aMaskId,
        const TInt aDimmedBmpId,
        const TInt aDimmedMaskId,
        const TInt aPressedBmpId,
        const TInt aPressedMaskId,
        const TInt aHoverBmpId,
        const TInt aHoverMaskId,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags,
        const TAknsItemID& aId = KAknsIIDNone,
        const TAknsItemID& aDimmedId = KAknsIIDNone,
        const TAknsItemID& aPressedId = KAknsIIDNone,
        const TAknsItemID& aHoverId = KAknsIIDNone);

    IMPORT_C static CSwtButtonProxy* NewL(
        CCoeControl* aParent,
        CGulIcon* aIcon,
        const TDesC& aText,
        const TInt aFlags);

    IMPORT_C virtual ~CSwtButtonProxy();

    /**
     * Provides the text inside the button.
     * @return The button text.
     */
    IMPORT_C const TDesC& Text() const;

    /**
     * Sets the text inside the button.
     * @param aText The text inside the button.
     */
    IMPORT_C void SetTextL(const TDesC& aText);

    /**
     * Provides the icon.
     * @return The icon.
     */
    IMPORT_C const CGulIcon* Icon(void) const;

    /**
     * Sets the icon. Takes ownership.
     * @param aIcon The icon.
     */
    IMPORT_C void SetIcon(CGulIcon* aIcon);

    /**
     * Provides the button flags.
     * @return The button flags.
     */
    IMPORT_C TInt Flags() const;

    /**
     * Sets the button flags.
     * @param aFlags The button flags.
     */
    IMPORT_C void SetFlags(const TInt aFlags);

// From CAknButton
public:
    IMPORT_C void SetTextFont(const CFont* aFont);
    IMPORT_C void SetTextHorizontalAlignment(const TInt aHorizontalAlignment);
    IMPORT_C void SetTextUnderlineStyle(TFontUnderline aUnderlineStyle);
    IMPORT_C void SetIconScaleMode(const TScaleMode aScaleMode);
    IMPORT_C void SetIconHorizontalAlignment(const TInt aHorizontalAlignment);
    IMPORT_C void SetHighlightRect(const TRect& aRect);
    IMPORT_C TRect HighlightRect() const;
    IMPORT_C TBool IsDimmed() const;
    IMPORT_C CSwtButtonStateProxy* State() const;
    IMPORT_C TInt ButtonFlags() const;
    IMPORT_C TInt SetIconSize(const TSize& aSize);
    IMPORT_C void ResetState();

// From CCoeControl
public:
    IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;
    IMPORT_C TInt CountComponentControls() const;
    IMPORT_C TSize MinimumSize();
    IMPORT_C void ActivateL();
    IMPORT_C void SetDimmed(TBool aDimmed);
    IMPORT_C void MakeVisible(TBool aVisible);
    IMPORT_C void PrepareForFocusLossL();
    IMPORT_C void PrepareForFocusGainL();
    IMPORT_C TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                         TEventCode aType);
    IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    IMPORT_C void HandleResourceChange(TInt aType);
    IMPORT_C void SetBackground(const MCoeControlBackground* aBackground);
    IMPORT_C TBool IsFocused() const;
    IMPORT_C void SetFocus(TBool aFocus, TDrawNow aDrawNow=ENoDrawNow);

// From CCoeControl
protected:
    IMPORT_C void SizeChanged();

// From MCoeControlObserver
public:
    IMPORT_C void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

// Own methods
protected:
    IMPORT_C CSwtButtonProxy();

    IMPORT_C virtual void ConstructL(
        CCoeControl* aParent,
        CGulIcon* aIcon,
        CGulIcon* aDimmedIcon,
        CGulIcon* aPressedIcon,
        CGulIcon* aHoverIcon,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags);

    IMPORT_C virtual void ConstructL(
        CCoeControl* aParent,
        const TDesC& aFilePath,
        const TInt aBmpId,
        const TInt aMaskId,
        const TInt aDimmedBmpId,
        const TInt aDimmedMaskId,
        const TInt aPressedBmpId,
        const TInt aPressedMaskId,
        const TInt aHoverBmpId,
        const TInt aHoverMaskId,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags,
        const TAknsItemID& aId = KAknsIIDNone,
        const TAknsItemID& aDimmedId = KAknsIIDNone,
        const TAknsItemID& aPressedId = KAknsIIDNone,
        const TAknsItemID& aHoverId = KAknsIIDNone);

    IMPORT_C virtual void ConstructL(
        CCoeControl* aParent,
        CGulIcon* aIcon,
        const TDesC& aText,
        const TInt aFlags);

// Data
private:
    CAknButton* iDelegate; // own
    CSwtButtonStateProxy* iDelegateState; // own
};


#endif // SWTBUTTONPROXY_H
