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

package com.nokia.mj.impl.tckrunner;

/**
 * TCK Runner state change notification interface
 */
public interface TckRunnerStateListener
{

    /**
     * TCK Runner states
     */
    public static final int STATE_NOT_RUNNING                  = 0;
    public static final int STATE_INSTALL_APPLICATION          = 1;
    public static final int STATE_UNINSTALL_APPLICATION        = 2;
    public static final int STATE_RUN_APPLICATION              = 3;

    /**
     * Called when TCK Runner state changes
     */
    public void stateChanged(int aState);

    /**
     * Called if error occurs in install/uninstall/run states
     */
    public void operationFailed(int aState, int aRetryCount);
}