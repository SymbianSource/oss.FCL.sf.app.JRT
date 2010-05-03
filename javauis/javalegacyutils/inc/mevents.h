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


#ifndef MEVENTS_H
#define MEVENTS_H

#include <jutils.h>

#include "logger.h"

/**
 * Callback interface for event server thread.
 */
class MEventServer
{
public:

    /**
     * Event server started.
     */
    virtual void Started(TInt aError, RServer2 server) = 0;

    /**
     * Argument for event server thread.
     */
    virtual TAny* Arg() const = 0;
};

class CJavaEventServer;
class TJavaEventServer
{
    friend class CJavaEventServer;
public:
    IMPORT_C static TJavaEventServer NewL(const TDesC& aName, TThreadFunction aServerThread=NULL, TAny* aServerArg=NULL);
    IMPORT_C static TJavaEventServer NewL(const TDesC& aName, TThreadFunction aServerThread, TAny* aServerArg, TUint aStackSize, RAllocator* aHeap = 0);

    inline TJavaEventServer()
    {}
    inline TJavaEventServer(TInt aServer)
            :iServer(JavaUnhand<CJavaEventServer>(aServer))
    {}
    inline CJavaEventServer* operator->() const
    {
        return iServer;
    }
    static inline TInt Handle(CJavaEventServer* aServer)
    {
        return JavaMakeHandle(aServer);
    }
    inline TInt Handle() const
    {
        return Handle(iServer);
    }
    IMPORT_C void Shutdown();
private:
    inline TJavaEventServer(CJavaEventServer* aServer);
private:
    CJavaEventServer* iServer;
};

#pragma warning(disable: 4786) // Truncated browser info for long mangled names

class CJavaEventBase;
class CJavaEventSourceBase : public CBase
{
    enum {KMaxExecuteArgumentSize=32};
    static inline void CallCheck(TInt aSize)
    {       // fault at run time for invalid JpServer calls
#pragma warning(disable: 4127)
        if (aSize>KMaxExecuteArgumentSize) Panic(aSize);
#pragma warning(default: 4127)
    }
public:
    inline TInt Handle() const
    {
        return JavaMakeHandle(this);
    }
    inline jweak Peer() const
    {
        return iPeer;
    }
    inline TJavaEventServer Server() const
    {
        return iServer;
    }
    inline void Dispose(JNIEnv& aJni)
    {
        iDead=ETrue;
        Close(aJni);
    }
    inline TBool IsDisposed() const
    {
        return iDead;
    }
    TBool Open();
    IMPORT_C void Close(JNIEnv& aJni);


    void OnVmClose();

