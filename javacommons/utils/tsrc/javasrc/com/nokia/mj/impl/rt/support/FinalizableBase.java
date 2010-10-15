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

package com.nokia.mj.impl.rt.support;

abstract class FinalizableBase
{
    private Finalizer mFinalizer;

    protected FinalizableBase()
    {
        FinalizerStatistics.finalizableCreated(getClass().getName());
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    void doFinalize()
    {
        if (mFinalizer != null)
        {
            mFinalizer = null;
            FinalizerStatistics.classFinalized(getClass().getName());
        }
    }
}

