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
* Description:  This class implements generic RateControl functionality.
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <e32base.h>

#include "cmmaratecontrol.h"

CMMARateControl::CMMARateControl()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMARateControl::CMMARateControl");
}

CMMARateControl::~CMMARateControl()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMARateControl::~CMMARateControl");
}

const TDesC& CMMARateControl::ClassName() const
{
    return KMMARateControlName;
}

TInt CMMARateControl::MaxRateL()
{
    return KMMADefaultRate;
}

TInt CMMARateControl::MinRateL()
{
    return KMMAMinRate;
}

//  END OF FILE
