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
 *  @version $Id: L2CAPConnection.java 2530 2008-12-09 18:52:53Z skarzhevskyy $
 */
package javax.bluetooth;

import java.io.IOException;

import javax.microedition.io.Connection;

/**
 * Please refer JSR-82
 */
public interface L2CAPConnection extends Connection {

    /**
     * Please refer JSR-82
     */
	public static final int DEFAULT_MTU = 672;

    /**
     * Please refer JSR-82
     */
	public static final int MINIMUM_MTU = 48;

    /**
     * Please refer JSR-82
     */
	public int getTransmitMTU() throws IOException;

    /**
     * Please refer JSR-82
     */
	public int getReceiveMTU() throws IOException;

    /**
     * Please refer JSR-82
     */
	public void send(byte[] data) throws IOException;

    /**
     * Please refer JSR-82
     */
	public int receive(byte[] inBuf) throws IOException;

    /**
     * Please refer JSR-82
     */
	public boolean ready() throws IOException;

}
