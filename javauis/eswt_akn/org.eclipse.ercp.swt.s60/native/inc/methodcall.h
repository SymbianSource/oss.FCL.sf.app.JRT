/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef METHODCALL_H
#define METHODCALL_H


#include "methodwrappers.h"


// Disable bothersome warning
//#pragma warning(disable: 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


/*
 * Note: Call() used for calling const methods cause errors in Lint and are
 * thus wrapped in a #ifndef / #endif clause to prevent Lint from seeing them.
 */


class CSwtDisplay;


/**
 * A functor executing object
 *
 * An executor is a function that takes a functor as argument and executes it
 * in the UI thread.
 */
typedef void (*TExecutor)(const MSwtFunctor& aFunctor, CSwtDisplay* aDisplay);


void ExecuteRegularFunctor(const MSwtFunctor& aFunctor, CSwtDisplay* aDisplay);
void ExecuteLeavingFunctorL(const MSwtFunctor& aFunctor, CSwtDisplay* aDisplay);




/**
 * Utility class that constructs a method call wrapping object and
 *        passes it on to an executor.
 *
 * <b>Known limitations</b>:
 * - methods that return references are not handled.
 */
template<TExecutor Execute>
class TMethodCall
{
public:
    /**
     * Calls a method with no argument and a return value
     */
    template<class S, class R, class C>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(), CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR0<S,R,C>(aResult, *aObject, aMethod), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with no argument and a return value
     */
    template<class S, class R, class C>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)() const, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)()>(aMethod), aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with no argument and no return value
     */
    template<class C>
    static inline void Call(C* aObject, void (C::*aMethod)(), CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper0<C>(*aObject, aMethod), aDisplay);
    }

