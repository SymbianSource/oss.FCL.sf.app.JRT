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


#ifndef ESWTWIDGETSEXPANDED_H
#define ESWTWIDGETSEXPANDED_H


#include <eiklbi.h>
#include "eswt.h"
#include "eswtwidgetscore.h"


class MSwtListBase;
class MSwtColor;
class CSwtFontData;
class MSwtTable;
class CGraphicsContext;


class MSwtClipboard
{
protected:
    virtual ~MSwtClipboard() {}

public:
    /**
     * Disposes of (i.e. destroys) the control
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Clears the Clipboard.
     */
    virtual void ClearL() =0;

    /**
     * Gets the types of the data available in the Clipboard.
     * @param aTypes Upon call, this array must contain the data types to
     *        enquire about. Upon return, the non-available types will
     *        have been removed from it.
     */
    virtual void GetAvailableTypesL(RArray<TInt>& aTypes) const =0;

    /**
     * Returns the contents of the clipboard, as an array of bytes.
     * @param aTypeId Identifier of the expected type of the data
     *        contained in the clipboard.
     * @return the data, or NULL if no data of type aTypeId is found
     *        the clipboard.
     */
    virtual HBufC8* GetByteContentsL(TInt aDataId) const =0;

    /**
     * Returns the contents of the clipboard, as a text descriptor.
     * @return the text descriptor, or NULL if no text is found in the clipboard.
     */
    virtual HBufC* GetTextContentsL() const =0;

    /**
     * Set the content of the clipboard with a byte descriptor.
     * @param aPtr      The byte descriptor.
     * @param aJavaType The Java type identifier of the data.
     */
    virtual void SetByteContentL(const TDesC8& aPtr, TInt aJavaType) =0;

    /**
     * Set the content of the clipboard with a text descriptor.
     * @param aPtr The text descriptor.
     */
    virtual void SetTextContentL(const TDesC& aPtr) =0;
};


class MSwtTableColumn
{
protected:
    virtual ~MSwtTableColumn() {}

public:
    virtual TSwtPeer JavaPeer() const =0;

    /**
     * Disposes of (i.e. destroys) the TableColumn.
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the alignment
     */
    virtual TInt Alignment() const =0;

    /**
     * Sets the column to its preferred size.
     */
    virtual void Pack() =0;

    /**
     * Sets the alignment of the column.
     */
    virtual void SetAlignment(TInt aAlignment) =0;

    /**
     * Sets the text of the column.
     */
    virtual void SetTextL(const TDesC& aText) =0;

    /**
     * Returns the text of the TableColumn
     *
     * @result A pointer ot the text. Cannot be null.
     */
    virtual const TDesC* GetText() const =0;

    /**
     * Sets the width of the column.
     */
    virtual void SetWidth(TInt aWidth) =0;

    /**
     * Returns the width of the column.
     */
    virtual TInt Width() const =0;

    /**
     * set the image of the TableColumn
     */
    virtual void SetColumnImage(const MSwtImage* aImage) =0;

    /**
     * Returns whether the column can be resized by the user
     */
    virtual TBool GetResizable() const = 0;

    /**
     * Sets the column's ability to be resized by the user
     */
    virtual void SetResizable(TBool aResizable) =0;

    /**
     * Returns this column's style flags
     */
    virtual TInt GetStyle() const =0;
};


class MSwtTableItem
{
protected:
    virtual ~MSwtTableItem() {}

public:
    /**
     * Disposes of (i.e. destroys) the TableItem.
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the background at the specified index.
     */
    virtual const MSwtColor* GetBackground(TInt aColumnIndex) const =0;

    /**
     * Returns the receiver's size and location relative to its parent
     * @param aColumnIndex  the column index
     */
    virtual TRect GetBounds(TInt aColumnIndex) const =0;

    /**
     * Returns the check state of the receiver.
     * If the Table was created without the CHECK style flag, this
     * method returns EFalse
     */
    virtual TBool GetChecked() const =0;

    /**
     * Returns the font used at the specified index.
     */
    virtual const MSwtFont* GetFont(TInt aColumnIndex) const =0;

    /**
     * Returns the foreground at the specified index.
     */
    virtual const MSwtColor* GetForeground(TInt aColumnIndex) const =0;

