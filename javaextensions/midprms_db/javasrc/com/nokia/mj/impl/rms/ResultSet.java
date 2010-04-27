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
* Description: ResultSet
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

/**
 * ResultSet is used to handle SQL query results.
 */
class ResultSet
{
    private int iStmtHandle;
    private boolean iHasRow;
    private int iRowIndex;

    private static final int INVALID_INDEX = -1;

    public ResultSet(int aStmtHandle, int aHasRow)
    {
        iStmtHandle = aStmtHandle;
        setHasRow(aHasRow);
        iRowIndex = INVALID_INDEX;
    }

    public byte[] getBytes(int aColumnIndex) throws RecordStoreException
    {
        throwIfNotValidRow();
        byte[] rc = SQLite.sqlite3_column_blob(iStmtHandle, aColumnIndex-1);
        return rc;
    }

    public int getInt(int aColumnIndex) throws RecordStoreException
    {
        throwIfNotValidRow();
        int rc = SQLite.sqlite3_column_int(iStmtHandle, aColumnIndex-1);
        return rc;
    }

    public long getLong(int aColumnIndex) throws RecordStoreException
    {
        throwIfNotValidRow();
        long rc = SQLite.sqlite3_column_int64(iStmtHandle, aColumnIndex-1);
        return rc;
    }

    public String getString(int aColumnIndex) throws RecordStoreException
    {
        throwIfNotValidRow();
        String rc = SQLite.sqlite3_column_text(iStmtHandle, aColumnIndex-1);
        return rc;
    }

    public boolean next() throws RecordStoreException
    {
        if (iRowIndex != INVALID_INDEX)
        {
            int rc = SQLite.sqlite3_step(iStmtHandle);
            if (rc != SQLite.SQLITE_DONE && rc != SQLite.SQLITE_ROW)
            {
                throw new RecordStoreException("ResultSet.next() failed:"+rc);
            }
            setHasRow(rc);
        }
        iRowIndex++;
        return iHasRow;

    }

    private void setHasRow(int aResult)
    {
        if (aResult == SQLite.SQLITE_ROW)
        {
            iHasRow = true;
        }
        else
        {
            iHasRow = false;
        }
    }

    private void throwIfNotValidRow() throws RecordStoreException
    {
        if (!iHasRow)
        {
            throw new RecordStoreException("ResultSet: no rows left");
        }
    }

}

