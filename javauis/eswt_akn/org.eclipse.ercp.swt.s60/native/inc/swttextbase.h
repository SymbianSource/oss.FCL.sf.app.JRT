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


#ifndef SWTTEXTBASE_H
#define SWTTEXTBASE_H


#include <coecntrl.h>
#include "swtscrollablebase.h"
#include "swteditindicator.h"

#ifdef RD_SCALABLE_UI_V2
#include <fepbase.h>
#endif //RD_SCALABLE_UI_V2

class CEikEdwin;


/**
 * CSwtTextBase
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTextBase)
        : public CAknControl
        , public ASwtScrollableBase
#ifdef RD_SCALABLE_UI_V2
        , public MCoeCaptionRetrieverForFep
#endif //RD_SCALABLE_UI_V2
{
protected:
    /**
     * First phase constructor.
     * @param aDisplay parent display
     * @param aPeer java peer
     * @param aParen composite parent
     * @param aStyle supported styles
     */
    CSwtTextBase(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle);

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CSwtTextBase();

    /**
     * Get reference to the current Edwin editor.
     * @return Edwin reference.
     */
    inline CEikEdwin& Editor() const;

    /**
     * Set Edwin editor. Cannot be NULL.
     * @param aEditor Edwin pointer.
     */
    void SetEditor(CEikEdwin* aEditor);

    /**
     * Update default font.
     */
    void RetrieveDefaultFontL();

    /**
     * Compute size between text and control's rect.
     * @return computed size
     */
    TSize ComputeDelta() const;

    /**
     * Check if specified key event trnaslates into a hotkey.
     * param aKeyEvent key event structure.
     * @return one of CEikEdwin::TEdwinHotKeys or -1 if cannot translate.
     */
    TInt HotKeyL(const TKeyEvent& aKeyEvent) const;

    /**
     * Is the editor editable?
     * @return editability state.
     */
    inline TBool GetEditable() const;

    /**
     * Make the editor editable.
     * @param aEditable editability state.
     */
    void SetEditable(TBool aEditable);

    /**
     * Replace the entire text of the editor.
     * @param aText the text to be set.
     * @return ETrue if operation succeeded, EFalse otherwise.
     */
    TBool SetTextL(const TDesC& aText);

    /**
     * Append at the end of the editor's text.
     * @param aText the text to be appended.
     * @return ETrue if operation succeeded, EFalse otherwise.
     */
    TBool AppendL(const TDesC& aText);

    /**
     * Insert into editor's text starting at cursor's position.
     * @param aText the text to be inserted.
     * @return ETrue if operation succeeded, EFalse otherwise.
     */
    TBool InsertL(const TDesC& aText);

    /**
     * Cut into clipboard the text that is selected in the editor.
     * @return ETrue if operation succeeded, EFalse otherwise.
     */
    TBool CutL();

    /**
     * Set the maximum length of the editor's text. Typing, appending
     * inserting and setting text should not pass over this limit.
     * @param aLimit limit in number of chars.
     * @return ETrue if operation succeeded, EFalse otherwise.
     */
    TBool SetLimitL(TInt aLimit);

    /**
     * Forward key event to the contained Edwin editor.
     */
    void ForwardKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

    /**
     * Adds some reasonable spacing to facilitate finger touch usability.
     */
    void FixPadding();

    void UpdateTextColor();

#if defined( RD_SCALABLE_UI_V2)
// From MCoeCaptionRetrieverForFep
private:
    virtual void GetCaptionForFep(TDes& aCaption) const;
    virtual void MCoeCaptionRetrieverForFep_Reserved_1();
    virtual void MCoeCaptionRetrieverForFep_Reserved_2();
#endif //RD_SCALABLE_UI_V2

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    void HandleResourceChange(TInt aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

// From CCoeControl
protected:
    void FocusChanged(TDrawNow aDrawNow);
    void PositionChanged();
    void SizeChanged();
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
#if defined( RD_SCALABLE_UI_V2)
    TCoeInputCapabilities InputCapabilities() const;
#endif //RD_SCALABLE_UI_V2

// From MSwtControl
public:
    TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);
    TRect ClientRect() const;
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void SetFontL(const MSwtFont* aFont);
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    TBool IsKeyUsed(TUint aKeyCode) const;
    TBool CanDrawOutsideRect() const;
    void SetBackgroundL(const MSwtColor* aColor);
    void SetForegroundL(const MSwtColor* aColor);
    void SetBounds(const TRect& aRect);
    void SetWidgetSize(const TSize& aSize);
    TSwtPeer Dispose();
    TInt PressBackgroundPolicy() const;
    void HandleHighlightChange();

// From ASwtControlBase
protected:
    const MSwtFont& DefaultFont() const;

// From ASwtScrollableBase
protected:
    CEikScrollBarFrame* SbFrame() const;
    void ProcessFontUpdateL();

// From MEikScrollBarObserver
public:
    void HandleScrollEventL(CEikScrollBar* aScrollBar,
                            TEikScrollEvent aEventType);

// Data
protected:

    /**
     * Edit indicator for multipage dialog can be NULL
     * Own.
     */
    CSwtEditIndicator* iIndicator;

    /**
     * Flag for ignoring the first pointer down event after gaining focus,
     * used for disabling showing of VKB when unfocused Text is tapped.
     */
    TBool iIgnorePointerDown;

// Data
private:
    /**
     * The contained editor.
     * Own.
     */
    CEikEdwin* iEditor;

    /**
     * The default font, cannot be NULL.
     * Own.
     */
    MSwtFont* iDefaultFont;

    /**
     * Maximum length of the text.
     */
    TInt iTextLimit;

    /**
    * The Custom Color checking, setted by API.
    */
    TBool iTextColor;

    /**
     * Global text in single line need to change the font later.
     */
    CCharFormatLayer* iCharFormatLayer;

    /**
     * Deliver all pointer event to the scrollbar when this on.
     */
    TBool iVScrollBarGrabsPointerEvents;
};


inline CEikEdwin& CSwtTextBase::Editor() const
{
    ASSERT(iEditor);
    return *iEditor;
}

inline TBool CSwtTextBase::GetEditable() const
{
    return !(iStyle & KSwtStyleReadOnly);
}


#endif // SWTTEXTBASE_H
