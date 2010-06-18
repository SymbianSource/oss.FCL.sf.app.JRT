/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Timer;
import java.util.TimerTask;

import java.security.AccessControlException;

import com.nokia.mj.impl.rt.utils.ExtensionUtil;
import com.nokia.mj.impl.rt.utils.CmdLineArgsPermission;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

import com.nokia.mj.impl.rt.legacy.LegacySupport;

import com.nokia.mj.impl.security.packageprotection.PackageProtector;
import com.nokia.mj.impl.security.common.RuntimeSecurityException;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.storage.AuthenticationStorageData;

import com.nokia.mj.impl.fileutils.FileUtility;

import com.nokia.mj.impl.coreui.CoreUi;

import com.nokia.mj.impl.utils.LineReader;
import com.nokia.mj.impl.utils.TaskQueue;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.StartUpTrace;


/**
 * A core of the MIDP life cycle. This class controls the life time of the
 * MIDlet.
 * <p>
 * The implementation of this class can be divided into three catgories:
 *   -The initializing of the life cycle.
 *   -The state machine.
 *   -The shut down of the life cycle.
 * <p>
 * Initialization can be divided into three different phases. The pre init
 * are the actions that can be done without knowing the MIDlet to be started.
 * The pre init phase is done always - even when going into pre warm state.
 * The last phase is the post actions which require knowing the MIDlet.
 * The pre warm init is a special phase, which is done only when the MIDlet
 * after firstly started into pre-warmed state. When the life cycle receves
 * indication that the MIDlet should be started and receives the MIDlet UID,
 * the life cycle will do first the pre-warm init phae and finally does the
 * post initial phase.
 * <p>
 * The state machine is based on a simple FIFO style thread safe task queue.
 * When the life cycle enters into state machine phase it starts wait for
 * various tasks. It should be noted that in order to ensure thread safeness
 * of the life cycle all most all actions should be done via sending a task
 * queue. Once the state machine is exited a shut down phase begins.
 * <p>
 * In the shut down phase the life cycle will firstly inform Java Captain that
 * it is about to start the shut down procedures. If anything should fail in
 * shut down procedure, it is a responsibility of the Java Captain to terminate
 * the MIDP process. The MIDP runtime tries to close gracefully the JVM, but
 * that will fail if any of the started Java threads dont stop as a result of
 * shut down notifications. That is considered a failure in the shut down phase
 * and the Java Captain will terminate forcefully the process.
 */
final class MidletLifeCycle
{
    /**
     * Possible states of the state machine
     */
    private static final int STARTING_INIT   = 0x1;
    private static final int PRE_INIT_DONE   = 0x2;
    private static final int POST_INIT_DONE  = 0x4;
    private static final int STARTING_MIDLET = 0x8;
    private static final int RUNNING         = 0x10;
    private static final int PAUSED          = 0x20;
    private static final int STOPPING_MIDLET = 0x40;
    private static final int CLOSED          = 0x80;
    private static final int RESUMING        = 0x100;

    /**
     * A singleton instance of the life cycle.
     */
    private static MidletLifeCycle       mInstance = new MidletLifeCycle();

    /**
     * Timers for ensuring that correct actions are done if MIDlet doesn't
     * return from destroyApp().
     */
    private        Timer                 mTimer;
    private        ShutDownTimerTask     mShutDownTimerTask;

    /**
     * Arguments recived from native starter.
     */
    private        MainArgs              mMainArgs;

    /**
     * The task queue of the life cycle.
     */
    private        TaskQueue             mTaskQueue;

    /**
     * The state of the state machine of the life cycle.
     */
    private        int                   mState;

    /**
     * A comms connection to Java Captain.
     */
    private        MidpComms             mMidpcomms;

    /**
     * The gate to the MIDlet. All the MIDP runtime specific actions from
     * and to MIDlet are done by using this object.
     */
    private        MidletApplicationBase mMidletApplication;

    /**
     * The UID of the MIDlet. Is not be available in pre-warm start.
     */
    private        Uid                   mMidletUid;

    /**
     * A pointer to native peer.
     */
    private        int                   mNativeRuntimeStarterHandle;

    /**
     * On error cases that require showing error dialogs an instace of
     * RuntimeErrorDialog will be created. Before starting shut down procedures
     * the life cycle will check if this contains a valid reference. On normal
     * case this is null, but if not then the dialog is prompted before
     * continuing shut down procedures.
     */
    private        RuntimeErrorDialog    mRuntimeErrDialog;

    /**
     * Flag for identifying if the life cycle should be started into pre-warmed
     * state.
     */
    private        boolean               mPrewarmStart = false;
    /**
     * Flag for identifying whether the MIDlet should be started to background.
     */
    private        boolean               mBackGroundStart = false;

    /**
     * How many times MIDlet has been 're-started'
     */
    private        int                   mRelaunchCount = 0;

    /**
     * This is set to true if the MIDlet is started with url and
     * the url contains MIDlet argument 'PromptAppStartup'
     */
    private        boolean               mAutoinvocationFromUrl = false;

