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


#ifndef CMIDCOMMANDLIST_H
#define CMIDCOMMANDLIST_H

#include <lcdui.h>
#include "CMIDCommand.h"

class CMIDSoftKey;

struct TCommandEntry
{
    TInt            iKey;
    CMIDCommand*    iCommand;
};


//
// Enscapsulates list of commands for a particular Displayable or Item
//
NONSHARABLE_CLASS(CMIDCommandList) : public CBase
{
public:
    CMIDCommandList();
    ~CMIDCommandList();

    void AddL(MMIDCommand* aCommand);
    void Remove(MMIDCommand* aCommand);
    TInt FindCommandIndex(MMIDCommand* aCommand);

    TInt HighestNonMappedPriorityCommand(MMIDCommand::TCommandType aType,
                                         TBool aIgnoreForImplicitList = EFalse) const;
    TInt HighestPriorityCommand(MMIDCommand::TCommandType aType) const;
    TInt FindCommandForSoftKey(const CMIDSoftKey& aSoftKey,
                               TBool aIgnoreForImplicitList = EFalse) const;
    void UnMapCommands();

    TInt Count() const;
    const TDesC& Label(TInt aIndex) const;

    const TDesC& ShortLabel(TInt aIndex) const;
    const TCommandEntry& At(TInt aIndex) const;

    void SetCommandOffset(TInt aOffset);
    TInt CommandOffset() const;

    TBool IsValidIndex(TInt aIndex) const;

private:
    enum {EGranularity=4};

private:
    RArray<TCommandEntry> iArray;
    TInt iOffset; //The offset to add to command indexes in order to obtains a command ID
};


#endif // CMIDCOMMANDLIST_H

// End of File
