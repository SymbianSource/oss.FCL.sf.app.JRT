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
 *  @version $Id: ServerRequestHandler.java 2532 2008-12-09 20:23:14Z skarzhevskyy $  
 */
package javax.obex;

import com.intel.bluetooth.obex.OBEXClientSessionImpl;

/**
 * Please refer JSR-82
 */
public class ServerRequestHandler {

    private long connectionID;

    /**
     * Please refer JSR-82
     */
    protected ServerRequestHandler() {
        connectionID = -1;
    }

    /**
     * Please refer JSR-82
     */
    public final HeaderSet createHeaderSet() {
        return OBEXClientSessionImpl.createOBEXHeaderSet();
    }

    /**
     * Please refer JSR-82
     */
    public void setConnectionID(long id) {
        if ((id != -1) && ((id < 0 || id > 0xffffffffl))) {
            throw new IllegalArgumentException("Invalid connectionID " + id);
        }
        this.connectionID = id;
    }

    /**
     * Please refer JSR-82
     */
    public long getConnectionID() {
        return this.connectionID;
    }

    /**
     * Please refer JSR-82
     */
    public int onConnect(HeaderSet request, HeaderSet reply) {
        return ResponseCodes.OBEX_HTTP_OK;
    }

    /**
     * Please refer JSR-82
     */
    public void onDisconnect(HeaderSet request, HeaderSet reply) {
    }

    /**
     * Please refer JSR-82
     */
    public int onSetPath(HeaderSet request, HeaderSet reply, boolean backup, boolean create) {
        return ResponseCodes.OBEX_HTTP_NOT_IMPLEMENTED;
    }

    /**
     * Please refer JSR-82
     */
    public int onDelete(HeaderSet request, HeaderSet reply) {
        return ResponseCodes.OBEX_HTTP_NOT_IMPLEMENTED;
    }

    /**
     * Please refer JSR-82
     */
    public int onPut(Operation op) {
        return ResponseCodes.OBEX_HTTP_NOT_IMPLEMENTED;
    }

    /**
     * Please refer JSR-82
     */
    public int onGet(Operation op) {
        return ResponseCodes.OBEX_HTTP_NOT_IMPLEMENTED;
    }

    /**
     * Please refer JSR-82
     */
    public void onAuthenticationFailure(byte[] userName) {
    }
}
