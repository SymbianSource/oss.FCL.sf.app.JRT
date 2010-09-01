/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is invoked when MMA event source is created.
*
*/

package com.nokia.amms;

import com.nokia.microedition.media.ManagerImpl;
import com.nokia.microedition.media.MMAInvokeListener;

/**
 * This class will be created and invoked when MMA event source is created.
 */
public class MMAInvokeListenerImpl implements MMAInvokeListener
{
    /**
     * Public empty constructor to used from MMA.
     */
    public MMAInvokeListenerImpl()
    {
    }

    /**
     * From MMAInvokeListener. Invokes GlobalManager and adds AMMSPlugin to
     * the MMA Manager.
     * @param aEventSourceHandle Handle to native MMA event source.
     */
    public void notifyInvoke(int aEventSourceHandle)
    {
        // Create GlobalManagerImpl static instance.
        GlobalManagerImpl.invoke(aEventSourceHandle);

        // Add AMMSPlugin to MMA Manager. The plugin is called every time
        // a player is created.
        ManagerImpl.getInstance().addPlugIn(new AMMSPlugin());
    }
}
