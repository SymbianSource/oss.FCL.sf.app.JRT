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


package com.nokia.mj.impl.rt.midp;

import javax.microedition.midlet.MIDletStateChangeException;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.StartUpTrace;

/**
 * A class for delegating the start and stop commands to the MIDlet. For
 * each call a new thread will be created. This is done because the it can't
 * be guarnteed that these calls always return. When using a dedicated threads
 * we avoid a possible blocking of the MIDP life cycle.
 */
final class MidletInvoker implements Runnable
{
    /**
     * Available tasks to the thread.
     */
    private static final int START_REQUEST   = 1;
    private static final int STOP_REQUEST    = 2;
    private static final int RESUME_REQUEST  = 3;
    private static final int PAUSE_REQUEST   = 4;

    /**
     * A task to be done by the thread.
     */
    private int mTask;

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * The constructor of MidletInvoker.
     * @param task to be done by the invoker thread.
     */
    private MidletInvoker(int task)
    {
        mTask = task;
    }

    /**
     * The default constructor of MidletInvoker. Not used.
     */
    private MidletInvoker()
    {
    }

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * A method for creating a thread for starting the MIDlet.
     */
    static void startMidlet()
    {
        if (Log.mOn) Log.logI("Creating a thread for starting MIDlet.");
        new Thread(new MidletInvoker(START_REQUEST)).start();
    }

    /**
     * A method for creating a thread for resuming the MIDlet.
     */
    static void resumeMidlet()
    {
        if (Log.mOn) Log.logI("Creating a thread for resuming MIDlet.");
        new Thread(new MidletInvoker(RESUME_REQUEST)).start();
    }

    /**
     * A method for creating a thread for stopping the MIDlet.
     */
    static void stopMidlet()
    {
        if (Log.mOn) Log.logI("Creating a thread for destroying MIDlet.");
        new Thread(new MidletInvoker(STOP_REQUEST)).start();
    }

    /**
     * A method for creating a thread for pausing the MIDlet.
     */
    static void pauseMidlet()
    {
        if (Log.mOn) Log.logI("Creating a thread for pausing MIDlet.");
        new Thread(new MidletInvoker(PAUSE_REQUEST)).start();
    }

    /*** ----------------------------- PUBLIC ------------------------------ */
    /**
     * An antry point of the thread. The thread will do the task set by the
     * creator of the thread.
     */
    public void run()
    {
        try
        {
            // Handle task set by the thread creator.
            switch (mTask)
            {
            case START_REQUEST:
                handleStartRequest();
                break;

            case STOP_REQUEST:
                handleStopRequest();
                break;

            case RESUME_REQUEST:
                handleResumeRequest();
                break;

            case PAUSE_REQUEST:
                handlePauseRequest();
                break;

            default:
                Log.logE("Unknown task in MidletInvoker! Task: "+ mTask);

            }
        }
        catch (Throwable th)
        {
            Log.logE("Error in MidletInvoker! ", th);
        }
    }

    /**
     * Handles the MIDlet start request. Will lead calling of
     * MIDlet.startApp().
     */
    private void handleStartRequest()
    {
        // Get a reference to MIDP life cycle.
        MidletLifeCycle lifeCycle = MidletLifeCycle.getInstance();

        try
        {
            // Start the memory monitoring feature.
            MemoryLogger.startMonitoring();

            // Get the MIDlet main class.
            String className = ApplicationInfoImpl.getMidletInfo()
                               .getMainClass();
            StartUpTrace.doTrace("Starting MIDLET");
            Log.logP("Starting MIDlet "
                     + ApplicationInfoImpl.getMidletInfo().getName() +
                     ", uid="
                     + ApplicationInfoImpl.getMidletInfo().getUid() +
                     ", className=" + className);

            // Load the main class. It should be noted that after this point
            // all the statics of the MIDlet class will be initialized. So this
            // might lead to initializations of some JSR(s) before an instance
            // of a MIDlet is even created!
            Class clazz = Jvm.loadApplicationClass(className);
            if (Log.mOn) Log.logI("  Class "+ clazz + " loaded");

            // This will call the constructor of the MIDlet. That will lead to
            // construction of javax.microedition.MidletApplication which is
            // inherited from com.nokia.mj.impl.rt.midp.MidletApplicationBase.
            // MidletApplicationBase will register itself to MIDP life cycle.
            Object midlet = clazz.newInstance();
            if (Log.mOn) Log.logI("  Object "+ midlet + " created");

            // Delegate the start cmd to the MIDlet. Will lead calling
            // of MIDlet.startApp().
            lifeCycle.getMidletApplication().invokeStartApp();
            if (Log.mOn) Log.logI("  startApp() returned");
            StartUpTrace.doTrace("MIDLET returned from startApp()");

        }
        catch (Throwable th)
        {
            // Inform the lifeCycle that the MIDlet starting has failed due to
            // some exception.
            lifeCycle.handleMidletStartUpFailure(th);
        }

        // Inform the lifeCycle that the MIDlet has been starterd.
        lifeCycle.midletStarted();
    }

