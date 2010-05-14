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
* Description:  Abstract base class for FramePositioningControl
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <e32base.h>

#include "cmmaplayer.h"
#include "cmmaframepositioningcontrol.h"

CMMAFramePositioningControl::CMMAFramePositioningControl(CMMAPlayer* aPlayer)
        : iPlayer(aPlayer)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAFramePositioningControl::CMMAFramePositioningControl");
}

CMMAFramePositioningControl::~CMMAFramePositioningControl()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAFramePositioningControl::~CMMAFramePositioningControl");
}

const TDesC& CMMAFramePositioningControl::ClassName() const
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAFramePositioningControl::ClassName");
    return KMMAFramePositioningControlName;
}

TInt CMMAFramePositioningControl::ClampMediaTime(TInt64& aMediaTime)
{
    TInt64 duration = 0;
    iPlayer->GetDuration(&duration);
    if (duration < 0)
    {
        return KErrNotFound;
    }

    if (aMediaTime > duration)
    {
        aMediaTime = duration;
    }
    else if (aMediaTime < 0)
    {
        aMediaTime = 0;
    }
    return KErrNone;
}

//  END OF FILE
