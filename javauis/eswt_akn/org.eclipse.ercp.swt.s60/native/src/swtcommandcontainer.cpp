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


#include "swtcommandcontainer.h"


// ======== MEMBER FUNCTIONS ========


inline CSwtSubCommandContainer::CSwtSubCommandContainer(TLinearOrder<CSwtSubCommandContainer> &aSubContainerOrder)
        : iSubContainerOrder(aSubContainerOrder)
{
}


CSwtSubCommandContainer::~CSwtSubCommandContainer()
{
    delete iSearchedSubCommandContainer;

    ASSERT(iSubCommands.Count() == 0);
    ASSERT(iSubSubCommandContainers.Count() == 0);
    iSubCommands.Close();
    iSubSubCommandContainers.Close();

#ifdef _lint
    iParentCommand = NULL;
#endif
}


void CSwtSubCommandContainer::ConstructL(TBool aFindOperationNeeded)
{
    if (aFindOperationNeeded)
    {
        iSearchedSubCommandContainer = CSwtSubCommandContainer::NewL(EFalse, iSubContainerOrder);
    }
}


CSwtSubCommandContainer* CSwtSubCommandContainer::NewL(TBool aFindOperationNeeded,
        TLinearOrder<CSwtSubCommandContainer> &aSubContainerOrder)
{
    CSwtSubCommandContainer* self = new(ELeave) CSwtSubCommandContainer(aSubContainerOrder);
    CleanupStack::PushL(self);
    self->ConstructL(aFindOperationNeeded);
    CleanupStack::Pop(self);
    return self;
}


void CSwtSubCommandContainer::SetParentCommand(const MSwtCommand& aCommand)
{
    iParentCommand = &aCommand;
}


/**
 * Returns the container of sub commands of the sub command
 */
const CSwtSubCommandContainer& CSwtSubCommandContainer::SubSubCommandContainer(const MSwtCommand& aCommand) const
{
    ASSERT(iSearchedSubCommandContainer);
    iSearchedSubCommandContainer->SetParentCommand(aCommand);
    TInt indexOfSubContainer = iSubSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
    // (indexOfSubContainer == KErrNotFound) should never happen
    return *iSubSubCommandContainers[indexOfSubContainer];
}


TInt CSwtSubCommandContainer::InsertCommandInOrder(const MSwtCommand& aCommand,
        const TLinearOrder<MSwtCommand>& aOrder, TInt aParentalDepth)
{
    TInt insertError = KErrNone;

    // Find parent command related to parental depth
    const MSwtCommand* parentCommand = &aCommand;
    for (TInt depthIndex = 0; depthIndex < aParentalDepth; ++depthIndex)
    {
        ASSERT(parentCommand->ParentCommand() != NULL);
        parentCommand = parentCommand->ParentCommand();
    }

    // Insert command
    if (aParentalDepth == 0) // means aCommand belongs to this SubCommandContainer
    {
        ASSERT(aCommand.ParentCommand() == ParentCommand());
        // If added command is a commandgroup then add a new sub command container for it
        CSwtSubCommandContainer* newSubContainer = NULL;
        if (aCommand.Type() == MSwtCommand::ECommandGroup)
        {
            TRAP(insertError, (newSubContainer = CSwtSubCommandContainer::NewL(ETrue, iSubContainerOrder)));
            if (insertError == KErrNone && newSubContainer != NULL)
            {
                newSubContainer->SetParentCommand(aCommand);
                insertError = iSubSubCommandContainers.InsertInOrder(newSubContainer, iSubContainerOrder);
                if (insertError != KErrNone)
                {
                    delete newSubContainer;
                }
            }
        }

        // Actual command insert
        if (insertError == KErrNone)
        {
            insertError = iSubCommands.InsertInOrderAllowRepeats(&aCommand, aOrder);
            // If insertion of command fails and a new sub command container was allocated for the command then delete it
            if ((insertError != KErrNone) && (newSubContainer != NULL)) //lint !e449
            {
                TInt indexOfNewSubContainer = iSubSubCommandContainers.FindInOrder(newSubContainer, iSubContainerOrder);
                // (indexOfNewSubContainer == KErrNotFound) should never happen
                iSubSubCommandContainers.Remove(indexOfNewSubContainer);
                delete newSubContainer;
            }
        }
    }
    else // means aCommand belongs to a SubCommandContainer of this SubCommandContainer
    {
        ASSERT(iSearchedSubCommandContainer);
        iSearchedSubCommandContainer->SetParentCommand(*parentCommand);
        TInt indexOfSubContainer = iSubSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
        // (indexOfSubContainer == KErrNotFound) should never happen
        insertError = iSubSubCommandContainers[indexOfSubContainer]->InsertCommandInOrder(aCommand, aOrder, aParentalDepth - 1);
    }

    return insertError;
}


