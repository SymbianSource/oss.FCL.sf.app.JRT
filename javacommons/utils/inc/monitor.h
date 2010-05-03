/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>
#include "javaosheaders.h"

namespace java
{
namespace util
{

class Monitor
{

public:
    //Constructor(s) & destructor
    virtual ~Monitor();

    //API
    OS_IMPORT static Monitor* createMonitor();
    OS_IMPORT int wait(int timeOut = 0);
    OS_IMPORT int notify();


private: //Methods
    Monitor();
    Monitor(const Monitor&);              //No copy constructor allowed
    Monitor& operator= (const Monitor&);  //No Assignment operator allowed

private: //Members
    pthread_mutex_t         mMutex;
    pthread_cond_t          mCondVar;
    bool                    mWaiting;
    bool                    mNotifyBeforeWait;
};

} //end namespace util
} //end namespace java
#endif // MONITOR_H

