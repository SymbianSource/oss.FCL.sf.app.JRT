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


#ifndef FS_METHODWRAPPERS_H
#define FS_METHODWRAPPERS_H

#include "fs_functor.h"
#include "logger.h"

// Disable bothersome warning
#pragma warning(disable: 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


// Call wrapping objects for methods without return value

/**
 * Wrapper for a void method with no argument
 */
template<class C>
class MethodWrapper0 : public Functor
{
public:
    typedef void (C::*Method)();

    inline MethodWrapper0(C& object, Method method)
            : mObject(object)
            , mMethod(method)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)();
    }

private:
    C&            mObject;
    const Method  mMethod;
};


/**
 * Wrapper for a void method with 1 argument
 */
template<class C, class A1>
class MethodWrapper1 : public Functor
{
public:
    typedef void (C::*Method)(A1);

    inline MethodWrapper1(C& object, Method method, A1 arg1)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
};


/**
 * Wrapper for a void method with 2 arguments
 */
template<class C, class A1, class A2>
class MethodWrapper2 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2);

    inline MethodWrapper2(C& object, Method method, A1 arg1, A2 arg2)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
    {}

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
};


/**
 * Wrapper for a void method with 3 arguments
 */
template<class C, class A1, class A2, class A3>
class MethodWrapper3 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2,A3);

    inline MethodWrapper3(C& object, Method method, A1 arg1, A2 arg2, A3 arg3)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2,mArg3);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
};


/**
 * Wrapper for a void method with 4 arguments
 */
template<class C, class A1, class A2, class A3, class A4>
class MethodWrapper4 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2,A3,A4);

    inline MethodWrapper4(C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
};


/**
 * Wrapper for a void method with 5 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5>
class MethodWrapper5 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2,A3,A4,A5);

    inline MethodWrapper5(C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
};


/**
 * Wrapper for a void method with 6 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5, class A6>
class MethodWrapper6 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2,A3,A4,A5,A6);

    inline MethodWrapper6(C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
};


/**
 * Wrapper for a void method with 7 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class MethodWrapper7 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2,A3,A4,A5,A6,A7);

    inline MethodWrapper7(C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
};


/**
 * Wrapper for a void method with 8 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class MethodWrapper8 : public Functor
{
public:
    typedef void (C::*Method)(A1,A2,A3,A4,A5,A6,A7,A8);

    inline MethodWrapper8(C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8)
            : mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
            , mArg8(arg8)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7,mArg8);
    }

private:
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
    A8            mArg8;
};




// Call wrapping objects for methods with a return value

/**
 * Wrapper for a value-returning method with no argument
 */
template<class S, class R, class C>
class MethodWrapperR0 : public Functor
{
public:
    typedef R(C::*Method)();

    inline MethodWrapperR0(S& result, C& object, Method method)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)();
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
};


/**
 * Wrapper for a value-returning method with 1 argument
 */
template<class S, class R, class C, class A1>
class MethodWrapperR1 : public Functor
{
public:
    typedef R(C::*Method)(A1);

    inline MethodWrapperR1(S& result, C& object, Method method, A1 arg1)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
};


/**
 * Wrapper for a value-returning method with 2 arguments
 */
template<class S, class R, class C, class A1, class A2>
class MethodWrapperR2: public Functor
{
public:
    typedef R(C::*Method)(A1,A2);

    inline MethodWrapperR2(S& result, C& object, Method method, A1 arg1, A2 arg2)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1,mArg2);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
};


