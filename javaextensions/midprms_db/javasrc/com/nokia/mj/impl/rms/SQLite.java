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
* Description: SQLite wrapper
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * SQLite is a wrapper to SQLite native API.
 */
class SQLite
{
    static
    {
        Jvm.loadSystemLibrary("javarms");
    }

    public static final void throwIfError(int aDbHandle, int aErrorCode)
    throws RecordStoreException
    {
        checkHandle(aDbHandle);
        if (aErrorCode != SQLite.SQLITE_OK)
        {
            String message = SQLite.sqlite3_errmsg(aDbHandle);
            throw new RecordStoreException(message + ": " + aErrorCode);
        }
    }

    public static final void checkHandle(int aHandle)
    {
        if (aHandle == 0)
        {
            throw new NullPointerException("Invalid handle");
        }
    }

    public static final int SQLITE_OK = 0;
    public static final int SQLITE_DONE = 101;
    public static final int SQLITE_ROW = 100;

    public static native int sqlite3_open(String aFilename);
    public static native int sqlite3_close(int aDbHandle);

    public static native int sqlite3_exec(int aDbHandle, String aStatement);
    public static native int sqlite3_prepare_v2(int aDbHandle, String aStatement);
    public static native int sqlite3_step(int aStmtHandle);
    public static native int sqlite3_reset(int aStmtHandle);
    public static native int sqlite3_finalize(int aStmtHandle);
    public static native int sqlite3_changes(int aDbHandle);

    public static native String sqlite3_errmsg(int aDbHandle);

    public static native int sqlite3_bind_blob(int aStmtHandle, int aIndex, byte[] aBytes);
    public static native int sqlite3_bind_int(int aStmtHandle, int aIndex, int aInt);
    public static native int sqlite3_bind_int64(int aStmtHandle, int aIndex, long aLong);
    public static native int sqlite3_bind_text(int aStmtHandle, int aIndex, String aText);

    public static native byte[] sqlite3_column_blob(int aStmtHandle, int aCol);
    public static native int sqlite3_column_int(int aStmtHandle, int aCol);
    public static native long sqlite3_column_int64(int aStmtHandle, int aCol);
    public static native String sqlite3_column_text(int aStmtHandle, int aCol);

    public static native long sqlite3_last_insert_rowid(int aDbHandle);
    public static native int sqlite3_busy_timeout(int aDbHandle, int aMilliseconds);
}

