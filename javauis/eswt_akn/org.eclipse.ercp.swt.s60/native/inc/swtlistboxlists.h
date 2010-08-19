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


#ifndef SWTLISTBOXLISTS_H
#define SWTLISTBOXLISTS_H


#include <aknlists.h>
#include "eswtwidgetscore.h"
#include "swtlistboxlistsitemdrawer.h"

template <class, class, class> class CSwtListBoxTemplate;


/** List box types. Same order as in KSwtLbCells */
enum TSwtLbType
{
    ESwtLbSingle = 0,
    ESwtLbSingleGraphic,
    ESwtLbSingleLarge,
    ESwtLbSingleHeading,
    ESwtLbSingleHeadingGraphic,
    ESwtLbDouble,
    ESwtLbDoubleGraphic,
    ESwtLbDoubleLarge,
    ESwtLbTypeCount
};

/** Avkon list boxes for each list box type */
typedef CSwtListBoxTemplate <CAknSingleStyleListBox,
CSwtColumnListBoxItemDrawer,
CColumnListBoxData> CSwtListBoxSingle;
typedef CSwtListBoxTemplate <CAknSingleGraphicStyleListBox,
CSwtColumnListBoxItemDrawer,
CColumnListBoxData> CSwtListBoxSingleGraphic;
typedef CSwtListBoxTemplate <CAknSingleLargeStyleListBox,
CSwtColumnListBoxItemDrawer,
CColumnListBoxData> CSwtListBoxSingleLarge;
typedef CSwtListBoxTemplate <CAknSingleHeadingStyleListBox,
CSwtSingleHeadingStyleListBoxItemDrawer,
CColumnListBoxData> CSwtListBoxSingleHeading;
typedef CSwtListBoxTemplate <CAknSingleGraphicHeadingStyleListBox,
CSwtSingleHeadingStyleListBoxItemDrawer,
CColumnListBoxData> CSwtListBoxSingleHeadingGraphic;
typedef CSwtListBoxTemplate <CAknDoubleStyleListBox,
CSwtFormattedCellListBoxItemDrawer,
CFormattedCellListBoxData> CSwtListBoxDouble;
typedef CSwtListBoxTemplate <CAknDoubleGraphicStyleListBox,
CSwtFormattedCellListBoxItemDrawer,
CFormattedCellListBoxData> CSwtListBoxDoubleGraphic;
typedef CSwtListBoxTemplate <CAknDoubleLargeStyleListBox,
CSwtDoubleLargeStyleItemDrawer,
CFormattedCellListBoxData> CSwtListBoxDoubleLarge;

/** List box cells */
enum TSwtLbCellType
{
    ECellInvalid = 0,
    ECellEmpty,
    ECellDtxt,
    ECellDimg,
    ECellHtxt,
    ECellHimg
};

/** Maximum number of cells per item */
const TInt KMaxCellCount = 5;

/** List box cells for each list box type. Same order as in TSwtLbType */
const TInt KSwtLbCells[ESwtLbTypeCount][KMaxCellCount] =
{
    { ECellEmpty, ECellDtxt, ECellDimg, 0,         0 }, // ESwtLbSingle
    { ECellHimg,  ECellDtxt, ECellDimg, 0,         0 }, // ESwtLbSingleGraphic
    { ECellHimg,  ECellDtxt, ECellDimg, 0,         0 }, // ESwtLbSingleLarge
    { ECellHtxt,  ECellDtxt, ECellDimg, 0,         0 }, // ESwtLbSingleHeading
    { ECellHimg,  ECellHtxt, ECellDtxt, ECellDimg, 0 }, // ESwtLbSingleHeadingGraphic
    { ECellEmpty, ECellHtxt, ECellDtxt, ECellDimg, 0 }, // ESwtLbDouble
    { ECellHimg,  ECellHtxt, ECellDtxt, ECellDimg, 0 }, // ESwtLbDoubleGraphic
    { ECellHimg,  ECellHtxt, ECellDtxt, ECellDimg, 0 }, // ESwtLbDoubleLarge
};

/** List box selection types */
enum TSwtLbSelectionType
{
    ESwtLbSingleSelection = 0,
    ESwtLbMultiSelection
};


