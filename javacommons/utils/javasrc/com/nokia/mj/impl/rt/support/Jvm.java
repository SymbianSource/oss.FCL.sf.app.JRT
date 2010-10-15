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

package com.nokia.mj.impl.rt.support;

import com.nokia.mj.impl.rt.JvmPort;

/**
 * Provides JVM utilities for Java API implementations to use. Some of these
 * utilities hide differences between CLDC and CDC configurations and some
 * provide completely non-standard access to JVM features and to process memory.
 *<p>
 * Example how to load a system library
 * <pre>
 * package com.nokia.mj.impl.mypackage;
 *
 * import com.nokia.mj.impl.rt.support;
 *      public void myClass()
 *      {
 *          public void myMethod()
 *          {
 *              //loads the system library.
 *              Jvm.loadSystemLibrary("my_system_library");
 *
 *              //my implementation...
 *          }
 *      }
 *</pre>
 * @author Nokia Corporation
 * @version 1.0
 */

public class Jvm
{
    /**
     * Loads the system library specified by the libName argument.
     * <p>
     * This method uses <code>System.loadLibrary</code> in CDC based
     * configuration while in CLDC a proprieatary API is used.
     * @param libName The name of the library.
     */
    public static void loadSystemLibrary(String libName)
    {
        JvmPort.getInstance().loadSystemLibrary(libName);
    }

    /**
     * JAVADOCS ARE UNDER CONSTRUCTION!!
     * <p>
     * Returns the Class object associated with the class or interface with the
     * given string name.
     * @param name A fully qualified name of the desired class.
     * @return class object representing the desired class .
     */
    public static Class loadApplicationClass(String name)
    throws ClassNotFoundException
    {
        return JvmPort.getInstance().loadApplicationClass(name);
    }

    /**
     * Marks the given thread as either a daemon thread or a user thread
     * This can only be done before the Thread starts running.
     * @param thread The thread to be modified.
     * @param daemon If true, marks the given thread as a daemon thread.
     * @throws IllegalThreadStateException if the Thread has already been
     * started.
     * @throws SecurityException if the caller is not in the boot class path.
     */
    public static void setThreadAsDaemon(Thread thread, boolean daemon)
    throws IllegalThreadStateException, SecurityException
    {
        JvmPort.getInstance().setThreadAsDaemon(thread, daemon);
    }

    /**
     * JAVADOCS ARE UNDER CONSTRUCTION!!
     * <p>
     * Gets a NativeMemoryBlock. NativeMemoryBlock provids a way
     * read the whole content of resource in jar file into native memory
     * buffer. The content is not copien into Java side - only pointer
     * and size of the data is usable in the Java side.
     *
     * @param jarPath If jarPath is null then the resource is searched from
     * the classpath - like in normal class.getResourceAsStream. If it is
     * provided then it can be any Jar file availble in the file system.
     * @param resName Name of the desired resource
     *
     * @return NativeMemoryBlock instance if resource is found, otherwise null.
     */
    public static NativeMemoryBlock getResourceAsNativeMemory(String jarPath,
            String resName)
    {
        return JvmPort.getInstance().getResourceAsNativeMemory(jarPath, resName);
    }
}
