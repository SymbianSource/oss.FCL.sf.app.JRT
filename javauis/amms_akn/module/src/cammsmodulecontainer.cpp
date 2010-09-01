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
* Description:  Container for CAMMSModule objects.
*
*/


// INCLUDE FILES
#include <jdebug.h>

#include "cammsmodulecontainer.h"

// CONSTANTS
// Before further testing is done, 4 is sufficient average value to be used here.
const TInt KAMMSContainerDefaultGranularity = 4;

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSModuleContainer::~CAMMSModuleContainer()
{
    ResetAndDestroy();
}

// -----------------------------------------------------------------------------
// CAMMSModuleContainer::RemovePlayer
// Removes player from all modules.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSModuleContainer::RemovePlayer(CMMAPlayer* aPlayer)
{
    // Remove player from all modules
    for (TInt i = 0; i < Count(); i++)
    {
        At(i)->RemovePlayerNoStateCheck(aPlayer);
    }
}

// -----------------------------------------------------------------------------
// CAMMSModuleContainer::RemoveModule
// Removes module from container.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSModuleContainer::RemoveModule(CAMMSModule* aModule)
{
    // If the element is found, index is set to the position of that module
    // element within the array.
    TInt index = KErrNotFound;

    TKeyArrayFix key(0, ECmpTInt);   // offset = 0, type integer

    // find returns 0, if the element with the specified key is found. Non-zero
    // if the element with the specified key is not found
    TInt retVal = Find(aModule, key, index);

    DEBUG_INT("AMMS::CAMMSModuleContainer::DisposeModule find result %d",
              retVal);

    if (retVal == KErrNone)
    {
        // Remove the element by position. The module itself is not deleted
        // here, so it has to be deleted elsewhere if needed.
        Delete(index);
    }
    // else, module was not added to this container
}

// -----------------------------------------------------------------------------
// CAMMSModuleContainer::CAMMSModuleContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSModuleContainer::CAMMSModuleContainer():
        CArrayPtrSeg< CAMMSModule >(KAMMSContainerDefaultGranularity)
{
}

//  End of File
