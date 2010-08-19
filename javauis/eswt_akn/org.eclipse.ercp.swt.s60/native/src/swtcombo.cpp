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


#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknLayoutFont.h>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK
#include <swtlaffacade.h>
#include <swtcomboproxy.h>
#include "swtcombo.h"


// NOTE: Due to several issues in operating a CAknChoiceList while open,
// workarounds have been put in place troughout this implementation,
// which consist of closing the combo, before operating on it.


const TInt KMaxVisibleItemCount(6);
_LIT(KTxtSpace, " ");

#ifdef RD_TACTILE_FEEDBACK
const TInt KTouchFeedbackRectId1(0);
#endif // RD_TACTILE_FEEDBACK


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtCombo::NewL
// ---------------------------------------------------------------------------
//
CSwtCombo* CSwtCombo::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite& aParent, TInt aStyle)
{
    CSwtCombo* self = new(ELeave) CSwtCombo(aDisplay, aPeer,
                                            aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtCombo::CSwtCombo
// ---------------------------------------------------------------------------
//
CSwtCombo::CSwtCombo(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                     MSwtComposite& aParent, TInt aStyle)
        : ASwtScrollableBase(aDisplay, aPeer, &aParent, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtCombo::~CSwtCombo
// ---------------------------------------------------------------------------
//
CSwtCombo::~CSwtCombo()
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif // RD_TACTILE_FEEDBACK

    // Choice list, own.
    delete iCombo;
    iCombo = NULL;

    // String item array, not own
    iItems = NULL;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtCombo::ConstructL()
{
    // Handle parent
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);
    CAknControl::MakeVisible(coeParent.IsVisible());
    CAknControl::SetDimmed(coeParent.IsDimmed());

    // Create Avkon choice list and its item array.
    iItems = new(ELeave) CDesCArrayFlat(1);
    iCombo = CSwtComboProxy::NewL(this, iItems);

    // Construct choice list.
    iCombo->SetContainerWindowL(*this);
    iCombo->SetObserver(this);
    iCombo->SetFlags(iCombo->Flags() | CSwtComboProxy::EComboPositionBottom);
    iCombo->SetSelectedIndex(-1);
    iPrevSelIdx = -1;

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    // Disabling feedback for CSwtCombo.
    // When enabled - first tap on combo creates extra feedback,
    // that is not needed.
    if (iFeedback)
    {
        iFeedback->EnableFeedbackForControl(this, EFalse);
    }
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#endif // RD_TACTILE_FEEDBACK

    // Ready to draw
    SetComponentsToInheritVisibility(ETrue);
    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw
    ActivateL();
}

// ---------------------------------------------------------------------------
// CSwtCombo::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::CountComponentControls() const
{
    return 1;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtCombo::ComponentControl(TInt /*aIdx*/) const
{
    return iCombo;
}

// ---------------------------------------------------------------------------
// CSwtCombo::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::PositionChanged()
{
    CenterContainedCombo();
    UpdateTouchFeedbackRect();
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtCombo::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::SizeChanged()
{
    TRect rect(BorderInnerRect());
    iCombo->SetSize(TSize(rect.Size().iWidth, iCombo->MinimumSize().iHeight));
    CenterContainedCombo();
    UpdateTouchFeedbackRect();
    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtCombo::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::FocusChanged(TDrawNow aDrawNow)
{
    // The Combo looses focus to the open popup list due to stack change.
    if (iOpening)
    {
        if (!IsFocused())
        {
            SetFocus(ETrue);
        }
        return;
    }
    TBool isFocused = IsFocusControl();
    iCombo->SetFocus(isFocused, aDrawNow);
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtCombo::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::MakeVisible(TBool aVisible)
{
    DoClose();
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->EnableFeedbackForControl(this, aVisible);
    }
#endif // RD_TACTILE_FEEDBACK
    CAknControl::MakeVisible(aVisible);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetDimmed(TBool aDimmed)
{
    DoClose();
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->EnableFeedbackForControl(this, !aDimmed);
    }
#endif // RD_TACTILE_FEEDBACK
    CAknControl::SetDimmed(aDimmed);
    iCombo->SetDimmed(aDimmed);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtCombo::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtCombo::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (iOpen)
    {
        // The popup list already handled the key event at this point.
        // Without this return, the popup list would open again.
        // Therefore, while open, the Combo does not generate key events.
        return EKeyWasConsumed;
    }
    TBool traversalDoIt = ETrue;
    if (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK)
    {
        traversalDoIt = EFalse;
    }
    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------
// CSwtCombo::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtCombo::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    TBool pressed = iPressed;
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        iPressed = ETrue;
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        // When clicking on area outside combo (e.g. on Caption of captioned
        // control), no native feedback (from CAknChoiceList) is generated.
        // So eSWT has to generate the feedback.
        if (iFeedback && !iCombo->Rect().Contains(aPointerEvent.iPosition))
        {
            iFeedback->InstantFeedback(ETouchFeedbackList);
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    }

    if (aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        if (!iDisplay.RevertPointerEvent())
        {
            // Opening the popup choice list must be always done manually.
            DoOpenL();
        }
        else
        {
            iPressed = EFalse;
        }
    }
    else
    {
        CCoeControl::HandlePointerEventL(aPointerEvent);
    }

    if (pressed != iPressed)
    {
        GetShell().UpdateHighlight(ETrue); // draw now
    }

    PostMouseEventL(aPointerEvent);
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtCombo::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------
// CSwtCombo::Draw
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
    TRect rect(iCombo->Rect());
    TAknLayoutRect topLeft = CSwtLafFacade::GetLayoutRect(
                                 CSwtLafFacade::EListHighlightSkinPlacingGeneralLine2, rect);

    TAknLayoutRect bottomRight = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EListHighlightSkinPlacingGeneralLine5, rect);

    TRect innerRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);

    // "Connect" the label and the button with a toolbar frame.
    AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(),
                             gc,
                             rect,
                             innerRect,
                             iPressed || iOpen ? KAknsIIDQsnFrButtonTbPressed : KAknsIIDQsnFrButtonTb,
                             iPressed || iOpen ? KAknsIIDQsnFrButtonTbCenterPressed : KAknsIIDQsnFrButtonTbCenter);
}

// ---------------------------------------------------------------------------
// CSwtCombo::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtCombo::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);
    if (!id.Pointer())
    {
        return CAknControl::MopSupplyObject(aId);
    }
    else
    {
        return id;
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::SbFrame
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtCombo::SbFrame() const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCombo::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtCombo::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtCombo::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtCombo::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtCombo::ComputeSizeL(TInt aWhint, TInt aHhint)
{
    TSize prefSize(aWhint, aHhint);

    if (aWhint != KSwtDefault && aHhint != KSwtDefault)
    {
        return prefSize;
    }

    // WORKAROUND: The platform API does not provide functions that could
    // return ideal dimensions based on content. Therefore, default values
    // from CAknChoiceList layouts and text measuring give the computed size.
    // WARNING: We asume here that CAknChoiceList will not change layouts.
    TInt w = aWhint;
    TInt h = aHhint;

    CSwtLafFacade::TSwtLafFacadeWindowLineLayout layoutPane =
        CSwtLafFacade::GetChoiceListPaneLayout();
    CSwtLafFacade::TSwtLafFacadeTextLineLayout layoutLabel =
        CSwtLafFacade::GetChoiceListPaneT1Layout();

    if (aHhint == KSwtDefault)
    {
        // Calculate height based on choice list pane and border.
        h = layoutPane.iH;

        MSwtUiUtils& utils = iDisplay.UiUtils();
        TMargins8 padding = iDisplay.UiUtils().InlineReferencePadding();
        TInt height = utils.InlineReferenceFontHeight();
        height += padding.iTop + padding.iBottom;
        if (h < height) h = height;

        h += BorderSizeDelta().iHeight;
    }

    if (aWhint == KSwtDefault)
    {
        // Go trough all the items and get the with of the widest one.
        TInt txtMaxW = 0;
        const CAknLayoutFont* font = AknLayoutUtils::LayoutFontFromId(
                                         layoutLabel.iFont);
        if (font)
        {
            TInt count = iItems->Count();
            for (TInt i = 0; i < count; i++)
            {
                txtMaxW = Max(txtMaxW,
                              font->TextWidthInPixels((*iItems)[i]));
            }
            // WORKAROUND: Add space for one more char to avoid clipping :)
            txtMaxW += font->TextWidthInPixels(KTxtSpace);
        }

        // Calculate width based on widest item, label pane and border.
        w += txtMaxW;
        w += layoutLabel.il;
        w += layoutLabel.ir;
        w += BorderSizeDelta().iWidth;
    }
    prefSize.iHeight = h;
    prefSize.iWidth = w;

    return prefSize;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCombo::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType == EEventKey)
    {
        if (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK)
        {
            if (!iOpen)
            {
                Invalidate(Rect());
                DoOpenL();
                iDisplay.PostDefaultSelectionEventL(iPeer);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::IsKeyUsed
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtCombo::IsKeyUsed(TUint aKeyCode) const
{
    TBool used = ETrue;
    switch (aKeyCode)
    {
    case EKeyBackspace:
        used = EFalse;
        break;

    default:
        break;
    }
    return used;
}

// ---------------------------------------------------------------------------
// CSwtCombo::SwtHandleResourceChangeL
// ---------------------------------------------------------------------------
//
void CSwtCombo::SwtHandleResourceChangeL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        DoClose();
    }

    CAknControl::HandleResourceChange(aType);
}

// ---------------------------------------------------------------------------
// CSwtCombo::MSKLabelL
// From MSwtControl
// ---------------------------------------------------------------------------
//
HBufC* CSwtCombo::MSKLabelL() const
{
    return iEikonEnv->AllocReadResourceL(R_QTN_MSK_OPEN);
}

// ---------------------------------------------------------------------------
// CSwtCombo::PressBackgroundPolicy
// From MSwtControl
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::PressBackgroundPolicy() const
{
    return EEmbeddedPressBackground;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
void CSwtCombo::ComputeTrim(TRect& aRect) const
{
    aRect = BorderOuterRect(aRect);
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtCombo::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::RegisterChildL(MSwtControl& /*aChild*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::UnregisterChild(MSwtControl& /*aChild*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtCombo::GetChildrenPeersL()
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCombo::Children
// From MSwtComposite
// ---------------------------------------------------------------------------
//
const RSwtControlArray* CSwtCombo::Children() const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ChildFocusChanged
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::ChildFocusChanged(MSwtControl& /*aControl*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::CountFocusableChildren
// From MSwtComposite
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::CountFocusableChildren(TInt, const MSwtControl*, TBool) const
{
    // DO NOTHING
    return 0;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetTabListL(MSwtControl** /*aControls*/, TInt /*aCount*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtCombo::GetTabListL() const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::MoveChildAbove(MSwtControl& /*aChild1*/, const MSwtControl* /*aChild2*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::MoveChildBelow(MSwtControl& /*aChild1*/, const MSwtControl* /*aChild2*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::ComputeTrim
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::NotifyRadioGroup(MSwtButton* /*aSelectedButton*/)
{
    // DO NOTHING
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetScrolledCompositeContentL
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetScrolledCompositeContentL(MSwtControl* /*aContent*/)
{
}

// ---------------------------------------------------------------------------
// CSwtCombo::ScrolledCompositeContent
// From MSwtComposite
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtCombo::ScrolledCompositeContent() const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCombo::ScrolledCompositePysicsAction
// From MSwtComposite
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::ScrolledCompositePysicsAction() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CSwtCombo::DrawScrollBarBackground
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCombo::DrawScrollBarBackground(const TRect& /*aRect*/) const
{
    // Do nothing
}

// ---------------------------------------------------------------------------
// CSwtCombo::Composite
// From MSwtCombo
// ---------------------------------------------------------------------------
//
MSwtComposite* CSwtCombo::Composite()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtCombo::AppendL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::AppendL(const TDesC& aTxt)
{
    if (iItems->Count() == 0)
    {
        UpdateTouchFeedbackRect();
    }

    // Get selected index
    TInt selIdx = iCombo->SelectedIndex();

    // Close if opened
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    // Append item.
    iItems->AppendL(aTxt);

    // Restore selected index
    iCombo->SetSelectedIndex(selIdx);

    // Reopen if previously opened
    if (reopen)
    {
        DoOpenL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::InsertL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::InsertL(TInt aIdx, const TDesC& aTxt)
{
    // Throw exception on invalid range
    if ((aIdx < 0) || (aIdx > iItems->Count()))
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    if (iItems->Count() == 0)
    {
        UpdateTouchFeedbackRect();
    }

    // Get selected index
    TInt selIdx = iCombo->SelectedIndex();

    // Close if opened
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    // Insert string
    iItems->InsertL(aIdx, aTxt);

    // Restore selected index
    if (aIdx <= selIdx)
    {
        selIdx++;
    }
    iCombo->SetSelectedIndex(selIdx);

    // Reopen if previously opened
    if (reopen)
    {
        DoOpenL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::ClearSelectionL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::ClearSelectionL()
{
    // NOT SUPPORTED FOR READ_ONLY
}

// ---------------------------------------------------------------------------
// CSwtCombo::CopyL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::CopyL()
{
    // NOT SUPPORTED FOR READ_ONLY
}

// ---------------------------------------------------------------------------
// CSwtCombo::DeselectItem
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::DeselectItem(TInt aIdx)
{
    if ((aIdx >= 0) && (aIdx < iItems->Count()))
    {
        if (iCombo->SelectedIndex() == aIdx)
        {
            // Item exists and is selected, clear selection
            TRAP_IGNORE(DoSelectItemL(-1));
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::DeselectAll
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::DeselectAll()
{
    TRAP_IGNORE(DoSelectItemL(-1));
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetItemL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::GetItemL(TInt aIdx, TPtrC& aTxt) const
{
    if ((aIdx >= 0) && (aIdx < iItems->Count()))
    {
        // Item exists, return its string
        aTxt.Set((*iItems)[aIdx]);
    }
    else
    {
        // Item does not exist, leave
        User::Leave(ESwtErrorInvalidRange);
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetItemCount
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetItemCount() const
{
    return iItems->Count();
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetItemHeight
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetItemHeight() const
{
    // NOT SUPPORTED BY AVKON
    return 0;
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetItemsL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
CPtrCArray* CSwtCombo::GetItemsL() const
{
    TInt txtCount = iItems->Count();

    // No items, return NULL
    if (txtCount == 0)
    {
        return NULL;
    }

    // Return copy of the item array
    CPtrCArray* arr = new(ELeave) CPtrCArray(txtCount);
    CleanupStack::PushL(arr);
    for (TInt i = 0; i < txtCount; i++)
    {
        arr->AppendL((*iItems)[i]);
    }
    CleanupStack::Pop(arr);

    return arr;
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetOrientation
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetOrientation() const
{
    if (iCombo->Flags() & CSwtComboProxy::EComboPositionLeft)
    {
        return KSwtStyleRightToLeft;
    }
    else
    {
        return KSwtStyleLeftToRight;
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetSelection
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TPoint CSwtCombo::GetSelection() const
{
    // NOT SUPPORTED FOR READ_ONLY
    return TPoint();
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetSelectionIndex
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetSelectionIndex() const
{
    return iCombo->SelectedIndex();
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetTextL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
HBufC* CSwtCombo::GetTextL() const
{
    TInt idx = iCombo->SelectedIndex();
    if (idx >= 0 && idx < iItems->Count())
    {
        // An item is selected / shown in the label, return its string
        HBufC* buf = NULL;
        TPtrC txt((*iItems)[iCombo->SelectedIndex()]);
        buf = HBufC::NewL(txt.Length());
        *buf = txt;
        return buf;
    }
    else
    {
        // No item selected / empty label
        return KNullDesC().AllocL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetTextHeightL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetTextHeightL() const
{
    return iCombo->MinimumSize().iHeight;
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetTextLimit
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetTextLimit() const
{
    // NOT SUPPORTED FOR READ_ONLY
    return -1;
}

// ---------------------------------------------------------------------------
// CSwtCombo::GetVisibleItemCount
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::GetVisibleItemCount() const
{
    // WORKAROUND No API for this but we know that this number is max 6
    if (iItems->Count() < KMaxVisibleItemCount)
    {
        return iItems->Count();
    }
    else
    {
        return KMaxVisibleItemCount;
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::IndexOf
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TInt CSwtCombo::IndexOf(const TDesC& aTxt, TInt& aStart) const
{
    // Out of range check
    if ((aStart < 0) || (aStart >= iItems->Count()))
    {
        return KErrNotFound;
    }

    // Search for the string, return index
    if (aStart == 0)
    {
        TInt pos;
        if (iItems->Find(aTxt, pos) == KErrNone)
        {
            return pos;
        }
    }
    else
    {
        for (TInt i = aStart; i < iItems->Count(); i++)
        {
            if (aTxt.Compare((*iItems)[i]) == KErrNone)
            {
                return i;
            }
        }
    }

    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// CSwtCombo::InsertInTextControlL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TBool CSwtCombo::InsertInTextControlL(const TDesC& /*aTxt*/)
{
    // NOT SUPPORTED FOR READ_ONLY
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtCombo::PasteL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TBool CSwtCombo::PasteL()
{
    // NOT SUPPORTED FOR READ_ONLY
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtCombo::RemoveItemL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::RemoveItemL(TInt aIdx)
{
    // Leave if out of range
    if ((aIdx < 0) || (aIdx >= iItems->Count()))
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    // Get selected index
    TInt selIdx = iCombo->SelectedIndex();

    // Close if open
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    // Remove item
    iItems->Delete(aIdx);

    // Restore selected index
    if (aIdx <= selIdx)
    {
        if (aIdx == selIdx)
        {
            selIdx = -1;
        }
        else
        {
            selIdx--;
        }
    }
    iCombo->SetSelectedIndex(selIdx);

    // Clear feedback if no items left
    if (iItems->Count() == 0)
    {
        UpdateTouchFeedbackRect();
    }

    // Reopen if previously open
    if (reopen)
    {
        DoOpenL();
    }
}

// --------------------------------------------------------------------------
// CSwtCombo::RemoveRangeL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::RemoveRangeL(TInt aStart, TInt aEnd)
{
    ASSERT(aStart <= aEnd);

    // Leave if out of range
    if ((aStart < 0) || (aEnd >= iItems->Count()))
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    // Get selected index
    TInt selIdx = iCombo->SelectedIndex();

    // Close if open
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    // Remove inclusive range of items
    iItems->Delete(aStart, aEnd - aStart + 1);

    // Restore selected index
    if (aStart <= selIdx)
    {
        if (aEnd >= selIdx)
        {
            selIdx = -1;
        }
        else
        {
            selIdx = selIdx - (aEnd - aStart + 1);
        }
    }
    iCombo->SetSelectedIndex(selIdx);

    // Clear feedback if no items left
    if (iItems->Count() == 0)
    {
        UpdateTouchFeedbackRect();
    }

    // Reopen if previously open
    if (reopen)
    {
        DoOpenL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::RemoveAllL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::RemoveAllL()
{
    // Close if open
    if (iOpen)
    {
        DoClose();
    }

    // Remove all items
    iItems->Reset();

    // Clear selection and feedback area
    iCombo->SetSelectedIndex(-1);
    UpdateTouchFeedbackRect();
}

// ---------------------------------------------------------------------------
// CSwtCombo::SelectItemL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::SelectItemL(TInt aIdx)
{
    if ((aIdx >= 0) && (aIdx < iItems->Count()))
    {
        // Item exists, select it
        DoSelectItemL(aIdx);
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetItemsL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetItemsL(MDesCArray* aTxtArr)
{
    // Copy the array
    CDesCArray* arr = new(ELeave) CDesCArrayFlat(aTxtArr->MdcaCount());
    CleanupStack::PushL(arr);
    for (TInt i = 0; i < aTxtArr->MdcaCount(); i++)
    {
        arr->AppendL(aTxtArr->MdcaPoint(i));
    }
    CleanupStack::Pop(arr);

    // Close if open
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    // Set copied array. The previous item array is cleared by CAknChoiceList
    iCombo->SetItems(arr);
    iItems = arr;

    // Clear selection
    iCombo->SetSelectedIndex(-1);

    UpdateTouchFeedbackRect();

    // Reopen if previously open
    if (reopen)
    {
        DoOpenL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetOrientationL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetOrientationL(TInt aOrientation)
{
    if (!(aOrientation & KSwtStyleRightToLeft
            || aOrientation & KSwtStyleLeftToRight))
    {
        return;
    }

    // Close if open
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    // Change flags
    TInt flags = iCombo->Flags();
    if (aOrientation & KSwtStyleRightToLeft)
    {
        flags &= ~CSwtComboProxy::EComboPositionRight;
        flags |= CSwtComboProxy::EComboPositionLeft;
    }
    else
    {
        flags &= ~CSwtComboProxy::EComboPositionLeft;
        flags |= CSwtComboProxy::EComboPositionRight;
    }
    iCombo->SetFlags(flags);

    // Reopen if previously open
    if (reopen)
    {
        DoOpenL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetSelectionL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetSelectionL(TInt /*aStart*/, TInt /*aEnd*/)
{
    // NOT SUPPORTED FOR READ_ONLY
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetTextL
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TBool CSwtCombo::SetTextL(TDes& /*aTxt*/)
{
    // NOT SUPPORTED FOR READ_ONLY
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetTextLimit
// From MSwtCombo
// ---------------------------------------------------------------------------
//
TBool CSwtCombo::SetTextLimit(TInt /*aTxtLimit*/)
{
    // NOT SUPPORTED BY AVKON
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtCombo::SetVisibleItemCount
// From MSwtCombo
// ---------------------------------------------------------------------------
//
void CSwtCombo::SetVisibleItemCount(TInt /*aCount*/)
{
    // NOT SUPPORTED BY AVKON
}

// ---------------------------------------------------------------------------
// CSwtCombo::HandleControlEventL
// From MCoeControlObserver
// ---------------------------------------------------------------------------
//
void CSwtCombo::HandleControlEventL(
    CCoeControl* aControl,
    TCoeEvent aEventType)
{
    if ((aControl == iCombo) && (aEventType == EEventStateChanged || aEventType == EEventRequestCancel))
    {
        // The choice list got closed
        iOpen = EFalse;
        iPressed = EFalse;

        // If selection changed, post selection event
        if (iPrevSelIdx != iCombo->SelectedIndex())
        {
            iDisplay.PostSelectionEventL(iPeer);
        }

        // Get new selection
        iPrevSelIdx = iCombo->SelectedIndex();

        // Remove the pressed down highlight
        GetShell().UpdateHighlight(ETrue);
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::DoOpenL()
// ---------------------------------------------------------------------------
//
void CSwtCombo::DoOpenL()
{
    if ((!iOpen) && (iItems->Count() > 0))
    {
        // Open choice list only if there are items
        iOpening = ETrue;
        iCombo->ShowChoiceListL();
        iOpening = EFalse;
        iOpen = ETrue;

        // This is needed by the Composite pointer handling. Composite must
        // receive a pointer up event to clear the pointer capturing control.
        ClaimPointerGrab();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::DoClose()
// ---------------------------------------------------------------------------
//
void CSwtCombo::DoClose()
{
    if (iOpen)
    {
        iCombo->HideChoiceList();
        iOpen = EFalse;
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::DoSelectItemL()
// ---------------------------------------------------------------------------
//
void CSwtCombo::DoSelectItemL(TInt aIdx)
{
    // Close if opened
    TBool reopen = EFalse;
    if (iOpen)
    {
        DoClose();
        reopen = ETrue;
    }

    iCombo->SetSelectedIndex(aIdx);

    // Reopen if previously opened
    if (reopen)
    {
        DoOpenL();
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::CenterContainedCombo()
// ---------------------------------------------------------------------------
//
void CSwtCombo::CenterContainedCombo()
{
    TRect rect(BorderInnerRect());
    TInt h(iCombo->Size().iHeight);
    if (h < rect.Height())
    {
        iCombo->SetPosition(TPoint(rect.iTl.iX,
                                   rect.iTl.iY + (rect.Height() - h) / 2));
    }
    else
    {
        iCombo->SetPosition(rect.iTl);
    }
}

// ---------------------------------------------------------------------------
// CSwtCombo::UpdateTouchFeedbackRect()
// ---------------------------------------------------------------------------
//
void CSwtCombo::UpdateTouchFeedbackRect() const
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        if (iItems->Count() > 0)
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            CFeedbackSpec* spec = CFeedbackSpec::New();
            if (spec)
            {
                spec->AddFeedback(ETouchEventStylusDown, ETouchFeedbackList);
                spec->AddFeedback(ETouchEventStylusUp, ETouchFeedbackList);
                iFeedback->SetFeedbackArea(
                    this, KTouchFeedbackRectId1, Rect(), spec);
                delete spec;
            }
#else
            iFeedback->SetFeedbackArea(this, KTouchFeedbackRectId1, Rect(),
                                       ETouchFeedbackBasic, ETouchEventStylusDown);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        }
        else
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            CFeedbackSpec* spec = CFeedbackSpec::New();
            if (spec)
            {
                spec->AddFeedback(ETouchEventStylusDown, ETouchFeedbackNone);
                spec->AddFeedback(ETouchEventStylusUp, ETouchFeedbackNone);
                iFeedback->SetFeedbackArea(
                    this, KTouchFeedbackRectId1, Rect(), spec);
                delete spec;
            }
#else
            iFeedback->ChangeFeedbackType(this, KTouchFeedbackRectId1,
                                          ETouchFeedbackNone);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        }
    }
#endif //RD_TACTILE_FEEDBACK
}

