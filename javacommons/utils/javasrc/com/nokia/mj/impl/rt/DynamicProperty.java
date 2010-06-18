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


package com.nokia.mj.impl.rt;

/**
 * Interface for accessing configuration dependent system property extension
 * mechanism.
 */

interface DynamicProperty
{
    /**
     * Adds the defined system property containing all the system
     * properties. The value is set as is without any escaping.
     * The semanitc is equivalent to Hashtable.put() method.
     * @param key a key.
     * @param value a value.
     * @return the value before adding, null if this was the first add.
     * @throws NullPointerException if either key or value is null.
     */
    public Object setSystemProperty(Object key, Object value);

    /**
     * Adds the defined user property containing all the user
     * properties. This will not override any system properies.
     * The semanitc is equivalent to Hashtable.put() method.
     * @param key a key.
     * @param value a value.
     * @return the value before adding, null if this was the first add.
     * @throws NullPointerException if either key or value is null.
     */
    public Object setUserProperty(Object key, Object value);
}