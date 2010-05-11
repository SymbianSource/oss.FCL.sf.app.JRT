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


#ifndef SWTGRID_H
#define SWTGRID_H


#include <AknGrid.h>


class CSwtListView;


/*
 * CSwtGrid
 *
 * This class is used to override some of the functionality in
 * S60 Grid class CAknGrid. These overrides are needed in order
 * to enable resizing and repositioning of the grid. The grid
 * is used in the ListView widget.
 *
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtGrid)
        : public CAknGrid
{
public:
    CSwtGrid(CSwtListView* aCompoundCtrl);

// From CCoeControl
    void PositionChanged();
    void SizeChanged();

// From CAknGrid.
    void UpdateScrollBarsL();
    void UpdateScrollBarThumbs() const;

private:
    CSwtGrid() {}

// New functions
public:
    /**
     * Calculates the area capable of displaying client data.
     */
    TRect ClientRect(const TRect& aRect, const TBool& aExcludeScrollBars) const;

    /**
     * Turn on / off scrollbars visibility states.
     */
    void SetScrollbarVisibility(TInt aStyle, TBool aState);

    /**
     * Apply the visibility states to the scrollbars.
     */
    void ApplyScrollbarVisibility();

    /**
     * Updates item drawer's custom clipping rectangle.
     */
    void SetItemDrawerClippingRect(const TRect& aRect);

    /**
     * Wrapper for protected member function HandleScrollEventL of CAknGrid.
     */
#if defined( RD_SCALABLE_UI_V2)
    void DoHandleScrollEventL(CEikScrollBar* aScrollBar,
                              TEikScrollEvent aEventType);
#endif

// From CAknGrid
protected:
    void CreateItemDrawerL();

private:
    /**
     * Called when size or position has changed.
     */
    void RectChangedL();

// Data
private:
    /**
     * Pointer to the compound control.
     */
    CSwtListView* iCompoundCtrl;

    /**
     * State of vertical scrollbar visibility.
     */
    TBool iVScrollVisible;

    /**
     * State of horizontal scrollbar visibility.
     */
    TBool iHScrollVisible;
};

#endif // SWTGRID_H
