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
* Description:  Group for reverb source controls
*
*/


// INCLUDE FILES
#include "cammsreverbsourcecontrolgroup.h"
#include "cammsbasereverbsourcecontrol.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSReverbSourceControlGroup* CAMMSReverbSourceControlGroup::NewLC()
{
    CAMMSReverbSourceControlGroup* self =
        new(ELeave) CAMMSReverbSourceControlGroup;

    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// Destructor
CAMMSReverbSourceControlGroup::~CAMMSReverbSourceControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::RoomLevel
// Gets the gain level of the reverberation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CAMMSReverbSourceControlGroup::RoomLevel()
{
    return iRoomLevel;
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::SetRoomLevelL
// Sets the gain level of the reverberation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbSourceControlGroup::SetRoomLevelL(TInt aLevel)
{
    // Set room level to controls.
    TInt count = ControlCount();

    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetRoomLevelL(aLevel);
    }

    iRoomLevel = aLevel;
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSBaseReverbSourceControl*
CAMMSReverbSourceControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast< CAMMSBaseReverbSourceControl* >(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSReverbSourceControlGroup::ClassName()
{
    return KAMMSReverbSourceControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::NotifyPlayerAddedL
// Called by PlayerRemoved when new player is added.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSReverbSourceControlGroup::NotifyPlayerAddedL(
    CMMAPlayer* aPlayer,
    CMMAControl* aControl)
{
    CAMMSControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSBaseReverbSourceControl* control =
        static_cast<CAMMSBaseReverbSourceControl*>(aControl);

    // set the current parameters
    control->SetRoomLevelL(iRoomLevel);
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::CAMMSReverbSourceControlGroup
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CAMMSReverbSourceControlGroup::CAMMSReverbSourceControlGroup()
        : CAMMSControlGroup(KAMMSBaseReverbSourceControl),
        iRoomLevel(0)
{
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSReverbSourceControlGroup::ConstructL()
{
    CAMMSControlGroup::ConstructL();
}

//  End of File
