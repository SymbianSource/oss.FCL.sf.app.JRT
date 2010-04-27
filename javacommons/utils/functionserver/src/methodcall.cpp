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


#include "fs_methodcall.h"
#include "functionserver.h"
#include "logger.h"


OS_EXPORT void MethodCaller::ExecuteRegularFunctor(const Functor& functor, java::util::FunctionServer* functionServer, int* /*res*/)
{
    JELOG2(EUtils);
    functionServer->executeInServerThread(functor);
}

/**
 * Function executor for leaving functors
 *
 */
OS_EXPORT void MethodCaller::ExecuteLeavingFunctorL(const Functor& functor, java::util::FunctionServer* functionServer, int* /*res*/)
{
    JELOG2(EUtils);
    int error = functionServer->executeInServerThread(functor);
    if (error) // Do not use LeaveIfError(), we want to handle positive error codes as well
    {
        User::Leave(error);
    }
}
/**
 * Function executor for leaving functors with trap
 *
 */
OS_EXPORT void MethodCaller::ExecuteTrapingFunctorL(const Functor& functor, java::util::FunctionServer* functionServer, int* res)
{
    JELOG2(EUtils);
    *res = functionServer->executeInServerThread(functor);
}
