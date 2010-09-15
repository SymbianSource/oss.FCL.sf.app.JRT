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
* Description: Java side Core UI tester.
*
*/
package com.nokia.mj.impl.coreuitest;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.coreui.CoreUi;


/**
 * A gate to CoreUi test implementation.
 */
public class CoreUiImpl extends CoreUi
{
    private static int[] mCounters = new int[6];

    static void resetCounters()
    {
        for (int i = 0; i < mCounters.length; ++i)
        {
            mCounters[i] = 0;
        }
    }

    static void checkCounters(int[] expected)
    {
        if (expected.length != mCounters.length)
        {
            throw new RuntimeException("expected.length != mCounters.length");
        }
        for (int i = 0; i < mCounters.length; ++i)
        {
            if (mCounters[i] != expected[i])
            {
                throw new RuntimeException("Index: " + i + " didn't match. Expected: " +
                                           expected[i] + ", was: " + mCounters[i]);
            }
        }
    }

    protected boolean connectToUiImpl()
    {
        mCounters[0]++;
        return true;
    }

    private static Uid     mUid = null;
    private static boolean mBackGroundStart = false;

    static void createUiImplCheck(Uid uid, boolean backGroundStart)
    {
        if (uid != mUid)
        {
            throw new RuntimeException("createUiImplCheck uid mismatch");
        }
        if (backGroundStart != mBackGroundStart)
        {
            throw new RuntimeException("createUiImplCheck backGroundStart mismatch");
        }
        mUid = null;
        mBackGroundStart = false;
    }

    protected void createUiImpl(Uid uid, boolean backGroundStart)
    {
        mCounters[1]++;
        mUid = uid;
        mBackGroundStart = backGroundStart;
    }

    protected void shutdownRequestImpl()
    {
        mCounters[2]++;
    }

    protected void foregroundRequestImpl()
    {
        mCounters[3]++;
    }

    protected boolean isUiInForegroundImpl()
    {
        mCounters[4]++;
        return true;
    }

    private static boolean mHide = false;

    static void hideApplicationImplCheck(boolean hide)
    {
        if (mHide != hide)
        {
            throw new RuntimeException("hideApplicationImplCheck hide mismatch");
        }
        mHide = false;
    }

    protected void hideApplicationImpl(boolean hide)
    {
        mCounters[5]++;
        mHide = hide;
    }
}
