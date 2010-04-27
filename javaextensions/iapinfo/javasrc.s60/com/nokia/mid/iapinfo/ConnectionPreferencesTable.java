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
* Description:  Provides access for ConnectionPreferences CommDB table.
 *
*/

package com.nokia.mid.iapinfo;

/**
 * This class provides access for ConnectionPreferences CommDB table. Public
 * member variables of the class contains data of the current record.
 *
 * @see com.nokia.mid.commdb.CommsTable
 *
 */
final class ConnectionPreferencesTable extends CommsTable
{

    /**
     * Ranking of this connection preference.
     */
    public int iRanking;

    /**
     * Enum indicating the direction of the connection.
     */
    public int iDirection;

    /**
     * Bitwise OR-ed value representing the bearers which can be used for this
     * connection.
     */
    public int iBearerSet;

    /**
     * Enum indicating whether or not the user should be prompted or warned on
     * connection.
     */
    public int iDialogPref;

    /**
     * Identifier of preferred IAP to use.
     */
    public int iDefaultIAP;

    /**
     * Public constructor.
     */
    public ConnectionPreferencesTable() throws CommDBException
    {
        super(CommsTable.TABLE_CONNECTION_PREFERENCES);
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
        iRanking = readIntFieldValue("Ranking");
        iDirection = readIntFieldValue("Direction");
        iBearerSet = readIntFieldValue("BearerSet");
        iDialogPref = readIntFieldValue("DialogPref");
        iDefaultIAP = readIntFieldValue("IAP");
    }

    protected void clearFieldValues()
    {
        iRecordId = 0;
        iRecordTag = 0;
        iRecordName = null;
        iRanking = 0;
        iDirection = 0;
        iBearerSet = 0;
        iDialogPref = 0;
        iDefaultIAP = 0;
    }
}