    /**
     * Returns the gray state.
     * If the Table was created without the CHECK style flag, this
     * method returns EFalse
     */
    virtual TBool GetGrayed() const =0;

    /**
     * Returns the  size and location of an image, relative to its parent
     * @param aColumnIndex  the column index
     */
    virtual TRect GetImageBounds(TInt aColumnIndex) const =0;

    /**
     * Returns the preferred width of the cell at index aColumnIndex,
     * depending on its font, image and checkbox.
     */
    virtual TSize GetPreferredSize(TInt aColumnIndex) const =0;

    /**
     * Returns the preferred width of the entire row,
     * depending on its font, image and checkbox.
     */
    virtual TSize GetPreferredSize() const =0;

    /**
     * Returns the text of the item, specified by its index.
     * @param aColumnIndex   the column index
     * @return The text, cannot be NULL.
     */
    virtual const TDesC* GetText(TInt aColumnIndex) const =0;

    /**
     * Insert a new column to the row.
     * No text or images is set. The font used is the default font.
     */
    virtual void InsertNewColumnL(TInt aColumnIndex) =0;

    /**
     * Remove a column from the row.
     */
    virtual void RemoveColumn(TInt aColumnIndex) =0;

    /**
     * Sets the background of the cell at index aColumnIndex.
     */
    virtual void SetBackground(TInt aColumnIndex, const MSwtColor* aColor) =0;

    /**
     * Sets the background for the entire row,
     */
    virtual void SetBackgroundForAll(const MSwtColor* aColor) =0;

    /**
     * Sets the font of the cell at index aColumnIndex.
     */
    virtual void SetFont(TInt aColumnIndex, const MSwtFont* aFont) =0;

    /**
     * Sets the font for the entire row.
     */
    virtual void SetFontForAll(const MSwtFont* aFont) =0;

    /**
     * Sets the foreground of the cell at index aColumnIndex,
     */
    virtual void SetForeground(TInt aColumnIndex, const MSwtColor* aColor) =0;

    /**
     * Sets the foreground for the entire row.
     */
    virtual void SetForegroundForAll(const MSwtColor* aColor) =0;

    /**
     * Sets the gray state.
     * If the Table was created without the CHECK style flag, this
     * method does nothing.
     */
    virtual void SetGrayed(TBool aIsGrayed) =0;

    /**
     * Sets an array of texts to the row.
     */
    virtual void SetTextArrayL(const CDesCArray* aTextArray) =0;

    /**
     * Sets the text to display at the specified index .
     */
    virtual void SetTextL(TInt aColumnIndex, const TDesC& aText) =0;

    /**
     * Sets an array of images to the row.
     */
    virtual void SetImageArray(const RPointerArray<MSwtImage>& aImages) =0;

    /**
     * Sets the image at the specified index.
     */
    virtual void SetImageByIndex(TInt aColumnIndex, const MSwtImage* aImage) =0;

    /**
     * Sets the check state of the receiver.
     */
    virtual void  SetChecked(TBool aChecked)=0;

    /**
     * Returns the Java Peer.
     */
    virtual TSwtPeer JavaPeer() const =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Table class.
 */
class MSwtTable
{
protected:
    virtual ~MSwtTable() {} // Made protected to prevent destruction through the interface

public:

    /**
     * Returns the MSwtComposite interface
     */
    virtual MSwtComposite* Composite() =0;

    /**
     * Returns the TableItem array of the Table
     */
    virtual CSwtPeerArray* GetTableItemsL() =0;

    /**
     * Returns the TableColumn array of the Table
     */
    virtual CSwtPeerArray* GetTableColumnsL() =0;

    /**
     * Returns an array of TableItems that are currently
     * selected in the receiver.
     * Returned array is null if no item is selected.
     */
    virtual CSwtPeerArray* GetSelectionL() =0;

    /**
     * Register a TableItem
     * @param aChild the TableItem
     * @param aRowIndex where to add the TableItem
     */
    virtual void AddTableItemL(MSwtTableItem& aChild, TInt aRowIndex) =0;

