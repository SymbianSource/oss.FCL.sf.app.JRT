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

#include <sys/types.h>

#include "tickerproviderinterface.h"

namespace java
{
namespace captain
{

class TickerProvider : public TickerProviderInterface
{
public:
    TickerProvider(TickerProviderEventsInterface* aEvents);
    virtual ~TickerProvider();

    virtual void nextTickAt(const long long& aJavaTime);
    virtual long long getNextTickAt();
    virtual void cancel();

    // Helpers
//    virtual long long getCurrentJavaTime();

private:
    timer_t     mTimerId;
    long long   mNextTickAt;
};

} // namespace captain
} // namespace java

#endif // TICKERPROVIDER_H

