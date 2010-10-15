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
package com.nokia.mj.impl.rt.support;

import java.util.Hashtable;
import java.util.Enumeration;

class FinalizerStatistics
{
    private static Hashtable mReferences = new Hashtable();

    private static final boolean mTrace = false;
    private static class Refcount
    {
        int    mCount;
        String mClassName;

        private Refcount(String className)
        {
            mCount = 1;
            mClassName = className;
        }
    }

    public static void finalizableCreated(String className)
    {
        synchronized (mReferences)
        {
            Refcount r = (Refcount)mReferences.get(className);
            int count = 1;
            if (r != null)
            {
                r.mCount++;
                count = r.mCount;
            }
            else
            {
                mReferences.put(className, new Refcount(className));
            }
            if (mTrace) System.out.println("Finalizable created. name: " +
                                               className + ", count: " + count);
        }
    }

    public static void classFinalized(String className)
    {
        synchronized (mReferences)
        {
            Refcount r = (Refcount)mReferences.get(className);
            r.mCount--;
            if (mTrace) System.out.println("Finalizable destroyed. name: " +
                                               className + ", count: " + r.mCount);
        }
    }

    public static boolean getNonfinalizedObjects(StringBuffer sb)
    {
        synchronized (mReferences)
        {
            sb.append("Finalizable objects\n");
            Enumeration e = mReferences.elements();
            boolean allFinalized = true;
            while (e.hasMoreElements())
            {
                Refcount r = (Refcount)e.nextElement();

                if (r != null)
                {
                    if (r.mCount != 0)
                    {
                        allFinalized = false;
                    }
                    sb.append("  class=" + r.mClassName +
                              " count="  + r.mCount + "\n");
                }
            }
            sb.append("\nStatus ");
            if (allFinalized)
            {
                sb.append("ALL FINALIZED");
            }
            else
            {
                sb.append("OBJECTS WAITING FOR FINALIZATION");
            }
            if (mTrace) System.out.println(sb.toString());

            return allFinalized;
        }
    }

    public static void reset()
    {
        mReferences.clear();
    }

}
