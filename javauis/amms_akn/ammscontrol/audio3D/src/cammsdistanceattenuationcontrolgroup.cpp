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
* Description:  Group for distance attenuation controls
*
*/


// INCLUDE FILES

#include "cammsdistanceattenuationcontrolgroup.h"
#include "cammsbasedistanceattenuationcontrol.h"

// CONSTANTS
static const TInt KAMMSMinDistance = 1000;
static const TInt KAMMSRolloffFactor = 1000;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSDistanceAttenuationControlGroup*
CAMMSDistanceAttenuationControlGroup::NewLC()
{
    CAMMSDistanceAttenuationControlGroup* self =
        new(ELeave) CAMMSDistanceAttenuationControlGroup;

    CleanupStack::PushL(self);
    // calls base class ConstructL
    self->ConstructL();

    return self;
}

//Destructor
CAMMSDistanceAttenuationControlGroup::~CAMMSDistanceAttenuationControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::MaxDistance
// Returns the maximum distance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt32 CAMMSDistanceAttenuationControlGroup::MaxDistance() const
{
    return iCommited.iMaxDistance;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::MinDistance
// Returns the distance where the source is loudest.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt32 CAMMSDistanceAttenuationControlGroup::MinDistance() const
{
    return iCommited.iMinDistance;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::MuteAfterMax
// Returns how the distance gain behaves for distances beyond
// the maximum distance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CAMMSDistanceAttenuationControlGroup::MuteAfterMax() const
{
    return iCommited.iMuteAfterMax;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::RolloffFactor
// Returns the rolloff factor for the distance gain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUint32 CAMMSDistanceAttenuationControlGroup::RolloffFactor() const
{
    return iCommited.iRolloffFactor;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::SetParametersL
// Sets all the 3D audio distance attenuation parameters simultaneously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDistanceAttenuationControlGroup::SetParametersL(
    TInt32 aMinDistance,
    TInt32 aMaxDistance,
    TBool aMuteAfterMax,
    TUint32 aRolloffFactor)
{
    // temporary values, moved to the commited variables in CommitL() method
    iUncommited.iMinDistance = aMinDistance;
    iUncommited.iMaxDistance = aMaxDistance;
    iUncommited.iMuteAfterMax = aMuteAfterMax;
    iUncommited.iRolloffFactor = aRolloffFactor;

    UpdateL(EDistance);
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::TypeSafeControl
// Get and cast a control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSBaseDistanceAttenuationControl*
CAMMSDistanceAttenuationControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSBaseDistanceAttenuationControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSDistanceAttenuationControlGroup::ClassName()
{
    return KAMMSDistanceAttenuationClassName;
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::CommitL
// Transfers all the pending parameters to the audio processing system.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDistanceAttenuationControlGroup::CommitL(TInt aCommit)
{
    if (aCommit & EDistance)
    {
        TInt controls = ControlCount();
        for (TInt i = 0; i < controls; i++)
        {
            TypeSafeControl(i)->SetParametersL(
                iUncommited.iMinDistance,
                iUncommited.iMaxDistance,
                iUncommited.iMuteAfterMax,
                iUncommited.iRolloffFactor);
        }
        iCommited = iUncommited;
    }
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSDistanceAttenuationControlGroup::NotifyPlayerAddedL(
    CMMAPlayer* aPlayer,
    CMMAControl* aControl)
{
    CAMMSAudio3DControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSBaseDistanceAttenuationControl* control =
        static_cast<CAMMSBaseDistanceAttenuationControl*>(aControl);

    // set the current parameters
    control->SetParametersL(
        iCommited.iMinDistance,
        iCommited.iMaxDistance,
        iCommited.iMuteAfterMax,
        iCommited.iRolloffFactor);
}

// -----------------------------------------------------------------------------
// CAMMSDistanceAttenuationControlGroup::CAMMSDistanceAttenuationControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSDistanceAttenuationControlGroup::CAMMSDistanceAttenuationControlGroup()
        : CAMMSAudio3DControlGroup(KAMMSBaseDistanceAttenuationControl)
{
    iCommited.iMinDistance = KAMMSMinDistance;
    iCommited.iMaxDistance = KMaxTInt;
    iCommited.iMuteAfterMax = ETrue;
    iCommited.iRolloffFactor = KAMMSRolloffFactor;
}


//  End of File
