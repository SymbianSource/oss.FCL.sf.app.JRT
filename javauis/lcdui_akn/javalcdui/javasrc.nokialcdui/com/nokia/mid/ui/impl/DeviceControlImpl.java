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
* Description:  Provides methods for controlling vibrator and screen backlight.
*
*/


package com.nokia.mid.ui.impl;

import javax.microedition.lcdui.Display;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.legacy.ToolkitObserver;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * DeviceControlImpl implements backlight control for UI API and lcdui.
 */
public class DeviceControlImpl
{
    // handle to native toolkit
    private int iToolkitHandle;

    // handle to native devicecontol
    private int iNativeHandle;

    // used for calling Toolkit methods
    private ToolkitInvoker iToolkitInvoker;

    // toolkit, stored as object as we don't have access to Toolkit class.
    private Object iToolkit;

    // static instance of this class
    private static DeviceControlImpl sDeviceControlImpl;

    // error value returned from native side when no error occured
    private static final int ERR_NONE = 0;

    // error value returned from native side when light is not supported
    private static final int ERR_NOT_SUPPORTED = -5;

    // error value returned from native side when application is in the
    // background while reserving light
    private static final int ERR_NOT_READY = -18;

    // error string for negative duration
    private static final String ERR_NEGATIVE_DURATION_STRING =
        "Negative duration";

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    /**
     * Private constructor. This class can't be instantiated.
     */
    private DeviceControlImpl()
    {
        iToolkitInvoker = ToolkitInvoker.getToolkitInvoker();

        iToolkit = iToolkitInvoker.getToolkit();

        iToolkitHandle = iToolkitInvoker.toolkitGetHandle(iToolkit);
        int nativeHandle =  _createDeviceControl(iToolkitHandle);
        if (nativeHandle < 0)
        {
            if (nativeHandle == ERR_NOT_SUPPORTED)
            {
                throw new IllegalArgumentException(
                    "lights are not supported");
            }
            throw new OutOfMemoryError();
        }
        iNativeHandle = nativeHandle;

        iToolkitInvoker.addObserver(
            iToolkit,
            new ToolkitObserver()
        {
            public final void destroyNotify()
            {
                release();
            }
        }
        );
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    // this method deletes all native resources
    final protected void registeredFinalize()
    {
        iToolkitInvoker.toolkitDisposeObject(iToolkit, iNativeHandle);
    }

    // this method restores display backlight state
    synchronized final void release()
    {
        _release(iToolkitHandle, iNativeHandle);
    }

    /**
     * Function return the time in seconds since the last user activity.
     */
    public static int getUserInactivityTime()
    {
        if (sDeviceControlImpl == null)
        {
            sDeviceControlImpl = new DeviceControlImpl();
        }
        synchronized (ToolkitInvoker.getToolkitInvoker().getToolkit())
        {
            return sDeviceControlImpl._getUserInactivityTime(
                       sDeviceControlImpl.iToolkitHandle,
                       sDeviceControlImpl.iNativeHandle);
        }
    }
    /**
     *  Function reset user inactivity time. Thus, if the screen saver should be
     *  wholly disabled, it is needed to call the function repeatedly,
     *  for example in a separate thread. The delay between two calls should be
     *  smaller than the time-out of the screensaver (the time-out may be
     *  for example 15 seconds or more, depending on the used device).
     */
    public static void resetUserInactivityTime()
    {
        if (sDeviceControlImpl == null)
        {
            sDeviceControlImpl = new DeviceControlImpl();
        }
        synchronized (ToolkitInvoker.getToolkitInvoker().getToolkit())
        {
            int retVal = sDeviceControlImpl._resetUserInactivityTime(
                             sDeviceControlImpl.iToolkitHandle,
                             sDeviceControlImpl.iNativeHandle);
            switch (retVal)
            {
            case ERR_NONE: // everything was ok
            case ERR_NOT_READY: // application was on the background
            {
                break;
            }
            default:
            {
                // all other error values are either bugs in the
                // implementation or hardware failures
                throw new OutOfMemoryError();
            }
            } // end of switch (retVal)
        } // end of synchronized (ToolkitInvoker.getToolkitInvoker().getToolkit())
    }

    /**
     *  Activates and deactivates lights of the device. Paramter
     *  num indicated the number of the device light to control.
     *  Currently only one num is specified:
     *  num value 0 is used for controlling the screen backlight.
     *  Parameter level is a value between 0-100
     *  indicating the light brightness. In many implementations
     *  there in only two levels: lights on and off. Value 0 indicates
     *  a setting for lights off (monochrome displays) or other
     *  minimum brightness setting (color displays), all other level
     *  values (1-100) are used for setting the lights on, possibly
     *  with different brightness levels depending on the value. A more
     *  higher level value always results either the same brightness setting
     *  as a lower one, or a more brighter setting. For may products
     *  passing values 1-100 will just turn the lights on.
     *
     *  @param num is the number or id for light, 0 is used for display
     *  backlight, other numbers are currently left unspecified
     *  @param level the lighting level 0-100. 0 means "off" or other
     *  minimum lighting setting. For many devices greater
     *  than 0 value just means "light on".
     *  @throws java.lang.IllegalArgumentException if light num is not
     *  supported or level is not between 0-100.
     *  @since 1.0
     */
    public static void setLights(int aNum, int aLevel)
    {
        if (aNum != 0)
        {
            throw new IllegalArgumentException("Light num is not supported");
        }
        if (aLevel < 0 || aLevel > 100)
        {
            throw(new IllegalArgumentException("Level not between 0 and 100"));
        }
        if (sDeviceControlImpl == null)
        {
            sDeviceControlImpl = new DeviceControlImpl();
        }
        synchronized (ToolkitInvoker.getToolkitInvoker().getToolkit())
        {
            int retVal = sDeviceControlImpl._setLights(sDeviceControlImpl.iToolkitHandle,
                         sDeviceControlImpl.iNativeHandle,
                         aLevel);
            switch (retVal)
            {
            case ERR_NONE: // everything was ok
            case ERR_NOT_READY: // application was on the background
            {
                break;
            }
            default:
            {
                // all other error values are either bugs in the
                // implementation or hardware failures
                throw new OutOfMemoryError();
            }
            } // end of switch (retVal)
        } // end of synchronized (ToolkitInvoker.getToolkitInvoker().getToolkit())
    }

    /**
     *  Does flashing lights effect for specific duration.
     *  This is a generic method for doing implementation specific light
     *  flashing effect. This could be used as feedback e.g. in games.
     *  If the device is not capable of doing the effect the method call
     *  silently returns. In most devices there is at least screen backlight
     *  and different LEDs that could be used for the effect. There might
     *  be maximum limit in implementation for the duration: implementation
     *  stops automatically the effect after maximum duration is reached.
     *  When the flashing lights effect ends the implementation sets the
     *  lights back to the light state the device was before the
     *  method call.
     *
     *  @param duration duration in milliseconds the effect should be
     *  active
     *  @throws java.lang.IllegalArgumentException if duration < 0
     *  @since 1.0
     */
    public static boolean flashLights(long aDuration)
    {
        if (aDuration < 0)
        {
            throw new IllegalArgumentException(ERR_NEGATIVE_DURATION_STRING);
        }

        int duration = (int)aDuration;
        if (aDuration > java.lang.Integer.MAX_VALUE)
        {
            duration = java.lang.Integer.MAX_VALUE;
        }
        if (sDeviceControlImpl == null)
        {
            sDeviceControlImpl = new DeviceControlImpl();
        }
        return sDeviceControlImpl._flashLights(
                   sDeviceControlImpl.iToolkitHandle,
                   sDeviceControlImpl.iNativeHandle,
                   duration);
    }

    /**
     * creates native DeviceControlImpl
     */
    private native int _createDeviceControl(int aToolkitHandle);


    /**
     * Returns user inactivity time in seconds.
     */
    private native int _getUserInactivityTime(int aToolkitHandle,
            int aDeviceControlHandle);


    /**
     * Reset the user inactivity time.
     */
    private native int _resetUserInactivityTime(int aToolkitHandle,
            int aDeviceControlHandle);


    /**
     * Sets the backlight level
     */
    private native int _setLights(int aToolkitHandle,
                                  int aDeviceControlHandle,
                                  int aLevel);

    /**
     * Flashes the backlight
     */
    private native boolean _flashLights(int aToolkitHandle,
                                        int aDeviceControlHandle,
                                        int aDuration);

    /**
     * Releases native resources and restores backlight state.
     */
    private native void _release(int aToolkitHandle,
                                 int aDeviceControlHandle);

}
