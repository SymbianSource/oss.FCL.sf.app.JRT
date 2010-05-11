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
* Description:  This class implements generic RateControl functionality.
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <e32base.h>

#include "cmmammfratecontrol.h"

namespace
{
const TInt KErrorMessageSize = 32;
_LIT(KErrDefaultError, "Symbian OS Error: %d");
}

CMMAMMFRateControl* CMMAMMFRateControl::NewL(CMMAMMFPlayerBase* aPlayer)
{
    CMMAMMFRateControl* self = new(ELeave) CMMAMMFRateControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CMMAMMFRateControl::~CMMAMMFRateControl()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAMMFRateControl::~CMMAMMFRateControl");
}

CMMAMMFRateControl::CMMAMMFRateControl(CMMAMMFPlayerBase* aPlayer) :
        iPlayer(aPlayer), iCurrentRate(KMMADefaultRate)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAMMFRateControl::CMMAMMFRateControl");
}

void CMMAMMFRateControl::ConstructL()
{
    iPlayer->AddStateListenerL(this);
}

void CMMAMMFRateControl::StateChanged(TInt aState)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAMMFRateControl::StateChanged");
    if (aState == CMMAPlayer::EStarted && iCurrentRate == KMMAMinRate)
    {
        RMMFController& controller = iPlayer->Controller();
        TInt err = controller.Pause();
        if ((err != KErrNone) && (err != KErrNotReady))
        {
            ELOG1( EJavaMMAPI, "CMMAMMFRateControl::StateChanged: Pause error %d", err);
            TBuf<KErrorMessageSize> errorMessage;
            errorMessage.Format(KErrDefaultError, err);
            iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        }
    }
}

TInt CMMAMMFRateControl::SetRateL(TInt aRate)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAMMFRateControl::SetRateL");
    RMMFController& controller = iPlayer->Controller();

    TInt newRate;
    if (aRate <= KMMAMinRate)
    {
        newRate = KMMAMinRate;
    }
    else
    {
        newRate = KMMADefaultRate;
    }

    if ((iPlayer->State() == CMMAPlayer::EStarted) &&
            (newRate != iCurrentRate))
    {
        if (newRate == KMMAMinRate)
        {
            TInt err = controller.Pause();
            if ((err != KErrNone) && (err != KErrNotReady))
            {
                User::Leave(err);
            }
        }
        else
        {
            User::LeaveIfError(controller.Play());
        }
    }
    iCurrentRate = newRate;
    return iCurrentRate;
}

TInt CMMAMMFRateControl::RateL()
{
    return iCurrentRate;
}


//  END OF FILE
