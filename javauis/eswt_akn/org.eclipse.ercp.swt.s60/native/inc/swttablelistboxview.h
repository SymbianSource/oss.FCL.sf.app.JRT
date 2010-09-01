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


#ifndef SWTTABLELISTBOXVIEW_H
#define SWTTABLELISTBOXVIEW_H


#include <eiklbv.h>


class CSwtTable;


/**
 * The view implementation for CSwtTableListBox.
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTableListBoxView)
        : public CListBoxView
{

public:

    /**
     * The constructor.
     * @param aTable The parent table
     */
    CSwtTableListBoxView(CSwtTable &aTable);

    /**
     * Check if it is possible to scroll the view further left.
     * @return True if scrolling is possible.
     */
    TBool CanScrollLeft();

    /**
     * Check if it is possible to scroll the view further right.
     * @return True if scrolling is possible.
     */
    TBool CanScrollRight();

    /**
     * Sets the horizontal scrolling position to a valid position after
     * a change in data or view widths that might have invalidated the position.
     * Might not do anything if the position is already valid.
     */
    void ValidateHScrollOffset();

    /**
     * Sets the horizontal scrolling position all the way to the leadging edge of the table.
     * I.e. the edge with the first column.
     */
    void HScrollToLeadingEdge();

    /**
     * Scrolls the view horizontally to bring the specified column into the view.
     * @param aColumnIndex The column index to scroll to.
     */
    void ScrollToMakeColumnVisible(const TInt& aColumnIndex);

    /**
     * Sets the horizontal scroll offset in pixels.
     *
     * @param aHorizontalOffset New value for the horizontal scroll offset, in
     *        pixels.
     */
    void SetHScrollOffset(const TInt& aHorizontalOffset);

    /**
     * These just forward the call to the corresponding functions in the CListBoxView.
     */
    void DoSetFlags(TInt aMask);
    void DoClearFlags(TInt aMask);

// From CListBoxView

    void HScroll(TInt aHScrollAmount);
    void CalcDataWidth();
    void Draw(const TRect* aClipRect = NULL) const;

private:

    /**
     * Scrolls the view horizontally to the one of the specified offsets
     * that is closer to the current scrolling position.
     * @param aHScrollOffset1 candidate offset 1
     * @param aHScrollOffset2 candidate offset 1
     */
    void HScrollMin(const TInt& aHScrollOffset1, const TInt& aHScrollOffset2);

private: // data

    /**
     * The parent table.
     */
    CSwtTable& iTable;
};

#endif // SWTTABLELISTBOXVIEW_H
