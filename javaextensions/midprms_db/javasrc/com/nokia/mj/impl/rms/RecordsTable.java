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
* Description: RecordsTable
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import java.util.Vector;

/**
 * RecordsTable provides access to Records SQL table.
 * Each record has id, data and tag columns.
 */
class RecordsTable
{
    private SQLiteConnection iCon;
    PreparedStatement iAddRecord;
    PreparedStatement iGetRecord;
    PreparedStatement iSetRecord;
    PreparedStatement iDeleteRecord;
    PreparedStatement iGetNumRecords;

    public RecordsTable(SQLiteConnection aCon)
    {
        iCon = aCon;
    }

    public void create() throws RecordStoreException
    {
        createTable();
    }

    public void close()
    {
        closePreparedStatements();
    }

    private void createTable() throws RecordStoreException
    {
        String sql = "CREATE TABLE IF NOT EXISTS Records(id INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB, tag INT);";
        Statement s = iCon.createStatement(sql);
        s.execute();
    }

    private void closePreparedStatements()
    {
        closePreparedStatement(iAddRecord);
        closePreparedStatement(iGetRecord);
        closePreparedStatement(iSetRecord);
        closePreparedStatement(iDeleteRecord);
        closePreparedStatement(iGetNumRecords);
    }

    public int addRecord(Record aRecord) throws RecordStoreException
    {
        iAddRecord = initPreparedStatement(iAddRecord, "INSERT INTO Records VALUES(NULL, ?, ?);");
        try
        {
            iAddRecord.setBytes(1, aRecord.getData());
            iAddRecord.setInt(2, aRecord.getTag());
            iAddRecord.execute();
            int recordId = iAddRecord.getInsertRowId();
            return recordId;
        }
        finally
        {
            iAddRecord.reset();
        }
    }

    public Record getRecord(Record aRecord) throws RecordStoreException, InvalidRecordIDException
    {
        iGetRecord = initPreparedStatement(iGetRecord, "SELECT * FROM Records WHERE id=?;");
        try
        {
            iGetRecord.setInt(1, aRecord.getId());
            ResultSet rs = iGetRecord.executeQuery();

            boolean recordFound = rs.next();
            if (recordFound)
            {
                byte[] bytes = rs.getBytes(2);
                if (bytes.length == 0)
                {
                    bytes = null;
                }
                int tag = rs.getInt(3);
                Record record = new Record(aRecord.getId(), bytes, tag);
                return record;
            }
            else
            {
                throw new InvalidRecordIDException(RmsErrorStrings.INVALID_ID + aRecord.getId());
            }
        }
        finally
        {
            iGetRecord.reset();
        }
    }

    public void setRecord(Record aRecord) throws RecordStoreException, InvalidRecordIDException
    {
        iSetRecord = initPreparedStatement(iSetRecord, "UPDATE Records SET data=?, tag=? WHERE id=?;");
        try
        {
            iSetRecord.setBytes(1, aRecord.getData());
            iSetRecord.setInt(2, aRecord.getTag());
            iSetRecord.setInt(3, aRecord.getId());
            iSetRecord.execute();
            int changedRows = iSetRecord.getModifiedRowCount();

            if (changedRows != 1)
            {
                throw new InvalidRecordIDException(RmsErrorStrings.INVALID_ID + aRecord.getId());
            }
        }
        finally
        {
            iSetRecord.reset();
        }
    }

    public void deleteRecord(Record aRecord) throws RecordStoreException, InvalidRecordIDException
    {
        iDeleteRecord = initPreparedStatement(iDeleteRecord, "DELETE FROM Records WHERE id=?;");
        try
        {
            iDeleteRecord.setInt(1, aRecord.getId());
            iDeleteRecord.execute();
            int changedRows = iDeleteRecord.getModifiedRowCount();
            if (changedRows != 1)
            {
                throw new InvalidRecordIDException(RmsErrorStrings.INVALID_ID + aRecord.getId());
            }
        }
        finally
        {
            iDeleteRecord.reset();
        }
    }

    public int getNumRecords() throws RecordStoreException
    {
        iGetNumRecords = initPreparedStatement(iGetNumRecords, "SELECT COUNT(*) FROM Records;");
        try
        {
            ResultSet rs = iGetNumRecords.executeQuery();
            int recordCount = rs.getInt(1);
            return recordCount;
        }
        finally
        {
            iGetNumRecords.reset();
        }
    }

    public int[] getRecordIds() throws RecordStoreException
    {
        String sql = "SELECT id FROM Records;";
        PreparedStatement ps = iCon.createPreparedStatement(sql);
        ResultSet rs = ps.executeQuery();
        Vector ids = new Vector();
        while (rs.next())
        {
            int id = rs.getInt(1);
            ids.addElement(new Integer(id));
        }
        ps.close();

        int size = ids.size();
        int[] result = new int[size];
        for (int i = 0; i < size; i++)
        {
            Integer val = (Integer)ids.elementAt(i);
            result[i] = val.intValue();
        }
        return result;
    }

    private PreparedStatement initPreparedStatement(PreparedStatement aStatement, String aSql) throws RecordStoreException
    {
        if (aStatement == null)
        {
            return iCon.createPreparedStatement(aSql);
        }
        return aStatement;
    }

    private void closePreparedStatement(PreparedStatement aStatement)
    {
        if (aStatement != null)
        {
            aStatement.close();
        }
    }

}

