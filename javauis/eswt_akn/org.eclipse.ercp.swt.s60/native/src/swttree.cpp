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


#include <aknsinglecolumnstyletreelist.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <avkon.mbg>
#include <barsread.h>
#include <swtlaffacade.h>
#include "eswtmobileextensions.h"
#include "swtfont.h"
#include "swttree.h"
#include "swtcontrolhelper.h"

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// ======== MEMBER FUNCTIONS ========


CSwtTree* CSwtTree::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtComposite& aParent, TInt aStyle)
{
    CCoeControl& parentCtrl = aParent.Control()->CoeControl();
    CSwtTree* self = new(ELeave) CSwtTree(aDisplay, aPeer, aParent, aStyle,
                                          parentCtrl.IsVisible(), parentCtrl.IsDimmed());
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

CSwtTree::CSwtTree(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                   MSwtComposite& aParent, TInt aStyle, TBool aVisibility, TBool aDimmed)
        : CSwtComposite(aDisplay, aPeer, &aParent, aStyle, aVisibility, aDimmed)
        , iItemHeightValid(EFalse)
{
}

void CSwtTree::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);
    SetComponentsToInheritVisibility(ETrue);
    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());

    iTree = CAknSingleColumnStyleTreeList::NewL(*this);
    TInt flags = KAknTreeListMarqueeScrolling;
    if (IsMarkable())
    {
        flags |= KAknTreeListMarkable;
    }
    iTree->SetFlags(flags);
    iTree->AddObserverL(this);

    // Eventhough container already given, we still need to do this for the skin background
    iTree->SetContainerWindowL(*this);

    // Give initial LAF value
    UpdateItemHeight();

    ActivateL();

    TDisplayMode displayMode(ENone);
    CEikonEnv* eikonEnv = iDisplay.CoeEnv();
    if (eikonEnv)
    {
        CWsScreenDevice* screenDevice = eikonEnv->ScreenDevice();
        if (screenDevice)
        {
            displayMode = screenDevice->DisplayMode();
        }
    }
    iMaskHandler = new CSwtMaskHandler(displayMode);
}

CSwtTree::~CSwtTree()
{
    delete iTree;
    iItems.Close();
    delete iMaskHandler;
    TInt count = iImages.Count();
    for (TInt i = 0; i < count; i++)
    {
        TSize imageSize = iImages[i]->Bitmap().SizeInPixels();
        if (imageSize.iHeight > iImageMaxSize.iHeight ||
                imageSize.iWidth > iImageMaxSize.iWidth)
        {
            // If image size is bigger than the old maximum size, then
            // the image has been also previously scaled.
            imageSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                            imageSize, iImageMaxSize);
        }
        iImages[i]->RemoveSubRef(imageSize);
        iImages[i]->RemoveRef();
    }
    iImages.Close();
    iImageIds.Close();
    iImageRefs.Close();
}

TInt CSwtTree::CountComponentControls() const
{
    return 1;
}

CCoeControl* CSwtTree::ComponentControl(TInt /*aIdx*/) const
{
    return iTree;
}

void CSwtTree::HandleResourceChange(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        iItemHeightValid = EFalse;
        UpdateItemHeight();
    }
    CAknControl::HandleResourceChange(aType);
    UpdateImagesSize();
}

