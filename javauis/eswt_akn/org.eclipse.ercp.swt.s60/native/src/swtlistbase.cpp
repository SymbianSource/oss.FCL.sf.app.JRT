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


#include <eikenv.h>
#include <aknenv.h>
#include <eikclbd.h>
#include <gulicon.h>
#include <AknUtils.h>
#include <aknappui.h>
#include <avkon.mbg>
#include <aknconsts.h>
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfx.h>
#endif // RD_UI_TRANSITION_EFFECTS_LIST
#include <swtlaffacade.h>
#include "eswtmobileextensions.h"
#include "swtfont.h"
#include "swtlistbase.h"


_LIT(KTextTab, "\t");
const TInt KCheckBoxOffIndex = 1;
const TInt KCheckBoxOnIndex = 0;
const TUint KIconIndexLengthMax = 1;
const TInt KVBorderSides = 2;
const TInt KHBorderSides = 2;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtListBase::CreateListBoxL
// ---------------------------------------------------------------------------
//
CEikTextListBox* CSwtListBase::CreateListBoxL(TInt aStyle)
{
    iListType = (aStyle & KSwtStyleMulti) ? ESwtLbSingleGraphic : ESwtLbSingle;

    CEikTextListBox* listBox = CSwtListBoxLists::NewListL(iListType);
    CSwtListBoxLists::SetListObserver(iListType, listBox, this);


    CleanupStack::PushL(listBox);
    listBox->ConstructL(this, (iStyle & KSwtStyleMulti) ?
                        EAknListBoxStylusMultiselectionList : EAknListBoxSelectionList);

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    // Effects do not work with non window owning scrollbars.
    CWindowGc* gc = listBox->View()->ItemDrawer()->Gc();
    MAknListBoxTfx* transApi = CAknListLoader::TfxApi(gc);
    if (transApi)
    {
        transApi->EnableEffects(EFalse);
    }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    listBox->SetContainerWindowL(*this);
    listBox->SetCurrentItemIndex(0);
    listBox->MakeVisible(ETrue);
    listBox->SetComponentsToInheritVisibility();
    static_cast<CEikColumnListBox*>(listBox)->ItemDrawer()->ColumnData()->SetSkinEnabledL(EFalse);
#ifdef RD_SCALABLE_UI_V2
    listBox->SetListBoxObserver(this);
#endif // RD_SCALABLE_UI_V2
    CleanupStack::Pop(listBox);

    return listBox;
}

// ---------------------------------------------------------------------------
// CSwtListBase::CreateListBoxModelL
// ---------------------------------------------------------------------------
//
void CSwtListBase::CreateListBoxModelL()
{
    iTextItems = new(ELeave) CDesCArrayFlat(3);
}

// ---------------------------------------------------------------------------
// CSwtListBase::CreateScrollBarsL
// ---------------------------------------------------------------------------
//
void CSwtListBase::CreateScrollBarsL(CEikTextListBox* aLb)
{
    ASSERT(aLb);
    aLb->CreateScrollBarFrameL();

#ifdef RD_SCALABLE_UI_V2
    // WARNING!!! The expanded touch area does not move correctly togehter with the scrollbars!
    aLb->ScrollBarFrame()->SetScrollBarFrameFlags(CEikScrollBarFrame::EDisableExpandedTouchArea);
#endif // RD_SCALABLE_UI_V2

    // In eSWT all scrollbars must be non window owing the main reason being that
    // the scrollbars must draw at exact same time with the parent control. This
    // is especially essential in ScrolledComposite. If the scrollbars would be
    // window owing, they would draw too much ahead of the parents creating an
    // ugly visual effect when flicking the ScrolledComposite.
    // The drawback of having non window owing scrollbars is too many paint events
    // which affects on the speed of kinetic scrolling in lists.
    aLb->ScrollBarFrame()->CreateDoubleSpanScrollBarsL(EFalse, EFalse, ETrue, EFalse);
    aLb->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        iStyle & KSwtStyleVScroll ?
        CEikScrollBarFrame::EOn : CEikScrollBarFrame::EOff);
}

