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


#include <eswtcore.rsg>
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <avkon.mbg>
#include <gulicon.h>
#include <swtlaffacade.h>
#include "eswtmobileextensions.h"
#include "swtcontrolhelper.h"
#include "swttable.h"
#include "swttableitem.h"
#include "swttableitemdrawer.h"
#include "eikbtgpc.h"
#include "swttablelistbox.h"
#include "swttablelistboxview.h"
#include "swtimagetextitem.h"
#include "swtuiutils.h"
#include "swttableheader.h"
#include "swttablecolumn.h"


// ======== MEMBER FUNCTIONS ========


CSwtTable* CSwtTable::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite& aParent, TInt aStyle)
{
    CSwtTable* self = new(ELeave) CSwtTable(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}


CSwtTable::~CSwtTable()
{
    ASSERT(iTableItems.Count() == 0);
    ASSERT(iTableColumns.Count() == 0);

    iTableItems.Close();
    iTableColumns.Close();
    delete iHeader;
    iHeader = NULL;
    delete iTableListBox;
    iTableListBox = NULL;
    iColumnWidths.Close();
    iImageTextItems.Close();
    iColumnAlignments.Close();
    iColumnImagesCounts.Close();
    delete iCheckBoxOnBitmap;
    delete iCheckBoxOnMask;
    delete iCheckBoxOffBitmap;
    delete iCheckBoxOffMask;
    iCheckBoxOnBitmap = NULL;
    iCheckBoxOnMask = NULL;
    iCheckBoxOffBitmap = NULL;
    iCheckBoxOffMask = NULL;
    delete iOldSelectionArray;
}


const RPointerArray<CSwtImageTextItem>& CSwtTable::GetImageTextItems() const
{
    return iImageTextItems;
}


void CSwtTable::PostSelectionEventL(TInt aDetail, TInt aItemIndex) const
{
    ASSERT(aItemIndex != -1);

    if ((aDetail == KSwtStyleCheck) && !(Style() & KSwtStyleCheck))
    {
        aDetail = KSwtNone;
    }

    TSwtPeer itemPeer(GetItemL(aItemIndex));
    TRAP_IGNORE(iDisplay.PostSelectionEventL(iPeer, aDetail, itemPeer));
}


void CSwtTable::PostDefaultSelectionEventL(TInt aItemIndex) const
{
    ASSERT(aItemIndex != -1);

    TSwtPeer itemPeer(GetItemL(aItemIndex));
    TInt detail = KSwtNone;
    if (Style() & KSwtStyleCheck)
    {
        detail = KSwtStyleCheck;
    }
    iDisplay.PostDefaultSelectionEventL(iPeer, detail, itemPeer);
}


void CSwtTable::PostColumnSelectionEventL(TSwtPeer aColumn) const
{
    iDisplay.PostSelectionEventL(aColumn);
}


void CSwtTable::PostColumnResizeEventL(TSwtPeer aColumn) const
{
    iDisplay.PostResizeEventL(aColumn);
}


void CSwtTable::PostColumnMoveEventL(TSwtPeer aColumn) const
{
    iDisplay.PostMoveEventL(aColumn);
}


// ---------------------------------------------------------------------------
// The header is placed above the view rectangle of the list box.
// ---------------------------------------------------------------------------
//
TRect CSwtTable::GetHeaderRect() const
{
    TRect clientRect(BorderInnerRect());
    TPoint headerTl(clientRect.iTl);

    TInt columnCount(GetColumnCount());
    TInt headerWidth(0);
    for (TInt i = 0; i < columnCount; ++i)
    {
        headerWidth += GetColumnWidth(i);
    }
    TSwtTableMargins margins = GetMargins();
    headerWidth += margins.iLeft;
    headerWidth += margins .iRight;
    if (Style() & KSwtStyleMulti || Style() & KSwtStyleCheck)
    {
        headerWidth += GetCheckBoxSize().iWidth;
    }
    TRect headerRect(headerTl,
                     TSize(Max(headerWidth, clientRect.Width()),
                           GetHeaderHeight()));
    return headerRect;
}


TSwtTableMargins CSwtTable::GetMargins() const
{
    TSwtTableMargins margins;
    margins.iTop          = iHighlightTopBottomMargin;
    margins.iBottom       = iHighlightTopBottomMargin;
    margins.iLeft         = iHighlightLeftRightMargin;
    margins.iRight        = iHighlightLeftRightMargin;
    margins.iBetweenItems = iHighlightLeftRightMargin / 3;
    return margins;
}


TSize CSwtTable::GetCheckBoxSize() const
{
    return iCheckBoxSize;
}


void CSwtTable::GetCheckBoxBitmaps(const TBool& aChecked,
                                   const CFbsBitmap** aBitmap,
                                   const CFbsBitmap** aMask) const
{
    if (aChecked)
    {
        *aBitmap = iCheckBoxOnBitmap;
        *aMask   = iCheckBoxOnMask;
    }
    else
    {
        *aBitmap = iCheckBoxOffBitmap;
        *aMask   = iCheckBoxOffMask;
    }
}


TInt CSwtTable::IndexOfTableItem(const CSwtImageTextItem* aItem) const
{
    if (!aItem)
        return -1;

    for (TInt i = 0; i < iImageTextItems.Count(); ++i)
    {
        if (iImageTextItems[ i ] == aItem)
        {
            return i;
        }
    }
    return -1;
}


TRgb CSwtTable::GetHighlightTextRgb() const
{
    if (iTableForegroundSet)
    {
        return iTableForeground;
    }
    else
    {
        return iTableDefaultHighlightText;
    }
}


TBool CSwtTable::BackgroundSet() const
{
    return iTableBackgroundSet;
}


TBool CSwtTable::IsChecked(const TInt& aIndex) const
{
    if (Style() & KSwtStyleCheck)
    {
        if (aIndex < 0 || aIndex >= iTableItems.Count())
        {
            return EFalse;
        }
        return iTableListBox->View()->ItemIsSelected(aIndex);
    }
    return EFalse;
}


void CSwtTable::CheckItem(const TInt& aIndex)
{
    TRAP_IGNORE
    (
        iTableListBox->SelectItemL(aIndex, EFalse);
        UpdateMSKLabelL();
    );
}


void CSwtTable::UncheckItem(const TInt& aIndex)
{
    iTableListBox->DeselectRange(aIndex, aIndex);
    TRAP_IGNORE(UpdateMSKLabelL());
}


TBool CSwtTable::ListMode() const
{
    return !(GetTableColumnCount() > 0);
}


void CSwtTable::UpdateHeaderHScrollOffset(const TInt& aHScrollOffset)
{
    iHeader->SetHScrollOffset(aHScrollOffset);
}


void CSwtTable::ListBoxDefaultFontChanged()
{
    const MSwtFont* currentTableFont = GetFont();
    TSwtTableMargins margins = GetMargins();

    // The default font changed but we don't know if it's used by any
    // item/cell. So let's not pass the default font height here but
    // instead go through everything to find the highest one.
    UpdateItemHeight(0);

    iHeader->SetFont(&currentTableFont->Font());

    // Header height might have changed due to the font change.
    LayoutTable();
}


void CSwtTable::HandlePhysics(TBool aStatus)
{
    GetShell().SetUrgentPaintControl(aStatus ? this : NULL);
}


const TRgb* CSwtTable::CustomForeground() const
{
    if (iTableForegroundSet)
        return &iTableForeground;
    else
        return NULL;
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtTable::CountComponentControls() const
{
    TInt count(0);
    if (iTableListBox)
    {
        ++count;
    }
    if (iHeader)
    {
        ++count;
    }
    return count;
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtTable::ComponentControl(TInt aIndex) const
{
    CCoeControl* control = NULL;

    // It is important that the header draws after the list. Kinetic scrolling
    // can cause slight incorrect draw of the list on top of the header buttons.
    if (aIndex == 0)
    {
        control = iTableListBox;
    }
    else if (aIndex == 1)
    {
        control = iHeader;
    }

    return control;
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTable::SizeChanged()
{
    LayoutTable();
    CSwtComposite::SizeChanged();
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTable::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(HandleResourceChangeL(aType));
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtTable::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    TBool traversalDoIt = EFalse;
    TSwtTraversal detail = ESwtTraverseNone;
    TInt currentItemRowIndex = 0;
    TInt lastItemRowIndex = 0;
    TInt itemCount(GetItemCount());

    // Up/down arrows, try to traverse if not at the middle
    if (aKeyEvent.iCode == EKeyDownArrow ||
            aKeyEvent.iCode == EKeyUpArrow)
    {
        if (itemCount > 0)
        {
            currentItemRowIndex = iTableListBox->View()->CurrentItemIndex();
            lastItemRowIndex = itemCount - 1;
        }
        else
        {
            // In empty table this makes it to traverse
            currentItemRowIndex = 0;
            lastItemRowIndex = 0;
        }
    }
    if (aKeyEvent.iCode == EKeyDownArrow)
    {
        if (currentItemRowIndex == lastItemRowIndex)
        {
            detail = ESwtTraverseArrowNext;
        }
    }
    else if (aKeyEvent.iCode == EKeyUpArrow)
    {
        if (currentItemRowIndex == 0)
        {
            detail = ESwtTraverseArrowPrevious;
        }
    }
    // Left/right arrows, always try to traverse
    if (aKeyEvent.iCode == EKeyLeftArrow)
    {
        if (!iTableListBox->TableView()->CanScrollLeft())
        {
            detail = ESwtTraverseArrowPrevious;
        }
    }
    else if (aKeyEvent.iCode == EKeyRightArrow)
    {
        if (!iTableListBox->TableView()->CanScrollRight())
        {
            detail = ESwtTraverseArrowNext;
        }
    }

    // Tab key traversal is handled in HandleKeyL
    if (aKeyEvent.iCode == EKeyTab)
    {
        traversalDoIt = ETrue;
    }

    if (detail != ESwtTraverseNone &&
            (GetShell().FindTraversalTargetL(detail, *this)))
    {
        traversalDoIt = ETrue;
    }
    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtTable::FocusChanged(TDrawNow aDrawNow)
{
    TBool isFocused = IsFocusControl();
#ifdef RD_JAVA_S60_RELEASE_9_2
    EnableFocusHighlight(isFocused);
#endif //RD_JAVA_S60_RELEASE_9_2
    iTableListBox->SetFocus(isFocused);
    HandleFocusChanged(aDrawNow);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtTable::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == MAknsControlContext::ETypeId)
    {
        return TTypeUid::Null();
    }

    return ASwtControlBase::SwtMopSupplyObject(aId);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Overridden to prevent the background drawing implemented by CSwtComposite.
// The component controls take care of painting the background for Table.
// ---------------------------------------------------------------------------
//
void CSwtTable::Draw(const TRect& /*aRect*/) const
{
}

// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtTable::GetTableItemsL()
{
    const TInt itemCount = iTableItems.Count();

    if (itemCount == 0)
    {
        return NULL;
    }

    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(itemCount);
    CleanupStack::PushL(array);

    for (TInt i = 0; i < itemCount; ++i)
    {
        MSwtTableItem* item = iTableItems[ i ];
        array->AppendL(item->JavaPeer());
    }

    CleanupStack::Pop(array);
    return array;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtTable::GetTableColumnsL()
{
    const TInt columnCount = iTableColumns.Count();

    if (columnCount == 0)
    {
        return NULL;
    }

    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(columnCount);
    CleanupStack::PushL(array);

    for (TInt i = 0; i < columnCount; ++i)
    {
        MSwtTableColumn* column = iTableColumns[ i ];
        array->AppendL(column->JavaPeer());
    }

    CleanupStack::Pop(array);
    return array;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const RArray<TInt>& CSwtTable::GetColumnWidths() const
{
    return iColumnWidths;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const RArray<TInt>& CSwtTable::GetColumnAlignments() const
{
    return iColumnAlignments;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const RArray<TInt>& CSwtTable::GetColumnImageCounts() const
{
    return iColumnImagesCounts;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
MSwtComposite* CSwtTable::Composite()
{
    return this;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::AddTableItemL(MSwtTableItem& aChild, TInt aRowIndex)
{
    // Store the selection state
    iTableListBox->TableView()->GetSelectionIndexesL(iOldSelectionArray);

    CDesCArray* itemTextArray = static_cast< CDesCArray* >(
                                    iTableListBox->Model()->ItemTextArray());
    TRAPD(error, itemTextArray->AppendL(KNullDesC));
    if (error)
    {
        User::Leave(ESwtErrorItemNotAdded);
    }

    // Append the child to the list now.
    error = iTableItems.Insert(&aChild, aRowIndex);
    if (error != KErrNone)
    {
        itemTextArray->Delete(itemTextArray->Count() - 1);
        User::Leave(ESwtErrorItemNotAdded);
    }

    error = iImageTextItems.Insert(
                static_cast<CSwtTableItem&>(aChild).GetImageTextItem(), aRowIndex);
    if (error != KErrNone)
    {
        iTableItems.Remove(aRowIndex);
        itemTextArray->Delete(itemTextArray->Count() - 1);
        User::Leave(ESwtErrorItemNotAdded);
    }
    // The focus is on the newly added item.
    iTableListBox->SetCurrentItemIndex(aRowIndex);

    iTableListBox->SetRequiredHeightInNumOfItems(GetItemCount());
    iTableListBox->HandleItemAdditionL();

    // The added item is shown as the first item in the view.
    iTableListBox->SetTopItemIndex(aRowIndex);

    // Restore the selection state updating the indexes.
    UpdateSelectionIndexesL(aRowIndex, aRowIndex, iOldSelectionArray, ETrue);

    if (ListMode())
    {
        TInt newItemWidth = iImageTextItems[ aRowIndex ]->GetPreferredListModeItemWidth();
        if (newItemWidth > GetColumnWidth(0))
        {
            SetColumnWidth(0, newItemWidth);
        }
    }

    UpdateMSKLabelL();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::AddTableColumnL(MSwtTableColumn& aChild, TInt aColumnIndex)
{
    ASSERT(aColumnIndex <= iTableColumns.Count());

    TInt err(KErrNone);
    TInt oldColumnCount(GetTableColumnCount());
    TInt iUpdatedTableItem(0);
    TBool headerUpdated(EFalse);

    // If this is the first TableColumn added by the application then we are
    // in List mode and we actually have these items for the List mode column
    // already and we don't want to add new ones.
    if (oldColumnCount > 0)
    {
        User::LeaveIfError(iColumnWidths.Insert(0, aColumnIndex));
        err = iColumnAlignments.Insert(CGraphicsContext::ELeft, aColumnIndex);
        if (err == KErrNone)
        {
            err = iColumnImagesCounts.Insert(0, aColumnIndex);
        }
        if (err == KErrNone)
        {
            TRAP(err, iHeader->InsertNewColumnL(aColumnIndex));
            if (err == KErrNone)
            {
                MCoeControlObserver* observer = static_cast<CSwtTableColumn*>(&aChild);
                iHeader->SetColumnHeaderObserver(observer, aColumnIndex);
                headerUpdated = ETrue;
            }
        }
        if (err == KErrNone)
        {
            // Add the column to each TableItem
            TInt nAllItems(iTableItems.Count());
            for (; iUpdatedTableItem < nAllItems; iUpdatedTableItem++)
            {
                TRAP(err, iTableItems[ iUpdatedTableItem ]->InsertNewColumnL(aColumnIndex));
                if (err != KErrNone)
                {
                    break;
                }
            }
        }
    }
    // Add the TableColumn, this we don't have in List mode so it's added always
    if (err == KErrNone)
    {
        err = iTableColumns.Insert(&aChild, aColumnIndex);
        if (err == KErrNone)
        {
            MCoeControlObserver* observer = static_cast<CSwtTableColumn*>(&aChild);
            iHeader->SetColumnHeaderObserver(observer, aColumnIndex);
        }
    }

    // If something went wrong then restore the arrays and leave.
    if (err != KErrNone)
    {
        for (TInt i = 0; i < iUpdatedTableItem; ++i)
        {
            iTableItems[ i ]->RemoveColumn(aColumnIndex);
        }
        if (headerUpdated)
        {
            iHeader->SetColumnHeaderObserver(NULL, aColumnIndex);
            iHeader->RemoveColumn(aColumnIndex);
        }
        if (iColumnImagesCounts.Count() > oldColumnCount)
        {
            iColumnImagesCounts.Remove(aColumnIndex);
        }
        if (iColumnAlignments.Count() > oldColumnCount)
        {
            iColumnAlignments.Remove(aColumnIndex);
        }
        if (iColumnWidths.Count() > oldColumnCount)
        {
            iColumnWidths.Remove(aColumnIndex);
        }
        User::Leave(err);
    }

    iTableListBox->HandleDataSizeChangeL();

    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void  CSwtTable::RemoveTableColumn(MSwtTableColumn& aChild)
{
    TInt nbColumns = GetTableColumnCount();
    TInt columnIndex = IndexOfTableColumn(&aChild);
    ASSERT(columnIndex != KErrNotFound);

    // Keep a column for the List-mode
    if (nbColumns > 1)
    {
        iColumnWidths.Remove(columnIndex);
        iColumnAlignments.Remove(columnIndex);
        iColumnImagesCounts.Remove(columnIndex);
        iHeader->SetColumnHeaderObserver(NULL, columnIndex);
        iHeader->RemoveColumn(columnIndex);
        TInt nbItems(iTableItems.Count());
        for (TInt i = 0; i < nbItems; i++)
        {
            iTableItems[ i ]->RemoveColumn(columnIndex);
        }
    }
    else
    {
        TRAP_IGNORE(iHeader->SetTextL(0, KNullDesC));
    }

    // remove the TableColumn
    iTableColumns.Remove(columnIndex);

    // The column alignment on first column is always Left
    iColumnAlignments[0] = CGraphicsContext::ELeft;

    if (ListMode())
    {
        PackColumn(0);
    }

    TRAP_IGNORE(iTableListBox->HandleDataSizeChangeL());

    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::DeselectItems(const TInt* aIndices, TInt aCount)
{
    if (!(Style() & KSwtStyleMulti))
    {
        return;
    }
    iTableListBox->DeselectItems(aIndices, aCount);
    TRAP_IGNORE(UpdateMSKLabelL());
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::DeselectRange(TInt aStart, TInt aEnd)
{
    if (!(Style() & KSwtStyleMulti))
    {
        return;
    }
    iTableListBox->DeselectRange(aStart, aEnd);
    TRAP_IGNORE(UpdateMSKLabelL());
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::IndexOfTableColumn(const MSwtTableColumn *aColumn) const
{
    // We return -1 if not found.
    return iTableColumns.Find(aColumn);
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::IndexOfTableItemAt(const MSwtTableItem *aItem, TInt aStart) const
{
    if (aStart < 0 || aStart >= GetItemCount())
    {
        return -1;
    }

    if (aStart == 0)
    {
        TInt pos = 0;
        while (pos < iTableItems.Count())
        {
            if (iTableItems[pos] == aItem)
            {
                return pos;
            }
            else
            {
                pos++;
            }
        }
    }
    else
    {
        for (TInt i = aStart; i < iTableItems.Count(); ++i)
        {
            if (aItem == iTableItems[i])
            {
                return i;
            }
        }
    }
    return -1;
}

TInt CSwtTable::IndexOfTableItem(const MSwtTableItem *aItem) const
{
    // We return -1 if not found.
    return iTableItems.Find(aItem);
}

// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetColumnCount() const
{
    return iColumnWidths.Count();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetTableColumnCount() const
{
    return iTableColumns.Count();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TBool CSwtTable::GetHeaderVisible() const
{
    return iHeaderVisible;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTable::GetItemL(TInt aIndex) const
{
    if (aIndex < 0 || aIndex >= iTableItems.Count())
    {
        User::Leave(ESwtErrorInvalidRange);
    }
    return iTableItems[ aIndex ]->JavaPeer();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTable::GetColumnL(TInt aIndex) const
{
    // Range is already checked in java side
    if (aIndex < 0 || aIndex >= iTableColumns.Count())
    {
        User::Leave(ESwtErrorInvalidRange);
    }
    return iTableColumns[ aIndex ]->JavaPeer();
}

TInt CSwtTable::GetItemCount() const
{
    return iTableItems.Count();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TRect CSwtTable::GetItemRect(TInt aRowIndex) const
{
    TRect itemRect(iTableListBox->GetItemRect(aRowIndex));

    TRect rect(BorderInnerRect());
    if (iHeaderVisible)
    {
        rect.iTl.iY += GetHeaderHeight();
    }
    TPoint origin(SbInnerRect(rect).iTl);
    itemRect.Move(-origin);
    return itemRect;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TBool CSwtTable::GetLinesVisible() const
{
    return iLinesVisible;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetSelectionIndex() const
{
    if (!(iStyle & KSwtStyleMulti))
    {
        return GetFocusIndex();
    }

    // Multi

    const CArrayFix< TInt >* selectionIndices = iTableListBox->SelectionIndexes();

    if (!selectionIndices)
    {
        return -1;
    }

    // We want the first index
    TInt index = -1;    // This is for no item selected.
    for (TInt i = 0; i < selectionIndices->Count(); i++)
    {
        TInt currentIndex = selectionIndices->At(i);
        if ((index == -1) || (currentIndex < index))
        {
            index = currentIndex;
        }
    }
    return index;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetSelectionCount() const
{
    if (iStyle & KSwtStyleMulti)
    {
        return iTableListBox->GetSelectionCount();
    }
    else
    {
        if (GetItemCount() > 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const CArrayFix<TInt>* CSwtTable::GetSelectionIndicesL() const
{
    CArrayFix<TInt>* array = NULL;
    if (iStyle & KSwtStyleMulti)
    {
        const CArrayFix<TInt>* selectionArray = iTableListBox->SelectionIndexes();
        array = new(ELeave) CArrayFixFlat<TInt>(selectionArray->Length());
        CleanupStack::PushL(array);
        for (TInt i = 0; i < selectionArray->Count(); ++i)
        {
            array->AppendL(selectionArray->At(i));
        }
        CleanupStack::Pop(array);
    }
    else
    {
        TInt selectedItems = GetItemCount() > 0 ? 1 : 0;
        array = new(ELeave) CArrayFixFlat<TInt>(1);
        if (selectedItems)
        {
            array->AppendL(GetFocusIndex());
        }
    }
    return array;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const TSize& CSwtTable::ImageSize() const
{
    return iImageSize;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::NotifyImageAdded(TInt aColumnIndex)
{
    ++iColumnImagesCounts[ aColumnIndex ];
    ASSERT(iColumnImagesCounts[ aColumnIndex ] <= GetItemCount());
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::NotifyImageRemoved(TInt aColumnIndex)
{
    --iColumnImagesCounts[ aColumnIndex ];
    ASSERT(iColumnImagesCounts[ aColumnIndex ] >= 0);
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::PackColumn(TInt aColumnIndex)
{
    TInt maxWidth = 0;
    TInt nbItems = iTableItems.Count();

    for (TInt itemIndex = 0; itemIndex < nbItems; itemIndex++)
    {
        TInt currentWidth = iTableItems[ itemIndex ]->
                            GetPreferredSize(aColumnIndex).iWidth;
        if (maxWidth < currentWidth)
        {
            maxWidth = currentWidth;
        }
    }

    TInt headerWidth = iHeader->GetPreferredSize(aColumnIndex).iWidth;
    if (maxWidth < headerWidth)
    {
        maxWidth = headerWidth;
    }

    SetColumnWidth(aColumnIndex, maxWidth);
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::RemoveItemL(TInt aIndex)
{
    if (aIndex < 0 ||
            aIndex >= iTableItems.Count())
    {
        User::Leave(ESwtErrorInvalidRange);
    }
    RemoveRangeL(aIndex, aIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::RemoveRangeL(TInt aStart, TInt aEnd)
{
    if (aStart < 0 ||
            aStart >= iTableItems.Count() ||
            aEnd < 0 ||
            aEnd >= iTableItems.Count())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    iTableListBox->TableView()->SetDisableRedraw(ETrue);

    // Store the selection state
    TInt oldFocusIndex = GetFocusIndex();
    iTableListBox->TableView()->GetSelectionIndexesL(iOldSelectionArray);

    CDesCArray* itemTextArray = static_cast< CDesCArray* >(
                                    iTableListBox->Model()->ItemTextArray());
    itemTextArray->Delete(aStart, aEnd - aStart + 1);    //lint !e834

    iTableListBox->SetRequiredHeightInNumOfItems(GetItemCount());

    for (TInt i = aStart; i <= aEnd; ++i)
    {
        iImageTextItems.Remove(aStart);
        iTableItems.Remove(aStart);
    }

    iTableListBox->HandleItemRemovalL();

    // Take care of updating the focus index if the focused item was removed
    TInt focusIndex = GetFocusIndex();
    if (oldFocusIndex != focusIndex)
    {
        // Focus moves to the next item
        focusIndex = aStart;
    }
    if (focusIndex > GetItemCount() - 1)
    {
        // Focus loops around to the first item
        focusIndex = 0;
    }
    iTableListBox->SetCurrentItemIndex(focusIndex);

    if (focusIndex >= 0)
    {
        iTableListBox->ScrollToMakeItemVisible(focusIndex);
    }

    // Restore the selection state updating the indexes.
    UpdateSelectionIndexesL(aStart, aEnd, iOldSelectionArray, EFalse);

    iTableListBox->TableView()->SetDisableRedraw(EFalse);

    if (ListMode())
    {
        PackColumn(0);
    }
    else
    {
        Redraw();
    }

    UpdateMSKLabelL();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::RemoveItemsL(const TInt* aIndices, TInt aCount)
{
    if (aIndices[0] < 0 ||
            aIndices[0] >= iTableItems.Count() ||
            aIndices[aCount-1] < 0 ||
            aIndices[aCount-1] >= iTableItems.Count())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    // Store the focus index
    TInt oldFocusIndex = GetFocusIndex();

    TInt oldIndex = -1;
    CDesCArray* itemTextArray = static_cast< CDesCArray* >(
                                    iTableListBox->Model()->ItemTextArray());
    for (TInt i = 0; i < aCount; ++i)
    {
        // Indexes must be sorted in descending order.
        // This avoids removing the same index twice.
        ASSERT((oldIndex == -1) || (aIndices[ i ] <= oldIndex));

        // Store the selection state
        iTableListBox->TableView()->GetSelectionIndexesL(iOldSelectionArray);

        if (aIndices[i] != oldIndex)
        {
            TInt index = aIndices[ i ];
            itemTextArray->Delete(index);
            iImageTextItems.Remove(index);
            iTableItems.Remove(index);

            oldIndex = index;

            // Restore the selection state updating the indexes.
            UpdateSelectionIndexesL(index, index, iOldSelectionArray, EFalse);
        }
    }

    iTableListBox->SetRequiredHeightInNumOfItems(GetItemCount());
    iTableListBox->HandleItemRemovalL();

    // Take care of updating the focus index if the focused item was removed
    TInt focusIndex = GetFocusIndex();
    if (oldFocusIndex != focusIndex)
    {
        // Focus moves to the next item
        focusIndex = oldFocusIndex;
    }
    if (focusIndex > GetItemCount() - 1)
    {
        // Focus loops around to the first item
        focusIndex = 0;
    }
    iTableListBox->SetCurrentItemIndex(focusIndex);

    if (focusIndex >= 0)
    {
        iTableListBox->ScrollToMakeItemVisible(focusIndex);
    }

    if (ListMode())
    {
        PackColumn(0);
    }
    else
    {
        Redraw();
    }

    UpdateMSKLabelL();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::RemoveAll()
{
    if (GetItemCount() == 0)
    {
        return;
    }

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(
                                    iTableListBox->Model()->ItemTextArray());
    itemTextArray->Reset();

    iTableListBox->SetRequiredHeightInNumOfItems(0);

    // As there are no more elements, we must reset the cell width to 0,
    // because MinimumSize is using this value to compute the max width
    // of the items, according to the longest string.
    iTableListBox->TableItemDrawer()->SetCellWidthInChars(0);

    // We remove the children to the list now
    iImageTextItems.Reset();
    iTableItems.Reset();

    iTableListBox->SetCurrentItemIndex(0);
    TRAP_IGNORE(iTableListBox->HandleItemRemovalL());

    if (ListMode())
    {
        PackColumn(0);
    }
    else
    {
        Redraw();
    }
    TRAP_IGNORE(UpdateMSKLabelL());
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SelectItemL(TInt aIndex, TBool aScroll)
{
    ASSERT(aIndex >= 0);   // Checked in Java side

    if (aIndex > GetItemCount() -1)
    {
        return;
    }

    iTableListBox->SelectItemL(aIndex, aScroll);

    UpdateMSKLabelL();
}

// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SelectAllL()
{
    iTableListBox->SelectAllL();
    UpdateMSKLabelL();
}

// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetFocusIndex(TInt aIndex)
{
    ASSERT(iTableListBox);
    if (aIndex < 0 || aIndex >= GetItemCount())
    {
        return;
    }

    iTableListBox->SetFocusIndex(aIndex);

    TRAP_IGNORE(UpdateMSKLabelL());
}

// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetHeaderVisible(TBool aShow)
{
    TBool oldVisibility(iHeaderVisible);
    // Maybe a call to NotifyImageRemoved or NotifyImageAdded could be done here
    // for each column when the header visibility is modified.
    iHeaderVisible = aShow;
    // Recompute the size of the TableListBox, with or without the header.
    LayoutTable();

    if (oldVisibility != iHeaderVisible)
    {
        TRAP_IGNORE(iTableListBox->HandleViewRectSizeChangeL());
    }

    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetLinesVisible(TBool aShow)
{
    iLinesVisible = aShow;
    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SelectRangeL(TInt aStart, TInt aEnd)
{
    // This function never gets called for single table due to Java side checks.
    iTableListBox->SelectRangeL(aStart, aEnd);
    UpdateMSKLabelL();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::ShowSelection()
{
    if (Style() & KSwtStyleMulti)
    {
        iTableListBox->ShowSelection();
    }
    else
    {
        TInt focusIndex = GetFocusIndex();
        if (focusIndex >= 0)
        {
            iTableListBox->ScrollToMakeItemVisible(focusIndex);
        }
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtTable::GetSelectionL()
{
    const TInt itemCount = iTableItems.Count();

    if (itemCount == 0)
    {
        return NULL;
    }

    if (Style() & KSwtStyleMulti)
    {
        const CArrayFix< TInt >* selectionArray = iTableListBox->SelectionIndexes();
        if (!selectionArray || selectionArray->Count() == 0)
        {
            return NULL;
        }

        CSwtPeerArray* array = new(ELeave) CSwtPeerArray(selectionArray->Count());
        CleanupStack::PushL(array);

        for (TInt i = 0; i < selectionArray->Count(); ++i)
        {
            MSwtTableItem* item = iTableItems[ selectionArray->At(i)];  //lint !e613
            array->AppendL(item->JavaPeer());
        }

        CleanupStack::Pop(array);
        return array;
    }
    else
    {
        CSwtPeerArray* array = new(ELeave) CSwtPeerArray(1);
        CleanupStack::PushL(array);
        MSwtTableItem* item = iTableItems[ GetFocusIndex()];  //lint !e613
        array->AppendL(item->JavaPeer());
        CleanupStack::Pop(array);
        return array;
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TBool CSwtTable::IsSelected(TInt aIndex) const
{
    if (Style() & KSwtStyleMulti)
    {
        if (aIndex < 0 || aIndex >= iTableItems.Count())
        {
            return EFalse;
        }
        return iTableListBox->View()->ItemIsSelected(aIndex);
    }
    else
    {
        return GetItemCount() > 0 ? (GetFocusIndex() == aIndex) : EFalse;
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetTopIndex() const
{
    return iTableListBox->TopItemIndex();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetTopIndex(TInt aIndex)
{

    TInt count = GetItemCount();

    if (count == 0)
    {
        return;
    }

    aIndex = Min(Max(0, aIndex), count - 1);

    iTableListBox->SetTopItemIndex(aIndex);

    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetColumnTextL(MSwtTableColumn& aTableColumn, const TDesC& aText)
{
    TInt indexColumn = IndexOfTableColumn(&aTableColumn);
    iHeader->SetTextL(indexColumn, aText);
    RedrawRegion(GetHeaderRect());
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetColumnWidth(TInt aColumnIndex) const
{
    return iColumnWidths[ aColumnIndex ];
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetColumnAlignment(TInt aColumnIndex)
{
    return iColumnAlignments[ aColumnIndex ];
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetColumnWidth(TInt aColumnIndex, TInt aWidth)
{
    ASSERT(aColumnIndex >= 0);
    ASSERT(aColumnIndex < iColumnWidths.Count());

    // Set only if positive, ignore otherwise
    if (aWidth >= 0)
    {
        iColumnWidths[ aColumnIndex ] = aWidth;
        iHeader->UpdateColumnWidth(aColumnIndex, aWidth);
        TRAP_IGNORE(iTableListBox->HandleDataSizeChangeL());

        // Send column resize and move events
        TInt colCount(GetTableColumnCount());
        TRAP_IGNORE
        (
            for (TInt i = aColumnIndex; i < colCount; ++i)
    {
        if (i == aColumnIndex)
            {
                PostColumnResizeEventL(iTableColumns[ i ]->JavaPeer());
            }
            else
            {
                PostColumnMoveEventL(iTableColumns[ i ]->JavaPeer());
            }
        }
        );
        Redraw();
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetColumnAlignment(TInt aColumnIndex, TInt aAlignment)
{
    ASSERT(aColumnIndex>=0);
    ASSERT(aColumnIndex<iColumnAlignments.Count());
    iColumnAlignments[ aColumnIndex ] = aAlignment;
    iHeader->UpdateColumnAlignment(aColumnIndex, aAlignment);
    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetTableImageSizeIfNotDefined(const TSize& aSize,
        const TBool& aAddedToItem)
{
    if (iImageSize == TSize(0, 0))
    {
        iImageSize = aSize;
        if (ListMode())
        {
            PackColumn(0);
        }
    }
    if (aAddedToItem)
    {
        UpdateItemHeight(iImageSize.iHeight);
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// Updates the table item (row) height. The items all have the same height.
//
// The height of the
// - image
// - checkbox
// - margins
// are the same for all items. The only item that has varying height between
// different table items is the font.
//
// As a parameter the font height of the changed item may be passed. It is
// used to check if the given height would require the common item height of
// all items to be increased. In this case it is not necessary to loop through
// all the items. If the given height is smaller than the current common item
// height of all items then all the items have to be looped through to find
// the one with the highest font that determines the minimum allowed common
// item height for all the items.
//
// Also the image size may be passed as the parameter when it is first set
// (done only once). Then if the image size would require to increase the
// common item height it is just set without any investigation. If the image
// size would require to decrease the common item height then all items are
// looped through and the highest font determines the common item height
// (actually this is useless because in this case the height is already
// correct, but currently the logic doesn't know is the given height is
// the image or the font).
//
// It is always checked if the checkbox or the image size requires more
// height than the given font. In that case they determine the item height
// and items don't need to be looped through.
// ---------------------------------------------------------------------------
//
void CSwtTable::UpdateItemHeight(const TInt& aChangedFontHeight)
{
    TInt columnCount(GetColumnCount());
    TInt itemCount(GetItemCount());
    TInt marginTop(GetMargins().iTop);
    TInt marginBottom(GetMargins().iBottom);

    // Check if current is in the view before updating item height.
    // If it is then it will be kept in the view.
    TInt currentItemIndex = iTableListBox->CurrentItemIndex();
    TBool currentItemWasVisible = EFalse;
    if (currentItemIndex >= 0)
    {
        currentItemWasVisible =
            iTableListBox->TableView()->ItemIsVisible(currentItemIndex);
    }

    // Initially the highest item found so far is the changed font height.
    TInt highestHeight(aChangedFontHeight);

    // If there are images then check if images would require more height.
    TBool images = EFalse;
    for (TInt i = 0; i < columnCount; ++i)
    {
        if (iColumnImagesCounts[ i ] != 0)
        {
            images = ETrue;
            break;
        }
    }
    if (images)
    {
        highestHeight = Max(iImageSize.iHeight, highestHeight);
    }

    // If there are checkbox graphics then check if they would require more
    // height.
    if (iStyle & KSwtStyleMulti || iStyle & KSwtStyleCheck)
    {
        highestHeight = Max(iCheckBoxSize.iHeight, highestHeight);
    }

    // If it looks like the items are higher than required by the changed
    // font, images, or checkboxes, then we have loop them through to find
    // the one that has the highest font.
    TInt oldItemHeight(iTableListBox->ItemHeight());
    if (highestHeight + marginTop + marginBottom < oldItemHeight)
    {
        for (TInt item = 0; item < itemCount; ++item)
        {
            for (TInt column = 0; column < columnCount; ++column)
            {
                const MSwtFont* swtFont = (static_cast<CSwtImageTextItem*>(
                                               iImageTextItems[ item ]))->GetFont(column);
                if (swtFont)
                {
                    TInt fontHeight = swtFont->Font().FontMaxHeight();
                    if (fontHeight > highestHeight)
                    {
                        highestHeight = fontHeight;
                    }
                }
            }
        }
    }
    // Now we know the highest item checkbox/image/font height.
    // Just add margins and set it.
    TInt newItemHeight(highestHeight + marginTop + marginBottom);

    MSwtUiUtils& utils = iDisplay.UiUtils();
    TMargins8 padding = iDisplay.UiUtils().InlineReferencePadding();
    TInt height = utils.InlineReferenceFontHeight();
    height += padding.iTop + padding.iBottom;
    if (newItemHeight < height) newItemHeight = height;

    if (oldItemHeight != newItemHeight)
    {
        TRAP_IGNORE(
            iTableListBox->SetItemHeightL(newItemHeight);
            iTableListBox->HandleDataSizeChangeL();
        );

        // Make sure the focused item is still in the view.
        if (currentItemWasVisible)
        {
            TInt focusIndex = GetFocusIndex();
            if (focusIndex >= 0)
            {
                iTableListBox->ScrollToMakeItemVisible(focusIndex);
            }
        }

        // Send column resize events
        TInt colCount(GetTableColumnCount());
        TRAP_IGNORE
        (
            for (TInt i = 0; i < colCount; ++i)
    {
        PostColumnResizeEventL(iTableColumns[ i ]->JavaPeer());
        }
        );
        Redraw();
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetGridLineWidth() const
{
    return iGridLineWidth;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetHeaderHeight() const
{
    if (!iHeaderVisible)
    {
        return 0;
    }
    return iHeader->GetPreferredSize().iHeight;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::ShowTableColumn(MSwtTableColumn& aTableColumn)
{
    // We have checked in java side that the TableColumn is not disposed,
    iTableListBox->TableView()->ScrollToMakeColumnVisible(IndexOfTableColumn(&aTableColumn));
    TRAP_IGNORE(iTableListBox->UpdateScrollBarsL());
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::ShowTableItem(MSwtTableItem& aTableItem)
{
    TInt index = IndexOfTableItem(&aTableItem);
    if (index != KErrNotFound)
    {
        iTableListBox->ScrollToMakeItemVisible(index);
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetItemHeight() const
{
    return iTableListBox->ItemHeight();
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTable::GetItemFromPoint(TPoint &aPoint) const
{
    TInt index = 0;
    TBool found = iTableListBox->View()->XYPosToItemIndex(aPoint, index);
    if (found && index >= 0 && index < iTableItems.Count())
    {
        return iTableItems[ index ]->JavaPeer();
    }
    else
    {
        return NULL;
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const TRgb& CSwtTable::GetDefaultForeground() const
{
    return iTableDefaultForeground;
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
const TDesC& CSwtTable::GetColumnText(TInt aColumnIndex) const
{
    return iHeader->GetText(aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTable
// ---------------------------------------------------------------------------
//
void CSwtTable::SetColumnImage(TInt aColumnIndex, const MSwtImage* aImage)
{
    if (aImage)
    {
        SetTableImageSizeIfNotDefined(aImage->GetBounds().Size(),
                                      EFalse);
    }
    iHeader->SetImageByIndex(aColumnIndex, aImage);
    LayoutTable();
    Redraw();
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtTable::CoeControl()
{
    return *this;
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtTable::CoeControl() const
{
    return *this;
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtTable::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    // Don't want CSwtComposite's focusability behavior
    return ASwtScrollableBase::IsFocusable(aReason);
}

// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TInt CSwtTable::FocusBackgroundPolicy() const
{
    return ASwtControlBase::FocusBackgroundPolicy();
};

// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtTable::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    TInt borderWidth = GetBorderWidth();
    if (aHHint != KSwtDefault)
    {
        TInt minimumHeight = borderWidth + GetHeaderHeight() + SbOuterRect(TRect()).Height();
        if (aHHint <  minimumHeight)
        {
            aHHint = minimumHeight;
        }
    }
    if (aWHint != KSwtDefault)
    {
        TInt minimumWidth = borderWidth + SbOuterRect(TRect()).Width();
        if (aWHint <  minimumWidth)
        {
            aWHint = minimumWidth;
        }
    }

    TSize preferredSize(aWHint, aHHint);
    preferredSize -= TSize(borderWidth * 2, borderWidth * 2);
    preferredSize = SbInnerRect(TRect(TPoint(), preferredSize)).Size();
    if (aHHint == KSwtDefault)
    {
        preferredSize.iHeight = GetHeaderHeight();
        TInt itemCount = GetItemCount();
        if (itemCount < 1)
        {
            itemCount = 1;
        }
        preferredSize.iHeight += iTableListBox->ItemHeight() * itemCount;
    }
    if (aWHint == KSwtDefault)
    {
        preferredSize.iWidth = iTableListBox->TableView()->DataWidth();
    }
    preferredSize = SbOuterRect(TRect(TPoint(), preferredSize)).Size();
    preferredSize = BorderOuterRect(TRect(TPoint(), preferredSize)).Size();
    return preferredSize;
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TInt CSwtTable::GetBorderWidth() const
{
    return ASwtControlBase::GetBorderWidth();
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRect CSwtTable::ClientRect() const
{
    return SbInnerRect(BorderInnerRect());
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtTable::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aType == EEventKeyDown)
    {
        // After panning focus highlight was disabled, so enabling again
        EnableFocusHighlight(ETrue);
    }
#endif //RD_JAVA_S60_RELEASE_9_2
    iTableListBox->ProcessKeyEventL(aKeyEvent, aType);
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRgb CSwtTable::GetBackground() const
{
    if (iTableBackgroundSet)
    {
        return iTableBackground;
    }
    else
    {
        // Multi color backgrounds are used. The color returned
        // here is not actually used.
        return ASwtControlBase::GetBackground();
    }
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRgb CSwtTable::GetForeground() const
{
    if (iTableForegroundSet)
    {
        return iTableForeground;
    }
    else
    {
        return iTableDefaultForeground;
    }
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtTable::SetBackgroundL(const MSwtColor* aColor)
{
    if (aColor)
    {
        iTableBackgroundSet = ETrue;
        iTableBackground = aColor->RgbValue();
    }
    else
    {
        iTableBackgroundSet = EFalse;
    }
    Redraw();
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtTable::SetForegroundL(const MSwtColor* aColor)
{
    if (aColor)
    {
        iTableForegroundSet = ETrue;
        iTableForeground = aColor->RgbValue();
    }
    else
    {
        iTableForegroundSet = EFalse;
    }
    iHeader->SetForegroundL(aColor);
    Redraw();
}


// ---------------------------------------------------------------------------
// From MSwtControl
// Sets the font for the table. This font is used for everything unless
// it is overridden by setting a specific font to an item or cell of the
// table. Null sets the default font.
// ---------------------------------------------------------------------------
//
void CSwtTable::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);

    if (aFont)
    {
        iHeader->SetFont(&(aFont->Font()));
    }
    else
    {
        iHeader->SetFont(&DefaultFont().Font());
    }

    TInt fontHeight(0);
    if (aFont)
    {
        fontHeight = aFont->Font().FontMaxHeight();
    }
    else
    {
        fontHeight = DefaultFont().Font().FontMaxHeight();
    }
    UpdateItemHeight(fontHeight);

    // In case the header height changed
    if (GetHeaderVisible())
    {
        LayoutTable();
    }

    if (ListMode())
    {
        PackColumn(0);
    }

    Redraw();
}

// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtTable::IsKeyUsed(TUint aKeyCode) const
{
    // Table doesn't use the Backspace but uses the OK key
    if (aKeyCode == EKeyBackspace)
    {
        return EFalse;
    }
    else if (aKeyCode == EKeyOK)
    {
        if (iStyle & KSwtStyleSingle && !(iStyle & KSwtStyleMulti || iStyle & KSwtStyleCheck))
        {
            MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
            if (commandArranger)  // From MobileExtensions
            {
                if (commandArranger->IsContextSensitiveOperationSet())
                {
                    return EFalse;
                }
            }
            return ETrue;
        }
        else
        {
            return ETrue;
        }
    }
    return ETrue;
}


TBool CSwtTable::MSKSelCmdEnabled() const
{
    return ETrue;
}


// ---------------------------------------------------------------------------
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtTable::UpdateDoNotDrawFlag()
{
    ASwtControlBase::UpdateDoNotDrawFlag();

    // Scrollbar/MSK update might have been skipped during the time the draw
    // flag has been set so it has to be done here.
    TRAP_IGNORE
    (
        iTableListBox->UpdateScrollBarsL();
        UpdateMSKLabelL();
    );
}

#ifdef RD_JAVA_S60_RELEASE_9_2
// ---------------------------------------------------------------------------
// CSwtTable::EnableFocusHighlight
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtTable::EnableFocusHighlight(TBool aEnable)
{
    ASSERT(iTableListBox);
    ASSERT(iTableListBox->View());

    CListItemDrawer* itemDrawer = iTableListBox->View()->ItemDrawer();
    if (itemDrawer)
    {
        TInt disabledHighlight =
            itemDrawer->Flags() & CListItemDrawer::EDisableHighlight;

        if (aEnable && disabledHighlight)
        {
            itemDrawer->ClearFlags(CListItemDrawer::EDisableHighlight);
        }
        else if (!aEnable && !disabledHighlight)
        {
            itemDrawer->SetFlags(CListItemDrawer::EDisableHighlight);
        }
    }
}
#endif //RD_JAVA_S60_RELEASE_9_2


// ---------------------------------------------------------------------------
// From ASwtScrollableBase
// Scrollbar frame is created and owned by the listbox component.
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtTable::SbFrame() const
{
    return iTableListBox->ScrollBarFrame();
}


// ---------------------------------------------------------------------------
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
TRect CSwtTable::SbInnerRect(const TRect& aRect) const
{
    return ASwtScrollableBase::SbInnerRect(aRect);
}


// ---------------------------------------------------------------------------
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
TRect CSwtTable::SbOuterRect(const TRect& aRect) const
{
    return ASwtScrollableBase::SbOuterRect(aRect);
}


// ---------------------------------------------------------------------------
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
void CSwtTable::ComputeTrim(TRect& aRect) const
{
    aRect = SbOuterRect(aRect);
    aRect.iTl.iY -= GetHeaderHeight();
    TInt borderWidth = GetBorderWidth();
    aRect.Grow(borderWidth, borderWidth);
}


// ---------------------------------------------------------------------------
// From ASwtScrollableBase (MEikScrollBarObserver)
// Listbox needs to react to scrolling events. Listbox's view will always
// inform also the column header component of changes in the horizontal
// scrolling offset. Base class implementation takes care of the scrollbar
// selection events.
// ---------------------------------------------------------------------------
//
void CSwtTable::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    switch (aEventType)
    {
    case EEikScrollThumbDragHoriz:
    case EEikScrollThumbDragVert:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEikScrollThumbReleaseHoriz:
    case EEikScrollThumbReleaseVert:
        GetShell().SetUrgentPaintControl(NULL);
        break;
    default:
        break;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    iTableListBox->HandleScrollEventL(aScrollBar, aEventType);
    ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
}


// ---------------------------------------------------------------------------
// From CSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtTable::RegisterChildL(MSwtControl& aChild)
{
    TBool oldFocusability(IsNonFocusing());

    CSwtComposite::RegisterChildL(aChild);

    // If it was focusable but is not anymore after adding the child
    if (IsNonFocusing() != oldFocusability && !oldFocusability)
    {
        // Table is different from normal Composite in that it remains
        // focusable even when it has children. Note that it briefly loses
        // focus when children are being added and then takes it back here.
        // Thus, the focus may move to the newly added child. This is the
        // desired behavior.
        SetFocusing(ETrue);
        FocusabilityChanged();
    }
}

// ---------------------------------------------------------------------------
// From MCoeControlObserver
// ---------------------------------------------------------------------------
//
void CSwtTable::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent aEventType)
{
    // This handles MSK label updates when any input device was used
    // to change the state.
    if (aEventType == EEventStateChanged)
    {
        UpdateMSKLabelL();
    }
}

// ---------------------------------------------------------------------------
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtTable::DefaultFont() const
{
    return iTableListBox->GetDefaultFont();
}


// ---------------------------------------------------------------------------
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
HBufC* CSwtTable::MSKLabelL() const
{
    if (!(GetItemCount() > 0))
    {
        return ASwtControlBase::MSKLabelL();
    }

    if (!(iStyle & KSwtStyleMulti || iStyle & KSwtStyleCheck))
    {
        HBufC* label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
        return label;
    }

    TBool marked = EFalse;
    if (iStyle & KSwtStyleMulti)
    {
        marked = IsSelected(iTableListBox->View()->CurrentItemIndex());
    }
    else // KSwtStyleCheck
    {
        marked = IsChecked(iTableListBox->View()->CurrentItemIndex());
    }

    if (iStyle & KSwtStyleMulti || iStyle & KSwtStyleCheck)
    {
        HBufC* label = NULL;
        if (marked)
        {
            label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_UNMARK);
        }
        else
        {
            label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_MARK);
        }
        return label;
    }
    return ASwtControlBase::MSKLabelL();
}


// ---------------------------------------------------------------------------
// The constructor.
// ---------------------------------------------------------------------------
//
CSwtTable::CSwtTable(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle)
        : CSwtComposite(aDisplay, aPeer, &aParent, aStyle,
                        aParent.Control()->CoeControl().IsVisible(),
                        aParent.Control()->CoeControl().IsDimmed()),
        iHeaderVisible(EFalse),
        iLinesVisible(EFalse)
{
}


// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CSwtTable::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();

    SetContainerWindowL(coeParent);

    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    SetComponentsToInheritVisibility(ETrue);

    LoadLayouts();
    LoadGraphicsL();

    // Create component controls
    iTableListBox = CSwtTableListBox::NewL(iDisplay, *this);
    iTableListBox->SetObserver(this);
    iHeader = CSwtTableHeader::NewL(*this);

    iHeader->SetFont(&DefaultFont().Font());

    const MSwtDevice& device = iDisplay.Device();

    // Create a dummy column
    User::LeaveIfError(iColumnWidths.Insert(0, 0));
    User::LeaveIfError(iColumnAlignments.Insert(CGraphicsContext::ELeft, 0));
    User::LeaveIfError(iColumnImagesCounts.Insert(0, 0));
    iHeader->InsertNewColumnL(0);

    UpdateItemHeight(GetFont()->Font().FontMaxHeight());

    iOldSelectionArray = new(ELeave) CArrayFixFlat<TInt>(2);

    ActivateL();
}


TInt CSwtTable::GetFocusIndex() const
{
    if (iTableListBox->Model()->NumberOfItems() == 0)
    {
        return -1;
    }
    return iTableListBox->CurrentItemIndex();
}


void CSwtTable::LayoutTable()
{
    const TRect  tableRect(BorderInnerRect());
    const TPoint tableLocation(tableRect.iTl);

    TSize headerSize(tableRect.Width(), GetHeaderHeight());
    iHeader->SetRect(GetHeaderRect());

    iTableListBox->SetPosition(
        TPoint(tableLocation.iX, tableLocation.iY + headerSize.iHeight));
    iTableListBox->SetSize(
        TSize(tableRect.Width(), tableRect.Height() - headerSize.iHeight));

    if (ListMode())
    {
        PackColumn(0);
    }
}


// ---------------------------------------------------------------------------
// Leaving version of HandleResourceChange.
// ---------------------------------------------------------------------------
//
void CSwtTable::HandleResourceChangeL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange)
    {
        LoadLayouts();
        LoadGraphicsL();
        if (iHeader)
        {
            // Column locations might have changed because the checkbox graphic
            // size might have changed. There might be a need to make the header
            // of the leading column at index 0 wider/narrower.
            iHeader->UpdateColumnWidth(0, GetColumnWidth(0));
        }
    }
    CCoeControl::HandleResourceChange(aType);
}


// ---------------------------------------------------------------------------
// Loading of the scaled layout data.
// ---------------------------------------------------------------------------
//
void CSwtTable::LoadLayouts()
{
    // The margins for the current item highlight frame graphic
    TRect dummyRect(0, 0, 100, 100);   // values here are insignificant

    TAknLayoutRect topLeft = CSwtLafFacade::GetLayoutRect(
                                 CSwtLafFacade::EListHighlightSkinPlacingGeneralLine2, dummyRect);

    TAknLayoutRect bottomRight = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EListHighlightSkinPlacingGeneralLine5, dummyRect);

    TRect outerRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
    TRect innerRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);

    iHighlightTopBottomMargin = innerRect.iTl.iX - outerRect.iTl.iX;
    iHighlightLeftRightMargin = outerRect.iBr.iY - innerRect.iBr.iY;

    // The frame area is very wide so we draw vertically 50% over
    // the frame to make the table more compact.
    iHighlightTopBottomMargin /= 2;

    // The check box size
    iCheckBoxSize = CSwtLafFacade::GetLayoutRect(
                        CSwtLafFacade::EListSingleGraphicPaneG1, dummyRect, 0).Rect().Size();

    // Grid line width
    iGridLineWidth = CSwtLafFacade::GetLayoutRect(
                         CSwtLafFacade::EEswtControlPaneG1, dummyRect, 0).Rect().Height();
}


// ---------------------------------------------------------------------------
// Loading of the scaled & skinned graphic data.
// ---------------------------------------------------------------------------
//
void CSwtTable::LoadGraphicsL()
{
    // The checkbox graphics
    delete iCheckBoxOnBitmap;
    delete iCheckBoxOnMask;
    delete iCheckBoxOffBitmap;
    delete iCheckBoxOffMask;
    iCheckBoxOnBitmap = NULL;
    iCheckBoxOnMask = NULL;
    iCheckBoxOffBitmap = NULL;
    iCheckBoxOffMask = NULL;

    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropCheckboxOn, bmp, mask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_prop_checkbox_on,
                            EMbmAvkonQgn_prop_checkbox_on_mask);
    AknIconUtils::SetSize(bmp,
                          iCheckBoxSize,
                          EAspectRatioPreservedAndUnusedSpaceRemoved);
    AknIconUtils::SetSize(mask,
                          iCheckBoxSize,
                          EAspectRatioPreservedAndUnusedSpaceRemoved);
    CleanupStack::Pop(2);   // bmp, mask
    iCheckBoxOnBitmap = bmp;
    iCheckBoxOnMask = mask;

    bmp = NULL;
    mask = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropCheckboxOff, bmp, mask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_prop_checkbox_off,
                            EMbmAvkonQgn_prop_checkbox_off_mask);
    AknIconUtils::SetSize(bmp,
                          iCheckBoxSize,
                          EAspectRatioPreservedAndUnusedSpaceRemoved);
    AknIconUtils::SetSize(mask,
                          iCheckBoxSize,
                          EAspectRatioPreservedAndUnusedSpaceRemoved);
    CleanupStack::Pop(2);   // bmp, mask

    iCheckBoxOffBitmap = bmp;
    iCheckBoxOffMask = mask;

    // The colors
    TRgb skinTextColor(KRgbBlack);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              skinTextColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG9);
    iTableDefaultForeground = skinTextColor;
    TRgb highlightTextColor(KRgbBlack);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              highlightTextColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG10);
    iTableDefaultHighlightText = highlightTextColor;
}


// ---------------------------------------------------------------------------
// Updates the selection indexes of items starting from the given index.
// Selected item indexes after the given index are decremented or incremented
// depending on if items have been added or removed.
// ---------------------------------------------------------------------------
//
void CSwtTable::UpdateSelectionIndexesL(const TInt& aStart,
                                        const TInt& aEnd,
                                        CArrayFix<TInt>* aSelectionArray,
                                        const TBool& aItemsAdded)
{
    TInt numberOfItemsInRange = aEnd - aStart + 1;
    if (!(numberOfItemsInRange > 0) || !aSelectionArray || aSelectionArray->Count() == 0)
    {
        return;
    }

    // Update the selection indexes
    TInt selectionCount = aSelectionArray->Count();
    for (TInt iOld = selectionCount - 1; iOld >= 0; --iOld)
    {
        // First delete the selection indexes for items that have been removed
        if (aItemsAdded == EFalse &&
                ((*aSelectionArray)[iOld] >= aStart && (*aSelectionArray)[iOld] <= aEnd))
        {
            aSelectionArray->Delete(iOld);
            continue;
        }
        // Then update the affected selection indexes
        if ((*aSelectionArray)[iOld] >= aStart)
        {
            if (aItemsAdded)
            {
                ((*aSelectionArray)[iOld]) += numberOfItemsInRange;
            }
            else // items were removed
            {
                ((*aSelectionArray)[iOld]) -= numberOfItemsInRange;
            }
        }
    }
    // New updated selections to the control
    iTableListBox->TableView()->SetSelectionIndexesL(aSelectionArray);
    aSelectionArray->Reset();
}


void CSwtTable::UpdateMSKLabelL()
{
    if (AknLayoutUtils::MSKEnabled() && !HasDoNotDrawFlag())
    {
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        if (commandArranger)
        {
            commandArranger->UpdateMSKLabelL();
        }
    }
}
