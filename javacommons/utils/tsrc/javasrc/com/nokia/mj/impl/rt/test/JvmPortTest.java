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

package com.nokia.mj.impl.rt.test;


import com.nokia.mj.impl.rt.JvmPort;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.rt.support.NativeMemoryBlock;

public class JvmPortTest extends JvmPort
{

    public static final int LOADSYSTEMLIBRARY_CALL = 0;
    public static final int LOADAPPLICATIONCLASS_CALL = 1;
    public static final int RUNYOUNGGENERATIONGC_CALL = 2;
    public static final int SETTHREADASDAEMON_CALL = 3;
    public static final int GETRESOURCEASNATIVEMEMORY_CALL = 4;
    public static final int DISABLERUNTIMEEXIT_CALL = 5;
    public static final int ENABLERUNTIMEEXIT_CALL = 6;
    public static final int EXITVM_CALL = 7;
    public static final int RUNFINALIZATION_CALL = 8;
    public static final int ENABLEFINALIZATION_CALL = 9;
    public static final int SETTHREADEVENTLISTENER_CALL = 10;
    public static final int ADDRESTRICTEDPACKAGEPREFIXES_CALL = 11;
    public static final int ADDPROTECTEDPACKAGEPREFIXES_CALL = 12;
    public static final int APPENDTOCLASSPATH_CALL = 13;
    public static final int SHRINKJAVAHEAPTOMINIMUM_CALL = 14;
    public static final int EXPANDJAVAHEAP_CALL = 15;


    public static int[] mCalledArr = new int[16];
    public static int[] mCalledTotalArr = new int[16];

    static
    {
        clear();
        for (int i = 0; i < mCalledTotalArr.length; ++i)
        {
            mCalledTotalArr[i] = 0;
        }
    }

    public static Object mObj1 = null;
    public static Object mObj2 = null;
    public static Object mReturnObj = null;
    public static Throwable mThrowable = null;

    public static void clear()
    {
        for (int i = 0; i < mCalledArr.length; ++i)
        {
            mCalledArr[i] = 0;
        }
        mObj1 = null;
        mObj2 = null;
        mReturnObj = null;
        mThrowable = null;
    }

    public static void resetInstance()
    {
        sInstance = null;
    }

    public static boolean allCalled()
    {
        for (int i = 0; i < mCalledTotalArr.length; ++i)
        {
            if (mCalledTotalArr[i] == 0)
            {
                StringBuffer sb = new StringBuffer();
                for (int j = 0; j < mCalledTotalArr.length; ++j)
                {
                    sb.append("\n");
                    sb.append("  " + (j + 1) + " == " + mCalledTotalArr[j]);
                }
                System.err.println("All calls:"+ sb.toString());

                return false;
            }
        }
        return true;
    }

    public static boolean checkCalls(int checkInd)
    {
        for (int i = 0; i < mCalledArr.length; ++i)
        {
            if (checkInd == i)
            {
                if (mCalledArr[i] != 1)
                {
                    return false;
                }
            }
            else if (mCalledArr[i] != 0)
            {
                return false;
            }
        }
        return true;
    }

