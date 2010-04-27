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

import com.nokia.mj.impl.rt.support.*;

public final class Test2
{
    private Finalizer mFinalizer;

    static
    {
        if (!TestMain.useAnonymousApproach)
        {
            JvmInternal.enableFinalization(Test2.class);
        }
    }

    public Test2()
    {
        FinalizerStatistics.debugFinalizeCreated(getClass().getName());
        if (TestMain.useAnonymousApproach)
        {
            mFinalizer = new Finalizer()
            {
                public void finalizeImpl()
                {
                    doFinalize();
                }
            };
        }
    }

    public void finalize()
    {
        doFinalize();
    }
    void doFinalize()
    {
        if (mFinalizer != null)
        {
            mFinalizer = null;
            FinalizerStatistics.debugFinalize(getClass().getName());
        }
    }
}

