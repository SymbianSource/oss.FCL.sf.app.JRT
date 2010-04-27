/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


// PACKAGE
package com.nokia.microedition.volumekeys;

// IMPORTS
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.microedition.volumekeys.LCDUIForegroundListener;

// for xm-radio fix
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;

// CLASS DESCRIPTION
/**
 * Provides information about the state of the MIDlet
 */
public class ForegroundListener
{
    private final LCDUIForegroundListener iLCDUIForegroundListener;
    private final int iForegroundHandle;
    private final int iEventSourceHandle;


    /*
     * Constructor. Creates Foreground listener
     * The listener is added to Event source native side
     */
    public ForegroundListener(int aEventSourceHandle)
    {
        iEventSourceHandle = aEventSourceHandle;
        int toolkitHandle = 0;
        if (!_isESWT())
        {
            // LCDUI midlet running
            // for xm-radio fix
            ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
            toolkitHandle = invoker.toolkitGetHandle(invoker.getToolkit());
        }
        else
        {
            // eSWT midlet running
            // Foreground status of the eSWT midlet is always set to 1 so midlet recieves volume key events even in background.
            // This is a minor problem to be fixed in future
        }

        iForegroundHandle = _initialize(aEventSourceHandle, this, toolkitHandle);

        if (iForegroundHandle < NativeError.KErrNone)
        {
            throw new OutOfMemoryError("Foreground initialization failed.");
        }
        iLCDUIForegroundListener = new LCDUIForegroundListener();
    }

    /*
     * Called from native side to check is midlet foreground or not
     */
    private synchronized void update()
    {
        boolean isFg = iLCDUIForegroundListener.isForeground();
        _setForeground(iEventSourceHandle, iForegroundHandle, isFg);
    }

    private native int _initialize(int aEventSourceHandle,
                                   Object aForegroundListener,
                                   int atoolkitHandle); // xm-radio fix

    private native void _setForeground(int aEventSourceHandle,
                                       int aForegroundHandle,
                                       boolean aForeground);

    private native boolean _isESWT();

}

// End of File
