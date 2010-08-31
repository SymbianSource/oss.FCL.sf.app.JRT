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
* Description:  This class implements RateControl for HTTP stream audio player.
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <e32base.h>

#include "cmmaaudiostreamratecontrol.h"

namespace
{
const TInt KErrorMessageSize = 32;
_LIT(KErrDefaultError, "Symbian OS Error: %d");
}

CMMAAudioStreamRateControl* CMMAAudioStreamRateControl::NewL(CMMAAudioStreamPlayer* aPlayer)
{
    CMMAAudioStreamRateControl* self = new(ELeave) CMMAAudioStreamRateControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CMMAAudioStreamRateControl::~CMMAAudioStreamRateControl()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAAudioStreamRateControl::~CMMAAudioStreamRateControl");
    if (iPlayer)
    {
        iPlayer->RemoveStateListener(this);
    }
}

CMMAAudioStreamRateControl::CMMAAudioStreamRateControl(CMMAAudioStreamPlayer* aPlayer) :
        iPlayer(aPlayer), iCurrentRate(KMMADefaultRate)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAAudioStreamRateControl::CMMAAudioStreamRateControl");
}

void CMMAAudioStreamRateControl::ConstructL()
{
    iPlayer->AddStateListenerL(this);
}

void CMMAAudioStreamRateControl::StateChanged(TInt aState)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAAudioStreamRateControl::StateChanged");
    if (aState == CMMAPlayer::EStarted && iCurrentRate == KMMAMinRate)
    {
        // do not post event to Java or change player state
        TInt err = iPlayer->Pause();
        if (err != KErrNone)
        {
            ELOG1(EJavaMMAPI, "CMMAAudioStreamRateControl::StateChanged: Pause error %d", err);
            TBuf<KErrorMessageSize> errorMessage;
            errorMessage.Format(KErrDefaultError, err);
            iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        }
    }
}

TInt CMMAAudioStreamRateControl::SetRateL(TInt aRate)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAAudioStreamRateControl::SetRateL");
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
            // do not post event to Java or change player state
            User::LeaveIfError(iPlayer->Pause());
        }
        else
        {
            // do not post event to Java or change player state
            iPlayer->StartL();
        }
    }
    iCurrentRate = newRate;
    return iCurrentRate;
}

TInt CMMAAudioStreamRateControl::RateL()
{
    return iCurrentRate;
}


//  END OF FILE
