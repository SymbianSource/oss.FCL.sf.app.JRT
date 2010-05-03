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

package com.nokia.mj.impl.rt.j9;

import com.nokia.mj.impl.rt.JvmPort;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.rt.support.NativeMemoryBlock;
import java.io.*;
import java.util.Properties;
import java.util.Enumeration;
import com.nokia.mj.impl.rt.DynamicSystemPropertiesSun;

/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public final class JvmPortTest extends JvmPort
{
    static
    {
        //The code below reads all the odc files from the location
        //where the API jsr files are. It also enables the System
        //property extension mechanism for Sun's J2SE vm.
        try
        {
            String odcLocation = System.getProperty("java.ext.dirs");
            // This filter only returns directories
            File odcDir = new File(odcLocation);
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

            File[] files = odcDir.listFiles(filter);
            DynamicSystemPropertiesSun props = new DynamicSystemPropertiesSun();

            Properties origProps = System.getProperties();
            Enumeration keys = origProps.keys();
            while (keys.hasMoreElements())
            {
                Object key = keys.nextElement();
                props.put(key, origProps.get(key));
            }

            for (int i=0; i<files.length; i++)
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
                            props.put(name, value);
                        }
                    }
                }
                finally
                {
                    input.close();
                }
            }
            System.setProperties(props);
        }
        catch (Throwable t)
        {
            System.err.println("JvmPortJ2se: Exception: "+t);
        }
    }
    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#loadSystemLibrary.
     */
    public final void loadSystemLibrary(String libName)
    {
        System.out.println("TestVm: loadSystemLibrary");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#loadApplicationClass.
     */
    public final Class loadApplicationClass(String name) throws ClassNotFoundException
    {
        System.out.println("TestVm: loadApplicationClass: "+name);
        return null;
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setThreadAsDaemon.
     */
    public final void setThreadAsDaemon(Thread thread, boolean daemon)
    throws IllegalThreadStateException, SecurityException
    {
        System.out.println("TestVm: loadApplicationClass");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#runFinalization.
     */
    public final void runFinalization()
    {
        System.out.println("TestVm: runFinalization");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#enableFinalization.
     */
    public final void enableFinalization(Class clazz)
    {
        System.out.println("TestVm: enableFinalization");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#runYoungGenerationGc.
     */
    public final boolean runYoungGenerationGc()
    {
        System.out.println("TestVm: runYoungGenerationGc");
        return false;
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#getResourceAsNativeMemory.
     */
    public final NativeMemoryBlock getResourceAsNativeMemory(String jarPath, String resName)
    {
        System.out.println("TestVm: getResourceAsNativeMemory");
        return null;
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#disableRuntimeExit.
     */
    public final void disableRuntimeExit()
    {
        System.out.println("TestVm: disableRuntimeExit");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#enableRuntimeExit.
     */
    public final void enableRuntimeExit()
    {
        System.out.println("TestVm: enableRuntimeExit");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#exitVm.
     */
    public final void exitVm(int status)
    {
        System.out.println("TestVm: exitVm "+status);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setThreadEventListener.
     */
    public final void setThreadEventListener(ThreadEventListener listener)
    {
        System.out.println("TestVm: setThreadEventListener");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#addRestrictedPackagePrefixes.
     */
    public final void addRestrictedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        System.out.println("TestVm: addRestrictedPackagePrefixes");
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#addProtectedPackagePrefixes.
     */
    public final void addProtectedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        System.out.println("TestVm: addProtectedPackagePrefixes");
    }
}
