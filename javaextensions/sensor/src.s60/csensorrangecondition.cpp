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
* Description:  Range condition implementation
*
*/

#include "csensorrangecondition.h"
#include "sensornativeconstants.h"
#include "logger.h"

CSensorRangeCondition::CSensorRangeCondition(TInt& aChannelId,
        TReal64& aLowerLimit, TReal64& aUpperLimit,
        TInt& aLowerOp, TInt& aUpperOp)
        : CSensorConditionBase(aChannelId), iLowerLimit(aLowerLimit),
        iUpperLimit(aUpperLimit), iLowerOp(aLowerOp), iUpperOp(aUpperOp)
{
    JELOG2(ESensor);
}

CSensorRangeCondition::~CSensorRangeCondition()
{
    JELOG2(ESensor);
}

TBool CSensorRangeCondition::Evaluate(TReal aValue, TInt aChannelId)
{
    JELOG2(ESensor);
    if (aChannelId != iChannelId)
    {
        return EFalse;
    }

    TBool lowerMet = EFalse;
    switch (iLowerOp)
    {
    case INT_GREATER_THAN:
    {
        lowerMet = aValue > iLowerLimit;
        break;
    }
    case INT_GREATER_THAN_OR_EQUALS:
    {
        lowerMet = aValue >= iLowerLimit;
        break;
    }
    }
    TBool upperMet = EFalse;
    switch (iUpperOp)
    {
    case INT_LESS_THAN:
    {
        upperMet = aValue < iUpperLimit;
        break;
    }
    case INT_LESS_THAN_OR_EQUALS:
    {
        upperMet = aValue <= iUpperLimit;
        break;
    }
    }
    if (lowerMet && upperMet)
    {
        return ETrue;
    }
    return EFalse;
}