// ---------------------------------------------------------------------------
// CSwtListBase::CreateIconArrayL
// ---------------------------------------------------------------------------
//
void CSwtListBase::CreateIconArrayL(CEikTextListBox* aLb)
{
    ASSERT(aLb);

    // Create icon array
    CArrayPtr<CGulIcon>* icons = new(ELeave)CArrayPtrFlat<CGulIcon>(2);
    CleanupStack::PushL(icons);

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Check box on
    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    CGulIcon* checkBox = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropCheckboxOn, bmp, mask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_prop_checkbox_on,
                            EMbmAvkonQgn_prop_checkbox_on_mask);
    checkBox = CGulIcon::NewL(bmp, mask);
    CleanupStack::PushL(checkBox);
    icons->AppendL(checkBox);
    CleanupStack::Pop(3);   // bmp, mask, checkbox

    // Check box off
    bmp = NULL;
    mask = NULL;
    checkBox = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropCheckboxOff, bmp, mask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_prop_checkbox_off,
                            EMbmAvkonQgn_prop_checkbox_off_mask);
    checkBox = CGulIcon::NewL(bmp, mask);
    CleanupStack::PushL(checkBox);
    icons->AppendL(checkBox);
    CleanupStack::Pop(3);   // bmp, mask, checkbox

    CleanupStack::Pop(icons);

    // Set icon array
    CColumnListBoxData* listBoxData = static_cast<CEikColumnListBox*>(aLb)->ItemDrawer()->ColumnData();
    if (listBoxData->IconArray())
    {
        listBoxData->IconArray()->ResetAndDestroy();
        delete listBoxData->IconArray();
    }
    listBoxData->SetIconArray(icons);
}

// ---------------------------------------------------------------------------
// CSwtListBase::CreateItemTextLC
// ---------------------------------------------------------------------------
//
HBufC* CSwtListBase::CreateItemTextLC(
    const TDesC& aPtr,
    TBool aSelected)
{
    // Allocate memory for the resut string.
    TInt textLength = aPtr.Length() + KTextTab().Length();
    if (iStyle & KSwtStyleMulti)
    {
        textLength += KIconIndexLengthMax;
    }
    HBufC* textBuf = HBufC::NewLC(textLength);
    TPtr itemText(textBuf->Des());
    itemText.Zero();
    if (iStyle & KSwtStyleMulti)
    {
        itemText.AppendNum(aSelected?KCheckBoxOnIndex:KCheckBoxOffIndex);
    }
    itemText.Append(KTextTab);
    itemText.Append(aPtr);

    return textBuf;
}

// ---------------------------------------------------------------------------
// CSwtListBase::UpdateSize
// ---------------------------------------------------------------------------
//
void CSwtListBase::UpdateSize()
{
    ASSERT(iList);
    ASSERT(iList->Model());

    iList->SetSize(BorderInnerRect().Size());
}

