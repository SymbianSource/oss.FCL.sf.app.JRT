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
* Description:  Controls the volume of a CMMAVolumeControl.
*
*/


// INCLUDE FILES
#include <cmmavolumecontrol.h>
#include <logger.h>

#include "cammsvolumecontrol.h"

#ifdef _DEBUG
// CONSTANTS
const TInt KAMMSMaxVolume = 100;
const TInt KAMMSMinVolume = 0;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSVolumeControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSVolumeControl* CAMMSVolumeControl::NewLC(const TDesC& aControlName,
        CMMAVolumeControl* aVolumeControl, CMMAPlayer* aPlayer)
{
    CAMMSVolumeControl* self = new(ELeave) CAMMSVolumeControl(
        aControlName, aVolumeControl, aPlayer);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSVolumeControl::~CAMMSVolumeControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSVolumeControl::~CAMMSVolumeControl");
}


// -----------------------------------------------------------------------------
// CAMMSVolumeControl::SetVolumeL
// Sets the volume level.
// -----------------------------------------------------------------------------
//
void CAMMSVolumeControl::SetVolumeL(TInt aVolume)
{
    // Check in debug build that aVolume is within valid range.
    __ASSERT_DEBUG(
        (aVolume <= KAMMSMaxVolume) &&
        (aVolume >= KAMMSMinVolume),
        User::Invariant());

    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSVolumeControl::SetVolumeL: Volume = %d", aVolume);
    iVolumeControl->SetVolumeLevelL(iControlLevelIndex, aVolume);
}

const TDesC& CAMMSVolumeControl::ClassName() const
{
    return iClassName;
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSVolumeControl::ConstructL()
{
    iControlLevelIndex = iVolumeControl->AddLevelL();
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSVolumeControl::ConstructL level index = %d",
              iControlLevelIndex);
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControl::CAMMSVolumeControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSVolumeControl::CAMMSVolumeControl(
    const TDesC& aControlName,
    CMMAVolumeControl* aVolumeControl,
    CMMAPlayer* aPlayer) :
        CAMMSControl(aPlayer),
        iClassName(aControlName)
{
    iVolumeControl = aVolumeControl;
}

//  End of File

