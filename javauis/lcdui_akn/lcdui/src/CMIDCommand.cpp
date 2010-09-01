/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


// macros definitions for resources
#include <lcdui.rsg>

#include "CMIDCommand.h"
//using CEikonEnv API for loading resources to label
#include "eikenv.h"


MMIDCommand* CMIDCommand::NewL(const TDesC& aShortLabel,const TDesC& aLongLabel,
                               TCommandType aType,TInt aPriority,TInt aCommandID)
{
    CMIDCommand* command = new(ELeave) CMIDCommand(aType,aPriority,aCommandID);
    CleanupStack::PushL(command);
    command->ConstructL(aShortLabel,aLongLabel);
    CleanupStack::Pop(command);
    return command;
}

CMIDCommand* CMIDCommand::NewBuiltInCommandL(const TDesC& aLabel, TCommandType aType, TInt aId)
{
    return static_cast<CMIDCommand*>(CMIDCommand::NewL(aLabel, aLabel, aType, 0, aId));
}

HBufC* CMIDCommand::AlertDismissLabelL()
{
    // This method is static - that's why member variable iEikEnv cannot be used
    return CEikonEnv::Static()->AllocReadResourceL(R_DISMISS_COMMAND_NAME);
}

HBufC* CMIDCommand::ListSelectLabelL()
{
    // This method is static - that's why member variable iEikEnv cannot be used
    return CEikonEnv::Static()->AllocReadResourceL(R_SELECT_COMMAND_NAME);
}

CMIDCommand::CMIDCommand(TCommandType aType,TInt aPriority, TInt aId)
        :iType(aType)
        ,iPriority(aPriority)
        ,iId(aId)
        ,iEikEnv(*(CEikonEnv::Static()))
{
}

CMIDCommand::~CMIDCommand()
{
    delete iShortLabel;
    delete iLongLabel;
}

void CMIDCommand::ConstructL(const TDesC& aShortLabel,const TDesC& aLongLabel)
{
    switch (iId)
    {
    case MMIDCommand::EAlertDismissCommand:
        iShortLabel = AlertDismissLabelL();
        iLongLabel = AlertDismissLabelL();
        break;

    case MMIDCommand::EListSelectCommand:
        iShortLabel = ListSelectLabelL();
        iLongLabel = ListSelectLabelL();
        break;

    case MMIDCommand::ENullCommand:
    default:
        if (aShortLabel.Length())
        {
            iShortLabel = aShortLabel.AllocL();
        }
        else
        {
            // Use default label only if both short and long labels are empt
            if (aLongLabel.Length())
            {
                iShortLabel= aLongLabel.AllocL();
            }
            else
            {
                iShortLabel = DefaultLabelL();
            }
        }
        iLongLabel = aLongLabel.AllocL();
        break;
    }

    // Finally, in all cases make sure that the long label is not left empty.
    // If it would be, replace it with the short label.
    if (!iLongLabel->Length())
    {
        delete iLongLabel;
        iLongLabel = NULL;
        iLongLabel = iShortLabel->AllocL();
    }
}

const TDesC& CMIDCommand::Label() const
{
    return iLongLabel->Length() ? *iLongLabel : *iShortLabel;
}

MMIDCommand::TCommandType CMIDCommand::CommandType() const
{
    return iType;
}

TInt CMIDCommand::Priority() const
{
    return iPriority;
}

TInt CMIDCommand::Id() const
{
    return iId;
}

void CMIDCommand::Dispose()
{
    delete this;
}

const TDesC& CMIDCommand::ShortLabel() const
{
    return *iShortLabel;
}

void CMIDCommand::SetMappedToSoftKey(TBool aMapped) const
{
    iIsMappedToSoftKey = aMapped;
}

TBool CMIDCommand::IsMappedToSoftKey() const
{
    return iIsMappedToSoftKey;
}

HBufC* CMIDCommand::DefaultLabelL()
{
    HBufC* label = NULL;
    switch (iType)
    {
    case EScreen:
        label = iEikEnv.AllocReadResourceL(R_MIDP_SCREEN_COMMAND_TEXT);
        break;
    case EBack:
        label = iEikEnv.AllocReadResourceL(R_MIDP_BACK_COMMAND_TEXT);
        break;
    case ECancel:
        label = iEikEnv.AllocReadResourceL(R_MIDP_CANCEL_COMMAND_TEXT);
        break;
    case EOk:
        label = iEikEnv.AllocReadResourceL(R_MIDP_OK_COMMAND_TEXT);
        break;
    case EHelp:
        label = iEikEnv.AllocReadResourceL(R_MIDP_HELP_COMMAND_TEXT);
        break;
    case EStop:
        label = iEikEnv.AllocReadResourceL(R_MIDP_STOP_COMMAND_TEXT);
        break;
    case EItem:
        label = iEikEnv.AllocReadResourceL(R_MIDP_ITEM_COMMAND_TEXT);
        break;
    case EExit:
        label = iEikEnv.AllocReadResourceL(R_MIDP_EXIT_COMMAND_TEXT);
        break;
    }
    return label;
}

/** Set the current cmd observer, see MMIDCommandObserver. This can be a NULL
pointer. Any previous observer is overwritten.*/
void CMIDCommand::SetObserver(MMIDCommandObserver* aObserver)
{
    iObserver = aObserver;
}

MMIDCommandObserver* CMIDCommand::Observer() const
{
    return iObserver;
}

// End of File
