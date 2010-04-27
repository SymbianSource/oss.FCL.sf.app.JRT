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
* Description:  ?Description
*
*/


#ifndef FS_METHODCALL_H
#define FS_METHODCALL_H

#include "fs_methodwrappers.h"

namespace java
{
namespace util
{
class FunctionServer;
}
}


// Disable bothersome warning
//#pragma warning(disable: 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


/*
 * Note: Call() used for calling const methods cause errors in Lint and are
 * thus wrapped in a #ifndef / #endif clause to prevent Lint from seeing them.
 */


/**
 * A functor executing object
 *
 * An executor is a function that takes a functor as argument and executes it
 * in the UI thread.
 */
typedef void (*Executor)(const Functor& functor, java::util::FunctionServer* functionServer, int* leaveResult);

class MethodCaller
{
public:
    OS_IMPORT static void ExecuteRegularFunctor(const Functor& functor, java::util::FunctionServer* functionServer, int* res);
    OS_IMPORT static void ExecuteLeavingFunctorL(const Functor& functor, java::util::FunctionServer* functionServer, int* res);
    OS_IMPORT static void ExecuteTrapingFunctorL(const Functor& functor, java::util::FunctionServer* functionServer, int* res);
};


/**
 * Utility class that constructs a method call wrapping object and
 *        passes it on to an executor.
 *
 * <b>Known limitations</b>:
 * - methods that return references are not handled.
 */
template<Executor Execute>
class MethodCall
{
public:
    /**
     * Calls a method with no argument and a return value
     */
    template<class S, class R, class C>
    static inline void Call(S& result, C* object, R(C::*method)(), java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR0<S,R,C>(result, *object, method), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with no argument and a return value
     */
    template<class S, class R, class C>
    static inline void Call(S& result, const C* object, R(C::*method)() const, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)()>(method), functionServer);
    }
#endif // _lint


    /**
     * Calls a method with no argument and no return value
     */
    template<class C>
    static inline void Call(C* object, void (C::*method)(), java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper0<C>(*object, method), functionServer, 0);
    }

