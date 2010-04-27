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
#include <AknUtils.h>
#ifdef RD_SCALABLE_UI_V2
#include <aknstyluspopupmenu.h>
#endif //RD_SCALABLE_UI_V2
#include <eswtcore.rsg>
#include <swtlaffacade.h>
#include "swtcontrolhelper.h"
#include "swtmenuarranger.h"
#include "swtmenu.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMenu::CSwtMenu
// ---------------------------------------------------------------------------
//
CSwtMenu::CSwtMenu(MSwtDisplay& aDisplay,
                   TSwtPeer aPeer, TInt aStyle)
        : iDisplay(aDisplay)
        , iPeer(aPeer)
        , iStyle(aStyle)
        , iEnabled(ETrue)
        , iVisible(ETrue)
        , iLocation()
        , iLocationEnabled(EFalse)
{
    // Beware that iPeer may be null if it's the default menubar created by
    // menuarranger (and so don't forget to test iPeer before sending
    // Show Hide events)
}

// ---------------------------------------------------------------------------
// CSwtMenu::~CSwtMenu
// ---------------------------------------------------------------------------
//
CSwtMenu::~CSwtMenu()
{
    // All menu item children should have been disposed (and therefore removed
    // from following array) when menu is deleted
    ASSERT(iItems.Count() == 0);
    iItems.Close();
}

// ---------------------------------------------------------------------------
// CSwtMenu::NewL
// ---------------------------------------------------------------------------
//
CSwtMenu* CSwtMenu::NewL(MSwtDisplay& aDisplay,
                         TSwtPeer aPeer, TInt aStyle)
{
    return new(ELeave) CSwtMenu(aDisplay, aPeer, aStyle);
}