    template <class R,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline R Execute(R(*aFuncL)(T1,T2,T3,T4,T5,T6,T7,T8),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6,T7 aT7,T8 aT8)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6)+sizeof(T7)+sizeof(T8));
        return (R)DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6,aT7,aT8);
    }
    template <class R,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline R Execute(R(*aFuncL)(T1,T2,T3,T4,T5,T6,T7),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6,T7 aT7)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6)+sizeof(T7));
        return (R)DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6,aT7);
    }
    template <class R,class T1,class T2,class T3,class T4,class T5,class T6>
    inline R Execute(R(*aFuncL)(T1,T2,T3,T4,T5,T6),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6));
        return (R)DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6);
    }
    template <class R,class T1,class T2,class T3,class T4,class T5>
    inline R Execute(R(*aFuncL)(T1,T2,T3,T4,T5),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5));
        return (R)DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5);
    }
    template <class R,class T1,class T2,class T3,class T4>
    inline R Execute(R(*aFunc)(T1,T2,T3,T4),T1 aT1,T2 aT2,T3 aT3,T4 aT4)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4));
        return (R)DoExecute((TAny*)aFunc,aT1,aT2,aT3,aT4);
    }
    template <class R,class T1,class T2,class T3>
    inline R Execute(R(*aFunc)(T1,T2,T3),T1 aT1,T2 aT2,T3 aT3)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3));
        return (R)DoExecute((TAny*)aFunc,aT1,aT2,aT3);
    }
    template <class R,class T1,class T2>
    inline R Execute(R(*aFunc)(T1,T2),T1 aT1,T2 aT2)
    {
        CallCheck(sizeof(T1)+sizeof(T2));
        return (R)DoExecute((TAny*)aFunc,aT1,aT2);
    }
    template <class R,class T1>
    inline R Execute(R(*aFunc)(T1),T1 aT1)
    {
        CallCheck(sizeof(T1));
        return (R)DoExecute((TAny*)aFunc,aT1);
    }
    template <class R>
    inline R Execute(R(*aFunc)())
    {
        return (R)DoExecute((TAny*)aFunc);
    }

    template <class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void ExecuteV(void (*aFuncL)(T1,T2,T3,T4,T5,T6,T7),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6,T7 aT7)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6)+sizeof(T7));
        DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6,aT7);
    }
    template <class T1,class T2,class T3,class T4,class T5,class T6>
    inline void ExecuteV(void (*aFuncL)(T1,T2,T3,T4,T5,T6),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6));
        DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6);
    }
    template <class T1,class T2,class T3,class T4,class T5>
    inline void ExecuteV(void (*aFuncL)(T1,T2,T3,T4,T5),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5));
        DoExecute((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5);
    }
    template <class T1,class T2,class T3,class T4>
    inline void ExecuteV(void (*aFunc)(T1,T2,T3,T4),T1 aT1,T2 aT2,T3 aT3,T4 aT4)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4));
        DoExecute((TAny*)aFunc,aT1,aT2,aT3,aT4);
    }
    template <class T1,class T2,class T3>
    inline void ExecuteV(void (*aFunc)(T1,T2,T3),T1 aT1,T2 aT2,T3 aT3)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3));
        DoExecute((TAny*)aFunc,aT1,aT2,aT3);
    }
    template <class T1,class T2>
    inline void ExecuteV(void (*aFunc)(T1,T2),T1 aT1,T2 aT2)
    {
        CallCheck(sizeof(T1)+sizeof(T2));
        DoExecute((TAny*)aFunc,aT1,aT2);
    }
    template <class T1>
    inline void ExecuteV(void (*aFunc)(T1),T1 aT1)
    {
        CallCheck(sizeof(T1));
        DoExecute((TAny*)aFunc,aT1);
    }
    inline void ExecuteV(void (*aFunc)())
    {
        DoExecute((TAny*)aFunc);
    }

    template <class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3,T4,T5,T6,T7,T8,T9),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6,T7 aT7,T8 aT8,T9 aT9)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6)+sizeof(T7)+sizeof(T8)+sizeof(T9));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6,aT7,aT8,aT9);
    }
    template <class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3,T4,T5,T6,T7,T8),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6,T7 aT7,T8 aT8)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6)+sizeof(T7)+sizeof(T8));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6,aT7,aT8);
    }
    template <class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3,T4,T5,T6,T7),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6,T7 aT7)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6)+sizeof(T7));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6,aT7);
    }
    template <class T1,class T2,class T3,class T4,class T5,class T6>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3,T4,T5,T6),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5,T6 aT6)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5,aT6);
    }
    template <class T1,class T2,class T3,class T4,class T5>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3,T4,T5),T1 aT1,T2 aT2,T3 aT3,T4 aT4,T5 aT5)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3,aT4,aT5);
    }
    template <class T1,class T2,class T3,class T4>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3,T4),T1 aT1,T2 aT2,T3 aT3,T4 aT4)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3,aT4);
    }
    template <class T1,class T2,class T3>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2,T3),T1 aT1,T2 aT2,T3 aT3)
    {
        CallCheck(sizeof(T1)+sizeof(T2)+sizeof(T3));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2,aT3);
    }
    template <class T1,class T2>
    inline TInt ExecuteTrap(void (*aFuncL)(T1,T2),T1 aT1,T2 aT2)
    {
        CallCheck(sizeof(T1)+sizeof(T2));
        return DoExecuteTrap((TAny*)aFuncL,aT1,aT2);
    }
    template <class T1>
    inline TInt ExecuteTrap(void (*aFuncL)(T1),T1 aT1)
    {
        CallCheck(sizeof(T1));
        return DoExecuteTrap((TAny*)aFuncL,aT1);
    }
    inline int ExecuteTrap(void (*aFuncL)())
    {
        return DoExecuteTrap((TAny*)aFuncL);
    }
