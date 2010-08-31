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
* Description:  AMMS player builder group for adding players.
*
*/



// INCLUDE FILES
#include "cammsplayerbuildergroup.h"

// CONSTANTS
const TInt KDefaultGranularity = 1;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSPlayerBuilderGroup::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSPlayerBuilderGroup* CAMMSPlayerBuilderGroup::NewL()
{
    CAMMSPlayerBuilderGroup* self = NewLC();
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSPlayerBuilderGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSPlayerBuilderGroup* CAMMSPlayerBuilderGroup::NewLC()
{
    CAMMSPlayerBuilderGroup* self = new(ELeave) CAMMSPlayerBuilderGroup();

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

// Destructor
CAMMSPlayerBuilderGroup::~CAMMSPlayerBuilderGroup()
{
    if (iPlayerBuilders)
    {
        iPlayerBuilders->ResetAndDestroy();
    }
    delete iPlayerBuilders;
}

// -----------------------------------------------------------------------------
// CAMMSPlayerBuilderGroup::PreparePlayerL
// Performs PreparePlayerL for every player builder in the array.
// -----------------------------------------------------------------------------
void CAMMSPlayerBuilderGroup::PreparePlayerL(CMMAPlayer* aPlayer)
{
    TInt builderCount = iPlayerBuilders->Count();
    for (TInt i = 0; i < builderCount; i++)
    {
        iPlayerBuilders->At(i)->PreparePlayerL(aPlayer);
    }
}

// -----------------------------------------------------------------------------
// CAMMSPlayerBuilderGroup::AddBuilderAndPopL
// Adds a player builder to the array.
// -----------------------------------------------------------------------------
//
void CAMMSPlayerBuilderGroup::AddBuilderAndPopL(
    CAMMSPlayerBuilder* aPlayerBuilder)
{
    iPlayerBuilders->AppendL(aPlayerBuilder);
    CleanupStack::Pop(aPlayerBuilder);
}

// -----------------------------------------------------------------------------
// CAMMSPlayerBuilderGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSPlayerBuilderGroup::ConstructL()
{
    iPlayerBuilders = new(ELeave) CArrayPtrSeg< CAMMSPlayerBuilder >
    (KDefaultGranularity);
}

// -----------------------------------------------------------------------------
// CAMMSPlayerBuilderGroup::CAMMSPlayerBuilderGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSPlayerBuilderGroup::CAMMSPlayerBuilderGroup()
{
}

//  End of File
