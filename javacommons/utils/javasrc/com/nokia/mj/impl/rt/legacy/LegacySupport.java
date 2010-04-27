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

package com.nokia.mj.impl.rt.legacy;

import java.util.Hashtable;

/**
 * A class for the runtime for accessing the legacy support. If the
 * actual port class implementation is not defined via specified system
 * property, all the provided public methods won't do anything.
 */
public abstract class LegacySupport
{

    static final String PORT_CLASS_PROPERTY_NAME = "com.nokia.legacy.support";

    static final String PACKAGE_NAME = "com.nokia.mj.impl.rt.legacy.";


    protected static LegacySupport sInstance = null;

    /**
     * Creates a singleton instance if defined by system property.
     */
    static
    {
        //Get the system proptery defining the port class.
        String portClassName =
            System.getProperty(PORT_CLASS_PROPERTY_NAME);

        if (portClassName != null)
        {
            //Construct the class
            String className = PACKAGE_NAME + portClassName;
            try
            {
                Class clazz = Class.forName(className);
                sInstance = (LegacySupport)clazz.newInstance();
            }
            catch (Exception e)
            {
                throw new RuntimeException("Not able to instantiate class " +
                                           className+". Reason is: " + e);
            }
        }
    }

    /**
     * Inits the Legacy support impl if exists.
     * @param args All the application argumets got from manifest of the
     *             Jar file and Jad file if exists.
     * @param startInBackGround Is the system requesting the start to
     *                          background.
     */
    public static void init(Hashtable args, boolean startInBackGround)
    {
        if (sInstance != null)
        {
            sInstance.initImpl(args, startInBackGround);
        }
    }

    /**
     * Sets the running MIDlet.
     */
    public static void setMidlet(Object midlet)
    {
        if (sInstance != null)
        {
            sInstance.setMidletImpl(midlet);
        }
    }

    /**
     * Closes the Legacy support impl if exists.
     */
    public static void close()
    {
        if (sInstance != null)
        {
            sInstance.closeImpl();
        }
    }

    /**
     * @see com.nokia.mj.impl.rt.legacy.LegacySupport#init(java.util.Hashtable, boolean)
     */
    protected abstract void initImpl(Hashtable args, boolean startInBackGround);

    /**
     * @see com.nokia.mj.impl.rt.legacy.LegacySupport#setMidlet(java.lang.Object)
     */
    protected abstract void setMidletImpl(Object midlet);

    /**
     * @see com.nokia.mj.impl.rt.legacy.LegacySupport#close(java.lang.Object)
     */
    protected abstract void closeImpl();

}
