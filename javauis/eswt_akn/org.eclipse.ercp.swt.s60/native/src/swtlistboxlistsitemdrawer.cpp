/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swtlistboxlistsitemdrawer.h"

// ---------------------------------------------------------------------------
// CSwtColumnListBoxItemDrawer::CSwtColumnListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtColumnListBoxItemDrawer::CSwtColumnListBoxItemDrawer(
    MTextListBoxModel* aTextListBoxModel, const CFont* aFont,
    CColumnListBoxData* aListBoxData, CEikListBox* /*aOwner*/)
        : CColumnListBoxItemDrawer(aTextListBoxModel, aFont, aListBoxData)
{
}

// ---------------------------------------------------------------------------
// CSwtColumnListBoxItemDrawer::~CSwtColumnListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtColumnListBoxItemDrawer::~CSwtColumnListBoxItemDrawer()
{
}

// ---------------------------------------------------------------------------
// CSwtSingleHeadingStyleListBoxItemDrawer::
//      CSwtSingleHeadingStyleListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtSingleHeadingStyleListBoxItemDrawer::CSwtSingleHeadingStyleListBoxItemDrawer(
    MTextListBoxModel* aTextListBoxModel, const CFont* aFont,
    CColumnListBoxData* aListBoxData, CEikListBox* /*aOwner*/)
        : CSingleHeadingStyleItemDrawer(aTextListBoxModel, aFont, aListBoxData)
{
}

// ---------------------------------------------------------------------------
// CSwtColumnListBoxItemDrawer::~CSwtColumnListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtSingleHeadingStyleListBoxItemDrawer::~CSwtSingleHeadingStyleListBoxItemDrawer()
{
}

// ---------------------------------------------------------------------------
// CSwtFormattedCellListBoxItemDrawer::CSwtFormattedCellListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtFormattedCellListBoxItemDrawer::CSwtFormattedCellListBoxItemDrawer(
    MTextListBoxModel* aTextListBoxModel, const CFont* aFont,
    CFormattedCellListBoxData* aListBoxData, CEikListBox* /*aOwner*/)
        : CFormattedCellListBoxItemDrawer(aTextListBoxModel, aFont, aListBoxData)
{
}

// ---------------------------------------------------------------------------
// CSwtColumnListBoxItemDrawer::~CSwtColumnListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtFormattedCellListBoxItemDrawer::~CSwtFormattedCellListBoxItemDrawer()
{
}

// ---------------------------------------------------------------------------
// CSwtDoubleLargeStyleItemDrawer::CSwtDoubleLargeStyleItemDrawer
// ---------------------------------------------------------------------------
//
CSwtDoubleLargeStyleItemDrawer::CSwtDoubleLargeStyleItemDrawer(
    MTextListBoxModel* aTextListBoxModel, const CFont* aFont,
    CFormattedCellListBoxData* aListBoxData, CEikListBox* aOwner)
        : CDoubleLargeStyleItemDrawer(aTextListBoxModel, aFont, aListBoxData,
                                      aOwner)
{
}

// ---------------------------------------------------------------------------
// CSwtColumnListBoxItemDrawer::~CSwtColumnListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CSwtDoubleLargeStyleItemDrawer::~CSwtDoubleLargeStyleItemDrawer()
{
}

// ---------------------------------------------------------------------------
// CSwtColumnListBoxItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CSwtColumnListBoxItemDrawer::DrawItemText(TInt aItemIndex,
        const TRect& aItemTextRect,
        TBool aItemIsCurrent,
        TBool aViewIsEmphasized,
        TBool aItemIsSelected) const
{
    BeginClipping(iGc);
    CColumnListBoxItemDrawer::DrawItemText(aItemIndex, aItemTextRect,
                                           aItemIsCurrent, aViewIsEmphasized, aItemIsSelected);
    EndClipping(iGc);
}

// ---------------------------------------------------------------------------
// CSwtSingleHeadingStyleListBoxItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CSwtSingleHeadingStyleListBoxItemDrawer::DrawItemText(TInt aItemIndex,
        const TRect& aItemTextRect,
        TBool aItemIsCurrent,
        TBool aViewIsEmphasized,
        TBool aItemIsSelected) const
{
    BeginClipping(iGc);
    CSingleHeadingStyleItemDrawer::DrawItemText(aItemIndex, aItemTextRect,
            aItemIsCurrent, aViewIsEmphasized, aItemIsSelected);
    EndClipping(iGc);
}

// ---------------------------------------------------------------------------
// CSwtFormattedCellListBoxItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CSwtFormattedCellListBoxItemDrawer::DrawItemText(TInt aItemIndex,
        const TRect& aItemTextRect,
        TBool aItemIsCurrent,
        TBool aViewIsEmphasized,
        TBool aItemIsSelected) const
{
    BeginClipping(iGc);
    CFormattedCellListBoxItemDrawer::DrawItemText(aItemIndex, aItemTextRect,
            aItemIsCurrent, aViewIsEmphasized, aItemIsSelected);
    EndClipping(iGc);
}

// ---------------------------------------------------------------------------
// CSwtDoubleLargeStyleItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CSwtDoubleLargeStyleItemDrawer::DrawItemText(TInt aItemIndex,
        const TRect& aItemTextRect,
        TBool aItemIsCurrent,
        TBool aViewIsEmphasized,
        TBool aItemIsSelected) const
{
    BeginClipping(iGc);
    CDoubleLargeStyleItemDrawer::DrawItemText(aItemIndex, aItemTextRect,
            aItemIsCurrent, aViewIsEmphasized, aItemIsSelected);
    EndClipping(iGc);
}
// ---------------------------------------------------------------------------
// ASwtListBoxItemDrawerBase::SetCustomClippingRect
// ---------------------------------------------------------------------------
//
void ASwtListBoxItemDrawerBase::SetCustomClippingRect(
    const TRect& aRect)
{
    iCustomClippingRect = aRect;
}

// ---------------------------------------------------------------------------
// ASwtListBoxItemDrawerBase::BeginClipping
// ---------------------------------------------------------------------------
//
void ASwtListBoxItemDrawerBase::BeginClipping(CWindowGc* aGc) const
{
    aGc->SetClippingRect(iCustomClippingRect);
}

// ---------------------------------------------------------------------------
// ASwtListBoxItemDrawerBase::EndClipping
// ---------------------------------------------------------------------------
//
void ASwtListBoxItemDrawerBase::EndClipping(CWindowGc* aGc) const
{
    aGc->CancelClippingRect();
}
