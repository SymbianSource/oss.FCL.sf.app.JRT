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
* Description:  LocationProvider implementation
 *
*/


// PACKAGE
package com.nokia.mj.impl.location;

// IMPORTS
import javax.microedition.location.*;
import com.nokia.mj.impl.location.NativeError;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.location.LocationPermission;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;

// CLASS DESCRIPTION
/**
 * LocationProvider implementation class. Delegates most of the Location
 * API function calls to the native side and provides native positioning
 * module functionalities.
 */
public final class LocationProviderImpl extends LocationProvider
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javalocation");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaLocation,
                        "Unable to load javalocation dll.");
        }

        try
        {
            Jvm.loadSystemLibrary("lbt.dll");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaLocation,
                        "Unable to load lbt dll.");
        }
    }

    private static final int ADDRESS_REQUIRED = 1 << 0;

    private static final int COST_ALLOWED = 1 << 1;

    private static final int ALTITUDE_REQUIRED = 1 << 2;

    private static final int SPEED_AND_COURSE_REQUIRED = 1 << 3;

    private static final int NUM_ADDRESS_FIELDS = 17;

    // Handle to the native function source
    private final int iFunctionServerHandle;

    private Finalizer iFinalizer;

    private final int iHandle;

    private int iTrackingPositionerHandle;

    private LocationListener iListener = null;

    private static final Blocker iLock = new Blocker();

    // Declare iState volatile to ensure correct value is returned in getState
    private volatile int iState = AVAILABLE;

    private int iLocationMethod;

    private int iRequired = 0;

    private int iTimeout;

    private static final String LOCATION_REQUEST_ERROR_STRING
    = "Location request timed out";

    private static final String ALL_POSITIONING_METHODS_DISABLED =
        "All positioning methods are disabled";

    private static final String OBTAIN_LOCATION_INTERRUPTED_ERROR =
        "Obtaining location was interrupted";

    private static final String INVALID_LOCATION_LISTENER_ARGUMENT_ERROR =
        "Invalid arguments for interval, timeout or maxAge: ";

    private static final String INVALID_TIMEOUT_ARGUMENT_ERROR =
        "Timeout value is invalid";

    private static final String OBTAIN_LOCATION_PROVIDER_ERROR =
        "Unable to obtain location provider: ";

    private static final String SET_LOCATION_LISTENER_ERROR =
        "Unable to set location listener:";

    public LocationProviderImpl()
    {

        iFinalizer = registerFinalize();

        iFunctionServerHandle = _createFunctionSource();

        iHandle = _createProvider(iFunctionServerHandle);

    }

    public boolean selectModule(Criteria aCr) throws LocationException
    {
        int hAcc = Criteria.NO_REQUIREMENT;
        int vAcc = Criteria.NO_REQUIREMENT;
        int respTime = Criteria.NO_REQUIREMENT;
        int power = Criteria.NO_REQUIREMENT;
        int required = COST_ALLOWED;

        if (aCr != null)
        {
            hAcc = aCr.getHorizontalAccuracy();
            vAcc = aCr.getVerticalAccuracy();
            respTime = aCr.getPreferredResponseTime();
            power = aCr.getPreferredPowerConsumption();

            if (aCr.isAddressInfoRequired())
            {
                required |= ADDRESS_REQUIRED;
            }
            if (!aCr.isAllowedToCost())
            {
                required ^= COST_ALLOWED; // Clear cost bit
            }
            if (aCr.isAltitudeRequired())
            {
                required |= ALTITUDE_REQUIRED;
            }
            if (aCr.isSpeedAndCourseRequired())
            {
                required |= SPEED_AND_COURSE_REQUIRED;
            }
        }

        iRequired = required;

        int[] retArgs = new int[2];
        int ret = _selectModule(iFunctionServerHandle, iHandle, hAcc, vAcc,
                                respTime, power, required, retArgs);

        if (ret < 0)
        {
            if (ret == NativeError.KErrNotFound)
            {
                throw new LocationException(ALL_POSITIONING_METHODS_DISABLED);
            }
            else if (ret == NativeError.KErrNotSupported)
            {
                return false; // Criteria too restrictive
            }
            else
            {
                NativeError.check(ret, OBTAIN_LOCATION_PROVIDER_ERROR);
            }
        }

        iTrackingPositionerHandle = ret;
        iLocationMethod = retArgs[0];
        int timeout = retArgs[1];
        // Default timeout is never less than 15 seconds
        iTimeout = Math.max(timeout, 15);
        return true; // Module was selected ok
    }

    public int getState()
    {
        return iState;
    }

    public void reset()
    {
        Positioner.resetAll();
    }

    public Location getLocation(int aTimeout) throws LocationException,
                InterruptedException
    {

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("location://*",
                "position");
        appUtils.checkPermission(permission);

        if (aTimeout <= 0 && aTimeout != -1)
        {
            throw new IllegalArgumentException(INVALID_TIMEOUT_ARGUMENT_ERROR);
        }

        Positioner pos = Positioner.getPositioner(this);
        int timeout = aTimeout == -1 ? iTimeout : aTimeout;

        int currentTimeout = timeout;
        long startTime = System.currentTimeMillis();

        while (true)
        {
            if (iState != OUT_OF_SERVICE)
            {
                int ret = pos.getLocation(currentTimeout);

                if (ret == 0)
                {
                    break;
                }

                if (pos.iCancelled)
                {
                    throw new
                    InterruptedException(OBTAIN_LOCATION_INTERRUPTED_ERROR);
                }

                // Check if time has run out
                long elapsedTime = System.currentTimeMillis() - startTime;
                currentTimeout = timeout - (int)(elapsedTime / 1000L);

                if (currentTimeout > 0)
                {
                    continue;
                }
            }

            // GetLocation failed due to time out. Native positioner
            // needs to be released if not released before this
            pos.release();
            // Throw exception since location request failed
            throw new LocationException(LOCATION_REQUEST_ERROR_STRING);
        }

        LocationData loc = new LocationData(iLocationMethod);
        setLocationData(loc, pos.iPositionerHandle);
        pos.release();
        return loc;
    }

    public void setLocationListener(LocationListener aListener, int aInterval,
                                    int aTimeout, int aMaxAge)
    {

        if (aListener == null || aInterval == 0)
        {
            // Only return state updates if interval is zero

            // Cancel existing listener
            if (iListener != null)
            {
                _stopTracking(iFunctionServerHandle, iHandle);
            }
            iListener = aListener;
        }
        else
        {
            // Check that arguments are valid
            if (aInterval < -1
                    || (aInterval > 0 && (aTimeout > aInterval
                                          || aMaxAge > aInterval
                                          || (aTimeout < 1 && aTimeout != -1) || (aMaxAge < 1 && aMaxAge != -1))))
            {
                throw new IllegalArgumentException(
                    INVALID_LOCATION_LISTENER_ARGUMENT_ERROR +
                    aInterval + ", "+ aTimeout + "or " + aMaxAge);
            }

            // Check if access is allowed
            // Note that access is always allowed for aListener == null
            // or aInterval == 0.
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            LocationPermission permission = new LocationPermission(
                "location://*", "position");
            appUtils.checkPermission(permission);

            // Cancel existing listener
            if (iListener != null)
            {
                _stopTracking(iFunctionServerHandle, iHandle);
            }

            iListener = aListener;

            int err = _startTracking(iFunctionServerHandle, iHandle, aInterval,
                                     aTimeout, aMaxAge);
            NativeError.check(err, SET_LOCATION_LISTENER_ERROR);
        }
    }

    public static Location getLastKnownLocation()
    {
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("location://*",
                "position");
        appUtils.checkPermission(permission);

        double[] latlong = new double[2];
        float[] altHaccVacc = new float[3];
        long[] timestamp = new long[1];
        int ret = _getLastKnownLocation(latlong, altHaccVacc, timestamp);

        if (ret == 0)
        {
            LocationData loc = new LocationData(0);
            QualifiedCoordinates qc = new QualifiedCoordinates(latlong[0],
                    latlong[1], altHaccVacc[0], altHaccVacc[1], altHaccVacc[2]);

            loc.setLocationData(qc, timestamp[0], null);
            return loc;
        }
        else
        {
            return null;
        }
    }

    /**
     * Registers with Finalizer to call a method when the object gets collected
     * by GC
     *
     * @return Finalizer object that will be notified when GC happens
     */
    private Finalizer registerFinalize()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                registeredFinalize();
            }
        };
    }

    void complete(int aError)
    {
        if (iListener == null)
        {
            return;
        }

        LocationData loc = new LocationData(iLocationMethod);

        if (aError == 0)
        {
            setLocationData(loc, iTrackingPositionerHandle);
        }

        iListener.locationUpdated(this, loc);
    }

    void syncComplete(int aError)
    {
        synchronized (iLock)
        {
            iLock.setresult(aError);
            iLock.notify();
        }
    }

    void stateChange(int aNewState)
    {
        iState = aNewState;

        if (iListener == null)
        {
            return;
        }

        iListener.providerStateChanged(this, aNewState);
    }

    private void setLocationData(LocationData aLocation, int aPositionerHandle)
    {

        if ((iRequired & ADDRESS_REQUIRED) != 0)
        {
            String[] addr = new String[NUM_ADDRESS_FIELDS];

            int availableFields = _getAddressInfo(aPositionerHandle, addr);

            if (availableFields > 0)
            {
                AddressInfo addrInfo = new AddressInfo();

                for (int i = 0; i < NUM_ADDRESS_FIELDS; ++i)
                {
                    if ((availableFields & (1 << i)) > 0)
                    {
                        // setField index starts at 1
                        addrInfo.setField(i + 1, addr[i]);
                    }
                }

                aLocation.setAddressInfo(addrInfo);
            }
        }

        if ((iRequired & SPEED_AND_COURSE_REQUIRED) != 0)
        {
            float[] speedCourse = new float[2];
            boolean hasSpeedAndCourse = _getSpeedAndCourse(aPositionerHandle,
                                        speedCourse);
            if (hasSpeedAndCourse)
            {
                aLocation.setSpeedAndCourse(speedCourse[0], speedCourse[1]);
            }
        }

        double[] latlong = new double[2];
        float[] altHaccVacc = new float[3];
        String[] nmeaData = new String[1];
        long timestamp = _getQualifiedCoordinates(aPositionerHandle, latlong,
                         altHaccVacc, nmeaData);

        QualifiedCoordinates qc = new QualifiedCoordinates(latlong[0],
                latlong[1], altHaccVacc[0], altHaccVacc[1], altHaccVacc[2]);

        aLocation.setLocationData(qc, timestamp, nmeaData[0]);
    }

    void registeredFinalize()
    {
        _dispose(iFunctionServerHandle, iHandle);
    }

    /**
     * Returns a handle to this location provider. This method is
     * intentionally package private
     */
    int getProviderHandle()
    {
        return iHandle;
    }

    /**
     * Returns a handle to the function source of this location provider.
     * This method is intentionally package private
     */
    int getFunctionSourceHandle()
    {
        return iFunctionServerHandle;
    }

    private native void _dispose(int aFunctionServerHandle, int aHandle);

    private native int _createProvider(int aFunctionServerHandle);

    private native int _createFunctionSource();

    private native int _selectModule(int aFunctionSource, int aHandle,
                                     int aHacc, int aVacc, int aRespTime, int aPower, int aRequired,
                                     int[] aMethodTimeout);

    private native long _getQualifiedCoordinates(int aPositionerHandle,
            double[] alatlong, float[] aAltHaccVacc, String[] aNmeaData);

    private native boolean _getSpeedAndCourse(int aPositionerHandle,
            float[] aSpeedCourse);

    private native int _startTracking(int aFunctionServerHandle, int aHandle,
                                      int aInterval, int aTimeout, int aMaxAge);

    private native void _stopTracking(int aFunctionServerHandle, int aHandle);

    private native int _getAddressInfo(int aHandle, String[] aAddress);

    private static native int _getLastKnownLocation(double[] alatlong,
            float[] aAltHaccVacc, long[] timestamp);
}

// End of file