// ---------------------------------------------------------------------------
// CSwtListBase::UpdateListBoxFontL
// ---------------------------------------------------------------------------
//
void CSwtListBase::UpdateListBoxFontL()
{
    ASSERT(iList);

    CColumnListBoxItemDrawer* itemDrawer = static_cast<CEikColumnListBox*>(iList)->ItemDrawer();
    ASSERT(itemDrawer);
    CColumnListBoxData* listBoxData = itemDrawer->ColumnData();
    ASSERT(listBoxData);

    // If a font has been set, apply it to the Avkon list. Its item height is also modified.
    if (iFont)
    {
        TInt newItemHeight(
            iDefaultItemHeight +
            (iFont->Font().HeightInPixels() - DefaultFont().Font().HeightInPixels()));
        newItemHeight = Max(iDefaultItemHeight, newItemHeight);
        iList->SetItemHeightL(newItemHeight);
        listBoxData->SetColumnFontL(KTextColumnIndex, &iFont->Font());
        listBoxData->SetColumnBaselinePosL(KTextColumnIndex, 0);
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::UpdateListBoxTextColorL
// ---------------------------------------------------------------------------
//
void CSwtListBase::UpdateListBoxTextColorL()
{
    ASSERT(iList);

    // If a text color has been set, apply it to the Avkon list.
    if (iTextColor)
    {
        static_cast<CEikColumnListBox*>(iList)->ItemDrawer()->SetTextColor(
            TRgb(iTextColor->RgbValue()));
    }
    // If no color set, then the default color is retrieved
    // from the current skin and applied to the Avkon list.
    else
    {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb defaultColor;
        TInt error = AknsUtils::GetCachedColor(
                         skin,
                         defaultColor,
                         KAknsIIDQsnTextColors,
                         EAknsCIQsnTextColorsCG6);

        if (!error)
        {
            static_cast<CEikColumnListBox*>(iList)->ItemDrawer()->SetTextColor(defaultColor);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::Scrollable
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtListBase::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtListBase::DeselectItems
// ---------------------------------------------------------------------------
//
void CSwtListBase::DeselectItems(
    const TInt* aIndices,
    TInt aCount)
{
    ASSERT(iList);
    ASSERT(iList->View());

    if (aIndices)
    {
        for (TInt i = 0; i < aCount; i++)
        {
            iList->View()->DeselectItem(aIndices[i]);
        }
    }
    TRAP_IGNORE(UpdateListMskL());
}

// ---------------------------------------------------------------------------
// CSwtListBase::DeselectItem
// ---------------------------------------------------------------------------
//
void CSwtListBase::DeselectItem(const TInt aIndex)
{
    ASSERT(iList);
    ASSERT(iList->Model());
    ASSERT(iList->View());

    if (iList->Model()->NumberOfItems() == 0
            || iList->SelectionIndexes()->Count() == 0)
    {
        return;
    }

    iList->View()->DeselectItem(aIndex);
    TRAP_IGNORE(UpdateListMskL());
}

// ---------------------------------------------------------------------------
// CSwtListBase::DeselectRange
// ---------------------------------------------------------------------------
//
void CSwtListBase::DeselectRange(TInt aStart, TInt aEnd)
{
    ASSERT(iList);
    ASSERT(iList->Model());
    ASSERT(iList->View());

    TInt count = iList->Model()->NumberOfItems();
    if (count == 0
            || aStart >= count
            || iList->SelectionIndexes()->Count() == 0)
    {
        return;
    }

    if (aStart < 0)
    {
        aStart = 0;
    }

    if (aEnd >= count)
    {
        aEnd = count - 1;
    }

    for (TInt i = aStart; i <= aEnd; i++)
    {
        iList->View()->DeselectItem(i);
    }
    TRAP_IGNORE(UpdateListMskL());
}

// ---------------------------------------------------------------------------
// CSwtListBase::DeselectAll
// ---------------------------------------------------------------------------
//
void CSwtListBase::DeselectAll()
{
    ASSERT(iList);
    ASSERT(iList->Model());

    if (iList->Model()->NumberOfItems() == 0
            || iList->SelectionIndexes()->Count() == 0)
    {
        return;
    }

    iList->ClearSelection();
    TRAP_IGNORE(UpdateListMskL());
}

// ---------------------------------------------------------------------------
// CSwtListBase::GetFocusIndex
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::GetFocusIndex() const
{
    ASSERT(iList);
    ASSERT(iList->Model());

    if (iList->Model()->NumberOfItems() == 0)
    {
        return -1;
    }
    return iList->CurrentItemIndex();
}

// ---------------------------------------------------------------------------
// CSwtListBase::GetSelectionCount
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::GetSelectionCount() const
{
    ASSERT(iList);
    return iList->SelectionIndexes()->Count();
}

// ---------------------------------------------------------------------------
// CSwtListBase::GetSelectionIndices
// ---------------------------------------------------------------------------
//
const CArrayFix<TInt>* CSwtListBase::GetSelectionIndices() const
{
    ASSERT(iList);
    return iList->SelectionIndexes();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SelectItemL
// ---------------------------------------------------------------------------
//
void CSwtListBase::SelectItemL(TInt aIndex, TBool aScroll)
{
    ASSERT(iList);
    ASSERT(iList->Model());
    ASSERT(iList->View());

    if (aIndex >= iList->Model()->NumberOfItems()
            || iList->View()->ItemIsSelected(aIndex))
    {
        return;
    }

    if (!(iStyle & KSwtStyleMulti))
    {
        if (aScroll && iList->View()->ViewRect().Height() >= iList->ItemHeight())
        {
            iList->View()->VerticalMoveToItemL(aIndex, CListBoxView::ESingleSelection);
        }
        else
        {
            TInt old = iList->CurrentItemIndex();
            //we do not call CEikListBox::SetCurrentItemIndex, because we do not want to scroll.
            iList->View()->SetCurrentItemIndex(aIndex);
            iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
            if (old != -1)
            {
                iList->View()->DrawItem(old);
            }
        }
    }
    else
    {
        iList->View()->SelectItemL(aIndex);
        if (aScroll)
        {
            iList->ScrollToMakeItemVisible(aIndex);
        }
    }
    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SelectRangeL
// ---------------------------------------------------------------------------
//
void CSwtListBase::SelectRangeL(TInt aStart, TInt aEnd)
{
    ASSERT(iList);
    ASSERT(iList->Model());
    ASSERT(iList->View());

    TInt count = iList->Model()->NumberOfItems();
    if (count == 0 || aStart >= count)
    {
        return;
    }

    TInt end(aEnd);
    if (end >= count)
    {
        end = count - 1;
    }
    TInt start(aStart);
    if (start < 0)
    {
        start = 0;
    }

    for (TInt index = start; index <= end; ++index)
    {
        iList->View()->SelectItemL(index);
    }
    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SelectAllL
// ---------------------------------------------------------------------------
//
void CSwtListBase::SelectAllL()
{
    ASSERT(iList);
    ASSERT(iList->View());

    for (TInt index = 0; index < iList->Model()->NumberOfItems(); ++index)
    {
        iList->View()->SelectItemL(index);
    }
    UpdateListMskL();
}

// ---------------------------------------------------------------------------
// CSwtListBase::ShowSelection
// ---------------------------------------------------------------------------
//
void CSwtListBase::ShowSelection() const
{
    ASSERT(iList);

    if (GetSelectionCount() == 0)
    {
        return;
    }

    iList->ScrollToMakeItemVisible((*iList->SelectionIndexes())[0]);
}

// ---------------------------------------------------------------------------
// CSwtListBase::PostSelectionEvent
// ---------------------------------------------------------------------------
//
void CSwtListBase::PostSelectionEvent() const
{
    TRAP_IGNORE(iDisplay.PostSelectionEventL(iPeer));
}

// ---------------------------------------------------------------------------
// CSwtListBase::CSwtListBase
// ---------------------------------------------------------------------------
//
CSwtListBase::CSwtListBase(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) :
        ASwtScrollableBase(aDisplay, aPeer, &aParent, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtListBase::~CSwtListBase
// ---------------------------------------------------------------------------
//
CSwtListBase::~CSwtListBase()
{
    delete iPrevSelItems;
    delete iList;
    delete iTextItems;
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
    }

    iRemovedItemIndices.Close();
    iDefaultFont = NULL;
    iTextColor = NULL;
    iCurrentLafFont = NULL;
}

// ---------------------------------------------------------------------------
// CSwtListBase::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtListBase::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);
    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    SetComponentsToInheritVisibility(ETrue);

    iList = CreateListBoxL(iStyle);
    CreateScrollBarsL(iList);
    if (iStyle & KSwtStyleMulti)
    {
        CreateIconArrayL(iList);
    }
    iPrevSelItems = new(ELeave) CArrayFixFlat<TInt>(1);
    CreateListBoxModelL();

    iList->SetPosition(TPoint(GetBorderWidth(), GetBorderWidth()));

    // Retrieve the default font; a size must be set before retrieving the default font
    // because the font is set in SizeChanged of Avkon lists.
    SetSize(TSize(0, 0));

    UpdateDefaultFontL();

    // This is indeed necessary, otherwise the background might not be drawn!
    SetBackground(this);

    ActivateL();
}

// ---------------------------------------------------------------------------
// CSwtListBase::UpdateDefaultFontL
// ---------------------------------------------------------------------------
//
void CSwtListBase::UpdateDefaultFontL()
{
    ASSERT(iList);

    CColumnListBoxData* listBoxData =
        static_cast<CEikColumnListBox*>(iList)->ItemDrawer()->ColumnData();
    ASSERT(listBoxData);
    const CFont* font = listBoxData->ColumnFont(KTextColumnIndex);

    if (font && iCurrentLafFont != font)
    {
        if (iDefaultFont)
        {
            iDefaultFont->RemoveRef();
            iDefaultFont = NULL;
        }
        iDefaultFont = CSwtFont::NewL(iDisplay.Device(), *font);

        // Current LAF font retrieved
        iCurrentLafFont = font;

        // Default item height retrieved
        iDefaultItemHeight = iList->ItemHeight();
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::HandleResourceChangeL
// ---------------------------------------------------------------------------
//
void CSwtListBase::HandleResourceChangeL(TInt aType)
{
    if (aType == KAknsMessageSkinChange)
    {
        CreateIconArrayL(iList);
    }

    CAknControl::HandleResourceChange(aType);
}

// ---------------------------------------------------------------------------
// CSwtListBase::GetUserSelectionL
// ---------------------------------------------------------------------------
//
void CSwtListBase::GetUserSelectionL(CArrayFix<TInt>* aCurrSelItems)
{
    if ((iStyle & KSwtStyleMulti) && aCurrSelItems)
    {
        iList->View()->GetSelectionIndexesL(aCurrSelItems);
    }
}

// ---------------------------------------------------------------------------
// CSwtList::CalcNewFocusIndexAndTopItemIndex
// ---------------------------------------------------------------------------
//
void CSwtListBase::CalcFocusIndexAfterItemRemoval(TInt& aNewFocusIndex,
        const RArray<TInt>& aRemovedItemIndices, TInt aOldFocusIndex) const
{
    ASSERT(iList);
    ASSERT(iList->View());
    ASSERT(iList->Model());

    aNewFocusIndex = aOldFocusIndex;

    if (aRemovedItemIndices.Count() > 0)
    {
        TInt oldItemCount(
            iList->Model()->NumberOfItems() + aRemovedItemIndices.Count());

        TInt counter(0);
        TInt ind = 0;
        while (ind < aRemovedItemIndices.Count()
                && aNewFocusIndex >= aRemovedItemIndices[ind])
        {
            counter++;
            if (aNewFocusIndex == aRemovedItemIndices[ind])
            {
                aNewFocusIndex = (aRemovedItemIndices[ind]+1) % oldItemCount;
            }
            ind++;
        }

        aNewFocusIndex -= counter;
        if ((aNewFocusIndex < 0)
                || (aNewFocusIndex >= iList->Model()->NumberOfItems()))
        {
            aNewFocusIndex = 0;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::MinimumListItemWidth
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::MinimumListItemWidth() const
{
    ASSERT(iTextItems);

    // Get general list Avkon layout
    TRect mainRect(TRect::EUninitialized);
    TAknLayoutRect layoutRect;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane,
        mainRect);
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListScrollGenPane, mainRect, 0);
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListGenPane, layoutRect.Rect(), 0);

    TAknLayoutText txtLayoutRect;

    if (iStyle & KSwtStyleMulti)
    {
        layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSingleGraphicPane, layoutRect.Rect(), 0);
        txtLayoutRect = CSwtLafFacade::GetLayoutText(CSwtLafFacade::EListSingleGraphicPaneT1, layoutRect.Rect(), 0);
    }
    else
    {
        layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSinglePane, layoutRect.Rect(), 0);
        txtLayoutRect = CSwtLafFacade::GetLayoutText(CSwtLafFacade::EListSinglePaneT1, layoutRect.Rect(), 0);
    }

    if (iFont)
    {
        CColumnListBoxData* listBoxData =
            static_cast<CEikColumnListBox*>
            (iList)->ItemDrawer()->ColumnData();
        ASSERT(listBoxData);
        return (listBoxData->ColumnWidthPixel(KCheckboxColumnIndex)
                + iTxtMaxWidth);
    }

    TInt diffBrX(Max(0, layoutRect.Rect().iBr.iX - txtLayoutRect.TextRect().iBr.iX));

    return (txtLayoutRect.TextRect().iTl.iX + diffBrX + iTxtMaxWidth);
}

// ---------------------------------------------------------------------------
// CSwtListBase::PreferredWidth
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::PreferredWidth() const
{
    TInt preferredWidth(0);

    // Add the border width
    preferredWidth = GetBorderWidth() * KHBorderSides;

    // Add the vertical scrollbar space if present
    if (SbFrame()->ScrollBarExists(CEikScrollBar::EVertical)
            && SbFrame()->ScrollBarVisibility(CEikScrollBar::EVertical)
            != CEikScrollBarFrame::EOff)
    {
        preferredWidth += iDisplay.UiUtils().ScrollBarBreadth(SbFrame()->VerticalScrollBar());
    }

    preferredWidth += MinimumListItemWidth();

    return preferredWidth;
}

// ---------------------------------------------------------------------------
// CSwtListBase::PreferredWidth
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::PreferredHeight() const
{
    TInt preferredHeight(0);

    // Add the border width
    preferredHeight = GetBorderWidth() * KVBorderSides;

    preferredHeight += iList->CalcHeightBasedOnNumOfItems(
                           Max(1, iList->Model()->NumberOfItems()));

    // Add horizontal scrollbar width
    if (SbFrame()->ScrollBarExists(CEikScrollBar::EHorizontal) &&
            SbFrame()->ScrollBarVisibility(CEikScrollBar::EHorizontal)
            != CEikScrollBarFrame::EOff)
    {
        preferredHeight += iDisplay.UiUtils().ScrollBarBreadth(
                               SbFrame()->GetScrollBarHandle(CEikScrollBar::EHorizontal));
    }

    return preferredHeight;
}

// ---------------------------------------------------------------------------
// CSwtListBase::ComputeTextMaxWidth
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::ComputeTextMaxWidth() const
{
    ASSERT(iTextItems);

    const CFont& font = GetFont()->Font();
    TInt txtMaxWidth(0);
    for (TInt ind = 0; ind < iTextItems->Count(); ind++)
    {
        txtMaxWidth = Max(txtMaxWidth,
                          font.TextWidthInPixels((*iTextItems)[ind]));
    }

    return txtMaxWidth;
}

// ---------------------------------------------------------------------------
// CSwtListBase::ComputeTextMaxWidth
// ---------------------------------------------------------------------------
//
void CSwtListBase::SetTextMaxWidth(TInt aMaxTxtWidth)
{
    iTxtMaxWidth = aMaxTxtWidth;
}

// ---------------------------------------------------------------------------
// CSwtListBase::UpdateListMSK
// ---------------------------------------------------------------------------
//
void CSwtListBase::UpdateListMskL()
{
    if (AknLayoutUtils::MSKEnabled())
    {
        MSwtCommandArranger* cmdArranger = iDisplay.CommandArranger();
        if (cmdArranger)
        {
            cmdArranger->UpdateMSKLabelL();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtListBase::CountComponentControls() const
{
    return iList ? 1 : 0;
}

// ---------------------------------------------------------------------------
// CSwtListBase::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtListBase::ComponentControl(TInt /*aIndex*/) const
{
    return iList;
}

// ---------------------------------------------------------------------------
// CSwtListBase::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::MakeVisible(TBool aVisible)
{
    CCoeControl::MakeVisible(aVisible);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::SetDimmed(TBool aDimmed)
{
    ASSERT(iList);

    CCoeControl::SetDimmed(aDimmed);
    iList->SetDimmed(aDimmed);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBase::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtListBase::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    ASSERT(iList);
    ASSERT(iList->Model());

    TBool traversalDoIt = ETrue;

    if ((iStyle & KSwtStyleMulti)
            && (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK))
    {
        traversalDoIt = EFalse;
    }

    if (aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow)
    {
        traversalDoIt = EFalse;
    }

    if (aKeyEvent.iCode == EKeyDownArrow)
    {
        if (iList->CurrentItemIndex() == (iList->Model()->NumberOfItems()-1))
        {
            if (GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this) != NULL)
            {
                traversalDoIt = ETrue;
            }
        }
    }
    else if (aKeyEvent.iCode == EKeyUpArrow)
    {
        if (iList->CurrentItemIndex() == 0)
        {
            if (GetShell().FindTraversalTargetL(ESwtTraverseArrowPrevious, *this) != NULL)
            {
                traversalDoIt = ETrue;
            }
        }
    }

    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------
// CSwtListBase::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(HandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------
// CSwtListBase::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListBase::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    ASSERT(iList);

    CEikScrollBarFrame* sbFrame = iList->ScrollBarFrame();
    CEikScrollBar* vsb = sbFrame ? sbFrame->GetScrollBarHandle(CEikScrollBar::EVertical) : NULL;

    // Check if we should start scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down && !iVScrollBarGrabsPointerEvents)
    {
        if (vsb && vsb->Rect().Contains(aPointerEvent.iPosition))
        {
            iVScrollBarGrabsPointerEvents = ETrue;
        }
    }

    // Deliver event to scrollbar
    if (iVScrollBarGrabsPointerEvents && vsb)
    {
        vsb->HandlePointerEventL(aPointerEvent);
    }

    // Deliver event to list
    if (!iVScrollBarGrabsPointerEvents)
    {
        // Store the selection on first pointer event.
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            if (iStyle & KSwtStyleMulti)
            {
                // Multi lists
                ASSERT(iPrevSelItems);
                GetUserSelectionL(iPrevSelItems);
            }
            iPrevFocusIndex = iList->View()->CurrentItemIndex();
        }

        iList->HandlePointerEventL(aPointerEvent);

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            TInt focusIndex = iList->View()->CurrentItemIndex();

            if (iStyle & KSwtStyleMulti)
            {
                // Multi lists
                if ((iList->View()->SelectionIndexes()->Count() != iPrevSelItems->Count()))
                {
                    if (iDisplay.RevertPointerEvent())
                    {
                        // Revert newly checked item.
                        if (iList->View()->ItemIsSelected(focusIndex))
                        {
                            iList->View()->DeselectItem(focusIndex);
                        }
                        else
                        {
                            iList->View()->SelectItemL(focusIndex);
                        }
                    }
                    else
                    {
                        iDisplay.PostSelectionEventL(iPeer);
                    }
                }
            }
            else
            {
                // Single lists
                // Compare the focus index after tap or drag ended
                if (focusIndex != iPrevFocusIndex)
                {
                    // Ensure the selection and focus index are in sync.
                    iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);

                    // Item focus highlight moved, send selection event.
                    // Changing the focus index is not subject to reverting.
                    iDisplay.PostSelectionEventL(iPeer);
                }

                // The default selection is sent from HandleListBoxEventL!
            }
        }
    }

    // Stop scrollbar grabbing
    if (iVScrollBarGrabsPointerEvents
            && aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iVScrollBarGrabsPointerEvents = EFalse;
    }
}
#else //RD_SCALABLE_UI_V2
void CSwtListBase::HandlePointerEventL(
    const TPointerEvent& /*aPointerEvent*/)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtListBase::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::SizeChanged()
{
    UpdateSize();
    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBase::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::PositionChanged()
{
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtListBase::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::FocusChanged(TDrawNow aDrawNow)
{
    ASSERT(iList);

    TBool isFocused = IsFocusControl();
    iList->SetFocus(isFocused, aDrawNow);
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtListBase::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtListBase::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);

    if (id.Pointer() == NULL)
    {
        return CAknControl::MopSupplyObject(aId);
    }
    else
    {
        return id;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::Draw
// From CCoeControl
// Overriding Draw just to set correct clipping rect for the item drawer of
// the contained list. The contained list will be drawn immediately after this.
// ---------------------------------------------------------------------------
//
void CSwtListBase::Draw(const TRect& /*aRect*/) const
{
    // Suppress margins - they are drawn over scrollbars
    CSwtListBoxLists::SetMargins(iListType, iList, 0, 0);

    if (GetShell().UrgentPaintControl() == this)
        return;

    TRect clipRect(ClipToVisibleRect(iList->View()->ViewRect()));
    if (clipRect != iLastViewVisibleRect)
    {
        CSwtListBoxLists::SetItemDrawerClippingRect(iListType, iList, clipRect);
        iLastViewVisibleRect = clipRect;
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::SetFontL
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
void CSwtListBase::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);

    SetTextMaxWidth(ComputeTextMaxWidth());

    // The font update will be performed in HandleSizeChangedL method.
    UpdateSize();

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtListBase::MSKLabelL
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
HBufC* CSwtListBase::MSKLabelL() const
{
    if (!iList || !(iList->Model()->NumberOfItems() > 0))
    {
        return ASwtControlBase::MSKLabelL();
    }

    if ((iStyle & KSwtStyleSingle))
    {
        HBufC* label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
        return label;
    }
    if ((iStyle & KSwtStyleMulti))
    {
        HBufC* label = NULL;
        if (iList->View()->ItemIsSelected(iList->CurrentItemIndex()))
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
// CSwtListBase::DefaultFont
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtListBase::DefaultFont() const
{
    if (iDefaultFont)
    {
        return *iDefaultFont;
    }

    return *(iDisplay.Device().GetSystemFont());
}

// ---------------------------------------------------------------------------
// CSwtListBase::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtListBase::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtListBase::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtListBase::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtListBase::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // No items or not a normal key event
    if (iList->Model()->NumberOfItems() == 0 || aType != EEventKey)
    {
        iList->OfferKeyEventL(aKeyEvent, aType);
        return;
    }

    TBool scroll = aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow
                   || aKeyEvent.iCode == EKeyNext || aKeyEvent.iCode == EKeyPrevious;
    TBool submit = aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK;

    // For performance considerations, assuming that Enter or Select always
    // does change the multi selection. If the assumption is wrong, we must
    // store the multi selection before the key is handled and then compare:
    // if ( ( iStyle & KSwtStyleMulti ) && submit ) GetUserSelectionL( iPrevSelItems );

    // Store selection before letting the list handle the key.
    if ((iStyle & KSwtStyleSingle) && scroll)
    {
        iPrevFocusIndex = iList->View()->CurrentItemIndex();
    }

    iList->OfferKeyEventL(aKeyEvent, aType);

    if (scroll || submit)
    {
        if (iStyle & KSwtStyleSingle)
        {
            // Single lists
            if (submit)
            {
                // Item submited, send default selection event.
                iDisplay.PostDefaultSelectionEventL(iPeer);
            }
            else
            {
                if (iList->View()->CurrentItemIndex() != iPrevFocusIndex)
                {
                    // Ensure the selection and focus index are in sync.
                    iList->View()->UpdateSelectionL(CListBoxView::ESingleSelection);

                    // Item focus highlight moved, send selection event.
                    iDisplay.PostSelectionEventL(iPeer);
                }
            }
        }
        else
        {
            // Multi lists
            // Msk could have changed: Mark / Unmark
            UpdateListMskL();

            // For performance considerations, assuming that Enter or Select always
            // does change the multi selection. If the assumption is wrong, here's
            // how we must check if the selection really changed:
            // if ( submit && iPrevSelItems && ( iList->View()->SelectionIndexes()->
            // Count() != iPrevSelItems->Count() ) )
            if (submit)
            {
                // Multi selection changed, send selection event.
                iDisplay.PostSelectionEventL(iPeer);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtListBase::IsKeyUsed
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtListBase::IsKeyUsed(TUint aKeyCode) const
{
    // A list does not use the Backspace but uses OK key
    if (aKeyCode == EKeyBackspace)
    {
        return EFalse;
    }
    else if (aKeyCode == EKeyOK)
    {
        if (iStyle & KSwtStyleSingle)
        {
            MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
            if (commandArranger)
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


TBool CSwtListBase::MSKSelCmdEnabled() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtListBase::GetForeground
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRgb CSwtListBase::GetForeground() const
{
    ASSERT(iList);
    return static_cast<CEikColumnListBox*>(iList)->ItemDrawer()->TextColor();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SetForegroundL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);
    iTextColor = aColor;
    UpdateListBoxTextColorL();
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtListBase::SetBackgroundL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListBase::SetBackgroundL(const MSwtColor* aColor)
{
    ASSERT(iList);
    ASwtControlBase::DoSetBackgroundL(aColor);
    iList->HandleResourceChange(KEikMessageColorSchemeChange);
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtListBase::IsLongTapAnimationCandidate
// From MSwtControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
TBool CSwtListBase::IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const
{
    ASSERT(iList);
    TInt itemIndex = KErrNotFound;
    TBool pointerOverItem = iList->View()->XYPosToItemIndex(aPointerEvent.iPosition, itemIndex);
    return pointerOverItem && ASwtControlBase::IsLongTapAnimationCandidate(aPointerEvent);
}
#else
TBool CSwtListBase::IsLongTapAnimationCandidate(const TPointerEvent& /*aPointerEvent*/) const
{
    return EFalse;
}
#endif

// ---------------------------------------------------------------------------
// CSwtListBase::SbFrame
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtListBase::SbFrame() const
{
    ASSERT(iList);
    return iList->ScrollBarFrame();
}

// ---------------------------------------------------------------------------
// CSwtListBase::HandleSizeChangedL
// From MSwtListObserver
// ---------------------------------------------------------------------------
//
void CSwtListBase::HandleSizeChangedL()
{
    ASSERT(iList);

    CColumnListBoxData* listBoxData =
        static_cast<CEikColumnListBox*>(iList)->ItemDrawer()->ColumnData();
    ASSERT(listBoxData);
    CColumnListBoxItemDrawer* listBoxDrawer =
        static_cast<CEikColumnListBox*>(iList)->ItemDrawer();
    ASSERT(listBoxDrawer);

    // Update default font if font of list box has changed
    UpdateDefaultFontL();

    if (iFont)
    {
        // If a font has been set, all item layouts are removed
        // to display all of them correctly
        listBoxData->ResetSLSubCellArray();

        // WORKAROUND to prevent a crash in CColumnListBoxData::Draw() method.
        // An assert ASSERT_ALWAYS is performed if column widths are not correct.
        TRect clientRect(ClientRect());
        for (TInt colIndex = 0;
                colIndex < listBoxData->LastColumn();
                colIndex++)
        {
            if (colIndex == KCheckboxColumnIndex)
            {
                if (listBoxData->ColumnWidthPixel(KCheckboxColumnIndex) <= 1)
                {
                    listBoxData->SetColumnWidthPixelL(KCheckboxColumnIndex, 2);
                }
            }
            else if (colIndex == KTextColumnIndex)
            {
                TInt txtMaxWidth(0);
                if (!iTextItems->Count())
                {
                    txtMaxWidth = clientRect.Width() -
                                  listBoxData->ColumnWidthPixel(KCheckboxColumnIndex);
                }
                else
                {
                    txtMaxWidth = Min(iTxtMaxWidth, clientRect.Width() -
                                      listBoxData->ColumnWidthPixel(KCheckboxColumnIndex));
                }
                txtMaxWidth = Max(1, txtMaxWidth);
                listBoxData->SetColumnWidthPixelL(
                    KTextColumnIndex, txtMaxWidth);
            }
            else if (listBoxData->ColumnWidthPixel(colIndex) < 0)
            {
                listBoxData->SetColumnWidthPixelL(colIndex, 0);
            }
        }
        // END WORKAROUND
    }

    // ==========================================
    // WORKAROUND to not draw the dotted lines.
    // ==========================================
    //
    listBoxData->SetSeparatorLinePosition(ENoLine);
    // END WORKAROUND

    // Update with current font
    UpdateListBoxFontL();

    // Update with current text color
    UpdateListBoxTextColorL();

    // Update background color
    listBoxDrawer->SetBackColor(GetBackground());
}

// ---------------------------------------------------------------------------
// CSwtListBase::Utils
// From MSwtListObserver
// ---------------------------------------------------------------------------
//
MSwtUiUtils& CSwtListBase::Utils() const
{
    return iDisplay.UiUtils();
}

// ---------------------------------------------------------------------------
// CSwtListBase::HandleScrollEventL
// From MEikScrollBarObserver
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListBase::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    switch (aEventType)
    {
    case EEikScrollThumbDragVert:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEikScrollThumbReleaseVert:
        GetShell().SetUrgentPaintControl(NULL);
        break;
    default:
        break;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    iList->HandleScrollEventL(aScrollBar, aEventType);
    ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
}
#else //RD_SCALABLE_UI_V2
void CSwtListBase::HandleScrollEventL(CEikScrollBar*, TEikScrollEvent)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtListBase::HandleScrollEventL
// From MEikListBoxObserver
// Handles default selection for touch. Unable to handle the selection events
// here since EEventItemClicked is not always sent for every tap.
// The selection events are handled in HandlePointerEvent.
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListBase::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
{
    if (aListBox != iList)
    {
        return;
    }

    switch (aEventType)
    {
        // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventPanningStarted:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEventFlickStopped:
        GetShell().SetUrgentPaintControl(NULL);
        break;
#endif // RD_JAVA_S60_RELEASE_9_2
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventItemSingleClicked:
#else
    case EEventItemDoubleClicked:
#endif // RD_JAVA_S60_RELEASE_9_2
        if (!(iStyle & KSwtStyleMulti))
        {
            if (!iDisplay.RevertPointerEvent())
                iDisplay.PostDefaultSelectionEventL(iPeer);
        }
        break;
    default:
        // Do nothing
        break;
    }
}
#else //RD_SCALABLE_UI_V2
void CSwtListBase::HandleListBoxEventL(CEikListBox*, TListBoxEvent)
{
}
#endif //RD_SCALABLE_UI_V2

