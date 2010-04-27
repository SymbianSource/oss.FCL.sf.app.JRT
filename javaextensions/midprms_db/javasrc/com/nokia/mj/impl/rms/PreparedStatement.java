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
* Description: PreparedStatement
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import com.nokia.mj.impl.utils.Logger;
/**
 *
 */
class PreparedStatement extends Statement
{
    private int iStmtHandle = 0;

    public PreparedStatement(int aDbHandle, String aStatement)
    throws RecordStoreException
    {
        super(aDbHandle, aStatement);

        iStmtHandle = SQLite.sqlite3_prepare_v2(aDbHandle, aStatement);
        if (iStmtHandle == 0)
        {
            String message = SQLite.sqlite3_errmsg(iDbHandle);
            throw new RecordStoreException(message);
        }
    }

    public int execute() throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = SQLite.sqlite3_step(iStmtHandle);
        if (rc != SQLite.SQLITE_DONE && rc != SQLite.SQLITE_ROW)
        {
            String message = SQLite.sqlite3_errmsg(iDbHandle);
            Logger.WLOG(Logger.EMidpRms, "PreparedStatement.execute() failed:"
                        + message + ": " + rc + " sql:" + iStatement);
            throw new RecordStoreException(message + ": " + rc);
        }
        return rc;
    }

    public ResultSet executeQuery() throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = execute();
        ResultSet rs = new ResultSet(iStmtHandle, rc);
        return rs;
    }

    public void close()
    {
        SQLite.checkHandle(iStmtHandle);
        SQLite.sqlite3_finalize(iStmtHandle);
        iStmtHandle = 0;
    }

    public void setBytes(int aColumnIndex, byte[] aBytes) throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = SQLite.sqlite3_bind_blob(iStmtHandle, aColumnIndex, aBytes);
        SQLite.throwIfError(iDbHandle, rc);
    }

    public void setInt(int aColumnIndex, int aValue) throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = SQLite.sqlite3_bind_int(iStmtHandle, aColumnIndex, aValue);
        SQLite.throwIfError(iDbHandle, rc);
    }

    public void setLong(int aColumnIndex, long aValue) throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = SQLite.sqlite3_bind_int64(iStmtHandle, aColumnIndex, aValue);
        SQLite.throwIfError(iDbHandle, rc);
    }

    public void setString(int aColumnIndex, String aValue) throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = SQLite.sqlite3_bind_text(iStmtHandle, aColumnIndex, aValue);
        SQLite.throwIfError(iDbHandle, rc);
    }

    public void reset() throws RecordStoreException
    {
        SQLite.checkHandle(iStmtHandle);
        int rc = SQLite.sqlite3_reset(iStmtHandle);
        SQLite.throwIfError(iDbHandle, rc);
    }

}

