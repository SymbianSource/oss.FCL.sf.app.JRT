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
* Description:  Manipulates the settings of an audio effect called reverb.
*
*/


// INCLUDE FILES
#include <logger.h>
#include "cammsbasereverbcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSBaseReverbControl::~CAMMSBaseReverbControl()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSBaseReverbControl::~CAMMSBaseReverbControl");
}

// -----------------------------------------------------------------------------
// CAMMSBaseReverbControl::GetEnvironmentalReverbUtilityL
// Returns the environmental reverb utility.
// -----------------------------------------------------------------------------
void CAMMSBaseReverbControl::GetEnvironmentalReverbUtilityL(
    CEnvironmentalReverbUtility** /*aEnvironmentalReverbUtility*/)
{
    // Derived Class will implement this method
}
// only require if platform support EMC
#ifdef RD_JAVA_HTTP_EMC_ENABLED
MReverbControl* CAMMSBaseReverbControl::GetReverbControlL()
{
    // derived class will implement this method
    return (MReverbControl*)NULL;
}
#endif
// -----------------------------------------------------------------------------
// CAMMSBaseReverbControl::CAMMSBaseReverbControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSBaseReverbControl::CAMMSBaseReverbControl(CMMAPlayer* aPlayer)
        : CAMMSEffectControl(aPlayer)
{
}

//  End of File
