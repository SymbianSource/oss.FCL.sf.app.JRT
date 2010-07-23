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


package com.nokia.mj.impl.installer.utils;

/**
 * Provides notifications whenever the subscribed property value changes.
 * <br>
 * One PropertyProvider instance can be used to subscribe events
 * for only one property. If events for more properties are needed
 * use separate PropertyProvider instance for each property.
 *
 * @see PropertyListener
 */
public class PropertyProvider
{
    /** Property category. */
    private int iCategory = 0;
    /** Property key. */
    private int iKey = 0;
    /** Property listener. */
    private PropertyListener iListener = null;
    /** Handle to native object. */
    private int iHandle = 0;

    /**
     * Constructor.
     */
    public PropertyProvider()
    {
    }

    /**
     * Subscribe to listen changes of specified property value.
     * One PropertyListener can subscribe to only one property
     * value at a time.
     *
     * @param aCategory property category
     * @param aKey property key
     * @param aListener listener to be notified.
     */
    public void subscribe(int aCategory, int aKey, PropertyListener aListener)
    {
        if (iHandle != 0)
        {
            InstallerException.internalError("PropertyProvider already in use.");
        }
        iCategory = aCategory;
        iKey = aKey;
        iListener = aListener;
        // Start a new thread which blocks until unsubscribe is called.
        final PropertyProvider provider = this;
        new Thread(new Runnable()
        {
            public void run()
            {
                synchronized (provider)
                {
                    iHandle = _subscribe(iCategory, iKey);
                    // Notify subscribe method after subscription
                    // result is available.
                    provider.notify();
                }
                if (iHandle > 0)
                {
                    // Subscription succeeded, start to process events.
                    _processEvents(iHandle, provider);
                }
            }
        }, "PropertyProviderThread").start();
        synchronized (this)
        {
            try
            {
                if (iHandle == 0)
                {
                    // Wait until subscription has been completed.
                    wait();
                }
            }
            catch (InterruptedException ie)
            {
            }
        }
        // Check if subscription failed.
        if (iHandle < 0)
        {
            Log.logError("PropertyProvider.subscribe failed with code " + iHandle);
            iHandle = 0;
        }
    }

    /**
     * Unubscribe from listening changes.
     */
    public void unsubscribe()
    {
        final PropertyProvider provider = this;
        new Thread(new Runnable()
        {
            public void run()
            {
                synchronized (provider)
                {
                    if (iHandle <= 0)
                    {
                        Log.logWarning(
                            "PropertyProvider.unsubscribe: no subscription.");
                        return;
                    }
                    int err = _unsubscribe(iHandle);
                    if (err < 0)
                    {
                        Log.logError(
                            "PropertyProvider.unsubscribe failed with code " +
                            err);
                    }
                    else
                    {
                        iHandle = 0;
                    }
                }
            }
        }, "PropertyProviderUnsubscribeThread").start();
    }

    /**
     * Called from native when value for the subscribed property changes.
     */
    private void valueChanged(int aValue)
    {
        if (iListener != null)
        {
            iListener.valueChanged(iCategory, iKey, aValue);
        }
    }

    /**
     * Subscribe to listen changes of specified property value.
     *
     * @param aCategory property category
     * @param aKey property key
     * @return handle to native side object or Symbian error
     * code (negative number)
     */
    private static native int _subscribe(int aCategory, int aKey);

    /**
     * Starts to process events. This call blocks until unsubscribe is called.
     *
     * @param aHandle handle to native side object
     * @param aProvider PropertyProvider class instance to be notified
     * @return 0 or Symbian error code (negative number)
     */
    private static native int _processEvents(int aHandle, PropertyProvider aProvider);

    /**
     * Unubscribe from listening changes.
     *
     * @param aHandle handle to native side object
     * @return 0 or Symbian error code (negative number)
     */
    private static native int _unsubscribe(int aHandle);
}
