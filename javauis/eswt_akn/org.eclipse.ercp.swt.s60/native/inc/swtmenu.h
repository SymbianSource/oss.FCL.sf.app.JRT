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


#ifndef SWTMENU_H
#define SWTMENU_H


#include <eikmenub.h>
#include <eikmenup.h>
#include "eswtwidgetscore.h"


/**
 * CSwtMenu
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMenu)
        : public CBase
        , public MSwtMenu
{
// Own methods
public:
    /**
     * Two phase constructor.
     * @param aDisplay
     * @param aPeer
     * @param aStyle
     * @return CSwtMenu*
     */
    static CSwtMenu* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle);

protected:
    /**
     * First phase constructor.
     * @param aDisplay
     * @param aPeer
     * @param aStyle
     */
    CSwtMenu(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle);

    /**
     * Destructor.
     */
    virtual ~CSwtMenu();

// From MSwtMenu
public:
    TSwtPeer Dispose();
    TSwtPeer JavaPeer() const;
    void RegisterChildL(MSwtMenuItem& aChild, TInt aIndex);
    void UnregisterChild(MSwtMenuItem& aChild);
    void SetEnabled(TBool aEnabled);
    void SetVisible(TBool aVisible);
    TBool IsEnabled() const;
    TBool IsVisible() const;
    void FillPaneL(CEikMenuPane& aPane) const;
    void RefreshPaneL(CEikMenuPane& aPane) const;
#ifdef RD_SCALABLE_UI_V2
    void FillPopupL(CAknStylusPopUpMenu& aPopupMenu) const;
#endif //RD_SCALABLE_UI_V2
    void ProcessCommandL(TInt aCommandId, TInt aItemIndex,
                         TBool aOfferToSubMenus = EFalse) const;
    TInt CountDisplayableItems(TBool aStylusPopupMenu = EFalse) const;
    const MSwtMenu* GetCascadeMenu(const CEikMenuPaneItem::SData& aItemData) const;
    TBool HasRadioGroup() const;
    CEikMenuPane* Host() const;
    void SetHost(CEikMenuPane* aPane);
    void SetLocation(TInt aX, TInt aY);
    TPoint GetLocation() const;
    TBool LocationEnabled() const;
    const RPointerArray<MSwtMenuItem>* Items() const;
    void SetDefaultItem(MSwtMenuItem* aMenuItem);
    MSwtMenuItem* DefaultItem() const;

// Own
private:
    /**
     * Removes items from current host pane.
     */
    void CleanPane() const;

// Data
private:
    /**
     * Current display.
     */
    MSwtDisplay& iDisplay;

    /**
     * Java peer.
     */
    const TSwtPeer iPeer;

    /**
     * Menu style.
     */
    TInt iStyle;

    /**
     * Menu state.
     */
    TBool iEnabled;

    /**
     * Visible state.
     */
    TBool iVisible;

    /**
     * Menu items.
     */
    RPointerArray<MSwtMenuItem> iItems;

    /**
     * Default item.
     * Not Own
     */
    MSwtMenuItem* iDefaultMenuItem;

    /**
     * Menu pane on which the menu displays its items. Can be null.
     * Not Own
     */
    mutable CEikMenuPane* iHost;

    /**
     * Location of menu.
     */
    TPoint iLocation;

    /**
     * Location is set or not.
     */
    TBool iLocationEnabled;
};


#endif // SWTMENU_H
