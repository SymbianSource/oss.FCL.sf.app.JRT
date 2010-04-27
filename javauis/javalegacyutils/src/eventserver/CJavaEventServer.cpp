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

#include <eikenv.h>
#include <eikappui.h>
#include <eiksrvc.h>
#include <stdio.h>

#include "JesServer.h"
#include "CJavaEventServer.h"



const TInt KMaxUserName=64;
const TInt KMaxJesName=3+1+8+1+KMaxUserName+1+8;


class TJesName : public TBuf<KMaxJesName>
{
public:
    TJesName(const TDesC& aUserName);
    TJesName(const TDesC& aUserName,TInt aHandle);
private:
    void Build(const TDesC& aUserName);
};

class TJesParam : public MEventServer
{
public:
    virtual void Started(TInt aError, RServer2 aServer)
    {
        iServerHandle = aServer;
        iThread.RequestComplete(iStatus,aError);
    }
    virtual TAny* Arg() const
    {
        return iArg;
    }
public:
    RThread iThread;
    RServer2 iServerHandle;
    TRequestStatus* iStatus;
    TAny* iArg;
};

class CJesSession : public CSession2
{
    class TExecute
    {
    public:
        inline TInt operator()() const;
    private:
        TInt(*iFunc)(TAny*,TAny*,TAny*,TAny*,TAny*,TAny*,TAny*,TAny*,TAny*);
        TAny* iParam[9];
    };
private:
    void ServiceL(const RMessage2& aMessage);
    static TInt ExecuteTrap(const TExecute& aExecute);
};

class CJesScheduler : public CActiveScheduler, public MJesShutdown
{
public:
    static TInt Create(RServer2& serverHandle);
    virtual void Shutdown();
private:
    CJesScheduler();
    static void CreateL(RServer2& serverHandle);
private:
    CJesServer   iServer;
};

// Class TJesName

// The thread name is made up of the user name, the process Id and
// the address of this object.

_LIT(KName,"jes-");

TJesName::TJesName(const TDesC& aUserName)
//
// Construct the base Java Event Server name
//
{
    Build(aUserName);
}

TJesName::TJesName(const TDesC& aUserName,TInt aHandle)
//
// Construct the full Java Event Server name
//
{
    Build(aUserName);
    AppendNum(aHandle,EHex);
}

void TJesName::Build(const TDesC& aUserName)
//
// Create the base of the server name
//
{
    Copy(KName);
    TProcessId id(RProcess().Id());
    AppendNum(*reinterpret_cast<TInt*>(&id),EHex);
    Append('-');
    Append(aUserName);
    Append('@');
}


// class RJess

TInt RJess::Connect(RServer2& aServer)
//
// Establish an IPC session with the event server and share it
//
{
    TInt r=CreateSession(aServer, TVersion(), CJavaEventBase::ELastPriority + 1);
    if (r==KErrNone)
    {
        r=ShareAuto();
        if (r!=KErrNone)
            Close();
    }
    return r;
}

// class TJavaEventServer

inline TJavaEventServer::TJavaEventServer(CJavaEventServer* aServer)
        :iServer(aServer)
{}

EXPORT_C TJavaEventServer TJavaEventServer::NewL(const TDesC& aName, TThreadFunction aServerThread, TAny* aServerArg)
{
    return CJavaEventServer::NewL(aName, aServerThread, aServerArg, 0, 0);
}

EXPORT_C TJavaEventServer TJavaEventServer::NewL(const TDesC& aName, TThreadFunction aServerThread, TAny* aServerArg,
        TUint aStackSize, RAllocator* aHeap)
{
    return CJavaEventServer::NewL(aName, aServerThread, aServerArg, aStackSize, aHeap);
}

EXPORT_C void TJavaEventServer::Shutdown()
{
    iServer->Shutdown();
}

// class CJesServer

EXPORT_C CJesServer::CJesServer(MJesShutdown* aShutdown):
//
// The Java Event Server shares the session between all thread clients
//
        CServer2(0,ESharableSessions)
        , iShutdown(aShutdown)
{
}

