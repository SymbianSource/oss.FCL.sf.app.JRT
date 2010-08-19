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
* Description:  TimerServer
*
*/

#ifndef TICKERPROVIDER_H
#define TICKERPROVIDER_H

#include <e32std.h>
#include <e32base.h>

#include "tickerproviderinterface.h"

namespace java
{
namespace captain
{

class TickerProvider : public CActive,
        public TickerProviderInterface
{
    TickerProvider();                   // Cannot be used
    TickerProvider(TickerProvider&);    // Cannot be used

public:
    TickerProvider(TickerProviderEventsInterface* aEvents);
    virtual ~TickerProvider();

    virtual void nextTickAt(const long long& aJavaTime);
    virtual long long getNextTickAt();
    virtual void cancel();

    // Helpers
    virtual long long getPlatformCurrentJavaTime();

protected:
    // CActive
    virtual void RunL();
    virtual void DoCancel();

private:
    RTimer      mRTimer;
    long long   mNextTickAt;
};

} // namespace captain
} // namespace java

#endif // TICKERPROVIDER_H

