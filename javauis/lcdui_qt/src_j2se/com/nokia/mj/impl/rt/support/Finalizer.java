/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
import java.util.Timer;
import java.util.TimerTask;

/**
 * Some public APIs are defined so that they do not define any explicit method
 * for informing when the object has become oblsolete - 
 * javax.microedition.lcdui.Image is good example. Image contains native 
 * resources and the class doesn't contain any close method. Native resources 
 * can be very large and those must be freed when the Java object is garbage 
 * collected. This leads to need to use finalization feature which is not part
 * of CLDC.
 * <p>
 * As the finalization itself is a problem in CLDC, there must be a VM vendor 
 * specific proprietary solution available. When using this class the 
 * finalization can be used in any configuration using same implementation. 
 * This class hides possible differences between different configurations.
 * <p>
 * There are some issues that should be taken into account when taking 
 * finalization into use:
 * <ul>
 * <li>Finalizers should be used in public APIs to ensure that resources are 
 * freed. This is the only option when there is no close() functionality on 
 * public API. Even if public API has close() funtionality, finalization 
 * should be used to ensure that resources are freed when MIDlet developer 
 * forgets to use close().</li>
 * <li>In our internal/implementation code finalization should not be used, 
 * as it is inaccurate and "slow" way of handling resource relase.</li>
 * <li>The class using finalization feature must not contain finalize() 
 * method - not even protected/private one. This is because in MIDP the 
 * application developer might have overridden the class and implemented the 
 * own finalize() method for some total other purposes.</li>
 * </ul>
 * <p>
 * <h3>How to add a class to be finalized</h3> 
 * When a class needs the finalization support it must implement an inner class
 * that extends the com.nokia.mj.impl.rt.support.Finalizer class. The inner 
 * class must implement abstract method 
 * {@link Finalizer#finalizeImpl}, which is called by the JVMwhen the object 
 * is finalized.
 * It is not recommended to do the actual finalization actions in the 
 * {@link Finalizer#finalizeImpl} method - instead there should be a dedicated 
 * finalize method in the API class which is called from the {@link 
 * Finalizer#finalizeImpl} method. It should be noted that the dedicated 
 * finalize method name must be other than <code>finalize</code> (e.g.
 * <code>doFinalize</code>). 
 * <p>
 * The class needing the finalization support must store one instance of 
 * the inner class into a private member variable. It is recommended to do
 * the initialization of the member variable directly in the definition part
 * as it is done in the example below. When doing this, it should be noted
 * that finalization of the class is enabled even if some class dependent
 * initialization fails.
 * <p>
 * Once the class is enabled for finalization, the finalization support can't
 * be cancelled.
 * <p>
 * It is not relevant whether the inner class is anonymous or not, but it is 
 * easier to use anonymous approach.
 * <p>
 * <h3>Example</h3> 
 * <pre>
 * package com.nokia.mj.impl.example;
 * 
 * import com.nokia.mj.impl.rt.support.Finalizer;
 * 
 * public final class FinalizerExample
 * {
 *     // A reference to Finalizer class. Copy these two lines to your class.
 *     private Finalizer mFinalizer = new Finalizer() {
 *                                    public void finalizeImpl() {doFinalize();}};
 * 
 *     // Clears the resources. Copy this method to your class and implement
 *     // the needed cleaning actions into this method
 *     private void doFinalize() 
 *     {
 *         //Do the necessary finalizations
 *     }
 * }
 *</pre>
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public abstract class Finalizer
{

    /*static 
    {
        // Enable finalization of this class
        JvmInternal.enableFinalization(Finalizer.class);
    }*/

    /**
     * Called by the JVM when the finalization of the object happens.
     * When the object is finalized the JVM will call the <code>finalize</code>
     * method of the com.nokia.mj.impl.rt.support.Finalizer class which leads
     * to call of this abstract method. It is guaranteed that this method is 
     * called only once.
     * <p>
     * If some exception happens during the execution of 
     * <code>finalizeImpl</code> the Finalizer class will catch it and print it
     * to stderr.
     */
    public abstract void finalizeImpl();

    protected Finalizer()
    {
        debugFinalizeCreated();
    }
    
    /**
     * Finalization method, which the JVM calls when finalizing the object.
     */
    protected final void finalize()
    { 
        try 
        {
            debugFinalize();
            finalizeImpl();
        }
        catch (Throwable t) 
        { 
            // Print first the exception as such, in case the stack trace will
            // crash or take large amount of time (e.g. in stack overflow case)
            System.err.println("Exception caught in finalizer: " + t);
            
            t.printStackTrace();
        } 
    }


    /** Are finalizers being traced */
    private static boolean traceFinalizers = false;

    /** Hashtable for keeping references to DumpReferred */
    private static Hashtable references;

    // Set up debug finalizers
    static 
    {
        String s = System.getProperty("com.nokia.mj.impl.rt.support.FinalizerDump");

        if (s != null)
        {
            // Default = 60 seconds, if value is non-integer
            int delay = 60;
            try 
            {
                delay = Integer.parseInt(s);
            }
            catch (NumberFormatException ex)
            {
                // Ignore, and use default
            }
            System.out.println("Finalizer debug will dump finalize classes every " +
                               delay + " seconds");
        
            references = new Hashtable();
            new Timer().schedule(new DumpReferred(),
                                 1000 * delay, 1000 * delay);
        }

        s = System.getProperty("com.nokia.mj.impl.rt.support.FinalizerTrace");
        if (s != null)
        {
            System.out.println("Finalizer debug will trace each allocation/free");
            traceFinalizers = true;
        }
    }

    /** Class to keep count of finalizable objects */
    static private class Refcount 
    {
        /** Total number of objects of the class */
        int total;

        /** Current number of alive objects of the class */
        int count;

        /** Maximum number of simultaneously alive objects of the class */
        int max;

        /** Name of the class */
        String clazz;
        private Refcount(String c)
        {
            total = 1;
            count = 1;
            max = 1;
            clazz = c;
        }
    }

    /** Class to dump the finalizable object statistics periodically */

    static private class DumpReferred extends TimerTask
    {
        static private StringBuffer sb = new StringBuffer();
        public void run()
        {
            System.gc();
            //JvmInternal.runFinalization();
            synchronized(references)
            {
                sb.append("Finalizable objects\n");
                Enumeration e = references.elements();
                while (e.hasMoreElements())
                {
                    Refcount r = (Refcount)e.nextElement();
                    if (r != null)
                    {
                        sb.append(
                            " count="  + r.count + 
                            " max="   + r.max +
                            " total=" + r.total +
                            " class=" + r.clazz +
                            "\n");
                    }
                }
            }
            System.out.println(sb.toString());
            sb.setLength(0);
        }
    }


    private void debugFinalizeCreated()
    {
        if ((traceFinalizers || references != null) == false) 
        {
            return;
        }
        
        String clazz = this.getClass().getName();
        if (traceFinalizers) 
        {
            System.out.println("Finalizable created: " + clazz);
        }

        if (references != null)
        {
            // Add reference to hash
            synchronized (references) 
            {
                Refcount r = (Refcount)references.get(clazz);
                if (r != null)
                {
                    r.total++;
                    r.count++;
                    if (r.count > r.max) 
                    {
                        r.max = r.count;
                    }
                }
                else
                {
                    references.put(clazz, new Refcount(clazz));
                }
            }
        }
    }

    /**
     * Finalization debug method, which the subclasses override.
     */
    private void debugFinalize()
    { 
        if ((traceFinalizers || references != null) == false) 
        {
            return;
        }

        String clazz = this.getClass().getName();
        if (traceFinalizers) 
        {
            System.out.println("Finalizable destroyed " + clazz);
        }
        
        if (references != null)
        {
            // Reduce reference
            synchronized(references)
            {
                Refcount r = (Refcount)references.get(clazz);
                r.count--;
            }
        }
    }
}
