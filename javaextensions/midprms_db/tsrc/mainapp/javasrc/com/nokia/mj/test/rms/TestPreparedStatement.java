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
* Description: TestPreparedStatement
*
*/


package com.nokia.mj.impl.rms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;

public class TestPreparedStatement extends TestCase
{

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestPreparedStatement("testReusePreparedStatement", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPreparedStatement) tc).testReusePreparedStatement();
            }
        }));

        suite.addTest(new TestPreparedStatement("testReusePreparedStatementWithResultSet", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPreparedStatement) tc).testReusePreparedStatementWithResultSet();
            }
        }));

        suite.addTest(new TestPreparedStatement("testTableExists", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPreparedStatement) tc).testTableExists();
            }
        }));

        return suite;
    }

    public TestPreparedStatement() {}
    public TestPreparedStatement(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    SQLiteConnection openConnection(String aName) throws RecordStoreException
    {
        RmsName storeName = new RmsName(aName);
        String filename = RmsFileManager.getFilename(storeName);

        SQLiteConnection con = new SQLiteConnection();
        con.open(filename);
        return con;
    }

    void deleteStore(String aName)
    {
        try
        {
            RmsName storeName = new RmsName(aName);
            RmsFileManager.deleteRecordStore(storeName);
        }
        catch (Exception e)
        {
        }
    }

    public void testReusePreparedStatement()
    {
        System.out.println("TestPreparedStatement.testReusePreparedStatement()");
        String storeName = "testReusePreparedStatement";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            RecordsTable records = new RecordsTable(con);
            records.create();

            String sql = "INSERT INTO Records VALUES(NULL, ?, ?);";
            PreparedStatement ps = con.createPreparedStatement(sql);

            for (int i = 0; i < 10; i++)
            {
                ps.setBytes(1, "hello world".getBytes());
                ps.setInt(2, 0);
                ps.execute();
                ps.reset();
            }
            ps.close();

            assertEquals(records.getNumRecords(), 10);
            records.close();
            con.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
        finally
        {
            deleteStore(storeName);
        }
    }

    public void testReusePreparedStatementWithResultSet()
    {
        System.out.println("TestPreparedStatement.testReusePreparedStatementWithResultSet()");
        String storeName = "testReusePreparedStatementWithResultSet";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            RecordsTable records = new RecordsTable(con);
            records.create();
            int recordId = records.addRecord(new Record(1));

            String sql = "SELECT * FROM Records WHERE id=?;";
            PreparedStatement ps = con.createPreparedStatement(sql);

            for (int i = 0; i < 10; i++)
            {
                ps.setInt(1, recordId);
                ResultSet rs = ps.executeQuery();
                int id = rs.getInt(1);
                ps.reset();
                assertEquals(id, recordId);
            }
            ps.close();

            records.close();
            con.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
        finally
        {
            deleteStore(storeName);
        }
    }

    public void testTableExists()
    {
        System.out.println("TestPreparedStatement.testTableExists()");
        String storeName = "testTableExists";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            InfoTable info = new InfoTable(con);

            boolean exists = info.tableExists();
            assertTrue(!exists);

            info.create(new RecordStoreOpenArgs("", "", "", ""));
            exists = info.tableExists();
            assertTrue(exists);

            info.close();
            con.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
        finally
        {
            deleteStore(storeName);
        }
    }
}
