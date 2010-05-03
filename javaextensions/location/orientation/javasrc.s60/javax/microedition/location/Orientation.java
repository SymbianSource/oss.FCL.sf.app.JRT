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
* Description:  JSR-179 Location API Orientation class
 *
*/


// PACKAGE
package javax.microedition.location;

// IMPORTS
import com.nokia.mj.impl.location.LocationPermission;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.location.NativeError;
import com.nokia.mj.impl.location.LAPIManager;

// CLASS DESCRIPTION
/**
 * J2ME JSR-179 Location API 1.0 complying Orientation class
 */
public class Orientation
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javalocation");
        }
        catch (Exception e)
        {
        }
    }

    private final static String UNABLE_TO_OBTAIN_ORIENTATION
    = "Unable to obtain orientation";

    private final static String ORIENTATION_NOT_SUPPORTED
    = "Orientation is not supported by the device";

    // Orienation Data
    private float iAzimuth;

    private float iPitch;

    private float iRoll;

    private boolean iIsMagnetic;

    private static final int AZIMUTH = 0;

    // Handle to the native Function source
    private final int iFunctionServerHandle;

    private Finalizer iFinalizer;

    private final int iHandle;

    private static boolean iIsAzimuthCorrectData = true;

    private static final Object iLock = new Object();

    public Orientation(float azimuth, boolean isMagnetic, float pitch,
                       float roll)
    {
        iFinalizer = registerFinalize();

        iFunctionServerHandle = LAPIManager.getInstance().getFunctionSourceHandle();

        if (iFunctionServerHandle == NativeError.KErrNone)
        {
            throw new OutOfMemoryError(UNABLE_TO_OBTAIN_ORIENTATION);
        }

        iHandle = _createNativeHandle(iFunctionServerHandle);

        this.iAzimuth = azimuth;
        this.iIsMagnetic = isMagnetic;
        this.iPitch = pitch;
        this.iRoll = roll;
    }

    // Returns the Azimuth Data
    public float getCompassAzimuth()
    {
        return iAzimuth;
    }

    // Returns the Orientation Data is Magnetic or w.r.t to True North
    public boolean isOrientationMagnetic()
    {
        return iIsMagnetic;
    }

    // Returns the Pitch Data
    public float getPitch()
    {
        return iPitch;
    }

    // Returns the Roll Data
    public float getRoll()
    {
        return iRoll;
    }

    // returns an Orientation object containing the terminal’s current
    // orientation or null if the orientation can’t be currently determined
    public static synchronized Orientation getOrientation()
    throws LocationException
    {
        boolean isMagnetic = true;

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("location://*",
                "orientation");
        appUtils.checkPermission(permission);

        // Create an empty object to create an event server and native peer
        Orientation orientInstance = new Orientation(Float.NaN, true,
                Float.NaN, Float.NaN);

        // Check if the handles are created correctly
        if ((orientInstance.getEventSourceHandle()) <= NativeError.KErrNone
                || (orientInstance.getNativeHandle() <= NativeError.KErrNone))
        {
            // Garbage collector will call registeredFinalize
            orientInstance = null;
            return null;
        }

        // Make request for azimuth data
        int res = _getAzimuthData(orientInstance.getEventSourceHandle(),
                                  orientInstance.getNativeHandle());

        // if the channel is not found, then hardware is not supported
        // throw a Location Exception.
        if (res == NativeError.KErrGeneral)
        {
            throw new LocationException(ORIENTATION_NOT_SUPPORTED);
        }
        else if (res < NativeError.KErrNone)
        {
            iIsAzimuthCorrectData = false;
        }
        else
        {
            // wait for java CallBack thread to notify
            synchronized (iLock)
            {
                try
                {
                    iLock.wait();
                }
                catch (InterruptedException e)
                {
                }
            }
        }

        // Ensure Azimuth Data is correct.
        if (iIsAzimuthCorrectData)
        {
            // Get the orientaion data from the native peer
            orientInstance.iAzimuth= _getData(orientInstance.getNativeHandle());

            orientInstance.iIsMagnetic = isMagnetic;

            return orientInstance;
        }
        else
        {
            // Garbage collector will call registeredFinalize
            orientInstance = null;
            return null;
        }
    }

    // Event server call back for azimuth Data
    static void AzimuthDataCallBack(int result)
    {
        if (result < NativeError.KErrNone)
        {
            iIsAzimuthCorrectData = false;
        }

        synchronized (iLock)
        {
            iLock.notify();
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

    /**
     * Disposes the Orienation native peer, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in
     * the first place.
     */
    void registeredFinalize()
    {
        _dispose(iFunctionServerHandle, iHandle);
    }

    /**
     * Returns a handle to the event source.
     * This method is intentionally package private
     */
    int getEventSourceHandle()
    {
        return iFunctionServerHandle;
    }

    /**
     * Returns a handle to the Native Peer.
     * This method is intentionally package private
     */
    int getNativeHandle()
    {
        return iHandle;
    }

    private native static int _getAzimuthData(int aFunctionServerHandle,
            int aHandle);

    private native static float _getData(int aHandle);

    /**
     * Disposes the native event source
     */
    private native void _dispose(int aFunctionServerHandle, int aHandle);

    private native int _createNativeHandle(int aFunctionServerHandle);

}

// End of file