TKeyResponse CSwtTree::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TBool traversalDoIt = ETrue;

    if (iItems.Count() > 0)
    {
        switch (aKeyEvent.iCode)
        {
        case EKeyEnter:
        case EKeyOK:
        {
            if (IsMarkable())
            {
                traversalDoIt = EFalse;
            }
            break;
        }
        case EKeyUpArrow:
        {
            traversalDoIt = EFalse;
            if (iTree->FocusedItem() == iItems[0])
            {
                if (GetShell().FindTraversalTargetL(ESwtTraverseArrowPrevious, *this))
                {
                    traversalDoIt = ETrue;
                }
            }
            break;
        }
        case EKeyDownArrow:
        {
            traversalDoIt = EFalse;
            if (iTree->FocusedItem() == LastCollapsedItem())
            {
                if (GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this))
                {
                    traversalDoIt = ETrue;
                }
            }
            break;
        }
        case EKeyLeftArrow:
        {
            // Left key collapses node
            TInt item = iTree->FocusedItem();
            if (item != KAknTreeIIDNone)
            {
                traversalDoIt = EFalse;
                if (iTree->Parent(item) == KAknTreeIIDRoot && !iTree->IsExpanded(item))
                {
                    if (GetShell().FindTraversalTargetL(ESwtTraverseArrowPrevious, *this))
                    {
                        traversalDoIt = ETrue;
                    }
                }
            }
            break;
        }
        case EKeyRightArrow:
        {
            // Right key expands node
            TInt item = iTree->FocusedItem();
            if (item != KAknTreeIIDNone)
            {
                traversalDoIt = EFalse;
                if (iTree->ChildCount(item) == 0)
                {
                    if (GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this))
                    {
                        traversalDoIt = ETrue;
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }

    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

void CSwtTree::SizeChanged()
{
    if (iTree)
    {
        iTree->SetRect(BorderInnerRect());
    }
    CSwtComposite::SizeChanged();
}

void CSwtTree::PositionChanged()
{
    if (iTree)
    {
        iTree->SetRect(BorderInnerRect());
    }
    CSwtComposite::PositionChanged();
}

void CSwtTree::FocusChanged(TDrawNow aDrawNow)
{
    // This gets called before contained list is created.
    if (iTree)
    {
        TBool isFocused = IsFocusControl();
        iTree->SetFocus(isFocused, aDrawNow);
        if (isFocused)
        {
            UpdateItemHeight();
        }
    }
    HandleFocusChanged(aDrawNow);
}

CCoeControl& CSwtTree::CoeControl()
{
    return *this;
}

const CCoeControl& CSwtTree::CoeControl() const
{
    return *this;
}

// Needed for finding the skin background
TTypeUid::Ptr CSwtTree::MopSupplyObject(TTypeUid aId)
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

TBool CSwtTree::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    // Bypass CSwtComposite's focusability behavior.
    return ASwtScrollableBase::IsFocusable(aReason);
}

TInt CSwtTree::FocusBackgroundPolicy() const
{
    // Bypass CSwtComposite's focus background.
    return ASwtControlBase::FocusBackgroundPolicy();
};

void CSwtTree::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    ASSERT(iTree);
    iTree->OfferKeyEventL(aKeyEvent, aType);
}

TBool CSwtTree::IsKeyUsed(TUint aKeyCode) const
{
    if (aKeyCode == EKeyBackspace)
    {
        return EFalse;
    }
    else if (aKeyCode == EKeyOK || aKeyCode == EKeyEnter)
    {
        if (!IsMarkable())
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
    else
    {
        return ETrue;
    }
}

TSize CSwtTree::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (!iTree)
    {
        return TSize(0, 0);
    }

    TSize res(aWHint, aHHint);

    TInt count = iTree->ChildCount(KAknTreeIIDRoot);

    if (aWHint == KSwtDefault)
    {
        TRect mainRect(TRect::EUninitialized);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainRect);
        TAknLayoutRect layoutRect = ItemLayoutRect();
        TRect rect = layoutRect.Rect();
        TInt vPadding = mainRect.Height() - rect.Height();
        TInt scrollBarAndTrimWidth = Max(0, mainRect.Width() - rect.Width());
        TAknLayoutText txtLayoutRect = CSwtLafFacade::GetLayoutText(CSwtLafFacade::EListSingle2HeadingMsgPaneT1,
                                       rect, IsMarkable() ? 3 : 2);
        TInt txtRightPadding = Max(0, rect.iBr.iX - txtLayoutRect.TextRect().iBr.iX);
        TInt txtLeftPadding = Max(0, txtLayoutRect.TextRect().iTl.iX);
        TInt markIconWidth(0);
        if (IsMarkable())
        {
            markIconWidth = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSingle2HeadingMsgPaneG2,
                            txtLayoutRect.TextRect(), 1).Rect().Width();
        }

        res.iWidth = 0;
        const CFont* font = txtLayoutRect.Font();
        if (font)
        {
            for (TInt i = 0; i < count; i++)
            {
                res.iWidth = Max(res.iWidth, font->TextWidthInPixels(iTree->Text(iTree->Child(KAknTreeIIDRoot, i))));
            }
        }

        res.iWidth += txtLeftPadding;
        res.iWidth += txtRightPadding;
        res.iWidth += scrollBarAndTrimWidth;
        res.iWidth += markIconWidth;
        res.iWidth += GetBorderWidth() * 2;
    }

    if (aHHint == KSwtDefault)
    {
        if (count == 0)
        {
            count = 1; // empty tree
        }
        res.iHeight = count * ItemHeight();
        res.iHeight += GetBorderWidth() * 2;
    }

    return res;
}