    /**
     * Handles the MIDlet pause request. Will lead calling of MIDlet.pauseApp().
     * Originator is always the system, not the application it self.
     */
    private void handlePauseRequest()
    {
        // Get a reference to MIDP life cycle.
        MidletLifeCycle lifeCycle = MidletLifeCycle.getInstance();
        try
        {
            Log.logP("Calling MIDlet's pauseApp().");

            // Get the MidletApplication.
            MidletApplicationBase midletApplication =
                lifeCycle.getMidletApplication();

            // Delegate the pause cmd to the MIDlet if it is
            // constructed. Silently ignore the case where MIDlet
            // is not yet constructed. The use case is when
            // initializing some static variable of the MIDlet class
            // fails.
            if (midletApplication != null)
            {
                // Delegate the destroy cmd to the MIDlet. Will
                // lead calling of MIDlet.destroyApp().
                midletApplication.invokePauseApp();
            }
            if (Log.mOn) Log.logI("MIDlet's pauseApp() returned.");
        }
        catch (Throwable th)
        {
            // If exception is thrown destroy MIDlet.
            Log.logE("Error in pauseApp! ", th);
            lifeCycle.handleUncaughtException(th);
        }
    }


    /**
     * Handles the MIDlet stop request. Will lead calling of MIDlet.startApp().
     * Originator is always the system, not the application it self.
     */
    private void handleStopRequest()
    {
        // Get a reference to MIDP life cycle.
        MidletLifeCycle lifeCycle = MidletLifeCycle.getInstance();
        try
        {
            Log.logP("Calling MIDlet's destroyApp().");

            // Get the MidletApplication.
            MidletApplicationBase midletApplication =
                lifeCycle.getMidletApplication();

            // Delegate the destroy cmd to the MIDlet if it is
            // constructed. Silently ignore the case where MIDlet
            // is not yet constructed. The use case is when
            // initializing some static variable of the MIDlet class
            // fails.
            if (midletApplication != null)
            {
                // Delegate the destroy cmd to the MIDlet. Will
                // lead calling of MIDlet.destroyApp().
                midletApplication.invokeDestroyApp(true);
            }
            if (Log.mOn) Log.logI("MIDlet's destroyApp() returned.");
        }
        catch (Throwable th)
        {
            // Ignore any exceptions in destroy call. Only write error log.
            Log.logE("Error in destroyApp! ", th);
        }
        // Inform life cycle that the MIDlet has returned from destroyApp().
        lifeCycle.midletDestroyed();
    }

    /**
     * Handles the MIDlet resume request. Will lead calling of
     * MIDlet.startApp().
     */
    private void handleResumeRequest()
    {
        // Get a reference to MIDP life cycle.
        MidletLifeCycle lifeCycle = MidletLifeCycle.getInstance();
        boolean ok = false;
        try
        {
            if (Log.mOn) Log.logI("Resuming. Calling startApp() again");

            // Delegate the destroy cmd to the MIDlet if it is
            // constructed. Silently ignore the case where MIDlet
            // is not yet constructed.
            MidletApplicationBase midletApplication =
                lifeCycle.getMidletApplication();
            if (midletApplication != null)
            {
                midletApplication.invokeStartApp();
            }
            if (Log.mOn) Log.logI("  startApp() returned");
            ok = true;
        }
        catch (MIDletStateChangeException me)
        {
            if (Log.mOn) Log.logI("MIDlet threw MIDletStateChangeException, " +
                                      "when resuming");
        }
        catch (Throwable th)
        {
            Log.logE("Error when doing startApp when resuming! ", th);
        }
        lifeCycle.midletResumed(ok);
    }
}