    /**
     * PropertyKeys that are not listed in the values of Jad / Manifest
     * attribute Nokia-MIDlet-Launch-Params
     */
    private        Hashtable             mAcceptedUserProperties;

    /**
     * Flag for identifying whether the MIDlet is a standalone MIDlet.
     */
    private        boolean               mStandAlone = false;

    /*** ----------------------------- PRIVATE ---------------------------- */


    /**
     * The constructor of the MidletLifeCycle.
     */
    private MidletLifeCycle()
    {
        mState = STARTING_INIT;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Will return a singleton instance of the life cycle.
     * @return a singleton instance of the life cycle.
     */
    static MidletLifeCycle getInstance()
    {
        return mInstance;
    }

    /**
     * Can be used for ensuring that important cleanings has been done if some
     * exception has occured during the shut down procedures. It also destroys
     * the singelton instance of the life cycle.
     */
    static void destroyInstance()
    {
        if (mInstance != null)
        {
            // Ensure that DRM rights are consumed when the MIDlet is
            // closed.
            DrmUtil.consumeRightsStop();

            if (mInstance.mState != CLOSED)
            {
                _closeInd(mInstance.mNativeRuntimeStarterHandle);
            }
            if (mInstance.mMidpcomms != null)
            {
                mInstance.mMidpcomms.close();
            }

            mInstance = null;
        }
    }

    /**
     * Entry point for starting the life cycle.
     * @param args The arguments provided to MIDP runtime.
     */
    void doRun(String[] args)
    {
        // Parse given args.
        mMainArgs = new MainArgs(args);

        // Make intializations that can be done without knowing the MIDlet to
        // be launched.
        doPreInit();
        startStateMachine();

        DrmUtil.consumeRightsStop();

        if (mRuntimeErrDialog != null)
        {
            mRuntimeErrDialog.showDialog();
        }

        if (!mStandAlone)
        {
            if (Log.mOn) Log.logI("Sending termination indication to Captain.");
            mMidpcomms.sendTerminatedIndication(0);

            if (Log.mOn) Log.logI("Closing COMMS.");
            mMidpcomms.close();
            mMidpcomms = null;
        }

        if (Log.mOn) Log.logI("Sending shutdown notifications to listeners.");
        ApplicationUtilsImpl.doShutdownImpl();

        if (Log.mOn) Log.logI("Sending close indication to runtime starter.");
        _closeInd(mNativeRuntimeStarterHandle);


        if (Log.mOn) Log.logI("Short pause before closing LegacySupport.");
        try
        {
            Thread.sleep(200);
        }
        catch (Exception e)
        {
        }

        if (Log.mOn) Log.logI("Closing LegacySupport.");
        LegacySupport.close();

        // Setting state to closed.
        mState = CLOSED;
    }

    /**
     * For setting the MidletApplication. This is set by the constructor of
     * the MidletApplicationBase clase during the construction of the MIDlet
     * class. See description of the MidletApplicationBase and the
     * mMidletApplication.
     * @param midletApplication The MidletApplication to be set.
     */
    void setMidletApplication(MidletApplicationBase midletApplication)
    {
        mMidletApplication = midletApplication;
    }

    /**
     * For getting the MidletApplication. See description of the
     * MidletApplicationBase and the mMidletApplication.
     * @return The MidletApplication.
     */
    MidletApplicationBase getMidletApplication()
    {
        return mMidletApplication;
    }

    /**
     * For notifying that MIDlet is entering into destroyed state. The
     * originator of this call is the MIDlet itself.
     */
    void notifyDestroyed()
    {
        if (Log.mOn) Log.logI("notifyDestroyed() called.");
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.STOP_REQUEST,
                                             LifeCycleTask.APP_STOP_REQUEST));
    }

    /**
     * For notifying that MIDlet is entering into paused state. The
     * originator of this call is the MIDlet itself.
     */
    void notifyPaused()
    {
        if (Log.mOn) Log.logI("Pause notification from MIDlet received.");
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.PAUSE_REQUEST));
    }

    /**
     * For notifying the MIDlet that something int the device has happened
     * that would require MIDlet to enter into   entering into actice state from paused
     * state. The originator of this call is the MIDlet itself or the user
     * of the ApplicationUtils.resumeApplication method of the Runtime support
     */
    void pauseApplication()
    {
        if (Log.mOn) Log.logI("Pause application request received.");
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.PAUSE_APP_REQUEST));
    }

    /**
     * For notifying that MIDlet is entering into actice state from paused
     * state. The originator of this call is the MIDlet itself or the user
     * of the ApplicationUtils.resumeApplication method of the Runtime support.
     */
    void resumeRequest()
    {
        if (Log.mOn) Log.logI("Resume request received.");
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.RESUME_REQUEST));
    }

    /**
     * For handling the start request received from the Java Captain. There are
     * two use cases when this could happen:
     *   - The MIDP runtime was in pre-warmed state and received a start cmd.
     *   - The MIDlet is already running and user "re-starts" the MIDlet.
     */
    void launchMidletRequest(Uid uid, boolean backGroundStart, String applicationArgs)
    {
        if (Log.mOn) Log.logI("Request for launching MIDlet received.");
        // Store the info whether MIDlet should be started to back ground.
        mBackGroundStart = backGroundStart;

        // Update relaunch count if really relaunching (not when activating
        // prewarmed MIDlet)
        if (mState != PRE_INIT_DONE)
        {
            mRelaunchCount++;
        }
        JvmInternal.setSystemProperty("com.nokia.mid.cmdline.instance",
                                      Integer.toString(mRelaunchCount + 1));
        if (Log.mOn)
            Log.logI("MIDlet launch count is : " + Integer.toString(mRelaunchCount + 1));

        if (applicationArgs != null)
        {
            JvmInternal.setSystemProperty("com.nokia.mid.cmdline", applicationArgs);
        }

        if (mState == PRE_INIT_DONE)
        {
            // We were waiting in prewarmed state the launch request
            // and now got it.
            StartUpTrace.doTrace("Start request in pre-warm state received.");
            mMidletUid = uid;
            mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.START_REQUEST,
                                                 LifeCycleTask.PRE_WARM_START));
        }
        else
        {
            // Update possible MIDlet arguments
            setMidletArguments();
            // Bring the MIDlet to foreground
            CoreUi.foregroundRequest();
        }
    }

    /**
     * For handling the terminate request originated by the system. There are
     * two sources for entering into this method - Java Captain sends a message
     * to the MIDP runtime that the MIDlet should be closed. In that case the
     * Captain has already a timer running for failures in shut down
     * procedures. In that case the internal timer won't be started.
     * The second option is that some one has called notifyExitCmd() of the
     * Runtime Support APIs. In that case a timer will be started to ensure
     * the shut down also in case the MIDlet doesn't return from the
     * destroyApp() call.
     * @param useTimer Info whether to start the interna timer for ensuring
     *                  proper shut down.
     */
    void terminateMidletRequest(boolean useTimer)
    {
        Log.logP("Request for terminating MIDlet " + mMidletUid +
                 "  received.");
        if (Log.mOn) Log.logI("  Internal timer used: " + useTimer);
        if (mState != STOPPING_MIDLET && mState != CLOSED)
        {
            if (useTimer)
            {
                startShutDownTimer();
            }
            mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.STOP_REQUEST,
                                                 LifeCycleTask.SYSTEM_STOP_REQUEST));
        }
        else
        {
            if (Log.mOn) Log.logI("  Already doing shutdown.");
        }
    }

    /**
     * For notifying that the MIDletInvoker has succesfully returned from
     * startApp() call of the MIDlet.
     */
    void midletStarted()
    {
        if (Log.mOn) Log.logI("MIDlet started indication received.");
        mTaskQueue.addTask(
            new LifeCycleTask(LifeCycleTask.MIDLET_RUNNING_IND));
    }

    /**
     * For notifying that the MIDletInvoker has succesfully returned from
     * startApp() call of the MIDlet as a result of resumeRequest.
     */
    void midletResumed(boolean ok)
    {
        if (Log.mOn) Log.logI("MIDlet resumed indication received. " + ok);
        if (ok)
        {
            mTaskQueue.addTask(
                new LifeCycleTask(LifeCycleTask.MIDLET_RUNNING_IND));
        }
    }

    /**
     * For notifying that the MIDletInvoker has succesfully returned from
     * destroyApp() call.
     */
    void midletDestroyed()
    {
        Log.logP("MIDlet " + mMidletUid + " has been destroyed.");
        if (Log.mOn) Log.logI("MIDlet destroyed indication received.");
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.STOPPED_IND));
    }

    /**
     * For notifying that an exception has occured during the MIDlet start.
     * @param th The throwable that was catched during the start.
     */
    void handleMidletStartUpFailure(Throwable th)
    {
        Log.logE("Failed to start MIDlet! ", th);
        mRuntimeErrDialog = RuntimeErrorDialog.getStartUpErrorDialog(th);
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.STARTUP_FAILURE));
    }

    /**
     * For notifying that an exception has occured during pausing the MIDlet.
     * The MIDlet will be destroyed.
     */
    void handleMidletPauseAppFailure()
    {
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.PAUSE_FAILURE));
    }

    /**
     * For notifying that some thread didn't catch a throwable.
     * @param th The throwable that some thread didn't catch.
     */
    void handleUncaughtException(Throwable th)
    {
        Log.logE("Uncaught exception! ", th);
        mRuntimeErrDialog = RuntimeErrorDialog.getUnhandledDialog(th);
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.UNCAUGHT_EXCEPTION));
    }

    /**
     * A getter for info whether the MIDlet should be started into background.
     * @return true if application should be started into back ground, false
     *         otherwise.
     */
    boolean isBackGroundStart()
    {
        // If the background flag has already been set by starter just return
        // If not then we need to check if the user has been set the UI into
        // background before the UI toolkit has been activated.
        if (!mBackGroundStart)
        {
            mBackGroundStart = !CoreUi.isUiInForeground();
        }
        return mBackGroundStart;
    }


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * For starting the state machine of the life cycle
     */
    private void startStateMachine()
    {
        if (Log.mOn) Log.logI("Entering main loop.");

        if (mPrewarmStart)
        {
            _restoreNormalProcessPriority();
            int shrinkSize = JvmInternal.shrinkJavaHeapToMinimum();
            StartUpTrace.doTrace("Heap shrunk by " + shrinkSize + " bytes.");
            if (Log.mOn) Log.logI("Heap shrunk by " + shrinkSize + " bytes.");
        }
        boolean loop = true;
        while (loop)
        {
            LifeCycleTask task = (LifeCycleTask)mTaskQueue.getTask();
            int mainTask = task.getMainTask();
            int subTask = task.getSubTask();

            switch (mainTask)
            {
                // Start request. Originator is Java Captain.
            case LifeCycleTask.START_REQUEST:
                handleStartRequest(subTask);
                break;

                // Running indication. Originator is MidletInvoker.
            case LifeCycleTask.MIDLET_RUNNING_IND:
                if (Log.mOn) Log.logI("LOOP: Handling MIDLET_RUNNING_IND.");
                mState = RUNNING;
                break;

                // Stop request. Originator is either Java Captain,
                // Runtime Support API caller or shut down timer..
            case LifeCycleTask.STOP_REQUEST:
                loop = handleStopRequest(subTask);
                break;

                // Stopped indication. Originator is MidletInvoker.
            case LifeCycleTask.STOPPED_IND:
                if (Log.mOn) Log.logI("LOOP: Handling STOPPED_IND.");
                cancelShutDownTimer();
                loop = false;
                break;

                // Pause request. Originator is MIDlet.
            case LifeCycleTask.PAUSE_REQUEST:
                handlePauseRequest();
                break;

                // Pause application request. Originator is MIDlet or the user
                // of the ApplicationUtils.pauseApplication method of the
                // Runtime support. This leads to call of MIDlet.pauseApp().
            case LifeCycleTask.PAUSE_APP_REQUEST:
                handlePauseAppRequest();
                break;

                // Resume request. Originator is MIDlet or the user
                // of the ApplicationUtils.resumeApplication method of the
                // Runtime support.
            case LifeCycleTask.RESUME_REQUEST:
                handleResumeRequest();
                break;

                // Error in startApp request. Originator is MidletInvoker.
            case LifeCycleTask.STARTUP_FAILURE:
                // Error in pauseApp request. Originator is MidletInvoker.
            case LifeCycleTask.PAUSE_FAILURE:
                MidletInvoker.stopMidlet();
                loop = false;
                break;

            case LifeCycleTask.UNCAUGHT_EXCEPTION:
                // Midlet will be immediately destroyd without calling
                // destroyApp().
                loop = false;
                break;
            default:
                Log.logE("LOOP: Unknown task: " + mainTask);
                break;
            }
        }
        mState = CLOSED;
        if (Log.mOn) Log.logI("Leaving main loop.");
    }

    /**
     * Handles the start request task. Allowed to be called only from
     * startStateMachine() method. The only intention of this method is
     * to diminish the switch case clause in the startStateMachine()
     * method.
     * @param subTask The sub task provided by the task setter.
     */
    private void handleStartRequest(int subTask)
    {
        if (Log.mOn) Log.logI("MidletLifeCycle.handleStartRequest(), subTask: "
                                  + subTask);
        if ((mState == POST_INIT_DONE && subTask != LifeCycleTask.PRE_WARM_START) ||
            (mState == PRE_INIT_DONE && subTask == LifeCycleTask.PRE_WARM_START))
        {
            if (subTask == LifeCycleTask.NORMAL_START)
            {
                if (Log.mOn) Log.logI("  Normal start");
                mState = STARTING_MIDLET;
                MidletInvoker.startMidlet();
            }
            else if (subTask == LifeCycleTask.PRE_WARM_START)
            {
                if (Log.mOn) Log.logI("  Pre-warm start");
                doPreWarmInit();
            }
        }
        else
        {
            if (Log.mOn) Log.logI("  No start, since the state was incorrect: "
                                      + mState);
        }
    }

    /**
     * Handles the stop request task. Allowed to be called only from
     * startStateMachine() method. The only intention of this method is
     * to diminish the switch case clause in the startStateMachine()
     * method.
     * @param subTask The sub task provided by the task setter.
     * @return true if the task listening should continue, false otherwise.
     */
    private boolean handleStopRequest(int subTask)
    {
        if (Log.mOn) Log.logI("MidletLifeCycle.handleStopRequest(), subTask: "
                                  + subTask);
        mState = STOPPING_MIDLET;
        boolean loop = true;
        if (subTask == LifeCycleTask.APP_STOP_REQUEST)
        {
            if (Log.mOn) Log.logI("  Request from app -> no destroyApp call.");
            loop = false;
        }
        else if (subTask == LifeCycleTask.SYSTEM_STOP_REQUEST)
        {
            if (Log.mOn) Log.logI("  Request from system -> call destroyApp.");
            MidletInvoker.stopMidlet();
        }
        else if (subTask == LifeCycleTask.DO_FORCED_TERMINATE)
        {
            if (Log.mOn) Log.logI("  Doing forceful termination.");
            loop = false;
        }
        return loop;
    }

    /**
     * Handles the pause request task. Allowed to be called only from
     * startStateMachine() method. The only intention of this method is
     * to diminish the switch case clause in the startStateMachine()
     * method. This is called when the MIDlet itself wants to go to
     * paused state.
     */
    private void handlePauseRequest()
    {
        if (Log.mOn) Log.logI("MidletLifeCycle.handlePauseRequest()");
        if (mState != RUNNING)
        {
            Log.logW("Pause request received in illegal state: " + mState);
        }
        mState = PAUSED;
    }

    /**
     * Handles the pause app request task. Allowed to be called only from
     * startStateMachine() method. The only intention of this method is
     * to diminish the switch case clause in the startStateMachine()
     * method. This is called when the System wants the MIDlet change to
     * paused state.
     */
    private void handlePauseAppRequest()
    {
        if (Log.mOn) Log.logI("MidletLifeCycle.handlePauseAppRequest()");
        if (mState != RUNNING)
        {
            Log.logW("Pause request received in illegal state: " + mState);
        }
        MidletInvoker.pauseMidlet();
        mState = PAUSED;
    }



    /**
     * Handles the resume request task. Allowed to be called only from
     * startStateMachine() method. The only intention of this method is
     * to diminish the switch case clause in the startStateMachine()
     * method.
     */
    private void handleResumeRequest()
    {
        if (Log.mOn) Log.logI("MidletLifeCycle.handleResumeRequest()");
        if (mState == PAUSED)
        {
            mState = STARTING_MIDLET;
            MidletInvoker.resumeMidlet();
        }
        else
        {
            Log.logW("Resume request received in illegal state: " + mState);
        }
    }

    /**
     * For informing the JVM about the protected and restricted packages.
     */
    private void setClassProtection()
    {
        if (Log.mOn) Log.logI("Setting protected classes.");
        PackageProtector protector = PackageProtector.getInstance();
        JvmInternal.addProtectedPackagePrefixes(
            protector.getProtectedPackageNames());
        JvmInternal.addRestrictedPackagePrefixes(
            protector.getRestrictedPackageNames());
    }

    /**
     * For doing the initializations that can be done before knowing the UID
     * of the MIDlet
     */
    private void doPreInit()
    {
        if (Log.mOn) Log.logI("Doing pre init.");

        // Load the native.
        Jvm.loadSystemLibrary("javamidpruntime");

        // Start to monitor uncaught exceptions.
        JvmInternal.setThreadEventListener(new ThreadEventListener()
        {
            public void threadStarting(Thread newThread, Thread parentThread)
            {
                if (Log.mOn) Log.logI("threadStarting newThread:" + newThread
                                          + ", parentThread:"+parentThread);
            }
            public void threadDied(Thread thread)
            {
                if (Log.mOn) Log.logI("threadDied thread:"+thread);
            }
            public void uncaughtException(Thread thread, Throwable throwable)
            {
                handleUncaughtException(throwable);
            }
        });

        // Store the starter handle for native access.
        mNativeRuntimeStarterHandle =
            Integer.parseInt(mMainArgs.findArgument("-handle"));

        mStandAlone = mMainArgs.findArgument("-standalone") != null;
        if (Log.mOn) Log.logI("Is standalone MIDlet = " + mStandAlone);

        if (!mStandAlone)
        {
            // Do not allow System.exit().
            JvmInternal.disableRuntimeExit();

            // Set protected and restricted packages.
            setClassProtection();

            // Check if there are add-on JSRs.
            ExtensionUtil.handleExtensions();
        }
        else
        {
            ApplicationUtilsImpl.setStandAloneMode();
        }

        mTaskQueue = new TaskQueue();

        // If system property com.nokia.mid.cmdline has value, it contains
        // the arguments for the current MIDlet.
        String encodedMidletArgs = System.getProperty("com.nokia.mid.cmdline");
        if ((encodedMidletArgs != null) && (encodedMidletArgs.length() > 0))
        {
            // Decode arguments
            String midletArgs = decodeArgs(encodedMidletArgs);

            // Resetting the system property after decoding.
            JvmInternal.setSystemProperty("com.nokia.mid.cmdline", midletArgs);
        }

        // If the runtime is set to pre warmed state, then the
        // value of the prewarm flag contans the process id which
        // is needed by Captain for identification.
        String pid = mMainArgs.findArgument("-prewarm");
        mPrewarmStart = pid != null;
        mState = PRE_INIT_DONE;
        if (!mPrewarmStart)
        {
            // We know the MIDlet to be launched, so post init can be done.
            mMidletUid = Uid.createUid(mMainArgs.findArgument("-uid"));
            if (!CoreUi.connectToUi())
            {
                // If not in prewarmed state we need to connect
                // to core UI. If the core Ui is closed, then
                // don't continue the start up.
                if (Log.mOn) Log.logI("CoreUi was closed - so no start.");
                throw new StartupException("Core UI closed.",false);
            }
            // Cache the info if the intention is to start the MIDlet into
            // background.
            mBackGroundStart = mMainArgs.findArgument("-background") != null;

            doPostInit();

            if (!mStandAlone)
            {
                // Create a comms connection to Java Captain.
                mMidpcomms = new MidpComms(mMidletUid);

                // Send running indication with success status.
                mMidpcomms.sendRunningIndication(0);
            }
        }
        else
        {
            // Create a comms connection to Java Captain.
            mMidpcomms = new MidpComms();

            // Send running indication. Send the pid to Captain for
            //identification.
            mMidpcomms.sendProcessRunningIndication(Integer.parseInt(pid));
            wakeUpUi();
        }
    }

    /**
     * Decode string in a private format that was safe to pass to Java side
     * as a Java system property.
     * Native side function java::util::runtime::MidpRuntimeStarter::encodeArgs()
     * was used to encode the string
     *
     * @param encodedArgs encoded arguments
     * @return decoded argument string
     */
    private String decodeArgs(String encodedArgs)
    {
        StringBuffer res = new StringBuffer();
        int  idx = 0;
        int  len = encodedArgs.length();
        char t;

        // Decoding is done in blocks of 4 or 2 characters.
        // Possible extra chars are ignored

        while (idx < len)
        {
            t = encodedArgs.charAt(idx);
            if (t < 'a')
            {
                if (idx + 3 >= len)
                    break;
                // decode one 16-bit char
                char a = (char)(t - 'A');
                char b = (char)(encodedArgs.charAt(idx+1) - 'A');
                char c = (char)(encodedArgs.charAt(idx+2) - 'A');
                char d = (char)(encodedArgs.charAt(idx+3) - 'A');
                char r = (char)(((a<<12)+(b<<8)+(c<<4)+d));
                res.append(r);
                idx = idx + 4;
            }
            else
            {
                if (idx + 1 >= len)
                    break;
                // decode one 8-bit char
                char a = (char)(t - 'a');
                char b = (char)(encodedArgs.charAt(idx+1) - 'a');
                char r = (char)((a<<4)+b);
                res.append(r);
                idx = idx + 2;
            }
        }

        return res.toString();
    }

    /**
     * This method is meant for informing the UI that the MIDP runtime has
     * entered into pre-warmed state. The method simply tries to instatiate
     * a class provided by a specified system property. In the constructor
     * of the object the UI may start the pre warming steps.
     */
    private void wakeUpUi()
    {
        // Get the class name.
        String className = System.getProperty("com.nokia.mj.impl.ui");
        if (className != null)
        {
            // If the property is set, instantiate the class.
            try
            {
                Class clazz = Class.forName(className);
                clazz.newInstance();
            }
            catch (Throwable t)
            {
                // Silently ignore exceptions.
            }
        }
    }

    /**
     * This method does the necessary initializations after leaving the pre-
     * warmed state. When calling this method, the MIDlet UID must be known.
     * Heap size is not set.
     */
    private void doPreWarmInit()
    {
        if (Log.mOn) Log.logI("Doing prewarm init.");

        doPostInit();

        // The native peer doesn't know the UID of the started MIDlet.
        // Informing native.
        _setUids(mMidletUid.toString(),
                 ApplicationInfoImpl.getMidletInfo().getSuiteUid().toString(),
                 mNativeRuntimeStarterHandle);
    }

    private void doPostInit()
    {
        if (Log.mOn) Log.logI("Doing post init.");

        // Set the MidletInfo class to contain correct data. The data is read
        // from storage.
        setMidletInfo();

        // If system property com.nokia.mid.cmdline has value, it contains
        // the arguments for the current MIDlet.
        setMidletArguments();

        if (mPrewarmStart)
        {
            // Get the recorded heap size from previous run.
            int targetHeapSize = MemoryLogger.getOldHeapSizeToExpand();

            int expandedSize = JvmInternal.expandJavaHeap(targetHeapSize);
            StartUpTrace.doTrace("Heap expand req:"+ targetHeapSize +" B. Result: "+ expandedSize + " B.");
            // Do the post init actions.
            // In case of pre-warm start we need to create the core ui
            // from Java side. In order to ensure that Runtime Support
            // API works ok, the setMidletInfo must be called before
            // starting the UI.
            CoreUi.createUi(mMidletUid, mBackGroundStart);

            // The Jvm doesn't know about MIDlet class path - need to set it.
            String classPath = ApplicationInfoImpl.getMidletInfo().getClassPath();
            if (Log.mOn) Log.logI("  Adding to classpath: "+classPath);
            JvmInternal.appendToClassPath(classPath);
        }
        if (!mStandAlone)
        {

            // Do the MIDlet suite authentication.
            verifyMIDletSuiteAuthenticity();

            // Inform the DRM util that the MIDlet is about to start.
            DrmUtil.consumeRightsStart();
        }

        // If the MIDlet launch is a result of auto invocation we need to
        // ensure that user allows the start up.
        if ((mMainArgs.findArgument("-autoinvocation") != null) ||
                mAutoinvocationFromUrl)
        {
            if (Log.mOn) Log.logI("Ensuring autoinvocation.");
            String pushAdditionalInfo =
                mMainArgs.findArgument("-autoInvocationAdditional");
            if (Log.mOn) Log.logI("  addInfo: '" + pushAdditionalInfo + "'");

            // ensure security
            try
            {
                ApplicationUtils appUtils = ApplicationUtils.getInstance();
                CmdLineArgsPermission cmdLineArgsPermission = 
                    new CmdLineArgsPermission();
                appUtils.checkPermission(cmdLineArgsPermission);
                if (Log.mOn) Log.logI("Autoinvocation allowed.");
            }catch(AccessControlException e)
            {
                if (Log.mOn) Log.logI("Autoinvocation NOT allowed.");                
                throw new StartupException(e.toString(),
                                           false);
            }
        }

        // Initialize the legacy support layer if exists.
        LegacySupport.init(
            ApplicationInfoImpl.getMidletInfo().getMidletAttributes(),
            isBackGroundStart());

        mState = POST_INIT_DONE;

        // All the initializations done now, ready to start the MIDlet.
        mTaskQueue.addTask(new LifeCycleTask(LifeCycleTask.START_REQUEST,
                                             LifeCycleTask.NORMAL_START));

        // If there are any waiters for the start up, release those.
        ApplicationUtilsImpl.releaseStartWaiterImpl(true);
    }

    /**
     * Verifies the authenticity of MIDlet suite.
     * @throws StartupException on error case.
     */
    private void verifyMIDletSuiteAuthenticity()
    {
        if (Log.mOn) Log.logI("Verifying MIDlet Suite authenticity.");
        // Get instance of MidleInfo for getting arguments.
        MidletInfo midletInfo = ApplicationInfoImpl.getMidletInfo();

        try
        {
            // Get instance of AuthenticationModule for doing the verification.
            AuthenticationModule authenticationModule =
                AuthenticationModule.getInstance();


            // Set a data object to conatin necessary info used during
            // verificaton.
            AuthenticationStorageData securityStorageData =
                new AuthenticationStorageData(
                midletInfo.getProtectionDomainName(),
                midletInfo.getProtectionDomain(),
                midletInfo.getMidletHash(),
                midletInfo.getRootHash(),
                midletInfo.getClassPath());
            authenticationModule.verifyMIDletSuiteAuthenticity(
                midletInfo.getSuiteUid(),
                securityStorageData);
        }
        catch (RuntimeSecurityException rse)
        {
            // Display the error to user
            RuntimeErrorDialog.showAuthenticationFailed(rse);
            throw new StartupException("Suite authentication failed.");
        }
        if (Log.mOn) Log.logI("MIDlet Suite authenticity verified.");
    }

    /**
     * Sets the MIDlet info to contain the MIDlet data from Java Storage.
     */
    private void setMidletInfo()
    {
        if (Log.mOn) Log.logI("Setting MIDlet data.");
        // Create MidletInfo instance for storing the ApplicationInfo for the
        // runtime support.
        MidletInfo midletInfo = new MidletInfo();
        midletInfo.setUid(mMidletUid);

        if (!mStandAlone)
        {

            // Read the "static" arguments. These are used for starting the MIDlet.
            StorageAccessor.setMidletStartArguments(midletInfo);

            // Read the MIDlet attributes defined in the manifest and Jad (if
            // present).
            StorageAccessor.setMidletAttributes(midletInfo);

        }
        else
        {
            // Read the MIDlet attributes defined in the manifest and Jad (if
            // present).
            SaMidletInfoProvider.setMidletAttributes(midletInfo, mMainArgs.findArgument("-jad"));

            // Read the "static" arguments. These are used for starting the MIDlet.
            SaMidletInfoProvider.setMidletStartArguments(midletInfo);
            // Set the root path of the MIDlet suite.
            midletInfo.setRootPath(mMainArgs.findArgument("-rootpath"));
        }
        // ApplicationInfoImpl will store the MIDlet info.
        ApplicationInfoImpl.setMidletInfo(midletInfo);
        if (Log.mOn) Log.logI("MidletInfo: " + midletInfo);
    }

    /**
     * Starts the shut down timer if not yet started. The timer is for
     * ensuring the proper shut down in a case where the MIDlet doesn't
     * return from destroyApp() in given time. The time is 5 seconds.
     */
    private void startShutDownTimer()
    {
        if (Log.mOn) Log.logI("Starting shutdown timer.");
        if (mTimer == null)
        {
            mTimer = new Timer();
            mShutDownTimerTask = new ShutDownTimerTask();
            mTimer.schedule(mShutDownTimerTask, 5000);
        }
        else
        {
            Log.logW("Shutdown timer already running");
        }

    }

    /**
     * Cancels the shut down timer.
     */
    private void cancelShutDownTimer()
    {
        if (Log.mOn) Log.logI("Cancelling shutdown timer.");
        if (mTimer != null)
        {
            mTimer.cancel();
            mTimer = null;
            mShutDownTimerTask = null;
        }
    }

    /**
     * Parse the MIDlet arguments given as parameter and set them
     * to system properties so that MIDlet can access them.
     */
    private void setMidletArguments()
    {
        String applicationArgs = System.getProperty("com.nokia.mid.cmdline");

        if (applicationArgs != null && applicationArgs.length() > 0)
        {
            if (Log.mOn)
                Log.logI("MIDlet arguments are : " + applicationArgs);

            // parse args and set individual system properties
            // e.g. "startMode=startFromCmdLine;sound=ON;wizard_mode;landscapeMode=true"
            int idx;
            int indEq;
            int cur = 0;
            StringBuffer argBuf = new StringBuffer();
            String arg;
            String propertyKey;
            String propertyValue;
            mAutoinvocationFromUrl = false;
            if (mAcceptedUserProperties == null)
            {
                // Doing intialization only once.
                String launchParams =
                  ApplicationInfoImpl.getMidletInfo().getAttribute("Nokia-MIDlet-Launch-Params");
                mAcceptedUserProperties = split(launchParams, ",");
            }

            do
            {
                argBuf.setLength(0);
                idx = applicationArgs.indexOf(";", cur);

                if (idx == 0)
                {
                    // Arguments string started with ';'
                    cur = idx + 1;
                    continue;
                }

                if (idx != -1)
                {
                    argBuf.append(applicationArgs.substring(cur, idx));
                }
                else
                {
                    argBuf.append(applicationArgs.substring(cur));
                }

                // split one arg to key and value
                arg = argBuf.toString();
                indEq = arg.indexOf('=');
                propertyKey = null;
                propertyValue = "";

                if (indEq == -1)
                {
                    // The property doesn't have a value.
                    propertyKey = arg;
                }
                else
                {
                    // The property has also a value.
                    propertyKey = arg.substring(0, indEq);
                    propertyValue = arg.substring(indEq + 1);
                }

                if (propertyKey.equals("PromptAppStartup"))
                {
                    mAutoinvocationFromUrl = true;
                    if (Log.mOn)
                        Log.logI("MIDlet had argument PromptAppStartup");
                }

                if (mAcceptedUserProperties.get(propertyKey) != null)
                {
                    JvmInternal.setUserProperty(propertyKey, propertyValue);
                }

                cur = idx + 1;
            }
            while ((idx != -1) && (cur < applicationArgs.length()));
        }
    }

    /**
     * Split a string into Hashtable. This util method splits the given
     * string separated by given separator into Hashtable so that tokens
     * are keys and values are just empty Strings. Before adding the tokens
     * into hashtable those are trimmed.
     *
     * @str String to split.
     * @separator A separator that separates the tokens.
     */
    static Hashtable split(String str, String separator)
    {
        Hashtable ht = new Hashtable();
        if (separator != null)
        {
            int separatorLen = separator.length();
            if (str != null && separatorLen > 0)
            {
                int index = str.indexOf(separator);
                while (index != -1)
                {
                    String token = str.substring(0,index).trim();
                    ht.put(token, "");
                    str = str.substring(index + separatorLen);
                    index = str.indexOf(separator);
                }
            }
        }
        if (str != null)
        {
            str = str.trim();
            if (str.length() > 0)
            {
                // add token after last separator
                ht.put(str, "");
            }
        }
        return ht;
    }

    /**
     * Class for extending the TimerTask. This is for the shut down timer.
     */
    private class ShutDownTimerTask extends TimerTask
    {
        /**
         * Method to be run if the shut down timer elapses.
         */
        public final void run()
        {
            try
            {
                if (Log.mOn) Log.logI("TIMER elapsed, do forced shut down.");
                // Do a forceful shutdown.
                mTaskQueue.addTask(
                    new LifeCycleTask(LifeCycleTask.STOP_REQUEST,
                                      LifeCycleTask.DO_FORCED_TERMINATE));

            }
            catch (Throwable t)
            {
                Log.logE("Error in Timer! ", t);
            }
        }
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * For informing the native peer to know the UID of the MIDlet.
     * When a MIDlet is started by using pre-warmed feature, the start message
     * is received by the Java peer of the MIDP runtime. The native peer needs
     * also to know the UID of the MIDlet.
     * @param midletUid The UID of the MIDlet.
     * @param midletSuiteUid The UID of the MIDlet suite.
     * @param starterHandle A pointer to the native runtime peer.
     */
    private native void _setUids(String midletUid, String midletSuiteUid,
                                 int starterHandle);

    /**
     * Changes the process priority back to normal.
     */
    private native void _restoreNormalProcessPriority();


    /**
     * For informing the native peer that the life cycle has started the shut
     * down procedures.
     */
    private static native void _closeInd(int starterHandle);
}
