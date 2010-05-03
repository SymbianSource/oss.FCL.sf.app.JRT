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
* Description: Statement
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;


/**
 * Statement class can be used to execute a standalone SQL statement.
 */
class Statement
{
    protected int iDbHandle;
    protected String iStatement;

    public Statement(int aDbHandle, String aStatement) throws RecordStoreException
    {
        iDbHandle = aDbHandle;
        iStatement = aStatement;
        SQLite.checkHandle(iDbHandle);
    }

    public int execute() throws RecordStoreException
    {
        SQLite.checkHandle(iDbHandle);
        int rc = SQLite.sqlite3_exec(iDbHandle, iStatement);
        SQLite.throwIfError(iDbHandle, rc);
        return rc;
    }

    public int getInsertRowId()
    {
        SQLite.checkHandle(iDbHandle);
        return (int)SQLite.sqlite3_last_insert_rowid(iDbHandle);
    }

    public int getModifiedRowCount()
    {
        SQLite.checkHandle(iDbHandle);
        return SQLite.sqlite3_changes(iDbHandle);
    }
}