/**
 * Wrapper for a value-returning method with 3 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3>
class MethodWrapperR3 : public Functor
{
public:
    typedef R(C::*Method)(A1,A2,A3);

    inline MethodWrapperR3(S& result, C& object, Method method, A1 arg1, A2 arg2, A3 arg3)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1,mArg2,mArg3);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
};


/**
 * Wrapper for a value-returning method with 4 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4>
class MethodWrapperR4 : public Functor
{
public:
    typedef R(C::*Method)(A1,A2,A3,A4);

    inline MethodWrapperR4(S& result, C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
};


/**
 * Wrapper for a value-returning method with 5 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5>
class MethodWrapperR5 : public Functor
{
public:
    typedef R(C::*Method)(A1,A2,A3,A4,A5);

    inline MethodWrapperR5(S& result, C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
};


/**
 * Wrapper for a value-returning method with 6 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6>
class MethodWrapperR6 : public Functor
{
public:
    typedef R(C::*Method)(A1,A2,A3,A4,A5,A6);

    inline MethodWrapperR6(S& result, C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
};


/**
 * Wrapper for a value-returning method with 7 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class MethodWrapperR7 : public Functor
{
public:
    typedef R(C::*Method)(A1,A2,A3,A4,A5,A6,A7);

    inline MethodWrapperR7(S& result, C& object, Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7)
            : mResult(result)
            , mObject(object)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (mObject.*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7);
    }

private:
    S&            mResult;
    C&            mObject;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
};

//************************* FOR LOCAL CALLS *****************************

//********NO RETURN VALUE*****************

/**
 * Wrapper for a void method with 0 argument for local call
 */
class MethodWrapperLocal0 : public Functor
{
public:
    typedef void (*Method)();

    inline MethodWrapperLocal0(Method method) :
            mMethod(method)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)();
    }

private:
    const Method  mMethod;
};


/**
 * Wrapper for a void method with 1 argument for local call
 */
template<class A1>
class MethodWrapperLocal1 : public Functor
{
public:
    typedef void (*Method)(A1);

    inline MethodWrapperLocal1(Method method, A1 arg1)
            : mMethod(method)
            , mArg1(arg1)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1);
    }

private:
    const Method  mMethod;
    A1            mArg1;
};



/**
 * Wrapper for a void method with 2 arguments for local call
 */
template< class A1, class A2>
class MethodWrapperLocal2: public Functor
{
public:
    typedef void (*Method)(A1,A2);

    inline MethodWrapperLocal2(Method method, A1 arg1, A2 arg2)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
};


/**
 * Wrapper for a void method with 3 arguments for local call
 */
template<class A1, class A2, class A3>
class MethodWrapperLocal3 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3);

    inline MethodWrapperLocal3(Method method, A1 arg1, A2 arg2, A3 arg3)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
};

/**
 * Wrapper for a void method with 4 arguments for local call
 */
template<class A1, class A2, class A3, class A4>
class MethodWrapperLocal4 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3,A4);

    inline MethodWrapperLocal4(Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3,mArg4);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
};


/**
 * Wrapper for a void method with 5 arguments for local call
 */
template<class A1, class A2, class A3, class A4, class A5>
class MethodWrapperLocal5 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3,A4,A5);

    inline MethodWrapperLocal5(Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
};

/**
 * Wrapper for a void method with 6 arguments for local call
 */
template<class A1, class A2, class A3, class A4, class A5, class A6>
class MethodWrapperLocal6 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3,A4,A5,A6);

    inline MethodWrapperLocal6(Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
};

/**
 * Wrapper for a void method with 7 arguments for local call
 */
template<class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class MethodWrapperLocal7 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3,A4,A5,A6,A7);

    inline MethodWrapperLocal7(Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
};

/**
 * Wrapper for a void method with 8 arguments for local call
 */
template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class MethodWrapperLocal8 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3,A4,A5,A6,A7,A8);

    inline MethodWrapperLocal8(Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
            , mArg8(arg8)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7,mArg8);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
    A8            mArg8;
};



/**
 * Wrapper for a void method with 9 arguments for local call
 */
template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class MethodWrapperLocal9 : public Functor
{
public:
    typedef void (*Method)(A1,A2,A3,A4,A5,A6,A7,A8,A9);

    inline MethodWrapperLocal9(Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8, A9 arg9)
            : mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
            , mArg8(arg8)
            , mArg9(arg9)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7,mArg8,mArg9);
    }

