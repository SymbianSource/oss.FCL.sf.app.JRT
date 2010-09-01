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


#ifndef METHODWRAPPERS_H
#define METHODWRAPPERS_H


#include "swtfunctor.h"


// Disable bothersome warning
#pragma warning(disable: 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


// Call wrapping objects for methods without return value

/**
 * Wrapper for a void method with no argument
 */
template<class C>
class TMethodWrapper0 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)();

    inline TMethodWrapper0(C& aObject, TMethod aMethod)
            : iObject(aObject)
            , iMethod(aMethod)
    {}

    void operator()() const
    {
        (iObject.*iMethod)();
    }

private:
    C&            iObject;
    const TMethod iMethod;
};


/**
 * Wrapper for a void method with 1 argument
 */
template<class C, class A1>
class TMethodWrapper1 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1);

    inline TMethodWrapper1(C& aObject, TMethod aMethod, A1 aArg1)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
};


/**
 * Wrapper for a void method with 2 arguments
 */
template<class C, class A1, class A2>
class TMethodWrapper2 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2);

    inline TMethodWrapper2(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
};


/**
 * Wrapper for a void method with 3 arguments
 */
template<class C, class A1, class A2, class A3>
class TMethodWrapper3 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2,A3);

    inline TMethodWrapper3(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2,iArg3);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
};


/**
 * Wrapper for a void method with 4 arguments
 */
template<class C, class A1, class A2, class A3, class A4>
class TMethodWrapper4 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2,A3,A4);

    inline TMethodWrapper4(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
};


/**
 * Wrapper for a void method with 5 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5>
class TMethodWrapper5 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2,A3,A4,A5);

    inline TMethodWrapper5(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
};


/**
 * Wrapper for a void method with 6 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5, class A6>
class TMethodWrapper6 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2,A3,A4,A5,A6);

    inline TMethodWrapper6(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5, A6 aArg6)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
            , iArg6(aArg6)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5,iArg6);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
    A6            iArg6;
};


/**
 * Wrapper for a void method with 7 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class TMethodWrapper7 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2,A3,A4,A5,A6,A7);

    inline TMethodWrapper7(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5, A6 aArg6, A7 aArg7)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
            , iArg6(aArg6)
            , iArg7(aArg7)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5,iArg6,iArg7);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
    A6            iArg6;
    A7            iArg7;
};


/**
 * Wrapper for a void method with 8 arguments
 */
template<class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class TMethodWrapper8 : public MSwtFunctor
{
public:
    typedef void (C::*TMethod)(A1,A2,A3,A4,A5,A6,A7,A8);

    inline TMethodWrapper8(C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5, A6 aArg6, A7 aArg7, A8 aArg8)
            : iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
            , iArg6(aArg6)
            , iArg7(aArg7)
            , iArg8(aArg8)
    {}

    void operator()() const
    {
        (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5,iArg6,iArg7,iArg8);
    }

private:
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
    A6            iArg6;
    A7            iArg7;
    A8            iArg8;
};




// Call wrapping objects for methods with a return value

/**
 * Wrapper for a value-returning method with no argument
 */
template<class S, class R, class C>
class TMethodWrapperR0 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)();

    inline TMethodWrapperR0(S& aResult, C& aObject, TMethod aMethod)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)();
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
};


/**
 * Wrapper for a value-returning method with 1 argument
 */
template<class S, class R, class C, class A1>
class TMethodWrapperR1 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1);

    inline TMethodWrapperR1(S& aResult, C& aObject, TMethod aMethod, A1 aArg1)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
};


/**
 * Wrapper for a value-returning method with 2 arguments
 */
template<class S, class R, class C, class A1, class A2>
class TMethodWrapperR2: public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1,A2);

    inline TMethodWrapperR2(S& aResult, C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1,iArg2);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
};


/**
 * Wrapper for a value-returning method with 3 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3>
class TMethodWrapperR3 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1,A2,A3);

    inline TMethodWrapperR3(S& aResult, C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1,iArg2,iArg3);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
};


/**
 * Wrapper for a value-returning method with 4 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4>
class TMethodWrapperR4 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1,A2,A3,A4);

    inline TMethodWrapperR4(S& aResult, C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
};


/**
 * Wrapper for a value-returning method with 5 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5>
class TMethodWrapperR5 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1,A2,A3,A4,A5);

    inline TMethodWrapperR5(S& aResult, C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
};


/**
 * Wrapper for a value-returning method with 6 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6>
class TMethodWrapperR6 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1,A2,A3,A4,A5,A6);

    inline TMethodWrapperR6(S& aResult, C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5, A6 aArg6)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
            , iArg6(aArg6)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5,iArg6);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
    A6            iArg6;
};


/**
 * Wrapper for a value-returning method with 7 arguments
 */
template<class S, class R, class C, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class TMethodWrapperR7 : public MSwtFunctor
{
public:
    typedef R(C::*TMethod)(A1,A2,A3,A4,A5,A6,A7);

    inline TMethodWrapperR7(S& aResult, C& aObject, TMethod aMethod, A1 aArg1, A2 aArg2, A3 aArg3, A4 aArg4, A5 aArg5, A6 aArg6, A7 aArg7)
            : iResult(aResult)
            , iObject(aObject)
            , iMethod(aMethod)
            , iArg1(aArg1)
            , iArg2(aArg2)
            , iArg3(aArg3)
            , iArg4(aArg4)
            , iArg5(aArg5)
            , iArg6(aArg6)
            , iArg7(aArg7)
    {}

    void operator()() const
    {
        iResult = (iObject.*iMethod)(iArg1,iArg2,iArg3,iArg4,iArg5,iArg6,iArg7);
    }

private:
    S&            iResult;
    C&            iObject;
    const TMethod iMethod;
    A1            iArg1;
    A2            iArg2;
    A3            iArg3;
    A4            iArg4;
    A5            iArg5;
    A6            iArg6;
    A7            iArg7;
};


#endif // METHODWRAPPERS_H
