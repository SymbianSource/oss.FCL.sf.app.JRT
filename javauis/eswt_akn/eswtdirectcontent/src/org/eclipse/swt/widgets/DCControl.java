/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


package org.eclipse.swt.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;

/**
 * eSWT direct content GUI class
 */
public class DCControl extends Control implements Runnable
{
    // native dll name containing jni implementations
    private static final String DC_CONTROL_LIBRARY =
        "eswtdirectcontent";

    // native CSwtDcControl
    private int iDCControlHandle = 0;

    // native handle to MMMADirectContent
    private int iContentHandle = 0;

    // used to avoid too many repain requests
    private boolean iRepaintPending = false;

    private ControlListener iTopShellListener = null;
    private Shell iTopShell = null;

    private boolean iFullScreen = false;

    static
    {
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary(
            DC_CONTROL_LIBRARY);
    }

    /**
     * Runnable to set new size for Control
     */
    private class DCInvalidate implements Runnable
    {
        Control iControl;
        Point iSize;

        /**
         * Constructor
         * @param aControl control to invalidate
         * @param aSize new size for control
         */
        public DCInvalidate(Control aControl, Point aSize)
        {
            iControl = aControl;
            iSize = aSize;
        }

        /**
         * From Runnable
         */
        public void run()
        {
            iControl.setSize(iSize);
        }
    }

    /**
     * Default constructor
     */
    public DCControl()
    {

    }

    /**
     * This method cannot be used to parent, meaning that DCControl is not
     * reparentable.
     * @see Control
     */
    public boolean setParent(Composite aParent)
    {
        if (aParent == null)
        {
            internal_error(SWT.ERROR_NULL_ARGUMENT);
        }
        if (aParent.isDisposed())
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        if (this.parent == aParent)
        {
            return true;
        }

        if (iContentHandle == 0)
        {
            // setContentHandle must have been called
            throw new IllegalStateException("Control is not initialized.");
        }

        // needed in checkWidget
        display = aParent.display;

        // This does thread checking
        checkWidget();

        parent = aParent;
        parent.addControl(this);

        // Create native object
        iDCControlHandle = _createDCControl(aParent.compositeHandle,
                                            iContentHandle);
        handle = _getControlHandle(iDCControlHandle);

        // Find top shell and listen to resize events (means actually screen rotation)
        iTopShellListener = new ControlListener()
        {
            public void controlMoved(ControlEvent ev)
            {
            }
            public void controlResized(ControlEvent ev)
            {
                // If we're in fullscreen mode this will cause the
                // content to be resized to fill the whole screen after
                // screen size/orientation has been changed.
                if (iFullScreen)
                    _setFullScreen(iDCControlHandle, iFullScreen);
            }
        };

        Shell iTopShellTemp = parent.getShell();
        while (iTopShellTemp.getParent() != null &&
                iTopShellTemp.getParent().getShell() != null)
        {
            iTopShellTemp = iTopShellTemp.getParent().getShell();
        }
        iTopShell = iTopShellTemp;
        iTopShell.addControlListener(iTopShellListener);

        return true;
    }

    /**
     * This must be called before native instance can be created with
     * setParent method.
     * @param MMMADirectControl native handle
     */
    public void setContentHandle(int aContentHandle)
    {
        iContentHandle = aContentHandle;
    }

    /**
     * called from internal_createWidget
     * @see Control
     */
    protected void internal_createHandle()
    {
    }

    /**
     * Called from native code.
     */
    protected void setFullScreen(boolean aFullScreen)
    {
        iFullScreen = aFullScreen;
        _setFullScreen(iDCControlHandle, aFullScreen);
    }

    /**
     * Called from native code.
     */
    protected void invalidate(int aWidth, int aHeight)
    {
        display.asyncExec(
            new DCInvalidate(this, new Point(aWidth, aHeight)));
    }

    /**
     * Called from native code.
     */
    protected void repaint()
    {
        if (!iRepaintPending)
        {
            iRepaintPending = true;
            display.asyncExec(this);
        }
    }

    /**
     * From Runnable, called from repaint() method
     */
    public void run()
    {
        if (!isDisposed())
        {
            _drawNow(iDCControlHandle);
        }
        iRepaintPending = false;
    }

    /**
     * Release resources
     */
    protected void internal_releaseResources()
    {
        iTopShell.removeControlListener(iTopShellListener);
        iTopShellListener = null;
        iTopShell = null;
        super.internal_releaseResources();
    }

    /**
     * Creates native instance
     * @param aParentHandle MSwtComposite native handle
     * @param aContentHandle MMMADirectContent native handle
     * @return CSwtDCControl native handle
     */
    private native int _createDCControl(int aParentHandle,
                                        int aContentHandle);

    /**
     * @param CSwtDCControl handle
     * @return MSwtControl handle
     */
    private native int _getControlHandle(int aDCControlHandle);

    /**
     * Calls directly DrawNow to avoid flickering
     * @param CSwtDCControl handle
     */
    private native void _drawNow(int aDCControlHandle);

    private native void _setFullScreen(int aDCControlHandle,
                                       boolean aFullScreen);
}
