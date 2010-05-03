/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.legacy;

import java.util.Hashtable;
import javax.microedition.midlet.MIDlet;


/**
 * A class for porting legacy JSRs to new runtime.
 */
class LegacySymbian extends LegacySupport
{

    /**
     * @see com.nokia.mj.impl.rt.legacy.LegacySupport#init(java.util.Hashtable, boolean)
     */
    protected void initImpl(Hashtable args, boolean startInBackGround)
    {
        ToolkitInvoker.getToolkitInvoker().
        createToolkit(args, startInBackGround);
        EventProcessor.startEventProcessing();
    }

    /**
     * @see com.nokia.mj.impl.rt.legacy.LegacySupport#setMidlet(java.lang.Object)
     */
    protected void setMidletImpl(Object midlet)
    {
        LegacyRtPort.setMidlet((MIDlet)midlet);
    }

    /**
     * @see com.nokia.mj.impl.rt.legacy.LegacySupport#close(java.lang.Object)
     */
    protected void closeImpl()
    {
        EventProcessor.exitThreads();
    }
}
