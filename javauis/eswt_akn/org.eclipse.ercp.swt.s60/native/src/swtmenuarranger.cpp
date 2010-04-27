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

#include <eikappui.h>
#include <eikmenub.h>
#include <eikhkeyt.h>
#include <eikkeys.h>
#include <avkon.hrh>
#include <gulbordr.h>
#include <aknborders.h>
#ifdef RD_SCALABLE_UI_V2
#include <aknstyluspopupmenu.h>
#endif //RD_SCALABLE_UI_V2

#include <swtlaffacade.h>
#include <eswtcore.rsg>
#include "eswtmobileextensions.h"
#include "eswtcore.hrh"
#include "swtmenu.h"
#include "swtmenubar.h"
#include "swtmenuarranger.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMenuArranger::CSwtMenuArranger
// ---------------------------------------------------------------------------
//
inline CSwtMenuArranger::CSwtMenuArranger(MSwtDisplay& aDisplay)
        : iDisplay(aDisplay)
{
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::~CSwtMenuArranger
// ---------------------------------------------------------------------------
//
CSwtMenuArranger::~CSwtMenuArranger()
{
    // Beware that, currently, menu arranger is deleted before
    // command arranger in CSwtDisplay::PrepareToExit()
    iOpenedMenus.Reset();

    // Cannot use protected delete
    if (iDefaultMenuBar)
    {
        iDefaultMenuBar->Dispose();
        iDefaultMenuBar = NULL;
    }

    if (iStylusPopup)
    {
        TRAP_IGNORE(iStylusPopup->HandleControlEventL(iMenuBar, MCoeControlObserver::EEventRequestExit));
    }

    // Remove our menubar control and restore menubar in its initial state
    if (iMenuBar)
    {
        CEikonEnv* env = iDisplay.CoeEnv();
        ASSERT(env);
        CEikMenuBar* oldMenuBar = env->AppUiFactory()->SwapMenuBar(iOldEikMenuBar);
        ASSERT(EikMenuBar() == oldMenuBar);

        // SetEmphasis mihg get called a second time here if the menu bar to be
        // destroyed is hosting a FEP edit menu for instance. SetEmphasis must
        // not update the stack anymore second time.
        if (EikMenuBar()->IsDisplayed())
        {
            EikMenuBar()->StopDisplayingMenuBar();
        }
        iMenuBar->RemoveFromControlStack();

        delete iMenuBar;
        iMenuBar = NULL;
    }

    iOpenedMenus.Close();
    iOpenedMenuPanes.Close();
    iOpenedMenuPanesVisibility.Close();

    delete iOptionsString;
    iOptionsString = NULL;

#ifdef RD_SCALABLE_UI_V2
    delete iStylusPopup;
#endif //RD_SCALABLE_UI_V2    

    // Not own
    iCommandMenu = NULL;
    iCommandMenuOk = NULL;
    iCurrentMenuBar = NULL;
    iOldEikMenuBar = NULL;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ConstructL()
{
    iOptionsString = iDisplay.CoeEnv()->AllocReadResourceL(
                         R_SWT_SOFTKEY_TEXT_OPTIONS);
    iDefaultMenuBar = CSwtMenu::NewL(iDisplay, NULL, KSwtStyleBar);
    iMenuBar = CSwtMenuBar::NewL(*this);
    CEikonEnv* env = iDisplay.CoeEnv();
    ASSERT(env);
    iOldEikMenuBar = env->AppUiFactory()->SwapMenuBar(EikMenuBar());
    if (iOldEikMenuBar)
    {
        if (iOldEikMenuBar->IsVisible())
        {
            iOldEikMenuBar->StopDisplayingMenuBar();
        }
    }

    // WARNING: Do not add to control stack the CEikMenuBar directly.
    // This is the correct way but there is no way of enforcing it.
    iMenuBar->AddToControlStackL();

    // Current menubar is set after AddToStackL so that
    // it stays null if AddToStackL Leaves
    iCurrentMenuBar = iDefaultMenuBar;

    // Set cached image size
    UpdateImageSize();
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::NewL
// ---------------------------------------------------------------------------
//
CSwtMenuArranger* CSwtMenuArranger::NewL(MSwtDisplay& aDisplay)
{
    CSwtMenuArranger* self = new(ELeave) CSwtMenuArranger(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::GetFocusedControl
// ---------------------------------------------------------------------------
//
const MSwtControl* CSwtMenuArranger::GetFocusedControl() const
{
    const MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
    if (activeShell)
    {
        return activeShell->FocusControl();
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::GetActiveMenuBar
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtMenuArranger::GetActiveMenuBar() const
{
    const MSwtControl* focusedControl = GetFocusedControl();
    if (focusedControl && focusedControl->IsDecorations())
    {
        return const_cast<MSwtControl*>(focusedControl)->DecorationsInterface()->MenuBar();
    }
    else
    {
        MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
        if (activeShell)
            return activeShell->Decorations()->MenuBar();
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::GetCascadeMenu
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtMenuArranger::GetCascadeMenu(
    const CEikMenuPaneItem::SData& aItemData) const
{
    const MSwtMenu* cascadeMenu = NULL;
    TInt count = iOpenedMenus.Count() - 1;
    for (TInt i = count; i >= 0; i--)
    {
        cascadeMenu = iOpenedMenus[i]->GetCascadeMenu(aItemData);
        if (cascadeMenu)
        {
            break;
        }
    }
    return cascadeMenu;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ComputeCurrentMenuBar
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ComputeCurrentMenuBar()
{
    iCurrentMenuBar = GetActiveMenuBar();
    if (!iCurrentMenuBar)
    {
        iCurrentMenuBar = iDefaultMenuBar;
    }

    ASSERT(iCurrentMenuBar);
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::GetCurrentPopupMenu
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtMenuArranger::GetCurrentPopupMenu() const
{
    const MSwtControl* focusedControl = GetFocusedControl();
    if (!focusedControl)
    {
        MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
        if (activeShell)
            focusedControl = activeShell->Control();
    }

    return focusedControl ? focusedControl->GetPopupMenu() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::GetCurrentControlMenu
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtMenuArranger::GetCurrentControlMenu() const
{
    const MSwtControl* focusedCtrl = GetFocusedControl();
    if (focusedCtrl)
    {
        return focusedCtrl->GetControlMenu();
    }
    else
    {
        return NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::OpenedMenusHaveRadioGroup
// ---------------------------------------------------------------------------
//
TBool CSwtMenuArranger::OpenedMenusHaveRadioGroup() const
{
    TInt count = iOpenedMenus.Count();
    for (TInt i = 0; i < count; i++)
    {
        const MSwtMenu* menu = iOpenedMenus[i];
        if (menu && menu->HasRadioGroup())
        {
            return ETrue;
        }
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::RefreshCba
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::RefreshCba(TBool aCommandArrangerRefreshRequired,
                                  TBool aFocusedControlOnly)
{
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    if (commandArranger)
    {
        if (aCommandArrangerRefreshRequired)
        {
            commandArranger->RefreshCba(aFocusedControlOnly);
        }
    }
    else
    {
        TRAP_IGNORE(UpdateOptionsCommandL());
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::HandlePaneVisibilityChangeL
// Do not use CEikMenuPane::IsVisible() inside this function!
// - Deregisters menu panes as they become invisible
// - Removes menus of the panes becoming invisible
// - Posts Show, Hide menu events for the menus contained by the changing pane.
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::HandlePaneVisibilityChangeL(CEikMenuPane* aPane, TBool aStatus)
{
    CEikMenuPane* pane(NULL);
    TInt paneIndex = iOpenedMenuPanes.Find(aPane);
    if (paneIndex == KErrNotFound)
    {
        return;
    }

    // Resolution change causes the cascade panes to get window shown event again.
    if (iOpenedMenuPanesVisibility[paneIndex] == aStatus)
    {
        return;
    }
    else
    {
        iOpenedMenuPanesVisibility[paneIndex] = aStatus;
    }

    pane = iOpenedMenuPanes[paneIndex];

    TInt compress(EFalse);
    for (TInt i = iOpenedMenus.Count() - 1; i >= 0; i--)
    {
        MSwtMenu* menu = iOpenedMenus[i];

        if (menu && menu->Host() == pane)
        {
            if (aStatus)
            {
                if (menu->JavaPeer())   // CommandMenu has no peer
                {
                    iDisplay.PostShowEventL(menu->JavaPeer());
                }
            }
            else
            {
                if (menu->JavaPeer())   // CommandMenu has no peer
                {
                    iDisplay.PostHideEventL(menu->JavaPeer());
                }
                menu->SetHost(NULL);
                iOpenedMenus.Remove(i);
                compress = ETrue;
            }
        }
    }

    if (compress)
    {
        iOpenedMenus.Compress();
    }

    // Deregister pane.
    if (!aStatus)
    {
        iOpenedMenuPanes.Remove(paneIndex);
        iOpenedMenuPanesVisibility.Remove(paneIndex);
        iOpenedMenuPanes.Compress();
        iOpenedMenuPanesVisibility.Compress();
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ResetOpenedLists
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ResetOpenedLists()
{
    TInt count = iOpenedMenus.Count();
    for (TInt i = 0; i < count; i++)
    {
        if (iOpenedMenus[i])
        {
            iOpenedMenus[i]->SetHost(NULL);
        }
    }

    iOpenedMenus.Reset();
    iOpenedMenuPanes.Reset();
    iOpenedMenuPanesVisibility.Reset();
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::UpdateImageSize
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::UpdateImageSize()
{
    TRect mainRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainRect);
    TAknLayoutRect iconLayoutRect = CSwtLafFacade::GetLayoutRect(
                                        CSwtLafFacade::EListSinglePaneG1Cp2, mainRect, 0);
    iImageSize = iconLayoutRect.Rect().Size();
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ControlFocusLost
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ControlFocusLost()
{
    // If the control loses focus we do not close the menu in case it is open.
    // The menu will be closed by one of the focus or activity gained functions.
    // The reason for not closing the menu is that when the menu is opened,
    // the focused control will lose focus.
    if (!EikMenuBar()->IsDisplayed())
    {
        // Swap menu bar with active shell's one
        ComputeCurrentMenuBar();
        RefreshCba(EFalse);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ControlFocusGained
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ControlFocusGained(const MSwtControl& aControl)
{
    // If the shell owning aControl (the control whose focus changed)
    // is the active shell, the menu bar should be updated.
    if (&(aControl.GetShell()) == iDisplay.UiUtils().GetActiveShell())
    {
        // If there's a focus change while displaying menus
        // (means focus change is done programatically)
        // then displayed menus are invalid (popup + commands)
        // => stop displaying them
        if (EikMenuBar()->IsDisplayed())
        {
            StopDisplayingMenuBar();
        }

        // Swap menu bar with active shell's one
        ComputeCurrentMenuBar();
        RefreshCba(EFalse);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ActiveShellLost
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ActiveShellLost()
{
    // If there's a focus change while displaying menus
    // (means focus change is done programatically)
    // then displayed menus are invalid (popup + commands)
    // => stop displaying them
    if (EikMenuBar()->IsDisplayed())
    {
        StopDisplayingMenuBar();
    }

    ComputeCurrentMenuBar();
    RefreshCba(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ActiveShellGained
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ActiveShellGained(const MSwtShell& /*aShell*/)
{
    // Loss of previously active shell (if any) must have been processed already
    // One exception: when a shell is disposed, it doesn't notify its activity
    // loss

    // If there's a focus change while displaying menus
    // (means focus change is done programatically)
    // then displayed menus are invalid (popup + commands)
    // => stop displaying them
    if (EikMenuBar()->IsDisplayed())
    {
        StopDisplayingMenuBar();
    }

    ComputeCurrentMenuBar();
    RefreshCba(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::MenuBarSetOnDecorations
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::MenuBarSetOnDecorations()
{
    const MSwtMenu* menuBar = GetActiveMenuBar();

    // Check if it is necessary to recompute the menu bar
    if (((menuBar) && (iCurrentMenuBar != menuBar))
            || ((!menuBar) && (iCurrentMenuBar != iDefaultMenuBar)))
    {
        if (EikMenuBar()->IsDisplayed())
        {
            StopDisplayingMenuBar();
        }
        else if (!menuBar)
        {
            // If the menubar got removed, reset internal state.
            StopDisplayingMenuBar();
        }

        ComputeCurrentMenuBar();
        RefreshCba();
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::SetCommandMenu
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::SetCommandMenu(const MSwtMenu* aCommandMenu)
{
    iCommandMenu = aCommandMenu;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::SetContextMenu
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::SetContextMenu(const MSwtMenu* aContextMenu)
{
    iCommandMenuOk = aContextMenu;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::HandleMenuChangedL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::HandleMenuChangedL(const MSwtMenu& aMenu, TSwtMenuEvent aType)
{
    if (aType == ESwtMenuRemoved
            || (aType == ESwtMenuEnableStateChanged
                && !(aMenu.IsEnabled() && aMenu.IsVisible())))
    {
        // Remove menu from the list of opened menus
        TInt index = iOpenedMenus.Find(&aMenu);
        if (index != KErrNotFound)
        {
            (const_cast<MSwtMenu*>(&aMenu))->SetHost(NULL);
            iOpenedMenus.Remove(index);
        }
    }

    if (!EikMenuBar()->IsDisplayed())
    {
        RefreshCba();
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::MenuBar
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
CEikMenuBar* CSwtMenuArranger::EikMenuBar() const
{
    ASSERT(iMenuBar);
    return iMenuBar->EikMenuBar();
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::AddOptionsCommandL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::AddOptionsCommandL()
{
    ASSERT(!iOptionsCommandAdded);

    CEikButtonGroupContainer& cba = iDisplay.UiUtils().Cba();
    ASSERT(cba.PositionById(KSwtCommandBaseId) == KErrNotFound);
    cba.UpdateCommandObserverL(0, *this);
    TRAPD(err, cba.AddCommandToStackL(0, EAknSoftkeyOptions,
                                      iOptionsString->Des()));
    if (err != KErrNone)
    {
        cba.RemoveCommandObserver(0);
        User::Leave(err);
    }
    iOptionsCommandAdded = ETrue;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::RemoveOptionsCommand
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::RemoveOptionsCommand()
{
    ASSERT(iOptionsCommandAdded);

    CEikButtonGroupContainer& cba = iDisplay.UiUtils().Cba();
    ASSERT(cba.PositionById(EAknSoftkeyOptions) != KErrNotFound);

    // Remove our command observer as it's still there
    cba.RemoveCommandObserver(0);
    // Remove our command from stack (even if someone else push another
    // command on top of it)
    cba.RemoveCommandFromStack(0, EAknSoftkeyOptions);
    iOptionsCommandAdded = EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::UpdateOptionsCommandL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::UpdateOptionsCommandL()
{
    ASSERT(iCurrentMenuBar);
    ASSERT(!iDisplay.CommandArranger());

    TBool oldState = iOptionsCommandAdded;
    TBool optionsCmdNeeded = HasMenuItems();

    // Add or remove options command if needed
    if (optionsCmdNeeded && !iOptionsCommandAdded)
    {
        AddOptionsCommandL();
    }
    else if (!optionsCmdNeeded && iOptionsCommandAdded)
    {
        RemoveOptionsCommand();
    }
    if (oldState != iOptionsCommandAdded)
    {
        iDisplay.UiUtils().Cba().DrawNow();
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::IsOptionsCommandAdded
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
TBool CSwtMenuArranger::IsOptionsCommandAdded()
{
    return iOptionsCommandAdded;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::TryDisplayMenuBarL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::TryDisplayMenuBarL(TBool aDisplayContextMenu)
{
    ASSERT(!EikMenuBar()->IsDisplayed());

    ResetOpenedLists();

    if (aDisplayContextMenu)
    {
        if (iCommandMenuOk)
        {
            User::LeaveIfError(iOpenedMenus.Append(iCommandMenuOk));
        }
        const MSwtMenu* currentControlMenu = GetCurrentControlMenu();
        if (currentControlMenu)
        {
            User::LeaveIfError(iOpenedMenus.Append(currentControlMenu));
        }
        iMenuBar->TryDisplayMenuBarL(aDisplayContextMenu);
    }
    else
    {
        if (iCommandMenu)
        {
            User::LeaveIfError(iOpenedMenus.Append(iCommandMenu));
        }

#ifndef RD_JAVA_S60_RELEASE_9_2
        const MSwtMenu* currentPopupMenu = GetCurrentPopupMenu();
        if (currentPopupMenu)
        {
            User::LeaveIfError(iOpenedMenus.Append(currentPopupMenu));
        }
#endif // RD_JAVA_S60_RELEASE_9_2

        ASSERT(iCurrentMenuBar);
        User::LeaveIfError(iOpenedMenus.Append(iCurrentMenuBar));

#ifndef RD_JAVA_S60_RELEASE_9_2
        const MSwtMenu* currentControlMenu = GetCurrentControlMenu();
        if (currentControlMenu)
        {
            User::LeaveIfError(iOpenedMenus.Append(currentControlMenu));
        }
#endif // RD_JAVA_S60_RELEASE_9_2

        iMenuBar->TryDisplayMenuBarL(aDisplayContextMenu, OpenedMenusHaveRadioGroup());
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::TryDisplayPopupMenuL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::TryDisplayPopupMenuL(const MSwtMenu& aMenu)
{
    ASSERT(!EikMenuBar()->IsDisplayed());

#ifdef RD_SCALABLE_UI_V2

    TPoint pos(0, 0);
    if (aMenu.LocationEnabled())
    {
        pos = aMenu.GetLocation();
        // if RD_SCALABLE_UI_V2 is available, open stylus popup menu (same as
        // a long tap popup menu)
        OpenStylusPopupMenuL(aMenu, pos);
    }
    else
    {
        const MSwtControl* ctrl = GetFocusedControl();
        if (ctrl)
        {
            pos = ctrl->GetLocation();
            TSize size(ctrl->GetWidgetSize());
            pos.iX = pos.iX + size.iWidth / 2;
            pos.iY = pos.iY + size.iHeight / 2;
            // if RD_SCALABLE_UI_V2 is available, open stylus popup menu (same as
            // a long tap popup menu)
            OpenStylusPopupMenuL(aMenu, pos);
        }
    }

#else //RD_SCALABLE_UI_V2

    RefreshCba(ETrue, ETrue);
    TryDisplayMenuBarL(EFalse);

#endif //RD_SCALABLE_UI_V2 
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::StopDisplayingPopuMenuBar
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::StopDisplayingPopupMenuL(const MSwtMenu& aMenu)
{
    ASSERT(EikMenuBar()->IsDisplayed());

    TBool find(EFalse);

    for (TInt i = iOpenedMenus.Count() -1; i >= 0; i--)
    {
        if (iOpenedMenus[i] == &aMenu)
        {
            find = ETrue;
        }
    }
    if (!find)
    {
        return;
    }

    StopDisplayingMenuBar();
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::StopDisplayingMenuBar
// From MSwtMenuArranger
// The list of opened menus and panes are reset in HandlePaneVisibilityChangeL
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::StopDisplayingMenuBar()
{
    if (iMenuBarIsClosing)
    {
        return;
    }

    iMenuBarIsClosing = ETrue;
    iCommandMenuOk = NULL;
    if (EikMenuBar()->IsDisplayed())
    {
        EikMenuBar()->StopDisplayingMenuBar();
    }
    EikMenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
    iMenuBarIsClosing = EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::HasMenuItems
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
TBool CSwtMenuArranger::HasMenuItems() const
{
    ASSERT(iCurrentMenuBar);

    if (iCurrentMenuBar->CountDisplayableItems() > 0)
    {
        return ETrue;
    }

#ifndef RD_JAVA_S60_RELEASE_9_2
    const MSwtMenu* currentPopupMenu = GetCurrentPopupMenu();
    if (currentPopupMenu
            && currentPopupMenu->CountDisplayableItems() > 0)
    {
        return ETrue;
    }

    const MSwtMenu* currentControlMenu = GetCurrentControlMenu();
    if (currentControlMenu
            && currentControlMenu->CountDisplayableItems() > 0)
    {
        return ETrue;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::HandleResolutionChangeL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::HandleResolutionChangeL()
{
    // Refresh cached image size
    UpdateImageSize();

    // Go trough the opened panes and inform the menus which have items
    // attached to those panes to reload their resources. For instance
    // the images of the menu items will have to be rescaled.
    TInt count = iOpenedMenus.Count();
    for (TInt i = 0; i < count; i++)
    {
        const MSwtMenu* menu = iOpenedMenus[i];
        if (menu)
        {
            CEikMenuPane* menuPane = menu->Host();
            if (menuPane && menuPane->IsVisible())
            {
                menu->RefreshPaneL(*menuPane);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::HandleWindowVisibilityChangeL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::HandleWindowVisibilityChangeL(CCoeControl* aDestination)
{
    // Search for pane
    CEikMenuPane* destination = reinterpret_cast<CEikMenuPane*>(aDestination);
    if (destination)
    {
        HandlePaneVisibilityChangeL(destination, destination->IsVisible());
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ImageSize
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
TSize CSwtMenuArranger::ImageSize() const
{
    return iImageSize;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ParentPane
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
CEikMenuPane* CSwtMenuArranger::ParentPane(CEikMenuPane& aPane) const
{
    TInt index = iOpenedMenuPanes.Find(&aPane);
    if (index != KErrNotFound && index > 0)
    {
        return iOpenedMenuPanes[index - 1];
    }
    else
    {
        return NULL;
    }
}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CSwtMenuArranger::OpenStylusPopupMenuL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::OpenStylusPopupMenuL(const MSwtControl& aControl,
        const TPoint& aPoint, MSwtPopupMenuCallBack* aCallBack /* = NULL*/)
{
    ASSERT(!EikMenuBar()->IsDisplayed());

    ASSERT(iDisplay.CommandArranger());

    // Recreate stylus popup because it does not have a method for clearing the items.
    if (iStylusPopup)
    {
        delete iStylusPopup;
    }
    iStylusPopup = NULL;
    iStylusPopup = CAknStylusPopUpMenu::NewL(this, aPoint);

    iPopupMenuCallback = aCallBack;

    ResetOpenedLists();

    TBool hasMenuItems = EFalse;

    // Register the menus
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (!aControl.IsShell())
    {
#endif // RD_JAVA_S60_RELEASE_9_2
        const MSwtMenu* stylusPopupCommandsMenu =
            iDisplay.CommandArranger()->GetStylusPopupCommandsMenu(aControl);
        if (stylusPopupCommandsMenu && stylusPopupCommandsMenu->CountDisplayableItems(ETrue) > 0)
        {
            hasMenuItems = ETrue;
            stylusPopupCommandsMenu->FillPopupL(*iStylusPopup);
            User::LeaveIfError(iOpenedMenus.Append(stylusPopupCommandsMenu));
        }
#ifdef RD_JAVA_S60_RELEASE_9_2
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    const MSwtMenu* popupMenu = aControl.GetPopupMenu();
    if (popupMenu && popupMenu->CountDisplayableItems(ETrue) > 0)
    {
        hasMenuItems = ETrue;
        popupMenu->FillPopupL(*iStylusPopup);
        User::LeaveIfError(iOpenedMenus.Append(popupMenu));
    }

    const MSwtMenu* controlMenu = aControl.GetStylusPopupControlMenu();
    if (controlMenu && controlMenu->CountDisplayableItems(ETrue) > 0)
    {
        hasMenuItems = ETrue;
        controlMenu->FillPopupL(*iStylusPopup);
        User::LeaveIfError(iOpenedMenus.Append(controlMenu));
    }

    if (!hasMenuItems)
    {
        return;
    }

    // Show the popup
    iStylusPopup->SetPosition(aPoint, CAknStylusPopUpMenu::EPositionTypeLeftBottom);
    iStylusPopup->ShowMenu();

    // Post menu events
    for (TInt i = iOpenedMenus.Count() - 1; i >= 0; i--)
    {
        MSwtMenu* menu = iOpenedMenus[i];
        if (menu && menu->JavaPeer())   // CommandMenu has no peer
        {
            iDisplay.PostShowEventL(menu->JavaPeer());
        }
    }

    iShowingStylusPopup = ETrue;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::OpenStylusPopupMenuL
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::OpenStylusPopupMenuL(const MSwtMenu& aMenu,
        const TPoint& aPoint)
{
    ASSERT(!EikMenuBar()->IsDisplayed());

    // Recreate stylus popup because it does not have a method for clearing the items.
    if (iStylusPopup)
    {
        delete iStylusPopup;
    }
    iStylusPopup = NULL;
    iStylusPopup = CAknStylusPopUpMenu::NewL(this, aPoint);

    ResetOpenedLists();

    aMenu.FillPopupL(*iStylusPopup);
    User::LeaveIfError(iOpenedMenus.Append(&aMenu));

    // Show the popup
    iStylusPopup->SetPosition(aPoint);
    iStylusPopup->ShowMenu();

    // Post menu events
    for (TInt i = iOpenedMenus.Count() - 1; i >= 0; i--)
    {
        MSwtMenu* menu = iOpenedMenus[i];
        if (menu && menu->JavaPeer())   // CommandMenu has no peer
        {
            iDisplay.PostShowEventL(menu->JavaPeer());
        }
    }

    iShowingStylusPopup = ETrue;
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::CloseStylusPopupMenu()
// From MSwtMenuArranger
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::CloseStylusPopupMenuL()
{
    if (!iShowingStylusPopup)
    {
        return;
    }

    // Post menu events and deregister opened menus
    for (TInt i = iOpenedMenus.Count() - 1; i >= 0; i--)
    {
        MSwtMenu* menu = iOpenedMenus[i];
        if (menu && menu->JavaPeer())   // CommandMenu has no peer
        {
            iDisplay.PostHideEventL(menu->JavaPeer());
        }
    }
    iOpenedMenus.Reset();

    // Cannot delete the stylus popup here due to animation timers.
    iShowingStylusPopup = EFalse;

    if (iPopupMenuCallback)
    {
        iPopupMenuCallback->HandlePopupMenuClosedL();
        iPopupMenuCallback = NULL;
    }
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtMenuArranger::DynInitMenuPaneL
// From MEikMenuObserver
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
    // Null safe check
    if (!aMenuPane)
    {
        return;
    }

    // This dummy item is present in the resource only to enable the radio group
    // functionnality. So now, it is removed from menu pane.
    if (aResourceId == R_SWT_OPTIONSMENUPANE_WITHRADIOGROUP
            || aResourceId == R_SWT_CONTEXTMENUPANE_WITHRADIOGROUP
            || aResourceId == R_SWT_SUBMENUPANE_WITHRADIOGROUP)
    {
        aMenuPane->DeleteMenuItem(ESwtDummyMenuItem);
    }

    // Options or Context Menu
    if (aResourceId == R_SWT_OPTIONSMENUPANE
            || aResourceId == R_SWT_CONTEXTMENUPANE
            || aResourceId == R_SWT_OPTIONSMENUPANE_WITHRADIOGROUP
            || aResourceId == R_SWT_CONTEXTMENUPANE_WITHRADIOGROUP)
    {
        // Register this pane in the opened panes but pay attention that the pane
        // is not actually visible yet. It will get visible after this functions
        // returns.
        User::LeaveIfError(iOpenedMenuPanes.Append(aMenuPane));
        User::LeaveIfError(iOpenedMenuPanesVisibility.Append(EFalse));

        // The main menu pane does not deliver window hidden change notifications
        // so there is no point in calling EnableVisibilityChangeEvents.
        // However SetEmphasized is called every time the main pane opens or closes.

        // Fill menu pane with command items
        TInt count = iOpenedMenus.Count();
        for (TInt i = 0; i < count; i++)
        {
            if (iOpenedMenus[i])
            {
                iOpenedMenus[i]->SetHost(aMenuPane);
                iOpenedMenus[i]->FillPaneL(*aMenuPane);
            }
        }
    }
    // Cascade Menu
    else if (aResourceId == R_SWT_SUBMENUPANE
             || aResourceId == R_SWT_SUBMENUPANE_WITHRADIOGROUP)
    {
        // Safe check
        ASSERT(iOpenedMenuPanes.Count() >= 1);
        CEikMenuPane* owner = iOpenedMenuPanes[iOpenedMenuPanes.Count() - 1];
        ASSERT(owner->CascadeMenuPane() == aMenuPane);

        // Register this pane in the opened pane list.
        User::LeaveIfError(iOpenedMenuPanes.Append(aMenuPane));
        User::LeaveIfError(iOpenedMenuPanesVisibility.Append(EFalse));

        // This enables us to be notified when the subpane opens and closes.
        aMenuPane->DrawableWindow()->EnableVisibilityChangeEvents();

        // Fill the menu, and register it in the opened menu list.
        CEikMenuPaneItem::SData& itemData = owner->ItemDataByIndexL(owner->SelectedItem());
        const MSwtMenu* cascadeMenu = GetCascadeMenu(itemData);
        if (cascadeMenu)
        {
            User::LeaveIfError(iOpenedMenus.Append(cascadeMenu));
            (const_cast<MSwtMenu*>(cascadeMenu))->SetHost(aMenuPane);
            cascadeMenu->FillPaneL(*aMenuPane);
        }
    }
    else
    {
        MEikMenuObserver::DynInitMenuPaneL(aResourceId, aMenuPane);
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::SetEmphasis
// From MEikMenuObserver
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::SetEmphasis(CCoeControl* aMenuControl, TBool aEmphasis)
{
    CEikAppUi* appUi = iDisplay.CoeEnv()->EikAppUi();

    // If control to emphasize is the menubar then emphasize the CSwtMenuBar
    // as it is it that was added to stack
    CCoeControl* emphasizedControl;
    TBool isMenuBarStacked = ETrue;
    if (iMenuBar && aMenuControl == EikMenuBar())
    {
        if (!iMenuBar->IsOnControlStack())
        {
            isMenuBarStacked = EFalse;
        }
        emphasizedControl = iMenuBar;
    }
    else
    {
        emphasizedControl = aMenuControl;
    }

    // find focused control
    MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
    MSwtControl* focusedControl(NULL);
    if (activeShell)
    {
        focusedControl = activeShell->FocusControl();
    }

    if (aEmphasis)
    {
        if (focusedControl)
        {
            // if there is a focused control, at least remove it from control stack
            // Otherwise, if the control is a text editor, feb manager  is always aware
            // of this control even after menu being opened.
            // focusedControl->RemoveFromControlStack();
            appUi->RemoveFromStack(&focusedControl->CoeControl());
        }
        if (isMenuBarStacked)
        {
            appUi->UpdateStackedControlFlags(emphasizedControl,
                                             ECoeStackPriorityMenu, ECoeStackFlagRefusesFocus);
        }
    }
    else
    {
        if (focusedControl)
        {
            // Add focused conrol to control stack again
            // focusedControl->AddToControlStackL( ECoeStackPriorityDefault );
            TRAP_IGNORE(appUi->AddToStackL(&focusedControl->CoeControl(),
                                           ECoeStackPriorityDefault));
        }
        if (isMenuBarStacked)
        {
            appUi->UpdateStackedControlFlags(emphasizedControl,
                                             ECoeStackFlagRefusesFocus, ECoeStackFlagRefusesFocus);
        }
    }

    // Main menu pane does not deliver window visibility change notifications.
    if (iMenuBar && aMenuControl == EikMenuBar())
    {
        TRAP_IGNORE(HandlePaneVisibilityChangeL(EikMenuBar()->MenuPane(), aEmphasis));
    }
}

// ---------------------------------------------------------------------------
// CSwtMenuArranger::ProcessCommandL
// From MEikCommandObserver
// Delivered by:
// - CEikButtonGroupContainer::ProcessCommandL (activating softkeys)
// - CEikMenuPane::ReportSelectionMadeL (activating menu items)
// - CEikMenuPane::ReportCanceled (clicking outside menu)
// - CAknStylusPopUpMenu::HandleControlEventL (stylus popup submit, cancel)
// ---------------------------------------------------------------------------
//
void CSwtMenuArranger::ProcessCommandL(TInt aCommandId)
{
    if (aCommandId == EAknSoftkeyOptions
            && IsOptionsCommandAdded()
            && !EikMenuBar()->IsDisplayed())
    {
        TryDisplayMenuBarL(EFalse);
    }
    else if (EikMenuBar()->IsDisplayed())
    {
        // iMenus may be reseted during processing of the command,
        // need to check the count on each loop iteration.
        for (TInt i = 0; i < iOpenedMenus.Count(); i++)
        {
            if (iOpenedMenus[i])
            {
                iOpenedMenus[i]->ProcessCommandL(aCommandId, EikMenuBar()->SelectedItem());
            }
        }
        StopDisplayingMenuBar();
    }
#ifdef RD_SCALABLE_UI_V2
    else if (iShowingStylusPopup)
    {
        // Backwords just to be safe. The open menu can close while
        // handling the commands hence the extra check for iShowingStylusPopup.
        for (TInt i = iOpenedMenus.Count() - 1; iShowingStylusPopup && i >= 0; i--)
        {
            MSwtMenu* menu = iOpenedMenus[i];
            if (menu)
            {
                // Pass KErrNotFound as item index, it is not used in this case
                menu->ProcessCommandL(aCommandId, KErrNotFound, ETrue);
            }
        }
        CloseStylusPopupMenuL();
    }
#endif //RD_SCALABLE_UI_V2            
}
