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
* Description:  ?Description
*
*/


#ifndef CMIDLABELCONTAINERITEM_H
#define CMIDLABELCONTAINERITEM_H

// CMIDControlItem inheritance in class declaration
#include "CMIDControlItem.h"
#include "lcdui.h"

const TInt KLabelContainerItem = 1000;

/**
    A class used for wrapping string item content into more than one
    row. This item contains a reference to a string item and one of its
    CEikLabel objects. These can either be label or content strings.
    This class relies on the string item to have already wrapped its text.
    It's only purpose is so that a string item can span more than one
    row.
*/
NONSHARABLE_CLASS(CMIDLabelContainerItem) : public CMIDControlItem
{
public:
    static CMIDLabelContainerItem* NewL(CMIDStringItem& aStringItem,
    CEikLabel& aLabel, TBool aHasHorizMargin);

    //
    // From CCoeControl
    //
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& /*aRect*/) const;
    void SizeChanged();
    void FocusChanged(TDrawNow aDrawNow);
    void SetContainerWindowL(const CCoeControl& aContainer);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

    //
    // From CMIDControlItem
    //
    TBool IsSelectable() const;
    TInt ItemPreferredHeightWithoutLabel();
    inline TInt Type();

    inline CMIDStringItem& StringItem();

    // Functions for setting/getting flag iIsStringItemContent
    TBool IsStringItemContent();
    void SetIsStringItemContent(TBool aIsStringItemContent);

private:
    CMIDLabelContainerItem(CMIDStringItem& aStringItem,
                           CEikLabel& aLabel, TBool aHasHorizMargin);

    ~CMIDLabelContainerItem();
    void ConstructL();

private:
    /** Reference to the string item whose labels we are wrapping */
    CMIDStringItem& iStringItem;

    /** The label inside the string item that we are wrapping */
    CEikLabel& iLabel;

    /** True when we need to draw the text with an x margin */
    TBool iHasHorizMargin;

    /**
     * True - label container item is created from string item content
     * False - label container item is created from string item label
     */
    TBool iIsStringItemContent;

};

inline TInt CMIDLabelContainerItem::Type()
{
    return KLabelContainerItem;
}

inline CMIDStringItem& CMIDLabelContainerItem::StringItem()
{
    return iStringItem;
}

#endif // CMIDLABELCONTAINERITEM_H
