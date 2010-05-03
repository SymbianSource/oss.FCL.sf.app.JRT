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
* Description: Transaction
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

/**
 * Transaction is a helper class for transaction related operations.
 * This class can be used to toggle autocommit mode off in SQLite
 */
class Transaction
{
    private SQLiteConnection iCon;

    public Transaction(SQLiteConnection aCon)
    {
        iCon = aCon;
    }

    public void begin() throws RecordStoreException
    {
        String statement = "BEGIN IMMEDIATE TRANSACTION;";
        execute(statement);
    }

    public void commit() throws RecordStoreException
    {
        String statement = "COMMIT;";
        execute(statement);
    }

    public void rollback() throws RecordStoreException
    {
        String statement = "ROLLBACK;";
        execute(statement);
    }

    private void execute(String aStatement) throws RecordStoreException
    {
        Statement s = iCon.createStatement(aStatement);
        s.execute();
    }

}