#ifndef _lint
    /**
     * Calls a const method with no argument and no return value
     */
    template<class C>
    static inline void Call(const C* aObject, void (C::*aMethod)() const, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)()>(aMethod), aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 1 argument and a return value
     */
    template<class S, class R, class C, class A1, class B1>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1), B1& aArg1, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR1<S,R,C,A1>(aResult, *aObject, aMethod, aArg1), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 1 argument and a return value
     */
    template<class S, class R, class C, class A1, class B1>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1) const, B1& aArg1, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1)>(aMethod), aArg1, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 1 argument and no return value
     */
    template<class C, class A1, class B1>
    static inline void Call(C* aObject, void (C::*aMethod)(A1), B1& aArg1, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper1<C,A1>(*aObject, aMethod, aArg1), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 1 argument and no return value
     */
    template<class C, class A1, class B1>
    static inline void Call(const C* aObject, void (C::*aMethod)(A1) const, B1& aArg1, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)(A1)>(aMethod), aArg1, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 2 arguments and no return value
     */
    template<class C, class A1, class A2, class B1, class B2>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2), B1& aArg1, B2& aArg2, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper2<C,A1,A2>(*aObject, aMethod, aArg1, aArg2), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 2 arguments and no return value
     */
    template<class C, class A1, class A2, class B1, class B2>
    static inline void Call(const C* aObject, void (C::*aMethod)(A1,A2) const, B1& aArg1, B2& aArg2, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)(A1,A2)>(aMethod), aArg1, aArg2, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 2 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class B1, class B2>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1,A2), B1& aArg1, B2& aArg2, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR2<S,R,C,A1,A2>(aResult, *aObject, aMethod, aArg1, aArg2), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 2 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class B1, class B2>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1,A2) const, B1& aArg1, B2& aArg2, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1,A2)>(aMethod), aArg1, aArg2, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 3 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2,A3), B1& aArg1, B2& aArg2, B3& aArg3, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper3<C,A1,A2,A3>(*aObject, aMethod, aArg1, aArg2, aArg3), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 3 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(const C* aObject, void (C::*aMethod)(A1,A2,A3) const, B1& aArg1, B2& aArg2, B3& aArg3, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)(A1,A2,A3)>(aMethod), aArg1, aArg2, aArg3, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 3 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1,A2,A3), B1& aArg1, B2& aArg2, B3& aArg3, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR3<S,R,C,A1,A2,A3>(aResult, *aObject, aMethod, aArg1, aArg2, aArg3), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 3 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class B1, class B2, class B3>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1,A2,A3) const, B1& aArg1, B2& aArg2, B3& aArg3, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1,A2,A3)>(aMethod), aArg1, aArg2, aArg3, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 4 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2,A3,A4), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper4<C,A1,A2,A3,A4>(*aObject, aMethod, aArg1, aArg2, aArg3, aArg4), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 4 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(const C* aObject, void (C::*aMethod)(A1,A2,A3,A4) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)(A1, A2, A3, A4)>(aMethod), aArg1, aArg2, aArg3, aArg4, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 4 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1,A2,A3,A4), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR4<S,R,C,A1,A2,A3,A4>(aResult, *aObject, aMethod, aArg1, aArg2, aArg3, aArg4), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 4 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class B1, class B2, class B3, class B4>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1,A2,A3,A4) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1,A2,A3,A4)>(aMethod), aArg1, aArg2, aArg3, aArg4, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 5 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2,A3,A4,A5), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper5<C,A1,A2,A3,A4,A5>(*aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5), aDisplay);
    }


    /**
     * Calls a method with 5 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1,A2,A3,A4,A5), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR5<S,R,C,A1,A2,A3,A4,A5>(aResult, *aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 5 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(const C* aObject, void (C::*aMethod)(A1,A2,A3,A4,A5) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)(A1, A2, A3, A4, A5)>(aMethod), aArg1, aArg2, aArg3, aArg4, aArg5, aDisplay);
    }
#endif // _lint


#ifndef _lint
    /**
     * Calls a const method with 5 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class B1, class B2, class B3, class B4, class B5>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1,A2,A3,A4,A5) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5)>(aMethod), aArg1, aArg2, aArg3, aArg4, aArg5, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 6 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2,A3,A4,A5,A6), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper6<C,A1,A2,A3,A4,A5,A6>(*aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5, aArg6), aDisplay);
    }


    /**
     * Calls a method with 6 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1,A2,A3,A4,A5,A6), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR6<S,R,C,A1,A2,A3,A4,A5,A6>(aResult, *aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5, aArg6), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 6 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1,A2,A3,A4,A5,A6) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6)>(aMethod), aArg1, aArg2, aArg3, aArg4, aArg5, aArg6, aDisplay);
    }
#endif // _lint


#ifndef _lint
    /**
     * Calls a const method with 6 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class B1, class B2, class B3, class B4, class B5, class B6>
    static inline void Call(const C* aObject, void (C::*aMethod)(A1,A2,A3,A4,A5,A6) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, CSwtDisplay* aDisplay = 0)
    {
        Call(const_cast<C*>(aObject), reinterpret_cast<void (C::*)(A1, A2, A3, A4, A5, A6)>(aMethod), aArg1, aArg2, aArg3, aArg4, aArg5, aArg6, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 7 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2,A3,A4,A5,A6,A7), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, B7& aArg7, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper7<C,A1,A2,A3,A4,A5,A6,A7>(*aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5, aArg6, aArg7), aDisplay);
    }


    /**
     * Calls a method with 7 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(S& aResult, C* aObject, R(C::*aMethod)(A1,A2,A3,A4,A5,A6,A7), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, B7& aArg7, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapperR7<S,R,C,A1,A2,A3,A4,A5,A6,A7>(aResult, *aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5, aArg6, aArg7), aDisplay);
    }


#ifndef _lint
    /**
     * Calls a const method with 7 arguments and a return value
     */
    template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class B1, class B2, class B3, class B4, class B5, class B6, class B7>
    static inline void Call(S& aResult, const C* aObject, R(C::*aMethod)(A1,A2,A3,A4,A5,A6,A7) const, B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, B7& aArg7, CSwtDisplay* aDisplay = 0)
    {
        Call(aResult, const_cast<C*>(aObject), reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6,A7)>(aMethod), aArg1, aArg2, aArg3, aArg4, aArg5, aArg6, aArg7, aDisplay);
    }
#endif // _lint


    /**
     * Calls a method with 8 arguments and no return value
     */
    template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7,class A8, class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8>
    static inline void Call(C* aObject, void (C::*aMethod)(A1,A2,A3,A4,A5,A6,A7,A8), B1& aArg1, B2& aArg2, B3& aArg3, B4& aArg4, B5& aArg5, B6& aArg6, B7& aArg7, B8& aArg8, CSwtDisplay* aDisplay = 0)
    {
        Execute(TMethodWrapper8<C,A1,A2,A3,A4,A5,A6,A7,A8>(*aObject, aMethod, aArg1, aArg2, aArg3, aArg4, aArg5, aArg6, aArg7, aArg8), aDisplay);
    }


};




/**
 * Specialisation of TMethodCall for non-leaving methods
 */
typedef TMethodCall<ExecuteRegularFunctor> TRegularMethodCall;


/**
 * Specialisation of TMethodCall for leaving methods
 */
typedef TMethodCall<ExecuteLeavingFunctorL> TLeavingMethodCall;


/**
 * Helper macro to ease calling a regular method
 */
#define CallMethod TRegularMethodCall::Call


/**
 * Helper macro to ease calling a leaving method
 */
#define CallMethodL TLeavingMethodCall::Call


#endif // METHODCALL_H



