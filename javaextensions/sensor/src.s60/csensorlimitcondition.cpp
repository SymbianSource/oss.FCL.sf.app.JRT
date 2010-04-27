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
* Description:  Limit condition implementation
*
*/

#include "csensorlimitcondition.h"
#include "sensornativeconstants.h"
#include "logger.h"

CSensorLimitCondition::CSensorLimitCondition(
    TInt aChannelId, TReal64& aLimit, TInt aOp)
        : CSensorConditionBase(aChannelId), iLimit(aLimit), iOp(aOp)
{
    JELOG2(ESensor);
}

CSensorLimitCondition::~CSensorLimitCondition()
{
    JELOG2(ESensor);
}

TBool CSensorLimitCondition::Evaluate(TReal aValue, TInt aChannelId)
{
    JELOG2(ESensor);
    if (aChannelId != iChannelId)
    {
        return EFalse;
    }
    switch (iOp)
    {
    case INT_EQUALS:
    {
        return aValue == iLimit;
    }
    case INT_GREATER_THAN:
    {
        return aValue > iLimit;
    }
    case INT_GREATER_THAN_OR_EQUALS:
    {
        return aValue >= iLimit;
    }
    case INT_LESS_THAN:
    {
        return aValue < iLimit;
    }
    case INT_LESS_THAN_OR_EQUALS:
    {
        return aValue <= iLimit;
    }
    }
    return EFalse;
}