CJesServer::~CJesServer()
{
}

CSession2* CJesServer::NewSessionL(const TVersion& /*version*/, const RMessage2& /*message*/) const
{
    return new(ELeave) CJesSession();
}

void CJesServer::Shutdown()
{
    iShutdown.Start();
}


// class CJesScheduler
CJesScheduler::CJesScheduler()
        : iServer(this)
{
}

void CJesScheduler::Shutdown()
{
    CActiveScheduler::Stop();
}

void CJesScheduler::CreateL(RServer2& serverHandle)
{
    CJesScheduler* s=new(ELeave) CJesScheduler;
    CActiveScheduler::Install(s);
    s->iServer.StartL(KNullDesC);
    serverHandle = s->iServer.Server();
}

TInt CJesScheduler::Create(RServer2& serverHandle)
//
// Create the Java Event thread scheduler and server
//
{
    TRAPD(r,CreateL(serverHandle));
    return r;
}

// java-side
void CJavaEventServer::AddRef()
{
    iMutex.Wait();
    ++iRef;
    iMutex.Signal();
}

// java-side
void CJavaEventServer::RemoveRef()
{
    iMutex.Wait();
    if ((--iRef == 0) && iShutdown)
    {
        iSession.Shutdown();
    }
    iMutex.Signal();
}

// java-side - consider purging event queue.
void CJavaEventServer::Shutdown()
{
    iMutex.Wait();
    iShutdown=ETrue;
    if (0 == iRef)
    {
        iSession.Shutdown();
    }
    iMutex.Signal();
}

TInt CJavaEventServer::ServerThread(TAny* aParam)
//
// The thread function for the Java event server thread
// This initialises the thread environment before reporting success
// and then starting its scheduler
//
{
    MEventServer* server  = static_cast<MEventServer*>(aParam);

    TInt err = KErrNoMemory;

    RServer2 serverHandle;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    if (cleanup)
    {
        err = CJesScheduler::Create(serverHandle);
    }

    server->Started(err, serverHandle);   // don't touch server again - its gone.
    server = NULL;
    if (err==KErrNone)
    {
        CActiveScheduler::Start();
    }
    delete CActiveScheduler::Current();         // will delete all objects
    delete cleanup;
    return 0;
}

#ifndef JAVA_STACKSIZE
#define JAVA_STACKSIZE 0x2000
#endif

void CJavaEventServer::ConstructL(const TDesC& aUserName, TThreadFunction aServerThread, TAny* aServerArg,
                                  TUint aStackSize, RAllocator* aHeap)
//
// Main construction of the event server
//
{
    User::LeaveIfError(iMutex.CreateLocal());

    if (NULL == aServerThread)
    {
        // default thread function.
        aServerThread = &ServerThread;
    }

    TJesParam param;
    param.iArg = aServerArg;
    User::LeaveIfError(param.iThread.Duplicate(param.iThread,EOwnerProcess));   // make the thread handle process relative
    CleanupClosePushL(param.iThread);
    TRequestStatus s(KRequestPending);
    param.iStatus=&s;
    RThread js;
    if (aStackSize == 0)
    {
        aStackSize = JAVA_STACKSIZE;
    }

    TJesName name(aUserName,TJavaEventServer::Handle(this));
    User::LeaveIfError(js.Create(name,aServerThread,aStackSize,aHeap,&param,EOwnerProcess));
//    js.SetPriority(EPriorityMore);
    js.Resume();
    js.Close();
    User::WaitForRequest(s);            // wait for signal from server thread
    User::LeaveIfError(s.Int());
    CleanupStack::PopAndDestroy();      // param.iThread
    User::LeaveIfError(iSession.Connect(param.iServerHandle));
}

CJavaEventServer::CJavaEventServer()
{
}

CJavaEventServer::~CJavaEventServer()
{
    iSession.Close();
    iMutex.Close();
}

