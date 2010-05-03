/**
 *  BlueCove - Java library for Bluetooth
 *
 *  Java docs licensed under the Apache License, Version 2.0
 *  http://www.apache.org/licenses/LICENSE-2.0 
 *   (c) Copyright 2001, 2002 Motorola, Inc.  ALL RIGHTS RESERVED.
 *
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 *  
 *  @version $Id: LocalDevice.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */
package javax.bluetooth;

import java.util.Hashtable;

import javax.microedition.io.Connection;

import com.intel.bluetooth.BlueCoveImpl;
import com.intel.bluetooth.BlueCoveLocalDeviceProperties;
import com.intel.bluetooth.BluetoothConnectionNotifierServiceRecordAccess;
import com.intel.bluetooth.BluetoothConsts;
import com.intel.bluetooth.BluetoothStack;
import com.intel.bluetooth.RemoteDeviceHelper;
import com.intel.bluetooth.ServiceRecordsRegistry;
import com.intel.bluetooth.UtilsJavaSE;

/**
 * Please refer JSR-82
 */
public class LocalDevice {

    private static Hashtable localDevices = new Hashtable();

    private BluetoothStack bluetoothStack;

    private DiscoveryAgent discoveryAgent;

    private String addressStr;

    /**
     * The default constructor is hidden so that no one can create a new
     * instance of the LocalDevice. To get the LocalDevice object for this
     * device, use the <code>getLocalDevice()</code> static method in this
     * class.
     * 
     * @see #getLocalDevice
     */
    private LocalDevice(BluetoothStack stack) throws BluetoothStateException {
        this.bluetoothStack = stack;
        discoveryAgent = new DiscoveryAgent(this.bluetoothStack);
        addressStr = RemoteDeviceHelper.formatBluetoothAddress(this.bluetoothStack.getLocalDeviceBluetoothAddress());
    }

    private static synchronized LocalDevice getLocalDeviceInstance() throws BluetoothStateException {
        BluetoothStack stack = BlueCoveImpl.instance().getBluetoothStack();
        LocalDevice localDevice = (LocalDevice) localDevices.get(stack);
        if (localDevice == null) {
            localDevice = new LocalDevice(stack);
            localDevices.put(stack, localDevice);
        }
        return localDevice;
    }

    /**
     * Please refer JSR-82
     */
    public static LocalDevice getLocalDevice() throws BluetoothStateException {
        return getLocalDeviceInstance();
    }

    /**
     * Please refer JSR-82
     */
    public static boolean isPowerOn() {
        try {
            return BlueCoveImpl.instance().getBluetoothStack().isLocalDevicePowerOn();
        } catch (BluetoothStateException e) {
            return false;
        }
    }

    /**
     * Please refer JSR-82
     */
    public DiscoveryAgent getDiscoveryAgent() {
        return discoveryAgent;
    }

    /**
     * Please refer JSR-82
     */
    public String getFriendlyName() {
        return this.bluetoothStack.getLocalDeviceName();
    }

    /**
     * Please refer JSR-82
     */
    public DeviceClass getDeviceClass() {
        return this.bluetoothStack.getLocalDeviceClass();
    }

    /**
     * Please refer JSR-82
     */
    public boolean setDiscoverable(int mode) throws BluetoothStateException {
        if ((mode != DiscoveryAgent.GIAC) && (mode != DiscoveryAgent.LIAC) && (mode != DiscoveryAgent.NOT_DISCOVERABLE) && (mode < 0x9E8B00 || mode > 0x9E8B3F)) {
            throw new IllegalArgumentException("Invalid discoverable mode");
        }
        return this.bluetoothStack.setLocalDeviceDiscoverable(mode);
    }

    /**
     * Please refer JSR-82
     */
    public static String getProperty(String property) {
        try {
            if (BluetoothConsts.PROPERTY_BLUETOOTH_API_VERSION.equals(property)) {
                return BlueCoveImpl.BLUETOOTH_API_VERSION;
            } else if (BluetoothConsts.PROPERTY_OBEX_API_VERSION.equals(property)) {
                return BlueCoveImpl.OBEX_API_VERSION;
            } else if (BlueCoveLocalDeviceProperties.LOCAL_DEVICE_PROPERTY_BLUECOVE_VERSION.equals(property)) {
                return BlueCoveImpl.version;
            } else if (BlueCoveLocalDeviceProperties.LOCAL_DEVICE_PROPERTY_STACK.equals(property)) {
                return BlueCoveImpl.instance().getBluetoothStack().getStackID();
            } else if (BlueCoveLocalDeviceProperties.LOCAL_DEVICE_PROPERTY_FEATURE_L2CAP.equals(property)) {
                return BlueCoveImpl.instance().getLocalDeviceFeature(BluetoothStack.FEATURE_L2CAP);
            } else if (BlueCoveLocalDeviceProperties.LOCAL_DEVICE_PROPERTY_FEATURE_SERVICE_ATTRIBUTES.equals(property)) {
                return BlueCoveImpl.instance().getLocalDeviceFeature(BluetoothStack.FEATURE_SERVICE_ATTRIBUTES);
            } else if (BlueCoveLocalDeviceProperties.LOCAL_DEVICE_PROPERTY_FEATURE_SET_DEVICE_SERVICE_CLASSES.equals(property)) {
                return BlueCoveImpl.instance().getLocalDeviceFeature(BluetoothStack.FEATURE_SET_DEVICE_SERVICE_CLASSES);
            } else if (BlueCoveLocalDeviceProperties.LOCAL_DEVICE_PROPERTY_OPEN_CONNECTIONS.equals(property)) {
                return String.valueOf(RemoteDeviceHelper.openConnections());
            }
            return BlueCoveImpl.instance().getBluetoothStack().getLocalDeviceProperty(property);
        } catch (BluetoothStateException e) {
            throw (RuntimeException) UtilsJavaSE.initCause(new RuntimeException(e.getMessage()), e);
        }
    }

    /**
     * Please refer JSR-82
     */
    public int getDiscoverable() {
        return this.bluetoothStack.getLocalDeviceDiscoverable();
    }

    /**
     * Please refer JSR-82
     */
    public String getBluetoothAddress() {
        return addressStr;
    }

    /**
     * Please refer JSR-82
     */
    public ServiceRecord getRecord(Connection notifier) {
        if (notifier == null) {
            throw new NullPointerException("notifier is null");
        }

        if (!(notifier instanceof BluetoothConnectionNotifierServiceRecordAccess)) {
            throw new IllegalArgumentException("connection is not a Bluetooth notifier");
        }

        return ((BluetoothConnectionNotifierServiceRecordAccess) notifier).getServiceRecord();
    }

    /**
     * Please refer JSR-82
     */
    public void updateRecord(ServiceRecord srvRecord) throws ServiceRegistrationException {
        if (srvRecord == null) {
            throw new NullPointerException("Service Record is null");
        }
        ServiceRecordsRegistry.updateServiceRecord(srvRecord);
    }

}