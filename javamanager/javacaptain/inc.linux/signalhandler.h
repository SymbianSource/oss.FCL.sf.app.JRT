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
* Description:  SignalHandler
*
*/

#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <pthread.h>

namespace java
{
namespace captain
{

class SignalHandler
{
public:
    SignalHandler();
    virtual ~SignalHandler();

    void start();
    void stop();

private:
    static void* signalHandlerThread(void* aParams);

    pthread_t mThreadId;
};

} // namespace captain
} // namespace java

#endif // SIGNALHANDLER_H

