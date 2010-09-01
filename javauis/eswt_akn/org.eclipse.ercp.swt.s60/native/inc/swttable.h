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


#ifndef SWTTABLE_H
#define SWTTABLE_H


#include "eswtwidgetsexpanded.h"
#include "swtscrollablebase.h"
#include "swtcomposite.h"


class MSwtTableItem;
class MSwtTableColumn;
class CSwtTableListBox;
class CSwtImageTextItem;
class TSwtTableMargins;
class CSwtTableHeader;


/**
 *  This class implements the native peer of Table.
 *  Table is implemented as a compound control with 2 components:
 *  a listbox control and a column header control.
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTable)
        : public CSwtComposite
        , public MSwtTable
        , public MCoeControlObserver
{
public:
    /**
     * Two-phased constructor.
     * @param aDisplay  The display object
     * @param aPeer     The Java peer of the table
     * @param aParent   The parent Composite of the table
     * @param aStyle    The style flags given to the table Java constructor
     */
    static CSwtTable* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

    /**
    * Destructor.
    */
    virtual ~CSwtTable();

    /**
     * Returns the component listbox items.
     * @return An array of items.
     */
    const RPointerArray<CSwtImageTextItem>& GetImageTextItems() const;

    /**
     * A helper for posting a selection event for an item in the Table.
     * @param aDetail       Detail code for the event.
     * @param aItemIndex    The index of the selected item.
     */
    void PostSelectionEventL(TInt aDetail, TInt aItemIndex) const;

    /**
     * A helper for posting a default selection event for an item in the Table.
     * @param The index of the selected item.
     */
    void PostDefaultSelectionEventL(TInt aItemIndex) const;

    /**
     * A helper for posting a selection event for a column in the Table.
     * @param The native peer of the selected column.
     */
    void PostColumnSelectionEventL(TSwtPeer aColumn) const;

    /**
     * A helper for posting a resize event for a column in the Table.
     * @param The native peer of the selected column.
     */
    void PostColumnResizeEventL(TSwtPeer aColumn) const;

    /**
     * A helper for posting a move event for a column in the Table.
     * @param The native peer of the selected column.
     */
    void PostColumnMoveEventL(TSwtPeer aColumn) const;

    /**
     * Returns the position and extent of the header.
     * @return The header rectangle.
     */
    TRect GetHeaderRect() const;

    /**
     * Return the margins common to all items.
     * @return The margins to use in the currently active layout.
     */
    TSwtTableMargins GetMargins() const;

    /**
     * Return the checkbox size common to all items.
     * @return The checkbox size to use in the currently active layout.
     */
    TSize GetCheckBoxSize() const;

    /**
     * Return the checkbox bitmaps common to all items.
     * @param aChecked  True if the checked state bitmap should be returned, false for unchecked
     * @param aBitmap   aBitmap Returned checkbox bitmap. Owned by this class.
     * @param aMask     aMask   Returned checkbox mask. Owned by this class.
     */
    void GetCheckBoxBitmaps(const TBool& aChecked, const CFbsBitmap** aBitmap,
                            const CFbsBitmap** aMask) const;

    /**
     * Gets index of an item by its CSwtImageTextItem pointer.
     * @param aItem The pointer.
     * @return      The index. -1 if not found.
     */
    TInt IndexOfTableItem(const CSwtImageTextItem* aItem) const;

    /**
     * Returns the highlighted text color rgb value to use in the currently active layout.
     * @return The rgb value.
     */
    TRgb GetHighlightTextRgb() const;

    /**
     * Returns if the table background has a custom color set for it.
     * @return True if a custom color has been set, false otherwise.
     */
    TBool BackgroundSet() const;

    /**
     * Returns if the item at the specified index has been checked.
     * Items can only be checked in KSwtStyleCheck style table.
     * @param aIndex    The index of the item to query.
     * @return          True if the item has been checked, false otherwise.
     */
    TBool IsChecked(const TInt& aIndex) const;

    /**
     * Checks the item at the specified index.
     * Items can only be checked in KSwtStyleCheck style table.
     * @param aIndex    The index of the item to check.
     */
    void CheckItem(const TInt& aIndex);

    /**
     * Unchecks the item at the specified index.
     * Items can only be checked in KSwtStyleCheck style table.
     * @param aIndex    The index of the item to uncheck.
     */
    void UncheckItem(const TInt& aIndex);

    /**
     * @return True if the table is operating in list-mode (no TableColumns).
     */
    TBool ListMode() const;

    /**
     * Unchecks the item at the specified index.
     * Items can only be checked in KSwtStyleCheck style table.
     * @param aHScrollOffset
     */
    void UpdateHeaderHScrollOffset(const TInt& aHScrollOffset);

    /**
     * Notifies the Table compound control that the default font in the component
     * list box has changed.
     */
    void ListBoxDefaultFontChanged();

    /**
     * Get notified by the contained listbox about kinetic scrolling
     */
    void HandlePhysics(TBool aStatus);

    /**
     * If a fg colors has been set, get its rgb otherwise get NULL.
     */
    const TRgb* CustomForeground() const;

