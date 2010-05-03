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
* Description:
*
*/

package javax.microedition.lcdui;

import java.io.InputStream;
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.ToolkitObserver;
import com.nokia.mj.impl.rt.support.Finalizer;

class ImageLoader
{
    final Toolkit iToolkit;
    private ToolkitObserver iObserver;
    private int iHandle;
    private AsyncCall iStatus;
    private Finalizer mFinalizer;

    ImageLoader(Toolkit aToolkit)
    {
        iToolkit = aToolkit;
        iStatus  = new AsyncCall();
        //
        // Initialize cleanup: Register this object with the finalizer and with
        // the toolkit.
        //
        iObserver = new ToolkitObserver()
        {
            public void destroyNotify()
            {
                dispose();
            }
        };
        iToolkit.addObserver(iObserver);
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

        synchronized (iToolkit)
        {
            iHandle = NativeError.check(_create(iToolkit.getEventServerHandle(), iToolkit.getHandle()));
        }
    }

    synchronized boolean append(byte[] aByteArray, int aOffset, int aCount)
    {
        if (iHandle == 0)
            return false;

        synchronized (this)
        {
            NativeError.check(_append(iHandle, aByteArray, aOffset, aCount));
        }
        return true;
    }

    /**
     * Blocking decode, streams encoded data to native decoder.
     */
    void decode()
    {
        synchronized (iStatus)
        {
            synchronized (this)
            {
                synchronized (iToolkit)
                {
                    NativeError.check(_decode(iHandle));
                }
                iStatus.request();
            }
            NativeError.check(iStatus.waitForRequest());
        }
    }

    /**
     * WARNING - converters may be disposed on toolkit shutdown.
     * Handle could become invalid unless toolkit is locked down
     * over duration of use.
     */
    int getHandle()
    {
        return iHandle;
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    private void registeredFinalize()
    {
        dispose();
    }

    synchronized void dispose()
    {

        if (iStatus.isPending())
        {
            iStatus.complete(-1);   // unblock waiting thread(s)
        }
        if (iObserver != null)
        {
            iToolkit.removeObserver(iObserver); // never throws
            iObserver = null;
        }
        final int handle = iHandle;
        iHandle = 0;
        if (0 != handle)
        {
            _destroy(handle);
        }
    }

    /**
     * upcall.
     */
    private void signal(int aError)
    {
        iStatus.complete(aError);
    }

    private native int  _create(int aEventServer, int aToolkitHandle);
    private native void _destroy(int aHandle);
    private native int  _append(int aHandle, byte[] aArray, int aOffset, int aCount);
    private native int  _decode(int aHandle);
}
