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
 *  @version $Id: ResponseCodes.java 2532 2008-12-09 20:23:14Z skarzhevskyy $  
 */
package javax.obex;

/**
 * Please refer JSR-82
 */
public class ResponseCodes {

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_OK = 0xA0;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_CREATED = 0xA1;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_ACCEPTED = 0xA2;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_NOT_AUTHORITATIVE = 0xA3;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_NO_CONTENT = 0xA4;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_RESET = 0xA5;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_PARTIAL = 0xA6;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_MULT_CHOICE = 0xB0;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_MOVED_PERM = 0xB1;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_MOVED_TEMP = 0xB2;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_SEE_OTHER = 0xB3;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_NOT_MODIFIED = 0xB4;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_USE_PROXY = 0xB5;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_BAD_REQUEST = 0xC0;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_UNAUTHORIZED = 0xC1;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_PAYMENT_REQUIRED = 0xC2;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_FORBIDDEN = 0xC3;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_NOT_FOUND = 0xC4;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_BAD_METHOD = 0xC5;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_NOT_ACCEPTABLE = 0xC6;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_PROXY_AUTH = 0xC7;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_TIMEOUT = 0xC8;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_CONFLICT = 0xC9;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_GONE = 0xCA;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_LENGTH_REQUIRED = 0xCB;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_PRECON_FAILED = 0xCC;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_ENTITY_TOO_LARGE = 0xCD;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_REQ_TOO_LARGE = 0xCE;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_UNSUPPORTED_TYPE = 0xCF;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_INTERNAL_ERROR = 0xD0;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_NOT_IMPLEMENTED = 0xD1;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_BAD_GATEWAY = 0xD2;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_UNAVAILABLE = 0xD3;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_GATEWAY_TIMEOUT = 0xD4;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_HTTP_VERSION = 0xD5;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_DATABASE_FULL = 0xE0;

    /**
     * Please refer JSR-82
     */
    public static final int OBEX_DATABASE_LOCKED = 0xE1;

    /**
     * Constructor does nothing.
     */
    private ResponseCodes() {
        throw new RuntimeException("Not Implemented! Used to compile Code");
    }
}
