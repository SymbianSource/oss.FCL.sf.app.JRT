/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTTABLEHEADER_H
#define SWTTABLEHEADER_H


#include "swttablecolumnar.h"


class CSwtTable;
class CSwtButtonProxy;
class MSwtColor;


/**
 * This class implements the Table header. It is a compound control
 * which has CSwtButtonProxy instances as components.
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTableHeader)
        : public CAknControl
        , public MSwtTableColumnar
{
public:
    /**
     * Two-phased constructor.
     * @param aDisplay      The display object
     * @param aParent       The parent table object
     * @param aImageSize    The image size common to all images
     * @param aColumnCount  Number of columns in the table
     * @param aIsHeader     If this item represents the header
     */
    static CSwtTableHeader* NewL(CSwtTable& aTable);

    /**
     * Destructor.
     */
    virtual ~CSwtTableHeader();

    /**
     * Sets the horizontal scrolling offset so that header is drawn
     * to the correct position.
     * @param  aHScrollOffset The horizontal scrolling offset
     */
    void SetHScrollOffset(const TInt& aHScrollOffset);

    /**
     * Updates the width of a column at the given index.
     * @param  aColumnIndex The index of the column to update.
     */
    void UpdateColumnWidth(const TInt& aColumnIndex, const TInt& aWidth);

    /**
     * Updates the horizontal alignment of a column at the given index.
     * @param  aColumnIndex The index of the column to update.
     */
    void UpdateColumnAlignment(const TInt& aColumnIndex, const TInt& aAlignment);

    /**
     * Sets the font to use for column header texts in all columns.
     * @param aFont The font to set.
     */
    void SetFont(const CFont* aFont);

    /**
     * Sets the foreground color to use for column header texts in all columns.
     * @param aColor The color to set. Null pointer sets the default color.
     */
    void SetForegroundL(const MSwtColor* aColor);
    void SetForegroundL(const TRgb* aColor, TInt aIndex);

    /**
     * Sets an observer for the column at specified index.
     * @param aObserver The observer to call when column in the header is selected.
     * @param aIndex    The index of the column to observe.
     */
    void SetColumnHeaderObserver(MCoeControlObserver* aObserver, const TInt& aIndex);

// From CCoeControl

    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void SizeChanged();
    void PositionChanged();
    void Draw(const TRect& aRect) const;

// From MSwtTableColumnar

    TSize GetPreferredSize() const;
    TSize GetPreferredSize(const TInt& aColumnIndex) const;
    void InsertNewColumnL(const TInt& aIndex);
    void RemoveColumn(const TInt& aIndex);
    void SetTextL(const TInt& aColumnIndex, const TDesC& aText);
    const TDesC& GetText(const TInt& aColumnIndex) const;
    TBool IsImageSet(const TInt& aColumnIndex) const;
    void SetImageByIndex(const TInt& aColumnIndex, const MSwtImage* aImage);

private:

    /**
     * The constructor.
     */
    CSwtTableHeader(CSwtTable& aTable);

    /**
     * 2nd phase constructor.
     * @param aColumnCount The number of columns at the construction. Can be zero.
     */
    void ConstructL();

    /**
     * Creates a new header button.
     * @return The newly created header button.
     */
    CSwtButtonProxy* CreateHeaderButtonL();

    /**
     * Inserts a column header button to the control.
     * @param  aColumnIndex The index to insert at
     */
    void InsertButtonL(const TInt& aColumnIndex);

    /**
     * Removes a column header button from the control.
     * @param  aColumnIndex The index to remove from
     */
    void RemoveButton(const TInt& aColumnIndex);

    /**
     * Repositions the column header buttons according to the current
     * horizontal scroll position and the parent control position.
     */
    void RepositionButtons();

    /**
     * Updates the button height at the given index to match the current
     * header height.
     */
    void UpdateButtonHeight(const TInt& aColumnIndex);

    /**
     * Updates the dummy button width to cover the area tailing
     * the column header buttons.
     */
    void UpdateDummyButtonWidth();

    /**
     * Calculates the leading extra width to column width that is available
     * for the header above the leading margin and the checkboxes.
     * @return The calculated extra width
     */
    TInt LeadingExtraWidth() const;

    /**
     * Calculates the tailing extra width to column width that is available
     * for the header above the tailing margin and the scrollbar.
     * @return The calculated extra width
     */
    TInt TailingExtraWidth() const;

    /**
     * Creates an icon from the given image for the purpose of setting it to the header.
     * The bitmap and mask of the icon are referenced, not copied.
     * @param  source Image
     * @return Pointer to the icon owned by the caller.
     */
    CGulIcon* CreateIconL(const MSwtImage* aImage);

// Data

    // This array contains the header buttons.
    RPointerArray<CSwtButtonProxy> iColumnHeaderButtons;

    // A dummy button used just for cosmetic reasons to fill the extra space
    // in the header's tailing end.
    CSwtButtonProxy* iDummyButton;

    // Reference to the Table control that hosts this header control.
    CSwtTable& iTable;

    // The current horizontal scrolling offset.
    TInt iHScrollOffset;

    // The currently used font for all buttons.
    const CFont* iFont;

    // Referenced images.
    RPointerArray<const MSwtImage> iRefImages;
};


#endif // SWTTABLEHEADER_H
