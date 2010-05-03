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


#include <aknenv.h>
#include <AknUtils.h>
#include <eiktxlbx.h>
#include <aknlists.h>
#include "swtlist.h"
#include "swtscrollbar.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtList::NewL
// ---------------------------------------------------------------------------
//
CSwtList* CSwtList::NewL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
{
    CSwtList* self = new(ELeave) CSwtList(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtList::CSwtList
// ---------------------------------------------------------------------------
//
CSwtList::CSwtList(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
        : CSwtListBase(aDisplay, aPeer, aParent, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtList::~CSwtList
// ---------------------------------------------------------------------------
//
CSwtList::~CSwtList()
{
}

// ---------------------------------------------------------------------------
// CSwtList::HandleItemAdditionL
// ---------------------------------------------------------------------------
//
void CSwtList::HandleItemAdditionL(TInt aNewFocusIndex)
{
    ASSERT(iList);

    SetTextMaxWidth(ComputeTextMaxWidth());

    // S60 behavior: the new added item is the new focused item
    // and becomes the top item
    TInt newItemIndex(0);
    if ((aNewFocusIndex >= 0) && (aNewFocusIndex < GetItemCount()))
    {
        newItemIndex = aNewFocusIndex;
    }

    iList->SetCurrentItemIndex(newItemIndex);

    if (iList->CurrentItemIndex() >= 0)
    {
        iList->SetTopItemIndex(iList->CurrentItemIndex());
    }

    iList->HandleItemAdditionL();

    // if we are in single selection, update the selection
    // to be the same as the current item index.
    if ((iStyle & KSwtStyleSingle) != 0)
    {
        iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
    }

    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtList::HandleItemRemovalL
// ---------------------------------------------------------------------------
//
void CSwtList::HandleItemRemovalL(
    const RArray<TInt>& aRemovedItemIndices,
    TInt aOldFocusIndex)
{
    ASSERT(iList);
    ASSERT(iList->View());

    SetTextMaxWidth(ComputeTextMaxWidth());

    TInt newFocusIndex(0);

    if (aRemovedItemIndices.Count())
    {
        CalcFocusIndexAfterItemRemoval(
            newFocusIndex,
            aRemovedItemIndices,
            aOldFocusIndex);
    }

    ASSERT((newFocusIndex == 0)
           || ((newFocusIndex > 0)
               && (newFocusIndex < iList->Model()->NumberOfItems())));
    iList->SetCurrentItemIndex(newFocusIndex);

    iList->HandleItemRemovalL();

    if ((iStyle & KSwtStyleSingle) != 0)
    {
        iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
    }

    UpdateListMskL();

    // iList->View()->UpdateSelectionL is not sufficient.
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtList::ComputeSizeL
// ---------------------------------------------------------------------------
//
TSize CSwtList::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (iList == NULL)
    {
        return TSize(0, 0);
    }

    TSize preferredSize(aWHint, aHHint);

    if (aWHint == KSwtDefault)
    {
        preferredSize.iWidth = PreferredWidth();
    }

    if (aHHint == KSwtDefault)
    {
        preferredSize.iHeight = PreferredHeight();
    }

    return preferredSize;
}


// ---------------------------------------------------------------------------
// CSwtList::Scrollable
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtList::Scrollable()
{
    return CSwtListBase::Scrollable();
}

// ---------------------------------------------------------------------------
// CSwtList::DeselectItems
// ---------------------------------------------------------------------------
//
void CSwtList::DeselectItems(const TInt* aIndices, TInt aCount)
{
    CSwtListBase::DeselectItems(aIndices, aCount);
}

// ---------------------------------------------------------------------------
// CSwtList::DeselectItem
// ---------------------------------------------------------------------------
//
void CSwtList::DeselectItem(TInt aIndex)
{
    CSwtListBase::DeselectItem(aIndex);
}

// ---------------------------------------------------------------------------
// CSwtList::DeselectRange
// ---------------------------------------------------------------------------
//
void CSwtList::DeselectRange(TInt aStart, TInt aEnd)
{
    CSwtListBase::DeselectRange(aStart, aEnd);
}

// ---------------------------------------------------------------------------
// CSwtList::DeselectAll
// ---------------------------------------------------------------------------
//
void CSwtList::DeselectAll()
{
    CSwtListBase::DeselectAll();
}

// ---------------------------------------------------------------------------
// CSwtList::GetFocusIndex
// ---------------------------------------------------------------------------
//
TInt CSwtList::GetFocusIndex() const
{
    return CSwtListBase::GetFocusIndex();
}

// ---------------------------------------------------------------------------
// CSwtList::GetSelectionCount
// ---------------------------------------------------------------------------
//
TInt CSwtList::GetSelectionCount() const
{
    return CSwtListBase::GetSelectionCount();
}

// ---------------------------------------------------------------------------
// CSwtList::GetSelectionIndices
// ---------------------------------------------------------------------------
//
const CArrayFix<TInt>* CSwtList::GetSelectionIndices() const
{
    return CSwtListBase::GetSelectionIndices();
}

// ---------------------------------------------------------------------------
// CSwtList::SelectItemL
// ---------------------------------------------------------------------------
//
void CSwtList::SelectItemL(TInt aIndex, TBool aScroll)
{
    CSwtListBase::SelectItemL(aIndex, aScroll);
}

// ---------------------------------------------------------------------------
// CSwtList::SelectRangeL
// ---------------------------------------------------------------------------
//
void CSwtList::SelectRangeL(TInt aStart, TInt aEnd)
{
    CSwtListBase::SelectRangeL(aStart, aEnd);
}

// ---------------------------------------------------------------------------
// CSwtList::SelectAllL
// ---------------------------------------------------------------------------
//
void CSwtList::SelectAllL()
{
    CSwtListBase::SelectAllL();
}

// ---------------------------------------------------------------------------
// CSwtList::ShowSelection
// ---------------------------------------------------------------------------
//
void CSwtList::ShowSelection()
{
    CSwtListBase::ShowSelection();
}

// ---------------------------------------------------------------------------
// CSwtList::AppendL
// ---------------------------------------------------------------------------
//
void CSwtList::AppendL(const TDesC& aPtr)
{
    ASSERT(iList);
    ASSERT(iTextItems);

    // CDesCArray is the base class for descriptor arrays implementing MDesCArray,
    // meaning that the array returned by ItemTextArray() is necessarily a subclass
    // of CDesCArray.
    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    ASSERT(itemTextArray);
    HBufC* itemText = CreateItemTextLC(aPtr);
    TRAPD(error, itemTextArray->AppendL(*itemText));
    CleanupStack::PopAndDestroy();
    if (error)
    {
        User::Leave(ESwtErrorItemNotAdded);
    }

    TRAP(error, iTextItems->AppendL(aPtr));
    if (error)
    {
        itemTextArray->Delete(itemTextArray->Count()-1);
        User::Leave(ESwtErrorItemNotAdded);
    }

    HandleItemAdditionL(GetItemCount()-1);
}

// ---------------------------------------------------------------------------
// CSwtList::InsertL
// ---------------------------------------------------------------------------
//
void CSwtList::InsertL(TInt aPos, const TDesC& aPtr)
{
    ASSERT(iList);
    ASSERT(iTextItems);

    // CDesCArray is the base class for descriptor arrays implementing MDesCArray,
    // meaning that the array returned by ItemTextArray() is necessarily a subclass
    // of CDesCArray.
    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    ASSERT(itemTextArray);
    if (aPos < 0 || aPos > itemTextArray->Count())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    HBufC* itemText = CreateItemTextLC(aPtr);
    TRAPD(error, itemTextArray->InsertL(aPos, *itemText));
    CleanupStack::PopAndDestroy();
    if (error)
    {
        User::Leave(ESwtErrorItemNotAdded);
    }

    TRAP(error, iTextItems->InsertL(aPos, aPtr));
    if (error)
    {
        itemTextArray->Delete(aPos);
        User::Leave(ESwtErrorItemNotAdded);
    }

    // Update selection indices after item insertion.
    const CArrayFix<TInt>* selectionIndices = GetSelectionIndices();
    if (selectionIndices != NULL)
    {
        if (selectionIndices->Count() > 0)
        {
            CArrayFix<TInt>* newSelectionArray = new(ELeave) CArrayFixFlat<TInt>(selectionIndices->Count());
            CleanupStack::PushL(newSelectionArray);
            for (TInt ind = 0; ind < selectionIndices->Count(); ind++)
            {
                if ((*selectionIndices)[ind] >= aPos)
                {
                    newSelectionArray->AppendL((*selectionIndices)[ind]+1);
                }
                else
                {
                    newSelectionArray->AppendL((*selectionIndices)[ind]);
                }
            }
            iList->SetSelectionIndexesL(newSelectionArray);
            CleanupStack::PopAndDestroy();
        }
    }

    HandleItemAdditionL(aPos);
}

// ---------------------------------------------------------------------------
// CSwtList::GetItemL
// ---------------------------------------------------------------------------
//
void CSwtList::GetItemL(TInt aItemIndex, TPtrC& aString) const
{
    ASSERT(iList);
    ASSERT(aString.Length()==0);

    // verify the range
    TInt count = iList->Model()->NumberOfItems();
    if (aItemIndex < 0 || aItemIndex >= count)
    {
        User::Leave(ESwtErrorInvalidRange);
    }
    aString.Set((*iTextItems)[aItemIndex]); // Remove the inserted tab for avkon list items.
}

// ---------------------------------------------------------------------------
// CSwtList::GetItemCount
// ---------------------------------------------------------------------------
//
TInt CSwtList::GetItemCount() const
{
    ASSERT(iList);
    return iList->Model()->NumberOfItems();
}

// ---------------------------------------------------------------------------
// CSwtList::GetItemHeight
// ---------------------------------------------------------------------------
//
TInt CSwtList::GetItemHeight() const
{
    ASSERT(iList);
    return iList->ItemHeight();
}

// ---------------------------------------------------------------------------
// CSwtList::GetSelectionIndex
// ---------------------------------------------------------------------------
//
TInt CSwtList::GetSelectionIndex() const
{
    if (!iList)
    {
        return -1;
    }

    if (!(iStyle & KSwtStyleMulti))
    {
        if (iList->Model()->NumberOfItems() == 0)
        {
            return -1;
        }
        else
        {
            return iList->CurrentItemIndex();
        }
    }
    else
    {
        if (iList->SelectionIndexes()->Count() == 0)
        {
            return -1; // no elements selected
        }
        else
        {
            TInt current = iList->CurrentItemIndex();
            if (iList->View()->ItemIsSelected(current))
            {
                return current;
            }
            return (*(iList->SelectionIndexes()))[0];
        }

    }
}

// ---------------------------------------------------------------------------
// CSwtList::GetTopIndex
// ---------------------------------------------------------------------------
//
TInt CSwtList::GetTopIndex() const
{
    ASSERT(iList);
    return iList->TopItemIndex();
}

// ---------------------------------------------------------------------------
// CSwtList::IndexOf
// ---------------------------------------------------------------------------
//
TInt CSwtList::IndexOf(const TDesC& aString, TInt& aStart) const
{
    if (aStart < 0 || aStart >= GetItemCount())
    {
        return -1;
    }

    if (aStart == 0)
    {
        TInt pos;
        if (iTextItems->Find(aString, pos) == KErrNone)
        {
            return pos;
        }
    }
    else
    {
        for (TInt i = aStart; i < iTextItems->Count(); ++i)
        {
            if (aString.Compare((*iTextItems)[i]) == KErrNone)
            {
                return i;
            }
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------
// CSwtList::IsSelected
// ---------------------------------------------------------------------------
//
TBool CSwtList::IsSelected(TInt aIndex) const
{
    ASSERT(iList);
    return iList->View()->ItemIsSelected(aIndex);
}

// ---------------------------------------------------------------------------
// CSwtList::RemoveItemsL
// ---------------------------------------------------------------------------
//
void CSwtList::RemoveItemsL(const TInt* aIndices, TInt aCount)
{
    ASSERT(iList);
    ASSERT(iTextItems);
    ASSERT(aIndices);
    ASSERT(aCount > 0);

    TInt count = GetItemCount();
    // Elements are ordered from high to low in Java side.
    TInt min = aIndices[aCount - 1];
    TInt max = aIndices[0];
    if (min < 0 || max >= count)
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    TInt currentItemIndex(iList->CurrentItemIndex());
    if (iRemovedItemIndices.Count() > 0)
    {
        iRemovedItemIndices.Reset();
    }

    TInt oldIndex = -1;
    CDesCArray* itemTextArray =
        static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    ASSERT(itemTextArray);
    for (TInt i = 0; i < aCount; ++i)
    {
        if (aIndices[i] != oldIndex)
        {
            itemTextArray->Delete(aIndices[i]);
            iTextItems->Delete(aIndices[i]);
            oldIndex = aIndices[i];
            iRemovedItemIndices.Insert(oldIndex, 0);
        }
    }

    itemTextArray->Compress();
    iTextItems->Compress();

    HandleItemRemovalL(iRemovedItemIndices, currentItemIndex);
    iRemovedItemIndices.Reset();
}

// ---------------------------------------------------------------------------
// CSwtList::RemoveItemL
// ---------------------------------------------------------------------------
//
void CSwtList::RemoveItemL(TInt aIndex)
{
    ASSERT(iList);
    ASSERT(iTextItems);

    if (aIndex < 0 || aIndex >= GetItemCount())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    TInt currentItemIndex(iList->CurrentItemIndex());

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    ASSERT(itemTextArray);
    itemTextArray->Delete(aIndex);
    iTextItems->Delete(aIndex);

    itemTextArray->Compress();
    iTextItems->Compress();

    if (iRemovedItemIndices.Count() > 0)
    {
        iRemovedItemIndices.Reset();
    }
    iRemovedItemIndices.Append(aIndex);
    HandleItemRemovalL(iRemovedItemIndices, currentItemIndex);
    iRemovedItemIndices.Reset();
}

// ---------------------------------------------------------------------------
// CSwtList::RemoveRangeL
// ---------------------------------------------------------------------------
//
void CSwtList::RemoveRangeL(TInt aStart, TInt aEnd)
{
    ASSERT(iList);
    ASSERT(iTextItems);

    if (aStart < 0 || aEnd >= GetItemCount())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    TInt currentItemIndex(iList->CurrentItemIndex());

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    ASSERT(itemTextArray);
    itemTextArray->Delete(aStart, (aEnd - aStart) + 1);  //lint !e834
    iTextItems->Delete(aStart, (aEnd - aStart) + 1);

    itemTextArray->Compress();
    iTextItems->Compress();


    if (iRemovedItemIndices.Count() > 0)
    {
        iRemovedItemIndices.Reset();
    }
    for (TInt ind = aStart; ind <= aEnd; ind++)
    {
        iRemovedItemIndices.Append(ind);
    }
    HandleItemRemovalL(iRemovedItemIndices, currentItemIndex);
    iRemovedItemIndices.Reset();
}

// ---------------------------------------------------------------------------
// CSwtList::RemoveAllL
// ---------------------------------------------------------------------------
//
void CSwtList::RemoveAllL()
{
    ASSERT(iList);
    ASSERT(iTextItems);

    if (GetItemCount() == 0)
    {
        return;
    }

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    ASSERT(itemTextArray);
    itemTextArray->Reset();
    iTextItems->Reset();

    if (iRemovedItemIndices.Count() > 0)
    {
        iRemovedItemIndices.Reset();
    }
    HandleItemRemovalL(iRemovedItemIndices, 0);
}

// ---------------------------------------------------------------------------
// CSwtList::SetFocusIndex
// ---------------------------------------------------------------------------
//
void CSwtList::SetFocusIndex(TInt aIndex)
{
    ASSERT(iList);

    // By now, we only call this method in multiple selection.
    // In single selection, we assume that setting the focus means selecting.
    ASSERT(iStyle & KSwtStyleMulti);

    if (aIndex < 0 || aIndex >= GetItemCount())
    {
        return;
    }

    TInt old = iList->CurrentItemIndex();
    if (old != aIndex)
    {
        iList->SetCurrentItemIndex(aIndex);
        iList->View()->DrawItem(aIndex);
        if (old != -1)
        {
            iList->View()->DrawItem(old);
        }
    }
    TRAP_IGNORE(UpdateListMskL());
}

// ---------------------------------------------------------------------------
// CSwtList::SetItemsL
// ---------------------------------------------------------------------------
//
void CSwtList::SetItemsL(MDesCArray* aStringArray)
{
    ASSERT(iList);
    ASSERT(iTextItems);
    ASSERT(aStringArray);

    // We remove the old selection
    iList->View()->ClearSelection();

    CDesCArray* itemArray = new(ELeave) CDesCArrayFlat(aStringArray->MdcaCount());
    CleanupStack::PushL(itemArray);

    iTextItems->Reset();

    for (TInt i = 0; i < aStringArray->MdcaCount(); ++i)
    {
        HBufC* itemText = CreateItemTextLC(aStringArray->MdcaPoint(i));
        itemArray->AppendL(*itemText);
        CleanupStack::PopAndDestroy(itemText);
        iTextItems->AppendL(aStringArray->MdcaPoint(i));
    }

    iList->Model()->SetItemTextArray(itemArray);

    CleanupStack::Pop(itemArray);

    HandleItemAdditionL(0);

    // iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
    // in HandleItemAdditionL is not sufficient.
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtList::SetTopIndex
// ---------------------------------------------------------------------------
//
void CSwtList::SetTopIndex(TInt aIndex)
{
    ASSERT(iList);

    TInt count = GetItemCount();

    if (count == 0)
    {
        return;
    }

    if (aIndex < 0 || aIndex >= count)
    {
        return;
    }

    iList->SetTopItemIndex(aIndex);
    Redraw();
}