void CSwtTree::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    //Native control used for tree doesn't have flag for MULTI, so
    //given feedback is normal list feedback.
    //When tree is MULTI, we should give checkbox feedback on touch down.
    //So first we disable feedback for native side and do our
    //checkbox feedback. Then feedback for native side is restored.
    MTouchFeedback* feedback = NULL;
    if (aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        TUint32 flags = iTree->Flags();
        if ((flags & KAknTreeListMarkable) &&
                iTree->IsLeaf(iTree->FocusedItem()))
        {
            feedback = MTouchFeedback::Instance();
            if (feedback)
            {
                feedback->EnableFeedbackForControl(iTree, EFalse);
                feedback->InstantFeedback(ETouchFeedbackCheckbox);
            }
        }
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    iLastFocusedItem = iTree->FocusedItem();
    CSwtComposite::HandlePointerEventL(aPointerEvent);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    if (feedback)
    {
        //Restore native feedback.
        feedback->EnableFeedbackForControl(iTree, ETrue);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    // The mouse event is posted to Java from CSwtComposite
}

MSwtComposite* CSwtTree::Composite()
{
    return this;
}

TInt CSwtTree::AddL(TInt aItemHandle)
{
    ASSERT(iTree);
    TInt flags = CAknSingleColumnStyleTreeList::EPersistent;
    if (aItemHandle < KAknTreeIIDRoot)
    {
        aItemHandle = KAknTreeIIDRoot;
    }

    TInt id = iTree->AddSubtitleRowL(aItemHandle, KNullDesC, flags, ETrue);

    // Ids of the children items are inserted after the id of the parent.
    // This is to facilitate 'insert with sort' mechanism.
    TBool parentFound = EFalse;
    if (aItemHandle != KAknTreeIIDRoot)
    {
        TInt index = iItems.Find(aItemHandle);
        if (index != KErrNotFound)
        {
            // Always at least +1 since item already added
            index += iTree->ChildCount(aItemHandle);
            User::LeaveIfError(iItems.Insert(id, index));
            parentFound = ETrue;
        }
    }

    if (!parentFound)
    {
        User::LeaveIfError(iItems.Append(id));
    }

    return id;
}

TRect CSwtTree::Bounds(TInt aItemHandle) const
{
    ASSERT(iTree);
    TRect rect(0, 0, 0, 0);
    TInt posInScreen = iTree->VisibleItemIndex(aItemHandle);
    if (posInScreen >= 0)
    {
        TInt itemHeight = ItemHeight();
        rect.iTl.iY = itemHeight * posInScreen;
        rect.SetWidth(iTree->Rect().Width());
        rect.SetHeight(itemHeight);
    }
    return rect;
}

void CSwtTree::Check(TInt aItemHandle, TBool aState)
{
    ASSERT(iTree);
    iTree->SetMarked(aItemHandle, aState, ETrue);
}

void CSwtTree::Collapse(TInt aItemHandle)
{
    ASSERT(iTree);
    if (iTree->ChildCount(aItemHandle) > 0)
    {
        iTree->CollapseNode(aItemHandle, ETrue);
    }
}

void CSwtTree::Expand(TInt aItemHandle)
{
    ASSERT(iTree);
    if (iTree->ChildCount(aItemHandle) > 0)
    {
        iTree->ExpandNode(aItemHandle, ETrue);
    }
}

TInt CSwtTree::InsertL(TInt aItemHandle, TInt aIndex)
{
    ASSERT(iTree);
    TInt flags = CAknSingleColumnStyleTreeList::EPersistent;
    if (aItemHandle < KAknTreeIIDRoot)
    {
        aItemHandle = KAknTreeIIDRoot;
    }

    TInt id = iTree->AddSubtitleRowL(aItemHandle, KNullDesC, flags, EFalse);

    // Insert id in the list ordered item id array
    if (KAknTreeIIDRoot == aItemHandle)
    {
        // Root item ids are inserted before the id position of aIndex sibbling
        TInt sibbling = iTree->Child(KAknTreeIIDRoot, aIndex);
        TInt index = iItems.Find(sibbling);
        if (index != KErrNotFound)
        {
            // Insert at sibbling's position
            ASSERT(index <= iItems.Count());
            User::LeaveIfError(iItems.Insert(id, index));
            iTree->Sort(this, ETrue);
        }
        else
        {
            // Insertion failure, just append the item id and pray
            User::LeaveIfError(iItems.Append(id));
        }
    }
    else
    {
        // Ids of the children items are inserted after the id of the parent.
        TInt index = iItems.Find(aItemHandle);
        if (index != KErrNotFound)
        {
            index++; // first position after parent
            index += aIndex;
            ASSERT(index <= iItems.Count());
            User::LeaveIfError(iItems.Insert(id, index));
            iTree->Sort(this, ETrue);
        }
        else
        {
            // Insertion failure, just append the item id and pray
            User::LeaveIfError(iItems.Append(id));
        }
    }

    return id;
}

TBool CSwtTree::IsExpanded(TInt aItemHandle) const
{
    ASSERT(iTree);
    if (iTree->ChildCount(aItemHandle) == 0)
    {
        return EFalse;
    }
    else
    {
        return iTree->IsExpanded(aItemHandle);
    }
}

TBool CSwtTree::IsSelected(TInt aItemHandle) const
{
    ASSERT(iTree);
    return iTree->IsMarked(aItemHandle);   // selected or checked
}

TInt CSwtTree::ItemAt(TInt aX, TInt aY) const
{
    TInt res = KErrNotFound;
    TRect rect = iTree->Rect();

    TPoint point(aX, aY);
    if (rect.Contains(point))
    {
        ASSERT(iTree);
        TInt count = iItems.Count();
        for (TInt i = 0; i < count; i++)
        {
            TInt id = iItems[i];
            if (Bounds(id).Contains(point))
            {
                return id;
            }
        }
    }

    return res;
}

TInt CSwtTree::ItemHeight() const
{
    const_cast<CSwtTree*>(this)->UpdateItemHeight();
    return iItemHeight;
}

void CSwtTree::Remove(TInt aItemHandle)
{
    ASSERT(iTree);

    // Must remove the item ids before removing the actual items.
    RemoveItemRefs(aItemHandle);

    // All children will automatically be removed too.
    iTree->RemoveItem(aItemHandle, ETrue);
}

void CSwtTree::RemoveAll()
{
    ASSERT(iTree);
    iItems.Reset();
    iTree->RemoveItem(KAknTreeIIDRoot, ETrue);
}

void CSwtTree::Select(const TInt* aItemHandles, TInt aCount, TBool aState)
{
    ASSERT(iTree);
    if (iStyle & KSwtStyleMulti)
    {
        for (TInt i = 0; i < aCount; i++)
        {
            iTree->SetMarked(aItemHandles[i], aState, ETrue);
        }
    }
    else
    {
        if (aState && aCount > 0)
        {
            iTree->SetFocusedItem(aItemHandles[0]);
        }
    }
}

void CSwtTree::SelectAll(TBool aState)
{
    ASSERT(iTree);
    iTree->SetMarked(KAknTreeIIDRoot, aState, ETrue);
}

TInt CSwtTree::SelectionCount() const
{
    const CArrayFix<TInt>* arr = NULL;
    TRAP_IGNORE(arr = SelectionL());
    TInt res(0);
    if (arr)
    {
        res = arr->Count();
    }
    delete arr;
    return res;
}

const CArrayFix<TInt>* CSwtTree::SelectionL() const
{
    ASSERT(iTree);
    CArrayFix<TInt>* res = NULL;
    if (iStyle & KSwtStyleMulti)
    {
        RArray<TInt> arr;
        CleanupClosePushL(arr);
        iTree->GetMarkedItemsL(arr);
        TInt count = arr.Count();
        if (count > 0)
        {
            res = new(ELeave) CArrayFixFlat<TInt>(count);
            CleanupStack::PushL(res);
            for (TInt i = 0; i < count; i++)
            {
                res->AppendL(arr[i]);
            }
            CleanupStack::Pop(res);
        }
        CleanupStack::PopAndDestroy(&arr);
    }
    else
    {
        TInt item = iTree->FocusedItem();
        if (item != KAknTreeIIDNone)
        {
            res = new(ELeave) CArrayFixFlat<TInt>(1);
            CleanupStack::PushL(res);
            res->AppendL(item);
            CleanupStack::Pop(res);
        }
    }
    return res;
}

// CSwtLafFacade::EListSingle2HeadingMsgPaneG1 could be used to implement downscaling
void CSwtTree::SetImageL(TInt aItemHandle, const MSwtImage* aImage)
{
    ASSERT(iTree);
    if (aImage)
    {
        TInt id = KErrNotFound;
        TInt index = iImages.Find(aImage);
        if (index != KErrNotFound)
        {
            // Image already used by other items, increase local ref count
            id = iImageIds[index];
            iImageRefs[index]++;
        }
        else
        {
            // best suitable size from layouts.
            TSize layoutImgSize = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSingle2HeadingMsgPaneG1,
                                  ItemLayoutRect().Rect(), 4).Rect().Size();
            // best image size we can make to fit in the above layout size.
            TSize bitmapSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                   aImage->Bitmap().SizeInPixels(), layoutImgSize);

            // Register image with CAknTree, increase MSwtImage ref count, store the new id for later use
            const CFbsBitmap& bmp = aImage->SubBitmap(bitmapSize);
            const CFbsBitmap* mask = aImage->SubMaskBitmap(bitmapSize, ETrue);
            aImage->AddSubRef(bitmapSize);
            iImageMaxSize = bitmapSize;
            if (!mask)
            {
                mask = iMaskHandler->GetMask(bmp.SizeInPixels());
            }
            id = iTree->AddIconL(const_cast<CFbsBitmap*>(&bmp), const_cast<CFbsBitmap*>(mask),
                                 EFalse, EAspectRatioPreserved);
            aImage->AddRef();
            User::LeaveIfError(iImages.Append(aImage));
            User::LeaveIfError(iImageIds.Append(id));
            User::LeaveIfError(iImageRefs.Append(1));
        }

        if (id != KErrNotFound)
        {
            // Set item custom image id
            iTree->SetIcon(aItemHandle, CAknSingleColumnStyleTreeList::ECollapsedNode, id, ETrue);
            iTree->SetIcon(aItemHandle, CAknSingleColumnStyleTreeList::EExpandedNode, id, ETrue);
        }
    }
    else
    {
        TInt id = iTree->Icon(aItemHandle, CAknSingleColumnStyleTreeList::EExpandedNode);
        if (id != KErrNotFound)
        {
            // Set default image id to item
            iTree->SetIcon(aItemHandle, CAknSingleColumnStyleTreeList::EExpandedNode,
                           AknTreeListIconID::KDefault, ETrue);
            iTree->SetIcon(aItemHandle, CAknSingleColumnStyleTreeList::ECollapsedNode,
                           AknTreeListIconID::KDefault, ETrue);
            TInt index = iImageIds.Find(id);
            if (index != KErrNotFound)
            {
                // Decrease local ref
                iImageRefs[index]--;
                if (iImageRefs[index] == 0)
                {
                    // No items are using the image, deregister it and decrease the MSwtImage ref count
                    iTree->RemoveIconL(id);
                    const MSwtImage* image = iImages[index];
                    TSize imageSize = image->Bitmap().SizeInPixels();
                    if (imageSize.iHeight > iImageMaxSize.iHeight ||
                            imageSize.iWidth > iImageMaxSize.iWidth)
                    {
                        imageSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                        imageSize, iImageMaxSize);
                    }
                    image->RemoveSubRef(imageSize);
                    image->RemoveRef();
                    iImages.Remove(index);
                    iImageIds.Remove(index);
                    iImageRefs.Remove(index);
                    iImages.Compress();
                    iImageIds.Compress();
                    iImageRefs.Compress();
                }
            }
        }
    }
}

