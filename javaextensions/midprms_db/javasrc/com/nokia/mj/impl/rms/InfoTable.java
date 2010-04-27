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
* Description: InfoTable
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

/**
 * InfoTable provides access to record store details SQL table.
 * Details table has single row containing among other things:
 * name, version, last modified and next record id fields.
 */
class InfoTable
{
    private SQLiteConnection iCon;

    PreparedStatement iUpdateModified;
    PreparedStatement iUpdateModifiedAndNextRecordId;

    public InfoTable(SQLiteConnection aCon)
    {
        iCon = aCon;
    }

    public void create(RecordStoreOpenArgs aArgs) throws RecordStoreException
    {
        createTable();
        initValues(aArgs);
    }

    public void close()
    {
        closePreparedStatements();
    }

    private void createTable() throws RecordStoreException
    {
        String sql = "CREATE TABLE IF NOT EXISTS Info(format INT, name TEXT, " +
                     "lastModified LONG, version INT, authMode INT, " +
                     "writable INT, nextRecordId INT, encrypted INT, mdAlgorithm TEXT);";

        Statement s = iCon.createStatement(sql);
        s.execute();
    }

    public boolean tableExists() throws RecordStoreException
    {
        String sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='Info';";
        PreparedStatement ps = iCon.createPreparedStatement(sql);
        ResultSet rs = ps.executeQuery();
        boolean exists = rs.next();
        ps.close();
        return exists;
    }

    private void initValues(RecordStoreOpenArgs aArgs) throws RecordStoreException
    {
        String sql = "INSERT INTO Info VALUES(1, ?, 0, 1, ?, ?, 1, 0, '');";
        PreparedStatement ps = iCon.createPreparedStatement(sql);
        ps.setString(1, aArgs.getRecordStoreName().getName());
        ps.setInt(2, aArgs.getAuthmode());
        ps.setInt(3, aArgs.getWritableFlag() ? 1 : 0);
        ps.execute();
        ps.close();
    }

    private void closePreparedStatements()
    {
        closePreparedStatement(iUpdateModified);
        closePreparedStatement(iUpdateModifiedAndNextRecordId);
    }

    public void updateModified() throws RecordStoreException
    {
        iUpdateModified = initPreparedStatement(iUpdateModified,
                                                "UPDATE Info SET lastModified=?, version=version+1");
        try
        {
            iUpdateModified.setLong(1, System.currentTimeMillis());
            iUpdateModified.execute();
        }
        finally
        {
            iUpdateModified.reset();
        }
    }

    public void updateModifiedAndNextRecordId(int aNextRecordId) throws RecordStoreException
    {
        iUpdateModifiedAndNextRecordId = initPreparedStatement(iUpdateModifiedAndNextRecordId,
                                         "UPDATE Info SET nextRecordId=?, lastModified=?, version=version+1");
        try
        {
            iUpdateModifiedAndNextRecordId.setInt(1, aNextRecordId);
            iUpdateModifiedAndNextRecordId.setLong(2, System.currentTimeMillis());
            iUpdateModifiedAndNextRecordId.execute();
        }
        finally
        {
            iUpdateModifiedAndNextRecordId.reset();
        }
    }

    public RmsInfo getInfo() throws RecordStoreException
    {
        String sql = "SELECT * FROM Info";
        PreparedStatement ps = iCon.createPreparedStatement(sql);
        ResultSet rs = ps.executeQuery();
        String name = rs.getString(2);
        long lastModified = rs.getLong(3);
        int version = rs.getInt(4);
        int authMode = rs.getInt(5);
        int writable = rs.getInt(6);
        int nextRecordId = rs.getInt(7);
        int encrypted = rs.getInt(8);
        String algorithm = rs.getString(9);
        ps.close();

        RmsInfo info = new RmsInfo(name, lastModified, version, authMode,
                                   writable, nextRecordId, encrypted, algorithm);
        return info;
    }

    public void updateMode(int aAuthmode, boolean aWritable) throws RecordStoreException
    {
        String sql = "UPDATE Info SET authMode=?, writable=?";
        PreparedStatement ps = iCon.createPreparedStatement(sql);
        ps.setInt(1, aAuthmode);
        ps.setInt(2, aWritable ? 1 : 0);
        ps.execute();
        ps.close();
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

