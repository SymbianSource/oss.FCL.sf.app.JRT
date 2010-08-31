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
* Description:  Manipulates the equalization settings of a Player.
*
*/


// INCLUDE FILES
#include <logger.h>
#include "cammsbaseequalizercontrol.h"



// ============================ MEMBER FUNCTIONS ===============================


// Destructor
CAMMSBaseEqualizerControl::~CAMMSBaseEqualizerControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSBaseEqualizerControl::~CAMMSBaseEqualizerControl");

}

// -----------------------------------------------------------------------------
// CAMMSBaseEqualizerControl::CAMMSBaseEqualizerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSBaseEqualizerControl::CAMMSBaseEqualizerControl(CMMAPlayer* aPlayer)
        : CAMMSEffectControl(aPlayer)
{
}

//  End of File