    /**
     * Register a TableColumn
     * @param aChild the TableColumn
     * @param aColumnIndex where to add the TableColumn
     */
    virtual void AddTableColumnL(MSwtTableColumn& aChild, TInt aColumnIndex) =0;

    /**
     * Unregister a TableColumn
     * @param aChild the TableColumn
     */
    virtual void RemoveTableColumn(MSwtTableColumn& aChild) =0;

    /**
     * Returns the default font.
     */
    virtual const MSwtFont& DefaultFont() const =0;

    /**
     * Deselects the items defined in the aIndices array from the <a>Table</a>.
     * @param aIndices  The array of the items to deselect.
     * @param aCount   The number of items of this array.
     */
    virtual void DeselectItems(const TInt* aIndices, TInt aCount) =0;

    /**
     * Deselects the items indexed from aStart to aEnd from the <a>Table</a>.
     * @param aStart    The first element to deselect.
     * @param aEnd      The last element to deselect.
     */
    virtual void DeselectRange(TInt aStart, TInt aEnd) =0;

    /**
     * Searches the receiver's list starting at the first column (index 0)
     * until a column is found that is equal to the argument, and returns
     * the index of that column.
     * @param aColumn the TableColumn
     * @return the index of the Tablecolumn
     */
    virtual TInt IndexOfTableColumn(const MSwtTableColumn *aColumn) const =0;

    /**
     * Searches the receiver's list starting at the first item (index 0)
     * until an item is found that is equal to the argument, and returns
     * the index of that item.
     * @param aItem the TableItem
     * @return the TableItem
     */
    virtual TInt IndexOfTableItemAt(const MSwtTableItem *aItem, TInt aStart) const =0;

    virtual TInt IndexOfTableItem(const MSwtTableItem *aItem) const =0;

    /**
     * Returns the number of displayed columns in the Table.
     * A table with no TableColumn behaves as a List, displaying a dummy column
     * and therefore the method will return 1. To know the number of TableColumns
     * in the table, use GetTableColumnCount().
     * @return the number of columns in the table. Value is always strictly positive
     */
    virtual TInt GetColumnCount() const =0;

    /**
     * Returns the number of TableColumns in the Table.
     * A Table with no TableColumn will display a dummy column
     * and will behave as a list. GetTableColumnCount() will return 0 in that case.
     * To know how many columns are displayed, use GetColumnCount().
     * @return the number of TableColumn in the table.
     */
    virtual TInt GetTableColumnCount() const =0;

    /**
     * Returns ETrue if the receiver's header is visible, and EFalse otherwise.
     * the value is EFalse by default.
     * @return  ETrue if the header is visible.
     */
    virtual TBool GetHeaderVisible() const =0;

    /**
     * Returns ETrue if the item is selected, and EFalse otherwise.
     */
    virtual TBool   IsSelected(TInt aIndex) const =0;

    /**
     * Returns the column at the given, zero-relative index in the Table.
     */
    virtual TSwtPeer GetColumnL(TInt aIndex) const =0;

    /**
     * Returns a reference to the array of the alignments of the columns.
     */
    virtual const RArray<TInt>& GetColumnAlignments() const =0; // LeaveScan misreport

    /**
     * Returns a reference to the array of the widths of the columns.
     */
    virtual const RArray<TInt>& GetColumnWidths() const =0;

    /**
     * Returns a reference to the array of number of images of the columns.
     */
    virtual const RArray<TInt>& GetColumnImageCounts() const =0;

    /**
     * Returns the width in pixels of a grid line.
     */
    virtual TInt GetGridLineWidth() const =0;

    /**
     * Returns the height of the receiver's header
     */
    virtual TInt GetHeaderHeight() const =0;

    /**
     * Returns the item at the given, zero-relative index in the Table.
     * @param aIndex the index of the row
     * @return the item at the given, zero-relative index in the Table.
     */
    virtual TSwtPeer GetItemL(TInt aIndex) const =0;

    /**
     * Returns the number of items in the Table
     */
    virtual TInt GetItemCount() const =0;

    /**
     * Returns the rectangle of the specified TableItem, given an index in the Table.
     * @param aRowIndex the row index of the Table
     * @return the on-screen rectangle of the specified TableItem.
     */
    virtual TRect GetItemRect(TInt aRowIndex) const =0;

