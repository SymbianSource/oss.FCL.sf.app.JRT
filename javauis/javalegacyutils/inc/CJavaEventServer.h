/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CJAVAEVENTSERVER_H
#define CJAVAEVENTSERVER_H

#include <e32std.h>
#include <e32cmn.h>

#include "mevents.h"

enum {EJessExecute,EJessExecuteTrap, EJessShutdown};

class RJess : public RSessionBase
{
public:
    TInt Connect(RServer2& aServer);
    inline TInt Execute(TAny* aParam)
    {
        return SendReceive(EJessExecute,TIpcArgs(aParam));
    }
    inline TInt ExecuteTrap(TAny* aParam)
    {
        return SendReceive(EJessExecuteTrap,TIpcArgs(aParam));
    }
    inline TInt Shutdown()
    {
        return SendReceive(EJessShutdown, TIpcArgs());
    }
};

class CJavaEventServer : public CBase
{
public:
    ~CJavaEventServer();
    static CJavaEventServer* NewL(const TDesC& aName,TThreadFunction aServerThread, TAny* aServerArg,
                                  TUint aStackSize = 0, RAllocator* aHeap = 0);

    // Notify Java dispatcher thread about new event, called from C++ thread
    static void NotifyJavaCall(TInt aEvent, TInt aPriority);

    // Cleanup the event server stuff, called from Java thread through JNI
    static void Cleanup(RArray<TInt>& aServerHandles);

    // Helper method to do tracing from the native to the Java side
    static void Trace(JNIEnv& aJni, TInt aValue);

    inline RJess& Session()
    {
        return iSession;
    }
    void PostEvent(CJavaEventBase* aEvent, TInt aPriority);
    static TInt ServerThread(TAny* aParam);

    void AddRef();
    void RemoveRef();
    void Shutdown();

private:
    CJavaEventServer();
    void ConstructL(const TDesC& aUserName,TThreadFunction aServerThread, TAny* aServerArg,
                    TUint aStackSize, RAllocator* aHeap);

private:
    TInt  iRef;
    RJess iSession;
    TBool iShutdown;
    RCriticalSection iMutex;

};

// CIntQueue helper class implements an integer FIFO queue The implementation is based on
// fixed-sized array, because we don't want any garbage to be collected here and we don't
// want to grab all the memory due to some misbehaving midlet. Synchronization is provided
// in putItem() and removeItem() functions so it's safe to call them from different threads

class CIntQueue : public CBase
{

public:
    virtual ~CIntQueue();

    static CIntQueue* NewL();        // two-phased constructor
    static CIntQueue* NewLC();       // two-phased constructor

    void  Enqueue(TInt aValue);      // add integer to the end of the queue
    TInt  Dequeue();                 // return integer from the beginning of the queue
    TBool IsEmpty();                 // checks if there are any items in queue

private:
    void PutItem(TInt aItem);        // put new item to the buffer
    TInt RemoveItem();               // take item from the buffer

    CIntQueue();                     // default C++ constructor
    void ConstructL();               // by default Symbian 2nd phase constructor is private

private:
    RArray<TInt>      iQueue;
    RMutex            iMutex;        // protect a critical regions of code
    RSemaphore        iFull;         // counts full buffer slots
    TInt              iCompressCounter; // counts event dequeuing
};

// CEventLock implements queues for manipulating of event and notification handlers.

class CEventQueue : public CBase
{

public:
    virtual ~CEventQueue();

    void NewEvent(TInt aEvent);
    TInt GetNext();

    static void NotifyServer(TInt aEvent, TInt aThreadNumber);
    static void OnCloseThreads();

    static void InitL();
    static void Cleanup();

    static CEventQueue* GetQueue(TInt aThread);
    static const TInt KDispatchThreads = 2;      // two dispatching threads: events and notifications

private:
    CEventQueue();
    void ConstructL();

private:
    CIntQueue* iEventQueue;                      // queue for storing event handles
};

// clean-up event server objects on vm exit
void CleanupEventServer();
// init event server objects on vm start
void InitEventServer();

#endif // CJAVAEVENTSERVER_H

