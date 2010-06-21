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


#ifndef SWTTREE_H
#define SWTTREE_H


#include <e32base.h>
#include <akntreelistobserver.h>
#include <akncustomtreeordering.h>
#include "eswtwidgetsexpanded.h"
#include "swtcomposite.h"
#include "swtmaskhandler.h"


class CAknSingleColumnStyleTreeList;



/**
 * Class manages the contents of the Symbian Tree.
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTree)
        : public CSwtComposite
        , public MSwtTree
        , public MAknTreeListObserver
        , public MAknCustomTreeOrdering
{
// Own api
public:
    static CSwtTree* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);
    virtual ~CSwtTree();

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIdx) const;
    void HandleResourceChange(TInt aType);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

// From CCoeControl
protected:
    void SizeChanged();
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;
    TInt FocusBackgroundPolicy() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TBool IsKeyUsed(TUint aKeyCode) const;
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

// From MSwtTree
public:
    MSwtComposite* Composite();
    TInt AddL(TInt aItemHandle);
    TRect Bounds(TInt aItemHandle) const;
    void Check(TInt aItemHandle, TBool aState);
    void Collapse(TInt aItemHandle);
    void Expand(TInt aItemHandle);
    TInt InsertL(TInt aItemHandle, TInt aIndex);
    TBool IsExpanded(TInt aItemHandle) const;
    TBool IsSelected(TInt aItemHandle) const;
    TInt ItemAt(TInt aX, TInt aY) const;
    TInt ItemHeight() const;
    void Remove(TInt aItemHandle);
    void RemoveAll();
    void Select(const TInt* aItemHandles, TInt aCount, TBool aState);
    void SelectAll(TBool aState);
    TInt SelectionCount() const;
    const CArrayFix<TInt>* SelectionL() const;
    void SetImageL(TInt aItemHandle, const MSwtImage* aImage);
    void SetTextL(TInt aItemHandle, const TDesC& aText);
    TInt TopItem() const;

// From MAknTreeListObserver
public:
    TInt HandleTreeListEvent(CAknTreeList& aList, TAknTreeItemID aItem,
                             MAknTreeListObserver::TEvent aEvent);

// From MAknCustomTreeOrdering
public:
    TInt Compare(TAknTreeItemID aFirst, TAknTreeItemID aSecond);

// Own api
protected:
    CSwtTree(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent,
             TInt aStyle,TBool aVisibility, TBool aDimmed);
    void ConstructL();
    void UpdateItemHeight();
    TInt LastCollapsedItem();
    void RemoveItemRefs(TInt aItemHandle);
    TBool IsMarkable() const;
    TAknLayoutRect ItemLayoutRect();
    void UpdateImagesSize();

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
public:
    void DoControlSpecificFeedback(
        const TBool& aFirstTap,
        const TBool& aTappedToChildRect,
        const TPointerEvent& aPointerEvent) const;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// Data
private:
    CAknSingleColumnStyleTreeList* iTree;

    // Keep track of the items order for 'insert with sort' mechanism.
    RArray<TInt> iItems;

    // List of images that are registered with CAknTreeList.
    // Keep in sync with iRegisteredImageIds & iRegisteredImageRefs
    RArray<const MSwtImage*> iImages;

    // Id for each image that was registered with CAknTreeList
    // Keep in sync with iRegisteredImages & iRegisteredImageRefs
    RArray<TInt> iImageIds;

    // Number of uses for each registered image. Avoiding over-registration / memory allocation.
    // Keep in sync with iRegisteredImages & iRegisteredImageIds
    RArray<TInt> iImageRefs;

    // For learning the item height.
    TBool iItemHeightValid;
    TInt iItemHeight;
    TSize iImageMaxSize;

    // Last focused item.
    TInt iLastFocusedItem;

    // CAknTree ignores images, that don't have mask
    // This class creates such masks
    CSwtMaskHandler* iMaskHandler;

    /*
     * Attributes used for reverting pointer events on Scrollable Composite
     * iPointerRevertExpandNeeded   - need to expand item in pointer revert
     * iPointerRevertCollapseNeeded - need to collapse item in pointer revert
     * iRevertedItem                - item, that should revert its state
     */
    TBool iPointerRevertExpandNeeded;
    TBool iPointerRevertCollapseNeeded;
    TAknTreeItemID iRevertedItem;
};


#endif // SWTTREE_H



