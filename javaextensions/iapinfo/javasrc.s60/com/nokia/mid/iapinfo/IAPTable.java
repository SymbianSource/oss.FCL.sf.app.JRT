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
* Description:  Provides access for IAP CommDB table.
 *
*/

package com.nokia.mid.iapinfo;

/**
 * This class provides access for IAP CommDB table. Public member variables of
 * the class contains data of the current record.
 *
 * @see com.nokia.mid.commdb.CommsTable
 *
 */
final class IAPTable extends CommsTable
{

    /**
     * Identifier of service in this IAP.
     */
    public int iService;

    /**
     * Name of service table in this IAP.
     */
    public String iServiceType;

    /**
     * Name of bearer table in this IAP.
     */
    public String iBearerType;

    /**
     * Identifier of bearer in this IAP.
     */
    public int iBearer;

    /**
     * Identifier of the network which this IAP connects to.
     */
    public int iNetwork;

    /**
     * Weighting of IAP in relation to other IAPs in same network.
     */
    public long iNetworkWeighting;

    /**
     * Identifier of location in this IAP.
     */
    public int iLocation;

    /**
     * Public constructor.
     */
    public IAPTable() throws CommDBException
    {
        super(CommsTable.TABLE_IAP);
    }

    protected void readFieldValues() throws CommDBException
    {
        if (getRecordCount() <= 0)
        {
            return;
        }

        // Get field values from native side
        // Field names are defined in CommsDatTypeInfoV1_1.h
        iRecordId = readIntFieldValue("RecordId");
        iRecordTag = readIntFieldValue("Id");
        iRecordName = readStringFieldValue("Name");
        iServiceType = readStringFieldValue("IAPServiceType");
        iService = readIntFieldValue("IAPService");
        iBearerType = readStringFieldValue("IAPBearerType");
        iBearer = readIntFieldValue("IAPBearer");
        iNetwork = readIntFieldValue("IAPNetwork");
        iNetworkWeighting = readIntFieldValue("IAPNetworkWeighting");
        iLocation = readIntFieldValue("Location");
    }

    protected void clearFieldValues()
    {
        iRecordId = 0;
        iRecordTag = 0;
        iRecordName = null;
        iServiceType = null;
        iService = 0;
        iBearerType = null;
        iBearer = 0;
        iNetwork = 0;
        iNetworkWeighting = 0;
        iLocation = 0;
    }
}
