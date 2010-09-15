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

#include <aknenv.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include "swtcommandarranger.h"
#include "swtcommandcontainer.h"
#include "swtcommandmenu.h"


static const TInt KPositiveCbaCategory = 0;
static const TInt KNegativeCbaCategory = 1;
static const TInt KCommandGroupCategory = 3;
static const TInt KPositiveCbaButton = 0;
static const TInt KNegativeCbaButton = 1;
static const TInt KMSKCbaButton = 2;

const TInt KPositionsInCBAForSoftkeys[KCbaButtonCountMax] =
{
    0, // Left softkey
    2, // Right softkey
    3  // Middle softkey
};


// ======== MEMBER FUNCTIONS ========


inline CSwtCommandArranger::CSwtCommandArranger(MSwtDisplay& aDisplay)
        : iDisplay(aDisplay)
        , iCba(aDisplay.UiUtils().Cba())
{
    iCbaCommands.Reset();
    iDefaultCommand = NULL;
    iClearCommand = NULL;
    iSelectionCommand = NULL;
    iHelpCommand = NULL;
}


CSwtCommandArranger::~CSwtCommandArranger()
{
    delete iSearchedContainer;

    // Clean our mess in CBA (in case some commands weren't disposed?)
    ClearCba();
    // Clean our mess in menus (in case some commands weren't disposed?)
    ClearMenu();

    // Cannot use protected delete
    if (iCommandMenu)
    {
        // The command menu is removed from the menu arranger
        iDisplay.MenuArranger().SetCommandMenu(NULL);
        iCommandMenu->Dispose();
        iCommandMenu = NULL;
    }

    // Cannot use protected delete
    if (iCommandMenuOK)
    {
        iCommandMenuOK->Dispose();
        iCommandMenuOK = NULL;
    }

#ifdef RD_SCALABLE_UI_V2
    if (iStylusPopupMenuCommands)
    {
        iStylusPopupMenuCommands->Reset();
        iStylusPopupMenuCommands->Dispose();
        iStylusPopupMenuCommands = NULL;
    }
#endif //RD_SCALABLE_UI_V2

    // All commands should have been disposed (and therefore removed
    // from following array) when arranger is deleted
    ASSERT(iCommands.Count() == 0);
    iCommands.Close();

    iCommandCascadeMenus.Close();

#ifdef _lint
    iDefaultCommand = NULL;
    iClearCommand   = NULL;
    iSelectionCommand = NULL;
    iHelpCommand = NULL;
#endif
}


void CSwtCommandArranger::ConstructL()
{
    iButtonCount = KCbaButtonCountMax;//iCba.ButtonCount();

    iSearchedContainer = CSwtCommandContainer::NewL();
    iCommandMenu = CSwtCommandMenu::NewL(iDisplay);
#ifndef RD_JAVA_S60_RELEASE_9_2
    iCommandMenuOK = CSwtCommandMenu::NewL(iDisplay);
#endif
#ifdef RD_SCALABLE_UI_V2
    iStylusPopupMenuCommands = CSwtCommandMenu::NewL(iDisplay);
#endif //RD_SCALABLE_UI_V2
    // The command menu is set to the menu arranger
    iDisplay.MenuArranger().SetCommandMenu(iCommandMenu);
}


CSwtCommandArranger* CSwtCommandArranger::NewL(MSwtDisplay& aDisplay)
{
    CSwtCommandArranger* self = new(ELeave) CSwtCommandArranger(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

const MSwtControl* CSwtCommandArranger::GetFocusedControl() const
{
    const MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
    if (activeShell)
    {
        return activeShell->FocusControl();
    }
    return NULL;
}

const MSwtControl* CSwtCommandArranger::GetFirstFocusedControl() const
{
    // Get pointer to first control (the one that is actually focused) to process
    const MSwtControl* firstFocusedControl = GetFocusedControl();

    if (!firstFocusedControl && iDisplay.UiUtils().GetActiveShell())
    {
        firstFocusedControl = iDisplay.UiUtils().GetActiveShell()->Control();
    }

    return firstFocusedControl;
}

void CSwtCommandArranger::ClearCba()
{
    // Clear right softkey command and observer added by calling UpdateRightSoftkeyCommandAndObserver
    // if there are
    if (iRightSoftkeyObserverAddedByControl)
    {
        // remove command observer for right softkey added by control
        iCba.RemoveCommandObserver(KPositionsInCBAForSoftkeys[ KNegativeCbaButton ]);
        iRightSoftkeyObserverAddedByControl = EFalse;
    }
    if (iRightSoftkeyCommandIdFromControl)
    {
        // remove command added by control
        iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[ KNegativeCbaButton ],
                                    iRightSoftkeyCommandIdFromControl);
        iRightSoftkeyCommandIdFromControl = 0;
    }

    // For each command that has been mapped to CBA, clean the correponding CBA button
    for (TInt cbaIndex = 0; cbaIndex < iButtonCount; ++cbaIndex)
    {
        if (iCbaCommands[cbaIndex])
        {
            TInt commandId = KSwtCommandBaseId + cbaIndex;
            // Remove our command observer as it's still there
            iCba.RemoveCommandObserver(KPositionsInCBAForSoftkeys[cbaIndex]);
            // Remove our command from stack (even if someone else push another command on top of it)
            iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[cbaIndex], commandId);
        }
        else if (cbaIndex == KMSKCbaButton)
        {
            MEikButtonGroup* lButtonGroup = iCba.ButtonGroup();
            TInt cmdID = lButtonGroup->CommandId(KPositionsInCBAForSoftkeys[KMSKCbaButton]);
            iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[KMSKCbaButton], cmdID);
            if (cmdID == EAknSoftkeyContextOptions || cmdID == KSwtCommandBaseId + KMSKCbaButton)
            {
                iCba.RemoveCommandObserver(KPositionsInCBAForSoftkeys[KMSKCbaButton]);
            }
        }
    }

    // Clear array of CBA commands
    iCbaCommands.Reset();
    // Refresh CBA
    if (iCba.IsVisible())
    {
        iCba.DrawDeferred();
    }
}

void CSwtCommandArranger::RefreshCba(TBool aFocusedControlOnly)
{
    // Compute CBA and Menu with active shell commands
    TRAPD(err, ComputeAndRefreshCbaAndMenuL(aFocusedControlOnly));
    if (err)
    {
        CleanCbaAndMenuAfterLeaveInCompute();
    }
}

