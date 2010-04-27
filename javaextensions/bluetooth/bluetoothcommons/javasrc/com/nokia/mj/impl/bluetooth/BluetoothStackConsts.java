/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


package com.nokia.mj.impl.bluetooth;

import javax.bluetooth.BluetoothConnectionException;
import javax.bluetooth.DataElement;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.ServiceRecord;

/**
 * Export constants to native code
 *
 */
public abstract class BluetoothStackConsts
{
    public static final int NOT_DISCOVERABLE = DiscoveryAgent.NOT_DISCOVERABLE;

    public static final int GIAC = DiscoveryAgent.GIAC;

    public static final int LIAC = DiscoveryAgent.LIAC;

    public static final int INQUIRY_COMPLETED = DiscoveryListener.INQUIRY_COMPLETED;

    public static final int INQUIRY_TERMINATED = DiscoveryListener.INQUIRY_TERMINATED;

    public static final int INQUIRY_ERROR = DiscoveryListener.INQUIRY_ERROR;

    public static final int SERVICE_SEARCH_COMPLETED = DiscoveryListener.SERVICE_SEARCH_COMPLETED;

    public static final int SERVICE_SEARCH_TERMINATED = DiscoveryListener.SERVICE_SEARCH_TERMINATED;

    public static final int SERVICE_SEARCH_ERROR = DiscoveryListener.SERVICE_SEARCH_ERROR;

    public static final int SERVICE_SEARCH_NO_RECORDS = DiscoveryListener.SERVICE_SEARCH_NO_RECORDS;

    public static final int SERVICE_SEARCH_DEVICE_NOT_REACHABLE = DiscoveryListener.SERVICE_SEARCH_DEVICE_NOT_REACHABLE;

    public static final int NOAUTHENTICATE_NOENCRYPT = ServiceRecord.NOAUTHENTICATE_NOENCRYPT;

    public static final int AUTHENTICATE_NOENCRYPT = ServiceRecord.AUTHENTICATE_NOENCRYPT;

    public static final int AUTHENTICATE_ENCRYPT = ServiceRecord.AUTHENTICATE_ENCRYPT;

    public static final int DataElement_NULL = DataElement.NULL;

    public static final int DataElement_U_INT_1 = DataElement.U_INT_1;

    public static final int DataElement_U_INT_2 = DataElement.U_INT_2;

    public static final int DataElement_U_INT_4 = DataElement.U_INT_4;

    public static final int DataElement_U_INT_8 = DataElement.U_INT_8;

    public static final int DataElement_U_INT_16 = DataElement.U_INT_16;

    public static final int DataElement_INT_1 = DataElement.INT_1;

    public static final int DataElement_INT_2 = DataElement.INT_2;

    public static final int DataElement_INT_4 = DataElement.INT_4;

    public static final int DataElement_INT_8 = DataElement.INT_8;

    public static final int DataElement_INT_16 = DataElement.INT_16;

    public static final int DataElement_URL = DataElement.URL;

    public static final int DataElement_UUID = DataElement.UUID;

    public static final int DataElement_BOOL = DataElement.BOOL;

    public static final int DataElement_STRING = DataElement.STRING;

    public static final int DataElement_DATSEQ = DataElement.DATSEQ;

    public static final int DataElement_DATALT = DataElement.DATALT;

    public static final int CONNECTION_ERROR_UNKNOWN_PSM = BluetoothConnectionException.UNKNOWN_PSM;

    public static final int CONNECTION_ERROR_SECURITY_BLOCK = BluetoothConnectionException.SECURITY_BLOCK;

    public static final int CONNECTION_ERROR_NO_RESOURCES = BluetoothConnectionException.NO_RESOURCES;

    public static final int CONNECTION_ERROR_FAILED_NOINFO = BluetoothConnectionException.FAILED_NOINFO;

    public static final int CONNECTION_ERROR_TIMEOUT = BluetoothConnectionException.TIMEOUT;

    public static final int CONNECTION_ERROR_UNACCEPTABLE_PARAMS = BluetoothConnectionException.UNACCEPTABLE_PARAMS;

    // Used by Persistent storage to indicate the end of SEQ/ALT list
    public static final int DataElement_EndList = 0x00AA;
}
