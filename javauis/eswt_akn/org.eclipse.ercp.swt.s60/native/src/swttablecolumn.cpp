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


#include "swttablecolumn.h"
#include "swttable.h"


// ======== MEMBER FUNCTIONS ========


CSwtTableColumn* CSwtTableColumn::NewL(TSwtPeer aPeer,
                                       MSwtTable& aParent,
                                       TInt aStyle,
                                       TInt aIndex)
{
    CSwtTableColumn* self = new(ELeave) CSwtTableColumn(aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL(aIndex);
    CleanupStack::Pop(self);
    return self;
}


CSwtTableColumn::~CSwtTableColumn()
{
#ifdef _lint
    //lint -esym(1540, CSwtTableColumn::iPeer)
    //lint -esym(1740, CSwtTableColumn::iPeer)
#endif
}


TSwtPeer CSwtTableColumn::JavaPeer() const
{
    return iPeer;
} //lint !e1763


// ---------------------------------------------------------------------------
// From class MCoeControlObserver.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent aEventType)
{
    if (aEventType == MCoeControlObserver::EEventStateChanged)
    {
        CSwtTable& table = static_cast<CSwtTable&>(iParent);
        table.PostColumnSelectionEventL(JavaPeer());
    }
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtTableColumn::Dispose()
{
    iParent.RemoveTableColumn(*this);
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
TInt CSwtTableColumn::Alignment() const
{
    return iStyle & KSwtAlignmentMask;
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::Pack()
{
    TInt index = iParent.IndexOfTableColumn(this);
    iParent.PackColumn(index);
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::SetAlignment(TInt aAlignment)
{
    iStyle &= ~KSwtAlignmentMask;

    TInt columnindex = iParent.IndexOfTableColumn(this);
    if (columnindex == 0)
    {
        // No alignment on the first column, default to lead
        iStyle |= KSwtStyleLead;
        return;
    }

    CGraphicsContext::TTextAlign textAlign;
    switch (aAlignment)
    {
    default:
        aAlignment = KSwtStyleLead;
        //lint -fallthrough
        // Intentional fallthrough
    case KSwtStyleLead:
        textAlign = CGraphicsContext::ELeft;
        break;

    case KSwtStyleTrail:
        textAlign = CGraphicsContext::ERight;
        break;

    case KSwtStyleCenter:
        textAlign = CGraphicsContext::ECenter;
        break;
    }

    iStyle |= aAlignment;

    iParent.SetColumnAlignment(columnindex, textAlign);
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::SetWidth(TInt aWidth)
{
    TInt columnindex = iParent.IndexOfTableColumn(this);
    iParent.SetColumnWidth(columnindex, aWidth);
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::SetTextL(const TDesC& aText)
{
    iParent.SetColumnTextL(*this, aText);
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
const TDesC* CSwtTableColumn::GetText() const
{
    TInt index = iParent.IndexOfTableColumn(this);
    return &(iParent.GetColumnText(index));
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::SetColumnImage(const MSwtImage* aImage)
{
    TInt columnindex = iParent.IndexOfTableColumn(this);
    iParent.SetColumnImage(columnindex, aImage);
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
TInt CSwtTableColumn::Width() const
{
    TInt columnindex = iParent.IndexOfTableColumn(this);
    return iParent.GetColumnWidth(columnindex);
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
TBool CSwtTableColumn::GetResizable() const
{
    return iResizable;
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
void CSwtTableColumn::SetResizable(TBool aResizable)
{
    iResizable = aResizable;
}


// ---------------------------------------------------------------------------
// From class MSwtTableColumn.
// ---------------------------------------------------------------------------
//
TInt CSwtTableColumn::GetStyle() const
{
    return iStyle;
}


CSwtTableColumn::CSwtTableColumn(TSwtPeer aPeer, MSwtTable& aParent, TInt aStyle)
        : iPeer(aPeer),
        iParent(aParent),
        iStyle(aStyle)
{
}


void CSwtTableColumn::ConstructL(TInt aIndex)
{
    iParent.AddTableColumnL(*this, aIndex);
    SetAlignment(iStyle & KSwtAlignmentMask);
}
