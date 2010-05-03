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
package com.nokia.mj.impl.properties.lcduib;

import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;


/**
 * This class provides the LCDUI the dynamic property settings for:
 * "com.nokia.keyboard.type"<br>
 * "com.nokia.key.scancode"<br>
 * "com.nokia.key.modifier"
 */
public final class LcduiDynamicProperty implements SystemPropertyProvider {

    /* (non-Javadoc)
     * @see com.nokia.mj.impl.rt.support.SystemPropertyProvider#getProperty(java.lang.String)
     */
    public String getProperty(String propertyName) {
        // Set the property value.
        return LCDUIInvoker.getDynamicProperty(propertyName);
    }

    public boolean isStatic(String key)
    {
        // This property MAY change during the lifetime of the application.
        return false;
    }

}