protected:
    IMPORT_C CJavaEventSourceBase();
    IMPORT_C ~CJavaEventSourceBase();
    IMPORT_C void ConstructL(JNIEnv& aJni, jobject aPeer,TJavaEventServer aServer);

    IMPORT_C TBool DoPostEvent(CJavaEventBase* aEvent, TInt aPriority);

    IMPORT_C virtual TBool CheckEvent(CJavaEventBase* aEvent);
private:
    IMPORT_C virtual void FinalizeSvr();

    IMPORT_C virtual void FinalizeJni(JNIEnv& aJni);

    IMPORT_C TInt DoExecute(TAny* aFunc,...);
    IMPORT_C TInt DoExecuteTrap(TAny* aFuncL,...);
    static void DoFinalize(CJavaEventSourceBase* aThis);

    IMPORT_C static void Panic(TInt aPanic);
public:
    class TConstructorBase
    {
    public:
        inline TInt GetHandle()
        {
            CleanupStack::Pop();
            return iObject->Handle();
        }
    protected:
        IMPORT_C TConstructorBase(CJavaEventSourceBase* aObject, JNIEnv& aJni);
    private:
        static void Disposer(TAny* aThis);
    protected:
        CJavaEventSourceBase* iObject;
        JNIEnv& iJni;
    };
private:
    jweak iPeer;
    TJavaEventServer iServer;
    TInt iRef;
    TBool iDead;
};

#pragma warning(default: 4786)

class CJavaEventBase : public CBase
{
    friend class CJavaEventSourceBase;
    friend class CJavaEventServer;

public:
    enum {EEventPriority = 0, ENotifyPriority = 1, ELastPriority = 1};
    enum TDisposability { EDisposableEvent, EReusableEvent };
public:
    inline TBool IsDisposable() const
    {
        return iDisposableState == EDisposableEvent;
    }
protected:
    CJavaEventBase(TDisposability aDisposable)
            : iDisposableState(aDisposable) {}

public:
    inline CJavaEventSourceBase& Object() const
    {
        return *iObject;
    }

    void Run(JNIEnv& aJni);

private:
    virtual void Dispatch(JNIEnv& aJni) =0;
    TSglQueLink iLink;
    CJavaEventSourceBase* iObject;
    TDisposability iDisposableState;
};

template <class T>
class CJavaEvent : public CJavaEventBase
{
protected:
    CJavaEvent(TDisposability aDisposable = EDisposableEvent)
            : CJavaEventBase(aDisposable) {}
    inline T& Object() const
    {
        return static_cast<T&>(CJavaEventBase::Object());
    }
};

template <class T>
class CJavaEventSource : public CJavaEventSourceBase
{
protected:
    class TConstructor : public CJavaEventSourceBase::TConstructorBase
    {
    public:
        inline TConstructor(T* aObject,JNIEnv& aJni)
                :TConstructorBase(aObject,aJni)
        {}
        inline TConstructor(JNIEnv& aJni)
                :TConstructorBase(new(ELeave) T,aJni)
        {}
        inline T* operator->() const
        {
            return static_cast<T*>(iObject);
        }
    };
public:
    inline static T& Unhand(TInt aHandle)
    {
        return *JavaUnhand<T>(aHandle);
    }
    inline TBool PostEvent(CJavaEvent<T>* aEvent, TInt aPriority = CJavaEventBase::EEventPriority)
    {
        return DoPostEvent(aEvent, aPriority);
    }
    static TInt New(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer)
    {
        TRAPD(h,TConstructor self(aJni); self->ConstructL(aJni,aPeer,aServer); h=self.GetHandle());
        return h;
    }
};

#endif // MEVENTS_H
