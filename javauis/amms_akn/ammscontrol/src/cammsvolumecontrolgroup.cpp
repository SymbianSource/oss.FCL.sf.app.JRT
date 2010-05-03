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
* Description:  Group for volume controls
*
*/


// INCLUDE FILES
#include "cammsvolumecontrolgroup.h"
#include "cammsvolumecontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSVolumeControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSVolumeControlGroup* CAMMSVolumeControlGroup::NewLC(const TDesC& aControlName)
{
    CAMMSVolumeControlGroup* self = new(ELeave) CAMMSVolumeControlGroup(
        aControlName);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

//   Destructor
CAMMSVolumeControlGroup::~CAMMSVolumeControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControlGroup::SetVolumeL
// Sets the current volume set.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSVolumeControlGroup::SetVolumeL(TInt aVolume)
{
    TInt controls = ControlCount();
    for (TInt i = 0; i < controls; i++)
    {
        TypeSafeControl(i)->SetVolumeL(aVolume);
    }

    iVolume = aVolume;
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSVolumeControl*
CAMMSVolumeControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast< CAMMSVolumeControl* >(Control(aIndex));
}

// -----------------------------------------------------------------------------
// TDesC16& CAMMSVolumeControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSVolumeControlGroup::ClassName()
{
    return KAMMSVolumeControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSVolumeControlGroup::NotifyPlayerAddedL(
    CMMAPlayer *aPlayer,
    CMMAControl* aControl)
{
    CAMMSControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSVolumeControl* control =
        static_cast< CAMMSVolumeControl* >(aControl);

    // set the current parameters
    control->SetVolumeL(iVolume);
}

// -----------------------------------------------------------------------------
// CAMMSVolumeControlGroup::CAMMSVolumeControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSVolumeControlGroup::CAMMSVolumeControlGroup(const TDesC& aControlName)
        : CAMMSControlGroup(aControlName)
{
    iVolume = KAMMSMAXVolume;
}

//  End of File