private:
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
    A8            mArg8;
    A9            mArg9;
};


//********WITH RETURN VALUE*****************
/**
 * Wrapper for a value-returning method with 0 argument for local call
 */
template<class R>
class MethodWrapperLocalR0 : public Functor
{
public:
    typedef R(*Method)();

    inline MethodWrapperLocalR0(R& result, Method method)
            : mResult(result)
            , mMethod(method)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)();
    }

private:
    R&            mResult;
    const Method  mMethod;
};


/**
 * Wrapper for a value-returning method with 1 argument for local call
 */
template<class R, class A1>
class MethodWrapperLocalR1 : public Functor
{
public:
    typedef R(*Method)(A1);

    inline MethodWrapperLocalR1(R& result, Method method, A1 arg1)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
};



/**
 * Wrapper for a value-returning method with 2 arguments for local call
 */
template<class R,  class A1, class A2>
class MethodWrapperLocalR2: public Functor
{
public:
    typedef R(*Method)(A1,A2);

    inline MethodWrapperLocalR2(R& result, Method method, A1 arg1, A2 arg2)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
};


/**
 * Wrapper for a value-returning method with 3 arguments for local call
 */
template<class R, class A1, class A2, class A3>
class MethodWrapperLocalR3 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3);

    inline MethodWrapperLocalR3(R& result,Method method, A1 arg1, A2 arg2, A3 arg3)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
};

/**
 * Wrapper for a value-returning method with 4 arguments for local call
 */
template<class R, class A1, class A2, class A3, class A4>
class MethodWrapperLocalR4 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3,A4);

    inline MethodWrapperLocalR4(R& result,Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3,mArg4);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
};


/**
 * Wrapper for a value-returning method with 5 arguments for local call
 */
template<class R, class A1, class A2, class A3, class A4, class A5>
class MethodWrapperLocalR5 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3,A4,A5);

    inline MethodWrapperLocalR5(R& result,Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
};

/**
 * Wrapper for a value-returning method with 6 arguments for local call
 */
template<class R, class A1, class A2, class A3, class A4, class A5, class A6>
class MethodWrapperLocalR6 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3,A4,A5,A6);

    inline MethodWrapperLocalR6(R& result,Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
};

/**
 * Wrapper for a value-returning method with 7 arguments for local call
 */
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class MethodWrapperLocalR7 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3,A4,A5,A6,A7);

    inline MethodWrapperLocalR7(R& result,Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
};

/**
 * Wrapper for a value-returning method with 8 arguments for local call
 */
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class MethodWrapperLocalR8 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3,A4,A5,A6,A7,A8);

    inline MethodWrapperLocalR8(R& result,Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
            , mArg8(arg8)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7,mArg8);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
    A8            mArg8;
};



/**
 * Wrapper for a value-returning method with 9 arguments for local call
 */
template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class MethodWrapperLocalR9 : public Functor
{
public:
    typedef R(*Method)(A1,A2,A3,A4,A5,A6,A7,A8,A9);

    inline MethodWrapperLocalR9(R& result,Method method, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8, A9 arg9)
            : mResult(result)
            , mMethod(method)
            , mArg1(arg1)
            , mArg2(arg2)
            , mArg3(arg3)
            , mArg4(arg4)
            , mArg5(arg5)
            , mArg6(arg6)
            , mArg7(arg7)
            , mArg8(arg8)
            , mArg9(arg9)
    {
        JELOG2(EUtils);
    }

    void operator()() const
    {
        JELOG2(EUtils);
        mResult = (*mMethod)(mArg1,mArg2,mArg3,mArg4,mArg5,mArg6,mArg7,mArg8,mArg9);
    }

private:
    R&            mResult;
    const Method  mMethod;
    A1            mArg1;
    A2            mArg2;
    A3            mArg3;
    A4            mArg4;
    A5            mArg5;
    A6            mArg6;
    A7            mArg7;
    A8            mArg8;
    A9            mArg9;
};


#endif // FS_METHODWRAPPERS_H

