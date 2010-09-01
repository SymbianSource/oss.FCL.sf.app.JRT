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


#ifndef SWTIMAGETEXTITEM_H
#define SWTIMAGETEXTITEM_H


#include <e32std.h>
#include "eswt.h"
#include "eswtgraphics.h"
#include "eswtwidgetsexpanded.h"
#include "swttablecolumnar.h"


class MSwtDisplay;
class MSwtControl;
class TSwtImageTextItemLayout;
class CSwtTable;


/**
 * A small helper class to encapsulate a single custom color used in drawing.
 * @lib eswt
 */
NONSHARABLE_CLASS(TSwtTableColor)
{
public:
    TSwtTableColor()
            : iIsSet(EFalse) {}
public:
    TRgb  iRgb;
    TBool iIsSet;
};

/**
 * A small helper class to encapsulate margins used in drawing.
 * @lib eswt
 */
NONSHARABLE_CLASS(TSwtTableMargins)
{
public:
    TInt iLeft;
    TInt iRight;
    TInt iTop;
    TInt iBottom;
    TInt iBetweenItems;
};

/**
 * A small helper class to encapsulate the drawing layout of
 * a single cell in a table.
 * @lib eswt
 */
NONSHARABLE_CLASS(TSwtImageTextItemLayout)
{
public:
    TSwtImageTextItemLayout()
            : iImagePos(TPoint::EUninitialized),
            iTextRect(TRect::EUninitialized)
    {
    }
public:
    TPoint iImagePos;
    TRect  iTextRect;
};

