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


package com.nokia.mj.impl.rt;

import java.util.Enumeration;
import java.util.Properties;
import java.util.Hashtable;

/**
 * A class that will be used to store system properties in non CLDC VM.
 * The class is inherited from java.util.Properties and the VM will replace
 * the original Properties object with instace of this object which gives
 * us the control when application is asking for any system property.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public class SystemPropertyProperties extends Properties
                                      implements DynamicProperty
{
    /** Storage place for user defined properties. */
    private Properties mUserProperties = new Properties(); 

    public SystemPropertyProperties(Hashtable originalProperties)
    {
        // Store the original properties
        //Be careful if you would like to make this as a utility
        //function. This class overrides put method which might
        //lead to problems...
        Enumeration keys = originalProperties.keys();
        while (keys.hasMoreElements())
        {
            Object key = keys.nextElement();
            Object value = originalProperties.get(key);
            super.put(key, value);
        }
    }

    /**
     * Gets the system property indicated by the specified key. This overrides
     * get() method from java.util.Hashtable.
     * @param name the name of the system property.
     * @return string value of the system property, or null if there is no
     *         property with that key.
     */
    public Object get(Object name)
    {
        Object val = super.get(name);
        if (val != null)
        {
            return SystemPropertyUtils.solvePropertyValue(name, val, this);
        }
        return mUserProperties.get(name);
    }


    /**
     * Overrides put() method from java.util.Hashtable in order to
     * handle values that starts with ':'
     * @see java.util.Hashtable.put()
     */
    public Object put(Object key, Object value)
    {
        if ((key instanceof String) &&
                (value instanceof String) &&
                (value != null))
        {
            String valueStr = (String)value;

            // Encode a leading semicolon by doubling it
            if (valueStr.length() > 0 &&
                    valueStr.charAt(0) == ':')
            {
                return super.put(key, ":"+valueStr);
            }
        }
        return super.put(key, value);
    }

    /**
     * Adds the defined system property to Hashtable containing all the system
     * properties without escaping any leading colon.
     */
    public Object setSystemProperty(Object key, Object value)
    {
        return super.put(key, value);
    }

    /**
     * Adds the defined system property to Hashtable containing all the user
     */
    public Object setUserProperty(Object key, Object value)
    {
        return mUserProperties.put(key, value);
    }
}
