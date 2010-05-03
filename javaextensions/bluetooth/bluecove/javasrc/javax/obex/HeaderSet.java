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
 *  @version $Id: HeaderSet.java 2531 2008-12-09 19:43:45Z skarzhevskyy $  
 */
package javax.obex;

import java.io.IOException;

/**
 * Please refer JSR-82
 */
public interface HeaderSet {

    /**
     * Please refer JSR-82
     */
	public static final int COUNT = 0xC0;

    /**
     * Please refer JSR-82
     */
	public static final int NAME = 0x01;

    /**
     * Please refer JSR-82
     */
	public static final int TYPE = 0x42;

    /**
     * Please refer JSR-82
     */
	public static final int LENGTH = 0xC3;

    /**
     * Please refer JSR-82
     */
	public static final int TIME_ISO_8601 = 0x44;

    /**
     * Please refer JSR-82
     */
	public static final int TIME_4_BYTE = 0xC4;

    /**
     * Please refer JSR-82
     */
	public static final int DESCRIPTION = 0x05;

    /**
     * Please refer JSR-82
     */
	public static final int TARGET = 0x46;

    /**
     * Please refer JSR-82
     */
	public static final int HTTP = 0x47;

    /**
     * Please refer JSR-82
     */
	public static final int WHO = 0x4A;

    /**
     * Please refer JSR-82
     */
	public static final int OBJECT_CLASS = 0x4F;

    /**
     * Please refer JSR-82
     */
	public static final int APPLICATION_PARAMETER = 0x4C;

    /**
     * Please refer JSR-82
     */
	public void setHeader(int headerID, Object headerValue);

    /**
     * Please refer JSR-82
     */
	public Object getHeader(int headerID) throws IOException;

    /**
     * Please refer JSR-82
     */
	public int[] getHeaderList() throws IOException;

    /**
     * Please refer JSR-82
     */
	public void createAuthenticationChallenge(String realm, boolean userID, boolean access);

    /**
     * Please refer JSR-82
     */
	public int getResponseCode() throws IOException;
}
