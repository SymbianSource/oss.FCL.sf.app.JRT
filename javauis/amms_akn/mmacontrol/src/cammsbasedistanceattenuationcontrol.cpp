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
* Description:  Controls how the sound is attenuated with its distance.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include "cammsbasedistanceattenuationcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================


// Destructor
CAMMSBaseDistanceAttenuationControl::~CAMMSBaseDistanceAttenuationControl()
{
    DEBUG("AMMS::CAMMSBaseDistanceAttenuationControl::~CAMMSBaseDistanceAttenuationControl");

}

// -----------------------------------------------------------------------------
// CAMMSBaseDistanceAttenuationControl::CAMMSBaseDistanceAttenuationControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSBaseDistanceAttenuationControl::CAMMSBaseDistanceAttenuationControl(
    CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer)
{
}

//  End of File
