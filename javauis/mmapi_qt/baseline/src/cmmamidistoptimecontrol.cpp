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
* Description:  This class is used for stoptime controlling
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <e32std.h>

#include "cmmamidistoptimecontrol.h"
#include "cmmaplayer.h"

CMMAMIDIStopTimeControl* CMMAMIDIStopTimeControl::NewL(CMMAPlayer* aPlayer)
{
    CMMAMIDIStopTimeControl* control =
        new(ELeave) CMMAMIDIStopTimeControl(aPlayer);
    CleanupStack::PushL(control);
    // calls base class ConstructL
    control->ConstructL();
    CleanupStack::Pop(); // control
    return control;
}


CMMAMIDIStopTimeControl::~CMMAMIDIStopTimeControl()
{
}

CMMAMIDIStopTimeControl::CMMAMIDIStopTimeControl(CMMAPlayer* aPlayer)
        : CMMAStopTimeControl(aPlayer)
{
}


void CMMAMIDIStopTimeControl::StopAtTimeL()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAMIDIStopTimeControl::StopAtTime");

    // Stop the player only when it's playing
    if (iPlayer->State() == CMMAPlayer::EStarted)
    {
        TInt64 time;
        iPlayer->GetMediaTime(&time);
        if (time >= 0 && time < iStopTime)
        {
            LOG1(EJavaMMAPI, EInfo, "MMA:CMMAMIDIStopTimeControl::StopAtTime - Called %dms too early",
                 I64INT((time - iStopTime)/1000));
            StartTimer(time);
            return;
        }

        TInt64 stopTime;

        iPlayer->StopL(EFalse);
        iPlayer->SetMediaTimeL(&iStopTime);
        iPlayer->GetMediaTime(&stopTime);

        // Inform the player that it's "stopped at time"
        iPlayer->PostLongEvent(CMMAPlayerEvent::EStoppedAtTime, stopTime);

        iStopTime = iNoTimer; // Timer is reseted
    }
}

//  END OF FILE