void CSwtSubCommandContainer::Remove(const MSwtCommand& aCommand, TInt aParentalDepth)
{
    // Find parent command related to parental depth
    const MSwtCommand* parentCommand = &aCommand;
    for (TInt depthIndex = 0; depthIndex < aParentalDepth; ++depthIndex)
    {
        ASSERT(parentCommand->ParentCommand() != NULL);
        parentCommand = parentCommand->ParentCommand();
    }

    // Remove command
    if (aParentalDepth == 0) // means aCommand belongs to this SubCommandContainer
    {
        ASSERT(aCommand.ParentCommand() == ParentCommand());
        // If removed command is a commandgroup then delete its sub command container
        if (aCommand.Type() == MSwtCommand::ECommandGroup)
        {
            ASSERT(iSearchedSubCommandContainer);
            iSearchedSubCommandContainer->SetParentCommand(aCommand);
            TInt indexOfSubContainer = iSubSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
            // (indexOfSubContainer == KErrNotFound) should never happen
            ASSERT(iSubSubCommandContainers[indexOfSubContainer]->SubCommands().Count() == 0);
            delete iSubSubCommandContainers[indexOfSubContainer];
            iSubSubCommandContainers.Remove(indexOfSubContainer);
        }

        // Actual command remove
        TInt indexOfCommand = iSubCommands.Find(&aCommand);
        iSubCommands.Remove(indexOfCommand);
    }
    else // means aCommand belongs to a SubCommandContainer of this SubCommandContainer
    {
        ASSERT(iSearchedSubCommandContainer);
        iSearchedSubCommandContainer->SetParentCommand(*parentCommand);
        TInt indexOfSubContainer = iSubSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
        // (indexOfSubContainer == KErrNotFound) should never happen
        iSubSubCommandContainers[indexOfSubContainer]->Remove(aCommand, aParentalDepth - 1);
    }
}


inline CSwtCommandContainer::CSwtCommandContainer()
        : iSubContainerOrder(CSwtCommandContainer::SubCommandContainerCompare)
{
}


CSwtCommandContainer::~CSwtCommandContainer()
{
    delete iSearchedSubCommandContainer;

    ASSERT(iCommands.Count() == 0);
    ASSERT(iSubCommandContainers.Count() == 0);
    iCommands.Close();
    iSubCommandContainers.Close();

#ifdef _lint
    iControl        = NULL;
    iDefaultCommand = NULL;
#endif
}


void CSwtCommandContainer::ConstructL()
{
    iSearchedSubCommandContainer = CSwtSubCommandContainer::NewL(EFalse, iSubContainerOrder);
}


