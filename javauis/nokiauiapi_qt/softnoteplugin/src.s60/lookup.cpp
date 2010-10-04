/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <string> //For strcmp
#include <javasymbianoslayer.h>
#include <logger.h>

#include "javasoftnotehandler.h"

using namespace java::captain;

ExtensionPluginInterface* getExtensionPlugin();

EXPORT_C FuncPtr findDllMethod(const char* funcName)
    {
    JELOG2(EJavaCaptain);
    FuncPtr ptr = 0;
    if (funcName)
        {
        if (strcmp(funcName, "getExtensionPlugin") == 0)
            {
            ptr = (FuncPtr) getExtensionPlugin;
            }
        }
    else
        {
        ELOG(EJavaCaptain, "javasoftnoteplugin findDllMethod() funcName == null");
        }
    return ptr;
    }
