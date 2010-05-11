/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for setting volume to tuner player
*
*/



// INCLUDE FILES
#include    "CAMMSTunerVolumeControl.h"
#include    "CAMMSTunerPlayer.h"

#include <logger.h>


CAMMSTunerVolumeControl::CAMMSTunerVolumeControl(CAMMSTunerPlayer* aTunerPlayer)
        : CMMAVolumeControl(aTunerPlayer)
{
    iTunerPlayer = aTunerPlayer;
}

CAMMSTunerVolumeControl* CAMMSTunerVolumeControl::NewL(CAMMSTunerPlayer* aTunerPlayer)
{
    LOG( EJavaAMMS, EInfo, "CAMMSTunerVolumeControl::NewL +");
    CAMMSTunerVolumeControl* self = new(ELeave) CAMMSTunerVolumeControl(aTunerPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

void CAMMSTunerVolumeControl::DoSetLevelL(TInt aLevel)
{
    LOG1( EJavaAMMS, EInfo, "CAMMSTunerVolumeControl::DoSetLevelL, aLevel = %d", aLevel);
    // TunerPlayer does not currently have SetVolumeL method
    User::Invariant();
}

//  End of File