void CSwtCommandArranger::ClearMenu()
{
    iCommandMenu->Reset();

    for (TInt menuIndex = 0; menuIndex < iCommandCascadeMenus.Count(); ++menuIndex)
    {
        iCommandCascadeMenus[menuIndex]->Reset();
        iCommandCascadeMenus[menuIndex]->Dispose();
    }
    iCommandCascadeMenus.Reset();

    if (iCommandMenuOK)
    {
        iCommandMenuOK->Reset();
    }

    if (iDisplay.MenuArranger().EikMenuBar()->IsDisplayed())
    {
        iDisplay.MenuArranger().StopDisplayingMenuBar();
    }

#ifdef RD_SCALABLE_UI_V2
    TRAP_IGNORE(iDisplay.MenuArranger().CloseStylusPopupMenuL());
#endif //RD_SCALABLE_UI_V2);
}

void CSwtCommandArranger::ComputeAndRefreshCbaAndMenuL(TBool aFocusedControlOnly)
{
    if (iCbaIsBeingComputed)
        return;

    iCbaIsBeingComputed = ETrue;

    MSwtMenuArranger& menuArranger = iDisplay.MenuArranger();

    iClearCommand = NULL;
    iHelpCommand = NULL;
    iDefaultCommand = NULL;
    iSelectionCommand = NULL;
    iCbaIndexWhenComputeLeave = 0;
    iNumberOfContextOptionMenuItems = 0;

    // Get pointer to first control (the one that is actually focused) to process
    const MSwtControl* control = GetFirstFocusedControl();

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (control)
    {
        control = control->GetShell().Control();
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    const MSwtControl* focusedControl(control);

    TFixedArray<TBool, KCbaButtonCountMax> cbaIsAvailable;
    TInt cbaIndex;

    // Ask focused control for CBA buttons availability
    if (control != NULL)
    {
        for (cbaIndex = 0; cbaIndex < iButtonCount; ++cbaIndex)
        {
            if (control->CbaButtonIsUsed(cbaIndex))
            {
                cbaIsAvailable[cbaIndex] = EFalse;
            }
            else
            {
                cbaIsAvailable[cbaIndex] = ETrue;
            }
        }
    }
    else
    {
        for (cbaIndex = 0; cbaIndex < iButtonCount; ++cbaIndex)
        {
            cbaIsAvailable[cbaIndex] = ETrue;
        }
    }

    // Clear commands in CBA and menus
    // It is important to clear menu before the cba.
    ClearMenu();
    ClearCba();

    // Go through list of commands belonging to focused control and its parents
    TBool itIsNotTheFirstControlWithCommands = EFalse;
    while (control)
    {
        // Find command container of control
        TLinearOrder<CSwtCommandContainer> containerOrder(CSwtCommandArranger::CommandContainerCompare);
        iSearchedContainer->SetParentControl(*control);
        TInt indexOfContainer = iCommands.FindInOrder(iSearchedContainer, containerOrder);
        // If control has commands then process them
        if (indexOfContainer != KErrNotFound)
        {
            // Add a separator in the command menu before appending the new commands to it
            if (itIsNotTheFirstControlWithCommands)
            {
                iCommandMenu->AppendSeparatorL();
                if (iCommandMenuOK)
                {
                    iCommandMenuOK->AppendSeparatorL();
                }
            }
            else
            {
                itIsNotTheFirstControlWithCommands = ETrue;
            }

            RPointerArray<MSwtCommand>& commands = iCommands[indexOfContainer]->Commands();
            // commands are ordered by priority
            for (TInt commandIndex = 0; commandIndex < commands.Count(); ++commandIndex)
            {
                // Assign command to corresponding CBA button if available
                if (commands[commandIndex] != NULL)
                {
                    // Find selection type command.
                    // This selection type command can substitute for MSK selection operation
                    // that is handled by list, listbox, listview or table
                    if (!iSelectionCommand && control == focusedControl
                            && &(commands[commandIndex]->ParentControl()) == focusedControl
                            && commands[commandIndex]->Type() == MSwtCommand::ESelect)
                    {
                        iSelectionCommand = commands[commandIndex];
                    }
                    // Assign command to CBA buttons and menus.
                    AssignCommandToCbaAndMenuL(
                        cbaIsAvailable,
                        *commands[commandIndex],
                        *control,
                        indexOfContainer);
                }
            }
        }

        if (aFocusedControlOnly)
        {
            break;
        }

        // Stop going through parents if we encounter a (modal or not) shell
        if (control->IsShell() /*&& ((control->Style() & KSwtModalityMask)!=KSwtStyleModeless)*/)
        {
            control = NULL;
        }
        // Next control
        else
        {
            MSwtComposite* parent = control->GetParent();
            control = (parent) ? parent->Control() : NULL;
        }
    }

    // Check if the Options command is needed:
    //  - if there are menu items
    //  - if there are more than one command in command menu
    //  - if there is one command and this item is a command group
    TBool optionsCmdNeeded = EFalse;
    TBool isFullscreen = (iDisplay.UiUtils().GetActiveShell()) ?
                         iDisplay.UiUtils().GetActiveShell()->FullScreenMode() : EFalse;
    if (menuArranger.HasMenuItems())
    {
        optionsCmdNeeded = ETrue;
    }
    else if (iCommandMenu->CountDisplayableItems() > 1)
    {
        optionsCmdNeeded = ETrue;
    }
    else if (iCommandMenu->CountDisplayableItems() == 1)
    {
        if (isFullscreen)
        {
            optionsCmdNeeded = ETrue;
        }
        else
        {
            const RPointerArray<MSwtCommand>& menuItems = iCommandMenu->Commands();
            for (TInt indexMenuItem = 0; indexMenuItem < menuItems.Count(); indexMenuItem++)
            {
                if (menuItems[indexMenuItem]->IsEnabled()
                        && menuItems[indexMenuItem]->Type() == MSwtCommand::ECommandGroup)
                {
                    optionsCmdNeeded = ETrue;
                }
            }
        }
    }

    // Update the options command:
    //  - if the options command is not set and needed, it is added to the cba.
    //  - if the options command is already set and no more needed, it is removed from cba;
    //  - if the options command is already set and is still needed, nothing is done;
    if (optionsCmdNeeded && !menuArranger.IsOptionsCommandAdded())
    {
        menuArranger.AddOptionsCommandL();
    }
    else if (!optionsCmdNeeded && menuArranger.IsOptionsCommandAdded())
    {
        menuArranger.RemoveOptionsCommand();
    }

    // If options command has already been added,
    // no other command can be set to left cba button
    if (menuArranger.IsOptionsCommandAdded())
    {
        // The left cba command is set to null
        //  in case that a command has been set earlier
        iCbaCommands[KPositiveCbaButton] = NULL;
        // Since the options command is set, the left cba button is not available
        cbaIsAvailable[KPositiveCbaButton] = EFalse;
    }

    // Compute the number of items available in context options menu
    // Includes the default command, the OK (or SELECT) commands and
    // the menu items provided by a control (retrieved by GetControlMenu)
    ComputeNumberOfContextOptionsMenuItems();

    // For each command that has been mapped to CBA, set the correponding CBA button
    cbaIndex = 0;
    iCbaIndexWhenComputeLeave = 0;
    while (cbaIndex < iButtonCount)
    {
        iCbaIndexWhenComputeLeave = 2 * cbaIndex;
        if (iCbaCommands[cbaIndex])
        {
            TInt commandId = KSwtCommandBaseId + cbaIndex;
            // Update command bar
            iCba.UpdateCommandObserverL(
                KPositionsInCBAForSoftkeys[cbaIndex],
                *this);
            ++iCbaIndexWhenComputeLeave;

            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[cbaIndex],
                commandId,
                iCbaCommands[cbaIndex]->Text());

            // Dimming
            iCba.DimCommand(commandId, !iCbaCommands[cbaIndex]->IsEnabled());

            // Set default command
            if (iCbaCommands[cbaIndex] == iDefaultCommand)
            {
                iCba.SetDefaultCommand(commandId);
            }
        }
        // If MSK not already set, assign it to CBA
        // if MSK enabled and not in fullscreen mode
        else if (AknLayoutUtils::MSKEnabled()
                 && cbaIndex == KMSKCbaButton
                 && !isFullscreen)
        {
            ASSERT(iCbaCommands[KMSKCbaButton] == NULL);
            AssignMSKToCbaL();
        }
        ++cbaIndex;
    }

    // Refresh CBA
    if (iCba.IsVisible())
    {
        iCba.DrawDeferred();
    }

    iCbaIsBeingComputed = EFalse;
}

