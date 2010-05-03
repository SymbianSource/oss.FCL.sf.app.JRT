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
 * Provides JVM utilities <b>only</b> for Java runtime implementations to use.
 * Some of these
 * utilities hide differences between CLDC and CDC configurations and some
 * provide completely non-standard access to JVM features and to process memory.
 * <p>
 * Example how to enable System.exit() funtionality.
 * <pre>
 * package com.nokia.mj.impl.mypackage;
 *
 * import com.nokia.mj.impl.rt.support;
 *      public void myClass()
 *      {
 *          public void myMethod()
 *          {
 *              //loads the system library.
 *              JvmInternal.enableRuntimeExit();
 *
 *              //my implementation...
 *          }
 *      }
 * </pre>
 * @author Nokia Corporation
 * @version 1.0
 */

public class JvmInternal
{

    /**
     * Enables ability to use methods <code>System.exit</code> and
     * <code>Runtime.exit</code>.
     * MIDP requires that if MIDlet tries to call <code>System.exit</code>
     * or <code>Runtime.exit</code>. If this is prevented by calling method
     * {@link JvmInternal#disableRuntimeExit} either runtime implementation is not allowed
     * to call the <code>exit</code> methods. Calling
     * <code>enableRuntimeExit</code> removes the prevention.
     */
    public static void enableRuntimeExit()
    {
        JvmPort.getInstance().enableRuntimeExit();
    }

    /**
     * Disables ability to use methods <code>System.exit</code> and
     * <code>Runtime.exit</code>.
     * MIDP requires that if MIDlet tries to call <code>System.exit</code>
     * or <code>Runtime.exit</code>. If this is prevented it can be released
     * using {@link JvmInternal#enableRuntimeExit} method.
     */
    public static void disableRuntimeExit()
    {
        JvmPort.getInstance().disableRuntimeExit();
    }

    /**
     * Terminates forcefully JVM process. This method is used to
     * forcefully terminate the process running the JVM. It should
     * be used if the graceful shutdown cant be done.
     * <p>
     * It should be noted that after calling this method the process
     * terminates. So if there are any necessary action related to the
     * shut down, those should be done before calling this method.
     *
     * @param status The exit status.
     */
    public static void exitVm(int status)
    {
        JvmPort.getInstance().exitVm(status);
    }

    /**
     * Enables finalization for given class.
     * @param clazz The class to be finalized.
     */
    public static void enableFinalization(Class clazz)
    {
        JvmPort.getInstance().enableFinalization(clazz);
    }

    /**
     * Runs the finalization methods of any objects pending finalization.
     * This method uses <code>System.runFinalization</code> in CDC based
     * configurations while in CLDC a proprieatary API is used.
     */
    public static void runFinalization()
    {
        JvmPort.getInstance().runFinalization();
    }

    /**
     * Cleans the young generation heap. The use JVM might not support
     * cleaning of the young generation heap.
     * @return True if running youngGenerationGc is supported, otherwise
     * false
     */
    public static boolean runYoungGenerationGc()
    {
        return JvmPort.getInstance().runYoungGenerationGc();
    }

    /**
     * Registers a ThreadEventListener to receive notifications about Java
     * thread events. There can be at most one registered listener at time
     * - new registration replaces the old one and null can be used to clear
     * the registration completely.
     * <p>
     * The registered listener will be notified on each subsequent Thread
     * creation, Thread death, and uncaught exceptions.
     *
     * @param listener the Listener or null to clear the active listener
     */
    public static void setThreadEventListener(ThreadEventListener listener)
    {
        JvmPort.getInstance().setThreadEventListener(listener);
    }

    /**
     * JAVADOCS ARE UNDER CONSTRUCTION!!
     * <p>
     * Sets package names to the list of restricted packages. Normally the
     * package names should end with '.' to avoid matching word prefixes.
     * For example, using "com.nokia" as a restricted package name restricts
     * "com.nokiapkg.*", while restricting "com.nokia." does not.
     *
     * @param packageNames an array of package names to restrict
     * @throws SecurityException if one of the restricted package names
     * is a prefix of the name of a class being loaded
     */
    public static void addRestrictedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        JvmPort.getInstance().addRestrictedPackagePrefixes(packageNames);
    }


    /**
     * JAVADOCS ARE UNDER CONSTRUCTION!!
     * <p>
     * Sets package names to the list of protected packages. Normally the
     * package names should end with '.' to avoid matching word prefixes.
     * For example, using "com.nokia" as a protected package name protectets
     * "com.nokiapkg.*", while protecting "com.nokia." does not.
     *
     * @param packageNames an array of package names to restrict
     * @throws SecurityException if one of the restricted package names
     * is a prefix of the name of a class being loaded
     */
    public static void addProtectedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        JvmPort.getInstance().addProtectedPackagePrefixes(packageNames);
    }

    /**
     * Appends the given path to class path.
     */
    public static void appendToClassPath(String path)
    {
        JvmPort.getInstance().appendToClassPath(path);
    }

    /**
     * Sets a system property.
     * @param key a key.
     * @param value a value.
     */
    public static void setSystemProperty(Object key, Object value)
    {
        JvmPort.getInstance().setSystemProperty(key, value);
    }

    /**
     * Shrinks the Java heap to a minimum. This call will lead to call
     * of the gc.
     *
     * @return Number of bytes shrinked.
     */
    public static int shrinkJavaHeapToMinimum()
    {
        return JvmPort.getInstance().shrinkJavaHeapToMinimum();
    }

    /**
     * Expands the size of the Java heap.
     *
     * @param amount Amount of bytes to be expanded.
     * @return Number of bytes expanded.
     */
    public static int expandJavaHeap(int amount)
    {
        return JvmPort.getInstance().expandJavaHeap(amount);
    }
}