// ---------------------------------------------------------------------------
// CSwtMenu::Dispose
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtMenu::Dispose()
{
    // Notify the MenuArranger
    TRAP_IGNORE(iDisplay.MenuArranger().HandleMenuChangedL(*this,
                MSwtMenuArranger::ESwtMenuRemoved));

    // Actual dispose
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

// ---------------------------------------------------------------------------
// CSwtMenu::JavaPeer
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtMenu::JavaPeer() const
{
    return iPeer;
}

// ---------------------------------------------------------------------------
// CSwtMenu::RegisterChildL
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::RegisterChildL(MSwtMenuItem& aChild, TInt aIndex)
{
    User::LeaveIfError(iItems.Insert(&aChild, aIndex));
}

// ---------------------------------------------------------------------------
// CSwtMenu::UnregisterChild
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::UnregisterChild(MSwtMenuItem& aChild)
{
    TInt indexOfItem = iItems.Find(&aChild);
    iItems.Remove(indexOfItem);
}

// ---------------------------------------------------------------------------
// CSwtMenu::SetEnabled
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::SetEnabled(TBool aEnabled)
{
    if (iEnabled != aEnabled)
    {
        iEnabled = aEnabled;

        if (!iEnabled)
        {
            CleanPane();
        }

        TRAP_IGNORE(iDisplay.MenuArranger().HandleMenuChangedL(*this,
                    MSwtMenuArranger::ESwtMenuEnableStateChanged));
    }
}

// ---------------------------------------------------------------------------
// CSwtMenu::SetVisible
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::SetVisible(TBool aVisible)
{
    TBool changed = iVisible != aVisible;

    iVisible = aVisible;

    // Opening/closing a menu by SetVisible( true/false ) works only on popup menu
    if (!(iStyle & KSwtStylePopUp))
    {
        if (changed)
        {
            if (!iVisible)
            {
                CleanPane();
            }

            TRAP_IGNORE(iDisplay.MenuArranger().HandleMenuChangedL(*this,
                        MSwtMenuArranger::ESwtMenuEnableStateChanged));
        }
        return;
    }

    CSwtMenuArranger& menuArranger = static_cast< CSwtMenuArranger& >(iDisplay.MenuArranger());
    // IsDisplayed does not return the ETrue value if true.
    TBool menuDisplayed = menuArranger.EikMenuBar()->IsDisplayed() ? ETrue : EFalse;
    if (aVisible == menuDisplayed)
    {
        return;
    }

    if (aVisible)
    {
        TRAP_IGNORE(menuArranger.TryDisplayPopupMenuL(*this));
    }
    else
    {
        TRAP_IGNORE(menuArranger.StopDisplayingPopupMenuL(*this));
    }
}

// ---------------------------------------------------------------------------
// CSwtMenu::SetDefaultItem
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::SetDefaultItem(MSwtMenuItem* aMenuItem)
{
    iDefaultMenuItem = aMenuItem;
}

// ---------------------------------------------------------------------------
// CSwtMenu::DefaultItem
// From MSwtMenu
// ---------------------------------------------------------------------------
//
MSwtMenuItem* CSwtMenu::DefaultItem() const
{
    return iDefaultMenuItem;
}

// ---------------------------------------------------------------------------
// CSwtMenu::IsEnabled
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TBool CSwtMenu::IsEnabled() const
{
    return iEnabled;
}

// ---------------------------------------------------------------------------
// CSwtMenu::IsVisible
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TBool CSwtMenu::IsVisible() const
{
    return iVisible;
}

// ---------------------------------------------------------------------------
// CSwtMenu::FillPaneL
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::FillPaneL(CEikMenuPane& aPane) const
{
    ASSERT(Host() == &aPane);
    TInt count(iItems.Count());
    for (TInt i = 0; i < count; i++)
    {
        // Separator items are handled with EEikMenuItemSeparatorAfter set for
        // previous item and therefore are not added to CEikMenuPane (note also
        // that heading separator and consecutive separators are ignored as not
        // platform style)
        if ((iItems[i]->Style() & KSwtStyleSeparator)
                || (!iItems[i]->IsEnabled()))
        {
            continue;
        }

        CEikMenuPaneItem::SData itemData;
        itemData.iText = KNullDesC;
        itemData.iExtraText = KNullDesC;
        itemData.iFlags = 0;
        itemData.iCascadeId = 0;
        itemData.iCommandId = iItems[i]->CommandId();
        aPane.AddMenuItemL(itemData);
        iItems[i]->UpdatePaneL(aPane, MSwtMenuItem::ESwtMenuItemDataAll);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenu::RefreshPaneL
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::RefreshPaneL(CEikMenuPane& aMenuPaneControl) const
{
    // Texts, background, scrollbar and menu indicators are reloaded by Avkon
    // What remains to be processed manually are the images set by Java.

    // Get layout for the menu item additional indication icon

    TRect mainRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainRect);
    TAknLayoutRect iconLayoutRect = CSwtLafFacade::GetLayoutRect(
                                        CSwtLafFacade::EListSinglePaneG1Cp2, mainRect, 0);
    TSize iconLayoutSize(iconLayoutRect.Rect().Size());

    // Go trough the items of the pane, map them to the menu items and
    // if the item had an image set, rescale it to fit the new layout rect.
    TInt paneItemsCount = aMenuPaneControl.NumberOfItemsInPane();
    TInt menuItemsCount = iItems.Count();
    for (TInt i = 0; i < paneItemsCount; i++)
    {
        CEikMenuPaneItem::SData& data = aMenuPaneControl.ItemDataByIndexL(i);
        TInt menuPaneItemPos;
        CEikMenuPaneItem* menuPaneItem = aMenuPaneControl.ItemAndPos(
                                             data.iCommandId, menuPaneItemPos);
        if (!menuPaneItem)
        {
            // Menu pane item not found?
            continue;
        }

        // Map the menu pane item to one of the local menu items.
        // Achieving this by asuming the command ids are unique!!!
        MSwtMenuItem* menuItem = NULL;
        for (TInt j = 0; j < menuItemsCount; j++)
        {
            if (iItems[j]->CommandId() == data.iCommandId)
            {
                // Mapping ok.
                menuItem = iItems[j];
                break;
            }
        }

        if (!menuItem)
        {
            // Could not map menu pane item to local menu item.
            continue;
        }

        // Set image. Image re-scaled.
        MSwtImage* menuItemImage = const_cast<MSwtImage*>(menuItem->Image());
        if (menuItemImage)
        {
            TSize iconRealSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                     menuItemImage->Bitmap().SizeInPixels(), iconLayoutSize);

            CFbsBitmap* bmp = NULL;
            CFbsBitmap* mask = NULL;
            menuItem->GetImageBitmapsL(bmp, mask, iconRealSize);
            if (bmp)
            {
                menuPaneItem->CreateIconL(bmp, mask);   // bmp cannot be NULL here
                menuPaneItem->SetBitmapsOwnedExternally(ETrue);   // Bitmaps deleted by CSwtImage
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtMenu::ProcessCommandL
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::ProcessCommandL(TInt aCommandId, TInt aIdx,
                               TBool aOfferToSubMenus /*= EFalse*/) const
{
    TInt count(iItems.Count());
    TInt i(0);
    aIdx = count;
    for (i = 0; i < count; i++)
    {
        if (iItems[i])
        {
            if (iItems[i]->CommandId() == aCommandId)
            {
                aIdx = i;
                break;
            }
            else if (aOfferToSubMenus && iItems[i]->Menu())
            {
                // aIdx parameter not used in this case, pass KErrNotFound
                iItems[i]->Menu()->ProcessCommandL(aCommandId, KErrNotFound, ETrue);
            }
        }
    }

    if (aIdx < count)
    {
        if (iItems[aIdx]->Style() & KSwtStyleCheck)
        {
            iItems[aIdx]->SetSelection(
                !iItems[aIdx]->GetSelection());
        }

        if (iItems[aIdx]->Style() & KSwtStyleRadio)
        {
            iItems[aIdx]->SetSelection(ETrue);
            for (i = aIdx - 1; i >= 0; i--)
            {
                if (iItems[i] && (iItems[i]->Style() & KSwtStyleRadio))
                {
                    if (iItems[i]->GetSelection())
                    {
                        iItems[i]->SetSelection(EFalse);
                        iDisplay.PostSelectionEventL(iItems[i]->JavaPeer());
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            for (i = aIdx + 1; i < count; i++)
            {
                if (iItems[i] && (iItems[i]->Style() & KSwtStyleRadio))
                {
                    if (iItems[i]->GetSelection())
                    {
                        iItems[i]->SetSelection(EFalse);
                        iDisplay.PostSelectionEventL(iItems[i]->JavaPeer());
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        iDisplay.PostSelectionEventL(iItems[aIdx]->JavaPeer());
    }
}

// ---------------------------------------------------------------------------
// CSwtMenu::CountDisplayableItems
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TInt CSwtMenu::CountDisplayableItems(TBool aStylusPopupMenu /*= EFalse*/) const
{
    // The displayable items are the enabled items since, in S60,
    // the disabled items are not added in the menu panes

    if (!iEnabled || !iVisible)
    {
        // If the whole menu is disabled or invisible, all items are hidden.
        return 0;
    }

    TInt nbEnabledItems(0);
    TInt count(iItems.Count());
    for (TInt i = 0; i < count; i++)
    {
        if (iItems[i] && iItems[i]->IsEnabled())
        {
            // In stylus popup menu case the cascaded menu items are not included in the menu.
            // Also, the sub menu items are calculated because they are shown at once
            // in the stylus popup menu.
            if (aStylusPopupMenu)
            {
                if ((iItems[i]->Style() & KSwtStyleCascade) &&
                        iItems[i]->Menu())
                {
                    nbEnabledItems += iItems[i]->Menu()->CountDisplayableItems();
                }
                else if (!(iItems[i]->Style() & KSwtStyleCascade))
                {
                    nbEnabledItems++;
                }
            }
            else
            {
                nbEnabledItems++;
            }
        }
    }
    return nbEnabledItems;
}

// ---------------------------------------------------------------------------
// CSwtMenu::GetCascadeMenu
// From MSwtMenu
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtMenu::GetCascadeMenu(
    const CEikMenuPaneItem::SData& aItemData) const
{
    TInt count(iItems.Count());
    for (TInt i = 0; i < count; i++)
    {
        if (iItems[i] && iItems[i]->IsEnabled()
                && iItems[i]->CommandId() == aItemData.iCommandId)
        {
            return iItems[i]->Menu();
        }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtMenu::Host
// From MSwtMenu
// ---------------------------------------------------------------------------
//
CEikMenuPane* CSwtMenu::Host() const
{
    return iHost;
}

// ---------------------------------------------------------------------------
// CSwtMenu::SetHost
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::SetHost(CEikMenuPane* aPane)
{
    iHost = aPane;
}

// ---------------------------------------------------------------------------
// CSwtMenu::HasRadioGroup
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TBool CSwtMenu::HasRadioGroup() const
{
    TInt count(iItems.Count());
    for (TInt i = 0; i < count; i++)
    {
        if (iItems[i] && (iItems[i]->IsEnabled())
                && (iItems[i]->Style() & KSwtStyleRadio) != 0)
        {
            return ETrue;
        }
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMenu::SetLocation
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::SetLocation(TInt aX, TInt aY)
{
    if (!iLocationEnabled)
    {
        iLocationEnabled = ETrue;
    }
    iLocation.SetXY(aX, aY);
}

// ---------------------------------------------------------------------------
// CSwtMenu::GetLocation
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TPoint CSwtMenu::GetLocation() const
{
    return iLocation;
}

// ---------------------------------------------------------------------------
// CSwtMenu::LocationEnabled
// From MSwtMenu
// ---------------------------------------------------------------------------
//
TBool CSwtMenu::LocationEnabled() const
{
    return iLocationEnabled;
}

// ---------------------------------------------------------------------------
// CSwtMenu::Items
// From MSwtMenu
// ---------------------------------------------------------------------------
//
const RPointerArray<MSwtMenuItem>* CSwtMenu::Items() const
{
    return &iItems;
}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CSwtMenu::FillPopupL
// From MSwtMenu
// ---------------------------------------------------------------------------
//
void CSwtMenu::FillPopupL(CAknStylusPopUpMenu& aPopupMenu) const
{
    if (!iEnabled || !iVisible)
    {
        return;
    }

    TInt count(iItems.Count());

    for (TInt i = 0; i < count; i++)
    {
        if (!iItems[i]->IsEnabled() || (iItems[i]->Style() & KSwtStyleSeparator))
        {
            continue;
        }

        if (!(iItems[i]->Style() & KSwtStyleCascade))
        {
            aPopupMenu.AddMenuItemL(iItems[i]->Text(), iItems[i]->CommandId());
        }

        // Add sub menu items
        const MSwtMenu* subMenu = iItems[i]->Menu();
        if (subMenu)
        {
            subMenu->FillPopupL(aPopupMenu);
        }

    }
}
#endif //RD_SCALABLE_UI_V2


void CSwtMenu::CleanPane() const
{
    // Remove from visible pane. Enabling a menu to visible pane not supported.
    CEikMenuPane* pane = Host();
    if (pane && pane->IsVisible())
    {
        if (pane->IsCascadeMenuPane())
        {
            // Tell the parent of this pane to close its cascade
            if (iDisplay.MenuArranger().ParentPane(*pane))
            {
                iDisplay.MenuArranger().ParentPane(*pane)->CloseCascadeMenu();
            }
            else
            {
                iDisplay.MenuArranger().StopDisplayingMenuBar();
            }
        }
        else
        {
            // Remove the items from the menu bar
            pane->CloseCascadeMenu();
            pane->SetSelectedItem(0);
            TInt count = iItems.Count();
            for (TInt i = 0; i < count; i++)
            {
                TInt index(KErrNotFound);
                if (pane->MenuItemExists(iItems[i]->CommandId(), index))
                {
                    pane->DeleteMenuItem(iItems[i]->CommandId());
                }
            }
        }
    }
}
