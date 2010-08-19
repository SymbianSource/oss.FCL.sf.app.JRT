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
 * Description: Receives callbacks from the native side.
 *
 */

// PACKAGE
package com.nokia.mid.ui.impl;

// IMPORTS
import com.nokia.mid.ui.TextEditorListener;
import com.nokia.mid.ui.TextEditor;
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.support.Finalizer;


// CLASS DESCRIPTION
/**
 * Receives callbacks from the native side.
 */
public class TextEditorListenerImpl
{
    // Data
    private final TextEditor iEditor;
    private final TextEditorListener iListener;
    private final Object iToolkit;
    private final ToolkitInvoker iToolkitInvoker;
    private int iHandle;
    private int iActions;
    private Finalizer mFinalizer;

    /**
     * Constructor.
     *
     * @param aEditor
     *            The editor to which this listener is associated to. ' @param
     *            aListener The listener to which this implementation is
     *            associated to. Actions are delivered to this object.
     */
    public TextEditorListenerImpl(
        TextEditor aEditor,
        TextEditorListener aListener)
    {
        iEditor = aEditor;
        iListener = aListener;

        // Toolkit invoker is needed for accessing javax.microedition.lcdui
        // package
        iToolkitInvoker = ToolkitInvoker.getToolkitInvoker();
        iToolkit = iToolkitInvoker.getToolkit();

        int handle = 0;
        synchronized (iToolkit)
        {
            // Create native peer object for this Java object.
            handle =
                _createNativePeer(iToolkitInvoker.toolkitGetHandle(iToolkit));
        }

        // Check if construction failed and throw out of memory error.
        if (handle <= NativeError.KErrNone)
        {
            throw new OutOfMemoryError();
        }

        // Construction was successful. Store handle and register for
        // finalization.
        iHandle = handle;
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                registeredFinalize();
            }
        };
    }

    /**
     * This method is called when an action occurs in the native side.
     *
     * The actions are simply delegated to the associated
     * <code>TextEditorListener</code> object which provids the actual
     * functionality.
     *
     * @param aActions
     *            The actions that occured.
     * @return The status from the associated listener.
     */
    public void inputAction(int aActions)
    {
        // JDEBUG( "TextEditorListenerImpl.inputAction() + aActions=" + aActions
        // );

        try
        {
            iListener.inputAction(iEditor, aActions);
        }
        catch (Throwable t)
        {
            // JDEBUG(
            // "TextEditorListenerImpl.inputAction(), exception in callback" );

            // Ignored intentionally.

            t.printStackTrace();
        }

        // JDEBUG( "TextEditorListenerImpl.inputAction() -" );
    }

    /**
     * Returns the handle to the native side peer object.
     *
     * @return The handle to the native side peer object.
     */
    public int getHandle()
    {
        return iHandle;
    }

    /**
     * Disposes the Landmark native peer objecct, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in the
     * first place.
     */
    final void registeredFinalize()
    {
        // JDEBUG( "TextEditorListenerImpl.registeredFinalize() +" );
        if (mFinalizer!=null)
        {
            synchronized (iToolkit)
            {
                if (iHandle > 0)
                {
                    iToolkitInvoker.toolkitDisposeObject(iToolkit,iHandle);
                    iHandle = 0;
                }
            }
            mFinalizer = null;
        }

        // JDEBUG( "TextEditorListenerImpl.registeredFinalize() -" );
    }

    // Native methods

    /**
     * Creates the native side peer object for this object.
     *
     * @param aToolkitHandle
     *            A handle to the LCDUI toolkit.
     * @return A handle to the the native side peer object or a system-wide
     *         error code.
     */
    private native int _createNativePeer(int aToolkitHandle);

    /**
     * Disposes the native side peer object.
     *
     * @param aToolkitHandle
     *            A handle to the LCDUI toolkit.
     * @param aNativePeerHandle
     *            A handle to the native side peer object.
     */
    private native void _dispose(int aToolkitHandle, int aNativePeerHandle);
}

// End of file
