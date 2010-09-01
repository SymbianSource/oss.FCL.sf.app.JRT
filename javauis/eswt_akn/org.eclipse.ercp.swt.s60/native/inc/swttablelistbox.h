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
// SwtTableListBox.h

#ifndef SWTTABLELISTBOX_H
#define SWTTABLELISTBOX_H


#include <eiklbo.h>
#include <e32std.h>
#include <eiktxlbx.h>
#include "eswt.h"


class MSwtScrollBar;
class CSwtTableItemDrawer;
class CSwtTable;
class CSwtTableListBoxView;
class MTouchFeedback;


/**
 *  This class implements a listbox control that is utilised in Table as
 *  a component control.
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTableListBox)
        : public CEikTextListBox
        , public MEikListBoxObserver
{
public:

    /**
     * Two-phased constructor.
     * @param aDisplay  The display object.
     * @param aTable    The parent table.
     */
    static CSwtTableListBox* NewL(MSwtDisplay& aDisplay, CSwtTable &aTable);

    /**
    * Destructor.
    */
    virtual ~CSwtTableListBox();

    /**
     * A helper to deselect items at specified indices.
     * @param aIndices  An array of indices to deselect.
     * @param aCount    The number of indices in the array.
     */
    void DeselectItems(const TInt* aIndices, TInt aCount) const;

    /**
     * A helper to deselect a range of items.
     * @param aStart    The first index in range to deselect.
     * @param aEnd      The last index in range to deselect.
     */
    void DeselectRange(TInt aStart, TInt aEnd);

    /**
     * Returns the default font.
     * @return The default font.
     */
    const MSwtFont& GetDefaultFont() const;

    /**
     * Returns the bounding rectangle of the specified item.
     * @param aRowIndex The row index to get.
     * @return The bounding rectangle of the item.
     */
    TRect GetItemRect(TInt aRowIndex) const;

    /**
     * Returns the number of selected items.
     * @return The number of selected items.
     */
    TInt GetSelectionCount() const;

    /**
     * Selects an item.
     * @param aIndex    The index of the item to select.
     * @param aScroll   If the list should scroll to the selected item.
     */
    void SelectItemL(TInt aIndex, TBool aScroll) const;

    /**
     * A helper to select a range of items.
     * @param aStart    The first index in range to select.
     * @param aEnd      The last index in range to select.
     */
    void SelectRangeL(TInt aStart, TInt aEnd) const;

    /**
     * Select all the items.
     */
    void SelectAllL() const;

    /**
     * A helper to set the focus to the given index item.
     * @param aIndex    The index to focus.
     */
    void SetFocusIndex(TInt aIndex) const;

    /**
     * Scrolls to the first selected item.
     */
    void ShowSelection() const;

    /**
     * Handling of the key events.
     * @param aKeyEvent The key event to process.
     * @param aType     The event code.
     */
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Handles the situation where data size has changed. I.e. the size
     * of all the content including the data that fits to the view and
     * the data outside of it.
     */
    void HandleDataSizeChangeL();

    /**
     * Draws the grid lines.
     * @param aGc           The gc to use for drawing.
     * @param aClippingRect The clipping rectangle to use.
     */
    void DrawGrid(CWindowGc& aGc, const TRect& aClippingRect) const;

    /**
     * Leaving version of the HandleResourceChange.
     * @param aType The type parameter from HandleResourceChange.
     */
    void HandleResourceChangeL(TInt aType);

    /**
     * Returns the client rectangle for the listbox.
     */
    TRect ClientRect() const;

    /**
     * Sets the required height in number of items.
     * @param aRequiredHeightInNumOfItems The value to set.
     */
    void SetRequiredHeightInNumOfItems(const TInt& aRequiredHeightInNumOfItems);

    /**
     * Gets the item drawer for the listbox.
     * @return The item drawer.
     */
    CSwtTableItemDrawer* TableItemDrawer() const;

    /**
     * Gets the view for the listbox.
     * @return The view.
     */
    CSwtTableListBoxView* TableView() const;

    /**
     * A helper to set the default font to the one used by the item drawer.
     */
    void ResetDefaultFontFromItemDrawerL();

    /**
     * Gets the font for the currently active layout.
     * @return The font.
     */
    const CFont* LayoutFont();

    /**
     * Sets the scrollbar extents to invalid values to make sure it gets updated.
     */
    void InvalidateScrollBars();

    /**
     * Handles addition of an item in a way that doesn't cause a window server
     * redraw.
     */
    void HandleItemAdditionL();

    /**
     * Does the scrollbar update unless Redraw( false ) has been called.
     */
    void UpdateScrollBarsL();

    /**
     * Tactile feedback setter needed by the drawer.
     */
    void SetTouchFeedbackRect(const TRect& aRect) const;

// From CCoeControl

    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    void FocusChanged(TDrawNow aDrawNow);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

// From CEikListBox

    void HandleViewRectSizeChangeL();
    void SetItemHeightL(TInt aHeight);
    void SetTopItemIndex(TInt aItemIndex) const;
    void UpdateCurrentItem(TInt aItemIndex) const;
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

// From MEikListBoxObserver
public:
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

protected:

    /**
     * Creates the scrollbar frame.
     * @return The scrollbar frame.
     */
    CEikScrollBarFrame* CreateScrollBarFrameL();

// From CCoeControl

    void Draw(const TRect& aRect) const;
    void PositionChanged();
    void SizeChanged();
    void HandleResourceChange(TInt aType);

// from base CEikTextListBox

    void CreateItemDrawerL();

// from base CEikListBox

    CListBoxView* MakeViewClassInstanceL();

private:

    /**
     * The constructor.
     * @param aDisplay  The display object.
     * @param aTable    The parent table.
     */
    CSwtTableListBox(MSwtDisplay& aDisplay, CSwtTable &aTable);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    TInt HorizontalScrollPixelsPerArrowEvent();
    TInt HorizontalScrollPixelsPerPageEvent();

    /**
     * Updates flick scrolling status.
     * @param aEventType CEikListbox event type.
     */
    void UpdateFlickScrollingState(TListBoxEvent aEventType);

private: // data

    /**
     * The default font.
     * Not own.
     */
    MSwtFont* iDefaultFont;

    /**
     * The Selection array before a key event, for multiselection only.
     * Own.
     */
    CArrayFix<TInt>* iOldSelectionArray;

    /**
     * The display object.
     */
    MSwtDisplay& iDisplay;

    /**
     * The parent table.
     */
    CSwtTable& iTable;

    /**
     * Feedback instance.
     * Not own.
     */
    MTouchFeedback* iFeedback;

    /**
     * Deliver all pointer event to the scrollbar when this on.
     */
    TBool iVScrollBarGrabsPointerEvents;
    TBool iHScrollBarGrabsPointerEvents;

    /**
     * Stores last known focus index. Use only during paint and key handlers.
     */
    TInt iPrevFocusIndex;

    /**
     * Used to implement horizontal panning.
     */
    TPoint iDragPos;

    /**
     * True when list is flick scrolling
     */
    TBool iFlickScrollingOngoing;

    /**
     * True when pointer events on scrollbar should be delivered to listbox
     */
    TBool iScrollbarPointerEventToListbox;
};

#endif // SWTTABLELISTBOX_H
