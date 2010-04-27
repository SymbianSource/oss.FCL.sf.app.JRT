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
* Description:
 *
*/


#ifndef CTIMEOUTTIMER_H
#define CTIMEOUTTIMER_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace location
{
// CLASS DECLARATION

/**
 *  Handles timeouts for CTrackingPositioner.
 */
class CTimeoutTimer: public CTimer
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CTimeoutTimer* NewL(const TCallBack& aCallBack);

    /**
     * Destructor.
     */
    ~CTimeoutTimer();

public:
    // New functions

    void TimeoutAfter(const TTimeIntervalMicroSeconds& aDelay);

protected:
    // Functions from base classes

    /**
     * From CActive
     */
    void RunL();

private:

    /**
     * C++ default constructor.
     */
    CTimeoutTimer(const TCallBack& aCallBack);

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

private:
    // Data
    TTime iTimeoutTime;
    TCallBack iCallBack;
};

}
}

#endif // CTIMEOUTTIMER_H
// End of File
