/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for creating MIDI player.
*
*/


#ifndef CMMAMIDIPLAYERFACTORY_H
#define CMMAMIDIPLAYERFACTORY_H

//  INCLUDES
#include "cmmammfplayerfactory.h"

//  CLASS DECLARATION
/**
*   This class is used for creating MIDI player.
*
*
*/

NONSHARABLE_CLASS(CMMAMIDIPlayerFactory): public CMMAMMFPlayerFactory
{
public: // Constructor and destructot
    static CMMAMIDIPlayerFactory* NewLC();
    ~CMMAMIDIPlayerFactory();

private: // constructor
    CMMAMIDIPlayerFactory();

public: // From MMMAPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);

    void GetSupportedProtocolsL(const TDesC& aContentType,
                                CDesC16Array& aProtocolArray);

public: // From CMMAMMFPlayerFactory

    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);

    void MediaIdsL(RArray<TUid>& aMediaIds);


private: // new methods
    CMMAPlayer* CreateMidiFilePlayerL(const TDesC& aContentType, TFileName aFileName);
    CMMAPlayer* CreateMidiSynthPlayerL(const TDesC& aContentType);
};

#endif // CMMAMIDIPLAYERFACTORY_H
