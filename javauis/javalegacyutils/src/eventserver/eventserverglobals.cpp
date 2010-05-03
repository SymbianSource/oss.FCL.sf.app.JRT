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


#include "eventserverglobals.h"


EventServerGlobals::EventServerGlobals()
{
    JELOG2(EUtils);
    for (int i = 0; i < CEventQueue::KDispatchThreads; i++)
    {
        mLocks[i] = 0;
    }
    mVm = 0;
}

#if defined(__WINSCW__)

#include <pls.h>
EventServerGlobals* getEsStaticData()
{
    JELOG2(EUtils);
    // Access the PLS of this process
    EventServerGlobals* data  = Pls<EventServerGlobals>(TUid::Uid(0x200211E0));
    return data;
}

#else

static EventServerGlobals* sData = 0;

EventServerGlobals* getEsStaticData()
{
    JELOG2(EUtils);
    if (sData == 0)
    {
        sData = new EventServerGlobals();
    }
    return sData;
}
#endif

