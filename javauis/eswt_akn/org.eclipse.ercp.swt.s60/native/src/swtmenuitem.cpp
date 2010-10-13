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


#include <eikon.hrh>
#include <eikhkeyt.h>
#include <AknDef.h>
#include <eswtcore.rsg>
#include "swtcontrolhelper.h"
#include "swtmenuitem.h"


_LIT(KExtraSpaces, "   ");


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMenuItem::CSwtMenuItem
// ---------------------------------------------------------------------------
//
inline CSwtMenuItem::CSwtMenuItem(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                  MSwtMenu& aParent, TInt aIndex, TInt aStyle, TInt aCommandId)
        : iDisplay(aDisplay)
        , iPeer(aPeer)
        , iParent(aParent)
        , iIndexOfCreation(aIndex)
        , iStyle(aStyle)
        , iEnabled(ETrue)
{
    if (aCommandId != 0)
    {
        iCommandId = aCommandId;
    }
    else
    {
        iCommandId = reinterpret_cast<TInt>(this);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::CSwtMenuItem
// ---------------------------------------------------------------------------
//
CSwtMenuItem::~CSwtMenuItem()
{
    // Own
    delete iText;
    delete iDummyMask;
    iText = NULL;
    iDummyMask = NULL;

    // Not own
    TInt imageSizesCount = iImageSizes.Count();
    for (TInt i = 0; i < imageSizesCount; i++)
    {
        if (iImage)
        {
            iImage->RemoveSubRef(iImageSizes[i]);
        }
    }
    iImageSizes.Close();
    if (iImage)
    {
        iImage->RemoveRef();
    }
    iImage = NULL;
    iMenu = NULL;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::ConstructL()
{
    iParent.RegisterChildL(*this, iIndexOfCreation);

    CEikMenuPane* pane = Pane();
    if (pane)
    {
        CEikMenuPaneItem::SData itemData;
        itemData.iText = KNullDesC;
        itemData.iExtraText = KNullDesC;
        itemData.iFlags = 0;
        itemData.iCascadeId = 0;
        itemData.iCommandId = iCommandId;

        pane->AddMenuItemL(itemData);
        UpdatePaneL(*pane, ESwtMenuItemDataFlags);
        pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }

    iDisplay.MenuArranger().HandleMenuChangedL(iParent,
            MSwtMenuArranger::ESwtMenuItemAdded);
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::NewL
// ---------------------------------------------------------------------------
//
CSwtMenuItem* CSwtMenuItem::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                 MSwtMenu& aParent, TInt aIndex, TInt aStyle, TInt aCommandId /* = 0 */)
{
    CSwtMenuItem* self = new(ELeave) CSwtMenuItem(aDisplay, aPeer, aParent,
            aIndex, aStyle, aCommandId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Dispose
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtMenuItem::Dispose()
{
    iParent.UnregisterChild(*this);

    CleanPane();

    TRAP_IGNORE(iDisplay.MenuArranger().HandleMenuChangedL(iParent,
                MSwtMenuArranger::ESwtMenuItemRemoved));

    // Actual dispose
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::JavaPeer
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtMenuItem::JavaPeer() const
{
    return iPeer;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::GetSelection
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TBool CSwtMenuItem::GetSelection() const
{
    return iSelected;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::SetAcceleratorL
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::SetAcceleratorL(TInt aAccelerator)
{
    iAccelerator = aAccelerator;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::SetEnabled
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::SetEnabled(TBool aEnabled)
{
    if (iEnabled != aEnabled)
    {
        iEnabled = aEnabled;

        CEikMenuPane* pane = Pane();
        if (pane)
        {
            if (iEnabled)
            {
                CEikMenuPaneItem::SData itemData;
                itemData.iText = KNullDesC;
                itemData.iExtraText = KNullDesC;
                itemData.iFlags = 0;
                itemData.iCascadeId = 0;
                itemData.iCommandId = iCommandId;

                TRAP_IGNORE(
                    pane->AddMenuItemL(itemData);
                    UpdatePaneL(*pane, ESwtMenuItemDataAll);
                );
                pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
            }
            else
            {
                CleanPane();
            }
        }

        TRAP_IGNORE(iDisplay.MenuArranger().HandleMenuChangedL(iParent,
                    MSwtMenuArranger::ESwtMenuItemEnableStateChanged));
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::SetImageL
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::SetImageL(const MSwtImage* aImage)
{
    TInt imageSizesCount = iImageSizes.Count();
    for (TInt i = 0; i < imageSizesCount; i++)
    {
        if (iImage)
        {
            iImage->RemoveSubRef(iImageSizes[i]);
        }
    }
    iImageSizes.Close();
    if (iImage)
    {
        iImage->RemoveRef();
    }
    iImage = aImage;
    if (iImage)
    {
        iImage->AddRef();
    }

    CEikMenuPane* pane = Pane();
    if (pane)
    {
        UpdatePaneL(*pane, ESwtMenuItemDataImage);
        pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::SetMenuL
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::SetMenuL(const MSwtMenu* aMenu)
{
    if (iMenu == aMenu)
    {
        return;
    }

    // If item displayed, close cascade menu.
    CEikMenuPane* pane = Pane();
    if (pane)
    {
        pane->CloseCascadeMenu();
    }

    if (iMenu)
    {
        iDisplay.MenuArranger().HandleMenuChangedL(*iMenu,
                MSwtMenuArranger::ESwtMenuRemoved);
    }

    iMenu = aMenu;

    if (iMenu)
    {
        iDisplay.MenuArranger().HandleMenuChangedL(*iMenu,
                MSwtMenuArranger::ESwtMenuAdded);
    }

    // If item displayed, update cascade icon
    if (pane)
    {
        UpdatePaneL(*pane, ESwtMenuItemDataCascade);
        pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::SetSelection
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::SetSelection(TBool aSelected)
{
    iSelected = aSelected;

    CEikMenuPane* pane = Pane();
    if (pane)
    {
        TRAP_IGNORE(UpdatePaneL(*pane, ESwtMenuItemDataSelection));
        pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::SetTextL
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::SetTextL(const TDesC& aText)
{
    if (iText)
    {
        delete iText;
        iText = NULL;
    }
    iText = aText.AllocL();

    CEikMenuPane* pane = Pane();
    if (pane)
    {
        UpdatePaneL(*pane, ESwtMenuItemDataText);
        pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Accelerator
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TInt CSwtMenuItem::Accelerator() const
{
    return iAccelerator;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::IsEnabled
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TBool CSwtMenuItem::IsEnabled() const
{
    return iEnabled;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Image
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
const MSwtImage* CSwtMenuItem::Image() const
{
    return iImage;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::GetImageBitmapsL
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::GetImageBitmapsL(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask,
                                    const TSize& aSize)
{
    aBitmap = NULL;
    aMask = NULL;
    if (!iImage)
    {
        return;
    }

    CFbsBitmap* bmp = const_cast<CFbsBitmap*>(&iImage->SubBitmap(aSize));
    if (bmp)
    {
        if (iImageSizes.Find(aSize) == KErrNotFound)
        {
            iImage->AddSubRef(aSize);
            iImageSizes.Append(aSize);
        }
        aBitmap = bmp;
        aMask = const_cast<CFbsBitmap*>(iImage->SubMaskBitmap(aSize, ETrue));

        if (aMask == NULL)
        {
            if (!iDummyMask)
            {
                // MenuItem drawing does not support images with a NULL mask.
                // So create a 2-bit black image mask with the same size as aSize
                iDummyMask = new(ELeave) CFbsBitmap;
                User::LeaveIfError(iDummyMask->Create(aSize, EGray2));

                CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(iDummyMask);
                CleanupStack::PushL(device);
                CFbsBitGc* gc = CFbsBitGc::NewL();
                CleanupStack::PushL(gc);

                gc->Activate(device);
                gc->SetPenStyle(CGraphicsContext::ENullPen);
                gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
                gc->SetBrushColor(KRgbBlack);
                gc->DrawRect(TRect(aSize));

                CleanupStack::PopAndDestroy(gc);
                CleanupStack::PopAndDestroy(device);
            }
            aMask = iDummyMask;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Menu
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtMenuItem::Menu() const
{
    return iMenu;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Text
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
const TDesC& CSwtMenuItem::Text() const
{
    if (iText)
    {
        return *iText;
    }
    else
    {
        return KNullDesC;
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::CommandId
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TInt CSwtMenuItem::CommandId() const
{
    return iCommandId;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Style
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
TInt CSwtMenuItem::Style() const
{
    return iStyle;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Parent
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
const MSwtMenu& CSwtMenuItem::Parent() const
{
    return iParent;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::UpdatePaneL
// From MSwtMenuItem
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::UpdatePaneL(CEikMenuPane& aPane, TSwtMenuItemData aDetail)
{
    TInt paneItemIndex;
    CEikMenuPaneItem* paneItem(NULL);

    if (!aPane.MenuItemExists(iCommandId, paneItemIndex))
    {
        return;
    }

    paneItem = aPane.ItemAndPos(iCommandId, paneItemIndex);
    if (!paneItem || paneItemIndex == KErrNotFound)
    {
        return;
    }

    CEikMenuPaneItem::SData& data = paneItem->iData;

    // Text
    if (aDetail == ESwtMenuItemDataAll || aDetail == ESwtMenuItemDataText)
    {
        TInt maxLength = data.iText.MaxLength();
        if (Text().Length() <= maxLength)
        {
            data.iText = Text();
        }
        else
        {
            data.iText = Text().Left(maxLength - 1);
        }

        // If push or cascade add some space after text to fix overlapping image
        if (!(Style() & (KSwtStyleCheck | KSwtStyleRadio))
                && aPane.IsCascadeMenuPane()
                && data.iText.Length() > 0)
        {
            TInt diff = data.iText.MaxLength() - data.iText.Length();
            if (diff >= KExtraSpaces().Length())
            {
                data.iText.Append(KExtraSpaces);
            }
        }

        data.iExtraText = KNullDesC;

        if (aDetail != ESwtMenuItemDataAll)
        {
            return;
        }
    }

    // Cascade
    if (aDetail == ESwtMenuItemDataAll || aDetail == ESwtMenuItemDataCascade)
    {
        if (iMenu)
        {
            if (iMenu->HasRadioGroup())
            {
                data.iCascadeId = R_SWT_SUBMENUPANE_WITHRADIOGROUP;
            }
            else
            {
                data.iCascadeId = R_SWT_SUBMENUPANE;
            }
        }
        else
        {
            data.iCascadeId = 0;
        }

        if (aDetail != ESwtMenuItemDataAll)
        {
            return;
        }
    }

    // Selection
    if (aDetail == ESwtMenuItemDataAll || aDetail == ESwtMenuItemDataSelection)
    {
        if (GetSelection())
        {
            aPane.SetItemButtonState(iCommandId, EEikMenuItemSymbolOn);
        }
        else
        {
            aPane.SetItemButtonState(iCommandId, EEikMenuItemSymbolIndeterminate);
        }

        if (aDetail != ESwtMenuItemDataAll)
        {
            return;
        }
    }

    // Flags
    if (aDetail == ESwtMenuItemDataAll || aDetail == ESwtMenuItemDataFlags)
    {
        TInt index = Index();
        if (Style() & KSwtStyleRadio)
        {
            TInt firstRadioIndex = FirstRadioSibbling();
            TInt lastRadioIndex = LastRadioSibbling();
            if (index == firstRadioIndex)
            {
                if (index == firstRadioIndex)
                {
                    data.iFlags |= EEikMenuItemRadioStart;
                }
                else if (index == lastRadioIndex)
                {
                    data.iFlags |= EEikMenuItemRadioEnd;
                }
                else
                {
                    data.iFlags |= EEikMenuItemRadioMiddle;
                }
            }
        }
        else if (Style() & KSwtStyleCheck)
        {
            data.iFlags |= EEikMenuItemCheckBox;
        }

        if (iParent.Items() && index < iParent.Items()->Count() - 1
                && ((*iParent.Items())[index + 1]->Style() & KSwtStyleSeparator))
        {
            data.iFlags |= EEikMenuItemSeparatorAfter;
        }

        if (iParent.DefaultItem() == this)
        {
            aPane.SetSelectedItem(paneItemIndex);
        }

        if (aDetail != ESwtMenuItemDataAll)
        {
            return;
        }
    }

    // Image. Keep it last.
    if ((aDetail == ESwtMenuItemDataAll || aDetail == ESwtMenuItemDataImage))
    {
        if (Image())
        {
            // const cast for CreateIconL but bitmaps set as owned externally
            // CeikMenuPaneItem does not modify the image so that using cast
            // in relation to SetBitmapsOwnedExternally is safe.
            MSwtImage* image = const_cast<MSwtImage*>(Image());
            TSize iconRealSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                     image->Bitmap().SizeInPixels(), iDisplay.MenuArranger().ImageSize());

            CFbsBitmap* bmp = NULL;
            CFbsBitmap* mask = NULL;
            GetImageBitmapsL(bmp, mask, iconRealSize);
            if (bmp)
            {
                paneItem->CreateIconL(bmp, mask);   // bmp cannot be NULL here
                paneItem->SetBitmapsOwnedExternally(ETrue);   // Bitmaps deleted by CSwtImage
            }
        }
        else
        {
            paneItem->SetIcon(NULL);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Pane
// ---------------------------------------------------------------------------
//
CEikMenuPane* CSwtMenuItem::Pane() const
{
    CEikMenuPane* pane = iParent.Host();
    if (pane && pane->IsVisible())
    {
        return pane;
    }
    else
    {
        return NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::FirstRadioSibbling
// ---------------------------------------------------------------------------
//
TInt CSwtMenuItem::FirstRadioSibbling() const
{
    if (iParent.Items())
    {
        const RPointerArray<MSwtMenuItem>& items = *iParent.Items();
        TInt count = items.Count();
        for (TInt i = 0; i < count; i++)
        {
            MSwtMenuItem* item = items[i];
            if (item && item->Style() & KSwtStyleRadio)
            {
                return i;
            }
        }
    }
    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::LastRadioSibbling
// ---------------------------------------------------------------------------
//
TInt CSwtMenuItem::LastRadioSibbling() const
{
    if (iParent.Items())
    {
        const RPointerArray<MSwtMenuItem>& items = *iParent.Items();
        for (TInt i = items.Count() - 1; i >= 0; i--)
        {
            MSwtMenuItem* item = items[i];
            if (item && item->Style() & KSwtStyleRadio)
            {
                return i;
            }
        }
    }
    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::Index
// ---------------------------------------------------------------------------
//
TInt CSwtMenuItem::Index() const
{
    TInt res = KErrNotFound;
    if (iParent.Items())
    {
        res = iParent.Items()->Find(this);
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtMenuItem::CleanPane
// ---------------------------------------------------------------------------
//
void CSwtMenuItem::CleanPane() const
{
    CEikMenuPane* pane = Pane();
    if (pane)
    {
        TInt index(KErrNotFound);
        if (pane->MenuItemExists(iCommandId, index))
        {
            pane->CloseCascadeMenu();
            pane->SetSelectedItem(0);
            pane->DeleteMenuItem(iCommandId);
            pane->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
        }
    }
}
