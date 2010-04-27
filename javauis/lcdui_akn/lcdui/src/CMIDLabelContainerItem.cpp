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


// CEikLabel API for iLabel
#include <eiklabel.h>
// AknsDrawUtils::DrawFrame in Draw function
#include <AknsDrawUtils.h>
// SkinLayout in Draw function
#include <skinlayout.cdl.h>
// using TAknLayoutRect in Draw function
#include <AknUtils.h>

// using API for iStringItem
#include "CMIDStringItem.h"
#include "CMIDLabelContainerItem.h"
// using ItemLabelMargin() function for retrieving label margin to calculate iSize
// and for calculation position of iLabel
#include "CMIDItemLabel.h"

CMIDLabelContainerItem* CMIDLabelContainerItem::NewL
(CMIDStringItem& aStringItem, CEikLabel& aLabel, TBool aHasHorizMargin)
{
    CMIDLabelContainerItem* self = new(ELeave)
    CMIDLabelContainerItem(aStringItem, aLabel, aHasHorizMargin);

    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

TSize CMIDLabelContainerItem::MinimumSize()
{
    return iLabel.Size();
}

TInt CMIDLabelContainerItem::CountComponentControls() const
{
    return 1;
}

CCoeControl* CMIDLabelContainerItem::ComponentControl(TInt aIndex) const
{
    if (aIndex == 0)
    {
        return &iLabel;
    }

    return NULL;
}

void CMIDLabelContainerItem::Draw(const TRect& aRect) const
{
    if ((iStringItem.RenderedAppearance() == MMIDItem::EHyperLink) && iStringItem.IsFocused())
    {
        TAknLayoutRect topLeft;
        topLeft.LayoutRect(aRect, SkinLayout::Highlight_skin_placing__grid__Line_2());

        TAknLayoutRect bottomRight;
        bottomRight.LayoutRect(aRect, SkinLayout::Highlight_skin_placing__grid__Line_5());

        TRect outerRect = TRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
        TRect innerRect = TRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);

        if (!iIsStringItemContent)
        { // In case of StringItem divided due to concatenation:
            // If LabelContainerItem is part of StringItem and its label, adding
            // horizontal margin to highlighting frame for better look.
            TInt x = iHasHorizMargin ? iStringItem.StringContentControl()->ItemLabelMargin() : 0;
            outerRect.iBr.iX += x;
        }

        AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(), SystemGc(),
                                 outerRect, innerRect, KAknsIIDQsnFrList,
                                 KAknsIIDQsnFrListCenter);
    }
}

void CMIDLabelContainerItem::SizeChanged()
{
    TInt x = iHasHorizMargin ? iStringItem.StringContentControl()->ItemLabelMargin() : 0;
    iLabel.SetExtent(Position() + TPoint(x, 0), iLabel.Size());

    CMIDControlItem::SizeChanged();
}

void CMIDLabelContainerItem::FocusChanged(TDrawNow aDrawNow)
{
    iStringItem.FocusChanged(aDrawNow);
}

TBool CMIDLabelContainerItem::IsSelectable() const
{
    return iStringItem.IsSelectable();
}

TInt CMIDLabelContainerItem::ItemPreferredHeightWithoutLabel()
{
    return iLabel.Size().iHeight;
}

CMIDLabelContainerItem::CMIDLabelContainerItem(
    CMIDStringItem& aStringItem, CEikLabel& aLabel,
    TBool aHasHorizMargin)
        : CMIDControlItem(MMIDItem::EDefault, aStringItem.GetUIManager()),
        iStringItem(aStringItem),
        iLabel(aLabel),
        iHasHorizMargin(aHasHorizMargin),
        iIsStringItemContent(EFalse)
{
    SetFocusing(EFalse);
}

CMIDLabelContainerItem::~CMIDLabelContainerItem()
{
}

void CMIDLabelContainerItem::ConstructL()
{
    iPreferredSize = iLabel.Size();

    if (iHasHorizMargin)
    {
        iPreferredSize.iWidth += iStringItem.StringContentControl()->ItemLabelMargin();
    }

    iSize = iPreferredSize;
    SetLayoutL(iStringItem.Layout());
}

void CMIDLabelContainerItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    iLabel.SetContainerWindowL(aContainer);
}

#ifdef RD_SCALABLE_UI_V2
void CMIDLabelContainerItem::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    iStringItem.HandlePointerEventL(aPointerEvent);
}
#endif

TBool CMIDLabelContainerItem::IsStringItemContent()
{
    return iIsStringItemContent;
}

void CMIDLabelContainerItem::SetIsStringItemContent(TBool aIsStringItemContent)
{
    iIsStringItemContent = aIsStringItemContent;
}

// End of File
