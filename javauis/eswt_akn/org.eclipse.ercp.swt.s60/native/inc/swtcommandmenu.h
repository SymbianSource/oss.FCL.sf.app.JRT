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


#ifndef SWTCOMMANDMENU_H
#define SWTCOMMANDMENU_H


#include <eikmenub.h>
#include <eikmenup.h>
#include "eswtwidgetscore.h"
#include "eswtmobileextensions.h"


/**
 * CSwtCommandMenu
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCommandMenu)
        : public CBase
        , public MSwtMenu
{
// Methods
public:
    static CSwtCommandMenu* NewL(MSwtDisplay& aDisplay);
    void Reset();
    void AppendCommand(const MSwtCommand& aCommand,
                       const MSwtMenu* aCascadeMenu);
    void AppendSeparatorL();
    void InsertCommand(const MSwtCommand& aCommand,
                       const MSwtMenu* aCascadeMenu, TInt aPos);
    const RPointerArray<MSwtCommand>& Commands() const;

private:
    inline CSwtCommandMenu(MSwtDisplay& aDisplay);
    ~CSwtCommandMenu();
    void ConstructL();

// From MSwtMenu
public:
    TSwtPeer Dispose();
    TSwtPeer JavaPeer() const;
    void  RegisterChildL(MSwtMenuItem& aChild, TInt aIndex);
    void  UnregisterChild(MSwtMenuItem& aChild);
    void  SetEnabled(TBool aEnabled);
    void  SetVisible(TBool aVisible);
    TBool IsEnabled() const;
    TBool IsVisible() const;
    void  FillPaneL(CEikMenuPane& aMenuPaneControl) const;
    void  RefreshPaneL(CEikMenuPane& /*aMenuPaneControl*/) const {};
#ifdef RD_SCALABLE_UI_V2
    void FillPopupL(CAknStylusPopUpMenu& aPopupMenu) const;
#endif //RD_SCALABLE_UI_V2
    void  ProcessCommandL(TInt aCommandId, TInt aItemIndex,
                          TBool aOfferToSubMenus = EFalse) const;
    TInt  CountDisplayableItems(TBool aStylusPopupMenu = EFalse) const;
    const MSwtMenu* GetCascadeMenu(const CEikMenuPaneItem::SData& aItemData) const;
    TBool HasRadioGroup() const;
    CEikMenuPane* Host() const;
    void SetHost(CEikMenuPane* aPane);
    void  SetLocation(TInt aX, TInt aY);
    TPoint GetLocation() const;
    TBool LocationEnabled() const;
    const RPointerArray<MSwtMenuItem>* Items() const;
    void  SetDefaultItem(MSwtMenuItem*);
    MSwtMenuItem* DefaultItem() const;

// Data
private:
    MSwtDisplay&               iDisplay;
    RPointerArray<MSwtCommand> iItems;        // Command items
    RPointerArray<MSwtMenu>    iCascadeMenus; // Cascade menus of menu items
    RArray<TInt>               iSeparators;   // Index of separators between menu items
    TBool iDisabled;

    /**
     * Menu pane on which the menu displays its items. Can be null.
     * Not Own
     */
    mutable CEikMenuPane* iHost;
};


#endif // SWTCOMMANDMENU_H
