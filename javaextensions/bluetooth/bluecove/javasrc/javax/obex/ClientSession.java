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
 *  @version $Id: ClientSession.java 2531 2008-12-09 19:43:45Z skarzhevskyy $  
 */
package javax.obex;

import java.io.IOException;

import javax.microedition.io.Connection;

/**
 * Please refer JSR-82
 */
public interface ClientSession extends Connection {

    /**
     * Please refer JSR-82
     */
	public void setAuthenticator(Authenticator auth);

    /**
     * Please refer JSR-82
     */
	public HeaderSet createHeaderSet();

    /**
     * Please refer JSR-82
     */
	public void setConnectionID(long id);

    /**
     * Please refer JSR-82
     */
	public long getConnectionID();

    /**
     * Please refer JSR-82
     */
	public HeaderSet connect(HeaderSet headers) throws IOException;

    /**
     * Please refer JSR-82
     */
	public HeaderSet disconnect(HeaderSet headers) throws IOException;

    /**
     * Please refer JSR-82
     */
	public HeaderSet setPath(HeaderSet headers, boolean backup, boolean create) throws IOException;

    /**
     * Please refer JSR-82
     */
	public HeaderSet delete(HeaderSet headers) throws IOException;

    /**
     * Please refer JSR-82
     */
	public Operation get(HeaderSet headers) throws IOException;

    /**
     * Please refer JSR-82
     */
	public Operation put(HeaderSet headers) throws IOException;
}
