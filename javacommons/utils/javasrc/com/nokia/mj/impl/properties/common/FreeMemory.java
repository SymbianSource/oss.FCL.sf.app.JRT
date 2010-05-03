/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.properties.common;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.rt.support.Jvm;

public class FreeMemory implements SystemPropertyProvider
{
    static
    {
        Jvm.loadSystemLibrary("javautils");
    }

    public String getProperty(String key)
    {
        int freeMemory = _getFreeMemory();
        if (freeMemory == -1)
        {
            return null;
        }
        return Integer.toString(freeMemory);
    }

    public boolean isStatic(String key)
    {
        return false;
    }

    private native int _getFreeMemory();
}
