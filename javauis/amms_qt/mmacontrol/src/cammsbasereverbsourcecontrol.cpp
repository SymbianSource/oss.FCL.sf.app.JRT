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
* Description:  Manipulates the settings of an audio effect reverb source.
*
*/


// INCLUDE FILES
#include <logger.h>
#include "cammsbasereverbsourcecontrol.h"



// ============================ MEMBER FUNCTIONS ===============================


// Destructor
CAMMSBaseReverbSourceControl::~CAMMSBaseReverbSourceControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSBaseReverbSourceControl::~CAMMSBaseReverbSourceControl");
}

// -----------------------------------------------------------------------------
// CAMMSBaseReverbSourceControl::CAMMSBaseReverbSourceControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSBaseReverbSourceControl::CAMMSBaseReverbSourceControl(
    CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer)
{
}

//  End of File
