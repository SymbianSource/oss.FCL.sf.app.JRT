/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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


// For LyaoutMirrored() method.
#include <AknUtils.h>

#include "CMIDFormRow.h"
// CMIDForm API for iForm
#include "CMIDForm.h"
// API for items on rows
#include "CMIDControlItem.h"
#include "CMIDItemLabel.h"


CMIDFormRow* CMIDFormRow::NewL(CMIDForm& aForm)
{
    CMIDFormRow* self = new(ELeave) CMIDFormRow(aForm);
    CleanupStack::PushL(self);
    self->ActivateL();
    CleanupStack::Pop(self);
    return self;
}

CMIDFormRow::~CMIDFormRow()
{
    TInt i;
    for (i=0; i < iItems.Count(); i++)
    {
        if (CMIDForm::IsLabelContainerItem(*(iItems[i])))
        {
            delete iItems[i];
        }
        else if (CMIDForm::IsSpacerUsedForFormatting(*(iItems[i])))
        {
            delete iItems[i];
        }
    }
    iItems.Reset();
    iItems.Close();
}

TInt CMIDFormRow::CountComponentControls() const
{
    return iItems.Count();
}

CCoeControl* CMIDFormRow::ComponentControl(TInt aIndex) const
{
    return iItems[aIndex];
}

void CMIDFormRow::SizeChanged()
{
    TInt xOff = 0;

    TSize size(0,0);

    TInt i;
    for (i=0; i < iItems.Count(); i++)
    {
        CMIDControlItem& ci = *(iItems[i]);
        size.iWidth += ci.Size().iWidth;
        if (ci.Size().iHeight > size.iHeight)
        {
            size.iHeight = ci.Size().iHeight;
        }
    }

    if (iAlignment == MMIDItem::ERight)
    {
        xOff = iForm.Width() - size.iWidth;
        // If it is mirrored layout, we need translate a row.
        if (AknLayoutUtils::LayoutMirrored())
        {
            xOff -= iForm.GetMidpNaviPos();
        }
    }

    if (iAlignment == MMIDItem::ECenter)
    {
        xOff = (iForm.Width() - size.iWidth) / 2;
    }

    for (i=0; i < iItems.Count(); i++)
    {
        CMIDControlItem& ci = *(iItems[i]);
        TInt yOff = 0;
        if (CMIDForm::LayoutBottom(ci))
        {
            yOff = size.iHeight - ci.Size().iHeight;
        }
        if (CMIDForm::LayoutVerticalCenter(ci))
        {
            yOff = (size.iHeight - ci.Size().iHeight) / 2;
        }

        // Form left margin is added in the beginning of each row
        TPoint newPosition = Position() + TPoint(xOff, yOff);
        TSize itemSize = ci.Size();
        TRect itemRect = TRect(newPosition, itemSize);
        ci.SetRect(itemRect);
        xOff += ci.Size().iWidth;
    }
}

TSize CMIDFormRow::MinimumSize()
{
    if (iItems.Count() == 0)
    {
        return iEmptyRowSize;
    }

    TInt width = 0;
    TInt height = 0;

    for (TInt i=0; i < iItems.Count(); i++)
    {
        CMIDControlItem& ci = *(iItems[i]);

        width += CMIDForm::LayoutShrink(ci) ? ci.MinimumSize().iWidth : ci.PreferredSize().iWidth;

        if (CMIDForm::LayoutVerticalShrink(ci))
        {
            if (height < ci.MinimumSize().iHeight)
            {
                height = ci.MinimumSize().iHeight;
            }
        }
        else
        {
            if (height < ci.PreferredSize().iHeight)
            {
                height = ci.PreferredSize().iHeight;
            }
        }
    }
    return TSize(width, height);
}

void CMIDFormRow::AppendL(CMIDControlItem* aItem)
{
    if (iForm.InitialAlignment() == MMIDItem::ELeft)
    {
        User::LeaveIfError(iItems.Append(aItem));
    }
    else
    {
        User::LeaveIfError(iItems.Insert(aItem, 0));
    }
    iCurrentWidth += aItem->Size().iWidth;
}

void CMIDFormRow::SetAlignment(MMIDItem::TLayout aAlignment)
{
    ASSERT((aAlignment == MMIDItem::ELeft) || (aAlignment == MMIDItem::ERight) || (aAlignment == MMIDItem::ECenter));
    iAlignment = aAlignment;
}

