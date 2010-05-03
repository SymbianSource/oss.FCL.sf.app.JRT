/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.properties.http;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;

/**
 * DynamicPropertyHandler class, used to implement the system property for http connection
 *
 * @version 1.1
 * @author Nokia Corporation
 */

public class DynamicPropertyHandler implements SystemPropertyProvider
{

    static
    {
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"loading http from http DynamicPropertyHandler.java");
            Jvm.loadSystemLibrary("javahttp");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"after loading http dll");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }
    /**
     * Get the system property corresponding to the key specified.
     * There could be various system properties available for one interface.
     * Currently this function returns the proxy server address. So the paramter key is
     * not used at present.
     *
     * @param the system property name as a string
     * @return system property correspoding to the key
     */

    public String getProperty(String Key)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "DynamicPropertyHandler in http");
        StringBuffer proxy = new StringBuffer();
        String ret = _getProxyInfo();
        if (ret != null && ret.length() != 0)
        {
            int index = ret.indexOf(':');
            proxy.append(ret.substring(0, index + 5));
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "Proxy string returned in DynamicPropertyHandler.java : "
                       + proxy);
            return proxy.toString();
        }
        return null;
    }

    public boolean isStatic(String Key)
    {
        return false;
    }

    private native String _getProxyInfo();
}