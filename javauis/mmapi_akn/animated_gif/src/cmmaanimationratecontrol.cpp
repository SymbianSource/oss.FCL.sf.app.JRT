/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <e32base.h>

#include "cmmaanimationratecontrol.h"
#include "cmmaanimationplayer.h"


// -----------------------------------------------------------------------------
// CMMAAnimationRateControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMMAAnimationRateControl* CMMAAnimationRateControl::NewL(CMMAAnimationPlayer* aPlayer)
{
    CMMAAnimationRateControl* self = new(ELeave) CMMAAnimationRateControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CMMAAnimationRateControl::CMMAAnimationRateControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMMAAnimationRateControl::CMMAAnimationRateControl(CMMAAnimationPlayer* aPlayer)
{
    DEBUG("MMA:CMMAAnimationRateControl::CMMAAnimationRateControl");
    iPlayer = aPlayer;
}

// Destructor
CMMAAnimationRateControl::~CMMAAnimationRateControl()
{
    DEBUG("MMA:CMMAAnimationRateControl::~CMMAAnimationRateControl");
}

// -----------------------------------------------------------------------------
// CMMAAnimationRateControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMMAAnimationRateControl::ConstructL()
{
    DEBUG("MMA:CMMAAnimationRateControl::ConstructL");
    iPlayer->SetRateL(KMMADefaultRate);
}

// -----------------------------------------------------------------------------
// CMMAAnimationRateControl::SetRateL
// Set rate to minimum or default rate depending on parameter. Informs player
// that rate has changed. Returns set rate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CMMAAnimationRateControl::SetRateL(TInt aRate)
{
    DEBUG("MMA:CMMAAnimationRateControl::SetRateL");
    TInt rate = aRate;

    if (rate <= KMMAMinRate)
    {
        rate = KMMAMinRate;
    }
    else
    {
        rate = KMMADefaultRate;
    }
    return iPlayer->SetRateL(rate);
}

// -----------------------------------------------------------------------------
// CMMAAnimationRateControl::RateL
// Returns current rate.
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CMMAAnimationRateControl::RateL()
{
    DEBUG("MMA:CMMAAnimationRateControl::RateL");
    return iPlayer->RateL();
}

//  END OF FILE
