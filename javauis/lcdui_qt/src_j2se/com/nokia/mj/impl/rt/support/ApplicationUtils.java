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

import com.nokia.mj.impl.utils.Uid;
import java.util.Vector;
import java.util.Enumeration;
import java.security.Permission;
import java.security.AccessControlException;

/**
 * ApplicationUtils is an abstract base class for API implementations,
 * which allows them to use miscellanous utilities related to the running
 * application.
 * <p>
 * The class provides a static getter method for getting an instance of
 * the running application. In those runtimes that are always running only
 * one instance (like MIDP) returns always the same instance of the
 * ApplicationUtils. However those runtimes (like eRCP), that may run several
 * application in the same JVM instance, will return an application specific
 * instance.
 * <p>
 * The runtime implementation hides the solving of the correct application
 * when method <code>getInstace</code> is called. However it must be noted
 * that if the call is made some common thread, the <code>getInstace</code>
 * might not able to solve the correct application.
 * <p>
 * Example how to register a shutdown listener.
 * <pre>
 * package com.nokia.mj.impl.mypackage;
 *
 * import com.nokia.mj.impl.rt.support.ApplicationUtils;
 *      public void myClass()
 *      {
 *          public void myMethod()
 *          {
 *              // Get the insatnce of ApplicationUtils.
 *              ApplicationUtils appUtils = ApplicationUtils.getInstance();
 *
 *              // Get the name of the application.
 *              appUtils.addShutdownListener(new ShutdownListener()
 *              {
 *                  public void shuttingDown()
 *                  {
 *                      //Do cleaning...
 *                  }
 *
 *              });
 *
 *
 *              // My implementation...
 *          }
 *      }
 *</pre>
 * @author Nokia Corporation
 * @version 1.0
 */

public class ApplicationUtils
{
    public ApplicationUtils() {
    }

    /**
     * Name of the runtime specific class that extends this base class.
     */
    private static final String PORTCLASS = ".ApplicationUtilsImpl";

    /**
     * A singleton instance of runtime dependent instance of class extending
     * the ApplicationUtils base class.
     */
    protected static ApplicationUtils sInstance = null;

    /**
     * A set of listeners who wants to receive notification when the
     * application is abaout to close.
     */
    private Vector mListeners;

    /**
     * A lock for synchronizing the thw start up waiting feature.
     */
    private Object   mApplicationStartLock = new Object();

    /**
     * Information wheteher to continue start up or cancel it.
     */
    private Boolean  mStartCmd;


    /**
     * When this class is first loaded the static initializer will create a
     * singleton instance of runtime dependent instance of class extending the
     * ApplicationUtils base class. The class is created using dynamic
     * instantiation (<code>Class.forname & Class.newInstance</code>) and
     * the name of the class is a combination of PACKAGE_PREFIX +
     * &lt;value of system property PACKAGE_END_PROPERTY_NAME &gt;
     * + .&lt;value of PORTCLASS member variable &gt.
     */
    static
    {
        //Get the system proptery defining the end of package.
        /*String packageEnd =
               System.getProperty(ApplicationInfo.PACKAGE_END_PROPERTY_NAME);

        if(packageEnd == null)
        {
            throw new RuntimeException
                        ("Not able to load " + PORTCLASS + "class since " +
                        ApplicationInfo.PACKAGE_END_PROPERTY_NAME +
                        " system property is undefined");
        }

        //Construct the class
        String className = ApplicationInfo.PACKAGE_PREFIX +
                           packageEnd + PORTCLASS;
        try
        {
            Class clazz = Class.forName(className);
            sInstance = (ApplicationUtils)clazz.newInstance();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            throw new RuntimeException ("Not able to instantiate class " +
                                         className+". Reason is: " + e);
        }*/
        sInstance = new ApplicationUtils();
    }

    /**
     * Returns an instace of ApplicationUtils class which provides access to
     * application related runtime services.
     *
     * @return the ApplicationUtils object, null if the construction of the
     *         object has faild
     */
    public static ApplicationUtils getInstance()
    {
        return sInstance;
    }