CSwtCommandContainer* CSwtCommandContainer::NewL()
{
    CSwtCommandContainer* self = new(ELeave) CSwtCommandContainer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


void CSwtCommandContainer::SetParentControl(const MSwtControl& aControl)
{
    iControl = &aControl;
}


void CSwtCommandContainer::SetDefaultCommand(const MSwtCommand& aCommand)
{
    iDefaultCommand = &aCommand;
}


void CSwtCommandContainer::ClearDefaultCommand()
{
    iDefaultCommand = NULL;
}


TInt CSwtCommandContainer::SubCommandContainerCompare(const CSwtSubCommandContainer& aFirst,
        const CSwtSubCommandContainer& aSecond)
{
    if (aFirst.ParentCommand() < aSecond.ParentCommand())
    {
        return -1;
    }
    else if (aFirst.ParentCommand() > aSecond.ParentCommand())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 * Returns the container of sub commands of the command
 */
const CSwtSubCommandContainer& CSwtCommandContainer::SubCommandContainer(const MSwtCommand& aCommand)
{
    iSearchedSubCommandContainer->SetParentCommand(aCommand);
    TInt indexOfSubContainer = iSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
    // (indexOfSubContainer == KErrNotFound) should never happen
    return *iSubCommandContainers[indexOfSubContainer];
}


TInt CSwtCommandContainer::InsertCommandInOrder(const MSwtCommand& aCommand, const TLinearOrder<MSwtCommand>& aOrder)
{
    TInt insertError = KErrNone;

    // Find top parent command and compute related parental depth
    TInt parentalDepth = 0;
    const MSwtCommand* topParentCommand = &aCommand;
    while (topParentCommand->ParentCommand() != NULL)
    {
        ++parentalDepth;
        topParentCommand = topParentCommand->ParentCommand();
    }

    // Insert command
    if (parentalDepth == 0) // means aCommand.ParentCommand() == NULL
    {
        // If added command is a commandgroup then add a new sub command container for it
        CSwtSubCommandContainer* newSubContainer = NULL;
        if (aCommand.Type() == MSwtCommand::ECommandGroup)
        {
            TRAP(insertError, (newSubContainer = CSwtSubCommandContainer::NewL(ETrue, iSubContainerOrder)));
            if (insertError == KErrNone && newSubContainer != NULL)
            {
                newSubContainer->SetParentCommand(aCommand);
                insertError = iSubCommandContainers.InsertInOrder(newSubContainer, iSubContainerOrder);
                if (insertError != KErrNone)
                {
                    delete newSubContainer;
                }
            }
        }

        // Actual command insert
        if (insertError == KErrNone)
        {
            insertError = iCommands.InsertInOrderAllowRepeats(&aCommand, aOrder);
            // If insertion of command fails and a new sub command container was allocated for the command then delete it
            if ((insertError != KErrNone) && (newSubContainer != NULL)) //lint !e449
            {
                TInt indexOfNewSubContainer = iSubCommandContainers.FindInOrder(newSubContainer, iSubContainerOrder);
                // (indexOfNewSubContainer == KErrNotFound) should never happen
                iSubCommandContainers.Remove(indexOfNewSubContainer);
                delete newSubContainer;
            }
        }
    }
    else // means aCommand is a sub command
    {
        iSearchedSubCommandContainer->SetParentCommand(*topParentCommand);
        TInt indexOfSubContainer = iSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
        // (indexOfSubContainer == KErrNotFound) should never happen
        insertError = iSubCommandContainers[indexOfSubContainer]->InsertCommandInOrder(aCommand, aOrder, parentalDepth - 1);
    }

    return insertError;
}


void CSwtCommandContainer::Remove(const MSwtCommand& aCommand)
{
    // Find top parent command and compute related parental depth
    TInt parentalDepth = 0;
    const MSwtCommand* topParentCommand = &aCommand;
    while (topParentCommand->ParentCommand() != NULL)
    {
        ++parentalDepth;
        topParentCommand = topParentCommand->ParentCommand();
    }

    // Remove command
    if (parentalDepth == 0) // means aCommand.ParentCommand() == NULL
    {
        // If removed command is a commandgroup then delete its sub command container
        if (aCommand.Type() == MSwtCommand::ECommandGroup)
        {
            iSearchedSubCommandContainer->SetParentCommand(aCommand);
            TInt indexOfSubContainer = iSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
            // (indexOfSubContainer == KErrNotFound) should never happen
            ASSERT(iSubCommandContainers[indexOfSubContainer]->SubCommands().Count() == 0);
            delete iSubCommandContainers[indexOfSubContainer];
            iSubCommandContainers.Remove(indexOfSubContainer);
        }

        // Actual command remove
        TInt indexOfCommand = iCommands.Find(&aCommand);
        iCommands.Remove(indexOfCommand);
    }
    else // means aCommand is a sub command
    {
        iSearchedSubCommandContainer->SetParentCommand(*topParentCommand);
        TInt indexOfSubContainer = iSubCommandContainers.FindInOrder(iSearchedSubCommandContainer, iSubContainerOrder);
        // (indexOfSubContainer == KErrNotFound) should never happen
        iSubCommandContainers[indexOfSubContainer]->Remove(aCommand, parentalDepth - 1);
    }
}
