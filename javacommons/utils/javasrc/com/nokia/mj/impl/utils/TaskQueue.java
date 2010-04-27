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


package com.nokia.mj.impl.utils;

public final class TaskQueue
{
    Task   mFirst = null;
    Task   mLast  = null;

    boolean mIsActive = true;

    public synchronized Task getTask()
    {
        while (mIsActive)
        {
            try
            {
                Task task = getTaskImpl();
                if (task == null)
                {
                    wait();
                    task = getTaskImpl();
                }
                return task;
            }
            catch (InterruptedException ex)
            {
            }
        }
        return null;
    }

    private Task getTaskImpl()
    {
        Task task = mFirst;
        if (task != null)
        {
            if (mFirst == mLast)
            {
                mFirst = null;
                mLast = null;
            }
            else
            {
                mFirst = task.getNext();
            }
        }
        return task;
    }

    public synchronized void forcedTerminate()
    {
        mIsActive = false;
        notify();
    }

    public synchronized int addTask(Task task)
    {
        if (mIsActive)
        {
            if (mLast != null)
            {
                mLast.setNext(task);
                mLast = task;
            }
            else
            {
                // This becomes the only task in the queue.
                mFirst = task;
                mLast = task;
            }
            notify();
            return 0;
        }
        return -1;
    }
}
