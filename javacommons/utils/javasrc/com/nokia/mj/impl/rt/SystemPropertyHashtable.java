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
import java.util.Hashtable;

/**
 * A class that will be used to store system properties in CLDC VM.
 * The class is inherited from java.util.Hashtable and the VM will replace
 * the original Hashtable object with instace of this object which gives
 * us the control when application is asking for any system property.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public class SystemPropertyHashtable extends Hashtable
{
    public SystemPropertyHashtable(Hashtable originalProperties)
    {
        // Store the original properties
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
        return SystemPropertyUtils.solvePropertyValue(name,
                super.get(name),
                this);
    }
}
