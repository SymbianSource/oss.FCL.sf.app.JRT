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
 *  @version $Id: UUID.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */
package javax.bluetooth;

import com.intel.bluetooth.BluetoothConsts;
import com.intel.bluetooth.Utils;

/**
 * Please refer JSR-82
 */
public class UUID {

	private byte[] uuidValue;

    /**
     * Please refer JSR-82
     */
	public UUID(long uuidValue) {
		this(Utils.toHexString(uuidValue), true);
		if (uuidValue < 0 || uuidValue > 0xffffffffl) {
			throw new IllegalArgumentException("uuidValue is not in the range [0, 2^32 -1]");
		}
	}

    /**
     * Please refer JSR-82
     */
	public UUID(String uuidValue, boolean shortUUID) {
		if (uuidValue == null) {
            throw new NullPointerException("uuidValue is null");
		}
		int length = uuidValue.length();
		if (shortUUID) {
			if (length < 1 || length > 8) {
				throw new IllegalArgumentException();
			}
			this.uuidValue = Utils.UUIDToByteArray("00000000".substring(length) + uuidValue
					+ BluetoothConsts.SHORT_UUID_BASE);
		} else {
			if (length < 1 || length > 32) {
				throw new IllegalArgumentException();
			}
			this.uuidValue = Utils.UUIDToByteArray("00000000000000000000000000000000".substring(length) + uuidValue);
		}
	}

    /**
     * Please refer JSR-82
     */
	public String toString() {
		return Utils.UUIDByteArrayToString(uuidValue);
	}

    /**
     * Please refer JSR-82
     */
	public boolean equals(Object value) {
		if (value == null || !(value instanceof UUID)) {
			return false;
		}

		for (int i = 0; i < 16; i++) {
			if (uuidValue[i] != ((UUID) value).uuidValue[i]) {
				return false;
			}
		}

		return true;
	}

    /**
     * Please refer JSR-82
     */
	public int hashCode() {
		return uuidValue[12] << 24 & 0xff000000 | uuidValue[13] << 16
			& 0x00ff0000 | uuidValue[14] << 8 & 0x0000ff00 | uuidValue[15]
			& 0x000000ff;
	}
}