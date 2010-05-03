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


#include "CMIDSoftKey.h"
// CMIDCOmmand API for iMappedCommand
#include "CMIDCommand.h"


CMIDSoftKey* CMIDSoftKey::NewLC(TInt aIndex, const TInt* aAllowedCommandTypes, TInt aPositionInCBA)
{
    CMIDSoftKey* self = new(ELeave) CMIDSoftKey(aIndex, aPositionInCBA);
    CleanupStack::PushL(self);
    self->ConstructL(aAllowedCommandTypes);
    return self;
}

CMIDSoftKey::~CMIDSoftKey()
{
    iAllowedCommandTypes.Close();
}

CMIDSoftKey::CMIDSoftKey(TInt aIndex, TInt aPositionInCBA)
        : iIndex(aIndex), iPositionInCBA(aPositionInCBA)
{
}

/** Initialise the valid command types for this softkey. Types are currently
loaded from static tables
*/
void CMIDSoftKey::ConstructL(const TInt* aAllowedCommandTypes)
{
    while (*aAllowedCommandTypes != -1)
    {
        User::LeaveIfError(iAllowedCommandTypes.Append(
                               STATIC_CAST(MMIDCommand::TCommandType, *aAllowedCommandTypes)));
        aAllowedCommandTypes++;
    }
}

void  CMIDSoftKey::Map(const CMIDCommand* aCommand)
{
    iMappedCommand = aCommand;

    if (iMappedCommand)
    {
        iMappedCommand->SetMappedToSoftKey(ETrue);
    }
}