void CSwtCommandArranger::AssignCommandToCbaAndMenuL(
    TFixedArray<TBool, KCbaButtonCountMax>& aCbaIsAvailable,
    const MSwtCommand& aCommand,
    const MSwtControl& aControl,
    const TInt& aIndexOfContainer)
{
    TInt commandCategory = CSwtCommandArranger::CbaCategoryOfCommand(aCommand);
    CSwtCommandMenu* commandCascadeMenu = NULL;
    const MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();

    switch (commandCategory)
    {
    case KPositiveCbaCategory:
    {
        // Set left cba button

        // Handle Help and Clear command
        if (aCommand.IsEnabled())
        {
            // Set the Help command if the type of command is Help
            if ((!iHelpCommand)
                    && (aCommand.Type() == MSwtCommand::EHelp))
            {
                iHelpCommand = &aCommand;
            }

            // If command is positive category, enabled and the type of command is Delete,
            // set the Clear command
            if ((!iClearCommand)
                    && (!aControl.IsKeyUsed(EKeyBackspace))
                    && (aCommand.Type() == MSwtCommand::EDelete))
            {
                iClearCommand = &aCommand;
            }
        }

        if (!iDisplay.MenuArranger().HasMenuItems()
                && activeShell && !activeShell->FullScreenMode())
        {
            // If left button is available, the button can be set
            // Otherwise if the command currently set is disabled and the new one is enabled,
            // then the left button can be set with the new command.
            if ((aCbaIsAvailable[KPositiveCbaButton])
                    || (iCbaCommands[KPositiveCbaButton]
                        && !iCbaCommands[KPositiveCbaButton]->IsEnabled()
                        && aCommand.IsEnabled()))
            {
                // CBA may contain only one positive command
                iCbaCommands[KPositiveCbaButton] = &aCommand;
                aCbaIsAvailable[KPositiveCbaButton] = EFalse;
            }
        }

        if (iCommandMenuOK)
        {
            // Add command to command menu OK
            if (aCommand.Type() == MSwtCommand::EOk
                    || aCommand.Type() == MSwtCommand::ESelect)
            {
                iCommandMenuOK->AppendCommand(aCommand, NULL);
            }
        }
    }
    break;
    case KNegativeCbaCategory:
    {
        // Set right cba button
        if (activeShell && !activeShell->FullScreenMode()
                && aCbaIsAvailable[KNegativeCbaButton])
        {
            // CBA may contain only one negative command
            iCbaCommands[KNegativeCbaButton] = &aCommand;
            aCbaIsAvailable[KNegativeCbaButton] = EFalse;
            return;
        }
    }
    break;
    case KCommandGroupCategory:
    {
        // If command is a commandgroup then create a cascade menu and add sub commands to it.
        // Don't create the cascade menu if commandgroup is disabled.
        if (aCommand.Type() == MSwtCommand::ECommandGroup &&
                aCommand.IsEnabled())
        {
            commandCascadeMenu = CSwtCommandMenu::NewL(iDisplay);
            if (iCommandCascadeMenus.Append(commandCascadeMenu) == KErrNone)
            {
                // commandCascadeMenu is owned by CSwtCommandArranger (through iCommandCascadeMenus)
                // so no need to put it on CleanupStack
                AddSubCommandsToCommandMenuL(
                    &aControl,
                    iCommands[aIndexOfContainer]->SubCommandContainer(aCommand),
                    *commandCascadeMenu);
            }
            else
            {
                commandCascadeMenu->Dispose();
                commandCascadeMenu = NULL;
            }
        }
    }
    break;
    default:
        ASSERT(EFalse);
        break;
    }

    // Add command to command menu if not default command
    if (aCommand.IsDefaultCommand() && aCommand.IsEnabled()
            && !iDefaultCommand)
    {
        iDefaultCommand = &aCommand;
        iCommandMenu->InsertCommand(aCommand, commandCascadeMenu, 0);
    }
    else
    {
        iCommandMenu->AppendCommand(aCommand, commandCascadeMenu);
    }
}

