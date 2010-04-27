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
 *  @version $Id: BluetoothConnectionException.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */ 
package javax.bluetooth;

import java.io.IOException;

/**
 * Please refer JSR-82
 */
public class BluetoothConnectionException extends IOException {

	private static final long serialVersionUID = 1L;

    /**
     * Please refer JSR-82
     */
	public final static int UNKNOWN_PSM = 0x0001;
	
    /**
     * Please refer JSR-82
     */
	public final static int SECURITY_BLOCK = 0x002;

    /**
     * Please refer JSR-82
     */
	public final static int NO_RESOURCES = 0x0003;
	
    /**
     * Please refer JSR-82
     */
	public final static int FAILED_NOINFO = 0x0004;

    /**
     * Please refer JSR-82
     */
	public final static int TIMEOUT = 0x0005;
	
    /**
     * Please refer JSR-82
     */
	public final static int UNACCEPTABLE_PARAMS = 0x0006;
	
	
	private	int	errorCode;
	
    /**
     * Please refer JSR-82
     */
	public BluetoothConnectionException(int error) {
		super();
		if(error < 1 || error > 6) {
			throw new java.lang.IllegalArgumentException();
		}
		errorCode = error;			
	}
	
    /**
     * Please refer JSR-82
     */
	public BluetoothConnectionException(int error, String msg){
		super(msg);
		if (error < 1 || error > 6) {
			throw new java.lang.IllegalArgumentException();
		}
		errorCode = error;
	}

    /**
     * Please refer JSR-82
     */
	public int getStatus() {
		return errorCode;
	}
}