    /**
     * Notifies the runtime that an exit command has been received and the
     * application is being tried to close nicely. The runtime is responsible
     * for killing the application if it doesn't close itself nicely after
     * specified grace time. The specified grace time is runtime dependent.
     * <p>
     * This method is meant ONLY for the UI and no other API are allowed
     * to use it. The UI must use this API ONLY when the user explicitly
     * wants to close the application using some platform depedent feataure
     * (e.g. red key in S60). If the user tries to close the application
     * using the mechanism provided by the application itself, this method
     * must not be called.
     * <p>
     * This method is a problematic in such runtimes that may run more than one
     * application in same JVM instance (e.g. eRCP). If the application
     * doesn't close nicely the runtime may have to terminate
     * the whole JVM process, which leads closing all the running applications
     * and the runtime itself.
     *
     */
    public void notifyExitCmd() {
        doShutdown();
    };

    /**
     * Determines whether the access request indicated by the specified
     * permission should be allowed or denied, based on the security policy
     * currently in effect. This method quietly returns if the access request
     * is permitted, or throws a suitable AccessControlException otherwise.
     * <p>
     * The design decision was that all the API implementation should use this
     * method for making security checkings and avoid using the
     * <code>checkPermission</code> method <code>AccessController</code> class,
     * which is left for application usage only.
     * <p>
     * The runtime implementation is responsible for forwarding security
     * checking to correct securty handler. In MIDP this would be Midp security
     * component while in 'main' type of application the handler would be the
     * <code>AccessController</code> class.
     *
     * @param p the requested permission.
     * @throws AccessControlException if the specified permission is not
     *         permitted, based on the current security policy.
     * @throws NullPointerException if the specified permission is null.
     */
    /*public abstract void checkPermission(Permission p)
                                         throws AccessControlException,
                                                NullPointerException;

    public abstract void checkPermission(Uid appUid,Permission p)
                                         throws AccessControlException,
                                                NullPointerException;
*/
    /**
     * Adds a shutdown notifications listener to receive notifications when
     * the application is about to close.
     * <p>
     * When the runtime receives information that the application is closing
     * (either from the application itself or from the system) the runtime
     * will send a notification to all registered listeners by calling
     * {@link ShutdownListener#shuttingDown() shuttingDown} method. All the
     * calls are made in a loop in one thread, so if some listener blocks the
     * call the rest of the listeners don't get the notification.
     * <p>
     * There can be more than one listener.
     *
     * @param listener the new listener.
     */
    public void addShutdownListener(ShutdownListener listener)
    {
        if(mListeners == null)
        {
            mListeners = new Vector();
        }
        mListeners.addElement(listener);
    }


    /**
     * Waits (if needed) until the application to be started is known.
     * This method can be used for pre-warming features. If the runtime is in
     * pre-warmed state where the application to be launched is not yet known,
     * this method will block. It is safe to call this method if the application
     * is already known - in that case the method won't block. Once the 
     * application is known all the waiters will be released. Knowing the 
     * application doesn't necessarily mean that the application it self 
     * has been created (might happen later). The method will provide info 
     * whether to continue or cancel the start up as a return value.
     * @return true if the user should continue starting, otherwise the user
     *              should do the required shut down proceures.
     */
    public boolean waitStart()
    {
        synchronized (mApplicationStartLock)
        {
            if(mStartCmd == null)
            {
                try
                {
                    mApplicationStartLock.wait();
                }
                catch (InterruptedException ie)
                {
                }
            }
            return mStartCmd == null ? false : mStartCmd.booleanValue();
        }
    }

    /**
     * Can be used to inform the runtime to pause the running application. The
     * origin could be e.g. flip close or application going to back ground.
     */
    public void pauseApplication()
    {
        // Empty implementation as default.
    }

    /**
     * Can be used to inform the runtime to resume the paused application. The
     * origin could be e.g. flip open or application going to fore ground.
     */
    public void resumeApplication()
    {
        // Empty implementation as default.
    }

    /**
     * Releases the start waiters.
     */
    protected void releaseStartWaiter(boolean doStart)
    {
        if(mApplicationStartLock != null)
        {
            synchronized (mApplicationStartLock)
            {
                mStartCmd = new Boolean(doStart);
                mApplicationStartLock.notifyAll();
            }
        }
    }

    /**
     * Sends shutdown notification to all the registered listeners.
     */
    protected void doShutdown()
    {
        if(mListeners != null)
        {
            Enumeration enumer = mListeners.elements();
            while ( enumer.hasMoreElements())
            {
                ShutdownListener listener =
                                (ShutdownListener)enumer.nextElement();
                listener.shuttingDown();
            }
        }
    }
}