void CSwtCommandArranger::AssignMSKToCbaL()
{
    // Get pointer to first control (the one that is actually focused) to process
    const MSwtControl* focusedControl = GetFirstFocusedControl();

    // Check if there is only one command in context options menu and get it
    MSwtCommand* commandOK = NULL;
    if (iCommandMenuOK && !IsContextSensitiveOperationSet())
    {
        const RPointerArray<MSwtCommand>& cmdItems = iCommandMenuOK->Commands();
        for (TInt indexCommandItem = 0; indexCommandItem < cmdItems.Count(); indexCommandItem++)
        {
            if (cmdItems[indexCommandItem]->IsEnabled())
            {
                commandOK = cmdItems[indexCommandItem];
                break;
            }
        }
    }

    // If focused control uses the selection key for its internal operation
    if (focusedControl && focusedControl->IsKeyUsed(EKeyOK))
    {
        // control will handle EKeyOK internally
        iCba.UpdateCommandObserverL(KPositionsInCBAForSoftkeys[KMSKCbaButton],
                                    *this);
        ++iCbaIndexWhenComputeLeave;
        HBufC* mskLabel = focusedControl->MSKLabelL();
        ASSERT(mskLabel);
        CleanupStack::PushL(mskLabel);
        iCba.AddCommandToStackL(
            KPositionsInCBAForSoftkeys[KMSKCbaButton],
            KSwtCommandBaseId + KMSKCbaButton,
            *mskLabel);
        CleanupStack::PopAndDestroy(mskLabel);

    }
    //If default command set
    else if (iDefaultCommand)
    {
        if (iCbaCommands[KPositiveCbaButton] == iDefaultCommand)
        {
            //Set MSK to be the same as SK1
            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                KSwtCommandBaseId + KPositiveCbaButton, //commandId
                iCbaCommands[KPositiveCbaButton]->Text());
        }
        else
        {
            iCba.UpdateCommandObserverL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                *this);
            ++iCbaIndexWhenComputeLeave;
            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                KSwtCommandBaseId + KMSKCbaButton, //commandId
                iDefaultCommand->Text());
            iCbaCommands[KMSKCbaButton] = iDefaultCommand;
        }
    }
    // allow a external selection command to substitute internal selection operations from focused control
    else if (focusedControl && iSelectionCommand && focusedControl->MSKSelCmdEnabled())
    {
        // Selection type command is assigned to the MSK
        // operation in command listener will replace internal handling from the control
        if (iCbaCommands[KPositiveCbaButton] == iSelectionCommand)
        {
            //Set MSK to be the same as SK1
            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                KSwtCommandBaseId + KPositiveCbaButton,
                iCbaCommands[KPositiveCbaButton]->Text());
        }
        else
        {
            iCba.UpdateCommandObserverL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                *this);
            ++iCbaIndexWhenComputeLeave;
            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                KSwtCommandBaseId + KMSKCbaButton,
                iSelectionCommand->Text());
            iCbaCommands[KMSKCbaButton] = iSelectionCommand;
        }

    }
    // If there is only one command in context options menu
    else if (commandOK)
    {
        if (iCbaCommands[KPositiveCbaButton] == commandOK)
        {
            //Set MSK to be the same as SK1
            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                KSwtCommandBaseId + KPositiveCbaButton, //commandId
                iCbaCommands[KPositiveCbaButton]->Text());
        }
        else
        {
            iCba.UpdateCommandObserverL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                *this);
            ++iCbaIndexWhenComputeLeave;
            iCba.AddCommandToStackL(
                KPositionsInCBAForSoftkeys[KMSKCbaButton],
                KSwtCommandBaseId + KMSKCbaButton, //commandId
                commandOK->Text());
            iCbaCommands[KMSKCbaButton] = commandOK;
        }
    }
    // If context options menu has more than one command
    else if (iNumberOfContextOptionMenuItems > 0)
    {
        iCbaCommands[KMSKCbaButton] = NULL;
        iCba.UpdateCommandObserverL(
            KPositionsInCBAForSoftkeys[KMSKCbaButton],
            *this);
        ++iCbaIndexWhenComputeLeave;
        iCba.AddCommandToStackL(
            KPositionsInCBAForSoftkeys[KMSKCbaButton],
            EAknSoftkeyContextOptions,
            KNullDesC);
    }
    // If there is only one command mapped to softkey 1
    else if (iCbaCommands[KPositiveCbaButton])
    {
        //Set MSK to be the same as SK1
        iCba.AddCommandToStackL(
            KPositionsInCBAForSoftkeys[KMSKCbaButton],
            KSwtCommandBaseId + KPositiveCbaButton, //commandId
            iCbaCommands[KPositiveCbaButton]->Text());
    }
    else if (iDisplay.MenuArranger().IsOptionsCommandAdded())
    {
        //Set MSK to be the same as SK1
        iCba.AddCommandToStackL(
            KPositionsInCBAForSoftkeys[KMSKCbaButton],
            EAknSoftkeyOptions, //commandId
            KNullDesC);
    }
}

void CSwtCommandArranger::ComputeNumberOfContextOptionsMenuItems()
{
    // Get pointer to first control (the one that is actually focused) to process
    const MSwtControl* focusedControl = GetFirstFocusedControl();

    if (!iDefaultCommand)
    {
        // count context options displayable menu items
        TInt numberOfControlMenuItems(0);

        const MSwtMenu* controlMenu = focusedControl ? focusedControl->GetControlMenu() : NULL;
        if (controlMenu)
        {
            numberOfControlMenuItems = controlMenu->CountDisplayableItems();
        }

        if (iCommandMenuOK)
        {
            TInt numberOfOKCommands(iCommandMenuOK->CountDisplayableItems());

            // If MSK enabled, not in fullscreen, no control menu items,
            // and there is only one OK (or SELECT) command,
            // this command is not included in the context options menu since
            // it will be assigned to MSK instead
            const MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
            TBool isFullscreen = activeShell ? activeShell->FullScreenMode() : EFalse;
            if (AknLayoutUtils::MSKEnabled()
                    && !isFullscreen
                    && numberOfControlMenuItems == 0
                    && numberOfOKCommands == 1)
            {
                numberOfOKCommands = 0;
            }

            // Addition of control menu items and OK (or SELECT) commands
            iNumberOfContextOptionMenuItems =
                numberOfOKCommands + numberOfControlMenuItems;
        }
    }
}

void CSwtCommandArranger::CleanCbaAndMenuAfterLeaveInCompute()
{
    iCbaIsBeingComputed = EFalse;

    if (iCbaIndexWhenComputeLeave % 2)
    {
        iCba.RemoveCommandObserver(KPositionsInCBAForSoftkeys[iCbaIndexWhenComputeLeave / 2]);
    }
    for (TInt cbaIndex = iCbaIndexWhenComputeLeave / 2; cbaIndex < iButtonCount; ++cbaIndex)
    {
        iCbaCommands[cbaIndex] = NULL;
    }
}


