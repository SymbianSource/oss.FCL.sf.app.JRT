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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileFilter;
import java.io.FileReader;
import java.util.Properties;
import java.util.Enumeration;
import java.util.StringTokenizer;

import com.nokia.mj.impl.rt.JvmPort;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.rt.support.NativeMemoryBlock;
import com.nokia.mj.impl.utils.Logger;

/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public final class JvmPortJ2se extends JvmPort
{

    static
    {
        // The code below reads all the odc files from the location
        // where the API jsr files are. It also enables the System
        // property extension mechanism for Sun's J2SE vm.
        try
        {
            SystemPropertyProperties newProperties =
                new SystemPropertyProperties(System.getProperties());

            // Store the hashtable to be able to add properties during runtime.
            JvmPort.setPropertiesContainer(newProperties);


            String odcDirs = System.getProperty("java.ext.dirs");

            // This filter only returns directories
            FileFilter filter = new FileFilter()
            {
                public boolean accept(File f)
                {
                    if (f.getName().endsWith(".odc"))
                    {
                        return true;
                    }
                    return false;
                }
            };

            StringTokenizer st = new StringTokenizer(odcDirs, ":");
            while (st.hasMoreTokens())
            {
                String odcDirStr = st.nextToken();
                File odcDir = new File(odcDirStr);

                File[] files = odcDir.listFiles(filter);

                int fileCount = (files == null ? 0 : files.length);
                for (int i=0; i<fileCount; i++)
                {
                    BufferedReader input =
                        new BufferedReader(new FileReader(files[i]));
                    try
                    {
                        String line = null;
                        while ((line = input.readLine()) != null)
                        {
                            if (line.startsWith("-D"))
                            {
                                String name = null;
                                String value = null;
                                int pos = line.indexOf("=");
                                if (pos == -1)
                                {
                                    name = line.substring(2);
                                    value = "";
                                }
                                else
                                {
                                    name = line.substring(2,pos);
                                    value = line.substring(pos+1);
                                }
                                newProperties.setSystemProperty(name, value);
                            }
                        }
                    }
                    finally
                    {
                        input.close();
                    }
                }
            }
            System.setProperties(newProperties);
        }
        catch (Throwable t)
        {
            Logger.ELOG(Logger.EUtils,
                        "JvmPortJ2se: Exception in static init",
                        t);
        }
    }
    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#loadSystemLibrary.
     */
    public final void loadSystemLibrary(String libName)
    {
        System.loadLibrary(libName);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#loadApplicationClass.
     */
    public final Class loadApplicationClass(String name) throws ClassNotFoundException
    {
        return Class.forName(name,true,ClassLoader.getSystemClassLoader());
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setThreadAsDaemon.
     */
    public final void setThreadAsDaemon(Thread thread, boolean daemon)
    throws IllegalThreadStateException, SecurityException
    {
        thread.setDaemon(daemon);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#runFinalization.
     */
    public final void runFinalization()
    {
        System.runFinalization();
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#enableFinalization.
     */
    public final void enableFinalization(Class clazz)
    {
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#runYoungGenerationGc.
     */
    public final boolean runYoungGenerationGc()
    {
        return false;
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#getResourceAsNativeMemory.
     */
    public final NativeMemoryBlock getResourceAsNativeMemory(String jarPath, String resName)
    {
        return null;
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#disableRuntimeExit.
     */
    public final void disableRuntimeExit()
    {
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#enableRuntimeExit.
     */
    public final void enableRuntimeExit()
    {
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#exitVm.
     */
    public final void exitVm(int status)
    {
        System.exit(status);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setThreadEventListener.
     */
    public final void setThreadEventListener(ThreadEventListener listener)
    {
        //throw new RuntimeException("setThreadEventListener() not supported");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#addRestrictedPackagePrefixes.
     */
    public final void addRestrictedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        //throw new RuntimeException("addRestrictedPackagePrefixes() not supported");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#addProtectedPackagePrefixes.
     */
    public final void addProtectedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        //throw new RuntimeException("addProtectedPackagePrefixes() not supported");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#appendClasspath.
     */
    public final void appendToClassPath(String path)
    {
        throw new RuntimeException("appendClasspath() not supported");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#shrinkJavaHeapToMinimum.
     */
    public final int shrinkJavaHeapToMinimum()
    {
        throw new RuntimeException("shrinkJavaHeapToMinimum() not supported");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#expandJavaHeap.
     */
    public final int expandJavaHeap(int amount)
    {
        throw new RuntimeException("expandJavaHeap() not supported");
    }

}

