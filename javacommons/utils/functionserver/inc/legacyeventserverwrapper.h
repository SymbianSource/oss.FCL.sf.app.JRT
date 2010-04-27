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


#ifndef LEGACYEVENTSERVERWRAPPER_H
#define LEGACYEVENTSERVERWRAPPER_H

#include "functionserver.h"
#include "fs_methodcall.h"
#include "logger.h"

#pragma warning(disable: 4786) // Truncated browser info for long mangled names

class LegacyEventServerWrapper : public CBase
{
public:
    virtual java::util::FunctionServer* getFunctionServer() const = 0;


//*****Execute******
    template <class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    inline R Execute(R(*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, getFunctionServer());
    }

    template <class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline R Execute(R(*func)(T1,T2,T3,T4,T5,T6,T7,T8), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, getFunctionServer());
    }

    template <class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline R Execute(R(*func)(T1,T2,T3,T4,T5,T6,T7), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, getFunctionServer());
    }

    template <class R, class T1, class T2, class T3, class T4, class T5, class T6>
    inline R Execute(R(*func)(T1,T2,T3,T4,T5,T6), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, getFunctionServer());
    }

    template <class R, class T1, class T2, class T3, class T4, class T5>
    inline R Execute(R(*func)(T1,T2,T3,T4,T5), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, arg4, arg5, getFunctionServer());
    }

    template <class R, class T1, class T2, class T3, class T4>
    inline R Execute(R(*func)(T1,T2,T3,T4), T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, arg4, getFunctionServer());
    }

    template <class R, class T1, class T2, class T3>
    inline R Execute(R(*func)(T1,T2,T3), T1 arg1, T2 arg2, T3 arg3)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, arg3, getFunctionServer());
    }

    template <class R, class T1, class T2>
    inline R Execute(R(*func)(T1,T2), T1 arg1, T2 arg2)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, arg2, getFunctionServer());
    }

    template <class R, class T1>
    inline R Execute(R(*func)(T1), T1 arg1)
    {
        JELOG2(EUtils);
        return CallMethod(func, arg1, getFunctionServer());
    }

    template <class R>
    inline R Execute(R(*func)())
    {
        JELOG2(EUtils);
        return CallMethod(func, getFunctionServer());
    }


//*****ExecuteV******

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    inline void ExecuteV(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline void ExecuteV(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline void ExecuteV(void (*func)(T1,T2,T3,T4,T5,T6,T7), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    inline void ExecuteV(void (*func)(T1,T2,T3,T4,T5,T6), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, arg4, arg5, arg6, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5>
    inline void ExecuteV(void (*func)(T1,T2,T3,T4,T5), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, arg4, arg5, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4>
    inline void ExecuteV(void (*func)(T1,T2,T3,T4), T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, arg4, getFunctionServer());
    }

    template <class T1, class T2, class T3>
    inline void ExecuteV(void (*func)(T1,T2,T3), T1 arg1, T2 arg2, T3 arg3)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, arg3, getFunctionServer());
    }

    template <class T1, class T2>
    inline void ExecuteV(void (*func)(T1,T2), T1 arg1, T2 arg2)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, arg2, getFunctionServer());
    }

    template <class T1>
    inline void ExecuteV(void (*func)(T1), T1 arg1)
    {
        JELOG2(EUtils);
        CallMethod(func, arg1, getFunctionServer());
    }

    inline void ExecuteV(void (*func)())
    {
        JELOG2(EUtils);
        CallMethod(func, getFunctionServer());
    }
    /*
    //*****ExecuteTrap******

        template <class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
        inline void ExecuteTrap(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
        {
            JELOG2(EUtils);
            return CallMethodTrap(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, getFunctionServer());
        }
    */

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    inline int ExecuteTrap(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline int ExecuteTrap(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline int ExecuteTrap(void (*func)(T1,T2,T3,T4,T5,T6,T7), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    inline int ExecuteTrap(void (*func)(T1,T2,T3,T4,T5,T6), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, arg4, arg5, arg6, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4, class T5>
    inline int ExecuteTrap(void (*func)(T1,T2,T3,T4,T5), T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, arg4, arg5, getFunctionServer());
    }

    template <class T1, class T2, class T3, class T4>
    inline int ExecuteTrap(void (*func)(T1,T2,T3,T4), T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, arg4, getFunctionServer());
    }

    template <class T1, class T2, class T3>
    inline int ExecuteTrap(void (*func)(T1,T2,T3), T1 arg1, T2 arg2, T3 arg3)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, arg3, getFunctionServer());
    }

    template <class T1, class T2>
    inline int ExecuteTrap(void (*func)(T1,T2), T1 arg1, T2 arg2)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, arg2, getFunctionServer());
    }

    template <class T1>
    inline int ExecuteTrap(void (*func)(T1), T1 arg1)
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, arg1, getFunctionServer());
    }

    inline int ExecuteTrap(void (*func)())
    {
        JELOG2(EUtils);
        return CallMethodTrap(func, getFunctionServer());
    }

    /*
        template <class T1,class T2>
        inline int ExecuteTrap(void (*func)(T1,T2),T1 arg1,T2 arg2)
        {
            JELOG2(EUtils);
            return CallMethodTrap(func, arg1, arg2, getFunctionServer());
        }
        inline int ExecuteTrap(void (*func)())
        {
            JELOG2(EUtils);
            return CallMethodTrap(func, getFunctionServer());
        }
    */
};
#pragma warning(default: 4786)

#endif // LEGACYEVENTSERVERWRAPPER_H

