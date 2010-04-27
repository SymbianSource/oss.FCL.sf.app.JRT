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
* Description: SQLiteConnection
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

/**
 * SQLiteConnection provides connection to underlaying database.
 */
class SQLiteConnection
{
    private int iDbHandle = 0;
    public static final int DB_BUSY_TIMEOUT = 30000; // 30 secs

    public SQLiteConnection()
    {
    }

    public void open(String aName) throws RecordStoreException
    {
        iDbHandle = SQLite.sqlite3_open(aName);
        SQLite.checkHandle(iDbHandle);
        int rc = SQLite.sqlite3_busy_timeout(iDbHandle, DB_BUSY_TIMEOUT);
        SQLite.throwIfError(iDbHandle, rc);
    }

    public void close() throws RecordStoreException
    {
        SQLite.checkHandle(iDbHandle);
        int rc = SQLite.sqlite3_close(iDbHandle);
        SQLite.throwIfError(iDbHandle, rc);
        iDbHandle = 0;
    }

    public Statement createStatement(String aStatement)
    throws RecordStoreException
    {
        SQLite.checkHandle(iDbHandle);
        Statement s = new Statement(iDbHandle, aStatement);
        return s;
    }

    public PreparedStatement createPreparedStatement(String aStatement)
    throws RecordStoreException
    {
        SQLite.checkHandle(iDbHandle);
        PreparedStatement ps = new PreparedStatement(iDbHandle, aStatement);
        return ps;
    }

    public boolean isOpen()
    {
        if (iDbHandle == 0)
        {
            return false;
        }
        return true;
    }

    public int getHandle()
    {
        return iDbHandle;
    }

}

