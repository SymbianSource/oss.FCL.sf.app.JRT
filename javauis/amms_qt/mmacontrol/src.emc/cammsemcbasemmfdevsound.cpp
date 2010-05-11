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
* Description:  Group for effect controls
*
*/


// INCLUDE FILES
#include <logger.h>
#include "cammsemcbasemmfdevsound.h"


// ============================ MEMBER FUNCTIONS ===============================
// Constructors

CAMMSEMCBaseMMFDevSound::CAMMSEMCBaseMMFDevSound()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound:: CAMMSEMCBaseMMFDevSound()");
}

// Destructor
CAMMSEMCBaseMMFDevSound::~CAMMSEMCBaseMMFDevSound()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound::~ +");
    if (iMMFDevSound)
    {
        delete iMMFDevSound;
        iMMFDevSound = NULL;
    }

    if (iActiveSchedulerWait)
    {
        delete iActiveSchedulerWait;
        iActiveSchedulerWait = NULL;
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound::~ -");
}

/******************************************************************************
* Method Name:  CAMMSEMCBaseMMFDevSound::CreateAndInitializeDevSoundL()
******************************************************************************/
TInt CAMMSEMCBaseMMFDevSound::CreateAndInitializeDevSoundL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound::CreateAndInitializeDevSoundL");
    TInt initError;
    if (iMMFDevSound)
    {
        delete iMMFDevSound;
        iMMFDevSound = NULL;
    }
    TRAP(initError, iMMFDevSound = CMMFDevSound::NewL());
    if (initError)
        return initError;

    TRAP(initError,iMMFDevSound->InitializeL(*this, EMMFStatePlaying));

    if (!initError)
    {
        __ASSERT_DEBUG(!iActiveSchedulerWait->IsStarted(), User::Invariant());
        // Wait until InitializeComplete() has been called.
        iActiveSchedulerWait->Start();  // CSI: 10 iActiveSchedulerWait cannot be started, also checked in debug build #

    }

    return initError;
}

/******************************************************************************
* Method Name:  CAMMSEMCBaseMMFDevSound::DeleteDevSound()
******************************************************************************/
void CAMMSEMCBaseMMFDevSound::DeleteDevSound()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound::DeleteDevSound + ");
    if (iMMFDevSound)
    {
        delete iMMFDevSound;
        iMMFDevSound = NULL;
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound::DeleteDevSound - ");
}

/******************************************************************************
******************************** CALLBACK ************************************
*******************************************************************************/

/******************************************************************************
* Method Name:  CAMMSEMCBaseMMFDevSound::InitializeComplete
* Description:  CMMFDevSound callback when Initialization is complete
******************************************************************************/
void CAMMSEMCBaseMMFDevSound::InitializeComplete(TInt aError)
{
    ELOG1( EJavaAMMS, "AMMS::CAMMSEMCBaseMMFDevSound::InitializeComplete err = %d ",aError);
    // Just to suppress warning in release build
    (void)aError;
    __ASSERT_DEBUG(iActiveSchedulerWait->IsStarted(), User::Invariant());
    // Stop waiting in CreateAndInitializeDevSoundL() function.
    iActiveSchedulerWait->AsyncStop();

}


// -----------------------------------------------------------------------------
// CAMMSEMCBaseMMFDevSound::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEMCBaseMMFDevSound::ConstructL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEMCBaseMMFDevSound::ConstructL ");
    // create CActiveSchedulerWait
    iActiveSchedulerWait = new(ELeave)CActiveSchedulerWait;
}

//  End of File
