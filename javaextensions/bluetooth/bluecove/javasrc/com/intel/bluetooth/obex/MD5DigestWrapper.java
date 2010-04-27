/**
 *  BlueCove - Java library for Bluetooth
 *  Copyright (C) 2007-2008 Vlad Skarzhevskyy
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
 *  @version $Id: MD5DigestWrapper.java 2476 2008-12-01 17:41:59Z skarzhevskyy $
 */
package com.intel.bluetooth.obex;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 *
 *
 */
class MD5DigestWrapper {

	private MessageDigest md5impl;
	private byte[] aArray;

	MD5DigestWrapper() {
		try {
			md5impl = MessageDigest.getInstance("MD5");
			aArray = new byte[16];
		} catch (NoSuchAlgorithmException e) {
			throw new RuntimeException(e.getMessage());
		}
	}

	void update(byte[] input) {
	    md5impl.update(input, 0, input.length);
	}

	byte[] digest() {
	    try
	    {
		    int ret =  md5impl.digest(aArray, 0, aArray.length);
		}catch(Exception ex)
		{
		    return null;
		}
		return aArray;
	}
}
