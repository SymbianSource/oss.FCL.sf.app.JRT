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


#ifndef SWTLISTBASE_H
#define SWTLISTBASE_H


#include <eiklbo.h>
#include "swtscrollablebase.h"
#include "swtlistboxlists.h"
#include "eswtwidgetscore.h"


const TUint KCheckboxColumnIndex = 0;
const TUint KTextColumnIndex = 1;


/**
 * CSwtListBase
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtListBase)
        : public CAknControl
        , public ASwtScrollableBase
        , public MSwtListObserver
        , public MEikListBoxObserver
{
// Own methods
public:
    /**
     * Create a list box.
     * @param aStyle control style.
     */
    virtual CEikTextListBox* CreateListBoxL(TInt aStyle);

    /**
     * Create a list box model.
     */
    virtual void CreateListBoxModelL();

    /**
     * Create the list scrollbars.
     * @param aLb the list having the scrollbars.
     */
    virtual void CreateScrollBarsL(CEikTextListBox* aLb);

    /**
     * Create the icon array containing the icons for the multi selection.
     * @param aLb the list having the icons.
     */
    virtual void CreateIconArrayL(CEikTextListBox* aLb);

    /**
     * Create an item text according with its selection state.
     * @param aPtr the item text.
     * @param aSelected the item selection state.
     * @return the buffer containing the item text.
     */
    virtual HBufC* CreateItemTextLC(const TDesC& aPtr, TBool aSelected = EFalse);

    /**
     * Update control size.
     */
    virtual void UpdateSize();

    /**
     * Update the list box font.
     */
    void UpdateListBoxFontL();

    /**
     * Update the list box text color.
     */
    void UpdateListBoxTextColorL();

    /**
     * Returns MSwtScrollable reference.
     * @return MSwtScrollable reference.
     */
    MSwtScrollable* Scrollable();

    /**
     * Deselect items.
     * @param aIndices the indices of items to deselect.
     * @param aCount the number of items to deselect.
     */
    void DeselectItems(const TInt* aIndices, TInt aCount);

    /**
     * Deselect an item.
     * @param aIndex the index of the item to deselect.
     */
    void DeselectItem(TInt aIndex);

    /**
     * Deselect a range of items.
     * @param aStart the start index of the item range.
     * @param aEnd the end index of the item range.
     */
    void DeselectRange(TInt aStart, TInt aEnd);

    /**
     * Deselect all items.
     */
    void DeselectAll();

    /**
     * Get focus index.
     * @return the index of the focused item.
     */
    TInt GetFocusIndex() const;

    /**
     * Get the number of selected items.
     * @return the number of selected items.
     */
    TInt GetSelectionCount() const;

    /**
     * Get the indices of selected items.
     * @return the index array of selected items.
     */
    const CArrayFix<TInt>* GetSelectionIndices() const;

    /**
     * Select an item.
     * @param aIndex the index of the item to select.
     * @param aScroll If ETrue the list is scrolled to make visible the selected item.
     */
    void SelectItemL(TInt aIndex, TBool aScroll);

    /**
     * Select a range of items.
     * @param aStart the start index of the item range.
     * @param aEnd the end index of the item range.
     */
    void SelectRangeL(TInt aStart, TInt aEnd);

    /**
     * Select all items.
     */
    void SelectAllL();

    /**
      * Show the selected item.
      */
    void ShowSelection() const;

    /**
     * Post a selection event.
     */
    void PostSelectionEvent() const;

    /**
     * Calculate the focus index and the top item index after an item removal.
     * @param aNewFocusIndex the new focusindex.
     * @param aRemovedItemIndices the removed item indices.
     * @param aOldFocusIndex the old focus index.
     */
    void CalcFocusIndexAfterItemRemoval(TInt& aNewFocusIndex,
                                        const RArray<TInt>& aRemovedItemIndices, TInt aOldFocusIndex) const;

    /**
     * Get the minimum width of a list item.
     * @return the minimum width of a list item.
     */
    TInt MinimumListItemWidth() const;

    /**
     * Get the preferred width.
     * @return preferred width.
     */
    TInt PreferredWidth() const;

    /**
     * Get the preferred height.
     * @return preferred height.
     */
    TInt PreferredHeight() const;

    /**
     * Compute the maximum width of item text.
     * @return the the maximum width of item text.
     */
    TInt ComputeTextMaxWidth() const;

    /**
     * Compute the maximum width of item text.
     * @return the the maximum width of item text.
     */
    void SetTextMaxWidth(TInt aMaxTxtWidth);

    /**
     * Update MSK.
     */
    void UpdateListMskL();

protected:
    /**
     * First phase constructor
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     */
    CSwtListBase(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor
     */
    ~CSwtListBase();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:
    /**
     * Update the default font.
     */
    void UpdateDefaultFontL();

    /**
     * Handling of resource change events.
     * @param aType event type.
     */
    void HandleResourceChangeL(TInt aType);

    /**
     * Get current state of list selection.
     * @param aCurrSelItems resulting array of selected items.
     */
    void GetUserSelectionL(CArrayFix<TInt>* aCurrSelItems);

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandleResourceChange(TInt aType);

protected:
    void SizeChanged();
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

private:
    void Draw(const TRect& aRect) const;

// From ASwtControlbase
public:
    void SetFontL(const MSwtFont* aFont);
    HBufC* MSKLabelL() const;
    TBool MSKSelCmdEnabled() const;

protected:
    const MSwtFont& DefaultFont() const;

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    TBool IsKeyUsed(TUint aKeyCode) const;
    TRgb GetForeground() const;
    void SetForegroundL(const MSwtColor* aColor);
    void SetBackgroundL(const MSwtColor* aColor);
    TBool IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const;

// From ASwtScrollableBase
protected:
    CEikScrollBarFrame* SbFrame() const;

// From MSwtListObserver
public:
    void HandleSizeChangedL();
    MSwtUiUtils& Utils() const;

// From MEikScrollBarObserver
public:
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

// From MEikListBoxObserver
public:
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

// Data
protected:
    /**
     * Previously selected items (temporary)
     * Own
     */
    CArrayFix<TInt>* iPrevSelItems;

    /**
     * Pointer to the contained ListBox.
     * Own
     */
    CEikTextListBox* iList;

    /**
     * Array of text items.
     * Own
     */
    CDesCArray* iTextItems;

    /**
     * The default font.
     * Own
     */
    MSwtFont* iDefaultFont;

    /**
     * The current used LAF font.
     */
    const CFont* iCurrentLafFont;

    /**
     * The default item height (when default font is set).
     */
    TInt iDefaultItemHeight;

    /**
     * Pointer to the text color.
     */
    const MSwtColor* iTextColor;

    /**
     * Removed item indices.
     */
    RArray<TInt> iRemovedItemIndices;

    /**
     * The maximum width of item text.
     */
    TInt iTxtMaxWidth;

    /**
     * Deliver all pointer event to the scrollbar when this on.
     */
    TBool iVScrollBarGrabsPointerEvents;

    /**
     * Stores last known focus index. Use only during paint and key handlers.
     */
    TInt iPrevFocusIndex;

    /**
     * Type of the list
     */
    TInt iListType;

    /**
     * View visible rect at last draw
     */
    mutable TRect iLastViewVisibleRect;
};


#endif // SWTLISTBASE_H
