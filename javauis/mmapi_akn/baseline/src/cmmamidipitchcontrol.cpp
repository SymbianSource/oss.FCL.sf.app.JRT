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
* Description:  This class is a PitchControl.
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmamidipitchcontrol.h"
#include "cmmamidiplayer.h"

CMMAMIDIPitchControl* CMMAMIDIPitchControl::NewL(CMMAMIDIPlayer* aPlayer)
{
    CMMAMIDIPitchControl* self = new(ELeave) CMMAMIDIPitchControl(aPlayer);
    return self;
}

CMMAMIDIPitchControl::CMMAMIDIPitchControl(CMMAMIDIPlayer* aPlayer)
{
    iPlayer = aPlayer;
    DEBUG("MMA:CMMAMIDIPitchControl::CMMAMIDIPitchControl");
}

CMMAMIDIPitchControl::~CMMAMIDIPitchControl()
{
    DEBUG("MMA:CMMAMIDIPitchControl::~CMMAMIDIPitchControl");
}

const TDesC& CMMAMIDIPitchControl::ClassName() const
{
    DEBUG("MMA:CMMAMIDIPitchControl::ClassName");
    return KMIDIPitchControlName;
}


TInt CMMAMIDIPitchControl::PitchL()
{
    return iPlayer->MidiClient()->PitchTranspositionCentsL();
}

TInt CMMAMIDIPitchControl::SetPitchL(TInt aPitch)
{
    TInt pitch = aPitch;
    if (aPitch > KMIDIPitchControlMaxPitch)
    {
        pitch = KMIDIPitchControlMaxPitch;
    }
    else if (aPitch < KMIDIPitchControlMinPitch)
    {
        pitch = KMIDIPitchControlMinPitch;
    }
    iPlayer->MidiClient()->SetPitchTranspositionL(pitch);

    return PitchL();            // return the actual Pitch
}

// this method is intentionally left leaving to allow leaving implementation in the future
TInt CMMAMIDIPitchControl::MaxPitchL()
{
    return KMIDIPitchControlMaxPitch;
}

// this method is intentionally left leaving to allow leaving implementation in the future
TInt CMMAMIDIPitchControl::MinPitchL()
{
    return KMIDIPitchControlMinPitch;
}

//  END OF FILE
