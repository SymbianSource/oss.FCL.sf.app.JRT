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
* Description:  Group for panning controls
*
*/


// INCLUDE FILES
#include "cammspancontrolgroup.h"
#include "cammspancontrol.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSPanControlGroup* CAMMSPanControlGroup::NewLC()
{
    CAMMSPanControlGroup* self = new(ELeave) CAMMSPanControlGroup;

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;

}

// Destructor.
CAMMSPanControlGroup::~CAMMSPanControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::Pan
// Gets the current panning set.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSPanControlGroup::Pan()
{
    return iPan;
}

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::SetPanL
// Sets the panning value
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSPanControlGroup::SetPanL(TInt aPan, TInt& aSetPan)
{
    TInt groups = ControlCount();
    TInt retVal = aPan;

    for (TInt i = 0; i < groups; i++)
    {
        retVal = TypeSafeControl(i)->SetPanL(aPan);
    }

    iPan = retVal;

    aSetPan = retVal;
}

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSPanControl* CAMMSPanControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSPanControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSPanControlGroup::ClassName()
{
    return KAMMSPanControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSPanControlGroup::NotifyPlayerAddedL(
    CMMAPlayer *aPlayer,
    CMMAControl* aControl)
{
    CAMMSControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSPanControl* control = static_cast<CAMMSPanControl*>(aControl);

    // set the current parameters
    control->SetPanL(iPan);
}

// -----------------------------------------------------------------------------
// CAMMSPanControlGroup::CAMMSPanControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSPanControlGroup::CAMMSPanControlGroup()
        : CAMMSControlGroup(KAMMSPanControl)
{
}

//  End of File
