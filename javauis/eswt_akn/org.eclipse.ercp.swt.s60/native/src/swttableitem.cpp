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


#include <gdi.h>
#include "eswtwidgetscore.h"
#include "swttableitem.h"
#include "swtimagetextitem.h"
#include "swttable.h"


// ======== MEMBER FUNCTIONS ========


CSwtTableItem* CSwtTableItem::NewL(MSwtDisplay& aDisplay,
                                   TSwtPeer aPeer,
                                   MSwtTable& aParent,
                                   TInt aRowIndex)
{
    CSwtTableItem* self = new(ELeave) CSwtTableItem(aDisplay, aPeer, aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aRowIndex);
    CleanupStack::Pop(self);
    return self;
}


CSwtTableItem::~CSwtTableItem()
{
    delete iItem;

#ifdef _lint
    //lint -esym(1540, CSwtTableItem::iPeer)
    //lint -esym(1740, CSwtTableItem::iPeer)
    iItem = NULL;
#endif
}


TSwtPeer CSwtTableItem::JavaPeer() const
{
    return iPeer;
} //lint !e1763


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTableItem::Dispose()
{
    for (TInt columnIndex=0; columnIndex<iParent.GetColumnCount(); columnIndex++)
    {
        if (iItem->IsImageSet(columnIndex))
        {
            iParent.NotifyImageRemoved(columnIndex);
        }

    }
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
const MSwtColor* CSwtTableItem::GetBackground(TInt aColumnIndex) const
{
    return iItem->GetBackground(aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TRect CSwtTableItem::GetBounds(TInt aColumnIndex) const
{
    TRect tableItemRect(iParent.GetItemRect(iParent.IndexOfTableItem(this)));
    TRect rect(iItem->GetBounds(tableItemRect, aColumnIndex));

    // Some adjustment to better align with what SWT is returning
    if (AknLayoutUtils::LayoutMirrored())
    {
        if (rect.Width() > 0)
        {
            ++rect.iTl.iX;
        }
    }
    else
    {
        if (rect.Width() > 0)
        {
            --rect.iBr.iX;
        }
    }
    if (rect.Height() > 0)
    {
        ++rect.iTl.iY;
    }

    return rect;
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TBool CSwtTableItem::GetChecked() const
{
    return iItem->IsChecked();
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
const MSwtFont* CSwtTableItem::GetFont(TInt aColumnIndex) const
{
    return iItem->GetFont(aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
const MSwtColor* CSwtTableItem::GetForeground(TInt aColumnIndex) const
{
    return iItem->GetForeground(aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TBool CSwtTableItem::GetGrayed() const
{
    if (!(iParent.Composite()->Control()->Style() & KSwtStyleCheck))
        return EFalse;
    return iItem->GetGrayed();
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TRect CSwtTableItem::GetImageBounds(TInt aColumnIndex) const
{
    TRect tableItemRect(iParent.GetItemRect(iParent.IndexOfTableItem(this)));
    return  iItem->GetImageBounds(tableItemRect, aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
CSwtImageTextItem* CSwtTableItem::GetImageTextItem() const
{
    return iItem;
} //lint !e1763


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TSize CSwtTableItem::GetPreferredSize() const
{
    return iItem->GetPreferredSize();
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
TSize CSwtTableItem::GetPreferredSize(TInt aColumnIndex) const
{
    return iItem->GetPreferredSize(aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
const TDesC* CSwtTableItem::GetText(TInt aColumnIndex) const
{
    const TDesC* text = &(iItem->GetText(aColumnIndex));
    return text;
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::InsertNewColumnL(TInt aColumnIndex)
{
    // Invalidate is done in Table
    iItem->InsertNewColumnL(aColumnIndex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetBackground(TInt aColumnIndex, const MSwtColor* aColor)
{
    iItem->SetBackground(aColumnIndex, aColor);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetBackgroundForAll(const MSwtColor* aColor)
{
    iItem->SetBackgroundForAll(aColor);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetFont(TInt aColumnIndex, const MSwtFont* aFont)
{
    TInt height = iItem->SetFont(aColumnIndex, aFont);
    iParent.UpdateItemHeight(height);
    if (static_cast<CSwtTable&>(iParent).ListMode())
    {
        static_cast<CSwtTable&>(iParent).PackColumn(0);
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetFontForAll(const MSwtFont* aFont)
{
    iItem->SetFontForAll(aFont);
    TInt height = (aFont ? aFont->Font().FontMaxHeight() :
                   iParent.Composite()->Control()->GetFont()->Font().FontMaxHeight());
    iParent.UpdateItemHeight(height);
    if (static_cast<CSwtTable&>(iParent).ListMode())
    {
        static_cast<CSwtTable&>(iParent).PackColumn(0);
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetForeground(TInt aColumnIndex, const MSwtColor* aColor)
{
    iItem->SetForeground(aColumnIndex, aColor);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetForegroundForAll(const MSwtColor* aColor)
{
    iItem->SetForegroundForAll(aColor);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetGrayed(TBool aIsGrayed)
{
    if (iParent.Composite()->Control()->Style() & KSwtStyleCheck)
    {
        iItem->SetGrayed(aIsGrayed);
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetImageArray(const RPointerArray<MSwtImage>& aImages)
{
    TInt columnCount(iParent.GetColumnCount());
    TInt imageCount(aImages.Count() < columnCount ? aImages.Count() : columnCount);
    for (TInt i=0; i < imageCount; ++i)
    {
        if (iItem->IsImageSet(i))
        {
            iParent.NotifyImageRemoved(i);
        }
    }
    for (TInt i = 0; i < imageCount; ++i)
    {
        if (aImages[i] != NULL)
        {
            iParent.SetTableImageSizeIfNotDefined(aImages[i]->GetBounds().Size());
            iParent.NotifyImageAdded(i);
        }
    }
    iItem->SetImageArray(aImages);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetImageByIndex(TInt aColumnIndex, const MSwtImage* aImage)
{
    if (iItem->IsImageSet(aColumnIndex))
    {
        iParent.NotifyImageRemoved(aColumnIndex);
    }

    if (aImage)
    {
        iParent.SetTableImageSizeIfNotDefined(aImage->GetBounds().Size());
        iParent.NotifyImageAdded(aColumnIndex);
    }

    iItem->SetImageByIndex(aColumnIndex, aImage);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetTextArrayL(const CDesCArray* aTextArray)
{
    iItem->SetTextArrayL(aTextArray);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetTextL(TInt aColumnIndex, const TDesC& aText)
{
    iItem->SetTextL(aColumnIndex, aText);
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::SetChecked(TBool aChecked)
{
    CSwtTable& table = static_cast<CSwtTable&>(iParent);
    if (table.Style() & KSwtStyleCheck)
    {
        TInt myIndex = table.IndexOfTableItem(this);
        if (aChecked)
        {
            table.CheckItem(myIndex);
        }
        else
        {
            table.UncheckItem(myIndex);
        }
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTableItem.
// ---------------------------------------------------------------------------
//
void CSwtTableItem::RemoveColumn(TInt aColumnIndex)
{
    iItem->RemoveColumn(aColumnIndex);
}



CSwtTableItem::CSwtTableItem(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtTable& aParent)
        : iDisplay(aDisplay), iPeer(aPeer), iParent(aParent)
{
}


void CSwtTableItem::ConstructL(TInt aRowIndex)
{
    iItem = CSwtImageTextItem::NewL(iDisplay, *(iParent.Composite()->Control()),
                                    iParent.ImageSize(),
                                    iParent.GetColumnCount());
    iItem->SetColumnWidthsArray(iParent.GetColumnWidths());
    iItem->SetColumnAlignmentsArray(iParent.GetColumnAlignments());
    iItem->SetColumnImageCounts(iParent.GetColumnImageCounts());
    iItem->SetGridLineWidth(iParent.GetGridLineWidth());

    iParent.AddTableItemL(*this,aRowIndex);
}
