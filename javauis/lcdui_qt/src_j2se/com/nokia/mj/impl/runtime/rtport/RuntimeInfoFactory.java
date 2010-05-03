/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.runtime.rtport;

/**
 * Factory to get the RuntimeInfo implementation. 
 *
 * The RuntimeInfo implementation class is searched and dynamically loaded
 * based on the value of "nokia.rt.port" system property. 
 *
 * The class named "com.nokia.mj.impl.runtime.rtport.<value>.RuntimeInfoImpl" 
 * is tried to be loaded first, and if that is not found the value is tried to
 * be used as a full class name.
 *
 * If the system property does not exist, then the default value "midp" is used.
 */
public class RuntimeInfoFactory
{
    /** RuntimeInfo instance */
    private static RuntimeInfo sInstance = new RuntimeInfoImpl();

    /**
     * Returns the RuntimeInfo
     *
     * @return RuntimeInfo
     */
    public static RuntimeInfo getRuntimeInfo()
    {
        return sInstance;
    }
}
