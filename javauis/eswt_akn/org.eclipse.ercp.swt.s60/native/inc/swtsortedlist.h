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


#ifndef SWTSORTEDLIST_H
#define SWTSORTEDLIST_H


#include <aknsfld.h>
#include "eswtmobileextensions.h"
#include "swtlistbase.h"


class CAknSearchField;
class CAknListBoxFilterItems;


/**
 * CSwtSortedList
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtSortedList)
        : public CSwtListBase
        , public MSwtSortedList
        , public MEikEdwinObserver
        , public MAdaptiveSearchTextObserver
{
public:
    /**
     * Two phase constructor
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     * @return pointer to an object of this class
     */
    static CSwtSortedList* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

protected:
    /**
     * First phase constructor.
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     */
    CSwtSortedList(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                   MSwtComposite& aParent, TInt aStyle);

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Destructor
     */
    ~CSwtSortedList();

    /**
     * Get the filter.
     * @return pointer to the filter.
     */
    CAknListBoxFilterItems* Filter() const;

    /**
     * The item text aPtr is inserted in the data member iTextItems.
     * The item text aPtr is also inserted in an array passed on parameter
     * after being modified to include the tab separators required by a column list box.
     * The insertion respects the sorted order (up or down).
     * @param aPtr the string to insert.
     * @param aItemArray array where the string will be inserted.
     * @param aTextComparisonType comparison type.
     */
    void InsertItemL(const TDesC &aPtr, CDesCArray* aItemArray,
                     TKeyCmpText aTextComparisonType = ECmpCollated);

    /**
     * Get the strings of selected items.
     * @return pointer to the array containing the strings of selected items.
     */
    CPtrCArray* GetSelectionLC() const;

    /**
     * Handles an item addition.
     * @param aNewFocusIndex The new focus index.
     */
    void HandleItemAdditionL(TInt aNewFocusIndex);

    /**
     * Handles an item removal.
     * @param aRemovedItemIndices The removed item indices.
     * @param aOldFocusIndex The old focus index.
     */
    void HandleItemRemovalL(const RArray<TInt>& aRemovedItemIndices,
                            TInt aOldFocusIndex);

    /**
     * Handles an item removal.
     */
    void HandleOfferKeyEventL();

    /**
     * Get the search field height.
     * @return the search field height.
     */
    TInt SearchFieldHeight() const;

    /**
     * Get position of the search field.
     */
    void GetSearchFieldPosition(TPoint& aPosition);

    /**
     * Get size of the search field.
     */
    void GetSearchFieldSize(TSize& aSize);

    /**
     * Update the text view rect of the search field.
     * @param aSearchField the search field.
     */
    void UpdateSearchTextViewRect(const CAknSearchField* aSearchField);

    /**
     * Rebound the filter.
     */
    void UpdateSearchFieldBounds();

// From CSwtListBase
public:
    void UpdateSize();

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void SetDimmed(TBool aDimmed);
    void MakeVisible(TBool aVisible);
    void HandleResourceChange(TInt aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif //RD_SCALABLE_UI_V2

protected:
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);

// From MSwtControl
public:
    TRect ClientRect() const;
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
#ifdef RD_JAVA_S60_RELEASE_9_2
    void EnableFocusHighlight(TBool aEnable);
#endif //RD_JAVA_S60_RELEASE_9_2
    void PrepareForTraverse();

protected:
    void DoPaint(const TRect& aRect) const;

// From MSwtScrollable
public:
    void ComputeTrim(TRect& aRect) const;

// From MSwtSortedList
public:
    void AppendL(const TDesC& aPtr);
    void GetFocusedItem(TPtrC& aItem) const;
    TInt GetItemHeight() const;
    TInt GetSelectionCount() const;
    CPtrCArray* GetSelectionL() const;
    void RemoveAllL();
    TBool RemoveL(const HBufC& aPtr);
    MSwtScrollable* Scrollable();
    TBool SelectItemL(const TDesC* aString);
    void SetItemsL(MDesCArray* aStringArray);
    void SetModeStyleL(TInt aModeStyle);
    void SetSelectionL(MDesCArray* aStringArray);
    void ShowSelection();

// From MEikEdwinObserver
public:
    void HandleEdwinEventL(CEikEdwin *aEdwin, TEdwinEvent aEventType);

// From MAdaptiveSearchTextObserver
public:
    void AdaptiveSearchTextChanged(CAknSearchField* aSearchField);

// Data
private:
    /**
     * The search field with will be used to filter the list.
     * Own.
     */
    CAknSearchField* iSearchField;

    /**
     * SortedList mode style. Possible values: FILTER, UP, DOWN.
     */
    TInt iModeStyle;

    /**
     * ETrue if the sort should be in down order (from higher
     * to lower number). EFalse by default.
     */
    TBool iIsOrderDown;

    /**
     * Deliver all pointer event to the filter when this on.
     */
    TBool iFilterGrabsPointerEvents;
};


#endif // SWTSORTEDLIST_H