void CSwtCommandArranger::AddSubCommandsToCommandMenuL(const MSwtControl* aControl,
        const CSwtSubCommandContainer& aSubCommandContainer, CSwtCommandMenu& aCommandMenu)
{
    // Process sub commands of the container
    for (TInt subCommandIndex = 0; subCommandIndex < aSubCommandContainer.SubCommands().Count(); ++subCommandIndex)
    {
        CSwtCommandMenu* commandCascadeMenu = NULL;
        // If the sub command is a commandgroup then create cascade menu
        // and process sub commands of the sub command
        if (aSubCommandContainer.SubCommands()[subCommandIndex]->Type() == MSwtCommand::ECommandGroup)
        {
            commandCascadeMenu = CSwtCommandMenu::NewL(iDisplay);
            if (iCommandCascadeMenus.Append(commandCascadeMenu) == KErrNone)
            {
                // commandCascadeMenu is owned by CSwtCommandArranger
                // (through iCommandCascadeMenus) so no need to put it on CleanupStack
                AddSubCommandsToCommandMenuL(aControl,
                                             aSubCommandContainer.SubSubCommandContainer(
                                                 *aSubCommandContainer.SubCommands()[subCommandIndex]),
                                             *commandCascadeMenu);
            }
            else
            {
                commandCascadeMenu->Dispose();
                commandCascadeMenu = NULL;
            }
        }
        // Append sub command to command menu
        aCommandMenu.AppendCommand(*aSubCommandContainer.SubCommands()[subCommandIndex], commandCascadeMenu);

        if (iCommandMenuOK)
        {
            // Add command to command menu OK
            if (aSubCommandContainer.SubCommands()[subCommandIndex]->Type() == MSwtCommand::EOk ||
                    aSubCommandContainer.SubCommands()[subCommandIndex]->Type() == MSwtCommand::ESelect)
            {
                iCommandMenuOK->AppendCommand(*aSubCommandContainer.SubCommands()[subCommandIndex], NULL);
            }
        }

        // Set the clear command
        if (iClearCommand == NULL &&
                !aControl->IsKeyUsed(EKeyBackspace) &&
                aSubCommandContainer.SubCommands()[subCommandIndex]->Type() == MSwtCommand::EDelete)
        {
            iClearCommand = aSubCommandContainer.SubCommands()[subCommandIndex];
        }

        if (!iHelpCommand
                && aSubCommandContainer.SubCommands()[subCommandIndex]->Type() == MSwtCommand::EHelp)
        {
            iHelpCommand = aSubCommandContainer.SubCommands()[subCommandIndex];
        }
    }
}


