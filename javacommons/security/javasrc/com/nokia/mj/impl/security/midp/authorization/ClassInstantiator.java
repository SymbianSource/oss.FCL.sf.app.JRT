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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.rt.support.Jvm;

/**
 * Utility class to provide full reflection in CLDC. This class is needed
 * when instantiating Permission objects via different constructors with
 * one or two constructor parameters
 */
public final class ClassInstantiator
{
    static
    {
        Jvm.loadSystemLibrary("javasecurity");
    }

    public static Object newInstance(
        String className,
        String constructorParam1,
        String constructorParam2)
    throws InstantiationException
    {
        // need to translate the fully qualified className into a format with
        // "/" instead of "." as package separator
        className = className.replace('.', '/');
        // build up the arguments list
        int argsCount = ((constructorParam1 != null
                          && constructorParam1.length() > 0) ? 1 : 0)
                        + ((constructorParam2 != null
                            && constructorParam2.length() > 0) ? 1 : 0);
        String[] args = new String[argsCount];
        switch (argsCount)
        {
        case 2:
            args[1] = constructorParam2;
        case 1:
            args[0] = constructorParam1;
        }
        return _newInstance(className, args);
    }

    private static native Object _newInstance(String className, String[] args)
    throws InstantiationException;
}
