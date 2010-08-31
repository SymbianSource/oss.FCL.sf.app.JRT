/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Group for Doppler controls
*
*/


// INCLUDE FILES

#include "cammsdopplercontrolgroup.h"
#include "cammsdopplercontrol.h"
#include "ammsutil.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSDopplerControlGroup* CAMMSDopplerControlGroup::NewLC(
    TAMMSControlTypes aControlType)
{
    CAMMSDopplerControlGroup* self = new(ELeave) CAMMSDopplerControlGroup(
        aControlType);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSDopplerControlGroup::~CAMMSDopplerControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::VelocityCartesianL
// Returns the current velocity, used in calculations for the Doppler effect
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDopplerControlGroup::VelocityCartesianL(
    TInt aVelocity[ KAMMSVectorComponents ])
{
    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        aVelocity[ i ] = iCommited.iVelocity[ i ];
    }
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::Enabled
// Returns whether this Doppler effect is currently active.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CAMMSDopplerControlGroup::Enabled()
{
    return iCommited.iEnabled;
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::SetEnabledL
// Specifies if this Doppler effect is active or ignored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDopplerControlGroup::SetEnabledL(TBool aEnabled)
{
    // If the DopplerControl was fetched from the Spectator,
    // this method has no effect. So only disable the value for SoundSource3D.
    if (aEnabled ||
            (!aEnabled && iControlType == EAMMSSoundSource3DControl))
    {
        iUncommited.iEnabled = aEnabled;
        UpdateL(EEnabled);
    }
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::SetVelocityCartesianL
// Sets the velocity, used in calculations for the Doppler effect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDopplerControlGroup::SetVelocityCartesianL(
    TInt aX,
    TInt aY,
    TInt aZ)
{
    iUncommited.iVelocity[ EComponentX ] = aX;
    iUncommited.iVelocity[ EComponentY ] = aY;
    iUncommited.iVelocity[ EComponentZ ] = aZ;

    UpdateL(EVelocity);
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::SetVelocitySphericalL
// Sets the velocity, used in calculations for the Doppler effect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDopplerControlGroup::SetVelocitySphericalL(
    TInt aAzimuth,
    TInt aElevation,
    TInt aRadius)
{
    TInt sphericalVector[] = { aAzimuth, aElevation, aRadius };

    // Convert to cartesian.
    AMMSUtil::FromSphericalToCartesianL(sphericalVector,
                                        iUncommited.iVelocity);

    UpdateL(EVelocity);
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSDopplerControl*
CAMMSDopplerControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSDopplerControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSDopplerControlGroup::ClassName()
{
    return KAMMSDopplerControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::CommitL
// Transfers all the pending parameters to the audio processing system.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDopplerControlGroup::CommitL(TInt aCommit)
{
    TInt controls = ControlCount();

    // first commit enabled state
    if (aCommit & EEnabled)
    {
        for (TInt i = 0; i < controls; i++)
        {
            CAMMSDopplerControl* control = TypeSafeControl(i);
            control->SetEnabledL(iUncommited.iEnabled);
        }
        iCommited.iEnabled = iUncommited.iEnabled;
    }

    // then commit velocity
    if (aCommit & EVelocity)
    {
        for (TInt i = 0; i < controls; i++)
        {
            CAMMSDopplerControl* control = TypeSafeControl(i);

            control->SetVelocityCartesianL(
                iUncommited.iVelocity[ EComponentX ],
                iUncommited.iVelocity[ EComponentY ],
                iUncommited.iVelocity[ EComponentZ ]);
        }

        // Change uncommited velocity to commited
        for (TInt i = 0; i < KAMMSVectorComponents; i++)
        {
            iCommited.iVelocity[ i ] = iUncommited.iVelocity[ i ];
        }
    }
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDopplerControlGroup::NotifyPlayerAddedL(
    CMMAPlayer* aPlayer,
    CMMAControl* aControl)
{
    CAMMSAudio3DControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSDopplerControl* control =
        static_cast<CAMMSDopplerControl*>(aControl);

    // set the current parameters

    control->SetVelocityCartesianL(
        iCommited.iVelocity[ EComponentX ],
        iCommited.iVelocity[ EComponentY ],
        iCommited.iVelocity[ EComponentZ ]);

    control->SetEnabledL(iCommited.iEnabled);
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::CAMMSDopplerControlGroup
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CAMMSDopplerControlGroup::CAMMSDopplerControlGroup(
    TAMMSControlTypes aControlType):
        CAMMSAudio3DControlGroup(KAMMSDopplerControl, aControlType)
{
}

// -----------------------------------------------------------------------------
// CAMMSDopplerControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
inline void CAMMSDopplerControlGroup::ConstructL()
{
    CAMMSAudio3DControlGroup::ConstructL();

    // The default value for Spectator's DopplerControl is true
    // (the default value for SoundSource3D's DopplerControl is false)
    if (iControlType == EAMMSSpectatorControl)
    {
        SetEnabledL(ETrue);
    }
}

//  End of File
