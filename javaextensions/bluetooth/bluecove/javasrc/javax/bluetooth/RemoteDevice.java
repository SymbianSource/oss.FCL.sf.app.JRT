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
 *  @version $Id: RemoteDevice.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */
package javax.bluetooth;

import java.io.IOException;

import javax.microedition.io.Connection;

import com.intel.bluetooth.DebugLog;
import com.intel.bluetooth.RemoteDeviceHelper;
import com.intel.bluetooth.UtilsJavaSE;

/**
 * Please refer JSR-82
 */
public class RemoteDevice {

	/**
	 * A bluetooth hex address
	 */
	private String addressStr;

	/**
	 * A bluetooth address for internal use
	 */
	private long addressLong;

    /**
     * Please refer JSR-82
     */
	protected RemoteDevice(String address) {
		if (address == null) {
			throw new NullPointerException("address is null");
		}
		if (address.length() != 12) {
			throw new IllegalArgumentException("Malformed address: " + address + "; should be 12 characters");
		}
		if (address.startsWith("-")) {
			throw new IllegalArgumentException("Malformed address: " + address + "; can't be negative");
		}
		DebugLog.debug("new RemoteDevice", address);
		this.addressStr = RemoteDeviceHelper.formatBluetoothAddress(address);
		try {
			if (this.addressStr.equals(LocalDevice.getLocalDevice().getBluetoothAddress())) {
				throw new IllegalArgumentException("can't use the LocalDevice address.");
			}
		} catch (BluetoothStateException e) {
			throw (RuntimeException) UtilsJavaSE.initCause(new RuntimeException("Can't initialize bluetooth support"), e);
		}
		this.addressLong = RemoteDeviceHelper.getAddress(address);
	}

    /**
     * Please refer JSR-82
     */
	public boolean isTrustedDevice() {
		return RemoteDeviceHelper.isTrustedDevice(this);
	}

    /**
     * Please refer JSR-82
     */
	public String getFriendlyName(boolean alwaysAsk) throws IOException {
		return RemoteDeviceHelper.getFriendlyName(this, this.addressLong, alwaysAsk);
	}

    /**
     * Please refer JSR-82
     */
	public final String getBluetoothAddress() {
		return this.addressStr;
	}

    /**
     * Please refer JSR-82
     */
	public boolean equals(Object obj) {
		return obj != null && obj instanceof RemoteDevice && ((RemoteDevice) obj).addressLong == addressLong;
	}

    /**
     * Please refer JSR-82
     */
	public int hashCode() {
		return new Long(addressLong).hashCode();
	}

    /**
     * Please refer JSR-82
     */
	public static RemoteDevice getRemoteDevice(Connection conn) throws IOException {
		return RemoteDeviceHelper.getRemoteDevice(conn);
	}

    /**
     * Please refer JSR-82
     */
	public boolean authenticate() throws IOException {
		return RemoteDeviceHelper.authenticate(this);
	}

    /**
     * Please refer JSR-82
     */
	public boolean authorize(javax.microedition.io.Connection conn) throws IOException {
		return RemoteDeviceHelper.authorize(this, conn);
	}

    /**
     * Please refer JSR-82
     */
	public boolean encrypt(javax.microedition.io.Connection conn, boolean on) throws IOException {
		return RemoteDeviceHelper.encrypt(this, conn, on);
	}

    /**
     * Please refer JSR-82
     */
	public boolean isAuthenticated() {
		return RemoteDeviceHelper.isAuthenticated(this);
	}

    /**
     * Please refer JSR-82
     */
	public boolean isAuthorized(javax.microedition.io.Connection conn) throws IOException {
		return RemoteDeviceHelper.isAuthorized(this, conn);
	}

    /**
     * Please refer JSR-82
     */
	public boolean isEncrypted() {
		return RemoteDeviceHelper.isEncrypted(this);
	}

}
