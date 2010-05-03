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

#include "pushcontrollerstarter.h"

using namespace java::push;

PushControllerStarter& getPushControllerStarter();

EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    JELOG2(EJavaPush);
    FuncPtr ptr = 0;
    if (funcName)
    {
        if (strcmp(funcName, "getPushControllerStarter") == 0)
        {
            ptr = (FuncPtr)getPushControllerStarter;
        }
    }
    else
    {
        ELOG(EJavaPush, "filepushplugin findDllMethod() funcName == null");
    }
    return ptr;
}
