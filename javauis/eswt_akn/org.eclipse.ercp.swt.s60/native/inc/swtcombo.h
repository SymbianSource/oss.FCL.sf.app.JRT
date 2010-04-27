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


#ifndef SWTCOMBO_H
#define SWTCOMBO_H


#include <AknControl.h>
#include "eswtwidgetscore.h"
#include "swtscrollablebase.h"


class CSwtComboProxy;
class MTouchFeedback;


/**
 * CSwtCombo
 * Combo peer implementation. Uses CAknChoiceList.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCombo)
        : public CAknControl
        , public ASwtScrollableBase
        , public MSwtComposite
        , public MSwtCombo
        , public MCoeControlObserver
{
// Own methods
public:
    /**
     * Two phase constructor
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aParent composite parent
     * @param aStyle control style
     * @return pointer to an object of this class
     */
    static CSwtCombo* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

protected:
    /**
     * First phase constructor
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aParent composite parent
     * @param aStyle control style
     */
    CSwtCombo(MSwtDisplay& aDisplay, TSwtPeer aPeer,
              MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor
     */
    virtual ~CSwtCombo();

    /**
     * Second phase constructor
     */
    void ConstructL();

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIdx) const;
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif //RD_SCALABLE_UI_V2
    void HandleResourceChange(TInt aType);
    void Draw(const TRect& aRect) const;

// From CCoeControl
protected:
    void PositionChanged();
    void SizeChanged();
    void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    TSize ComputeSizeL(TInt aWhint, TInt aHhint);
    void ProcessKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType);
    TBool IsKeyUsed(TUint aKeyCode) const;
    HBufC* MSKLabelL() const;

// From ASwtScrollableBase
protected:
    CEikScrollBarFrame* SbFrame() const;

// From MSwtScrollable
public:
    void ComputeTrim(TRect& aRect) const;

// From MSwtComposite
public:
    MSwtScrollable* Scrollable();
    void RegisterChildL(MSwtControl& aChild);
    void UnregisterChild(MSwtControl& aChild);
    CSwtPeerArray* GetChildrenPeersL();
    const RSwtControlArray* Children() const;
    void ChildFocusChanged(MSwtControl& aControl);
    TInt CountFocusableChildren(TInt aTryBreakAfter = -1,
                                const MSwtControl* aExclude = NULL, TBool aVisualFocusCount = ETrue) const;
    void SetTabListL(MSwtControl** aControls, TInt aCount);
    CSwtPeerArray* GetTabListL() const;
    void MoveChildAbove(MSwtControl& aChild1,
                        const MSwtControl* aChild2);
    void MoveChildBelow(MSwtControl& aChild1,
                        const MSwtControl* aChild2);
    void NotifyRadioGroup(MSwtButton* aSelectedButton);
    void SetScrolledCompositeContentL(MSwtControl* aContent);
    MSwtControl* ScrolledCompositeContent() const;
    TInt ScrolledCompositePysicsAction() const;
    void DrawScrollBarBackground(const TRect& aRect) const;

// From MSwtCombo
public:
    MSwtComposite* Composite();
    void AppendL(const TDesC& aTxt);
    void InsertL(TInt aIdx, const TDesC& aTxt);
    void ClearSelectionL();
    void CopyL();
    void DeselectItem(TInt aIdx);
    void DeselectAll();
    void GetItemL(TInt aIdx, TPtrC& aTxt) const;
    TInt GetItemCount() const;
    TInt GetItemHeight() const;
    CPtrCArray* GetItemsL() const;
    TInt GetOrientation() const;
    TPoint GetSelection() const;
    TInt GetSelectionIndex() const;
    HBufC* GetTextL() const;
    TInt GetTextHeightL() const;
    TInt GetTextLimit() const;
    TInt GetVisibleItemCount() const;
    TInt IndexOf(const TDesC& aTxt, TInt& aStart) const;
    TBool InsertInTextControlL(const TDesC& aTxt);
    TBool PasteL();
    void RemoveItemL(TInt aIdx);
    void RemoveRangeL(TInt aStart, TInt aEnd);
    void RemoveAllL();
    void SelectItemL(TInt aIdx);
    void SetItemsL(MDesCArray* aTxtArr);
    void SetOrientationL(TInt aOrientation);
    void SetSelectionL(TInt aStart, TInt aEnd);
    TBool SetTextL(TDes& aTxt);
    TBool SetTextLimit(TInt aTxtLimit);
    void SetVisibleItemCount(TInt aCount);

// From MCoeControlObserver
public:
    void HandleControlEventL(
        CCoeControl* aControl,
        TCoeEvent aEventType);

// Own methods
private:
    /**
     * Open the choice list.
     */
    void DoOpenL();

    /**
     * Close the choice list.
     */
    void DoClose();

    /**
     * Item selection helper.
     */
    void DoSelectItemL(TInt aIdx);

    /**
     * Fixed border for the contained combo.
     */
    void DrawContainedComboBorder() const;

    /**
     * Position the contained combo in the middle of the compound control.
     */
    void CenterContainedCombo();

    /**
     * Make sure the feedback area follows the control's bounds.
     */
    void UpdateTouchFeedbackRect() const;

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

// Data
private:
    /**
     * Contained read-only Avkon choice list.
     * Own.
     */
    CSwtComboProxy* iCombo;

    /**
     * String item array.
     * Not own.
     */
    CDesCArray* iItems;

    /**
     * Previously selected index.
     */
    TInt iPrevSelIdx;

    /**
     * Open flag.
     */
    TBool iOpen;

    /**
     * True while opening.
     */
    TBool iOpening;

    /**
     * Feedback instance.
     * Not own.
     */
    MTouchFeedback* iFeedback;
};


#endif // SWTCOMBO_H