/**
 *  This class implements a list item (a row) that can contain
 *  columns with texts and images.
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtImageTextItem)
        : public CBase
        , public MSwtTableColumnar
{

public:
    /**
     * Two-phased constructor.
     * @param aDisplay      The display object
     * @param aParent       The parent table object
     * @param aImageSize    The image size common to all images
     * @param aColumnCount  Number of columns in the table
     */
    static CSwtImageTextItem* NewL(MSwtDisplay& aDisplay,
    MSwtControl& aParent,
    const TSize& aImageSize,
    const TInt& aColumnCount);

    /**
    * Destructor.
    */
    virtual ~CSwtImageTextItem();

    /**
     * Returns the background color used for the given column in this item.
     * @param aColumnIndex  The column to get
     * @return              The color of the given column in the item.
     */
    const MSwtColor* GetBackground(const TInt& aColumnIndex) const;

    /**
     * Returns the background color used for the given column in this item.
     * @param aColumnIndex  The column to get
     * @return              The color of the given column in the item.
     */
    const MSwtColor* GetForeground(const TInt& aColumnIndex) const;

    /**
     * Sets the background color of the cell at the given index in this item.
     * @param aColumnIndex  The column to set
     * @param aColor        The color to set
     */
    void SetBackground(const TInt& aColumnIndex, const MSwtColor* aColor);

    /**
     * Sets the background color for all the cells in this item.
     * @param aColor The color to set
     */
    void SetBackgroundForAll(const MSwtColor* aColor);

    /**
     * Sets the foreground color of the cell at the given index in this item.
     * @param aColumnIndex  The column to set
     * @param aColor        The color to set
     */
    void SetForeground(const TInt& aColumnIndex, const MSwtColor* aColor);

    /**
     * Sets the foreground color for all the cells in this item.
     * @param aColor The color to set
     */
    void SetForegroundForAll(const MSwtColor* aColor);

    /**
     * Returns the bounds of the specified cell.
     * @param aItemRect The bounding rectangle of the whole item
     * @param aIndex    The column index to get
     * @return          The bounding rectangle
     */
    TRect GetBounds(const TRect& aItemRect, const TInt& aIndex) const;

    /**
     * Returns the bounds of the specified image.
     * @param aItemRect     The bounding rectangle of the whole item
     * @param aColumnIndex  The column index to get
     * @return              The bounding rectangle of the image.
     */
    TRect GetImageBounds(const TRect& aItemRect, const TInt& aColumndex) const;

    /**
     * Returns the font of the cell at the specified index.
     * @param aColumnIndex  The column index to get
     * @return              The font
     */
    const MSwtFont* GetFont(const TInt& aColumnIndex) const;

    /**
     * Sets the font of the cell at the specified index.
     * @param aColumnIndex  The column index to set
     * @param aFont         The font to set. If NULL, the default font is set.
     * @return              The  height of the used font.
     */
    TInt SetFont(const TInt& aColumnIndex, const MSwtFont* aFont);

    /**
     * Sets the font for all cells.
     * @param aFont The font to set. If NULL, the default font is set.
     */
    void SetFontForAll(const MSwtFont* aFont);

    /**
     * Returns the Grayed state of the item.
     */
    TBool GetGrayed() const;

    /**
     * Sets the Grayed state of the item.
     * @param aIsGrayed True for grayed, false for non-grayed.
     */
    void SetGrayed(const TBool& aIsGrayed);

    /**
     * Returns the check state.
     * @return True for checked, false for not checked.
     */
    TBool IsChecked() const;

    /**
     * Sets the the text alignment array.
     * @param aColumnAlignments The array to use.
     */
    void SetColumnAlignmentsArray(const RArray<TInt>& aColumnAlignments);

    /**
     * Sets the image count array.
     * @param aColumnImageCounts The array to use.
     */
    void SetColumnImageCounts(const RArray<TInt>& aColumnImageCounts);

    /**
     * Sets the pointer to the column widths array.
     * @param aColumnWidths The array to use.
     */
    void SetColumnWidthsArray(const RArray<TInt>& aColumnWidths);

    /**
     * Computes the preferred list-mode width of the item.
     * @return The preferred width
     */
    TInt GetPreferredListModeItemWidth();

    /**
     * Returns the margins for the current layout.
     * @return Margins
     */
    TSwtTableMargins GetMargins() const;

    /**
     * Draws the item.
     * @param aGc               The gc to use for drawing.
     * @param aItemDrawer       The item drawer for the list control that we are drawing for.
     * @param aDrawRect         The rectangle to draw in.
     * @param aClippingRect     The rectangle to clip to.
     * @param aItemIsCurrent    True if this item is the currently focused one. Then highlight will be drawn.
     * @param aItemIsSelected   True if this item is currently selected.
     */
    void Draw(CWindowGc& aGc,
              const CListItemDrawer& aItemDrawer,
              const TRect& aDrawRect,
              const TRect& aClippingRect,
              TBool aItemIsCurrent,
              TBool aItemIsSelected) const;

    /**
     * Sets the text of the cells from an array of texts.
     * The texts are set starting from the first column.
     * The number of texts in the array can be different than the number of columns.
     * @param aTextArray The array of texts to set.
     */
    void SetTextArrayL(const CDesCArray* aTextArray);

    /**
     * Sets the images of the cells from an array of images.
     * The images are set starting from the first column.
     * The number of images in the array can be different than the number of columns.
     * @param aImages The array of images to set.
     */
    void SetImageArray(const RPointerArray<MSwtImage>& aImages);

    /**
     * This is used when drawing rectangles with graphics to reduce the size of the graphic to
     * the to the visible area only. In case there are frame graphics in the rectangle about
     * to be drawn then the frames will appear unaffected by this optimisation if the size of
     * the frame area is given.
     * @param aDrawRect     The rectangle about to be drawn.
     * @param aFrameLeft    The width of the left frame area in the rectangle.
     * @param aFrameRight   The width of the right frame area in the rectangle.
     * @param aFrameTop     The height of the top frame area in the rectangle.
     * @param aFrameBottom  The height of the bottom frame area in the rectangle.
     */
    void ReduceRectToVisibleArea(TRect& aDrawRect,
                                 const TInt& aFrameLeft = 0,
                                 const TInt& aFrameRight = 0,
                                 const TInt& aFrameTop = 0,
                                 const TInt& aFrameBottom = 0) const;