    /**
     * Returns ETrue if the receiver's lines are visible,
     * and EFalse otherwise. The default value is EFalse.
     */
    virtual TBool GetLinesVisible() const =0;

    /**
     * Gets the current selected item. return -1 if no item is selected.
     */
    virtual TInt GetSelectionIndex() const =0;

    /**
     * Returns the number of selected elements.
     */
    virtual TInt GetSelectionCount() const =0;

    /**
     * Returns the zero-relative index of the item which is currently
     * at the top of the receiver.
     */
    virtual TInt GetTopIndex() const =0;

    /**
     * Get the selected items indices.
     * @return  An array of indexes owned by the caller.
     */
    virtual const CArrayFix<TInt>* GetSelectionIndicesL() const =0;

    /**
     * Returns a reference to the image size used in the table.
     * The image size is the size of the first image added in the table
     * all other images in the table will use this size.
     * @return the image size used in the table.
     */
    virtual const TSize& ImageSize() const =0;

    /**
     * Notifies the Table that an image has been set in the specified column.
     * @param aColumnIndex the index of the column where an image has been added.
     */
    virtual void NotifyImageAdded(TInt aColumnIndex) =0;

    /**
     * Notifies the Table that an image has been removed in the specified column.
     * @param aColumnIndex the index of the column where an image has been removed.
     */
    virtual void NotifyImageRemoved(TInt aColumnIndex) =0;

    /**
     * Sets the column specified by index to its preferred size.
     * @param aColumnIndex the index of the Column to pack.
     */
    virtual void PackColumn(TInt aColumnIndex) =0;

    /**
     * Removes the specified item from the <a>Table</a>.
     * @param aIndex    The specified item.
     */
    virtual void RemoveItemL(TInt aIndex) =0;

    /**
     * Removes a range of items from the <a>Table</a>.
     * @param aStart    The first element to remove.
     * @param aEnd      The last element to remove.
     */
    virtual void RemoveRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Removes the items defined in the aIndices array from the <a>Table</a>.
     * @param aIndices  The array of the items to remove.
     * @param aCount    The number of items of this array.
     */
    virtual void RemoveItemsL(const TInt* aIndices, TInt aCount) =0;

    /**
     * Removes all items from the <a>Table</a>.
     */
    virtual void RemoveAll() =0;

    /**
     * Selects a single item.
     * @param aIndex    The item to select.
     */
    virtual void SelectItemL(TInt aIndex, TBool aScroll) =0;

    /**
     * Selects a range of items.
     *
     * @param aStart    The first item to select.
     * @param aEnd      The last item to select.
     */
    virtual void SelectRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Selects all items.
     */
    virtual void SelectAllL() =0;

    /**
     * Sets the current focused item.
     *
     * @param aIndex    The item to focus on.
     */
    virtual void SetFocusIndex(TInt aIndex) =0;

    /**
     * update the text of the column
     * @param aTableColumn the TableColumn to set the text.
     * @param aText the text to set
     */
    virtual void SetColumnTextL(MSwtTableColumn& aTableColumn, const TDesC& aText) =0;

    /**
     * return the text of the column
     * @param aColumnIndex the index of the column to get the text
     */
    virtual const TDesC& GetColumnText(TInt aColumnIndex) const =0;

    /**
     * Marks the receiver's header as visible if the argument is ETrue,
     * and marks it invisible otherwise
     * @param aShow the column visibility
     */
    virtual void SetHeaderVisible(TBool aShow) =0;

    /**
     * Sets the image size of the table, if the image size is currently TSize(0, 0).
     * @param aSize         The size of the image.
     * @param aAddedToItem  Should be EFalse if image is being added to TableColumn.
     *                      In this case item height is not updated.
     */
    virtual void SetTableImageSizeIfNotDefined(const TSize& aSize,
            const TBool& aAddedToItem = ETrue) = 0;

    /**
     * Marks the receiver's lines as visible if the argument is true,
     * and marks it invisible otherwise
     * @param aShow the grid visibility
     */
    virtual void SetLinesVisible(TBool aShow) =0;

