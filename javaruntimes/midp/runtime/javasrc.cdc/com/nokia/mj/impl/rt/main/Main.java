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

package com.nokia.mj.impl.rt.main;

import java.lang.reflect.Method;
import java.io.InputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

import com.nokia.mj.impl.rt.midp.Log;
import com.nokia.mj.impl.rt.midp.MainArgs;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.ManifestReader;
import com.nokia.mj.impl.utils.JarManifestReader;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.legacy.LegacySupport;

/**
 * A main entry class of the main type runtime.
 */
public final class Main
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * A main entry point of the main type runtime.
     */
    public static void main(String[] args)
    {
        if (Log.mOn) Log.logI("Starting main runtime");

        boolean ok = true;

        try
        {
            Jvm.loadSystemLibrary("javamidpruntime");
            MainArgs mainArgs = new MainArgs(args);
            Class clazz = null;

            Hashtable manifestArgs = null;
            String mainClass = mainArgs.findArgument("-main");
            if (mainClass != null)
            {
                clazz = Jvm.loadApplicationClass(mainClass);
                manifestArgs = getManifestAttributes(clazz);
            }
            else
            {
                manifestArgs = getManifestAttributes(mainArgs.findArgument("-jar"));
                if (manifestArgs != null)
                {
                    mainClass = (String)manifestArgs.get("Main-Class");
                }
                if (mainClass == null)
                {
                    throw new RuntimeException("Error locating main class!");
                }
                clazz = Jvm.loadApplicationClass(mainClass);
            }
            ApplicationInfoImpl.setManifestArgs(manifestArgs);
            ApplicationInfoImpl.setUid(Uid.createUid(mainArgs.findArgument("-uid")));
            ApplicationInfoImpl.setRootPath(mainArgs.findArgument("-rootpath"));
            LegacySupport.init(manifestArgs, false);

            Method mainMethod = clazz.getMethod("main", new Class[] {String[].class});
            mainMethod.invoke(null, new Object[] {mainArgs.getAppArgs()});
            _closeInd(Integer.parseInt(mainArgs.findArgument("-handle")));
            ApplicationUtilsImpl.doShutdownImpl();
        }

        catch (Throwable th)
        {
            Log.logE("Error in MAIN runtime: ", th);
            System.exit(-1);
        }
        LegacySupport.close();
        if (Log.mOn) Log.logI("Closing MAIN runtime, status " +
                                  (ok ? "OK":"NOK"));
    }

    private static Hashtable getManifestAttributes(Class clazz)
    {
        Hashtable attributes = null;
        InputStream is = null;
        try
        {
            is = clazz.getResourceAsStream("/META-INF/MANIFEST.MF");
            if (is != null)
            {
                attributes = ManifestReader.getAttributes(is);
            }
        }
        catch (IOException ioe)
        {
            Log.logE("Error setting main attributes (clz)", ioe);
        }
        finally
        {
            if (is != null)
            {
                try
                {
                    is.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
        return attributes;
    }

    private static Hashtable getManifestAttributes(String jarFile)
    {
        Hashtable attributes = null;
        try
        {
            Hashtable tmp = JarManifestReader.getAttributes(jarFile);
            if (tmp != null && tmp.size() > 0)
            {
                attributes = new Hashtable(tmp.size());
                Enumeration e  = tmp.elements();
                while (e.hasMoreElements())
                {
                    Attribute attr = (Attribute)e.nextElement();
                    attributes.put(attr.getName(), attr.getValue());
                }
            }

        }
        catch (IOException ioe)
        {
            Log.logE("Error setting main attributes (jar)", ioe);
        }
        return attributes;
    }

    private static native void _closeInd(int starterHandle);
}