// from base class CCoeControl

    TInt            CountComponentControls() const;
    CCoeControl*    ComponentControl(TInt aIndex) const;
    void            SizeChanged();
    void            HandleResourceChange(TInt aType);
    TKeyResponse    OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void            FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr   MopSupplyObject(TTypeUid aId);
    void            Draw(const TRect& aRect) const;

// from base class MSwtTable

    CSwtPeerArray*  GetTableItemsL();
    CSwtPeerArray*  GetTableColumnsL();
    const RArray<TInt>& GetColumnWidths() const;
    const RArray<TInt>& GetColumnAlignments() const;
    const RArray<TInt>& GetColumnImageCounts() const;
    MSwtComposite*  Composite();
    void            AddTableItemL(MSwtTableItem& aChild, TInt aRowIndex);
    void            AddTableColumnL(MSwtTableColumn& aChild, TInt aColumnIndex);
    void            RemoveTableColumn(MSwtTableColumn& aChild);
    void            DeselectItems(const TInt* aIndices, TInt aCount);
    void            DeselectRange(TInt aStart, TInt aEnd);
    TInt            IndexOfTableColumn(const MSwtTableColumn *aColumn) const;
    TInt            IndexOfTableItemAt(const MSwtTableItem *aItem, TInt aStart) const;
    TInt            IndexOfTableItem(const MSwtTableItem *aItem) const;
    TInt            GetColumnCount() const;
    TInt            GetTableColumnCount() const;
    TBool           GetHeaderVisible() const;
    TSwtPeer        GetItemL(TInt aIndex) const;
    TSwtPeer        GetColumnL(TInt aIndex) const;
    TInt            GetItemCount() const;
    TRect           GetItemRect(TInt aRowIndex) const;
    TBool           GetLinesVisible() const;
    TInt            GetSelectionIndex() const;
    TInt            GetSelectionCount() const;
    const CArrayFix<TInt>* GetSelectionIndicesL() const;
    const TSize&    ImageSize() const;
    void            NotifyImageAdded(TInt aColumnIndex);
    void            NotifyImageRemoved(TInt aColumnIndex);
    void            PackColumn(TInt aColumnIndex);
    void            RemoveItemL(TInt aIndex);
    void            RemoveRangeL(TInt aStart, TInt aEnd);
    void            RemoveItemsL(const TInt* aIndices, TInt aCount);
    void            RemoveAll();
    void            SelectItemL(TInt aIndex, TBool aScroll);
    void            SetHeaderVisible(TBool aShow);
    void            SetLinesVisible(TBool aShow);
    void            SelectRangeL(TInt aStart, TInt aEnd);
    void            SelectAllL();
    void            SetFocusIndex(TInt aIndex);
    void            ShowSelection();
    CSwtPeerArray*  GetSelectionL();
    TBool           IsSelected(TInt aIndex) const;
    TInt            GetTopIndex() const;
    void            SetTopIndex(TInt aIndex);
    void            SetColumnTextL(MSwtTableColumn& aTableColumn, const TDesC& aText);
    TInt            GetColumnWidth(TInt aColumnIndex) const;
    TInt            GetColumnAlignment(TInt aColumnIndex);
    void            SetColumnWidth(TInt aColumnIndex, TInt aWidth);
    void            SetColumnAlignment(TInt aColumnIndex, TInt aAlignment);
    void            SetTableImageSizeIfNotDefined(const TSize& aSize,
            const TBool& aAddedToItem = ETrue);
    void            UpdateItemHeight(const TInt& aChangedFontHeight);
    TInt            GetGridLineWidth() const;
    TInt            GetHeaderHeight() const;
    void            ShowTableColumn(MSwtTableColumn& aTableColumn);
    void            ShowTableItem(MSwtTableItem& aTableItem);
    TInt            GetItemHeight() const;
    TSwtPeer        GetItemFromPoint(TPoint &aPoint) const;
    const TRgb&     GetDefaultForeground() const;
    const TDesC&    GetColumnText(TInt aColumnIndex) const;
    void            SetColumnImage(TInt aColumnIndex, const MSwtImage* aImage);

// from base class MSwtControl

    CCoeControl&        CoeControl();
    const CCoeControl&  CoeControl() const;
    TBool               IsFocusable(TInt aReason = KSwtFocusByApi) const;
    TInt                FocusBackgroundPolicy() const;
    TSize               ComputeSizeL(TInt aWHint, TInt aHHint);
    TInt                GetBorderWidth() const;
    TRect               ClientRect() const;
    void                ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TRgb                GetBackground() const;
    TRgb                GetForeground() const;
    void                SetBackgroundL(const MSwtColor* aColor);
    void                SetForegroundL(const MSwtColor* aColor);
    void                SetFontL(const MSwtFont* aFont);
    TBool               IsKeyUsed(TUint aKeyCode) const;
    TBool               MSKSelCmdEnabled() const;
    void                UpdateDoNotDrawFlag();
