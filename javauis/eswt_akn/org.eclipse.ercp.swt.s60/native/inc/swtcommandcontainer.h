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


#ifndef SWTCOMMANDCONTAINER_H
#define SWTCOMMANDCONTAINER_H


#include "eswtmobileextensions.h"


/**
 * CSwtSubCommandContainer
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtSubCommandContainer)
        : public CBase
{
// Methods
public:
    static CSwtSubCommandContainer* NewL(TBool aFindOperationNeeded,
    TLinearOrder<CSwtSubCommandContainer> &aSubContainerOrder);
    ~CSwtSubCommandContainer();
    void SetParentCommand(const MSwtCommand& aCommand);
    inline const MSwtCommand* ParentCommand() const;
    inline const RPointerArray<MSwtCommand>& SubCommands() const;
    const CSwtSubCommandContainer& SubSubCommandContainer(
        const MSwtCommand& aCommand) const;
    TInt InsertCommandInOrder(const MSwtCommand& aCommand,
                              const TLinearOrder<MSwtCommand>& aOrder, TInt aParentalDepth);
    void Remove(const MSwtCommand& aCommand, TInt aParentalDepth);

private:
    inline CSwtSubCommandContainer(TLinearOrder<CSwtSubCommandContainer> &aSubContainerOrder);
    void ConstructL(TBool aFindOperationNeeded);

// Data
private:
    const MSwtCommand*                     iParentCommand;               // Parent command of sub commands, not owned
    RPointerArray<MSwtCommand>             iSubCommands;                 // Sub commands
    RPointerArray<CSwtSubCommandContainer> iSubSubCommandContainers;     // Containers of sub sub commands
    CSwtSubCommandContainer*               iSearchedSubCommandContainer; // Utility container used to perform find operation
    TLinearOrder<CSwtSubCommandContainer>  &iSubContainerOrder;
};

/**
 * Returns the parent command of the sub command container
 */
inline const MSwtCommand* CSwtSubCommandContainer::ParentCommand() const
{
    return iParentCommand;
}

/**
 * Returns the list of commands of the sub command container
 */
inline const RPointerArray<MSwtCommand>& CSwtSubCommandContainer::SubCommands() const
{
    return iSubCommands;
}


/**
 * CSwtCommandContainer
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCommandContainer)
        : public CBase
{
// Methods
public:
    static CSwtCommandContainer* NewL();
    ~CSwtCommandContainer();
    void SetParentControl(const MSwtControl& aControl);
    void SetDefaultCommand(const MSwtCommand& aCommand);
    void ClearDefaultCommand();
    inline const MSwtControl* Control() const;
    inline const MSwtCommand* DefaultCommand() const;
    inline RPointerArray<MSwtCommand>& Commands();
    const CSwtSubCommandContainer& SubCommandContainer(
        const MSwtCommand& aCommand);
    TInt InsertCommandInOrder(const MSwtCommand& aCommand,
                              const TLinearOrder<MSwtCommand>& aOrder);
    void Remove(const MSwtCommand& aCommand);

private:
    inline CSwtCommandContainer();
    void ConstructL();
    static TInt SubCommandContainerCompare(
        const CSwtSubCommandContainer& aFirst,
        const CSwtSubCommandContainer& aSecond);

// Data
private:
    const MSwtControl*                     iControl;                     // Parent control of commands, not owned
    RPointerArray<MSwtCommand>             iCommands;                    // Commands belonging to parent control
    RPointerArray<CSwtSubCommandContainer> iSubCommandContainers;        // Containers of sub commands (commands that are children of commandgroup commands)
    const MSwtCommand*                     iDefaultCommand;              // Default command of parent control, not owned, may be null
    CSwtSubCommandContainer*               iSearchedSubCommandContainer; // Utility container used to perform find operation
    TLinearOrder<CSwtSubCommandContainer>  iSubContainerOrder;
};


/**
 * Returns the parent control of the command container
 */
inline const MSwtControl* CSwtCommandContainer::Control() const
{
    return iControl;
}

/**
 * Returns the list of commands of the command container
 */
inline RPointerArray<MSwtCommand>& CSwtCommandContainer::Commands()
{
    return iCommands;
}

/**
 * Returns the default command of the command container
 */
inline const MSwtCommand* CSwtCommandContainer::DefaultCommand() const
{
    return iDefaultCommand;
}


#endif // SWTCOMMANDCONTAINER_H