TInt CSwtCommandArranger::CommandContainerCompare(const CSwtCommandContainer& aFirst,
        const CSwtCommandContainer& aSecond)
{
    if (aFirst.Control() < aSecond.Control())
    {
        return -1;
    }
    else if (aFirst.Control() > aSecond.Control())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


TInt CSwtCommandArranger::CommandCompare(const MSwtCommand& aFirst, const MSwtCommand& aSecond)
{
    if (aFirst.Priority() < aSecond.Priority())
    {
        return 1/*-1*/;
    }
    else if (aFirst.Priority() > aSecond.Priority())
    {
        return -1/*1*/;
    }
    else
    {
        return 0;
    }
}


TBool CSwtCommandArranger::ControlIsInFocusHierarchy(const MSwtControl& aControl)
{
    TBool returnedResult = EFalse;

    const MSwtControl* controlInFocusHierarchy;

    // If control is a shell then no need to go through focused control hierarchy. Instead, directly
    // check active shell hierarchy. Moreover, there may be an active shell without a focused control.
    if (aControl.IsShell())
    {
        controlInFocusHierarchy = iDisplay.UiUtils().GetActiveShell() ?
                                  iDisplay.UiUtils().GetActiveShell()->Control() : NULL;
    }
    // If control is a normal control then go through focused control hierarchy
    else
    {
        controlInFocusHierarchy = GetFocusedControl();
    }

    // Go through current focus and its parents
    while (controlInFocusHierarchy)
    {
        if (controlInFocusHierarchy == &aControl)
        {
            returnedResult = ETrue;
            break;
        }

        // Stop going through parents if we encounter a (modal or not) shell
        if (controlInFocusHierarchy->IsShell() /*&& ((controlInFocusHierarchy->Style() & KSwtModalityMask)!=KSwtStyleModeless)*/)
        {
            break;
        }
        else
        {
            MSwtComposite* parent = controlInFocusHierarchy->GetParent();
            controlInFocusHierarchy = (parent) ? parent->Control() : NULL;
        }
    }

    return returnedResult;
}


TBool CSwtCommandArranger::CbaCategoryIsAvailable(TInt aCategory,
        const TFixedArray<TBool, KCbaButtonCountMax>& aCbaAvailability, TInt /*aNeutralCommandCurrentNumber*/)
{
    TBool categoryIsAvailable(EFalse);

    switch (aCategory)
    {
    case KPositiveCbaCategory:
        categoryIsAvailable = aCbaAvailability[KPositiveCbaButton];
        break;

    case KNegativeCbaCategory:
        categoryIsAvailable = aCbaAvailability[KNegativeCbaButton];
        break;

    default:
        //ASSERT(aCategory == KNeutralCbaCategory);
        // CBA may contain as many neutral commands as its button count
        /*            TInt maxNeutralCommands = aCbaAvailability.Count();
                    if (!aCbaAvailability[KPositiveCbaButton])
                        {
                        maxNeutralCommands--;
                        }
                    if (!aCbaAvailability[KNegativeCbaButton])
                        {
                        maxNeutralCommands--;
                        }
                    categoryIsAvailable = (aNeutralCommandCurrentNumber < maxNeutralCommands) ? ETrue : EFalse;
        */
        ASSERT(EFalse);
        break;
    }

    return categoryIsAvailable;
}


TInt CSwtCommandArranger::CbaCategoryOfCommand(const MSwtCommand& aCommand)
{
    if (aCommand.ParentCommand() != NULL || aCommand.Type() == MSwtCommand::ECommandGroup)
    {
        return KCommandGroupCategory;
    }
    TInt cbaCategory(0);

    switch (aCommand.Type())
    {
    case MSwtCommand::EOk:
    case MSwtCommand::ESelect:
    case MSwtCommand::EDelete:
    case MSwtCommand::EGeneral:
    case MSwtCommand::EHelp:
        cbaCategory = KPositiveCbaCategory;
        break;

    case MSwtCommand::EStop:
    case MSwtCommand::ECancel:
        //case MSwtCommand::EDelete:
    case MSwtCommand::EBack:
    case MSwtCommand::EExit:
        cbaCategory = KNegativeCbaCategory;
        break;

    default:
        ASSERT(EFalse);
        //ASSERT(aCommand.Type() == MSwtCommand::EHelp || aCommand.Type() == MSwtCommand::EGeneral);
        //cbaCategory = KNeutralCbaCategory;
        break;
    }

    return cbaCategory;
}


TInt CSwtCommandArranger::FindAvailableNeutralButtonInCba(
    const TFixedArray<TBool, KCbaButtonCountMax>& aCbaAvailability)
{
    if (aCbaAvailability[KPositiveCbaButton])
    {
        return KPositiveCbaButton;
    }
    else if (aCbaAvailability[KNegativeCbaButton])
    {
        return KNegativeCbaButton;
    }
    else
    {
        return KErrNotFound;
    }
}




//
// From MSwtCommandArranger
//

void CSwtCommandArranger::ControlFocusLost()
{
    MSwtMenuArranger& menuArranger = iDisplay.MenuArranger();
    ASSERT(menuArranger.EikMenuBar() != NULL);
    if (!menuArranger.EikMenuBar()->IsDisplayed())
    {
        // Compute CBA and Menu with active shell commands
        RefreshCba();
    }
}

void CSwtCommandArranger::ControlFocusGained(const MSwtControl& aControl)
{
    if (&(aControl.GetShell()) == iDisplay.UiUtils().GetActiveShell())
    {
        RefreshCba();
    }
}

void CSwtCommandArranger::ActiveShellLost()
{
    // If no shell is active then no command is available so clear our mess in CBA and menus
    ClearCba();
    ClearMenu();
}

void CSwtCommandArranger::ActiveShellGained(const MSwtShell& /*aShell*/)
{
    ASSERT(iDisplay.UiUtils().GetActiveShell());
    RefreshCba();
}

void CSwtCommandArranger::CommandAddedL(const MSwtCommand& aCommand)
{
    // Find allocated command container of parent control of added command
    TInt indexOfContainer;
    TLinearOrder<CSwtCommandContainer> containerOrder(CSwtCommandArranger::CommandContainerCompare);
    iSearchedContainer->SetParentControl(aCommand.ParentControl());
    TInt findContainer = iCommands.FindInOrder(iSearchedContainer, indexOfContainer, containerOrder);
    if (findContainer == KErrNotFound)
    {
        // Insert a new command container as there is no existing one for parent control of added command
        CSwtCommandContainer* newContainer = CSwtCommandContainer::NewL();
        CleanupStack::PushL(newContainer);
        newContainer->SetParentControl(aCommand.ParentControl());
        User::LeaveIfError(iCommands.Insert(newContainer, indexOfContainer));
        // Ownership has been transferred to iCommands
        CleanupStack::Pop(newContainer);
    }

    // Add command to parent control list of commands
    TLinearOrder<MSwtCommand> commandOrder(CSwtCommandArranger::CommandCompare);
    TInt insertError = iCommands[indexOfContainer]->InsertCommandInOrder(aCommand, commandOrder);
    if (insertError)
    {
        // If insertion of command fails and a new container was allocated for the command then delete container
        if (findContainer == KErrNotFound)
        {
            delete iCommands[indexOfContainer];
            iCommands.Remove(indexOfContainer);
        }
        User::Leave(insertError);
    }

    // Compute CBA and Menu if parent control of added command is the current focused control or a parent
    if (ControlIsInFocusHierarchy(aCommand.ParentControl()))
    {
        RefreshCba();
    }
}

void CSwtCommandArranger::CommandRemoved(const MSwtCommand& aCommand)
{
    if (iDefaultCommand == &aCommand)
    {
        iDefaultCommand = NULL;
    }

    // Find allocated command container of parent control of removed command
    TLinearOrder<CSwtCommandContainer> containerOrder(CSwtCommandArranger::CommandContainerCompare);
    iSearchedContainer->SetParentControl(aCommand.ParentControl());
    TInt indexOfContainer = iCommands.FindInOrder(iSearchedContainer, containerOrder);
    if (indexOfContainer != KErrNotFound)
    {
        // If removed command was the default command then it is not anymore
        const MSwtCommand* defaultCommand = iCommands[indexOfContainer]->DefaultCommand();
        if (defaultCommand == &aCommand)
        {
            iCommands[indexOfContainer]->ClearDefaultCommand();
        }
        // Actually remove command
        iCommands[indexOfContainer]->Remove(aCommand);
        // If container is empty then delete container
        if (iCommands[indexOfContainer]->Commands().Count() == 0)
        {
            delete iCommands[indexOfContainer];
            iCommands.Remove(indexOfContainer);
        }
    }

    // Compute CBA and Menu if parent control of removed command is the current focused control or a parent
    if (ControlIsInFocusHierarchy(aCommand.ParentControl()))
    {
        RefreshCba();
    }
}

void CSwtCommandArranger::CommandContentUpdated(const MSwtCommand& aCommand)
{
    // Compute CBA and Menu if parent control of updated command is the current focused control or a parent
    if (ControlIsInFocusHierarchy(aCommand.ParentControl()))
    {
        RefreshCba();
    }
}

void CSwtCommandArranger::CommandEnablingUpdated(const MSwtCommand& aCommand)
{
    // Compute CBA and Menu if parent control of updated command is the current focused control or a parent
    if (ControlIsInFocusHierarchy(aCommand.ParentControl()))
    {
        RefreshCba();
    }
}

void CSwtCommandArranger::CommandSetAsDefault(const MSwtCommand& aCommand)
{
    // Find command container of control
    TLinearOrder<CSwtCommandContainer> containerOrder(CSwtCommandArranger::CommandContainerCompare);
    iSearchedContainer->SetParentControl(aCommand.ParentControl());
    TInt indexOfContainer = iCommands.FindInOrder(iSearchedContainer, containerOrder);
    if (indexOfContainer != KErrNotFound)
    {
        // Old default command of related parent control is no more default
        const MSwtCommand* defaultCommand = iCommands[indexOfContainer]->DefaultCommand();
        if (defaultCommand != NULL && defaultCommand != &aCommand)
        {
            TInt indexOfCommand = iCommands[indexOfContainer]->Commands().Find(defaultCommand);
            // (indexOfCommand == KErrNotFound) happens if defaultCommand is a sub command
            // (but in this case it will never be a candidate for CBA default, so we don't bother)
            if (indexOfCommand != KErrNotFound)
            {
                iCommands[indexOfContainer]->Commands()[indexOfCommand]->SetDefaultCommand(EFalse);
            }
        }
        // Set the new default command of related parent control
        iCommands[indexOfContainer]->SetDefaultCommand(aCommand);
    }

    // Compute CBA and Menu if parent control of updated command is the current focused control or a parent
    if (ControlIsInFocusHierarchy(aCommand.ParentControl()))
    {
        RefreshCba();
    }
}

TInt CSwtCommandArranger::NumberOfCommandsInCba() const
{
    TInt numberOfCommand = 0;
    for (TInt cbaIndex = 0; cbaIndex < iButtonCount; ++cbaIndex)
    {
        if (iCbaCommands[cbaIndex])
        {
            ++numberOfCommand;
        }
    }

    if (iDisplay.MenuArranger().IsOptionsCommandAdded())
    {
        ++numberOfCommand;
    }

    return numberOfCommand;
}

TBool CSwtCommandArranger::ShowMenuL(TSwtMenuType aMenuType)
{
    if (aMenuType == ESwtNoMenu)
    {
        return EFalse;
    }

    MSwtMenuArranger& menuArranger = iDisplay.MenuArranger();

    ASSERT(menuArranger.EikMenuBar() != NULL);
    if (menuArranger.EikMenuBar()->IsDisplayed())
    {
        return EFalse;
    }

    if (aMenuType == ESwtOptionsMenu)
    {
        if (!menuArranger.IsOptionsCommandAdded())
        {
            return EFalse;
        }
        menuArranger.TryDisplayMenuBarL(EFalse);
    }
    else if (aMenuType == ESwtContextOptionsMenu)
    {
        if (iNumberOfContextOptionMenuItems <= 0)
        {
            return EFalse;
        }
        menuArranger.SetContextMenu(iCommandMenuOK);
        menuArranger.TryDisplayMenuBarL(ETrue);
    }

    return ETrue;
}

TBool CSwtCommandArranger::DoContextSensitiveOperationL()
{
    if (iDefaultCommand && iDefaultCommand->JavaPeer())
    {
        iDisplay.PostSelectionEventL(iDefaultCommand->JavaPeer());
        return ETrue;
    }
    else
    {
        return ShowMenuL(MSwtCommandArranger::ESwtContextOptionsMenu);
    }
}

TBool CSwtCommandArranger::DoClearOperationL()
{
    if (iClearCommand != NULL)
    {
        iDisplay.PostSelectionEventL(iClearCommand->JavaPeer());
        return ETrue;
    }
    return EFalse;
}

TBool CSwtCommandArranger::IsContextSensitiveOperationSet() const
{
    if (iDefaultCommand
            || (iNumberOfContextOptionMenuItems > 0))
    {
        return ETrue;
    }
    return EFalse;
}

TBool CSwtCommandArranger::IsClearOperationSet() const
{
    if (iClearCommand != NULL && iClearCommand->IsEnabled())
    {
        return ETrue;
    }
    return EFalse;
}

//
// From MEikCommandObserver
// Notified from CEikCba::OfferKeyEventL > CEikButtonGroupContainer::ProcessCommandL
//
void CSwtCommandArranger::ProcessCommandL(TInt aCommandId)
{
    TInt cmdIndex(aCommandId - KSwtCommandBaseId);
    if (aCommandId == EAknSoftkeyContextOptions)
    {
        // This happens if pressing MSK with AknLayoutUtils::MSKEnabled() true.
        ShowMenuL(MSwtCommandArranger::ESwtContextOptionsMenu);
    }
    else
    {
        const MSwtCommand* command = NULL;
        if ((cmdIndex >= 0) && (cmdIndex < iCbaCommands.Count()))
        {
            command = iCbaCommands[cmdIndex];
        }
        if (command && command->IsEnabled())
        {
            iDisplay.PostSelectionEventL(command->JavaPeer());
        }
    }
}
void CSwtCommandArranger::UpdateMSKLabelL()
{

    if (iCba.PositionById(KSwtCommandBaseId + KMSKCbaButton) == KPositionsInCBAForSoftkeys[KMSKCbaButton])
    {
        const MSwtControl* control = GetFocusedControl();
        if (AknLayoutUtils::MSKEnabled() && control && control->IsKeyUsed(EKeyOK))
        {
            iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[KMSKCbaButton], KSwtCommandBaseId + KMSKCbaButton);
            HBufC* mskLabel = control->MSKLabelL();
            ASSERT(mskLabel);
            CleanupStack::PushL(mskLabel);
            iCba.AddCommandToStackL(KPositionsInCBAForSoftkeys[KMSKCbaButton], KSwtCommandBaseId + KMSKCbaButton, *mskLabel);
            CleanupStack::PopAndDestroy(mskLabel);
        }
    }
}
// This function is only used by browser currently in order to adding temporary command
// observer. Both added command and observer will be gone after caling RefreshCba();
void CSwtCommandArranger::UpdateRightSoftkeyCommandAndObserver(TInt aCommandId,
        const TDesC& aText, MEikCommandObserver& aCommandObserver)
{
    // remove current observer and command for right softkey
    if (iRightSoftkeyObserverAddedByControl || iCbaCommands[ KNegativeCbaButton ])
    {
        iCba.RemoveCommandObserver(KPositionsInCBAForSoftkeys[ KNegativeCbaButton ]);
    }

    if (iRightSoftkeyCommandIdFromControl)
    {
        iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[ KNegativeCbaButton  ],
                                    iRightSoftkeyCommandIdFromControl);
    }
    else if (iCbaCommands[ KNegativeCbaButton ])
    {
        iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[ KNegativeCbaButton  ],
                                    KSwtCommandBaseId + KNegativeCbaButton);
        iCbaCommands[ KNegativeCbaButton ] = NULL;
    }

    iRightSoftkeyCommandIdFromControl = 0;
    iRightSoftkeyObserverAddedByControl = EFalse;

    // add control specific command and observer
    TInt err(KErrNone);
    TRAP(err, iCba.AddCommandToStackL(KPositionsInCBAForSoftkeys[ KNegativeCbaButton ],
                                      aCommandId, aText));
    if (err)
    {
        return;
    }
    iRightSoftkeyCommandIdFromControl = aCommandId;

    TRAP(err, iCba.UpdateCommandObserverL(KPositionsInCBAForSoftkeys[ KNegativeCbaButton ]
                                          , aCommandObserver));
    if (err)
    {
        iCba.RemoveCommandFromStack(KPositionsInCBAForSoftkeys[ KNegativeCbaButton  ],
                                    iRightSoftkeyCommandIdFromControl);
        iRightSoftkeyCommandIdFromControl =0;
        return;
    }
    iRightSoftkeyObserverAddedByControl = ETrue;
}

