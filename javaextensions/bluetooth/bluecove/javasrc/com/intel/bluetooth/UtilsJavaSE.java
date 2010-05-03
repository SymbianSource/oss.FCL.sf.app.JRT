/**
 *  BlueCove - Java library for Bluetooth
 *  Copyright (C) 2006-2008 Vlad Skarzhevskyy
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
 *  @author vlads
 *  @version $Id: Utils.java 2476 2008-12-01 17:41:59Z skarzhevskyy $
 */

package com.intel.bluetooth;


/**
 * CSB: Replaced UtilsJavaSE as most of the methods in it are not used.
 * All JavaSE specific operation are never done outside. This remains as 
 * dummy implementation. 
 *
 * <p>
 * threadSetDaemon is SE just helps in closing the application even if there
 * are active threads running. In OMJ, Javacaptain itself must be having the
 * capability to destroy the application.
 *
 * <p>
 * initCase is used to detect the cause of crash and enable to print it.
 * currently it just returns the same throwable object back.
 */

public class UtilsJavaSE
{
    public static void threadSetDaemon(Thread thread) {
    }
    
	public static Throwable initCause(Throwable throwable, Throwable cause) {
	    return throwable;
	}
}