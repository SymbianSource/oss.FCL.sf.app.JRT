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


#ifndef SWTMENUARRANGER_H
#define SWTMENUARRANGER_H


#include "eswtwidgetscore.h"


class CSwtMenuBar;
#ifdef RD_SCALABLE_UI_V2
class CAknStylusPopUpMenu;
#endif //RD_SCALABLE_UI_V2

/**
 * CSwtMenuArranger
 * @lib eswt
 * 
 * Context menus contain:
 * # Prior to 9.2:
 *      - Specific control menu of focused control (Mark, Unmark, etc.).
 *      - OK & SELECT commands of focused control.
 *      - OK & SELECT commands of all parents of focused control.
 * # Since 9.2:
 *      - No context menus in 9.2.
 * 
 * Stylus popup menus contain:
 * # Prior to 9.2:
 *      - Specific control menu of long tapped control (Mark, Unmark, etc.).
 *      - OK & SELECT commands of long tapped control.
 *      - OK & SELECT commands of all parents of long tapped control.
 *      if the control is also the focused control.
 *      - Popup menu of long tapped control (Control.setMenu()).
 * # Since 9.2:
 *      - Specific control menu of long tapped control (Mark, Unmark, etc.).
 *      - All commands of long tapped control.
 *      - Popup menu of long tapped control (Control.setMenu()).
 * 
 * Options menus contain:
 * # Prior to 9.2:
 *      - Menu bar of active shell.
 *      - Specific control menu of focused control (Mark, Unmark, etc.).
 *      - All commands of all parents of focused control.
 *      - All commands of focused control.
 *      - Popup menu of focused control (Control.setMenu()).
 * # Since 9.2:
 *      - Menu bar of active shell.
 *      - All commands of active shell.
 */