/** */
void CMIDFormRow::SizeItemsL()
{
    TSize minSize = MinimumSize();

    // size the widths first
    TInt availableWidth = iForm.Width() - minSize.iWidth;
    // equation is also allowed - resizing when extra space is same as allowed
    if (availableWidth >= 0)
    {
        TInt totalExtraWidth = 0;
        for (TInt j=0; j < iItems.Count(); j++)
        {
            CMIDControlItem& ci = *(iItems[j]);
            if (CMIDForm::LayoutShrink(ci))
            {
                totalExtraWidth += ci.PreferredSize().iWidth - ci.MinimumSize().iWidth;
            }
        }

        if (availableWidth > totalExtraWidth)
        {
            // just give all items what they want
            for (TInt j=0; j < iItems.Count(); j++)
            {
                CMIDControlItem& ci = *(iItems[j]);
                ci.SetSizeQuiet(ci.PreferredSize());
            }

            availableWidth -= totalExtraWidth;
        }
        else
        {
            // distribute the size
            TReal scaleFactor;
            if (availableWidth == totalExtraWidth)
            {
                scaleFactor = 1;
            }
            else
            {
                if (totalExtraWidth != 0)
                {
                    scaleFactor = TReal(availableWidth) / TReal(totalExtraWidth);
                }
                else
                {
                    scaleFactor = 1;
                }
            }

            for (TInt j=0; j < iItems.Count(); j++)
            {
                CMIDControlItem& ci = *(iItems[j]);

                if (CMIDForm::LayoutShrink(ci))
                {
                    TInt minAndPreferredDif = ci.PreferredSize().iWidth - ci.MinimumSize().iWidth;
                    TInt newMinAndPreferredDif = TInt(scaleFactor * TReal(minAndPreferredDif));
                    TInt newWidth = ci.MinimumSize().iWidth + Min(newMinAndPreferredDif, minAndPreferredDif);
                    availableWidth -= Min(newMinAndPreferredDif, minAndPreferredDif);
                    ci.SetSizeQuiet(TSize(newWidth, ci.Size().iHeight));
                }
                else
                {
                    ci.SetSizeQuiet(TSize(ci.PreferredSize().iWidth, ci.Size().iHeight));
                }
            }
        }
        // note: should probably do another pass through to make sure the above step
        // went ok and didn't suffer from rounding errors (ie that the Shrink items didn't
        // end up smaller than their preferred size (if we have size left over)

        // loop through and expand the items with Expand layout
        if (availableWidth > 0)
        {
            TInt numExpand = 0;
            // count the items with Expand layout
            for (TInt j=0; j < iItems.Count(); j++)
            {
                CMIDControlItem& ci = *(iItems[j]);

                if (CMIDForm::LayoutExpand(ci))
                {
                    numExpand++;
                }
            }
            if (numExpand > 0)
            {
                TInt addedSize = availableWidth / numExpand;
                TInt expandIdx = -1;

                for (TInt j=0; j < iItems.Count(); j++)
                {
                    CMIDControlItem& ci = *(iItems[j]);

                    if (CMIDForm::LayoutExpand(ci))
                    {
                        ci.SetSizeQuiet(ci.Size() + TSize(addedSize, 0));
                        availableWidth -= addedSize;
                        expandIdx = j;
                        // at the end of the loop this will hold the last expanded
                        // item. At that time we can check if we still have extra
                        // space remaining and give it to the item at this index
                    }
                }
                if (availableWidth > 0)
                {
                    // if we still have extra width due to rounding error just give it to the last item
                    iItems[expandIdx]->SetSizeQuiet(iItems[expandIdx]->Size() + TSize(availableWidth, 0));
                }
            }
        }
    }


    // the widths have been calculated. This could have changed the height
    // due to for example label wrapping so ask the control to check
    // the new width.
    TInt j;
    for (j=0; j < iItems.Count(); j++)
    {
        CMIDControlItem& ci = *(iItems[j]);
        ci.AdjustToNewWidthL(ci.Size().iWidth);
    }

    // now size the heights

    // first find the row height
    TInt rowHeight = 0;
    for (j=0; j < iItems.Count(); j++)
    {
        CMIDControlItem& ci = *(iItems[j]);

        TInt height = ci.PreferredSize().iHeight;
        if (CMIDForm::LayoutVerticalShrink(ci))
        {
            height = ci.MinimumSize().iHeight;
        }
        if (height > rowHeight)
        {
            rowHeight = height;
        }
    }

    if (iItems.Count() == 0)
    {//if not items use empty row height
        rowHeight = iEmptyRowSize.iHeight;
    }

    // now set the row heights
    for (j=0; j < iItems.Count(); j++)
    {
        CMIDControlItem& ci = *(iItems[j]);

        // first set the size normally, then change it based on shrink and expand directives
        ci.SetSizeQuiet(TSize(ci.Size().iWidth, ci.PreferredSize().iHeight));
        if (CMIDForm::LayoutVerticalShrink(ci))
        {
            TInt height = Min(ci.PreferredSize().iHeight, rowHeight);
            ci.SetSizeQuiet(TSize(ci.Size().iWidth, height));
        }
        if (CMIDForm::LayoutVerticalExpand(ci))
        {
            ci.SetSizeQuiet(TSize(ci.Size().iWidth, rowHeight));
        }

        ci.AdjustToSizeL(ci.Size());
    }

    // set the size of the row
    SetSize(TSize(iForm.Width(), rowHeight));
}

