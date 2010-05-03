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
* Description: TestBadBehavior
*
*/


package com.nokia.mj.impl.rms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;

public class TestBadBehavior extends TestCase
{
    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestBadBehavior("testRecordStoreNotClosed", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestBadBehavior) tc).testRecordStoreNotClosed();
            }
        }));

        suite.addTest(new TestBadBehavior("testCloseWhileTransaction", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestBadBehavior) tc).testCloseWhileTransaction();
            }
        }));

        return suite;
    }

    public TestBadBehavior() {}
    public TestBadBehavior(String aTestName, TestMethod aTestMethod)
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

    public void testRecordStoreNotClosed()
    {
        System.out.println("TestBadBehavior.testRecordStoreNotClosed()");
        String storeName = "testRecordStoreNotClosed";
        try
        {
            RecordStore store = RecordStore.openRecordStore(storeName, true);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
    }

    public void testCloseWhileTransaction()
    {
        System.out.println("TestBadBehavior.testCloseWhileTransaction()");
        String storeName = "testCloseWhileTransaction";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            Transaction transaction = new Transaction(con);
            RecordsTable records = new RecordsTable(con);
            records.create();
            records.close();

            transaction.begin();
            String sql = "INSERT INTO Records VALUES(NULL, 0, 0);";
            Statement s = con.createStatement(sql);
            s.execute();

            con.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
    }

}
