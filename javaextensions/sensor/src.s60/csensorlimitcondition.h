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

#ifndef CSENSORLIMITCONDITION_H
#define CSENSORLIMITCONDITION_H

#include "csensorconditionbase.h"

class CSensorLimitCondition : public CSensorConditionBase
{
public:

    CSensorLimitCondition(TInt aChannelId, TReal64& aLimit, TInt aOp);

    virtual ~CSensorLimitCondition();

public: // From CSensorConditionBase

    virtual TBool Evaluate(TReal aValue, TInt aChannelId);

private: // Member data
    TReal64 iLimit;
    TInt iOp;
};

#endif // CSENSORLIMITCONDITION_H
