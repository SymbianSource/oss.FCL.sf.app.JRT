/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.properties.test;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

/**
 * A class for handling dynamic system properties of File API. In order to speed
 * the search all the property names are stored into two Hashtables except for
 * fileconn.dir.private and fileconn.dir.roots.names properties.
 */
public class Property implements SystemPropertyProvider
{


    private static int mCounter = 0;

    /* (non-Javadoc)
     * @see com.nokia.mj.impl.rt.DynamicProperty#getProperty(com.nokia.mj.impl.rt.DynamicPropertyInfo)
     */
    public String getProperty(String key)
    {
        //Get the asked property name.
        System.out.println("  test.Property");
        mCounter++;
        return key +" DynamicValue ("+mCounter+")";
    }
    public boolean isStatic(String key)
    {
        return false;
    }
}
