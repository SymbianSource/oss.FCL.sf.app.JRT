/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implements the Form LCDUI component.
*
*/


#ifndef CMIDFORMROW_H
#define CMIDFORMROW_H

// CCoeControl inheritance in CMIDFormRow class
// and API from CCoeControl
#include <coecntrl.h>

#include <lcdui.h>

class CMIDForm;
class CMIDControlItem;


/**
 * A row in a form. Manages form items that have been assigned to this
 * row by the form.
 *
 * @see CMIDForm, CMIDControlItem
 */
NONSHARABLE_CLASS(CMIDFormRow) : public CCoeControl
{
public:
    static CMIDFormRow* NewL(CMIDForm& aForm);
    ~CMIDFormRow();

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
#ifdef RD_JAVA_S60_RELEASE_9_2    
    void Draw(const TRect& /*aRect*/) const;
#endif // RD_JAVA_S60_RELEASE_9_2    
    virtual void SizeChanged();
    virtual TSize MinimumSize();

public: // interface to CMIDForm
    void AppendL(CMIDControlItem* aItem);
    void SetAlignment(MMIDItem::TLayout aAlignment);
    void SizeItemsL();
    TInt NumItems();
    TInt Find(const CMIDControlItem* aItem) const;
    CMIDControlItem* Item(TInt aIndex);
    TBool HasFocusableItemOnOrAboveScreen();
    TBool HasFocusableItemOnOrBelowScreen();
    CMIDControlItem* FirstFocusableItemOnScreen();
    CMIDControlItem* FirstFocusableItem();
    CMIDControlItem* LastFocusableItem();
    TBool RemoveItemIfExists(CMIDControlItem& aItem);
    TInt CurrentWidth();

    TBool HasNonSpacerItems() const;

    inline void SetEmptyRowSize(const TSize& aEmptyRowSize);
    inline const TSize& EmptyRowSize() const;

private:
    CMIDFormRow(CMIDForm& aForm);
    void ConstructL();
#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Checks whether the separator line must be drawn
     * above this form row.
     */
    void SetSeparator();
#endif // RD_JAVA_S60_RELEASE_9_2

private:
    CMIDForm& iForm;
    MMIDItem::TLayout iAlignment; // ELeft, ERight, or ECenter
    RArray<CMIDControlItem*> iItems;
    TSize  iEmptyRowSize; // size used for empty rows
    TInt iCurrentWidth;             // used when adding items to the row. Keeps track of the total width of items added to row
#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Separator line is drawn between Form rows (except between Form rows that
     * belong together e.g. concatenated StringItems).
     */
    TBool iSeparator;
#endif // RD_JAVA_S60_RELEASE_9_2    
};


inline void CMIDFormRow::SetEmptyRowSize(const TSize& aEmptyRowSize)
{
    iEmptyRowSize = aEmptyRowSize;
}

inline const TSize& CMIDFormRow:: EmptyRowSize() const
{
    return iEmptyRowSize;
}

#endif // CMIDFORMROW_H
