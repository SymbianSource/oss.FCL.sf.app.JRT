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
 *  @version $Id: ServiceRecord.java 2471 2008-12-01 03:44:20Z skarzhevskyy $
 */
package javax.bluetooth;

import java.io.IOException;

/**
 * Please refer JSR-82
 */
public interface ServiceRecord {

    /**
     * Please refer JSR-82
     */
	public static final int NOAUTHENTICATE_NOENCRYPT = 0;

    /**
     * Please refer JSR-82
     */
	public static final int AUTHENTICATE_NOENCRYPT = 1;

    /**
     * Please refer JSR-82
     */
	public static final int AUTHENTICATE_ENCRYPT = 2;

    /**
     * Please refer JSR-82
     */
	public DataElement getAttributeValue(int attrID);

    /**
     * Please refer JSR-82
     */
	public RemoteDevice getHostDevice();

    /**
     * Please refer JSR-82
     */
	public int[] getAttributeIDs();

    /**
     * Please refer JSR-82
     */
	public boolean populateRecord(int[] attrIDs) throws IOException;

    /**
     * Please refer JSR-82
     */
	public String getConnectionURL(int requiredSecurity, boolean mustBeMaster);

    /**
     * Please refer JSR-82
     */
	public void setDeviceServiceClasses(int classes);

    /**
     * Please refer JSR-82
     */
	public boolean setAttributeValue(int attrID, DataElement attrValue);

}