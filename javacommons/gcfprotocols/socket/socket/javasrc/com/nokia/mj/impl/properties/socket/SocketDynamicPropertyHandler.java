/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.properties.socket;

import java.util.Hashtable;
import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.utils.Logger;

public final class SocketDynamicPropertyHandler implements SystemPropertyProvider
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasocket");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    private static String SOCKET_LOCALHOST_NAME = "microedition.hostname";

    private static final int LOCALHOST_NAME = 1;

    private static Hashtable iPropertyKeys;

    static
    {
        iPropertyKeys = new Hashtable();
        iPropertyKeys.put(SOCKET_LOCALHOST_NAME, new Integer(
                              LOCALHOST_NAME));

    }

    /**
     * Retrieves socket localhost system property.
     *
     * @param aKey
     *            The property to retrieve as defined in this class.
     * @return The value of the property specified; null if the property is not
     *         defined
     */
    public String getProperty(String aKey)
    {
        String propertyValue = null;
        String propertyName = aKey;

        Object property = iPropertyKeys.get(propertyName);
        if (null == property)
        {
            return "localhost";
        }
        propertyValue = _getLocalhostname();

        return propertyValue;
    }

    public boolean isStatic(String aKey)
    {
        return true;
    }

    private static native String _getLocalhostname();
}
