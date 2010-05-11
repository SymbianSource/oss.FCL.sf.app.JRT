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


//  INCLUDE FILES
#include <logger.h>

#include "cmmadurationupdater.h"

CMMADurationUpdater* CMMADurationUpdater::NewL(CMMAPlayer& aPlayer)
{
    CMMADurationUpdater* self = new(ELeave) CMMADurationUpdater(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CMMADurationUpdater::CMMADurationUpdater(CMMAPlayer& aPlayer)
        : iPlayer(aPlayer), iDuration(KErrNotFound), iSecondStart(EFalse)
{
}

void CMMADurationUpdater::ConstructL()
{
    iPlayer.AddStateListenerL(this);
}

CMMADurationUpdater::~CMMADurationUpdater()
{
    iPlayer.RemoveStateListener(this);
}

void CMMADurationUpdater::StateChanged(TInt aState)
{
    TInt64 duration = 0;
    iPlayer.GetDuration(&duration);

    if ((duration >= KErrNotFound) && (duration != iDuration))
    {
        // Send DURATION_UPDATED
        iDuration = duration;
        iPlayer.PostLongEvent(CMMAPlayerEvent::EDurationUpdated, iDuration);
    }
    else if (duration == KErrNotFound && aState == CMMAPlayer::EStarted && !iSecondStart)
    {
        // for medias whose duration is unknown (e.g. streaming audio amr)
        // must post EDurationUpdated event with duration -1 in first start
        iPlayer.PostLongEvent(CMMAPlayerEvent::EDurationUpdated, iDuration);
        iSecondStart = ETrue;
    }

    if (aState == CMMAPlayer::ERealized)
    {
        iSecondStart = EFalse;
    }
}

//  END OF FILE
