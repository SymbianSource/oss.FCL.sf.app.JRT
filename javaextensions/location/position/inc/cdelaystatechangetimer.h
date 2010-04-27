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


#ifndef CDELAYSTATECHANGETIMER_H
#define CDELAYSTATECHANGETIMER_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace location
{

//  FORWARD DECLARATIONS
class CLocationProvider;

// CLASS DECLARATION

/**
 *  Timer used to delay state changes.
 */
class CDelayStateChangeTimer: public CTimer
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CDelayStateChangeTimer* NewL(CLocationProvider* aLocationProvider);

    /**
     * Destructor.
     */
    ~CDelayStateChangeTimer();

public:
    // New functions

    void ChangeStateAfter(const TTimeIntervalMicroSeconds32& aInterval,
                          TInt aNewState);

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
    CDelayStateChangeTimer(CLocationProvider* aLocationProvider);

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

private:
    // Data
    CLocationProvider* iLocationProvider;
    TInt iNewState;
};

}
}
#endif // CDELAYSTATECHANGETIMER_H
// End of File
