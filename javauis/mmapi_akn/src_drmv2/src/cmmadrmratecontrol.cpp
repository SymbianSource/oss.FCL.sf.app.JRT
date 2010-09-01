/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <jdebug.h>
#include <e32base.h>

#include "cmmadrmratecontrol.h"

CMMADRMRateControl* CMMADRMRateControl::NewL(CMMADRMAudioPlayer* aPlayer)
{
    CMMADRMRateControl* self = new(ELeave) CMMADRMRateControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CMMADRMRateControl::~CMMADRMRateControl()
{
    DEBUG("MMA:CMMADRMRateControl::~CMMADRMRateControl");
    if (iPlayer)
    {
        iPlayer->RemoveStateListener(this);
    }
}

CMMADRMRateControl::CMMADRMRateControl(CMMADRMAudioPlayer* aPlayer) :
        iPlayer(aPlayer), iCurrentRate(KMMADefaultRate)
{
    DEBUG("MMA:CMMADRMRateControl::CMMADRMRateControl");
}

void CMMADRMRateControl::ConstructL()
{
    iPlayer->AddStateListenerL(this);
}

void CMMADRMRateControl::StateChanged(TInt aState)
{
    DEBUG("MMA:CMMADRMRateControl::StateChanged");
    if (aState == CMMAPlayer::EStarted && iCurrentRate == KMMAMinRate)
    {
        iPlayer->DRMUtility()->Stop();
    }
}

TInt CMMADRMRateControl::SetRateL(TInt aRate)
{
    DEBUG("MMA:CMMADRMRateControl::SetRateL");
    if (aRate <= KMMAMinRate)
    {
        iCurrentRate = KMMAMinRate;
    }
    else
    {
        iCurrentRate = KMMADefaultRate;
    }

    if (iPlayer->State() == CMMAPlayer::EStarted)
    {
        if (iCurrentRate == KMMAMinRate)
        {
            iPlayer->DRMUtility()->Stop();
        }
        else
        {
            iPlayer->DRMUtility()->Play();
        }
    }
    return iCurrentRate;
}

TInt CMMADRMRateControl::RateL()
{
    return iCurrentRate;
}


//  END OF FILE