    /**
     * Sets the zero-relative index of the item which is currently at the top of the receiver.
     * @param aIndex the index of the row to set on top in the Table
     */
    virtual void SetTopIndex(TInt aIndex) =0;

    /**
     * Shows the selection.
     */
    virtual void ShowSelection() =0;

    /**
     * return the width of a column in the table
     * @param aColumnIndex the index of the column
     * @return the width of a column in the table
     */
    virtual TInt GetColumnWidth(TInt aColumnIndex) const =0;

    /**
     * Return the column alignment of a column in the table.
     * First column is always left aligned.
     * @param aColumnIndex the index of the column
     * @return the alignment of the column, of type CGraphicsContext::TTextAlign
     */
    virtual TInt GetColumnAlignment(TInt aColumnIndex) =0;

    /**
     * set the width in pixel of a column in the table
     * @param aColumnIndex the index of the column
     * @param aWidth the width in pixel
     */
    virtual void SetColumnWidth(TInt aColumnIndex, TInt aWidth) =0;

    /**
     * set the column alignment of a column in the table
     * @param aColumnIndex the index of the column.
     * @param aAlignment the column's alignment, of type CGraphicsContext::TTextAlign
     */
    virtual  void SetColumnAlignment(TInt aColumnIndex, TInt aAlignment) =0;

    /**
     * Updates the height of the rows if necessary.
     * @param aChangedFontHeight New relevant height.
     */
    virtual void UpdateItemHeight(const TInt& aChangedFontHeight) =0;

    /**
     * Show the TableColumn
     * @param aTableColumn the TableColumn to show
     */
    virtual void ShowTableColumn(MSwtTableColumn& aTableColumn) =0;

    /**
     * Show the TableItem
     * @param aTableItem the TableItem to show
     */
    virtual void ShowTableItem(MSwtTableItem& aTableItem) =0;

    /**
     * Return the height of a TableItem, in pixels. All TableItem have the same height.
     * @return the item Height
     */
    virtual TInt GetItemHeight() const =0;

    /**
     * Returns the item at the given point in the receiver, or null if no such item exists.
     * @param aPoint a point in the receiver
     * @return The TableItem peer under this point
     */
    virtual TSwtPeer GetItemFromPoint(TPoint &aPoint) const =0;

    /**
     * set the image of a column in the Table (will appear on the header)
     * @param aColumnIndex the index of the column
     * @param aImage the image to set in the column
     */
    virtual void SetColumnImage(TInt aColumnIndex, const MSwtImage* aImage) =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Tree class.
 */
class MSwtTree
{
protected:
    virtual ~MSwtTree() {} // Made protected to prevent destruction through the interface

public:
    virtual MSwtComposite* Composite() = 0;
    MSwtScrollable* Scrollable()
    {
        return Composite()->Scrollable();
    }
    MSwtControl* Control()
    {
        return Composite()->Control();
    }
    virtual TInt AddL(TInt aItemHandle) = 0;
    virtual TRect Bounds(TInt aItemHandle) const = 0;
    virtual void Check(TInt aItemHandle, TBool aState) = 0;
    virtual void Collapse(TInt aItemHandle) = 0;
    virtual void Expand(TInt aItemHandle) = 0;
    virtual TInt InsertL(TInt aItemHandle, TInt aIndex) = 0;
    virtual TBool IsExpanded(TInt aItemHandle) const = 0;
    virtual TBool IsSelected(TInt aItemHandle) const = 0;
    virtual TInt ItemAt(TInt aX, TInt aY) const = 0;
    virtual TInt ItemHeight() const = 0;
    virtual void Remove(TInt aItemHandle) = 0;
    virtual void RemoveAll() = 0;
    virtual void Select(const TInt* aItemHandles, TInt aCount, TBool aState) = 0;
    virtual void SelectAll(TBool aState) = 0;
    virtual const CArrayFix<TInt>* SelectionL() const = 0;
    virtual TInt SelectionCount() const = 0;
    virtual void SetImageL(TInt aItemHandle, const MSwtImage* aImage) = 0;
    virtual void SetTextL(TInt aItemHandle, const TDesC& aText) = 0;
    virtual TInt TopItem() const = 0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.FontDialog class.
 */
class MSwtFontDialog
{
protected:
    virtual ~MSwtFontDialog() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Initializes the FontDialog object in order to prepare the display with the selected font.
     */
    virtual void SetInitialSelection(const TDesC* aName, TInt aHeight, TInt aStyle) =0;

