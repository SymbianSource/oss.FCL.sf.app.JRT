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


package com.nokia.mj.impl.bluetooth.utils;

public interface BluetoothStateCheck
{
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
    public boolean checkDevicePowerOn(String aAppName, boolean aIsStaticReg);

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
            boolean aIsStaticReg);

}
