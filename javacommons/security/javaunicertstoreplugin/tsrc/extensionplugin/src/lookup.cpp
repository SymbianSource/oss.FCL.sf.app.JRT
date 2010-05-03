/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "extensionplugininterface.h"

using namespace java::captain;

ExtensionPluginInterface* getExtensionPlugin();

EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    JELOG2(EJavaSecurity);
    FuncPtr ptr = 0;
    if (funcName)
    {
        if (strcmp(funcName, "getExtensionPlugin") == 0)
        {
            ptr = (FuncPtr)getExtensionPlugin;
        }
    }
    else
    {
        ELOG(EJavaComms, "certstoretest extension plugin findDllMethod() funcName == null");
    }
    return ptr;
}
