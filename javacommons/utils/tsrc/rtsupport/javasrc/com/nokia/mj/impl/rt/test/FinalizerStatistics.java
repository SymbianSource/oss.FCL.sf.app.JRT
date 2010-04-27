/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

import java.util.Hashtable;
import java.util.Enumeration;

public class FinalizerStatistics
{
    private static Hashtable references;

    static
    {
        references = new Hashtable();
    }


    /** Class to keep count of finalizable objects */
    static private class Refcount
    {
        /** Current number of alive objects of the class */
        int count;

        /** Name of the class */
        String clazz;
        private Refcount(String cls)
        {
            count = 1;
            clazz = cls;
        }
    }

    public static void debugFinalizeCreated(String clazz)
    {
        System.out.println("Finalizable created: " + clazz);
        // Add reference to hash
        synchronized (references)
        {
            Refcount r = (Refcount)references.get(clazz);
            if (r != null)
            {
                r.count++;
            }
            else
            {
                references.put(clazz, new Refcount(clazz));
            }
        }
    }

    public static void debugFinalize(String clazz)
    {
        System.out.println("Finalizable destroyed " + clazz);
        // Reduce reference
        synchronized (references)
        {
            Refcount r = (Refcount)references.get(clazz);
            r.count--;
        }
    }

    public static void printStatus()
    {
        synchronized (references)
        {
            StringBuffer sb = new StringBuffer();
            sb.append("Finalizable objects\n");
            Enumeration e = references.elements();
            boolean empty = true;
            while (e.hasMoreElements())
            {
                Refcount r = (Refcount)e.nextElement();

                if (r != null)
                {
                    if (r.count != 0)
                    {
                        empty = false;
                    }
                    sb.append("  class=" + r.clazz +
                              " count="  + r.count + "\n");
                }
            }
            sb.append("\nStatus ");
            if (empty)
            {
                sb.append("ALL FINALIZED");
            }
            else
            {
                sb.append("OBJECTS WAITING FOR FINALIZATION");
            }
            references = null;
            references = new Hashtable();
            System.out.println(sb.toString());
        }
    }
}
