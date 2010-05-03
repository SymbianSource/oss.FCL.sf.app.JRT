/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: RmsErrorStrings
*
*/


package com.nokia.mj.impl.rms;

/**
 * Error strings to be used in RecordStoreExceptions
 */
public final class RmsErrorStrings
{
    public static final String DATA_NULL        = "Data cannot be null if data length is non-zero";
    public static final String FULL             = "Record store maximum size reached";
    public static final String INVALID_ID       = "Operation could not be completed because the record ID is invalid: ";
    public static final String INVALID_MODE     = "Access mode is invalid";
    public static final String INVALID_NAME     = "Record store name is invalid";
    public static final String NOT_ALLOWED      = "Only read operations allowed";
    public static final String NOT_FOUND        = "Record store does not exist: ";
    public static final String NOT_OPEN         = "Record store must be open";
    public static final String NOT_USABLE       = "RecordEnumeration is not usable after destroy() has been called";
    public static final String NO_NEXT_RECORD   = "Next/previous record not found";
    public static final String OPEN_FAILED      = "Cannot open record store: ";
    public static final String READ_FAILED      = "Read operation failed";
    public static final String RESIZE_FAILED    = "Resize operation failed";
    public static final String RS_IS_OPEN       = "Cannot delete currently opened record store: ";
    public static final String WRITE_FAILED     = "Write operation failed";
}