NONSHARABLE_CLASS(CSwtMenuArranger)
        : public CBase
        , public MEikMenuObserver
        , public MSwtMenuArranger
{
public:
    /**
     * Two phase constructor.
     * @param aDisplay Current display.
     * @return Object pointer.
     */
    static CSwtMenuArranger* NewL(MSwtDisplay& aDisplay);

    /**
     * Destructor
     */
    ~CSwtMenuArranger();

private:
    /**
     * First phase constructor.
     * @param aDisplay Current display.
     */
    inline CSwtMenuArranger(MSwtDisplay& aDisplay);

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Compute menu bar.
     */
    void ComputeCurrentMenuBar();

    /**
     * Popup menu getter.
     * @return MSwtMenu*
     */
    const MSwtMenu* GetCurrentPopupMenu() const;

    /**
     * Control menu getter.
     * @return MSwtMenu*
     */
    const MSwtMenu* GetCurrentControlMenu() const;

    /**
     * Active menu bar getter.
     * @return MSwtMenu*
     */
    const MSwtMenu* GetActiveMenuBar() const;

    /**
     * Cascade menu getter.
     * @param aItemData
     * @return MSwtMenu*
     */
    const MSwtMenu* GetCascadeMenu(const CEikMenuPaneItem::SData& aItemData) const;

    /**
     * Focused control getter.
     * @return MSwtControl*
     */
    const MSwtControl* GetFocusedControl() const;

    /**
     * Check if opened menu has radio group.
     * @return true if radio group
     */
    TBool OpenedMenusHaveRadioGroup() const;

    /**
     * Refresh CBA.
     * @param aItemData aCommandArrangerRefreshRequired
     */
    void RefreshCba(TBool aCommandArrangerRefreshRequired = ETrue,
                    TBool aFocusedControlOnly = EFalse);

    /**
     * Helper for handling menu pane visibility changes.
     * The status of the new visibility must be known and passed as parameter.
     * @param aStatus is the real visibility status.
     */
    void HandlePaneVisibilityChangeL(CEikMenuPane* aPane, TBool aStatus);

    /**
     * Resets the list of opened menus, menu panes, etc.
     */
    void ResetOpenedLists();

    /**
     * Updates the images size after resolution change
     */
    void UpdateImageSize();

// From MSwtMenuArranger
public:
    void ControlFocusLost();
    void ControlFocusGained(const MSwtControl& aControl);
    void ActiveShellLost();
    void ActiveShellGained(const MSwtShell& aShell);
    void MenuBarSetOnDecorations();
    void SetCommandMenu(const MSwtMenu* aCommandMenu);
    void SetContextMenu(const MSwtMenu* aCommandMenuOk);
    void HandleMenuChangedL(const MSwtMenu& aMenu, TSwtMenuEvent aType);
    CEikMenuBar* EikMenuBar() const;
    void AddOptionsCommandL();
    void RemoveOptionsCommand();
    void UpdateOptionsCommandL();
    TBool IsOptionsCommandAdded();
    void TryDisplayMenuBarL(TBool aDisplayContextMenu);
    void TryDisplayPopupMenuL(const MSwtMenu& aMenu);
    void StopDisplayingPopupMenuL(const MSwtMenu& aMenu);
    void StopDisplayingMenuBar();
    TBool HasMenuItems() const;
    void HandleResolutionChangeL();
    void HandleWindowVisibilityChangeL(CCoeControl* aDestination);
    TSize ImageSize() const;
    CEikMenuPane* ParentPane(CEikMenuPane& aPane) const;

#ifdef RD_SCALABLE_UI_V2
    void OpenStylusPopupMenuL(const MSwtControl& aControl, const TPoint& aPoint, 
                              MSwtPopupMenuCallBack* aCallBack = NULL);
    void OpenStylusPopupMenuL(const MSwtMenu& aMenu, const TPoint& aPoint);
    void CloseStylusPopupMenuL();
#endif //RD_SCALABLE_UI_V2

// From MEikMenuObserver
public:
    /**
     * Dynamically initialises a menu pane. The framework calls this function
     * immediately before the menu pane is activated. Typically should inquire
     * the value of application data, and initialise menu items accordingly.
     * This includes dimming menu items, changing their text, setting the state
     * of checked items and radio buttons and dynamically adding items to a menu.
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    /**
     * Called by the Uikon framework to handle the emphasising or de-emphasising
     * of a menu window. CEikMenuBar objects call this on their observer to
     * emphasise themselves when they are displayed, and de-emphasise themselves
     * when they stop displaying.
     */
    void SetEmphasis(CCoeControl* aMenuControl, TBool aEmphasis);

// From MEikCommandObserver
public:
    void ProcessCommandL(TInt aCommandId);

// Data
private:
    /**
     * Related display.
     */
    MSwtDisplay& iDisplay;

    /**
     * Current command menu, not owned, may be null.
     */
    const MSwtMenu* iCommandMenu;

    /**
     * Current context menu, not owned, may be null
     */
    const MSwtMenu* iCommandMenuOk;

    /**
     * Current menubar (default menubar or menubar of focused decorations)
     */
    const MSwtMenu* iCurrentMenuBar;

    /**
     * Current menubar when focused decorations has no menubar, owned
     */
    MSwtMenu* iDefaultMenuBar;

    /**
     * Control used to display menubar, owned
     */
    CSwtMenuBar* iMenuBar;

    /**
     * Previous menubar control, not owned, may be null
     */
    CEikMenuBar* iOldEikMenuBar;

    /**
     * Array of opened menus
     */
    RPointerArray<MSwtMenu> iOpenedMenus;

    /**
     * Array of opened menu panes
     */
    RPointerArray<CEikMenuPane> iOpenedMenuPanes;

    /**
     * Keeps track of the visibility of the panes.
     * Needed to avoid a resolution change glitch.
     */
    RArray<TBool> iOpenedMenuPanesVisibility;

    /**
     * The string for menu Options in right soft key
     */
    HBufC* iOptionsString;

    /**
     * Flag for indicating if the "Options" command has been added in CBA
     */
    TBool iOptionsCommandAdded;

    /**
     * Indicates if the menu bar is closing
     */
    TBool iMenuBarIsClosing;

#ifdef RD_SCALABLE_UI_V2
    /**
     * Stylus popup menu
     */
    CAknStylusPopUpMenu* iStylusPopup;

    /**
     * Indicates if the stylus popup menu is currently shown
     */
    TBool iShowingStylusPopup;
#endif //RD_SCALABLE_UI_V2

    /**
     * Cached image size for the panes
     */
    TSize iImageSize;
    
    MSwtPopupMenuCallBack* iPopupMenuCallback;
};


#endif // SWTMENUARRANGER_H