CJavaEventServer* CJavaEventServer::NewL(const TDesC& aUserName,TThreadFunction aServerThread, TAny* aServerArg,
        TUint aStackSize, RAllocator* aHeap)
{
    CJavaEventServer* self=new(ELeave) CJavaEventServer;
    CleanupStack::PushL(self);
    self->ConstructL(aUserName, aServerThread, aServerArg, aStackSize, aHeap);
    CleanupStack::Pop();
    return self;
}

// class CJavaEventBase

void CJavaEventBase::Run(JNIEnv& aJni)
//
// Dispatch this event and destroy the event object
//
{
    CJavaEventSourceBase& source = Object();

    // check if the event type is disposable before calling java callback
    // because if it's reusable it can be destroyed during servicing process
    TBool isDisposable = IsDisposable();

    if (!source.IsDisposed() && !aJni.IsSameObject(source.Peer(),0))
    {
        if (aJni.PushLocalFrame(16)==0)
        {
            Dispatch(aJni);
            if (aJni.ExceptionCheck())
            {   // Report any exceptions that were generated and clear them from the JNI environment
                aJni.ExceptionDescribe();
                aJni.ExceptionClear();
            }
            aJni.PopLocalFrame(0);
        }
    }
    if (isDisposable)
    {
        delete this;
    }
    // Close must be called last, because it may result in the destruction of this
    // event object if the event is a reusable event
    source.Close(aJni);
}

// class CJesSession

inline TInt CJesSession::TExecute::operator()() const
{
    return iFunc(iParam[0],iParam[1],iParam[2],iParam[3],iParam[4],iParam[5],iParam[6],iParam[7],iParam[8]);
}

TInt CJesSession::ExecuteTrap(const TExecute& aExecute)
//
// Execute the client function inside a trap harness
//
{
    TRAPD(r,aExecute());
    return r;
}


void CJesSession::ServiceL(const RMessage2& m)
//
// Invoke the requested client function
//
{
    switch (m.Function())
    {
    case EJessExecute:
    {
        const TExecute& e = *static_cast<const TExecute*>(m.Ptr0());
        TInt result = e();
        m.Complete(result != KRequestPending ? result : KErrNotSupported);
    }
    break;

    case EJessExecuteTrap:
        m.Complete(ExecuteTrap(*static_cast<const TExecute*>(m.Ptr0())));
        break;

    case EJessShutdown:
        ((CJesServer*)Server())->Shutdown();
        m.Complete(KErrNone);
        break;
    }
}

CJesShutdown::CJesShutdown(MJesShutdown* aShutdown)
        : CActive(CActive::EPriorityStandard)
        , iShutdown(aShutdown)
{
}

void CJesShutdown::Start()
{
    CActiveScheduler::Add(this);
    iStatus=KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
}

void CJesShutdown::RunL()
{
    ASSERT(iShutdown);
    iShutdown->Shutdown();
    iShutdown = NULL;
}

TInt CJesShutdown::RunError(TInt aError)
{
    if (aError == KLeaveExit)
    {
        return aError;
    }
    return KErrNone;
}

void CJesShutdown::DoCancel()
{
    // nop
}

// Helper method to trace output (int) to Java side, eg. System.out
void CJavaEventServer::Trace(JNIEnv& aJni, TInt value)
{
    jclass clazz = aJni.FindClass("com/symbian/lcdjava/lang/SystemExtensions");
    jmethodID method = aJni.GetStaticMethodID(clazz, "trace" , "(I)V");
    aJni.CallStaticVoidMethod(clazz, method, value);
}

// Notify Java thread of a new event
void CJavaEventServer::NotifyJavaCall(TInt aEvent, TInt aPriority)
{
    CEventQueue::NotifyServer(aEvent, aPriority);
}


void CJavaEventServer::Cleanup(RArray<TInt>& aServerHandles)
{
    CEventQueue::Cleanup();

    // Loop through event server instances and remove each
    for (TInt i = aServerHandles.Count() - 1; i >= 0; --i)
    {
        TInt handle = aServerHandles[i];
        aServerHandles.Remove(i);
        delete JavaUnhand<CJavaEventServer>(handle);
    }
}

