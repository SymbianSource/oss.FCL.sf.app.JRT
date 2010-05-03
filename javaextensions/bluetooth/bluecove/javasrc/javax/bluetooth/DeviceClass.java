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
 *  @version $Id: DeviceClass.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */ 
package javax.bluetooth;

import com.intel.bluetooth.BluetoothConsts;
import com.intel.bluetooth.DebugLog;

/**
 * Please refer JSR-82
 */
public class DeviceClass {

	private static final int SERVICE_MASK = 0xffe000;

	private static final int MAJOR_MASK = 0x001f00;

	private static final int MINOR_MASK = 0x0000fc;

	private int record;

    /**
     * Please refer JSR-82
     */

	public DeviceClass(int record) {
		
		DebugLog.debug("new DeviceClass", record);
		
		this.record = record;

		if ((record & 0xff000000) != 0)
			throw new IllegalArgumentException();
	}

    /**
     * Please refer JSR-82
     */

	public int getServiceClasses() {
		return record & SERVICE_MASK;
	}

    /**
     * Please refer JSR-82
     */

	public int getMajorDeviceClass() {
		return record & MAJOR_MASK;
	}

    /**
     * Please refer JSR-82
     */

	public int getMinorDeviceClass() {
		return record & MINOR_MASK;
	}

    /**
     * Please refer JSR-82
     */
	public String toString() {
		return BluetoothConsts.toString(this);
	}
}