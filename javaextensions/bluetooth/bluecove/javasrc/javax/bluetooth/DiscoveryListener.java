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
 *  @version $Id: DiscoveryListener.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */
package javax.bluetooth;

/**
 * Please refer JSR-82
 */
public interface DiscoveryListener {

    /**
     * Please refer JSR-82
     */
	public static final int INQUIRY_COMPLETED = 0x00;

    /**
     * Please refer JSR-82
     */
	public static final int INQUIRY_TERMINATED = 0x05;

    /**
     * Please refer JSR-82
     */
	public static final int INQUIRY_ERROR = 0x07;

    /**
     * Please refer JSR-82
     */
	public static final int SERVICE_SEARCH_COMPLETED = 0x01;

    /**
     * Please refer JSR-82
     */
	public static final int SERVICE_SEARCH_TERMINATED = 0x02;

    /**
     * Please refer JSR-82
     */
	public static final int SERVICE_SEARCH_ERROR = 0x03;

    /**
     * Please refer JSR-82
     */
	public static final int SERVICE_SEARCH_NO_RECORDS = 0x04;

    /**
     * Please refer JSR-82
     */
	public static final int SERVICE_SEARCH_DEVICE_NOT_REACHABLE = 0x06;

    /**
     * Please refer JSR-82
     */
	public void deviceDiscovered(RemoteDevice btDevice, DeviceClass cod);

    /**
     * Please refer JSR-82
     */
	public void servicesDiscovered(int transID, ServiceRecord[] servRecord);

    /**
     * Please refer JSR-82
     */
	public void serviceSearchCompleted(int transID, int respCode);

    /**
     * Please refer JSR-82
     */
	public void inquiryCompleted(int discType);
}
