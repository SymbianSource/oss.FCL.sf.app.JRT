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
 

#ifndef SWTLIST_H
#define SWTLIST_H


#include "swtlistbase.h"


/**
 * CSwtList
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtList)
        : public CSwtListBase
        , public MSwtList
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
    static CSwtList* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

protected:
    /**
     * First phase constructor
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     */
    CSwtList(MSwtDisplay& aDisplay, TSwtPeer aPeer,
             MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor
     */
    ~CSwtList();

private:
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

// From MSwtControl
public:
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);

// From MSwtList
public:
    MSwtScrollable* Scrollable();
    void DeselectItems(const TInt* aIndices, TInt aCount);
    void DeselectItem(TInt aIndex);
    void DeselectRange(TInt aStart, TInt aEnd);
    void DeselectAll();
    TInt GetFocusIndex() const;
    TInt GetSelectionCount() const;
    const CArrayFix<TInt>* GetSelectionIndices() const;
    void SelectItemL(TInt aIndex, TBool aScroll);
    void SelectRangeL(TInt aStart, TInt aEnd);
    void SelectAllL();
    void ShowSelection();
    void AppendL(const TDesC& aPtr);
    void InsertL(TInt aPos, const TDesC& aPtr);
    void GetItemL(TInt aItemIndex, TPtrC& aString) const;
    TInt GetItemCount() const;
    TInt GetItemHeight() const;
    TInt GetSelectionIndex() const;
    TInt GetTopIndex() const;
    TInt IndexOf(const TDesC& aString, TInt& aStart) const;
    TBool IsSelected(TInt aIndex) const;
    void RemoveItemsL(const TInt* aIndices, TInt aCount);
    void RemoveItemL(TInt aIndex);
    void RemoveRangeL(TInt aStart, TInt aEnd);
    void RemoveAllL();
    void SetFocusIndex(TInt aIndex);
    void SetItemsL(MDesCArray* aStringArray);
    void SetTopIndex(TInt aIndex);
};


#endif // SWTLIST_H
