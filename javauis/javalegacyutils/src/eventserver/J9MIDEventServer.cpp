/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <hal.h>

#include <e32def.h>
#include <e32const.h>

#include "eventserverglobals.h"
#include "CJavaEventServer.h"
#include "com_nokia_mj_impl_rt_legacy_MemoryUtil.h"
#include "com_nokia_mj_impl_rt_legacy_EventProcessor.h"

RArray<TInt>& EventServerHandles(); //Must be consistent with implementation
//in MIDEventServer.cpp in jniutils


const TInt KQueueGranularity = 100; // Define iQueue default granularity
const TInt KCompressThreshold = 100; // Define number of processed events


void InitEventServer()
{
    TRAPD(err, CEventQueue::InitL());
    if (err != KErrNone)
    {
        User::Panic(_L("EventServer::can't initialize event queue"), err);
    }
}

// Cleanup the event server; called from Java event dispatch thread
void CleanupEventServer()
{
    CJavaEventServer::Cleanup(EventServerHandles());
}

// Dispatch event to Java, called from Java event dispatch thread
void RunJavaEventServer(JNIEnv* aJni, TInt aServer)
{
    CJavaEventBase* ev = (CJavaEventBase*) aServer;
    if (aServer != KErrServerTerminated)
    {
        ev->Run(*aJni);
    }
}


JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_rt_legacy_MemoryUtil__1getFreeRAMInKB(JNIEnv*, jclass)
{
    TInt mem = 0;
    TInt err = HAL::Get(HALData::EMemoryRAMFree, mem);
    if (err == KErrNone)
    {
        return mem / 1024;
    }
    else
    {
        return err;
    }
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_legacy_EventProcessor__1dispatchCallbacks(JNIEnv* aJni, jclass /*aClass*/, jint aThread)
{
    TInt eventServer;
    // Loop for event dispatching
    InitEventServer();
    while ((eventServer = CEventQueue::GetQueue(aThread)->GetNext()) != 0)
    {
        RunJavaEventServer(aJni, eventServer);
    }
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_legacy_EventProcessor__1exitThreads(JNIEnv*, jclass)
{
    CEventQueue::OnCloseThreads();
}
// class CIntQueue
CIntQueue::CIntQueue() : iQueue(KQueueGranularity), iCompressCounter(0)
{
}

CIntQueue* CIntQueue::NewL()
{
    CIntQueue* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

CIntQueue* CIntQueue::NewLC()
{
    CIntQueue* self = new(ELeave) CIntQueue();
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

void CIntQueue::ConstructL()
{
    iMutex.CreateLocal();
    iFull.CreateLocal(0);
}

CIntQueue::~CIntQueue()
{
    iMutex.Close();
    iFull.Close();
    iQueue.Close();
}

// Add integer to the end of the queue
void CIntQueue::Enqueue(TInt aValue)
{
    iMutex.Wait();             // enter critical region
    PutItem(aValue);           // put new item to the buffer
    iMutex.Signal();           // leave critical region
    iFull.Signal();            // inform dispatcher about new event
}

// Return integer from the beginning of the queue
TInt CIntQueue::Dequeue()
{
    iFull.Wait();              // decrement count of full slots
    iMutex.Wait();             // enter critical region
    TInt ret = RemoveItem();   // take item from the buffer
    iMutex.Signal();           // leave critical region

    return ret;
}

void CIntQueue::PutItem(TInt aValue)
{
    TInt err = iQueue.Append(aValue); // put new item to the buffer
    __ASSERT_ALWAYS((err == KErrNone), User::Invariant());
}

TInt CIntQueue::RemoveItem()
{
    TInt ret = iQueue[0];      // take item from the buffer
    iQueue.Remove(0);

    iCompressCounter++;
    if (iCompressCounter == KCompressThreshold)
    {
        iQueue.GranularCompress();
        iCompressCounter = 0;
    }

    return ret;
}

TBool CIntQueue::IsEmpty()
{
    return (iQueue.Count() == 0);
}

/* ******************************************** */

CEventQueue* CEventQueue::GetQueue(TInt aThread)
{
    return getEsStaticData()->mLocks[aThread];
}

CEventQueue::CEventQueue()
{
}

void CEventQueue::ConstructL()
{
    iEventQueue = CIntQueue::NewL();
}

CEventQueue::~CEventQueue()
{
    while (!iEventQueue->IsEmpty())
    {
        TInt event = GetNext();
        if (event != KErrServerTerminated)
        {
            CJavaEventBase* ev = reinterpret_cast<CJavaEventBase*>(event);
            CJavaEventSourceBase& source = ev->Object();

            if (ev->IsDisposable())
            {
                delete ev;
            }
            source.OnVmClose();
        }
    }
    delete iEventQueue;
}

// Notify that new event has been posted. Called from the native code.
// aEventServer is C++ pointer to event server object and aThreadNumber indicates the event type
// and therefore which dispatcher thread should handle it

void CEventQueue::NotifyServer(TInt aEventServer, TInt aThreadNumber)
{
    ASSERT(aThreadNumber < KDispatchThreads);

    // Add event server of an event to the queue
    GetQueue(aThreadNumber)->iEventQueue->Enqueue(aEventServer);

}

// Get the next event server from the queue; wait if there are no events to be dispatched
TInt CEventQueue::GetNext()
{
    return iEventQueue->Dequeue();
}

void CEventQueue::OnCloseThreads()
{
    //sends fake zero events to exit from native loop back to java
    for (int i = 0; i < KDispatchThreads; i++)
    {
        GetQueue(i)->NewEvent(0);
    }
}

// Add event server of an event to the queue
void CEventQueue::NewEvent(TInt aEventServer)
{
    iEventQueue->Enqueue(aEventServer);
}


void CEventQueue::Cleanup()
{
    EventServerGlobals* esd = getEsStaticData();
    for (int i = 0; i < KDispatchThreads; i++)
    {
        delete esd->mLocks[i];
        esd->mLocks[i] = 0;
    }
}

void CEventQueue::InitL()
{
    EventServerGlobals* esd = getEsStaticData();
    for (int i = 0; i < KDispatchThreads; i++)
    {
        if (esd->mLocks[i] == 0)
        {
            CEventQueue* self = new(ELeave) CEventQueue();
            CleanupStack::PushL(self);
            self->ConstructL();
            CleanupStack::Pop();
            esd->mLocks[i] = self;
        }
    }
}

