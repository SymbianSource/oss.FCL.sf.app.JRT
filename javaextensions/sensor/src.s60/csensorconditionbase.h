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

#ifndef CSENSORCONDITIONBASE_H
#define CSENSORCONDITIONBASE_H

#include <e32base.h>

class CSensorConditionBase : public CBase
{
public:
    /**
     * Destructor
     */
    virtual ~CSensorConditionBase();

protected:
    /**
     * Constructor
     */
    CSensorConditionBase(TInt aChannelId);

public: // New methods

    /**
     * Evaluate condition
     * @param aData value this conditions is matched against
     * @return ETrue if matched, EFalse if not
     */
    virtual TBool Evaluate(TReal aValue, TInt aChannelId) = 0;

    inline TInt GetChannelId()
    {
        return iChannelId;
    };

protected: // Member data

    /**
     * ID number of the channel this condition is interested in
     */
    TInt iChannelId;
};

#endif // CSENSORCONDITIONBASE_H
