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
* Description:
*
*/

package com.nokia.mj.impl.bluetooth;

import java.io.IOException;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

import com.nokia.mj.impl.bluetooth.utils.BluetoothStateCheck;

/**
 * BTPlatformControl provides functionality check for Bluetooth power on and
 * visiblity mode. During the check, if either of the two are not in the state
 * required, they are changed. Prompts are shown to the user when necessary.
 */
public class BTPlatformControl implements BluetoothStateCheck
{
    static final int BT_GIAC = 0x9E8B33;

    static final int BT_LIAC = 0x9E8B00;

    static final int BT_HIDDEN = 0;

    static
    {
        try
        {
            // Loading common native libraries.
            Jvm.loadSystemLibrary("javabluetoothcommons");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaBluetooth,
                        "Unable to load JavaBluetoothCommons.");
        }
    }

    /**
     * Checks to see if Bluetooth is on. If it is not switched on currently, it
     * will be switched on.
     * <p>
     * Before Bluetooth can be switched on, it is necessary that the user is
     * prompted asking him to permit switching on Blueooth. If the user agrees,
     * then the it is switched on. The prompt is not needed in case the
     * application is manufacturer domain.
     *
     * @return true if bluetooth is currently on (switch on was successful),
     *         false otherwise.
     */
    public boolean checkDevicePowerOn(String aAppName, boolean aIsStaticReg)
    {
        return setBTPowerOn(aAppName, aIsStaticReg);
    }

    /**
     * Checks to see if the Bluetooth device is discoverable by all. If it is
     * currently not discoverable by other devices, the visibility mode will be
     * changed.
     * <p>
     * Before this can be done, user must be prompted for permission to change
     * the discoverability mode. If user agrees, then mode is changed. This
     * prompt is not needed in case the application is manufacturer domain.
     *
     * @return true if bluetooth device is currently discoverable by all. false
     *         otherwise.
     */
    public boolean checkDeviceDiscoverability(String aAppName,
            boolean aIsStaticReg)
    {
        return setDiscoverabilityGIAC(aAppName, aIsStaticReg);
    }

    /**
     * Checks to see if Bluetooth is on. If it is not switched on currently, it
     * will be switched on.
     * <p>
     * Before Bluetooth can be switched on, it is necessary that the user is
     * prompted asking him to permit switching on Blueooth. If the user agrees,
     * then the it is switched on. The prompt is not needed in case the
     * application is manufacturer domain.
     *
     * @return true if bluetooth is currently on (switch on was successful),
     *         false otherwise.
     */
    public static boolean setBTPowerOn(String aAppName, boolean aIsStaticReg)
    {
        Logger.LOG(Logger.EJavaBluetooth, Logger.EInfo,
                   "# BTPlatformControl::setBTPowerOn");

        if (_isPowerOn())
        {
            // If Bluetooth is already switched on,
            // we need not go any further.
            return true;
        }
        // Prompt the user to switch on bluetooth.
        boolean result = _promptPowerOn(aAppName, aIsStaticReg);
        if (false == result)
        {
            // User did not permit switching on bluetooth.
            Logger.WLOG(Logger.EJavaBluetooth,
                        "User refused Bluetooth Power on.");
            return false;
        }

        return _setPowerOn();
    }

    /**
     * Checks to see if the Bluetooth device is discoverable by all. If it is
     * currently not discoverable by other devices, the visibility mode will be
     * changed.
     * <p>
     * Before this can be done, user must be prompted for permission to change
     * the discoverability mode. If user agrees, then mode is changed. This
     * prompt is not needed in case the application is manufacturer domain.
     *
     * @return true if bluetooth device is currently discoverable by all. false
     *         otherwise.
     */
    public static boolean setDiscoverabilityGIAC(String aAppName,
            boolean aIsStaticReg)
    {
        Logger.LOG(Logger.EJavaBluetooth, Logger.EInfo,
                   "# BTPlatformControl::setDiscoverabilityGIAC");

        if (_getBluetoothDiscoverable() == BT_GIAC)
        {
            // If Bluetooth device is already discoverable, we need not go
            // any further.
            return true;
        }
        // Prompt the user to switch on bluetooth.
        boolean result = _promptVisibilityChange(aAppName, aIsStaticReg);
        if (false == result)
        {
            // User did not permit changing visibility mode.
            Logger.WLOG(Logger.EJavaBluetooth,
                        "User refused Bluetooth Visibility change.");
            return false;
        }

        return _setBluetoothDiscoverable(BT_GIAC);

    }

    /**
     * Retrieves bluetooth address of the current device.
     *
     * @return string containing the bluetooth address
     *
     */
    public static native String _getBluetoothAddress();

    /**
     * Retrieves the bluetooth name of the current device.
     *
     * @return string containing the bluetooth address
     */
    public static native String _getBluetoothName();

    /**
     * Retrieves the bluetooth device class of the current device.
     *
     * @return integer record represents the device class.
     */
    public static native int _getBluetoothDeviceClass();

    /**
     * Retrieves the bluetooth discoverability mode of the current device.
     *
     * @return integer represents the discoverability mode
     */
    public static native int _getBluetoothDiscoverable();

    /**
     * Set the bluetooth discoverability mode.
     *
     * @param aMode
     *            discoverability mode.
     * @return true if successfully set the mode, otherwise returns false.
     */
    public static native boolean _setBluetoothDiscoverable(int aMode);

    /**
     * Checks if Bluetooth is currently switched on.
     *
     * @return true if bluetooth is currently switched ON, otherwise returns
     *         false.
     */
    public static native boolean _isPowerOn();

    /**
     * Retrieves requested property from native
     *
     * @param aPropertyType
     *            property index
     * @return property value
     */
    public static native String _getProperty(int aPropertyType);

    /**
     * Sets the requested service device class
     *
     * @param aServiceClassBits
     *            New Service Class Bits
     * @return New Modified service class
     */
    public static native int _setServiceClasses(int aServiceClassBits);

    // Switch on Bluetooth.
    private static native boolean _setPowerOn();

    // Prompt the user to switch on Bluetooth.
    private static native boolean _promptPowerOn(String appName,
            boolean aIsStaticReg);

    // Prompt the user to change visibility mode.
    private static native boolean _promptVisibilityChange(String appName,
            boolean aIsStaticReg);
}