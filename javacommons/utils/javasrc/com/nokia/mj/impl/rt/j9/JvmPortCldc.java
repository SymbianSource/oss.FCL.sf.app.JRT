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

import com.ibm.oti.vm.VM;
/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public final class JvmPortCldc extends JvmPortCommon
{
    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#loadSystemLibrary.
     */
    public final void loadSystemLibrary(String libName)
    {
        try
        {
            VM.loadLibrary(libName);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            throw new RuntimeException("Not able to load library "+libName+". Reason is: "+e);
        }
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#loadApplicationClass.
     */
    public final Class loadApplicationClass(String name) throws ClassNotFoundException
    {
        return Class.forName(name);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#setThreadAsDaemon.
     */
    public final void setThreadAsDaemon(Thread thread, boolean daemon)
    throws IllegalThreadStateException, SecurityException
    {
        if (daemon)
        {
            VM.setDaemonThread(thread);
        }
        else
        {
            throw new RuntimeException("Setting thread as non-daemon not supported");
        }
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#runFinalization.
     */
    public final void runFinalization()
    {
        VM.runFinalization();
    }

    /**
     * Enables finalization for given class.
     * @param clazz The class to be finalized.
     */
    public final void enableFinalization(Class clazz)
    {
        VM.enableFinalization(clazz);
    }

}
