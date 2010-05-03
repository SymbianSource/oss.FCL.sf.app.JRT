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


#include <eikhkeyt.h>
#include <eikon.hrh>
#include <eswtcore.rsg>
#ifdef RD_SCALABLE_UI_V2
#include <aknstyluspopupmenu.h>
#endif //RD_SCALABLE_UI_V2
#include "swtcommandmenu.h"


const TUint KCknEnvClipEndChar=0x2026;


// ======== MEMBER FUNCTIONS ========


inline CSwtCommandMenu::CSwtCommandMenu(MSwtDisplay& aDisplay)
        : iDisplay(aDisplay)
{
}

CSwtCommandMenu::~CSwtCommandMenu()
{
    ASSERT(iItems.Count() == 0);
    iItems.Close();
    ASSERT(iCascadeMenus.Count() == 0);
    iCascadeMenus.Close();
    ASSERT(iSeparators.Count() == 0);
    iSeparators.Close();
}

void CSwtCommandMenu::ConstructL()
{
} //lint !e1762

CSwtCommandMenu* CSwtCommandMenu::NewL(MSwtDisplay& aDisplay)
{
    CSwtCommandMenu* self = new(ELeave) CSwtCommandMenu(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CSwtCommandMenu::Reset()
{
    iItems.Reset();
    iCascadeMenus.Reset();
    iSeparators.Reset();
}

void CSwtCommandMenu::AppendCommand(const MSwtCommand& aCommand,
                                    const MSwtMenu* aCascadeMenu)
{
    // If append fails then item won't appear in menu (and separators might
    // appear at wrong places if later appends succeed). It's a shame but
    // it's like that (nothing specific to do if this happens).
    if (iItems.Append(&aCommand) == KErrNone)
    {
        if (iCascadeMenus.Append(aCascadeMenu) != KErrNone)
        {
            // If append in cascademenus array fails then remove item to avoid
            // arrays not being synchronized
            iItems.Remove(iItems.Count() - 1);
        }
    }
}

void CSwtCommandMenu::InsertCommand(const MSwtCommand& aCommand,
                                    const MSwtMenu* aCascadeMenu, TInt aPos)
{
    // If insert fails then item won't appear in menu (and separators might
    // appear at wrong places if later appends succeed). It's a shame but
    // it's like that (nothing specific to do if this happens).
    if (iItems.Insert(&aCommand, aPos) == KErrNone)
    {
        if (iCascadeMenus.Insert(aCascadeMenu, aPos) != KErrNone)
        {
            // If insert in cascademenus array fails then remove item to avoid
            // arrays not being synchronized
            iItems.Remove(aPos);
        }
    }
}

void CSwtCommandMenu::AppendSeparator()
{
    // If append fails then separator won't appear in menu. It's a shame but
    // it's like that (nothing specific to do if this happens).
    iSeparators.Append(iItems.Count() - 1);
}

const RPointerArray<MSwtCommand>& CSwtCommandMenu::Commands() const
{
    return iItems;
}

TSwtPeer CSwtCommandMenu::Dispose()
{
    // Actual dispose
    delete this;
    return NULL;
}

TSwtPeer CSwtCommandMenu::JavaPeer() const
{
    return NULL;
}

void CSwtCommandMenu::RegisterChildL(MSwtMenuItem& /*aChild*/, TInt /*aIndex*/)
{
    // CommandMenu contains commands instead of menuitems, so this method is not used
}

void CSwtCommandMenu::UnregisterChild(MSwtMenuItem& /*aChild*/)
{
    // CommandMenu contains commands instead of menuitems, so this method is not used
}

void CSwtCommandMenu::SetEnabled(TBool)
{
}

void CSwtCommandMenu::SetVisible(TBool)
{
}

TBool CSwtCommandMenu::IsEnabled() const
{
    return ETrue;
}

TBool CSwtCommandMenu::IsVisible() const
{
    return ETrue;
}

void CSwtCommandMenu::SetDefaultItem(MSwtMenuItem*)
{
}

MSwtMenuItem* CSwtCommandMenu::DefaultItem() const
{
    return NULL;
}

void CSwtCommandMenu::FillPaneL(CEikMenuPane& aMenuPaneControl) const
{
    TInt separatorIndex = 0;
    TInt count = iItems.Count();
    for (TInt itemIndex = 0; itemIndex < count; ++itemIndex)
    {
        CEikMenuPaneItem::SData itemData;
        TInt maxLength = itemData.iText.MaxLength();
        if (iItems[itemIndex]->LongLabel().Length() > 0
                && iItems[itemIndex]->LongLabel().Length() <= maxLength)
        {
            itemData.iText = iItems[itemIndex]->LongLabel();
        }
        else if (iItems[itemIndex]->Text().Length() <= maxLength)
        {
            itemData.iText = iItems[itemIndex]->Text();
        }
        else
        {
            itemData.iText = iItems[itemIndex]->Text().Left(maxLength - 1);
            itemData.iText.Append(KCknEnvClipEndChar);
        }
        itemData.iExtraText = KNullDesC;
        itemData.iFlags = iItems[itemIndex]->IsEnabled() ? 0 : EEikMenuItemDimmed;
        if ((separatorIndex < iSeparators.Count()) && (iSeparators[separatorIndex] == itemIndex))
        {
            itemData.iFlags |= EEikMenuItemSeparatorAfter;
            ++separatorIndex;
        }
        itemData.iCommandId = reinterpret_cast<TInt>(iItems[itemIndex]);
        itemData.iCascadeId = (iCascadeMenus[itemIndex] != NULL) ? R_SWT_SUBMENUPANE : 0;

        aMenuPaneControl.AddMenuItemL(itemData);
    }

    // Register the pane host. See Host().
    iHost = &aMenuPaneControl;
}

void CSwtCommandMenu::ProcessCommandL(TInt aCommandId, TInt aItemIndex,
                                      TBool /*aOfferToSubMenus = EFalse*/) const
{
//  if (reinterpret_cast<TInt>(iItems[aItemIndex]) == aCommandId)
    const MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aCommandId);
    aItemIndex = iItems.Count();
    for (TInt itemIndex = 0; itemIndex < iItems.Count(); ++itemIndex)
    {
        if (command == iItems[itemIndex])
        {
            aItemIndex = itemIndex;
            break;
        }
    }
    if (aItemIndex < iItems.Count())
    {
        iDisplay.PostSelectionEventL(iItems[aItemIndex]->JavaPeer());
    }
}

TInt CSwtCommandMenu::CountDisplayableItems(TBool /*aStylusPopupMenu = EFalse*/) const
{
    // The displayable items are the enabled items
    // since, in S60, the disabled items are not added in the menu panes
    TInt nbDisplayableItems(0);
    for (TInt itemIndex = 0; itemIndex < iItems.Count(); ++itemIndex)
    {
        if (iItems[itemIndex]->IsEnabled())
        {
            nbDisplayableItems++;
        }
    }

    return nbDisplayableItems;
}

const MSwtMenu* CSwtCommandMenu::GetCascadeMenu(const CEikMenuPaneItem::SData& aItemData) const
{
    for (TInt itemIndex = 0; itemIndex < iItems.Count(); ++itemIndex)
    {
        if (iItems[itemIndex]->IsEnabled())
        {
            TInt commandId(REINTERPRET_CAST(TInt, iItems[itemIndex]));
            if (commandId == aItemData.iCommandId)
            {
                return iCascadeMenus[itemIndex];
            }
        }
    }

    return NULL;
}

TBool CSwtCommandMenu::HasRadioGroup() const
{
    return EFalse;
}

CEikMenuPane* CSwtCommandMenu::Host() const
{
    return iHost;
}

void CSwtCommandMenu::SetHost(CEikMenuPane* aPane)
{
    iHost = aPane;
}

void CSwtCommandMenu::SetLocation(TInt /*aX*/, TInt /*aY*/)
{
    // Command menu is not able to change its location
    // This part of code should never be used!
}

TPoint CSwtCommandMenu::GetLocation() const
{
    // Command menu is not able to change its location
    // This part of code should never be used!
    ASSERT(EFalse);
    return TPoint();
}

TBool CSwtCommandMenu::LocationEnabled() const
{
    return EFalse;
}

const RPointerArray<MSwtMenuItem>* CSwtCommandMenu::Items() const
{
    return NULL;
}

#ifdef RD_SCALABLE_UI_V2
void CSwtCommandMenu::FillPopupL(CAknStylusPopUpMenu& aPopupMenu) const
{
    for (TInt i = 0; i < iItems.Count(); ++i)
    {
        if (iItems[i]->IsEnabled())
        {
            if (iItems[i]->LongLabel().Length() > 0)
            {
                aPopupMenu.AddMenuItemL(iItems[i]->LongLabel(),
                                        reinterpret_cast<TInt>(iItems[i]));
            }
            else
            {
                aPopupMenu.AddMenuItemL(iItems[i]->Text(), reinterpret_cast<TInt>(iItems[i]));
            }
        }
    }
}
#endif //RD_SCALABLE_UI_V2


