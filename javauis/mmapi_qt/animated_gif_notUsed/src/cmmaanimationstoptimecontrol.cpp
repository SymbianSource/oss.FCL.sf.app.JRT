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

#include "cmmaanimationstoptimecontrol.h"
#include "cmmaplayer.h"

CMMAAnimationStopTimeControl* CMMAAnimationStopTimeControl::NewL(CMMAAnimationPlayer* aPlayer)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAAnimationStopTimeControl::NewL");
    CMMAAnimationStopTimeControl* control =
        new(ELeave) CMMAAnimationStopTimeControl(aPlayer);
    aPlayer->SetAnimationObserver(control);
    return control;
}


CMMAAnimationStopTimeControl::~CMMAAnimationStopTimeControl()
{
}


CMMAAnimationStopTimeControl::CMMAAnimationStopTimeControl(CMMAAnimationPlayer* aPlayer)
        : CMMAStopTimeControl(aPlayer)
{
}

void CMMAAnimationStopTimeControl::SetStopTimeL(const TInt64& aTime)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAAnimationStopTimeControl::SetStopTimeL");
    iStopTime = aTime;
}

void CMMAAnimationStopTimeControl::AnimationAdvancedL(TInt /*aFrame*/, TInt64 aMediaTime)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAAnimationStopTimeControl::AnimationAdvancedL");
    if (aMediaTime > iStopTime)
    {
        iPlayer->StopL(EFalse);
        iPlayer->PostLongEvent(CMMAPlayerEvent::EStoppedAtTime, aMediaTime);
        iStopTime = iNoTimer;
    }
}




//  END OF FILE
