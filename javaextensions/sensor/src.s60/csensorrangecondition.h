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

#ifndef CSENSORRANGECONDITION_H
#define CSENSORRANGECONDITION_H

#include "csensorconditionbase.h"

class CSensorRangeCondition : public CSensorConditionBase
{
public:

    CSensorRangeCondition(TInt& aChannelId,
                          TReal64& aLowerLimit, TReal64& aUpperLimit,
                          TInt& aLowerOp, TInt& aUpperOp);

    virtual ~CSensorRangeCondition();

public: // From CSensorConditionBase

    virtual TBool Evaluate(TReal aValue, TInt aChannelId);

private: // Member data
    TReal64 iLowerLimit;
    TReal64 iUpperLimit;
    TInt iLowerOp;
    TInt iUpperOp;
};

#endif // CSENSORRANGECONDITION_H