// From base class MSwtTableColumnar
    void SetGridLineWidth(const TInt& aPixelWidth);
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
     * @param aDisplay      The display
     * @param aParent       The parent table
     * @param aImageSize    The common size for all the images.
     */
    CSwtImageTextItem(MSwtDisplay& aDisplay,
                      MSwtControl& aParent,
                      const TSize& aImageSize);

    /**
     * 2nd phase constructor.
     * @param aColumnCount The number of columns at the construction. Can be zero.
     */
    void ConstructL(TInt aColumnCount);

    /**
     * Returns the size of the bitmap of the checkbox.
     * @return The size.
     */
    TSize GetCheckBoxSize() const;

    /**
     * Returns the number of columns in the table.
     * @return The number of columns.
     */
    TInt GetColumnCount() const;

    /**
     * Gets the bitmap and mask of the checkbox.
     * @param aBitmap   The bitmap of the checkbox graphic
     * @param aMask     The mask of the checkbox graphic
     * @aItemIsSelected True if the item is selected
     */
    void GetCheckBoxBitmaps(const CFbsBitmap** aBitmap,
                            const CFbsBitmap** aMask,
                            const TBool& aItemIsSelected) const;

    /**
     * Draws the checkbox for the item.
     * @param aGc The gc to use for drawing.
     * @param aDrawRect The rectangle to draw in .
     * @param aItemIsSelected True if the item is selected.
     */
    void DrawCheckBox(CWindowGc& aGc,
                      const TRect& aDrawRect,
                      const TBool& aItemIsSelected) const;

    /**
     * Draws one cell of the item.
     * @param aGc               The graphics context
     * @param aItemDrawer       The item drawer of the list.
     * @param aDrawRect         The bounds of the cell to draw
     * @param aColumnIndex      The index of the column to draw.
     * @param aAlignment        The alignment of the column.
     * @param aItemIsCurrent    True if the item is the current.
     * @param aLayoutMirrored   True if the layout is right-to-left.
     */
    void DrawCell(CWindowGc& aGc,
                  const CListItemDrawer& aItemDrawer,
                  const TRect& aCellRect,
                  const TRect& aItemClippingRect,
                  const TInt& aColumnIndex,
                  const TBool& aItemIsCurrent,
                  const TBool& aLayoutMirrored) const;

    /**
     * Sets if the text in the item should be truncated with ellipsis
     * if they can't be displayed completely due to going outside of
     * the visible area of the control. The texts are always truncated
     * at the edge of a column despite of this setting.
     * @param True if should truncate always, false for columns only
     */
    void TruncateText(const TBool& aIsUsing);

    /**
     * Computes the coordinates of the image and the text of a cell,
     * given the draw rectangle and the column index of the cell.
     * @param aDrawRect        The rectangle in which the cell will be drawn.
     * @param aColumnIndex     The column index of the cell.
     * @param aItemCoordResult The object which will contain the coordinates.
     * @param aLayoutMirrored  True if the layout is right-to-left.
     */
    void LayoutCell(const TRect& aDrawRect,
                    const TInt& aColumnIndex,
                    TSwtImageTextItemLayout& aItemCoordResult,
                    const TBool& aLayoutMirrored) const;

    /**
     * Calculates the layout for a checkbox inside the parent rectangle.
     * @param aParentRect The parent rectangle.
     * @param aLayoutMirrored   True if the layout is right-to-left.
     * @return            The rectangle for the checkbox.
     */
    TRect CheckBoxRect(const TRect& aParentRect, const TBool& aLayoutMirrored) const;

    /**
     * Adds horizontal margins appropriately when given the item rectangle.
     * @param aRowRect The item rectangle which will be modified.
     */
    void LeaveHorizontalMargin(TRect& aRowRect) const;

    /**
     * Adds vertical margins appropriately when given the item rectangle.
     * @param aColumnRect The item rectangle which will be modified.
     */
    void LeaveVerticalMargin(TRect& aColumnRect) const;

    /**
     * Returns the alignment for the given column appropriately mirrored.
     * @param aColumnIndex      The column index to get.
     * @param aLayoutMirrored   True if the layout is right-to-left.
     * @return                  The alignment of the specified column.
     */
    CGraphicsContext::TTextAlign Alignment(const TInt& aColumnIndex,
                                           const TBool& aLayoutMirrored) const;

    /**
     * Creates a new native Color object.
     * @param aRgb The color to create.
     * @return     The native color object.
     */
    MSwtColor* NewColorNativeObjectL(const TRgb& aRgb) const;

    /**
     * Returns the foreground color rgb value used for the given column index
     * in this item.
     * @param aColumnIndex The column index to get.
     * @return             The rgb value.
     */
    TRgb GetForegroundRgb(const TInt& aColumnIndex) const;

    /**
     * Returns the highlight color rgb value used for the given column index
     * in this item.
     * @param aColumnIndex The column index to get.
     * @return             The rgb value.
     */
    TRgb GetHighlightForegroundRgb(const TInt& aColumnIndex) const;

    /**
     * Returns the background color rgb value used for the given column index
     * in this item.
     * @param aColumnIndex  The column index to get.
     * @param aAlsoReturn   True if color should be returned even for multi color background
     * @return              The color
     */
    TSwtTableColor GetBackgroundRgb(const TInt& aColumnIndex = -1,
                                    const TBool& aAlsoReturnRgbForWallPaper = EFalse) const;

    /**
     * If hide selection style has been specified then the selection indicators
     * are not drawn for a non-focused control.
     * @return Returns ETrue if the selection indicators should not be drawn.
     */
    TBool HideSelection() const;

    /**
     * Creates an icon from the given image for the purpose of setting it to the item.
     * The bitmap and mask of the icon are referenced, not copied.
     * @param  source Image
     * @return Pointer to the icon owned by the caller.
     */
    CGulIcon* CreateIconL(const MSwtImage* aImage);

    /**
     * Frees the CGulIcon at position aIndex in iImages and
     * Removes the reference of the MSwtImage at position aIndex in iRefImages
     * Does not shrink the image arrays.
     */
    void DeleteIcon(const TInt& aColumnIndex);


