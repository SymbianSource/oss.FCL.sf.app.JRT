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

import com.nokia.mj.impl.utils.Task;

/**
 * A class to be used to state change request to the state machine of the
 * MIDP runtime life cycle.
 */

class LifeCycleTask extends Task
{
    /**
     * The main tasks.
     */
    static final int START_REQUEST      = 1;
    static final int STOP_REQUEST       = 2;
    static final int MIDLET_RUNNING_IND = 3;
    static final int STOPPED_IND        = 4;
    static final int PAUSE_REQUEST      = 5;
    static final int PAUSE_APP_REQUEST  = 6;
    static final int RESUME_REQUEST     = 7;
    static final int STARTUP_FAILURE    = 8;
    static final int UNCAUGHT_EXCEPTION = 9;
    static final int PAUSE_FAILURE      = 10;

    static final int NO_SUB_TASK        = -1;

    /**
     * The sub tasks of the main task STOP_REQUEST.
     */
    static final int APP_STOP_REQUEST    = 1;
    static final int SYSTEM_STOP_REQUEST = 2;
    static final int DO_FORCED_TERMINATE = 3;

    /**
     * The sub tasks of the main task START_REQUEST.
     */
    static final int NORMAL_START = 1;
    static final int PRE_WARM_START = 2;

    /**
     * The main task.
     */
    private int mMainTask;

    /**
     * The sub task.
     */
    private int mSubTask;


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * No default constructor allowed to LifeCycleTask.
     */
    private LifeCycleTask()
    {
    }

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * The constructor of LifeCycleTask when only main task is enough.
     * @param mainTask One of the main tasks listed above.
     */
    LifeCycleTask(int mainTask)
    {
        this(mainTask, NO_SUB_TASK);
    }

    /**
     * The constructor of LifeCycleTask when both main task and sub task
     * are required.
     * @param mainTask One of the main tasks listed above.
     * @param subTask One of the sub tasks listed above.
     */
    LifeCycleTask(int mainTask, int subTask)
    {
        if (Log.mOn) Log.logI("TASK: " + mainTask + ", sub: " + subTask);
        mMainTask = mainTask;
        mSubTask = subTask;
    }

    /**
     * A getter for the main task
     * @return the main task.
     */
    int getMainTask()
    {
        return mMainTask;
    }

    /**
     * A getter for the sub task
     * @return the sub task.
     */
    int getSubTask()
    {
        return mSubTask;
    }
}
