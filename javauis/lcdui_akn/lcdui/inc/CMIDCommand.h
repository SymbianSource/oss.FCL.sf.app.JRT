/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDCOMMAND_H
#define CMIDCOMMAND_H

#include "lcdui.h"

class CMIDCommand;
class CEikonEnv;

/** Commands can have zero or one observer that is allowed to process the command.
    This is needed for the so called "non-midlet" cmds, commands that exist only in
    the native side and should never reach java side. They are identified by a negative
    ID. See CMIDCommand. */
NONSHARABLE_CLASS(MMIDCommandObserver)
{
public:
    virtual TBool ProcessCommandL(CMIDCommand*) = 0;
};

NONSHARABLE_CLASS(CMIDCommand) : public CBase, public MMIDCommand
{
public:
    /**
     * Constant ID numbers for built-in commands in DateField, List and ChoiceGroup.
     **/
    enum
    {
        EAmPmToggleCommandId        = -101,
        EListSelectCommandId        = -102,
        EListMarkCommandId          = -103,
        EListUnmarkCommandId        = -104,
        EChoiceGroupSelectCommandId = -105,
        EChoiceGroupMarkCommandId   = -106,
        EChoiceGroupUnmarkCommandId = -107
    };

    /**
     * Creates a new command object with the given parameters.
     **/
    static MMIDCommand* NewL(const TDesC& aShortLabel,const TDesC& aLongLabel,
                             TCommandType aType, TInt aPriority, TInt aId);

    /**
     * Creates a new command object with an unique ID. This is meant to be used
     * for creating built-in commands needed e.g. to make selections on lists.
     * Typically a built-in command is set to MSK at appropriate times.
     **/
    static CMIDCommand* NewBuiltInCommandL(const TDesC& aLabel, TCommandType aType, TInt aId);

    static HBufC* AlertDismissLabelL();
    static HBufC* ListSelectLabelL();

public:
    //
    // From MMIDCommand
    //
    const TDesC& Label() const;
    TCommandType CommandType() const;
    TInt Priority() const;
    TInt Id() const;

public:
    //
    // From MMIDComponent
    //
    void Dispose();

public:
    void SetObserver(MMIDCommandObserver* aObserver);
    MMIDCommandObserver* Observer() const;

public:
    const TDesC& ShortLabel() const;

    void  SetMappedToSoftKey(TBool aMapped) const;
    TBool IsMappedToSoftKey() const;

private:
    CMIDCommand(TCommandType aType,TInt aPriority, TInt aId);
    ~CMIDCommand();
    void ConstructL(const TDesC& aShortLabel,const TDesC& aLongLabel);
    HBufC* DefaultLabelL();

protected:
    TCommandType iType;
    TInt iPriority;
    TInt iId;

    HBufC* iShortLabel;
    HBufC* iLongLabel;

    mutable TBool iIsMappedToSoftKey; //True when this command is mapped to a sk

    MMIDCommandObserver* iObserver; //Either an observer or null

    const CEikonEnv& iEikEnv;
};



#endif // CMIDCOMMAND_H

