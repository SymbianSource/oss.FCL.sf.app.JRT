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
* Description:
*
*/


#include <string> //For strcmp
#include "javasymbianoslayer.h"
#include "logger.h"
#include "serverconnectionfactory.h"

using namespace java::push;

ServerConnectionFactory& getServerConnectionFactory();

EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    LOG(EJavaPush, EInfo, "btl2capserverconnection findDllMethod() funcName");
    JELOG2(EJavaPush);
    FuncPtr ptr = 0;
    if (funcName)
    {
        if (strcmp(funcName, "getServerConnectionFactory") == 0)
        {
            ptr = (FuncPtr) getServerConnectionFactory;
        }
    }
    else
    {
        ELOG(EJavaPush,
             "btl2capserverconnection findDllMethod() funcName == null");
    }
    return ptr;
}
