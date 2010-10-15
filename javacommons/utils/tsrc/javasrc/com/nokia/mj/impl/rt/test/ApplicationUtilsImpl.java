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

import java.security.Permission;
import java.security.AccessControlException;

import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

public class ApplicationUtilsImpl extends ApplicationUtils
{

    public static final int NOTIFY_CALL = 0;
    public static final int UI_DISPOSED_CALL = 1;
    public static final int PAUSE_CALL = 2;
    public static final int RESUME_CALL = 3;
    public static final int PERMISSION_CALL1 = 4;
    public static final int PERMISSION_CALL2 = 5;

    public static int[] mCalledArr = new int[6];

    static
    {
        clear();
    }

    public static Permission mPermission = null;
    public static Uid mUid = null;

    public static void clear()
    {
        for (int i = 0; i < mCalledArr.length; ++i)
        {
            mCalledArr[i] = 0;
        }
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

    public static void doShutdownImpl()
    {
//        sInstance.doShutdown();
        ((ApplicationUtilsImpl)sInstance).doShutdown();
    }

    public void notifyExitCmd()
    {
        mCalledArr[NOTIFY_CALL]++;
    }

    public void uiDisposed()
    {
        mCalledArr[UI_DISPOSED_CALL]++;
    }

    public void pauseApplication()
    {
        mCalledArr[PAUSE_CALL]++;
    }

    public void resumeApplication()
    {
        mCalledArr[RESUME_CALL]++;
    }

    public void checkPermission(Permission p)
    throws AccessControlException,
        NullPointerException
    {
        mPermission = p;
        mCalledArr[PERMISSION_CALL1]++;
    }

    public void checkPermission(Uid appUid,Permission p)
    throws AccessControlException,
        NullPointerException
    {
        mPermission = p;
        mUid = appUid;
        mCalledArr[PERMISSION_CALL2]++;
    }

    public static void releaseWaiterImpl(boolean doStart)
    {
        ((ApplicationUtilsImpl)sInstance).releaseStartWaiter(doStart);
    }

}