#ifndef _lint
    /**
     * Calls a const method with no argument and no return value
     */
    template<class C>
    static inline void Call(const C* object, void (C::*method)() const, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)()>(method), functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 1 argument and a return value
     */
    template<class S, class R, class C, class A1, class B1>
    static inline void Call(S& result, C* object, R(C::*method)(A1), B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR1<S,R,C,A1>(result, *object, method, arg1), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 1 argument and a return value
     */
    template<class S, class R, class C, class A1, class B1>
    static inline void Call(S& result, const C* object, R(C::*method)(A1) const, B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1)>(method), arg1, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 1 argument and no return value
     */
    template<class C, class A1, class B1>
    static inline void Call(C* object, void (C::*method)(A1), B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper1<C,A1>(*object, method, arg1), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 1 argument and no return value
     */
    template<class C, class A1, class B1>
    static inline void Call(const C* object, void (C::*method)(A1) const, B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1)>(method), arg1, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 2 arguments and no return value
     */
    template<class C, class A1, class A2, class B1, class B2>
    static inline void Call(C* object, void (C::*method)(A1,A2), B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper2<C,A1,A2>(*object, method, arg1, arg2), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 2 arguments and no return value
     */
    template<class C, class A1, class A2, class B1, class B2>
    static inline void Call(const C* object, void (C::*method)(A1,A2) const, B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1,A2)>(method), arg1, arg2, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 2 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class B1, class B2>
    static inline void Call(S& result, C* object, R(C::*method)(A1,A2), B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR2<S,R,C,A1,A2>(result, *object, method, arg1, arg2), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 2 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class B1, class B2>
    static inline void Call(S& result, const C* object, R(C::*method)(A1,A2) const, B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1,A2)>(method), arg1, arg2, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 3 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(C* object, void (C::*method)(A1,A2,A3), B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper3<C,A1,A2,A3>(*object, method, arg1, arg2, arg3), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 3 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(const C* object, void (C::*method)(A1,A2,A3) const, B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1,A2,A3)>(method), arg1, arg2, arg3, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 3 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(S& result, C* object, R(C::*method)(A1,A2,A3), B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR3<S,R,C,A1,A2,A3>(result, *object, method, arg1, arg2, arg3), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 3 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(S& result, const C* object, R(C::*method)(A1,A2,A3) const, B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1,A2,A3)>(method), arg1, arg2, arg3, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 4 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(C* object, void (C::*method)(A1,A2,A3,A4), B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper4<C,A1,A2,A3,A4>(*object, method, arg1, arg2, arg3, arg4), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 4 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(const C* object, void (C::*method)(A1,A2,A3,A4) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1, A2, A3, A4)>(method), arg1, arg2, arg3, arg4, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 4 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(S& result, C* object, R(C::*method)(A1,A2,A3,A4), B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR4<S,R,C,A1,A2,A3,A4>(result, *object, method, arg1, arg2, arg3, arg4), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 4 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(S& result, const C* object, R(C::*method)(A1,A2,A3,A4) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1,A2,A3,A4)>(method), arg1, arg2, arg3, arg4, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 5 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(C* object, void (C::*method)(A1,A2,A3,A4,A5), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper5<C,A1,A2,A3,A4,A5>(*object, method, arg1, arg2, arg3, arg4, arg5), functionServer, 0);
    }


    /**
     * Calls a method with 5 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(S& result, C* object, R(C::*method)(A1,A2,A3,A4,A5), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR5<S,R,C,A1,A2,A3,A4,A5>(result, *object, method, arg1, arg2, arg3, arg4, arg5), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 5 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(const C* object, void (C::*method)(A1,A2,A3,A4,A5) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1, A2, A3, A4, A5)>(method), arg1, arg2, arg3, arg4, arg5, functionServer);
    }
#endif // _lint


#ifndef _lint
    /**
     * Calls a const method with 5 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(S& result, const C* object, R(C::*method)(A1,A2,A3,A4,A5) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5)>(method), arg1, arg2, arg3, arg4, arg5, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 6 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(C* object, void (C::*method)(A1,A2,A3,A4,A5,A6), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper6<C,A1,A2,A3,A4,A5,A6>(*object, method, arg1, arg2, arg3, arg4, arg5, arg6), functionServer, 0);
    }


    /**
     * Calls a method with 6 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(S& result, C* object, R(C::*method)(A1,A2,A3,A4,A5,A6), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR6<S,R,C,A1,A2,A3,A4,A5,A6>(result, *object, method, arg1, arg2, arg3, arg4, arg5, arg6), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 6 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(S& result, const C* object, R(C::*method)(A1,A2,A3,A4,A5,A6) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6)>(method), arg1, arg2, arg3, arg4, arg5, arg6, functionServer);
    }
#endif // _lint


#ifndef _lint
    /**
     * Calls a const method with 6 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(const C* object, void (C::*method)(A1,A2,A3,A4,A5,A6) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1, A2, A3, A4, A5, A6)>(method), arg1, arg2, arg3, arg4, arg5, arg6, functionServer);
    }
#endif // _lint


    /**
     * Calls a method with 7 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(C* object, void (C::*method)(A1,A2,A3,A4,A5,A6,A7), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapper7<C,A1,A2,A3,A4,A5,A6,A7>(*object, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7), functionServer, 0);
    }


    /**
     * Calls a method with 7 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(S& result, C* object, R(C::*method)(A1,A2,A3,A4,A5,A6,A7), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperR7<S,R,C,A1,A2,A3,A4,A5,A6,A7>(result, *object, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7), functionServer, 0);
    }


#ifndef _lint
    /**
     * Calls a const method with 7 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(S& result, const C* object, R(C::*method)(A1,A2,A3,A4,A5,A6,A7) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(result, const_cast<C*>(object), reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6,A7)>(method), arg1, arg2, arg3, arg4, arg5, arg6, arg7, functionServer);
    }
#endif // _lint

#ifndef _lint
    /**
     * Calls a const method with 7 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(const C* object, void (C::*method)(A1,A2,A3,A4,A5,A6,A7) const, B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Call(const_cast<C*>(object), reinterpret_cast<void (C::*)(A1, A2, A3, A4, A5, A6, A7)>(method), arg1, arg2, arg3, arg4, arg5, arg6, arg7, functionServer);
    }
#endif // _lint



//*****LOCAL CALLS*****************

    /********************************WITHOUT RETURN VALUE************************
    /**
     * Calls a local method with 0 arguments and no return value
     */
    static inline void Call(void (*method)(), java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal0(method), functionServer, 0);
    }


    /**
     * Calls a local method with 1 arguments and no return value
     */
    template<class A1, class B1>
    static inline void Call(void (*method)(A1), B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal1<A1>(method, arg1), functionServer, 0);
    }

    /**
     * Calls a local method with 2 arguments and no return value
     */
    template<class A1, class A2, class B1, class B2>
    static inline void Call(void (*method)(A1, A2), B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal2<A1,A2>(method, arg1, arg2), functionServer, 0);
    }

    /**
     * Calls a local method with 3 arguments and no return value
     */
    template<class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(void (*method)(A1,A2,A3), B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal3<A1,A2,A3>(method, arg1, arg2, arg3), functionServer, 0);
    }

    /**
     * Calls a local method with 4 arguments and no return value
     */
    template<class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(void (*method)(A1,A2,A3,A4), B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal4<A1,A2,A3,A4>(method, arg1, arg2, arg3, arg4), functionServer, 0);
    }

    /**
     * Calls a local method with 5 arguments and no return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(void (*method)(A1,A2,A3,A4,A5), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal5<A1,A2,A3,A4,A5>(method, arg1, arg2, arg3, arg4, arg5), functionServer, 0);
    }

    /**
     * Calls a local method with 6 arguments and no return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(void (*method)(A1,A2,A3,A4,A5,A6), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal6<A1,A2,A3,A4,A5,A6>(method, arg1, arg2, arg3, arg4, arg5, arg6), functionServer, 0);
    }

    /**
     * Calls a local method with 7 arguments and no return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(void (*method)(A1,A2,A3,A4,A5,A6,A7), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal7<A1,A2,A3,A4,A5,A6,A7>(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7), functionServer, 0);
    }

    /**
     * Calls a local method with 8 arguments and no return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8>
    static inline void Call(void (*method)(A1,A2,A3,A4,A5,A6,A7,A8), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, B8& arg8, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal8<A1,A2,A3,A4,A5,A6,A7,A8>(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), functionServer, 0);
    }

    /**
     * Calls a local method with 9 arguments and no return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9>
    static inline void Call(void (*method)(A1,A2,A3,A4,A5,A6,A7,A8,A9), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, B8& arg8, B9& arg9, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        Execute(MethodWrapperLocal9<A1,A2,A3,A4,A5,A6,A7,A8,A9>(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), functionServer, 0);
    }

    /********************************WITH RETURN VALUE************************

    /**
     * Calls a local method with 0 arguments and a return value
     */
    template< class R>
    static inline R Call(R(*method)(), java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR0<R>(result, method), functionServer, 0);
        return result;
    }


    /**
     * Calls a local method with 1 arguments and a return value
     */
    template< class R, class A1, class B1>
    static inline R Call(R(*method)(A1), B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR1<R,A1>(result, method, arg1), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 2 arguments and a return value
     */
    template< class R, class A1, class A2, class B1, class B2>
    static inline R Call(R(*method)(A1, A2), B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR2<R,A1,A2>(result, method, arg1, arg2), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 3 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline R Call(R(*method)(A1,A2,A3), B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR3<R,A1,A2,A3>(result, method, arg1, arg2, arg3), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 4 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline R Call(R(*method)(A1,A2,A3,A4), B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR4<R,A1,A2,A3,A4>(result, method, arg1, arg2, arg3, arg4), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 5 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline R Call(R(*method)(A1,A2,A3,A4,A5), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR5<R,A1,A2,A3,A4,A5>(result, method, arg1, arg2, arg3, arg4, arg5), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 6 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline R Call(R(*method)(A1,A2,A3,A4,A5,A6), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR6<R,A1,A2,A3,A4,A5,A6>(result, method, arg1, arg2, arg3, arg4, arg5, arg6), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 7 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline R Call(R(*method)(A1,A2,A3,A4,A5,A6,A7), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR7<R,A1,A2,A3,A4,A5,A6,A7>(result, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 8 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8>
    static inline R Call(R(*method)(A1,A2,A3,A4,A5,A6,A7,A8), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, B8& arg8, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR8<R,A1,A2,A3,A4,A5,A6,A7,A8>(result, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), functionServer, 0);
        return result;
    }

    /**
     * Calls a local method with 9 arguments and a return value
     */
    template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9>
    static inline R Call(R(*method)(A1,A2,A3,A4,A5,A6,A7,A8,A9), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, B8& arg8, B9& arg9, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        R result;
        Execute(MethodWrapperLocalR9<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>(result, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), functionServer, 0);
        return result;
    }

    /********************************TRAP************************


    /**
     * Calls a local method with 0 arguments and a TRAP result in return value
     */
    static inline int TrapCall(void (*method)(), java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal0(method), functionServer, &result);
        return result;
    }


    /**
     * Calls a local method with 1 arguments and a TRAP result in return value
     */
    template<class A1, class B1>
    static inline int TrapCall(void (*method)(A1), B1& arg1, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal1<A1>(method, arg1), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 2 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class B1, class B2>
    static inline int TrapCall(void (*method)(A1, A2), B1& arg1, B2& arg2, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal2<A1,A2>(method, arg1, arg2), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 3 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class B1, class B2, class B3>
    static inline int TrapCall(void (*method)(A1,A2,A3), B1& arg1, B2& arg2, B3& arg3, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal3<A1,A2,A3>(method, arg1, arg2, arg3), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 4 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline int TrapCall(void (*method)(A1,A2,A3,A4), B1& arg1, B2& arg2, B3& arg3, B4& arg4, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal4<A1,A2,A3,A4>(method, arg1, arg2, arg3, arg4), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 5 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline int TrapCall(void (*method)(A1,A2,A3,A4,A5), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal5<A1,A2,A3,A4,A5>(method, arg1, arg2, arg3, arg4, arg5), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 6 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline int TrapCall(void (*method)(A1,A2,A3,A4,A5,A6), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal6<A1,A2,A3,A4,A5,A6>(method, arg1, arg2, arg3, arg4, arg5, arg6), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 7 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline int TrapCall(void (*method)(A1,A2,A3,A4,A5,A6,A7), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal7<A1,A2,A3,A4,A5,A6,A7>(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 8 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8>
    static inline int TrapCall(void (*method)(A1,A2,A3,A4,A5,A6,A7,A8), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, B8& arg8, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal8<A1,A2,A3,A4,A5,A6,A7,A8>(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), functionServer, &result);
        return result;
    }

    /**
     * Calls a local method with 9 arguments and a TRAP result in return value
     */
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9>
    static inline int TrapCall(void (*method)(A1,A2,A3,A4,A5,A6,A7,A8,A9), B1& arg1, B2& arg2, B3& arg3, B4& arg4, B5& arg5, B6& arg6, B7& arg7, B8& arg8, B9& arg9, java::util::FunctionServer* functionServer)
    {
        JELOG2(EUtils);
        int result;
        Execute(MethodWrapperLocal9<A1,A2,A3,A4,A5,A6,A7,A8,A9>(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), functionServer, &result);
        return result;
    }


};



/**
 * Specialisation of MethodCall for non-leaving methods
 */
typedef MethodCall<MethodCaller::ExecuteRegularFunctor> RegularMethodCall;


/**
 * Specialisation of MethodCall for leaving methods
 */
typedef MethodCall<MethodCaller::ExecuteLeavingFunctorL> LeavingMethodCall;


/**
 * Specialisation of MethodCall for traping methods
 */
typedef MethodCall<MethodCaller::ExecuteTrapingFunctorL> TrapingMethodCall;


/**
 * Helper macro to ease calling a regular method
 */
#define CallMethod RegularMethodCall::Call


/**
 * Helper macro to ease calling a leaving method
 */
#define CallMethodL LeavingMethodCall::Call

/**
 * Helper macro to ease calling a leaving method with trap
 */
#define CallMethodTrap TrapingMethodCall::TrapCall


#endif // FS_METHODCALL_H

