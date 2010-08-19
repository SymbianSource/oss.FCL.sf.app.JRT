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
* Description:  This class is used for playing sounds
*
*/

#ifndef CMMAEMCAUDIOPLAYER_H
#define CMMAEMCAUDIOPLAYER_H

//  INCLUDES
#include "cmmaemcplayerbase.h"
#include "mmf/common/mmfbase.h"

// CONSTANTS

const TMdaPriorityPreference KMMAEMCPriorityPreference =
    EMdaPriorityPreferenceTimeAndQuality;

_LIT(KMMAEMCAudioPlayer, "EMCAudioPlayer");

//  CLASS DECLARATION
/**
*   This class is used for playing sounds
*
*/

NONSHARABLE_CLASS(CMMAEMCAudioPlayer): public CMMAEMCPlayerBase
{
public: // Construction
    /**
     * Creates new player.
     */
    static CMMAEMCAudioPlayer* NewLC(
        CMMAEMCResolver* aResolver);

    //   Destructor
    IMPORT_C ~CMMAEMCAudioPlayer();

protected:
    //   C++ constructor
    IMPORT_C CMMAEMCAudioPlayer(CMMAEMCResolver* aResolver);
    IMPORT_C void ConstructL();

protected: // New methods
    IMPORT_C virtual void PrefetchDataL(const TDesC8& aData);
    IMPORT_C virtual void PrefetchFileL();
    IMPORT_C virtual void PlayCompleteL(TInt aError);

public: // from CMMAPlayer
    IMPORT_C void RealizeL();
    IMPORT_C void PrefetchL();

    IMPORT_C const TDesC& Type();

public: // from CMMAPlayer/MMMASourceStreamListener
    IMPORT_C void ReadCompletedL(TInt aStatus, const TDesC8& aData);
};

#endif // CMMAEMCAUDIOPLAYER_H
