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


package com.nokia.mj.impl.rt.sun;

import java.util.Properties;
import java.util.Hashtable;

/**
 * For Sun J2SE vm to override getProperty method which doesn't
 * work as expected.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public class SystemPropertyProperties extends
        com.nokia.mj.impl.rt.SystemPropertyProperties
{
    public SystemPropertyProperties(Hashtable originalProperties)
    {
        super(originalProperties);
    }

    public String getProperty(String name)
    {
        Object value = get(name);
        String property = value instanceof String ? (String)value : null;
        if (property == null && defaults != null)
        {
            property = defaults.getProperty(name);
        }
        return property;
    }
}
