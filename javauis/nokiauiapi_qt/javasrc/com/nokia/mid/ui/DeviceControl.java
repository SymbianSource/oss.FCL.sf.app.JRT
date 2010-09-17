/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mid.ui;

import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.internal.extension.DisplayExtension;

/**
 * <p>
 * DeviceControl provides a collection of methods to control some of the special
 * features available in mobile devices, e.g., vibrator, screen (LCD)
 * backlight, and screen saver appearance.
 * <p>
 * This class is thread safe.
 * @version 1.0
 * @since 1.0
 */

public class DeviceControl
{
    // error string for negative duration
    private static final String ERR_NEGATIVE_DURATION_STRING =
        "Negative duration";
    private static int inactivityTime;
    private static int vibraDuration;
    private static boolean vibraSupported;

    /**
     * Private constructor. This class can't be instantiated.
     */
    private DeviceControl()
    {
    }
    /**
     * Function return the time since the last user activity in seconds.
     * @since 1.4
     */
    public static int getUserInactivityTime()
    {
        synchronized (DeviceControl.class)
        {
            if (DisplayExtension.getDisplayInstance() != null)
            {
                Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable()
                {
                    public void run()
                    {
                        inactivityTime = OS.MobileDevice_getUserInactivityTime(
                                             Internal_PackageSupport.initializeMobileDevice(
                                                 Internal_PackageSupport.getDisplayInstance()));
                    }
                });
            }
        }
        return inactivityTime;
    }
    /**
     *  Function reset user inactivity time. Thus, if the screen saver should be
     *  wholly disabled, it is needed to call the function repeatedly for example in a
     *  separate thread. The delay between two calls should be smaller than
     *  the time-out of the screensaver (the time-out may be for example 15 seconds
     *  or more depending on the used device).
     *  @since 1.4
     */
    public static void resetUserInactivityTime()
    {
        synchronized (DeviceControl.class)
        {
            if (DisplayExtension.getDisplayInstance() != null)
            {
                Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable()
                {
                    public void run()
                    {
                        OS.MobileDevice_resetUserInactivityTime(
                            Internal_PackageSupport.initializeMobileDevice(
                                Internal_PackageSupport.getDisplayInstance()));
                    }
                });
            }
        }
    }

    /**
     *  Activates and deactivates lights on the device; the function
     *  can also be used for preventing screen saver appearance. Parameter
     *  num indicates the number of the device light to control.
     *  Currently only one num parameter is specified:
     *  num value 0 is used for controlling the screen backlight.
     *  Parameter level is a value between 0-100
     *  indicating the light brightness. In many implementations
     *  there in only two levels: lights on and off. Value 0 indicates
     *  a setting for lights off (monochrome displays) or other
     *  minimum brightness setting (color displays), all other level
     *  values (1-100) are used for setting the lights on, possibly
     *  with different brightness levels depending on the value. A more
     *  higher level value always results either the same brightness setting
     *  as a lower one, or a more brighter setting. For many products
     *  passing values 1-100 will just turn the lights on.
     *  <p>
     *  Note: Since in some devices key presses may turn on some device lights,
     *  the light settings may be changed also by the system.
     *  <p>
     *  This function may also be used to prevent screen saver appearance
     *  (supported in S60 devices starting from S60 3rd Ed FP1,
     *  except for some early FP1 devices). Calling this function once
     *  will delay the screen saver appearance but does not disable it permanently.
     *  Thus, if the screen saver should be wholly disabled,
     *  it is needed to call the function repeatedly for example in a
     *  separate thread. The delay between two calls should be smaller than
     *  the time-out of the screensaver (the time-out may be for example 15 seconds
     *  or more depending on the used device).
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
        synchronized (DeviceControl.class)
        {
            if (aNum != 0)
            {
                throw new IllegalArgumentException("Light num is not supported");
            }
            if (aLevel < 0 || aLevel > 100)
            {
                throw(new IllegalArgumentException("Level not between 0 and 100"));
            }
            final int level = aLevel;

            if (DisplayExtension.getDisplayInstance() != null)
            {
                Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable()
                {
                    public void run()
                    {
                        OS.MobileDevice_setLight(Internal_PackageSupport.initializeMobileDevice(
                                                     Internal_PackageSupport.getDisplayInstance()), level);
                    }
                });
            }
        }
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
    public static void flashLights(long aDuration)
    {
        synchronized (DeviceControl.class)
        {
            if (aDuration < 0)
            {
                throw new IllegalArgumentException(ERR_NEGATIVE_DURATION_STRING);
            }

            final int duration;
            if (aDuration > java.lang.Integer.MAX_VALUE)
            {
                duration = java.lang.Integer.MAX_VALUE;
            }
            else
            {
                duration =(int)aDuration;
            }
            if (DisplayExtension.getDisplayInstance() != null)
            {
                Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable()
                {
                    public void run()
                    {
                    	System.out.println("Java Nokia UI API flashLights duration = "+duration);
                        OS.MobileDevice_flashLights(
                            Internal_PackageSupport.initializeMobileDevice(
                                Internal_PackageSupport.getDisplayInstance()),duration);
                        System.out.println("Java Nokia UI API End flashLights ");
                    }
                });
            }
        }

    }

    /**
     *  Activates the vibra device with given duration and frequency.
     *  Device vibra feedback can be used, for example, as tactile feedback
     *  effect e.g. in games. The frequency of the vibra device
     *  can be controlled with freq parameter. The frequency is a logical
     *  number between 0 to 100, value 100 being the maximum frequency,
     *  value 0 always means no vibra, value 1 always needs to give some
     *  effect. If a device doesn't support different frequencies then
     *  the just frequency allowed by device is used.
     *  <p>
     *  Parameter duration is used to indicate duration of vibra
     *  in milliseconds. Method won't block for the vibra operation.
     *  There is some maximum limit in implementation for the
     *  duration. Duration values that exceed this limit result a
     *  maximum vibration effect.
     *  <p>
     *  If the method is called during a previous vibra operation activated
     *  from this method, the previous vibra operation is stopped and the new
     *  vibra device is activated with the new given parameters.
     *  <p>
     *  IllegalStateException will be thrown if the freq is not 0 and
     *  if device doesn't allow vibra to be used, for example,
     *  it is common that when the device is being charged in desk stand or
     *  there is incoming call that vibra operation is not allowed.
     *  IllegalStateException will be thrown if there
     *  is no vibra capability in the device.
     *  <p>
     *  Note that if the frequency is 0, and the device doesn't have vibra
     *  capability IllegalStateException is thrown. This means that method
     *  call <code>DeviceControl.doVibra(0,0);</code> can be used to detect
     *  whether vibra is supported (IllegalStateException not thrown)
     *  or not (IllegalStateException thrown) without any vibra operation
     *  being done even if it is supported.
     *  @param freq the frequency of the vibra device. Value 0 can be used
     *  for detection whether there is vibra device or not. 100 means
     *  maximum frequency value of specific system. Value 1 must always
     *  result a minimum non-zero vibra operation.
     *  @param duration in milliseconds the duration the vibra device is
     *  active
     *  @throws java.lang.IllegalStateException For freq values 1-100:
     *  the use of vibra device isn't allowed or the system doesn't have
     *  vibra device. For frequency value 0: the device doesn't have vibra
     *  device.
     *  @throws java.lang.IllegalArgumentException if duration or freq is
     *  < 0, or freq is > 100.
     *  @since 1.0
     */
    public static void startVibra(int aFreq, long aDuration)
    {
        synchronized (DeviceControl.class)
        {
            if ((aFreq < 0) || (aFreq > 100) || (aDuration < 0))
            {
                throw new java.lang.IllegalArgumentException();

            }
            vibraDuration = (int)aDuration;
            if (aDuration > java.lang.Integer.MAX_VALUE)
            {
                vibraDuration = java.lang.Integer.MAX_VALUE;
            }
            if (aFreq == 0)
            {
                // frequency 0 disables vibration
                vibraDuration = 0;
            }
            if (DisplayExtension.getDisplayInstance() != null)
            {
                Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable()
                {
                    public void run()
                    {
                    	System.out.println("Java Nokia UI API start vibra with duration = "+vibraDuration);
                        vibraSupported = OS.MobileDevice_vibration(
                                             Internal_PackageSupport.initializeMobileDevice(
                                                 Internal_PackageSupport.getDisplayInstance()),vibraDuration);
                        System.out.println("Java Nokia UI API End vibra ");
                    }
                });
            }
            if (!vibraSupported)
            {
                throw new java.lang.IllegalStateException();
            }
        }
    }

    /**
     *  Stops the vibra device. This method is for manually stopping vibra
     *  operation activated with
     *  {@link DeviceControl#startVibra(int freq, long duration)}
     *  method.
     *  If the vibra device is not active the method silently returns.
     *  @since 1.0
     */
    public static void stopVibra()
    {
        synchronized (DeviceControl.class)
        {
            if (DisplayExtension.getDisplayInstance() != null)
            {
                Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable()
                {
                    public void run()
                    {
                        vibraSupported = OS.MobileDevice_vibration(
                                             Internal_PackageSupport.initializeMobileDevice(
                                                 Internal_PackageSupport.getDisplayInstance()),0);
                    }
                });
            }
        }
    }

}