void CSwtTree::SetTextL(TInt aItemHandle, const TDesC& aText)
{
    ASSERT(iTree);
    iTree->SetTextL(aItemHandle, aText, ETrue);
}

TInt CSwtTree::TopItem() const
{
    ASSERT(iTree);
    TInt count = iItems.Count();
    for (TInt i = 0; i < count; i++)
    {
        TInt id = iItems[i];
        if (iTree->VisibleItemIndex(id) == 0)
        {
            return id;
        }
    }
    return KErrNotFound;
}

TInt CSwtTree::HandleTreeListEvent(CAknTreeList& aList, TAknTreeItemID aItem,
                                   MAknTreeListObserver::TEvent aEvent)
{
    if (&aList != iTree)
    {
        return KErrNone;
    }

    // EItemSelected is never sent for nodes, hence the manual marking
    switch (aEvent)
    {
    case MAknTreeListObserver::ENodeExpanded:
    {
        TInt count = iTree->ChildCount(aItem);
        if (count > 0)
        {
            TRAP_IGNORE(iDisplay.PostTreeEventL(iPeer, ESwtEventExpand, aItem));
        }
        if (IsMarkable())
        {
            if (count == 0)
            {
                iTree->SetMarked(aItem, ETrue, ETrue);
            }
        }
        else
        {
            TRAP_IGNORE(iDisplay.PostTreeEventL(iPeer, ESwtEventDefaultSelection, aItem));
        }
        break;
    }
    case MAknTreeListObserver::ENodeCollapsed:
    {
        TInt count = iTree->ChildCount(aItem);
        if (count > 0)
        {
            TRAP_IGNORE(iDisplay.PostTreeEventL(iPeer, ESwtEventCollapse, aItem));
        }
        if (IsMarkable())
        {
            if (count == 0)
            {
                iTree->SetMarked(aItem, EFalse, ETrue);
            }
        }
        break;
    }
    case MAknTreeListObserver::EItemFocused:
    {
        if (iStyle & KSwtStyleSingle)
        {
            TRAP_IGNORE(iDisplay.PostTreeEventL(iPeer, ESwtEventSelection, aItem));
        }
        break;
    }
    case MAknTreeListObserver::EItemMarked:
    {
        TRAP_IGNORE(iDisplay.PostTreeEventL(iPeer, ESwtEventSelection, aItem));
        break;
    }
    case MAknTreeListObserver::EItemUnmarked:
    {
        TRAP_IGNORE(iDisplay.PostTreeEventL(iPeer, ESwtEventSelection, aItem));
        break;
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    case MAknTreeListObserver::EEventPanningStarted:
        GetShell().SetUrgentPaintControl(this);
        break;
    case MAknTreeListObserver::EEventFlickStopped:
        GetShell().SetUrgentPaintControl(NULL);
        break;
#endif // RD_JAVA_S60_RELEASE_9_2
    default:
        break;
    }

    return KErrNone;
}

TInt CSwtTree::Compare(TAknTreeItemID aFirst, TAknTreeItemID aSecond)
{
    TInt firstPos = iItems.Find(aFirst);
    TInt secondPos = iItems.Find(aSecond);
    if (firstPos < secondPos)
    {
        return -1;
    }
    else
    {
        return +1;
    }
}

void CSwtTree::UpdateItemHeight()
{
    if (iItemHeightValid)
    {
        return;
    }

    ASSERT(iTree);
    TRect rect = iTree->HighlightRect();
    if (!rect.IsEmpty())
    {
        iItemHeightValid = ETrue;
        iItemHeight = rect.Height();
        return;
    }

    // LAF as last resort, might be a few pixels inaccurate.
    iItemHeightValid = EFalse;
    iItemHeight = ItemLayoutRect().Rect().Height();
}

TAknLayoutRect CSwtTree::ItemLayoutRect()
{
    TRect mainRect(TRect::EUninitialized);
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainRect);
    TAknLayoutRect layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListScrollGenPane, mainRect, 0);
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListGenPane, layoutRect.Rect(), 0);
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSingleGraphicH1Pane, layoutRect.Rect(), 0);
    return layoutRect;
}

