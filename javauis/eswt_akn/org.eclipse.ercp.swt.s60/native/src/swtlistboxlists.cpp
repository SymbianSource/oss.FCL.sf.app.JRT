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


#include <eikclbd.h>
#include <barsread.h>
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif //RD_UI_TRANSITION_EFFECTS_LIST
#include "swtlistboxlists.h"


// ======== MEMBER FUNCTIONS ========


//
// CSwtListBoxTemplate
//

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::NewL
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
CSwtListBoxTemplate<T, U, V>* CSwtListBoxTemplate<T, U, V>::NewL()
{
    CSwtListBoxTemplate<T, U, V>* list = new(ELeave) CSwtListBoxTemplate<T, U, V>;
    CleanupStack::PushL(list);
    list->ConstructL();
    CleanupStack::Pop();
    return list;
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::~CSwtListBoxTemplate
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
CSwtListBoxTemplate<T, U, V>::~CSwtListBoxTemplate()
{
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::CSwtListBoxTemplate
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
CSwtListBoxTemplate<T, U, V>::CSwtListBoxTemplate()
        :T()
{
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::ConstructL
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::ConstructL()
{
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::SetItemDrawerClippingRect
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::SetItemDrawerClippingRect(const TRect& aRect)
{
    U* customItemDrawer = static_cast<U*>(T::ItemDrawer());

    if (customItemDrawer)
    {
        customItemDrawer->SetCustomClippingRect(aRect);
    }
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::SetMargins
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::SetMargins(TInt aHorizontal, TInt aVertical)
{
    T::SetHorizontalMargin(aHorizontal);
    T::SetVerticalMargin(aVertical);
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::SizeChanged
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::SizeChanged()
{
    T::SizeChanged();
    TRAP_IGNORE(RectChangedL(ETrue));
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::PositionChanged
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::PositionChanged()
{
    T::PositionChanged();
    TRAP_IGNORE(RectChangedL(EFalse));
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::RectChangedL
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::RectChangedL(TBool aDoHandleSizeChanged)
{
    if (iListObserver && aDoHandleSizeChanged)
    {
        iListObserver->HandleSizeChangedL();
    }

    TRect rect(ClientRect(T::Rect(), T::Border(), T::ScrollBarFrame(), EFalse));
    T::SetViewRectFromClientRect(rect);
    CAknDoubleSpanScrollBar* vBar = static_cast<CAknDoubleSpanScrollBar*>(
                                        T::ScrollBarFrame()->VerticalScrollBar());
    if (vBar)
    {
        // This will cause the scrollbar to be relocated in the right place
        vBar->SetFixedLayoutRect(TRect(0, 0, 0, 0));
    }
    T::HandleViewRectSizeChangeL();
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::SetItemHeightL
// From CEikListBox
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::SetItemHeightL(TInt aHeight)
{
    T::iItemHeight = aHeight;
    TRect rect(ClientRect(T::Rect(), T::Border(), T::ScrollBarFrame(), EFalse));
    T::SetViewRectFromClientRect(rect);
    T::View()->SetItemHeight(aHeight);
    CAknDoubleSpanScrollBar* vBar = static_cast<CAknDoubleSpanScrollBar*>(
                                        T::ScrollBarFrame()->VerticalScrollBar());
    if (vBar)
    {
        vBar->SetFixedLayoutRect(TRect(0, 0, 0, 0));
    }
    T::HandleViewRectSizeChangeL();
}


// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::MopSupplyObject
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
TTypeUid::Ptr CSwtListBoxTemplate<T, U, V>::MopSupplyObject(TTypeUid aId)
{
    return CEikTextListBox::MopSupplyObject(aId);
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::ClientRect
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
TRect CSwtListBoxTemplate<T, U, V>::ClientRect(TRect aRect, TGulBorder aBorder,
        CEikScrollBarFrame* aSbFrame, TBool aExcludeScrollBars) const
{
    TRect clientRect(aBorder.InnerRect(aRect));

    if (aSbFrame && !aExcludeScrollBars)
    {
        if (aSbFrame->ScrollBarExists(CEikScrollBar::EHorizontal)
                && aSbFrame->ScrollBarVisibility(CEikScrollBar::EHorizontal)
                != CEikScrollBarFrame::EOff)
        {
            TInt breadth(0);
            if (iListObserver)
            {
                breadth = iListObserver->Utils().ScrollBarBreadth(
                              aSbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal));
            }
            else
            {
                breadth = aSbFrame->ScrollBarBreadth(CEikScrollBar::EHorizontal);
            }
            clientRect.iBr.iY -= breadth;
        }
        if (aSbFrame->ScrollBarExists(CEikScrollBar::EVertical)
                && aSbFrame->ScrollBarVisibility(CEikScrollBar::EVertical)
                != CEikScrollBarFrame::EOff)
        {
            TInt breadth(0);
            if (iListObserver)
            {
                breadth = iListObserver->Utils().ScrollBarBreadth(
                              aSbFrame->VerticalScrollBar());
            }
            else
            {
                breadth = aSbFrame->ScrollBarBreadth(CEikScrollBar::EVertical);
            }

            if (!AknLayoutUtils::LayoutMirrored())
            {
                clientRect.iBr.iX -= breadth;
            }
            else
            {
                clientRect.iTl.iX += breadth;
            }
        }
    }

    return clientRect;
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::SetListObserver
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::SetListObserver(MSwtListObserver* aListObserver)
{
    iListObserver = aListObserver;
}

// ---------------------------------------------------------------------------
// CSwtListBoxTemplate<T, U, V>::CreateItemDrawerL
// ---------------------------------------------------------------------------
//
template<class T, class U, class V>
void CSwtListBoxTemplate<T, U, V>::CreateItemDrawerL()
{
    V* columnData = V::NewL();
    CleanupStack::PushL(columnData);
    T::iItemDrawer = new(ELeave) U(T::Model(),
                                   T::iCoeEnv->NormalFont(), columnData, this);
    CleanupStack::Pop();
}

//
// CSwtListBoxLists
//

// ---------------------------------------------------------------------------
// CSwtListBoxLists::NewListL
// ---------------------------------------------------------------------------
//
CEikTextListBox* CSwtListBoxLists::NewListL(TInt aListType)
{
    ASSERT(IsListType(aListType));
    CEikTextListBox* list = NULL;
    switch (aListType)
    {
    case ESwtLbSingle:
        list = CSwtListBoxSingle::NewL();
        break;
    case ESwtLbSingleGraphic:
        list = CSwtListBoxSingleGraphic::NewL();
        break;
    case ESwtLbSingleLarge:
        list = CSwtListBoxSingleLarge::NewL();
        break;
    case ESwtLbSingleHeading:
        list = CSwtListBoxSingleHeading::NewL();
        break;
    case ESwtLbSingleHeadingGraphic:
        list = CSwtListBoxSingleHeadingGraphic::NewL();
        break;
    case ESwtLbDouble:
        list = CSwtListBoxDouble::NewL();
        break;
    case ESwtLbDoubleGraphic:
        list = CSwtListBoxDoubleGraphic::NewL();
        break;
    case ESwtLbDoubleLarge:
        list = CSwtListBoxDoubleLarge::NewL();
        break;
    default:
        break;
    }
    return list;
}

// ---------------------------------------------------------------------------
// CSwtListBoxLists::SetListObserver
// ---------------------------------------------------------------------------
//
void CSwtListBoxLists::SetListObserver(TInt aListType, CEikTextListBox* aList,
                                       MSwtListObserver* aListObserver)
{
    ASSERT(IsListType(aListType));
    switch (aListType)
    {
    case ESwtLbSingle:
        STATIC_CAST(CSwtListBoxSingle*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbSingleGraphic:
        STATIC_CAST(CSwtListBoxSingleGraphic*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbSingleLarge:
        STATIC_CAST(CSwtListBoxSingleLarge*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbSingleHeading:
        STATIC_CAST(CSwtListBoxSingleHeading*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbSingleHeadingGraphic:
        STATIC_CAST(CSwtListBoxSingleHeadingGraphic*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbDouble:
        STATIC_CAST(CSwtListBoxDouble*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbDoubleGraphic:
        STATIC_CAST(CSwtListBoxDoubleGraphic*, aList)->SetListObserver(aListObserver);
        break;
    case ESwtLbDoubleLarge:
        STATIC_CAST(CSwtListBoxDoubleLarge*, aList)->SetListObserver(aListObserver);
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBoxLists::Cells
// ---------------------------------------------------------------------------
//
void CSwtListBoxLists::Cells(
    TInt aListType,
    RArray<TInt>& aCellArray)
{
    ASSERT(IsListType(aListType));
    for (TInt i = 0; (KSwtLbCells[aListType][i] != ECellInvalid)
            && (i < KMaxCellCount); i++)
    {
        aCellArray.Append(KSwtLbCells[aListType][i]);
    }
}

// ---------------------------------------------------------------------------
// CSwtListBoxLists::IsListFormatted
// ---------------------------------------------------------------------------
//
TBool CSwtListBoxLists::IsListFormatted(TInt aListType)
{
    ASSERT(IsListType(aListType));
    if (aListType > ESwtLbSingleHeadingGraphic)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBoxLists::IsListType
// ---------------------------------------------------------------------------
//
TBool CSwtListBoxLists::IsListType(TInt aVal)
{
    return aVal >= 0 && aVal < ESwtLbTypeCount;
}


// ---------------------------------------------------------------------------
// CSwtListBoxLists::SetItemDrawerClippingRect
// ---------------------------------------------------------------------------
//
void CSwtListBoxLists::SetItemDrawerClippingRect(TInt aListType,
        CEikTextListBox* aList, const TRect& aRect)
{
    ASSERT(IsListType(aListType));

    switch (aListType)
    {
    case ESwtLbSingle:
        STATIC_CAST(CSwtListBoxSingle*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbSingleGraphic:
        STATIC_CAST(CSwtListBoxSingleGraphic*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbSingleLarge:
        STATIC_CAST(CSwtListBoxSingleLarge*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbSingleHeading:
        STATIC_CAST(CSwtListBoxSingleHeading*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbSingleHeadingGraphic:
        STATIC_CAST(CSwtListBoxSingleHeadingGraphic*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbDouble:
        STATIC_CAST(CSwtListBoxDouble*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbDoubleGraphic:
        STATIC_CAST(CSwtListBoxDoubleGraphic*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    case ESwtLbDoubleLarge:
        STATIC_CAST(CSwtListBoxDoubleLarge*, aList)
        ->SetItemDrawerClippingRect(aRect);
        break;
    default:
        break;
    }
}


// ---------------------------------------------------------------------------
// CSwtListBoxLists::SetMargins
// ---------------------------------------------------------------------------
//
void CSwtListBoxLists::SetMargins(TInt aListType,
                                  CEikTextListBox* aList, TInt aHorizontal, TInt aVertical)
{
    ASSERT(IsListType(aListType));

    switch (aListType)
    {
    case ESwtLbSingle:
        STATIC_CAST(CSwtListBoxSingle*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbSingleGraphic:
        STATIC_CAST(CSwtListBoxSingleGraphic*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbSingleLarge:
        STATIC_CAST(CSwtListBoxSingleLarge*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbSingleHeading:
        STATIC_CAST(CSwtListBoxSingleHeading*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbSingleHeadingGraphic:
        STATIC_CAST(CSwtListBoxSingleHeadingGraphic*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbDouble:
        STATIC_CAST(CSwtListBoxDouble*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbDoubleGraphic:
        STATIC_CAST(CSwtListBoxDoubleGraphic*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    case ESwtLbDoubleLarge:
        STATIC_CAST(CSwtListBoxDoubleLarge*, aList)
        ->SetMargins(aHorizontal, aVertical);
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBoxLists::EnableStretching
// ---------------------------------------------------------------------------
//
void CSwtListBoxLists::EnableStretching(TInt aListType, CEikTextListBox* aList,
                                        TBool aEnabled)
{
    ASSERT(IsListType(aListType));

    switch (aListType)
    {
    case ESwtLbDouble:
        STATIC_CAST(CSwtListBoxDouble*, aList)
        ->EnableStretching(aEnabled);
        break;
    case ESwtLbDoubleGraphic:
        STATIC_CAST(CSwtListBoxDoubleGraphic*, aList)
        ->EnableStretching(aEnabled);
        break;
    case ESwtLbDoubleLarge:
        STATIC_CAST(CSwtListBoxDoubleLarge*, aList)
        ->EnableStretching(aEnabled);
        break;
    default:
        // Other list types are not supported
        break;
    }
}

// The compiler does not automatically instantiate templates defined in other
// files. Because of this, code written will often produce undefined symbol
// errors when compiled with the compiler. You need to tell the compiler which
// template instances you want, by explicitly instantiating them in the file
// where they are defined.
template class CSwtListBoxTemplate <CAknSingleStyleListBox,
CSwtColumnListBoxItemDrawer, CColumnListBoxData>;
template class CSwtListBoxTemplate <CAknSingleGraphicStyleListBox,
CSwtColumnListBoxItemDrawer, CColumnListBoxData>;
template class CSwtListBoxTemplate <CAknSingleLargeStyleListBox,
CSwtColumnListBoxItemDrawer, CColumnListBoxData>;
template class CSwtListBoxTemplate <CAknSingleHeadingStyleListBox,
CSwtSingleHeadingStyleListBoxItemDrawer,CColumnListBoxData>;
template class CSwtListBoxTemplate <CAknSingleGraphicHeadingStyleListBox,
CSwtSingleHeadingStyleListBoxItemDrawer,CColumnListBoxData>;
template class CSwtListBoxTemplate <CAknDoubleStyleListBox,
CSwtFormattedCellListBoxItemDrawer, CFormattedCellListBoxData>;
template class CSwtListBoxTemplate <CAknDoubleGraphicStyleListBox,
CSwtFormattedCellListBoxItemDrawer, CFormattedCellListBoxData>;
template class CSwtListBoxTemplate <CAknDoubleLargeStyleListBox,
CSwtDoubleLargeStyleItemDrawer, CFormattedCellListBoxData>;