    /**
     * Sets the initial font color.
     */
    virtual void SetInitialRGB(TInt aRed, TInt aGreen, TInt aBlue) =0;

    /**
     * Displays the dialog.
     */
    virtual TBool OpenL() =0;

    /**
     * Simple getter. Returns the selected font name or null if none was selected.
     */
    virtual const TDesC* GetFontName() const = 0;

    /**
     * Simple getter. Returns the selected font size.
     */
    virtual TInt GetFontHeight() const = 0;

    /**
     * Simple getter. Returns the selected font style.
     */
    virtual TInt GetFontStyle() const = 0;

    /**
     * Simple getter. Returns the selected font color.
     */
    virtual TInt GetFontRGB() const = 0;

    /**
     * Disposes (delete) the font object.
     */
    virtual void Dispose() =0;
};


/**
 * Interface class for the DirectoryDialog java class.
 */
class MSwtDirectoryDialog
{
protected:
    virtual ~MSwtDirectoryDialog() {}

public:
    /**
     * Displays the dialog.
     * @return A buffer containing the path and filename of the selected file,
     *         or NULL if Cancel button is used to close the dialog. The user
     *         has to release this buffer.
     */
    virtual HBufC* OpenLD() =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Browser class
 * A class that implements MSwtBrowser must also implement MSwtConposite.
 */
class MSwtBrowser
{
protected:
    /**
     * Destructor
     */
    virtual ~MSwtBrowser() {} // Made protected to prevent destruction through the interface

public:

    /**
     * @returns the MSwtComposite interface
     */
    virtual MSwtComposite* Composite() = 0;

    /**
     * @returns the MSwtScrollable interface
     */
    MSwtScrollable* Scrollable()
    {
        return Composite()->Scrollable();
    }

    /**
     * @returns the MSwtControl interface
     */
    MSwtControl* Control()
    {
        return Composite()->Control();
    }

    /**
     * Navigates to previous URL from history.
     * @return ETrue if back command was sent to browser contorl without error, and EFalse otherwise.
     */
    virtual TBool BackL() = 0;

    /**
     * Navigates to next URL from history
     * @return ETrue if forward command was sent to browser contorl without error, and EFalse otherwise.
     */
    virtual TBool ForwardL() = 0;

    /**
     * Get current URL
     * @return the Pointer to the current URL
     */
    virtual HBufC* GetUrlL() = 0;

    /**
     * Get availability of previous session history item
     * @return ETrue if previous session history item is available, and EFalse otherwise.
     */
    virtual TBool IsBackEnabled() const = 0;

    /**
     * Get availability of next session history item
     * @return ETrue if next session history item is available, and EFalse otherwise.
     */
    virtual TBool IsForwardEnabled() const = 0;

    /**
     * Refresh the current page
     * @return void.
     */
    virtual void  RefreshL() = 0;

    /**
     * Stop any loading and rendering activity
     * @return void.
     */
    virtual void  StopL() = 0;

    /**
     * Renders HTML.
     * @return ETrue if the operation was successful and EFalse otherwise. .
     */
    virtual TBool SetTextL(const TDesC&  aHtml) = 0;

    /**
     * Loads a URL.
     * @return ETrue if the operation was successful and EFalse otherwise. .
     */
    virtual TBool SetUrlL(const TDesC&  aUrl) = 0;

    /**
     * Load a URL based on the @param aOperationType. The function must be called by callback event framework.
     * Don't call it from anywhere else
     * @param aUrl,the URL to load
     * @param  aOperationType, the loading operation type
     * @return void
     */
    virtual void  DoSetUrlL(const TDesC&  aUrl, TSwtBrCallBackOperationType aOperationType) = 0;

    /**
     * Handles free ram memory event.
     * @param aEventType The type of event.
     * @return void.
     */
    virtual void HandleFreeRamEventL(TInt aEventType) = 0;
};

#endif // ESWTWIDGETSEXPANDED_H