void CSwtTree::UpdateImagesSize()
{
    TSize imageNewSize = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSingle2HeadingMsgPaneG1,
                         ItemLayoutRect().Rect(), 4).Rect().Size();

    if (imageNewSize != iImageMaxSize && iImages.Count() > 0)
    {
        TInt count = iItems.Count();
        for (TInt i = 0; i < count; i++)
        {
            TInt itemHandle = iItems[i];
            if (itemHandle)
            {
                // getting icon id from itemHandle.
                TInt id = iTree->Icon(itemHandle, CAknSingleColumnStyleTreeList::EExpandedNode);
                if (id != KErrNotFound)
                {
                    // image index from icon id.
                    TInt index = iImageIds.Find(id);
                    const MSwtImage* image = iImages[index];
                    TSize imageSize = image->Bitmap().SizeInPixels();

                    TSize oldSize;
                    TBool doScaling = ETrue;

                    if (imageSize.iHeight > iImageMaxSize.iHeight ||
                            imageSize.iWidth > iImageMaxSize.iWidth)
                    {
                        // If image size is bigger than the old maximum size, then
                        // the image has been also previously scaled.
                        oldSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                      imageSize, iImageMaxSize);
                    }
                    else if (imageSize.iHeight > imageNewSize.iHeight ||
                             imageSize.iWidth > imageNewSize.iWidth)
                    {
                        // Image is bigger than the new boundaries, but it did fit
                        // inside the old boundaries, so the image is default size.
                        oldSize = imageSize;
                    }
                    else
                    {
                        doScaling = EFalse;
                    }

                    if (doScaling)
                    {

                        // best image size to fit in the tree item.
                        imageSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                        imageSize, imageNewSize);
                        // scaling down the bitmap and mask.
                        const CFbsBitmap& bmp =  image->SubBitmap(imageSize);
                        const CFbsBitmap* mask =  image->SubMaskBitmap(imageSize, ETrue);

                        if (!mask)
                        {
                            mask = iMaskHandler->GetMask(bmp.SizeInPixels());
                        }
                        // replacing the old icon with new icon.
                        TRAP_IGNORE(iTree->AssignIconL(id, const_cast<CFbsBitmap*>(&bmp) ,
                                                       const_cast<CFbsBitmap*>(mask), EFalse, EAspectRatioPreserved));

                        image->AddSubRef(imageSize);
                        image->RemoveSubRef(oldSize);

                    }
                }
            }
        }
        iImageMaxSize = imageNewSize;
    }
}


