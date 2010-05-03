/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTLISTBOXLISTSITEMDRAWER_H
#define SWTLISTBOXLISTSITEMDRAWER_H


#include <aknlists.h>
#include "eswtwidgetscore.h"

/**
 * ASwtListBoxItemDrawerBase
 * Base class for eSWT item drawers implementing custom clipping of items.
 */
NONSHARABLE_CLASS(ASwtListBoxItemDrawerBase)
{
public:
    /**
     * Sets clipping to prevent drawing out of list parent's rectangle.
     */
    void SetCustomClippingRect(const TRect& aRect);

// New methods
protected:
    void BeginClipping(CWindowGc* aGc) const;
    void EndClipping(CWindowGc* aGc) const;

// Data
private:
    /**
     * Contains current clipping rectangle
     */
    TRect iCustomClippingRect;
};

/**
 * CSwtColumnListBoxItemDrawer
 */
NONSHARABLE_CLASS(CSwtColumnListBoxItemDrawer)
        : public CColumnListBoxItemDrawer
        , public ASwtListBoxItemDrawerBase
{
public:
    CSwtColumnListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont, CColumnListBoxData* aListBoxData,
    CEikListBox* aOwner);

    ~CSwtColumnListBoxItemDrawer();
    
// From CListItemDrawer
protected:
    void DrawItemText(TInt aItemIndex,
                      const TRect& aItemTextRect, TBool aItemIsCurrent,
                      TBool aViewIsEmphasized, TBool aItemIsSelected)  const;
};

/**
 * CSwtSingleHeadingStyleListBoxItemDrawer
 */
NONSHARABLE_CLASS(CSwtSingleHeadingStyleListBoxItemDrawer)
        : public CSingleHeadingStyleItemDrawer
        , public ASwtListBoxItemDrawerBase
{
public:
    CSwtSingleHeadingStyleListBoxItemDrawer(
        MTextListBoxModel* aTextListBoxModel, const CFont* aFont,
        CColumnListBoxData* aListBoxData, CEikListBox* aOwner);

    ~CSwtSingleHeadingStyleListBoxItemDrawer();
    
// From CListItemDrawer
protected:
    void DrawItemText(TInt aItemIndex,
                      const TRect& aItemTextRect, TBool aItemIsCurrent,
                      TBool aViewIsEmphasized, TBool aItemIsSelected)  const;
};

/**
 * CSwtFormattedCellListBoxItemDrawer
 */
NONSHARABLE_CLASS(CSwtFormattedCellListBoxItemDrawer)
        : public CFormattedCellListBoxItemDrawer
        , public ASwtListBoxItemDrawerBase
{
public:
    CSwtFormattedCellListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont, CFormattedCellListBoxData* aListBoxData,
    CEikListBox* aOwner);

    ~CSwtFormattedCellListBoxItemDrawer();

// From CListItemDrawer
protected:
    void DrawItemText(TInt aItemIndex,
                      const TRect& aItemTextRect, TBool aItemIsCurrent,
                      TBool aViewIsEmphasized, TBool aItemIsSelected)  const;
};

/**
 * CSwtDoubleLargeStyleItemDrawer
 */
NONSHARABLE_CLASS(CSwtDoubleLargeStyleItemDrawer)
        : public CDoubleLargeStyleItemDrawer
        , public ASwtListBoxItemDrawerBase
{
public:
    CSwtDoubleLargeStyleItemDrawer(MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont, CFormattedCellListBoxData* aListBoxData,
    CEikListBox* aOwner);

    ~CSwtDoubleLargeStyleItemDrawer();

// From CListItemDrawer
protected:
    void DrawItemText(TInt aItemIndex,
                      const TRect& aItemTextRect, TBool aItemIsCurrent,
                      TBool aViewIsEmphasized, TBool aItemIsSelected)  const;
};

#endif // SWTLISTBOXLISTSITEMDRAWER_H