private: // data

    /**
     * The display on which the items will be displayed.
     */
    MSwtDisplay& iDisplay;

    /**
     * The parent table.
     */
    MSwtControl& iParent;

    /**
     * The Grayed state.
     */
    TBool iIsGrayed;

    /**
     * True if the the table is multi-select. I.e. check or multi style.
     */
    TBool iIsMultiSelect;

    /**
     * The custom background color for the entire item, if set.
     */
    TSwtTableColor iCommonBackground;

    /**
     * The custom foreground color for the entire item, if set.
     */
    TSwtTableColor iCommonForeground;

    /**
     * The common size of all the images. It is crucial that it does not change.
     * The referenced images rely on it throughout the whole lifespam of the item.
     */
    const TSize& iImageSize;

    /**
     * True if long texts will be clipped and truncated by ellipsis in the case
     * part of the text is not visible. If the column is not wide enough to
     * display the complete text then it is truncated despite this setting.
     */
    TBool iTruncateText;

    /**
     * The width of the grid lines.
     */
    TInt iGridLineWidth;

    /**
     * Images for every cell in the item.
     * Entries are owned.
     */
    RPointerArray<CGulIcon> iImages;

    /**
     * Referenced images.
     * Entries not owned.
     */
    RPointerArray<const MSwtImage> iRefImages;

    /**
     * Texts for every cell in the item.
     * Entries are owned.
     */
    RPointerArray<HBufC> iTexts;

    /**
     * Fonts for every cell in the item, if set.
     * Entries not own.
     */
    RPointerArray<const MSwtFont> iFonts;

    /**
     * Background colors for every cell in the item, if set.
     */
    RArray<TSwtTableColor> iBackgrounds;

    /**
     * Foreground colors for every cell in the item, if set.
     */
    RArray<TSwtTableColor> iForegrounds;

    /**
     * The custom font font the entire item if set.
     * Not own.
     */
    const MSwtFont* iCommonFont;

    /**
     * The array containing the widths of the columns.
     * Not own.
     */
    RArray<TInt> const* iColumnWidths;

    /**
     * The array containing the alignments of the columns.
     * Not own.
     */
    RArray<TInt> const* iColumnAlignments;

    /**
     * The array containing the number of images in each column, including the header.
     * Not own.
     */
    RArray<TInt> const* iColumnImageCounts;
};


#endif // SWTIMAGETEXTITEM_H