TInt CSwtTree::LastCollapsedItem()
{
    TInt count = iItems.Count();
    if (count == 0)
    {
        return KErrNotFound;
    }
    TInt i;
    for (i = count - 1; i >= 0; i--)
    {
        TInt parentId = iTree->Parent(iItems[i]);
        if (parentId == KAknTreeIIDRoot)
        {
            break;
        }
        if (iTree->IsExpanded(parentId))
        {
            break;
        }
    }
    return iItems[i];
}

void CSwtTree::RemoveItemRefs(TInt aItemHandle)
{
    TInt index = iItems.Find(aItemHandle);
    if (index != KErrNotFound)
    {
        iItems.Remove(index);
    }
    TInt count = iTree->ChildCount(aItemHandle);
    for (int i = 0; i < count; i++)
    {
        RemoveItemRefs(iTree->Child(aItemHandle, i));
    }
}

TBool CSwtTree::IsMarkable() const
{
    // Natively, multi selection covers also the check style.
    // Java is responsible to not overselect in SINGLE & CHECK.
    return (iStyle & KSwtStyleMulti) || (iStyle & KSwtStyleCheck);
}

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
void CSwtTree::DoControlSpecificFeedback(
    const TBool& aFirstTap,
    const TBool& aTappedToChildRect,
    const TPointerEvent& aPointerEvent) const
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback && !aTappedToChildRect)
    {
        switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            if (aFirstTap)
            {
                feedback->InstantFeedback(ETouchFeedbackSensitiveList);
            }
            break;
        }
    }
}
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
