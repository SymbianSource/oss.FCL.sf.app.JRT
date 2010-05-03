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
* Description:  Group for location controls
*
*/


// INCLUDE FILES

#include "cammslocationcontrolgroup.h"
#include "cammslocationcontrol.h"
#include "ammsutil.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSLocationControlGroup* CAMMSLocationControlGroup::NewLC(
    TAMMSControlTypes aControlType)
{
    CAMMSLocationControlGroup* self = new(ELeave) CAMMSLocationControlGroup(
        aControlType);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;

}

// Destructor
CAMMSLocationControlGroup::~CAMMSLocationControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::Cartesian
// Gets the coordinates of the current location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSLocationControlGroup::CartesianL(
    TInt aLocation[ KAMMSVectorComponents ])
{
    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        aLocation[ i ] = iCommited.iLocation[ i ];
    }
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::SetCartesianL
// Moves the object to the new location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSLocationControlGroup::SetCartesianL(TInt aX, TInt aY, TInt aZ)
{
    iUncommited.iLocation[ EComponentX ] = aX;
    iUncommited.iLocation[ EComponentY ] = aY;
    iUncommited.iLocation[ EComponentZ ] = aZ;

    UpdateL(ELocation);
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::SetSphericalL
// Moves the object to the new location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSLocationControlGroup::SetSphericalL(
    TInt aAzimuth,
    TInt aElevation,
    TInt aRadius)
{
    __ASSERT_DEBUG(aRadius >= 0, User::Invariant());

    TInt sphericalVector[] = { aAzimuth, aElevation, aRadius };

    // Convert to cartesian.
    AMMSUtil::FromSphericalToCartesianL(sphericalVector,
                                        iUncommited.iLocation);

    UpdateL(ELocation);
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSLocationControl*
CAMMSLocationControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSLocationControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSLocationControlGroup::ClassName()
{
    return KAMMSLocationControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::CommitL
// Transfers all the pending parameters to the audio processing system.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSLocationControlGroup::CommitL(TInt aCommit)
{
    if (aCommit & ELocation)
    {
        TInt controls = ControlCount();
        for (TInt i = 0; i < controls; i++)
        {
            CAMMSLocationControl* control = TypeSafeControl(i);

            control->SetLocationCartesianL(
                iUncommited.iLocation[ EComponentX ],
                iUncommited.iLocation[ EComponentY ],
                iUncommited.iLocation[ EComponentZ ]);
        }

        iCommited = iUncommited;
    }
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSLocationControlGroup::NotifyPlayerAddedL(
    CMMAPlayer* aPlayer,
    CMMAControl* aControl)
{
    CAMMSAudio3DControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSLocationControl* control =
        static_cast<CAMMSLocationControl*>(aControl);

    // set the current parameters
    control->SetLocationCartesianL(
        iCommited.iLocation[ EComponentX ],
        iCommited.iLocation[ EComponentY ],
        iCommited.iLocation[ EComponentZ ]);
}

// -----------------------------------------------------------------------------
// CAMMSLocationControlGroup::CAMMSLocationControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSLocationControlGroup::CAMMSLocationControlGroup(
    TAMMSControlTypes aControlType):
        CAMMSAudio3DControlGroup(KAMMSLocationControl, aControlType)
{
}

//  End of File
