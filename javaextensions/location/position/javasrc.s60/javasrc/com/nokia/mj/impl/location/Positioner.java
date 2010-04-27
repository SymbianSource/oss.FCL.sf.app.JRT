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
* Description:  Positioning module implementation
 *
*/


// PACKAGE
package com.nokia.mj.impl.location;

// IMPORTS
import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.location.LocationException;
import com.nokia.mj.impl.location.NativeError;
// CLASS DESCRIPTION
/**
 * Handles Java side getLocation() requests and delegates them
 * to the correct native side positioner.
 */
public final class Positioner
{
    boolean iCancelled = false;

    // Handle to this positioner's native side peer
    int iPositionerHandle = 0;

    // Error strings
    private static final String GETLOCATION_ERROR_STRING = "Unable to obtain location";

    private static final Blocker iLock = new Blocker();

    private static final Vector iPositioners = new Vector();

    // The location provider which uses this positioner
    private final LocationProviderImpl iLocationProvider;

    /**
     * Hidden constructor. Creates an object from this class
     * @param aLocationProvider The Location Provider which uses this positioner
     */
    private Positioner(LocationProviderImpl aLocationProvider)
    {
        iLocationProvider = aLocationProvider;
    }

    /**
     * Gets a new positioner which can be used to obtain location request
     * A new positioner is created because different thread may request
     * location at the same time so one provider cannot handle all request
     * in that case
     *
     * @param aLocationProvider The Location Provider which uses this positioner
     * @return New positioner
     */
    static Positioner getPositioner(LocationProviderImpl aLocationProvider)
    {
        Positioner pos = new Positioner(aLocationProvider);
        iPositioners.addElement(pos);
        return pos;
    }

    /**
     * Resets all positioner and cancels all outstanding requests
     */
    static void resetAll()
    {
        synchronized (iPositioners)
        {
            Enumeration e = iPositioners.elements();
            while (e.hasMoreElements())
            {
                Positioner p = (Positioner) e.nextElement();
                p.cancel();
            }
        }
    }

    /**
     * Tries to get a location fix from the native positioner.
     * If the positioner has not been created or it has been released
     * due to an error within the Location Framework this methods
     * creates a new native side provider and start acquiring
     * a new location fix. The provider can be deleted due to many
     * different reasons. Most common might be that system runs of of
     * resources when acquiring location fix but it is still possible
     * the retrieve a location fix with the next request
     *
     * @param aTimeoutSecond Time out in seconds after which the locatoin
     *        request will be timed out
     * @return Error code if the location request was not successful.
     *         NativeError.KErrNone if the request was successful and
     *         correct location fix has been retrieved
     */
    int getLocation(int aTimeoutSeconds) throws LocationException
    {
        if (iCancelled)
        {
            return NativeError.KErrCancel;
        }

        int ret = 0;

        synchronized (iLock)
        {
            int functionSourceHandle = iLocationProvider.getFunctionSourceHandle();
            int providerHandle = iLocationProvider.getProviderHandle();
            // The positioner needs to be created if it has been released
            // previously for example due to location framework error. The case
            // might be for example that KErrTimeOut or some other error has
            // occured which does not prevent from getting another location fix.
            if (iPositionerHandle == 0)
            {
                int handle = _createPositioner(functionSourceHandle,
                                               providerHandle);

                if (handle < NativeError.KErrNone)
                {
                    throw new LocationException(GETLOCATION_ERROR_STRING);
                }
                // Positioner created successfully -> set handle
                iPositionerHandle = handle;
            }

            // Request a location fix
            ret = _getLocation(functionSourceHandle, iPositionerHandle,
                               aTimeoutSeconds);

            // Acquiring location fix has started successfully. Start
            // waiting for notificiation from the native side
            if (ret == NativeError.KErrNone)
            {
                try
                {
                    iLock.wait();
                    ret = iLock.getresult();
                }
                catch (java.lang.InterruptedException e)
                {
                }
            }
        }

        // Release position if error occured. Do not release if positive
        // error code is returned. The case might be that the quality of the
        // position is poor or the update is partial.
        if (ret < NativeError.KErrNone)
        {
            release();
        }
        return ret;
    }

    /**
     * Cancels the current location request
     */
    private void cancel()
    {
        iCancelled = true;
        _cancel(iLocationProvider.getFunctionSourceHandle(), iPositionerHandle);
    }

    void syncComplete(int aError)
    {
        synchronized (iLock)
        {
            iLock.setresult(aError);
            iLock.notify();
        }
    }

    /**
     * Releases the positioner (i.e deletes native side peer)
     */
    void release()
    {
        if (iPositioners.removeElement(this) && iPositionerHandle > 0)
        {
            _dispose(iLocationProvider.getFunctionSourceHandle(),
                     iPositionerHandle);
            iPositionerHandle = 0;
        }
    }

    private native int _getLocation(int aFunctionSource, int aPositionerHandle,
                                    int aTimeout);

    private native int _createPositioner(int aFunctionSource, int aProviderHandle);

    private native void _cancel(int aFunctionSource, int aPositonerHandle);

    private native void _dispose(int aFunctionSource, int aPositionerHandle);

}

// End of file
