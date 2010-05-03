/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for Sensor conditions
*
*/

#include "csensorconditionbase.h"
#include "logger.h"

CSensorConditionBase::CSensorConditionBase(TInt aChannelId)
        : iChannelId(aChannelId)
{
    JELOG2(ESensor);
}

CSensorConditionBase::~CSensorConditionBase()
{
    JELOG2(ESensor);
}
