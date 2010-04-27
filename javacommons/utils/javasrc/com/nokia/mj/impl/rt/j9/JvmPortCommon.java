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
import com.nokia.mj.impl.rt.JvmPort;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.rt.support.NativeMemoryBlock;

/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

abstract class JvmPortCommon extends JvmPort
{

    ThreadEventListener                     mClientListener = null;
    com.ibm.oti.util.ThreadEventListener    mVmListener = null;
    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#runYoungGenerationGc.
     */
    public final boolean runYoungGenerationGc()
    {
        VM.localGC();
        return true;
    }


    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#getResourceAsNativeMemory.
     */
    public final NativeMemoryBlock getResourceAsNativeMemory(String jarPath, String resName)
    {
        return new NativeMemoryBlockImpl(jarPath, resName);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#disableRuntimeExit.
     */
    public final void disableRuntimeExit()
    {
        VM.stopExit();
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#enableRuntimeExit.
     */
    public final void enableRuntimeExit()
    {
        VM.allowMidpExit();
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
    public void setThreadEventListener(ThreadEventListener listener)
    {
        mClientListener = listener;
        if (mVmListener == null)
        {
            mVmListener = new com.ibm.oti.util.ThreadEventListener()
            {
                public void threadStarting(Thread newThread, Thread parentThread)
                {
                    mClientListener.threadStarting(newThread, parentThread);
                }

                public void threadDeath(Thread thread)
                {
                    mClientListener.threadDied(thread);
                }

                public void uncaughtException(Thread thread, Throwable throwable)
                {
                    mClientListener.uncaughtException(thread, throwable);
                }
            };
            com.ibm.oti.util.ThreadEventHook.setListener(mVmListener);
        }
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#addRestrictedPackagePrefixes.
     */
    public final void addRestrictedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        VM.addRestrictedPackages(packageNames);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#addProtectedPackagePrefixes.
     */
    public final void addProtectedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        VM.addProtectedPackages(packageNames);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#appendClasspath.
     */
    public final void appendToClassPath(String path)
    {
        VM.appendToClassPath(path);
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#shrinkJavaHeapToMinimum.
     */
    public final int shrinkJavaHeapToMinimum()
    {
        return VM.contractOldSpaceFully();
    }

    /**
     * @see com.nokia.mj.impl.runtimesupport.JvmPort#expandJavaHeap.
     */
    public final int expandJavaHeap(int amount)
    {
        return VM.expandOldSpace(amount);
    }

}
