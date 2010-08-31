/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all AMMS controls.
*
*/

package com.nokia.amms.control;

import javax.microedition.media.Control;
import com.nokia.amms.ModuleBase;
import com.nokia.mj.impl.utils.Logger;


/**
 * Base class for all AMMS controls.
 */
public class ControlImpl implements Control
{
    protected int iControlHandle;
    protected int iEventSource;
    // Module that owns this control. Storing the reference to the module
    // will prevent the garbage collector from deleting the module if some
    // controls are still used.
    protected ModuleBase iModule;

    protected static final int NO_ERROR = 0;
    protected static final int NOT_READY = -18;
    protected static final int NOT_FOUND = -1;
    protected static final int NOT_SUPPORTED = -5;

    /**
     * Constructor
     */
    protected ControlImpl()
    {
    }

    /**
     * Initializes control with event source and native implementation handles.
     * @param aEventSource Handle to native event source.
     * @param aControl Handle to native control implementation.
     * @param aModule Module that owns this control.
     */
    public void initControl(int aEventSource,
                            int aControlHandle,
                            ModuleBase aModule)
    {
        iEventSource = aEventSource;
        iControlHandle = aControlHandle;
        iModule = aModule;

        Logger.LOG(Logger.EJavaMMAPI,
                   Logger.EInfo,
                   "created: " + toString() + " handle = " + aControlHandle);
    }

    /**
     * Invalidates the control. Using this control after invalidating
     * throws a RuntimeException.
     */
    public void invalidateControl()
    {
        iControlHandle = 0;
    }


    public void checkValid()
    {
        if ((iEventSource == 0) || (iControlHandle == 0))
        {
            throw new IllegalStateException("Control creation failed");
        }
    }
}
