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

import java.util.Hashtable;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.utils.Logger;

/**
 * A class that will be used to store system properties in CLDC VM.
 * The class is inherited from Hashtable and there will be only one instance
 * of it per VM instance. The VM will replace the original Hashtable with
 * instace of this object which gives us the control when MIDlet is asking
 * for any system property. Original system properties are in the private
 * member and the parent hash table is used to store dynamic handlers.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

class SystemPropertyUtils
{

    private static final String
    PROPERTY_HANDLER_CLASS_PREFIX = "com.nokia.mj.impl.properties.";

    /**
     * Solves the possible dynamic property value. This a utility method
     * to be called by VM dependent sytem properties extension mechanism.
     *
     * The utility will check if the value of the system property indicates
     * that the method is dynamic. if it is not, then the value is returned
     * directly, but if it is, then the value of the dynamic property will
     * be solved by using defined property handler.
     *
     * @param propertyName the name of the system property.
     * @param propertyValue the value of the system property. This must be
     *                      passed as an argument even if the original
     *                      Hashtable is also passed as third argument.
     *                      That can't be used because calling the get method
     *                      of the given Hashtable would lead to forever loop.
     * @param props A hashtable containing the system properties. This is used
     *              to store the "freezed" system property value.
     * @return value of the system property.
     */
    public static Object solvePropertyValue(Object propertyName,
                                            Object propertyValue,
                                            Hashtable systemProperties)
    {
        // Check that the property name and the obtained value are strings
        if (!(propertyName instanceof String) ||
                !(propertyValue instanceof String))
        {
            // Not strings, return the non-string property value.
            return propertyValue;
        }

        String propertyNameStr = (String)propertyName;
        String propertyValueStr = (String)propertyValue;

        try
        {
            // If the property value starts with ':' it is a dynamic property
            if (propertyValueStr != null && propertyValueStr.length() > 0 &&
                    propertyValueStr.charAt(0) == ':')
            {
                // Note for CDC. If some application wants to define some system
                // property value that starts with ':' then it is needed to
                // encode e.g. by using leading ':'.
                if (propertyValueStr.length() > 1 &&
                        propertyValueStr.charAt(1) == ':')
                {
                    return propertyValueStr.substring(1);
                }

                // Strip the dynamic property prefix away.
                final String classEnding = propertyValueStr.substring(1);
                // The result will be the end of the package and of
                // the class name.
                String className = PROPERTY_HANDLER_CLASS_PREFIX +
                                   classEnding;

                try
                {
                    Class clazz = Class.forName(className);
                    Object providerImpl = clazz.newInstance();

                    boolean isFrozen = false;
                    SystemPropertyProvider systemPropertyProvider =
                        (SystemPropertyProvider)providerImpl;

                    propertyValueStr =
                        systemPropertyProvider.getProperty(propertyNameStr);

                    isFrozen =
                        systemPropertyProvider.isStatic(propertyNameStr);

                    // Freeze the value if the provider wants so
                    if (propertyValueStr != null && isFrozen)
                    {
                        systemProperties.put(propertyNameStr,
                                             propertyValueStr);
                    }
                    return propertyValueStr;
                }
                catch (ClassNotFoundException e)
                {
                    Logger.ELOG(Logger.EUtils, "Dynamic property handler " +
                                className + " was not found for "
                                + propertyName);
                    return "";
                }

            }

            // It wasn't dynamic, so returning the static value.
            return propertyValueStr;
        }
        catch (Throwable t)
        {
            Logger.ELOG(Logger.EUtils, "Error in system properties: Key="
                        + propertyNameStr + ", Val ='" +
                        propertyValueStr, t);
            return "";
        }
    }
}

