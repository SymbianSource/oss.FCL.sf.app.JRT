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
 *  @version $Id: ServiceRegistrationException.java 2471 2008-12-01 03:44:20Z skarzhevskyy $
 */
package javax.bluetooth;

import java.io.IOException;

/**
 * Please refer JSR-82
 */
public class ServiceRegistrationException extends IOException {

	private static final long serialVersionUID = 1L;

    /**
     * Please refer JSR-82
     */
	public ServiceRegistrationException() {
		super();
	}

    /**
     * Please refer JSR-82
     */
	public ServiceRegistrationException(String msg) {
		super(msg);
	}

}
