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


#ifndef SWTTABLEITEM_H
#define SWTTABLEITEM_H


#include <e32base.h>
#include "eswtwidgetsexpanded.h"


class CSwtImageTextItem;
class CSwtTable;


/**
 *  This class implements the native peer of TableItem.
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTableItem): public CBase, public MSwtTableItem
{

public:

    /**
     * Two-phased constructor.
     * @param aDisplay  A reference to the Display.
     * @param aPeer     The Java object associated with this C++ object
     * @param aParent   Reference to the parent table.
     * @param aRowIndex The index where to add the row in the Table.
     */
    static CSwtTableItem* NewL(MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtTable& aParent,
    TInt aRowIndex);

    /**
    * Destructor.
    */
    virtual ~CSwtTableItem();

    /**
     * Returns the Java peer object of an instance of this class.
     * @return The native peer object.
     */
    TSwtPeer JavaPeer() const;

// From MSwtTableItem

    TSwtPeer            Dispose();
    const MSwtColor*    GetBackground(TInt aColumnIndex) const;
    TRect               GetBounds(TInt aColumnIndex) const;
    TBool               GetChecked() const;
    const MSwtFont*     GetFont(TInt aColumnIndex) const;
    const MSwtColor*    GetForeground(TInt aColumnIndex) const;
    TBool               GetGrayed() const;
    TRect               GetImageBounds(TInt aColumnIndex) const;
    CSwtImageTextItem*  GetImageTextItem() const;
    TSize               GetPreferredSize() const;
    TSize               GetPreferredSize(TInt aColumnIndex) const;
    const TDesC*        GetText(TInt aColumnIndex) const;
    void                InsertNewColumnL(TInt aColumnIndex);
    void                SetBackground(TInt aColumnIndex, const MSwtColor* aColor);
    void                SetBackgroundForAll(const MSwtColor* aColor);
    void                SetFont(TInt aColumnIndex, const MSwtFont* aFont);
    void                SetFontForAll(const MSwtFont* aFont);
    void                SetForeground(TInt aColumnIndex, const MSwtColor* aColor);
    void                SetForegroundForAll(const MSwtColor* aColor);
    void                SetGrayed(TBool aIsGrayed);
    void                SetImageArray(const RPointerArray<MSwtImage>& aImages);
    void                SetImageByIndex(TInt aColumnIndex, const MSwtImage* aImage);
    void                SetTextArrayL(const CDesCArray* aTextArray);
    void                SetTextL(TInt aColumnIndex, const TDesC& aText);
    void                SetChecked(TBool aChecked);
    void                RemoveColumn(TInt aColumnIndex);

private:

    /**
     * The constructor.
     * @param aDisplay  The display object.
     * @param aPeer     The Java peer object.
     * @param aParent   The parent table that contains this item.
     */
    CSwtTableItem(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtTable& aParent);

    /**
     * 2nd phase constructor.
     */
    void ConstructL(TInt aRowIndex);

private: // data

    /**
     * The display on which the TableItem will be displayed.
     */
    MSwtDisplay& iDisplay;

    /**
     * The Java peer object
     */
    const TSwtPeer iPeer;

    /**
     * The parent table.
     */
    MSwtTable& iParent;

    /**
     * The item data.
     * Own.
     */
    CSwtImageTextItem* iItem;
};


#endif // SWTTABLEITEM_H
