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


package com.ibm.oti.nokiaextcdc;

import java.util.Hashtable;

import com.ibm.oti.util.SystemPropertiesHook;

import com.nokia.mj.impl.rt.SystemPropertyProperties;
import com.nokia.mj.impl.rt.JvmPort;
import com.nokia.mj.impl.utils.Logger;

/**
 * A class that implements a system property extension mechanism provided
 * by J9 for CDC. The idea is that the VM provides the original system
 * properties in a Hashtable object as an argument in extendSystemProperties
 * method. In the method a new Hashtable based instance of object is returned
 * containing original properties. The object is from class that extendeds
 * the Hashtable where the get() method is overwritten in order to handle
 * dynamic properties.
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class SystemPropertyExtension implements SystemPropertiesHook
{
    /* (non-Javadoc)
     * @see com.ibm.oti.util.SystemProperties#extendSystemProperties(java.util.Properties)
     */
    public Hashtable extendSystemProperties(Hashtable originalProperties)
    {
        // Create a new object extending jva.util.Properties and fill with the
        // original properties.
        SystemPropertyProperties newProperties = null;

        try
        {
            newProperties = new SystemPropertyProperties(originalProperties);
            // Store the hashtable to be able to add properties during runtime.
            JvmPort.setPropertiesContainer(newProperties);
    
            // Override existing line.separator
            newProperties.put("line.separator", "\n");
        }
        catch (Throwable t)
        {
            Logger.LOG(Logger.EUtils, Logger.EInfo, 
               "Failed to init system properties", t);
            // In case of error, return original properties.
            return originalProperties;
        }

        return newProperties;
    }
}