/**
 *  MSwtListObserver
 */
class MSwtListObserver
{
public:
    virtual void HandleSizeChangedL() = 0;
    virtual MSwtUiUtils& Utils() const = 0;
};

/**
 *  CSwtListBoxListTemplate
 *  @lib eswt
 */
template<class CType, class CSwtListBoxItemDrawerType, class CListBoxDataType>
NONSHARABLE_CLASS(CSwtListBoxTemplate)
        : public CType
{
// New methods
public:
    /**
     * Factory
     */
    static CSwtListBoxTemplate<CType, CSwtListBoxItemDrawerType,
    CListBoxDataType>* NewL();

    /**
     * Sets item drawer's clipping rectangle
     */
    void SetItemDrawerClippingRect(const TRect& aRect);

    /**
     * Sets margins.
     */
    void SetMargins(TInt aHorizontal, TInt aVertical);

    /**
     * Destructor
     */
    ~CSwtListBoxTemplate();

private:
    /**
     * First Phase Constructor
     */
    CSwtListBoxTemplate();

    /**
     * Second Phase Constructor
     */
    void ConstructL();

// From CCoeControl
public:
    void PositionChanged();
    void SizeChanged();

// From CCoeControl
protected:
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From CEikListBox
public:
    void SetItemHeightL(TInt aHeight);

// From CType
protected:
    void CreateItemDrawerL();

// New methods
public:
    /**
     * Set a list observer.
     * @param aListObserver The list observer
     */
    void SetListObserver(MSwtListObserver* aListObserver);

private:
    /**
     * Calculate area capable of displaying client data.
     * @param aRect
     * @param aBorder
     * @param aSBrame
     * @param aExcludeScrollBars
     */
    TRect ClientRect(TRect aRect, TGulBorder aBorder,
                     CEikScrollBarFrame* aSBrame, TBool aExcludeScrollBars) const;

    /**
     * Size or position of this control has changed.
     */
    void RectChangedL(TBool aDoHandleSizeChanged);

// Data
private:
    MSwtListObserver* iListObserver;
};


/**
 * CSwtListBoxLists
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtListBoxLists)
        : public CBase
{
// New methods
public:
    /**
     * Create a list box.
     * @param aListType see TSwtLbType
     * @return pointer to created list box or NULL upon error.
     */
    static CEikTextListBox* NewListL(TInt aListType);

    /**
     * Set a list observer to a list.
     * @param aListType see TSwtLbType
     * @param aList Pointer to the list
     * @param aListObserver The list observer
     * @return pointer to created list box or NULL upon error.
     */
    static void SetListObserver(
        TInt aListType,
        CEikTextListBox* aList,
        MSwtListObserver* aListObserver);

    /**
     * Check if a certain type of listbox is formatted.
     * @param aListType see TSwtLbType
     * @return ETrue if listbox is formatted.
     */
    static TBool IsListFormatted(TInt aListType);

    /**
     * Compute an array of cell types for a certain list box type
     * @param aListType see TSwtLbType
     * @return ETrue if listbox is formatted.
     */
    static void CellsL(TInt aListType, RArray<TInt>& aCellArray);

    /**
     * Check if a int value is in the range of listbox types.
     * @param aVal listbox type or int value.
     * @return ETrue if value is listbox type.
     */
    static TBool IsListType(TInt aVal);

    /**
     * Sets custom clipping rectangle of the list's item drawer
     */
    static void SetItemDrawerClippingRect(
        TInt aListType,
        CEikTextListBox* aList,
        const TRect& aRect);

    /**
     * Sets listbox's margins
     */
    static void SetMargins(
        TInt aListType,
        CEikTextListBox* aList,
        TInt aHorizontal, TInt aVertical);

    /**
     * Change stretching
     */
    static void EnableStretching(
        TInt aListType,
        CEikTextListBox* aList,
        TBool aEnabled);

#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Enabling focus highlight for lists
     */
    static void EnableFocusHighlight(
        CListItemDrawer* aItemDrawer,
        TBool aEnable);
#endif //RD_JAVA_S60_RELEASE_9_2
};

#endif // SWTLISTBOXLISTS_H
