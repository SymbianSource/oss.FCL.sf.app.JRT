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


#ifndef SWTLISTBOX_H
#define SWTLISTBOX_H


#include <eiklbo.h>
#include "eswtmobileextensions.h"
#include "swtscrollablebase.h"
#include "swtlistboxlists.h"


/**
 * CSwtLbItem
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtLbItem)
        : public CSwtListBoxItem
{
// Own methods
public:
    /**
     * Constructor
     */
    CSwtLbItem();

    /**
     * Constructor
     */
    CSwtLbItem(
        HBufC* aDtxt,
        const MSwtImage* aDimg,
        HBufC* aHtxt,
        const MSwtImage* aHimg);

    /**
     * Constructor
     */
    CSwtLbItem(
        HBufC* aDtxt,
        const MSwtImage* aDimg,
        const TSize& aDimgSize,
        HBufC* aHtxt,
        const MSwtImage* aHimg,
        const TSize& aHimgSize);

    /**
     * Destructor
     */
    virtual ~CSwtLbItem();

// Data
public:
    /**
     * Heading image index
     */
    TInt iHimgIdx;

    /**
     * Detail image index
     */
    TInt iDimgIdx;

    /**
     * Item index
     */
    TInt iIdx;
};


/** Array of List Box Items */
typedef RPointerArray<CSwtLbItem> RSwtLbItems;


/**
 * CSwtListBox
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtListBox)
        : public CAknControl
        , public ASwtScrollableBase
        , public MSwtListBox
        , public MSwtListObserver
        , public MEikListBoxObserver
{
// Types
private:
    /** List box properties */
    struct TSwtLbProp
    {
        TInt iListType;
        TInt iSelectionType;
        TInt iDiconVisible;
    };

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
    static CSwtListBox* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite& aParent, TInt aStyle);

