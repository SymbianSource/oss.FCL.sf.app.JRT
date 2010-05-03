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


#ifndef SWTTABLEITEMDRAWER_H
#define SWTTABLEITEMDRAWER_H


#include <eiklbi.h>
#include <eiktxlbm.h>
#include <eiklbd.h>
#include <gulicon.h>
#include "swtcontrolhelper.h"
#include "swttable.h"


class CSwtImageTextItem;
class CSwtTableListBox;
class CSwtTable;


/**
 *  This class implements the item drawer for the listbox used in Table.
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTableItemDrawer)
        : public CTextListItemDrawer
{

public:

    /**
     * The constructor.
     * @param aTextListBoxModel The model of the listbox
     * @param aFont             The font to draw with
     * @param aTableListBox     The listbox that this drawer is for
     * @param aTable            The parent table
     */
    CSwtTableItemDrawer(MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CSwtTableListBox& aTableListBox,
    CSwtTable &aTable);

    /**
    * Destructor.
    */
    virtual ~CSwtTableItemDrawer();

    /**
     * Drawing of bi-directional text in the given rectangle.
     * @param aGc           The gc used for drawing.
     * @param aFont         The font used for drawing.
     * @param aTextColor    The pen color used for drawing.
     * @param aText         The text to draw.
     * @param aTextRect     The rectangle to draw in.
     * @param aAlignment    The horizontal alignment of the text inside the rectangle.
     */
    void DrawTableBidiText(CWindowGc& aGc,
                           const CFont* aFont,
                           const TRgb& aTextColor,
                           const TDesC& aText,
                           const TRect& aTextRect,
                           const CGraphicsContext::TTextAlign& aAlignment) const;

    /**
     * Calculates the column area based on the given rectangle by removing
     * the areas that don't belong into the columns such as margins and checkboxes.
     * @param aRect The whole table data area rectangle
     * @return      The column area rectangle
     */
    TRect ColumnRectFromWholeRect(const TRect& aRect) const;

// From CTextListItemDrawer

    TInt ItemWidthInPixels(TInt aItemIndex) const;
    TSize MinimumCellSize() const;
    void DoDrawItemText(const TDesC& aDes,
                        const TRect& aItemTextRect,
                        TBool aItemIsCurrent,
                        TBool aViewIsEmphasized,
                        TBool aItemIsSelected,
                        TInt aItemIndex) const;

private: // data

    /**
     * The listbox control of this item drawer.
     */
    CSwtTableListBox& iTableListBox;

    /**
     * The parent table.
     */
    CSwtTable& iTable;
};

#endif // SWTTABLEITEMDRAWER_H