TInt CMIDFormRow::NumItems()
{
    return iItems.Count();
}

TInt CMIDFormRow::Find(const CMIDControlItem* aItem) const
{
    for (TInt i=0; i < iItems.Count(); i++)
    {
        if (aItem == iItems[i])
        {
            return i;
        }
    }

    return KErrNotFound;
}

CMIDControlItem* CMIDFormRow::Item(TInt aIndex)
{
    ASSERT(aIndex >= 0 && aIndex < iItems.Count());
    return iItems[aIndex];
}

TBool CMIDFormRow::HasFocusableItemOnOrAboveScreen()
{
    for (TInt i=0; i < iItems.Count(); i++)
    {
        if (iItems[i]->IsSelectable())
        {
            TRect rect = iItems[i]->Rect();
            if (rect.iTl.iY < iForm.Height())
            {
                return ETrue;
            }
        }
    }
    return EFalse;
}

TBool CMIDFormRow::HasFocusableItemOnOrBelowScreen()
{
    for (TInt i=0; i < iItems.Count(); i++)
    {
        if (iItems[i]->IsSelectable())
        {
            TRect rect = iItems[i]->Rect();
            if (rect.iBr.iY >= 0)
            {
                return ETrue;
            }
        }
    }
    return EFalse;
}

CMIDControlItem* CMIDFormRow::FirstFocusableItemOnScreen()
{
    TInt idx = -1;
    while (++idx < iItems.Count())
    {
        if (iItems[idx]->IsSelectable())
        {
            TRect rect = iItems[idx]->Rect();
            if (iForm.RectPartiallyVisible(rect))
            {
                return iItems[idx];
            }
        }
    }
    return NULL;
}

CMIDControlItem* CMIDFormRow::FirstFocusableItem()
{
    TInt idx = -1;
    while (++idx < iItems.Count())
    {
        if (iItems[idx]->IsSelectable())
        {
            return iItems[idx];
        }
    }
    return NULL;
}

CMIDControlItem* CMIDFormRow::LastFocusableItem()
{
    TInt idx = iItems.Count();
    while (--idx >= 0)
    {
        if (iItems[idx]->IsSelectable())
        {
            return iItems[idx];
        }
    }
    return NULL;
}

// typically just used when cleaning up. This won't cause a layout.
TBool CMIDFormRow::RemoveItemIfExists(CMIDControlItem& aItem)
{
    TInt i = -1;
    while (++i < iItems.Count())
    {
        if (&aItem == iItems[i])
        {
            iItems.Remove(i);
            return ETrue;
        }
    }

    return EFalse;
}

TInt CMIDFormRow::CurrentWidth()
{
    return iCurrentWidth;
}

CMIDFormRow::CMIDFormRow(CMIDForm& aForm)
        :iForm(aForm)
{
#ifdef RD_SCALABLE_UI_V2
    SetAllowStrayPointers();
#endif
}

void CMIDFormRow::ConstructL()
{
}

/**
 *  Returns true if this row has at least an item which is not a spacer
 */
TBool CMIDFormRow::HasNonSpacerItems() const
{
    TBool ret = EFalse;

    TInt i = -1;
    while (++i < iItems.Count())
    {
        if (iItems[i]->Type() != MMIDComponent::ESpacer)
        {
            ret = ETrue;
            break;
        }
    }

    return ret;
}

// End of File
