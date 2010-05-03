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
 *  @version $Id: DiscoveryAgent.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */
package javax.bluetooth;

import com.intel.bluetooth.BluetoothStack;
import com.intel.bluetooth.DebugLog;
import com.intel.bluetooth.RemoteDeviceHelper;
import com.intel.bluetooth.SelectServiceHandler;

/**
 * Please refer JSR-82
 */

public class DiscoveryAgent {

    /**
     * Please refer JSR-82
     */
	public static final int NOT_DISCOVERABLE = 0;

    /**
     * Please refer JSR-82
     */
	public static final int GIAC = 0x9E8B33;

    /**
     * Please refer JSR-82
     */
	public static final int LIAC = 0x9E8B00;

    /**
     * Please refer JSR-82
     */
	public static final int CACHED = 0x00;

    /**
     * Please refer JSR-82
     */
	public static final int PREKNOWN = 0x01;

	private BluetoothStack bluetoothStack;

	/**
	 * Creates a <code>DiscoveryAgent</code> object.
	 */
	private DiscoveryAgent() {
	}

	DiscoveryAgent(BluetoothStack bluetoothStack) {
		this();
		this.bluetoothStack = bluetoothStack;
	}

    /**
     * Please refer JSR-82
     */
	public RemoteDevice[] retrieveDevices(int option) {
		return RemoteDeviceHelper.retrieveDevices(this.bluetoothStack, option);
	}

    /**
     * Please refer JSR-82
     */
	public boolean startInquiry(int accessCode, DiscoveryListener listener) throws BluetoothStateException {
		if (listener == null) {
			throw new NullPointerException("DiscoveryListener is null");
		}
		if ((accessCode != LIAC) && (accessCode != GIAC) && ((accessCode < 0x9E8B00) || (accessCode > 0x9E8B3F))) {
			throw new IllegalArgumentException("Invalid accessCode " + accessCode);
		}
		return this.bluetoothStack.startInquiry(accessCode, listener);
	}

    /**
     * Please refer JSR-82
     */
	public boolean cancelInquiry(DiscoveryListener listener) {
		if (listener == null) {
			throw new NullPointerException("DiscoveryListener is null");
		}
		DebugLog.debug("cancelInquiry");
		return this.bluetoothStack.cancelInquiry(listener);
	}

    /**
     * Please refer JSR-82
     */
	public int searchServices(int[] attrSet, UUID[] uuidSet, RemoteDevice btDev, DiscoveryListener discListener)
			throws BluetoothStateException {
		if (uuidSet == null) {
			throw new NullPointerException("uuidSet is null");
		}
		if (uuidSet.length == 0) {
			// The same as on Motorola, Nokia and SE Phones
			throw new IllegalArgumentException("uuidSet is empty");
		}
		for (int u1 = 0; u1 < uuidSet.length; u1++) {
			if (uuidSet[u1] == null) {
				throw new NullPointerException("uuidSet[" + u1 + "] is null");
			}
			for (int u2 = u1 + 1; u2 < uuidSet.length; u2++) {
				if (uuidSet[u1].equals(uuidSet[u2])) {
					throw new IllegalArgumentException("uuidSet has duplicate values " + uuidSet[u1].toString());
				}
			}
		}
		if (btDev == null) {
			throw new NullPointerException("RemoteDevice is null");
		}
		if (discListener == null) {
			throw new NullPointerException("DiscoveryListener is null");
		}
		for (int i = 0; attrSet != null && i < attrSet.length; i++) {
			if (attrSet[i] < 0x0000 || attrSet[i] > 0xffff) {
				throw new IllegalArgumentException("attrSet[" + i + "] not in range");
			}
		}
		return this.bluetoothStack.searchServices(attrSet, uuidSet, btDev, discListener);
	}

    /**
     * Please refer JSR-82
     */
	public boolean cancelServiceSearch(int transID) {
		DebugLog.debug("cancelServiceSearch", transID);
		return this.bluetoothStack.cancelServiceSearch(transID);
	}

    /**
     * Please refer JSR-82
     */
	public String selectService(UUID uuid, int security, boolean master) throws BluetoothStateException {
		return (new SelectServiceHandler(this)).selectService(uuid, security, master);
	}

}