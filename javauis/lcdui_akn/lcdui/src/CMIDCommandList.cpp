/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
*
*/


#include "CMIDCommandList.h"
// API used for members iArray (array of TCommandEntry items contais command)
#include "CMIDCommand.h"
// used in FindCommandForSoftKey function - retrieving index of command for given softkey
#include "CMIDSoftKey.h"


/*
This is the method used for ordering commands in the list. Commands are ordered
by type first and only if they have the same type they are then ordered by priority.

Each command type is assigned a value in KCmdTypeValues. According to this value
the commands are ordered so that the higher the value the higher the position of
the command in the array and hence the
lower the position of the command in the menus.
*/
TInt CompareTypeAndPriority(const TCommandEntry& aEntry1, const TCommandEntry& aEntry2)
{

    /** Each type is associated with a
    value that determines its position in the list
    as from specs. */
    const TInt KCmdTypeValues[9] =
    {
        -1, // No type
        5,  // EScreen=1,
        7,  // EBack,
        3,  // ECancel,
        2,  // EOk,
        6,  // EHelp,
        1,  // EStop,
        8,  // EExit,
        4   // EItem
    };

    TInt typeValue1 = KCmdTypeValues[aEntry1.iCommand->CommandType()];
    TInt typeValue2 = KCmdTypeValues[aEntry2.iCommand->CommandType()];

    if (typeValue1 != typeValue2)
    {
        return typeValue1 - typeValue2;
    }

    TInt priority1 = aEntry1.iCommand->Priority();
    TInt priority2 = aEntry2.iCommand->Priority();

    return (priority1 - priority2);
}

CMIDCommandList::CMIDCommandList()
        : iArray(EGranularity, _FOFF(TCommandEntry, iKey))
{
}

CMIDCommandList::~CMIDCommandList()
{
    iArray.Close();
}

void CMIDCommandList::AddL(MMIDCommand* aCommand)
{
    if (FindCommandIndex(aCommand) == KErrNotFound)
    {
        TCommandEntry entry;
        entry.iKey     = aCommand->Id();
        entry.iCommand = static_cast<CMIDCommand*>(aCommand);
        TLinearOrder<TCommandEntry> priorityOrder(CompareTypeAndPriority);
        iArray.InsertInOrderAllowRepeatsL(entry, priorityOrder);
    }
}

void CMIDCommandList::Remove(MMIDCommand* aCommand)
{
    TInt index = FindCommandIndex(aCommand);
    if (index >=0)
    {
        iArray.Remove(index);
    }
}

TInt CMIDCommandList::FindCommandIndex(MMIDCommand* aCommand)
{
    TCommandEntry entry;
    entry.iKey     = aCommand->Id();
    entry.iCommand = static_cast<CMIDCommand*>(aCommand);
    return iArray.Find(entry);
}

/**
  Return the index of the highest priority command of the specified type
   - commands are in priority order so the first one
  of a particular type that we find will be the highest priority
  command of that type.
*/
TInt CMIDCommandList::HighestPriorityCommand(MMIDCommand::TCommandType aType) const
{
    TInt numCommands = iArray.Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        if (At(i).iCommand->CommandType() == aType)
        {
            return i;
        }
    }

    return KErrNotFound;
}


/**
  Return the index of the highest priority command of the specified type
  unless the command has already been mapped to a sk
   - commands are in priority order so the first one
  of a particular type that we find will be the highest priority
  command of that type.
*/
TInt CMIDCommandList::HighestNonMappedPriorityCommand(MMIDCommand::TCommandType aType) const
{
    TInt numCommands = Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        CMIDCommand* cmd = At(i).iCommand;
        if ((cmd->CommandType() == aType) && !cmd->IsMappedToSoftKey())
        {
            return i;
        }
    }

    return KErrNotFound;
}

/**
    Given a softkey, return the index of the highest priority command
    that is allowed for this softkey.
 */
TInt CMIDCommandList::FindCommandForSoftKey(const CMIDSoftKey& aSoftKey) const
{
    TInt numTypes = aSoftKey.AllowedCommandTypes().Count();
    for (TInt i = 0; i < numTypes; i++)
    {
        TInt index = HighestNonMappedPriorityCommand(aSoftKey.AllowedCommandTypes()[i]);
        if (index != KErrNotFound)
        {
            return index;
        }
    }

    return KErrNotFound;
}

/** Resets the mapped-to-softkey status of all commands to not mapped */
void CMIDCommandList::UnMapCommands()
{
    TInt numCommands = Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        At(i).iCommand->SetMappedToSoftKey(EFalse);
    }
}

TInt CMIDCommandList::Count() const
{
    return iArray.Count();
}

const TCommandEntry& CMIDCommandList::At(TInt aIndex) const
{
    ASSERT(IsValidIndex(aIndex));
    return iArray[aIndex];
}

const TDesC& CMIDCommandList::Label(TInt aIndex) const
{
    ASSERT(IsValidIndex(aIndex));
    return At(aIndex).iCommand->Label();
}

const TDesC& CMIDCommandList::ShortLabel(TInt aIndex) const
{
    ASSERT(IsValidIndex(aIndex));
    return At(aIndex).iCommand->ShortLabel();
}

void CMIDCommandList::SetCommandOffset(TInt aOffset)
{
    iOffset = aOffset;
}

TInt CMIDCommandList::CommandOffset() const
{
    return iOffset;
}

TBool CMIDCommandList::IsValidIndex(TInt aIndex) const
{
    if (aIndex >= 0 && aIndex < iArray.Count())
    {
        return ETrue;
    }
    return EFalse;
}
// End of File