    public static String print()
    {
        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < mCalledArr.length; ++i)
        {
            sb.append("\n");
            sb.append("  " + (i + 1) + " == " + mCalledArr[i]);
        }
        return sb.toString();
    }

    public static void setSystemPropertyImpl(Object key, Object value)
    {
        mPropertiesContainer.setSystemProperty(key, value);
    }


    public void loadSystemLibrary(String libName)
    {
        mCalledArr[LOADSYSTEMLIBRARY_CALL]++;
        mCalledTotalArr[LOADSYSTEMLIBRARY_CALL]++;
        mObj1 = libName;
    }

    public Class loadApplicationClass(String name) throws ClassNotFoundException
    {
        mCalledArr[LOADAPPLICATIONCLASS_CALL]++;
        mCalledTotalArr[LOADAPPLICATIONCLASS_CALL]++;
        if (mThrowable != null)
        {
            throw(ClassNotFoundException)mThrowable;
        }
        mObj1 = name;
        sInstance = null;
        return (Class)mReturnObj;
    }

    public boolean runYoungGenerationGc()
    {
        mCalledArr[RUNYOUNGGENERATIONGC_CALL]++;
        mCalledTotalArr[RUNYOUNGGENERATIONGC_CALL]++;
        return ((Boolean)mReturnObj).booleanValue();
    }

    public void setThreadAsDaemon(Thread thread,
                                  boolean daemon)
    throws IllegalThreadStateException, SecurityException
    {
        mCalledArr[SETTHREADASDAEMON_CALL]++;
        mCalledTotalArr[SETTHREADASDAEMON_CALL]++;
        if (mThrowable != null)
        {
            if (mThrowable instanceof IllegalThreadStateException)
            {
                throw(IllegalThreadStateException)mThrowable;
            }
            else
            {
                throw(SecurityException)mThrowable;

            }
        }
        mObj1 = thread;
        mObj2 = new Boolean(daemon);
    }


    public NativeMemoryBlock getResourceAsNativeMemory(String jarPath,
            String resName)
    {
        mCalledArr[GETRESOURCEASNATIVEMEMORY_CALL]++;
        mCalledTotalArr[GETRESOURCEASNATIVEMEMORY_CALL]++;
        mObj1 = jarPath;
        mObj2 = resName;
        return (NativeMemoryBlock)mReturnObj;
    }


    public void disableRuntimeExit()
    {
        mCalledArr[DISABLERUNTIMEEXIT_CALL]++;
        mCalledTotalArr[DISABLERUNTIMEEXIT_CALL]++;
    }

    public void enableRuntimeExit()
    {
        mCalledArr[ENABLERUNTIMEEXIT_CALL]++;
        mCalledTotalArr[ENABLERUNTIMEEXIT_CALL]++;
    }

    public void exitVm(int status)
    {
        mCalledArr[EXITVM_CALL]++;
        mCalledTotalArr[EXITVM_CALL]++;
        mObj1 = new Integer(status);
    }

    public void runFinalization()
    {
        mCalledArr[RUNFINALIZATION_CALL]++;
        mCalledTotalArr[RUNFINALIZATION_CALL]++;
    }

    public void enableFinalization(Class clazz)
    {
        mCalledArr[ENABLEFINALIZATION_CALL]++;
        mCalledTotalArr[ENABLEFINALIZATION_CALL]++;
        mObj1 = clazz;
    }

    public void setThreadEventListener(ThreadEventListener listener)
    {
        mCalledArr[SETTHREADEVENTLISTENER_CALL]++;
        mCalledTotalArr[SETTHREADEVENTLISTENER_CALL]++;
        mObj1 = listener;
    }

    public void addRestrictedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        mCalledArr[ADDRESTRICTEDPACKAGEPREFIXES_CALL]++;
        mCalledTotalArr[ADDRESTRICTEDPACKAGEPREFIXES_CALL]++;
        if (mThrowable != null)
        {
            throw(SecurityException)mThrowable;
        }
        mObj1 = packageNames;
    }

    public void addProtectedPackagePrefixes(String[] packageNames)
    throws SecurityException
    {
        mCalledArr[ADDPROTECTEDPACKAGEPREFIXES_CALL]++;
        mCalledTotalArr[ADDPROTECTEDPACKAGEPREFIXES_CALL]++;
        if (mThrowable != null)
        {
            throw(SecurityException)mThrowable;
        }
        mObj1 = packageNames;
    }

    public void appendToClassPath(String path)
    {
        mCalledArr[APPENDTOCLASSPATH_CALL]++;
        mCalledTotalArr[APPENDTOCLASSPATH_CALL]++;
        mObj1 = path;
    }

    public int shrinkJavaHeapToMinimum()
    {
        mCalledArr[SHRINKJAVAHEAPTOMINIMUM_CALL]++;
        mCalledTotalArr[SHRINKJAVAHEAPTOMINIMUM_CALL]++;
        return ((Integer)mReturnObj).intValue();
    }

    public int expandJavaHeap(int amount)
    {
        mCalledArr[EXPANDJAVAHEAP_CALL]++;
        mCalledTotalArr[EXPANDJAVAHEAP_CALL]++;
        mObj1 = new Integer(amount);
        return ((Integer)mReturnObj).intValue();
    }

}