protected:
    /**
     * First phase constructor.
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     */
    CSwtListBox(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor
     */
    virtual ~CSwtListBox();

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
    void HandleResourceChange(TInt aType);

protected:
    void SizeChanged();
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

private:
    void Draw(const TRect& aRect) const;

// From ASwtControlBase
protected:
    HBufC* MSKLabelL() const;

// From ASwtScrollableBase
protected:
    CEikScrollBarFrame* SbFrame() const;

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    TSize ComputeSizeL(TInt aWhint, TInt aHhint);
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    TBool IsKeyUsed(TUint aKeyCode) const;
    TBool MSKSelCmdEnabled() const;
    TRgb GetForeground() const;
    void SetForegroundL(const MSwtColor* aColor);
    void SetBackgroundL(const MSwtColor* aColor);
    TBool IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const;
#ifdef RD_JAVA_S60_RELEASE_9_2
    void EnableFocusHighlight(TBool aEnable);
#endif //RD_JAVA_S60_RELEASE_9_2
    TInt FocusBackgroundPolicy() const;
    void PrepareForTraverse();

// From MSwtListBox
public:
    MSwtScrollable* Scrollable();
    void SetLayoutStyleL(TInt aLayoutStyle);
    void DeselectItemsL(const TInt* aIndices, TInt aCount);
    void DeselectItemL(TInt aIdx);
    void DeselectRangeL(TInt aStart, TInt aEnd);
    void DeselectAllL();
    TInt GetFocusIndex() const;
    TInt GetSelectionCount() const;
    const CArrayFix<TInt>* GetSelectionIndices() const;
    void RefreshItemL(TInt aIdx, const CSwtListBoxItem* aItem);
    void RefreshListL(const RSwtListBoxItemsArray& aItemsArray);
    void SelectItemL(TInt aIdx, TBool aScroll);
    void SelectRangeL(TInt aStart, TInt aEnd);
    void SelectAllL();
    void SetDataModelL(const RSwtListBoxItemsArray& aItemsArray);
    void SetHeadingFontL(const MSwtFont* aFont);
    void ShowSelection();
    void SetFocusIndex(TInt aIndex);

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

// New methods
private:
    /**
     * Calculate the listbox properties.
     * @param aLayoutStyle listbox layout style
     * @param aStyle control style
     * @return properties structure
     */
    TSwtLbProp ListProperties(TInt aLayoutStyle, TInt aStyle) const;

    /**
     * Create a certain type of list box
     * @param aListType one of the listbox types defined in SwtListBoxLists.h
     * @return pointer to the created listbox
     */
    CEikTextListBox* CreateListL(TInt aListType);

    /**
     * Create the scrollbars for a listbox.
     * @param aList listbox pointer
     */
    void CreateScrollBarsL(CEikTextListBox* aList) const;

    /**
     * Create a listbox image array.
     * @param aMultiSelImgs if ETrue adds checkbox icons to the image array.
     * @return image array
     */
    CArrayPtr<CGulIcon>* CreateListImgArrL(TBool aMultiSelImgs) const;

    /**
     * Create a list selection image.
     * @param aSelected state of the selection image (on/off).
     * @return pointer to created image.
     */
    CGulIcon* CreateListSelImgL(TBool aSelected) const;

    /**
     * Create or update the selection images in the list image array.
     * @param aImgArr the list image array.
     */
    void CreateListSelImgsL(CArrayPtr<CGulIcon>* aImgArr) const;

    /**
     * Insert item into list.
     * @param aItem item data.
     */
    void InsertListItemL(CSwtLbItem* aItem);

    /**
     * Create an image ready to be set to the list's image array.
     * @param aSwtImg swt image pointer.
     * @param aDetailIcon are we creating a detail icon or heading icon
     * @param aBitmapResSize size of the created bitmap is stored here
     * @return list image pointer.
     */
    CGulIcon* CreateListImgL(const MSwtImage* aSwtImg, const TBool aDetailIcon,
                             TSize& aBitmapResSize) const;

    /**
     * Insert a list image into the list (list image array)
     * @param aImg the image to be inserted.
     * @return position where the image was inserted.
     */
    TInt InsertListImgL(CGulIcon* aImg);

    /**
     * Create a formatted string ready to be set to the list's string array
     * based on the image indexes and strings of an item.
     * @param aItem item data.
     * @return formatted string.
     */
    HBufC* CreateListTxtL(const CSwtLbItem* aItem) const;

    /**
     * Insert a formatted item string into the list (list strings array)
     * @param aTxt the string to be inserted.
     * @param aIdx position where the string should be inserted.
     */
    void InsertListTxtL(const TDesC& aTxt, TInt aIdx);

    /**
     * Remove list item.
     * @param aIdx index of the item in the item array.
     */
    void DeleteListItem(TInt aIdx);

    /**
     * Remove all list items.
     */
    void DeleteAllListItems();

    /**
     * Remove list image.
     * @param aIdx index of the image in the image array.
     */
    void DeleteListImg(TInt aIdx);

    /**
     * Remove list item string.
     * @param aIdx index of the string in the string array.
     */
    void DeleteListTxt(TInt aIdx);

    /**
     * Select one item.
     * @param aIdx position of the item in the item array
     * @param aSelected new state of selection
     */
    void SelectListItemL(TInt aIdx, TBool aSelected, TBool aScroll);

    /**
     * Select a range of items.
     * @param aStart first item in range.
     * @param aEnd last item in range.
     * @param aSelected new state of selection
     */
    void SelectListItemsL(TInt aStart, TInt aEnd, TBool aSelected);

    /**
     * Select all items.
     * @param aSelected new state of selection
     */
    void SelectListItemsL(TBool aSelected);

    /**
     * Get current state of list selection.
     * @param aCurrSelItems resulting array of selected items.
     */
    void GetUserSelectionL(CArrayFix<TInt>* aCurrSelItems);

    /**
     * Set custom foreground color to the list. If the color is invalid,
     * the default color is set instead.
     */
    void UpdateListColor();

    /**
     * Update MSK. To be called after selection changed.
     */
    void UpdateListMskL();

    /**
     * Minimum width for which an item can display entirely its contents.
     * Very expensive to call. Goes trough all items and counts the maximum
     * string lengths. Asumes that the Avkon lists will always use the same
     * layouts and calculates the width based on these layouts.
     * @return minimum with for items
     */
    TInt MinimumListItemWidth() const;

    /**
     * Removes the tabs in a string and replaces them with spaces.
     * @param aString string to be processed
     */
    void TabsToSpaces(HBufC* aString) const;

    /*
     * Sets maximum icon sizes according to LAF.
     */
    void SetMaximumIconSizes();

    /*
     * Scales list images so that their size does not exceed the maximum
     * icon size from LAF.
     * @param aNewMaxDetailSize New maximum detail icon size.
     * @param aNewMaxHeadingSize New maximum heading icon size.
     */
    void UpdateImageSizes(const TSize& aNewMaxDetailSize,
                          const TSize& aNewMaxHeadingSize);

    /*
     * Scale an image so that it's size does not exceed maximum size
     * @param aImage Source image.
     * @param aIcon Update scaled image to aIcon.
     * @param aOldMaxSize Previous maximum size for images.
     * @param aNewMaxSize New maximum size for images.
     * @return Scaled size.
     */
    TSize ScaleImage(const MSwtImage* aImage, CGulIcon* aIcon,
                     const TSize& aOldMaxSize, const TSize& aNewMaxSize);

    /*
     * Remove sub-references from all images in all items.
     */
    void RemoveSubRefs();

    /**
     * Updates flick scrolling status.
     * @param aEventType CEikListbox event type.
     */
    void UpdateFlickScrollingState(TListBoxEvent aEventType);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
public:
    void DoControlSpecificFeedback(
        const TBool& aFirstTap,
        const TBool& aTappedToChildRect,
        const TPointerEvent& aPointerEvent) const;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// Data
private:
    /**
     * Contained listbox.
     * Own
     */
    CEikTextListBox* iList;

    /**
     * Cell array of the current list box
     */
    RArray<TInt> iCells;

    /**
     * Items array.
     * Own
     */
    RSwtLbItems iItems;

    /**
     * Listbox image array.
     * Own.
     */
    CArrayPtr<CGulIcon>* iImgs;

    /**
     * Listbox string array.
     * Own.
     */
    CDesCArray* iTxts;

    /**
     * Listbox properties.
     * Own.
     */
    TSwtLbProp iProps;

    /**
     * Previously selected items (temporary)
     * Own
     */
    CArrayFix<TInt>* iPrevSelItems;

    /**
     * Foreground color.
     * Own
     */
    const MSwtColor* iColor;

    /**
     * Default foreground color.
     */
    TRgb iDefColor;

    /**
     * Maximum icon sizes for the current layout.
     */
    TSize iMaxDetailIconSize;
    TSize iMaxHeadingIconSize;

    /**
     * Deliver all pointer event to the scrollbar when this on.
     */
    TBool iVScrollBarGrabsPointerEvents;

    /**
     * Stores last known focus index. Use only during paint and key handlers.
     */
    TInt iPrevFocusIndex;

    /**
     * View visible rect at last draw
     */
    mutable TRect iLastViewVisibleRect;

    /**
     * True when list is flick scrolling
     */
    TBool iFlickScrollingOngoing;

    /**
     * True when pointer events on scrollbar should be delivered to listbox
     */
    TBool iScrollbarPointerEventToListbox;
};

#endif // SWTLISTBOX_H