TBool CSwtCommandArranger::HandleEnterKeyL()
{
    // EnterKey should be treated as MSK

    if (!AknLayoutUtils::MSKEnabled())
    {
        return EFalse;
    }

    // get msk command id
    MEikButtonGroup* buttonGroup = iCba.ButtonGroup();
    TInt mskCmdId = buttonGroup->CommandId(KPositionsInCBAForSoftkeys[ KMSKCbaButton ]);

    if (mskCmdId == EAknSoftkeyOptions)
    {
        MSwtMenuArranger& menuArranger = iDisplay.MenuArranger();
        if (!menuArranger.EikMenuBar()->IsDisplayed())
        {
            menuArranger.TryDisplayMenuBarL(EFalse);
            return ETrue;
        }
    }
    else if (mskCmdId == EAknSoftkeyContextOptions)
    {
        return ShowMenuL(MSwtCommandArranger::ESwtContextOptionsMenu);
    }
    else
    {
        TInt cmdIndex(mskCmdId - KSwtCommandBaseId);
        if (cmdIndex == KMSKCbaButton || cmdIndex == KPositiveCbaButton)
        {
            const MSwtCommand* command = NULL;
            command = iCbaCommands[ cmdIndex ];
            if (command && command->IsEnabled() && command->JavaPeer())
            {
                iDisplay.PostSelectionEventL(command->JavaPeer());
                return ETrue;
            }
        }
    }

    return EFalse;
}

