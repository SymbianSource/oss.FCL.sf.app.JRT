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


#ifndef CMIDSOFTKEY_H
#define CMIDSOFTKEY_H

#include <lcdui.h>

class CMIDCommand;

/** A repository for softkey related information and operations,
like the command types allowed for this softkey, any command mapped
to it, the index of the softkey on the device. */
NONSHARABLE_CLASS(CMIDSoftKey) : public CBase
{
public:
    static CMIDSoftKey* NewLC(TInt aIndex, const TInt* aAllowedCommandTypes, TInt aPositionInCBA);
    ~CMIDSoftKey();

private:
    CMIDSoftKey(TInt aIndex, TInt aPositionInCBA);
    void ConstructL(const TInt* aAllowedCommandTypes);

public:
    inline const RArray<MMIDCommand::TCommandType>&
    AllowedCommandTypes() const;

    void Map(const CMIDCommand* aCommand);
    inline TBool HasAMappedCommand() const;

    inline TBool CommandIsMapped(const CMIDCommand* aCommand) const;
    inline TInt PositionInCBA() const;

    inline const CMIDCommand* MappedCommand() const;

private:
    TInt iIndex; //The index of the softkey on the device

    TInt iPositionInCBA; //The position of this sk in the CBA

    TBool iMapped; //True when this soft key has been mapped

    RArray<MMIDCommand::TCommandType> iAllowedCommandTypes;
    //The command types allowed for this softkey

    const CMIDCommand* iMappedCommand; //A reference to the command mapped to this CBA or null
};

inline const RArray<MMIDCommand::TCommandType>&
CMIDSoftKey::AllowedCommandTypes() const
{
    return iAllowedCommandTypes;
}

inline TBool  CMIDSoftKey::HasAMappedCommand() const
{
    return iMappedCommand != NULL;
}

inline TBool CMIDSoftKey::CommandIsMapped(const CMIDCommand* aCommand) const
{
    return iMappedCommand == aCommand; //pointer comparison!!
}

inline TInt  CMIDSoftKey::PositionInCBA() const
{
    return iPositionInCBA;
}

inline const CMIDCommand* CMIDSoftKey::MappedCommand() const
{
    return iMappedCommand;
}

#endif // CMIDSOFTKEY_H

// End of File
