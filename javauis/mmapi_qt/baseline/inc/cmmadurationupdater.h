/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Send update duration event if needed when player state changes
*
*/


#ifndef CMMADURATIONUPDATER_H
#define CMMADURATIONUPDATER_H

//  INCLUDES
#include "mmmaplayerstatelistener.h"
#include "cmmaplayer.h"

//  CLASS DECLARATION
/**
*   Checks if media duration has changed when player state changes and sends
*   DURATION_UPDATED event to Java.
*
*/
NONSHARABLE_CLASS(CMMADurationUpdater):
        public CBase, public MMMAPlayerStateListener
{
public: // Construction
    static CMMADurationUpdater* NewL(CMMAPlayer& aPlayer);
    ~CMMADurationUpdater();

private: // Construction
    CMMADurationUpdater(CMMAPlayer& aPlayer);
    void ConstructL();

public: // from MMMAPlayerStateListener
    virtual void StateChanged(TInt aState);

private: // Data

    /* The player which is monitored for duration changes */
    CMMAPlayer& iPlayer;

    /* Last obtained duration value */
    TInt iDuration;

    // to avoid event posting with streamable medias after first start
    TBool iSecondStart;
};

#endif // CMMADURATIONUPDATER_H