TBool CSwtCommandArranger::DoHelpOperationL()
{
    if (iHelpCommand && iHelpCommand->IsEnabled())
    {
        iDisplay.PostSelectionEventL(iHelpCommand->JavaPeer());
        return ETrue;
    }
    return EFalse;
}

TBool CSwtCommandArranger::IsDefaultCommandSet() const
{
    return (iDefaultCommand && (iCbaCommands[KPositiveCbaButton] == iDefaultCommand));
}

#ifdef RD_SCALABLE_UI_V2
TBool CSwtCommandArranger::ControlHasStylusPopupMenu(const MSwtControl& aControl) const
{
    // Check if there is popup/control menu for the control
    const MSwtMenu* popupMenu = aControl.GetPopupMenu();
    if (popupMenu && (popupMenu->CountDisplayableItems(ETrue) > 0))
    {
        return ETrue;
    }

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aControl.IsShell())
    {
        return EFalse;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    const MSwtMenu* controlMenu = aControl.GetStylusPopupControlMenu();
    if (controlMenu && (controlMenu->CountDisplayableItems(ETrue) > 0))
    {
        return ETrue;
    }

#ifndef RD_JAVA_S60_RELEASE_9_2
    // Check if there are commands. If aControl is the focused one, we can directly check iCommandMenuOk count.
    // Otherwise check that there is at least one ok/select command for aControl.
    if (&aControl == GetFocusedControl())
    {
        return (iCommandMenuOK && iCommandMenuOK->CountDisplayableItems(ETrue) > 0);
    }
    else
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        TInt indexOfContainer = GetIndexOfCommandContainer(aControl);
        if (indexOfContainer != KErrNotFound)
        {
            return ContainsStylusPopupMenuCommands(iCommands[indexOfContainer], NULL);
        }
        else
        {
            return EFalse;
        }
    }
}


const MSwtMenu* CSwtCommandArranger::GetStylusPopupCommandsMenu(const MSwtControl& aControl)
{
#ifndef RD_JAVA_S60_RELEASE_9_2
    if (&aControl == GetFocusedControl())
    {
        return iCommandMenuOK;
    }
    else
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        iStylusPopupMenuCommands->Reset();

        TInt indexOfContainer = GetIndexOfCommandContainer(aControl);
        if (indexOfContainer != KErrNotFound)
        {
            AddCommandsToStylusPopupCommandMenu(iCommands[indexOfContainer], NULL);
        }

        return iStylusPopupMenuCommands;
    }
}


TBool CSwtCommandArranger::ContainsStylusPopupMenuCommands(CSwtCommandContainer* aCmdContainer,
        CSwtSubCommandContainer* aSubCmdContainer) const
{
    ASSERT(aSubCmdContainer || aCmdContainer);
    ASSERT(!aSubCmdContainer || !aCmdContainer);

    const RPointerArray<MSwtCommand>* commands = aSubCmdContainer ?
            &(aSubCmdContainer->SubCommands()) : &(aCmdContainer->Commands()) ;

    TInt count = commands->Count();
    for (TInt commandIndex = 0; commandIndex < count; ++commandIndex)
    {
        MSwtCommand* cmd = (*commands)[commandIndex];
        if (cmd && cmd->IsEnabled())
        {
            TInt type = cmd->Type();
#ifndef RD_JAVA_S60_RELEASE_9_2
            if (type == MSwtCommand::ESelect || type == MSwtCommand::EOk)
#else
            if (type != MSwtCommand::ECommandGroup)
#endif // RD_JAVA_S60_RELEASE_9_2
            {
                return ETrue;
            }
#ifndef RD_JAVA_S60_RELEASE_9_2
            else if (type == MSwtCommand::ECommandGroup)
#else
            else
#endif // RD_JAVA_S60_RELEASE_9_2
            {
                const CSwtSubCommandContainer* subContainer = aSubCmdContainer ?
                        &(aSubCmdContainer->SubSubCommandContainer(*cmd)):
                        &(aCmdContainer->SubCommandContainer(*cmd));

                if (ContainsStylusPopupMenuCommands(NULL,
                                                    const_cast<CSwtSubCommandContainer*>(subContainer)))
                {
                    return ETrue;
                }
            }
        }
    }

    return EFalse;
}


void CSwtCommandArranger::AddCommandsToStylusPopupCommandMenu(
    CSwtCommandContainer* aCmdContainer,
    CSwtSubCommandContainer* aSubCmdContainer)
{
    ASSERT(aSubCmdContainer || aCmdContainer);
    ASSERT(!aSubCmdContainer || !aCmdContainer);

    const RPointerArray<MSwtCommand>* commands = aSubCmdContainer ?
            &(aSubCmdContainer->SubCommands()) : &(aCmdContainer->Commands()) ;

    TInt count = commands->Count();
    for (TInt commandIndex = 0; commandIndex < count; ++commandIndex)
    {
        MSwtCommand* cmd = (*commands)[commandIndex];
        if (cmd && cmd->IsEnabled())
        {
            TInt type = cmd->Type();
#ifndef RD_JAVA_S60_RELEASE_9_2
            if (type == MSwtCommand::ESelect || type == MSwtCommand::EOk)
#else
            if (type != MSwtCommand::ECommandGroup)
#endif // RD_JAVA_S60_RELEASE_9_2
            {
                iStylusPopupMenuCommands->AppendCommand(*cmd, NULL);
            }
#ifndef RD_JAVA_S60_RELEASE_9_2
            else if (type == MSwtCommand::ECommandGroup)
#else
            else
#endif // RD_JAVA_S60_RELEASE_9_2
            {
                const CSwtSubCommandContainer* subContainer = aSubCmdContainer ?
                        &(aSubCmdContainer->SubSubCommandContainer(*cmd)):
                        &(aCmdContainer->SubCommandContainer(*cmd));

                AddCommandsToStylusPopupCommandMenu(NULL,
                                                    const_cast<CSwtSubCommandContainer*>(subContainer));
            }
        }
    }
}


TInt CSwtCommandArranger::GetIndexOfCommandContainer(const MSwtControl& aControl) const
{
    TLinearOrder<CSwtCommandContainer> containerOrder(
        CSwtCommandArranger::CommandContainerCompare);
    iSearchedContainer->SetParentControl(aControl);
    return iCommands.FindInOrder(iSearchedContainer, containerOrder);
}

#endif //RD_SCALABLE_UI_V2