#ifdef RD_JAVA_S60_RELEASE_9_2
    void                EnableFocusHighlight(TBool aEnable);
#endif //RD_JAVA_S60_RELEASE_9_2
    void                PrepareForTraverse();

// from base class ASwtScrollableBase

    CEikScrollBarFrame* SbFrame() const;
    TRect               SbInnerRect(const TRect& aRect) const;
    TRect               SbOuterRect(const TRect& aRect) const;
    void                ComputeTrim(TRect& aRect) const;
    void                HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

// from base class CSwtComposite

    void RegisterChildL(MSwtControl& aChild);

// from base class MCoeControlObserver

    void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);

// from base class ASwtControlBase

public:
    const MSwtFont& DefaultFont() const;
protected:
    HBufC*          MSKLabelL() const;

private:

    /**
     * The constructor.
     * @param aDisplay  The display object.
     * @param aPeer     The java peer table object.
     * @param aParent   The parent composite.
     * @param aStyle    The style flags passed to the Java constructor.
     */
    CSwtTable(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Gets the currently focused item index.
     * @return The index.
     */
    TInt GetFocusIndex() const;

    /**
     * Recalculates the layout for the whole control and applies the new layout.
     */
    void LayoutTable();

    /**
     * A leaving version of HandleResourceChange
     * @param aType The type param received in HandleResourceChange
     */
    void HandleResourceChangeL(TInt aType);

    /**
     * Loading of the scaled layout data.
     */
    void LoadLayouts();

    /**
     * Loading of the scaled & skinned graphic data.
     */
    void LoadGraphicsL();

    /*
     * Restores the selection indexes from the given array after items have been added or removed.
     * @param aStart            The starting index after which items were added or removed.
     * @param aSelectionArray   The selection array containing the selection indexes before
                                adding/removing items.
     * @param aItemsAdded       True if items were added, false if removed.
     */
    void UpdateSelectionIndexesL(const TInt& aStart,
                                 const TInt& aEnd,
                                 CArrayFix<TInt>* aSelectionArray,
                                 const TBool& aItemsAdded);

    /**
     * Updates the middle-softkey label.
     */
    void UpdateMSKLabelL();

private: // data

    /**
     * Header visibility state.
     */
    TBool iHeaderVisible;

    /**
     * The grid visibility state.
     */
    TBool iLinesVisible;

    /**
     * The unique size for all the images.
     */
    TSize iImageSize;

    /**
     * If a custom background color has been set for the table.
     */
    TBool iTableBackgroundSet;

    /**
     * The custom background color set to table.
     */
    TRgb iTableBackground;

    /**
     * If a custom foreground color has been set for the table.
     */
    TBool iTableForegroundSet;

    /**
     * The custom foreground color set to table.
     */
    TRgb iTableForeground;

    /**
     * The skinned default foreground color used for unset/null.
     */
    TRgb iTableDefaultForeground;

    /**
     * The skinned default highlight text color used if foreground is not set.
     */
    TRgb iTableDefaultHighlightText;

    /**
     * This array contains the width of each column.
     */
    RArray<TInt> iColumnWidths;

    /**
     * This array contains the alignment of each column.
     */
    RArray<TInt> iColumnAlignments;

    /**
     * This array contains the number of images in the items of each column.
     * The images in the header are not included in this count.
     */
    RArray<TInt> iColumnImagesCounts;

    /**
     * Scaled layout data common to all items.
     */
    TInt iHighlightTopBottomMargin;
    TInt iHighlightLeftRightMargin;
    TSize iCheckBoxSize;
    TInt iGridLineWidth;

    /**
     * The native listbox control.
     * Own.
     */
    CSwtTableListBox* iTableListBox;

    /**
     * The header of the table.
     * Own.
     */
    CSwtTableHeader* iHeader;

    /**
     * The array of drawable listbox items.
     * Not own.
     */
    RPointerArray<CSwtImageTextItem> iImageTextItems;

    /**
     * The array of native TableItem peers.
     * Not own.
     */
    RPointerArray<MSwtTableItem> iTableItems;

    /**
     * The array of native TableColumn peers.
     * Not own.
     */
    RPointerArray<MSwtTableColumn> iTableColumns;

    /**
     * The Selection array for storing the selection state, for multiselection only.
     * Own.
     */
    CArrayFix<TInt>* iOldSelectionArray;

    /**
     * Scaled & skinned graphics common to all items.
     * Own.
     */
    CFbsBitmap* iCheckBoxOffBitmap;
    CFbsBitmap* iCheckBoxOffMask;
    CFbsBitmap* iCheckBoxOnBitmap;
    CFbsBitmap* iCheckBoxOnMask;
};

#endif // SWTTABLE_H
