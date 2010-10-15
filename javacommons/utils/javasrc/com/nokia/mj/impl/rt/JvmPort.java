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
import com.nokia.mj.impl.rt.support.NativeMemoryBlock;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.utils.Logger;

/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public abstract class JvmPort
{

    /**
     * Property name of VM port
     */
    private static final String PORTCLASS_PROPERTY_NAME = "com.nokia.jvm.port";

    private static final String PORTCLASS_PACKAGE = "com.nokia.mj.impl.rt.";

    /**
     * When the getInstace() is first called it will be tried to create the
     * VM porting class defined in the system property PORTCLASS_PROPERTY_NAME.
     * The package visiblity is set to protected for unit testing purposes.
     */
    protected static JvmPort sInstance = null;

    /**
     * A reference to object containing properties. In CLDC it is Hashtable,
     * otherwise it is Properties.
     * The package visiblity is set to protected for unit testing purposes.
     */
    protected static DynamicProperty mPropertiesContainer;

    protected JvmPort()
    {
    }

    /**
     * Static initializer
     */
    private static synchronized void loadImpl()
    {
		if (sInstance != null)
		{
			return;
		}
        String vmPortClass = System.getProperty(PORTCLASS_PROPERTY_NAME);
        if (vmPortClass == null)
        {
            throw new RuntimeException("Not able to load VM port class since "
                                       + PORTCLASS_PROPERTY_NAME +
                                       " is undefined");
        }
        vmPortClass = PORTCLASS_PACKAGE + vmPortClass;
        try
        {
            Class clazz = Class.forName(vmPortClass);
            sInstance = (JvmPort)clazz.newInstance();
        }
        catch (Exception e)
        {
            String errTxt = "Not able to instantiate class " + vmPortClass;
            Logger.ELOG(Logger.EUtils, errTxt, e);
            throw new RuntimeException(errTxt);
        }
    }

    /**
     * Returns an instcance of class implementing interface JvmPort.
     *
     * @return JvmPort
     */
    public static JvmPort getInstance()
    {
        if (sInstance == null)
        {
            loadImpl();
        }
        return sInstance;
    }

    /**
     * Sets the container containing system properties.
     * @param propertiesContainer system properties.
     */
    public static void setPropertiesContainer(DynamicProperty propertiesContainer)
    {
        mPropertiesContainer = propertiesContainer;
    }


    /**
     * @see com.nokia.mj.impl.rt.support.Jvm#loadSystemLibrary(java.lang.String).
     */
    public abstract void loadSystemLibrary(String libName);

    /**
     * @see com.nokia.mj.impl.rt.support.Jvm#
     *      loadApplicationClass(java.lang.String).
     */
    public abstract Class loadApplicationClass(String name)
    throws ClassNotFoundException;

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#runYoungGenerationGc().
     */
    public abstract boolean runYoungGenerationGc();

    /**
     * @see com.nokia.mj.impl.rt.support.Jvm#
     * setThreadAsDaemon((java.lang.Thread, boolean)).
     */
    public abstract void setThreadAsDaemon(Thread Thread,
                                           boolean daemon)
    throws IllegalThreadStateException,
        SecurityException;


    /**
     * @see com.nokia.mj.impl.rt.support.Jvm#
     *      getResourceAsNativeMemory(java.lang.String, java.lang.String).
     */
    public abstract
    NativeMemoryBlock getResourceAsNativeMemory(String jarPath,
            String resName);


    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#disableRuntimeExit().
     */
    public abstract void disableRuntimeExit();

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#enableRuntimeExit().
     */
    public abstract void enableRuntimeExit();

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#exitVm(int).
     */
    public abstract void exitVm(int status);

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#
     * runFinalization().
     */
    public abstract void runFinalization();

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#
     * enableFinalization(java.lang.Class).
     */
    public abstract void enableFinalization(Class clazz);

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#setThreadEventListener
     * (com.nokia.mj.impl.rt.support.ThreadEventListener).
     */
    public abstract void setThreadEventListener(ThreadEventListener listener);

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#
     * addRestrictedPackagePrefixes(java.lang.String[]).
     */
    public abstract void addRestrictedPackagePrefixes(String[] packageNames)
    throws SecurityException;

    /**
     * @see com.nokia.mj.impl.rt.support.JvmInternal#
     * addRestrictedPackagePrefixes(java.lang.String[]).
     */
    public abstract void addProtectedPackagePrefixes(String[] packageNames)
    throws SecurityException;


    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#appendClasspath.
     */
    public abstract void appendToClassPath(String path);

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setSystemProperty.
     */
    public void setSystemProperty(Object key, Object value)
    {
        mPropertiesContainer.setSystemProperty(key, value);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setUserProperty.
     */
    public void setUserProperty(Object key, Object value)
    {
        mPropertiesContainer.setUserProperty(key, value);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#shrinkJavaHeapToMinimum.
     */
    public abstract int shrinkJavaHeapToMinimum();

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#expandJavaHeap.
     */
    public abstract int expandJavaHeap(int amount);

}
