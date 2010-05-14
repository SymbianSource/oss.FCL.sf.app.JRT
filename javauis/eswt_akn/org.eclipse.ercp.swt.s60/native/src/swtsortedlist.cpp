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
#include <aknlists.h>
#include <eikclbd.h>
#include <eikclb.h>
#include "swtsortedlist.h"


const TInt KSwtArrayGranularity = 4;
const TInt KSwtSearchFieldTextLimit = 20;
const TInt KSwtExtraWidthToDisplayCursorInArabicLayout = 1;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtSortedList::NewL
// ---------------------------------------------------------------------------
//
CSwtSortedList* CSwtSortedList::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                     MSwtComposite& aParent, TInt aStyle)
{
    CSwtSortedList* self = new(ELeave) CSwtSortedList(
        aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::~CSwtSortedList
// ---------------------------------------------------------------------------
//
CSwtSortedList::~CSwtSortedList()
{
    delete iSearchField;
}


// ---------------------------------------------------------------------------
// CSwtSortedList::CSwtSortedList
// ---------------------------------------------------------------------------
//
CSwtSortedList::CSwtSortedList(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               MSwtComposite& aParent, TInt aStyle)
        : CSwtListBase(aDisplay, aPeer, aParent, aStyle)
{
}


// ---------------------------------------------------------------------------
// CSwtSortedList::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtSortedList::ConstructL()
{
    if ((iStyle & KSwtStyleDown) != 0)
    {
        iIsOrderDown = ETrue;
    }

    CSwtListBase::ConstructL();
}


// ---------------------------------------------------------------------------
// CSwtSortedList::Filter
// ---------------------------------------------------------------------------
//
CAknListBoxFilterItems* CSwtSortedList::Filter() const
{
    // The model created by CSwtListBoxSingle or
    // CSwtListBoxSingleGraphic is CAknFilteredTextListBoxModel.
    CAknFilteredTextListBoxModel* model = STATIC_CAST(
                                              CAknFilteredTextListBoxModel*,iList->Model());
    ASSERT(model);

    return model->Filter();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::InsertItemL
// ---------------------------------------------------------------------------
//
void CSwtSortedList::InsertItemL(
    const TDesC &aPtr,
    CDesCArray* aItemArray,
    TKeyCmpText aTextComparisonType)
{
    ASSERT(aItemArray != NULL);
    HBufC* itemText = CreateItemTextLC(aPtr);
    if (!iIsOrderDown)
    {
        aItemArray->InsertIsqAllowDuplicatesL(*itemText, aTextComparisonType);
        iTextItems->InsertIsqAllowDuplicatesL(aPtr, aTextComparisonType);
    }
    else
    {
        TInt ind(0);
        for (ind = 0; ind < iTextItems->Count(); ind++)
        {
            if ((*iTextItems)[ind].CompareC(aPtr) < 0)
            {
                aItemArray->InsertL(ind, *itemText);
                iTextItems->InsertL(ind, aPtr);
                break;
            }
        }
        // If no item in list or if the item must be appended at the end of the list
        if (ind == iTextItems->Count())
        {
            aItemArray->AppendL(*itemText);
            iTextItems->AppendL(aPtr);
        }
    }
    CleanupStack::PopAndDestroy(itemText);

    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::GetSelectionLC
// ---------------------------------------------------------------------------
//
CPtrCArray* CSwtSortedList::GetSelectionLC() const
{
    const CArrayFix<TInt>* selectionIndexes = iList->View()->SelectionIndexes();
    if (!selectionIndexes)
    {
        return NULL;
    }

    TInt nbSelection(selectionIndexes->Count());

    CPtrCArray* selectedItemArray = new(ELeave) CPtrC16Array(KSwtArrayGranularity);
    CleanupStack::PushL(selectedItemArray);
    TInt selIndex;
    CAknListBoxFilterItems* filter = NULL;
    if (iSearchField)
    {
        filter = Filter();
    }
    for (TInt index=0; index<nbSelection; ++index)
    {
        selIndex = selectionIndexes->At(index);
        if (selIndex >= 0 && selIndex < iTextItems->Count())
        {
            if (iSearchField && filter)
            {
                if (filter->FilteredNumberOfItems() > 0)// if there are visible items after filtering
                {
                    selIndex = filter->FilteredItemIndex(selIndex);
                    selectedItemArray->AppendL((*iTextItems)[selIndex]);
                }
            }
            else
            {
                selectedItemArray->AppendL((*iTextItems)[selIndex]);
            }
        }
    }
    return selectedItemArray;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::HandleItemAdditionL
// ---------------------------------------------------------------------------
//
void CSwtSortedList::HandleItemAdditionL(TInt aNewFocusIndex)
{
    ASSERT(iList->View());
    ASSERT(iList->Model());

    SetTextMaxWidth(ComputeTextMaxWidth());

    if (iSearchField)
    {
        CAknListBoxFilterItems* filter = Filter();
        ASSERT(filter);
        filter->HandleItemArrayChangeL();
    }
    else
    {
        // S60 behavior: the new added item is the new focused item
        // and becomes the top item
        TInt newItemIndex(0);
        if ((aNewFocusIndex >= 0) && (aNewFocusIndex < iList->Model()->NumberOfItems()))
        {
            newItemIndex = aNewFocusIndex;
        }

        iList->SetCurrentItemIndex(newItemIndex);

        if (iList->CurrentItemIndex() >= 0)
        {
            iList->SetTopItemIndex(iList->CurrentItemIndex());
        }

        iList->HandleItemAdditionL();
    }

    // if we are in single selection, update the selection
    // to be the same as the current item index.
    if ((iStyle & KSwtStyleSingle) != 0)
    {
        iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
    }

    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::HandleItemRemovalL
// ---------------------------------------------------------------------------
//
void CSwtSortedList::HandleItemRemovalL(
    const RArray<TInt>& aRemovedItemIndices,
    TInt aOldFocusIndex)
{
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

    // Filter update must happen after list update
    if (iSearchField)
    {
        CAknListBoxFilterItems* filter = Filter();
        ASSERT(filter);
        filter->HandleItemArrayChangeL();
    }

    // if we are in single selection, update the selection
    // to be the same as the current item index.
    if ((iStyle & KSwtStyleSingle) != 0)
    {
        iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
    }

    UpdateListMskL();

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::HandleOfferKeyEventL
// ---------------------------------------------------------------------------
//
void CSwtSortedList::HandleOfferKeyEventL()
{
    if (iSearchField)
    {
        if (iStyle & KSwtStyleSingle)
        {
            const_cast<CListBoxView::CSelectionIndexArray*>(
                iList->View()->SelectionIndexes())->Reset();
        }

        CAknListBoxFilterItems* filter = Filter();
        ASSERT(filter);
        filter->HandleOfferkeyEventL();

        if ((iStyle & KSwtStyleSingle) && iTextItems->Count() > 0)
        {
            iList->View()->SelectItemL(0);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SearchFieldHeight
// ---------------------------------------------------------------------------
//
TInt CSwtSortedList::SearchFieldHeight() const
{
    TInt searchHeight(0);
    if (iSearchField)
    {
        searchHeight = CSwtLafFacade::GetLayoutRect(
                           CSwtLafFacade::EFindPane, BorderInnerRect()).Rect().Height();
    }
    return searchHeight;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::GetSearchFieldPosition
// ---------------------------------------------------------------------------
//
void CSwtSortedList::GetSearchFieldPosition(TPoint& aPosition)
{
    TRect controlRect(BorderInnerRect());
    aPosition.iX = controlRect.iTl.iX;
    aPosition.iY = Max(0, controlRect.iBr.iY - SearchFieldHeight());
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SetSearchFieldSize
// ---------------------------------------------------------------------------
//
void CSwtSortedList::GetSearchFieldSize(TSize& aSize)
{
    TRect controlRect(BorderInnerRect());
    aSize.iWidth =  controlRect.Width();
    aSize.iHeight = Min(controlRect.Height(), SearchFieldHeight());
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SetSearchFieldSize
// ---------------------------------------------------------------------------
//
void CSwtSortedList::UpdateSearchTextViewRect(const CAknSearchField* aSearchField)
{
    if (aSearchField != NULL)
    {
        // Fix to display text cursor in Arabic layout
        TAknLayoutId currentLayoutId;
        CAknEnv::Static()->GetCurrentLayoutId(currentLayoutId);
        if (currentLayoutId == EAknLayoutIdABRW)
        {
            CTextView* textView = aSearchField->Editor().TextView();
            ASSERT(textView);
            TRect viewRect(textView->ViewRect());
            viewRect.iBr.iX += KSwtExtraWidthToDisplayCursorInArabicLayout;
            textView->SetViewRect(viewRect);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::UpdateSearchFieldBounds
// ---------------------------------------------------------------------------
//
void CSwtSortedList::UpdateSearchFieldBounds()
{
    TPoint pos;
    TSize size;
    GetSearchFieldPosition(pos);
    GetSearchFieldSize(size);
    iSearchField->SetExtent(pos, size);
    UpdateSearchTextViewRect(iSearchField);
}

// ---------------------------------------------------------------------------
// CSwtSortedList::UpdateSize
// From CSwtListBase
// ---------------------------------------------------------------------------
//
void CSwtSortedList::UpdateSize()
{
    TRect controlRect(BorderInnerRect());

    //Update the search field size if exists
    TInt listHeight(controlRect.Height());
    if (iSearchField != NULL)
    {
        listHeight = Max(0, listHeight - SearchFieldHeight());
        UpdateSearchFieldBounds();
    }

    iList->SetSize(TSize(controlRect.Width(), listHeight));
}

// ---------------------------------------------------------------------------
// CSwtSortedList::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtSortedList::CountComponentControls() const
{
    // The filter is orphaned to force its editor to draw CAknSearchField's
    // background instead of our Shell's background.
    return 1;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtSortedList::ComponentControl(TInt /*aIndex*/) const
{
    // The filter is orphaned to force its editor to draw CAknSearchField's
    // background instead of our Shell's background.
    return iList;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::MakeVisible(TBool aVisible)
{
    CCoeControl::MakeVisible(aVisible);
    FocusabilityChanged();

    // WORKAROUND START
    // There can be situations where the cursor is blinking even when the list
    // is not focused. This is yet another slow down but there is no other way
    // to avoid the strange behaviour. The cursor visibility setters are private.
    // Unfocusing the search field will also hide the cursor :)
    if (iSearchField && !IsFocused())
    {
        // Enforce no focus on the editor
        iSearchField->Editor().SetFocus(EFalse);
    }
    // WORKAROUND END
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::SetDimmed(TBool aDimmed)
{
    CCoeControl::SetDimmed(aDimmed);
    iList->SetDimmed(aDimmed);
    if (iSearchField)
    {
        iSearchField->SetDimmed(aDimmed);
    }
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::HandleResourceChange(TInt aType)
{
    if (aType == KAknsMessageSkinChange)
    {
        TRAP_IGNORE(CreateIconArrayL(iList));
    }

    CAknControl::HandleResourceChange(aType);
}

// ---------------------------------------------------------------------------
// CSwtSortedList::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtSortedList::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    // Check if we should start filter grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down
            && !iFilterGrabsPointerEvents)
    {
        if (iSearchField && iSearchField->Rect().Contains(aPointerEvent.iPosition))
        {
            iFilterGrabsPointerEvents = ETrue;
        }
    }

    // Deliver event to filter
    if (iFilterGrabsPointerEvents)
    {
        if (iSearchField)
        {
            iSearchField->HandlePointerEventL(aPointerEvent);
        }
    }

    // Check if we should stop filter grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Up
            && iFilterGrabsPointerEvents)
    {
        iFilterGrabsPointerEvents = EFalse;
    }

    // Deliver event to list
    if (!iFilterGrabsPointerEvents)
    {
        CSwtListBase::HandlePointerEventL(aPointerEvent);
    }
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtSortedList::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::PositionChanged()
{
    //Update the search field size if exists
    if (iSearchField != NULL)
    {
        UpdateSearchFieldBounds();
    }
    CSwtListBase::PositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::FocusChanged(TDrawNow aDrawNow)
{
    TBool isFocused = IsFocused();
    if (iList)
    {
        iList->SetFocus(isFocused, aDrawNow);
        if (iSearchField)
        {
            iSearchField->Editor().SetFocus(isFocused);
        }
    }
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtSortedList::ClientRect
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRect CSwtSortedList::ClientRect() const
{
    return SbInnerRect(iList->Rect());
}

// ---------------------------------------------------------------------------
// CSwtSortedList::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtSortedList::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    TSize preferredSize(aWHint, aHHint);

    if (aWHint == KSwtDefault)
    {
        preferredSize.iWidth = PreferredWidth();
    }

    if (aHHint == KSwtDefault)
    {
        preferredSize.iHeight = PreferredHeight();
        preferredSize.iHeight += SearchFieldHeight();
    }

    return preferredSize;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::ProcessKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    if (iSearchField != NULL)
    {
        TKeyResponse keyResponse = iSearchField->OfferKeyEventL(aKeyEvent, aType);
        if (keyResponse == EKeyWasConsumed)
        {
            HandleOfferKeyEventL();
            return;
        }
    }

    CSwtListBase::ProcessKeyEventL(aKeyEvent, aType);
}

// ---------------------------------------------------------------------------
// CSwtSortedList::DoPaint
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtSortedList::DoPaint(const TRect& aRect) const
{
    CSwtListBase::DoPaint(aRect);

    // The filter is orphaned to force its editor to draw CAknSearchField's
    // background instead of our Shell's background. Therefore drawing must
    // be done 'manually'.
    if (iSearchField)
    {
        CWindowGc& gc = SystemGc();
        iSearchField->SetGc(&gc); // required by effects
        iSearchField->DrawBackground(aRect);
        iSearchField->DrawForeground(aRect);
        DrawComponentControls(*iSearchField, aRect, gc, EFalse);
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::ComputeTrim
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
void CSwtSortedList::ComputeTrim(TRect& aRect) const
{
    // Add list rect with scroll bars
    TRect outerRect(SbOuterRect(aRect));
    aRect.SetRect(outerRect.iTl, outerRect.Size());

    // Add the search field rect
    if (iSearchField != NULL)
    {
        TSize searchFieldMinSize(TSize(0, SearchFieldHeight()));
        TInt addWidth = searchFieldMinSize.iWidth - aRect.Width();
        aRect.Resize(addWidth>0 ? addWidth:0, searchFieldMinSize.iHeight);
    }

    // Add the borders
    TRect borderOuterRect(BorderOuterRect(aRect));
    aRect.SetRect(borderOuterRect.iTl, borderOuterRect.Size());
}

// ---------------------------------------------------------------------------
// CSwtSortedList::AppendL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
void CSwtSortedList::AppendL(const TDesC& aPtr)
{
    ASSERT(iTextItems);

    CPtrCArray* selectedItemArray = GetSelectionLC();

    InsertItemL(aPtr, STATIC_CAST(CDesCArray*, iList->Model()->ItemTextArray()));

    SetSelectionL(selectedItemArray);
    CleanupStack::PopAndDestroy(selectedItemArray);

    HandleItemAdditionL(iTextItems->Count()-1);
}

// ---------------------------------------------------------------------------
// CSwtSortedList::GetFocusedItem
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
void CSwtSortedList::GetFocusedItem(TPtrC& aItem) const
{
    if (iList->Model()->NumberOfItems() == 0)
    {
        return;
    }

    // Get focused item index
    TInt focusedItem = GetFocusIndex();

    // If filter is active, it is neccessary to recalculate index
    if (iSearchField)
    {
        CAknListBoxFilterItems* filter = Filter();
        ASSERT(filter);
        // At least one filtered item is visible
        if (filter->FilteredNumberOfItems() > 0)
        {
            // Update index and get item string
            focusedItem = filter->FilteredItemIndex(focusedItem);
            aItem.Set((*iTextItems)[focusedItem]);
        }
        else
        {
            // When nothing is selected, function returns null-string
            aItem.Set(KNullDesC);
        }
    }
    else
    {
        // Get focused item string
        aItem.Set((*iTextItems)[focusedItem]);
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::GetItemHeight
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
TInt CSwtSortedList::GetItemHeight() const
{
    return iList->ItemHeight();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::GetSelectionCount
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
TInt CSwtSortedList::GetSelectionCount() const
{
    return CSwtListBase::GetSelectionCount();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::GetSelectionL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
CPtrCArray* CSwtSortedList::GetSelectionL() const
{
    CPtrCArray* selectedItemArray = GetSelectionLC();
    CleanupStack::Pop(selectedItemArray);
    return selectedItemArray;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::RemoveAllL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
void CSwtSortedList::RemoveAllL()
{
    if (iTextItems->Count() == 0)
    {
        return;
    }

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    itemTextArray->Reset();
    iTextItems->Reset();

    if (iRemovedItemIndices.Count() > 0)
    {
        iRemovedItemIndices.Reset();
    }
    HandleItemRemovalL(iRemovedItemIndices, 0);
    iRemovedItemIndices.Reset();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::RemoveL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
TBool CSwtSortedList::RemoveL(const HBufC& aPtr)
{
    TInt itemPos;
    TInt res = iTextItems->Find(aPtr, itemPos, ECmpCollated);
    if (res)
    {
        // String not found in list
        return ETrue;
    }

    TInt currentItemIndex(iList->CurrentItemIndex());

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iList->Model()->ItemTextArray());
    itemTextArray->Delete(itemPos);
    iTextItems->Delete(itemPos);

    if (iRemovedItemIndices.Count() > 0)
    {
        iRemovedItemIndices.Reset();
    }
    iRemovedItemIndices.Append(itemPos);
    HandleItemRemovalL(iRemovedItemIndices, currentItemIndex);
    iRemovedItemIndices.Reset();
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::Scrollable
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtSortedList::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SelectItemL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
TBool CSwtSortedList::SelectItemL(const TDesC* aString)
{
    if (aString == NULL)
    {
        if (iList->View()->SelectionIndexes()->Count() > 0)
        {
            iList->ClearSelection();
            UpdateListMskL();
            return EFalse;
        }
        else
        {
            return ETrue;
        }
    }

    TInt itemPos;
    TInt res = iTextItems->Find(*aString, itemPos, ECmpCollated);
    if (res)
    {
        // String not found in list
        return ETrue;
    }

    if (iSearchField)
    {
        CAknListBoxFilterItems* filter = Filter();
        ASSERT(filter);
        itemPos = filter->VisibleItemIndex(itemPos);
        if (itemPos < 0) // Item not visible do nothing
        {
            return EFalse;
        }
    }

    if (iList->View()->ItemIsSelected(itemPos))
    {
        // Already selected
        return EFalse;
    }

    if (iStyle & KSwtStyleMulti)
    {
        iList->View()->SelectItemL(itemPos);
        UpdateListMskL();
    }
    else
    {
        TInt old = iList->CurrentItemIndex();

        // We do not call CEikListBox::SetCurrentItemIndex, because we do not want to scroll.
        iList->View()->SetCurrentItemIndex(itemPos);
        iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
        if (old != -1)
        {
            iList->View()->DrawItem(old);
        }
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SetItemsL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
void CSwtSortedList::SetItemsL(MDesCArray* aStringArray)
{
    if (iList)
    {
        ASSERT(aStringArray);

        // We remove the old selection
        iList->ClearSelection();

        // Reset the text items
        iTextItems->Reset();

        CDesCArray* itemArray = new(ELeave) CDesCArrayFlat(aStringArray->MdcaCount());
        CleanupStack::PushL(itemArray);

        for (TInt i = 0; i < aStringArray->MdcaCount(); ++i)
        {
            // The item is inserted in both iTextItems and itemArray
            InsertItemL(aStringArray->MdcaPoint(i), itemArray);
        }

        iList->Model()->SetItemTextArray(itemArray);
        CleanupStack::Pop(itemArray);

        HandleItemAdditionL(0);

        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SetModeStyleL
// From MSwtSortedList
// This function should be called only once.
// It creates the filter if the SortedList has the FILTER style.
// ---------------------------------------------------------------------------
//
void CSwtSortedList::SetModeStyleL(TInt aModeStyle)
{
    iModeStyle = aModeStyle;

    if ((iModeStyle & ESwtFilter) != 0)
    {
        iSearchField = CAknSearchField::NewL(*this,
                                             CAknSearchField::EAdaptiveSearch, NULL, KSwtSearchFieldTextLimit);
        iSearchField->Editor().AddEdwinObserverL(this);

        // Orphane the filter to force its editor to draw CAknSearchField's
        // background instead of our Shell's background.
        iSearchField->SetParent(NULL);

        iSearchField->AddAdaptiveSearchTextObserverL(this);
        iSearchField->SetComponentsToInheritVisibility(ETrue);
        CAknFilteredTextListBoxModel* model = STATIC_CAST(
                                                  CAknFilteredTextListBoxModel*, iList->Model());
        ASSERT(model);
        if (!model->Filter())
        {
            model->CreateFilterL(iList, iSearchField);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::SetSelectionL
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
void CSwtSortedList::SetSelectionL(MDesCArray* aStringArray)
{
    ASSERT(aStringArray != NULL);

    TInt pos;
    TInt granularity(KSwtArrayGranularity);
    TInt nbSelection(aStringArray->MdcaCount());
    TKeyArrayFix keyArray(0, ECmpTUint);

    if (nbSelection == 0 || iTextItems->Count() == 0)
    {
        return;
    }

    CArrayFixFlat<TInt>* selectionIndexes = new(ELeave) CArrayFixFlat<TInt>(granularity);
    CleanupStack::PushL(selectionIndexes);
    CAknListBoxFilterItems* filter = NULL;
    if (iSearchField)
    {
        filter = Filter();
    }
    for (TInt numString=0; numString < nbSelection; ++numString)
    {
        if (iTextItems->Find(aStringArray->MdcaPoint(numString), pos, ECmpCollated) == 0)
        {
            if (iSearchField && filter)
            {
                pos = filter->VisibleItemIndex(pos);
            }
            if (pos != -1)
            {
                selectionIndexes->InsertIsqL(pos, keyArray);
            }
        }
    }

    iList->ClearSelection();

    // No selected index is found, return.
    if (selectionIndexes->Count() > 0)
    {
        TInt old = iList->CurrentItemIndex();
        // If more than one index is selected and List is Single-select, return
        if (!((iStyle & KSwtStyleSingle) && (selectionIndexes->Count() > 1)))
        {
            iList->View()->SetSelectionIndexesL(selectionIndexes);
        }
        iList->SetCurrentItemIndex(selectionIndexes->At(0));
        iList->View()->DrawItem(selectionIndexes->At(0));
        if (old != -1)
        {
            iList->View()->DrawItem(old);
        }
    }

    CleanupStack::PopAndDestroy(selectionIndexes);

    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::ShowSelection
// From MSwtSortedList
// ---------------------------------------------------------------------------
//
void CSwtSortedList::ShowSelection()
{
    CSwtListBase::ShowSelection();
}

// ---------------------------------------------------------------------------
// CSwtSortedList::HandleEdwinEventL
// From MEikEdwinObserver
// ---------------------------------------------------------------------------
//
void CSwtSortedList::HandleEdwinEventL(CEikEdwin* /*aEdwin*/, TEdwinEvent aEventType)
{
    if (aEventType == EEventTextUpdate)
    {
        HandleOfferKeyEventL();
    }
}

// ---------------------------------------------------------------------------
// CSwtSortedList::AdaptiveSearchTextChanged
// From MAdaptiveSearchTextObserver
// ---------------------------------------------------------------------------
//
void CSwtSortedList::AdaptiveSearchTextChanged(CAknSearchField* /*aSearchField*/)
{
    TRAP_IGNORE(HandleOfferKeyEventL());
}

