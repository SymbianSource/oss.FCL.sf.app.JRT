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
* Description: TestTransaction
*
*/


package com.nokia.mj.impl.rms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import javax.microedition.rms.*;

public class TestTransaction extends TestCase
{

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestTransaction("testReadDuringTransaction", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction) tc).testReadDuringTransaction();
            }
        }));

        suite.addTest(new TestTransaction("testWriteDuringTransaction", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction) tc).testWriteDuringTransaction();
            }
        }));

        suite.addTest(new TestTransaction("testBusyTimeout", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction) tc).testBusyTimeout();
            }
        }));

        suite.addTest(new TestTransaction("testMultipleTransactions", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction) tc).testMultipleTransactions();
            }
        }));

        return suite;
    }

    public TestTransaction() {}
    public TestTransaction(String aTestName, TestMethod aTestMethod)
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

    void doWait(int aTimeout)
    {
        try
        {
            Thread.sleep(aTimeout);
        }
        catch (InterruptedException e)
        {
        }
    }


    class RecordsWriter extends Thread
    {
        private String iName;
        private int iTimeout;
        private int iTransactionCount;
        private boolean iSuccess = true;

        public RecordsWriter(String aName, int aTimeout)
        {
            this(aName, aTimeout, 1);
        }

        public RecordsWriter(String aName, int aTimeout, int aTransactionCount)
        {
            iName = aName;
            iTimeout = aTimeout;
            iTransactionCount = aTransactionCount;
        }

        public void run()
        {
            try
            {
                SQLiteConnection con = openConnection(iName);
                Transaction transaction = new Transaction(con);
                RecordsTable records = new RecordsTable(con);
                records.create();

                for (int i = 0; i < iTransactionCount; i++)
                {
                    doWait(iTimeout);
                    transaction.begin();
                    byte[] b = "hello world".getBytes();
                    records.addRecord(new Record(b, 0, b.length, 0));
                    doWait(iTimeout);
                    transaction.commit();
                }
                records.close();
                con.close();
            }
            catch (Exception e)
            {
                e.printStackTrace();
                iSuccess = false;
            }
        }
        public boolean isSuccess()
        {
            return iSuccess;
        }
    }

    class RecordsReader
    {
        private String iName;
        private boolean iSuccess = true;
        private RecordStore iStore;
        private RecordEnumeration iEnumeration;

        public RecordsReader(String aName)
        {
            iName = aName;
        }

        public void start()
        {
            try
            {
                iStore = RecordStore.openRecordStore(iName, true);
                // keepUpdated = true --> enum makes a get operation for each RecordAdded event
                iEnumeration = iStore.enumerateRecords(null, null, true);
            }
            catch (Exception e)
            {
                e.printStackTrace();
                iSuccess = false;
            }
        }

        public void stop()
        {
            try
            {
                iEnumeration.destroy();
                iStore.closeRecordStore();
            }
            catch (Exception e)
            {
                e.printStackTrace();
                iSuccess = false;
            }
        }

        public boolean isSuccess()
        {
            return iSuccess;
        }
    }



    public void testReadDuringTransaction()
    {
        System.out.println("TestTransaction.testReadDuringTransaction()");
        String storeName = "testReadDuringTransaction";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            RecordsTable records = new RecordsTable(con);
            records.create();

            RecordsWriter rw = new RecordsWriter(storeName, 400);
            rw.start();

            doWait(200);
            assertEquals(records.getNumRecords(), 0); // read before transaction
            doWait(400);
            assertEquals(records.getNumRecords(), 0); // read while transaction
            rw.join();
            assertEquals(records.getNumRecords(), 1); // read after transaction
            assertTrue(rw.isSuccess());

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

    public void testWriteDuringTransaction()
    {
        System.out.println("TestTransaction.testWriteDuringTransaction()");
        String storeName = "testWriteDuringTransaction";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            RecordsTable records = new RecordsTable(con);
            records.create();

            RecordsWriter rw = new RecordsWriter(storeName, 400);
            rw.start();

            doWait(200);
            records.addRecord(new Record(1)); // write before transaction
            doWait(400);
            records.addRecord(new Record(1)); // write during transaction
            rw.join();
            records.addRecord(new Record(1)); // write after transaction
            assertEquals(records.getNumRecords(), 4);

            records.close();
            con.close();
            assertTrue(rw.isSuccess());
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

    public void testBusyTimeout()
    {
        System.out.println("TestTransaction.testBusyTimeout()");
        String storeName = "testBusyTimeout";

        class BusyTester extends Thread
        {
            private String iName;
            private int iTimeout;
            private boolean iSuccess = true;

            public BusyTester(String aName, int aTimeout)
            {
                iName = aName;
                iTimeout = aTimeout;
            }
            public void run()
            {
                try
                {
                    SQLiteConnection con = openConnection(iName);
                    Transaction transaction = new Transaction(con);
                    RecordsTable records = new RecordsTable(con);
                    records.create();

                    transaction.begin();
                    byte[] b = "hello world".getBytes();
                    records.addRecord(new Record(b, 0, b.length, 0));
                    doWait(iTimeout);
                    transaction.commit();

                    records.close();
                    con.close();
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                    iSuccess = false;
                }
            }
            public boolean isSuccess()
            {
                return iSuccess;
            }
        }

        try
        {
            SQLiteConnection con = openConnection(storeName);
            RecordsTable records = new RecordsTable(con);
            records.create();

            // set shorter timeout in unit test
            int DB_BUSY_TIMEOUT = 100;
            int rc = SQLite.sqlite3_busy_timeout(con.getHandle(), DB_BUSY_TIMEOUT);
            SQLite.throwIfError(con.getHandle(), rc);

            BusyTester rw = new BusyTester(storeName, DB_BUSY_TIMEOUT*10);
            rw.start();

            doWait(DB_BUSY_TIMEOUT);
            try
            {
                records.addRecord(new Record(1)); // write during transaction
                fail("expected busy timeout");
            }
            catch (RecordStoreException e)
            {
            }
            rw.join();
            assertEquals(records.getNumRecords(), 1);

            records.close();
            con.close();
            assertTrue(rw.isSuccess());
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

    public void testMultipleTransactions()
    {
        System.out.println("TestTransaction.testMultipleTransactions()");
        String storeName = "testMultipleTransactions";
        try
        {
            SQLiteConnection con = openConnection(storeName);
            RecordsTable records = new RecordsTable(con);
            Transaction transaction = new Transaction(con);
            records.create();

            RecordsWriter[] writers = new RecordsWriter[10];
            RecordsReader[] readers = new RecordsReader[10];

            for (int i = 0; i < writers.length; i++)
            {
                writers[i] = new RecordsWriter(storeName, 0, 100);
                readers[i] = new RecordsReader(storeName);
                readers[i].start();
            }

            // block writers
            transaction.begin();
            records.addRecord(new Record(1));
            for (int i = 0; i < writers.length; i++)
            {
                writers[i].start();
            }
            transaction.commit();

            for (int i = 0; i < writers.length; i++)
            {
                writers[i].join();
                assertTrue(writers[i].isSuccess());
            }

            for (int i = 0; i < readers.length; i++)
            {
                readers[i].stop();
                assertTrue(readers[i].isSuccess());
            }

            assertEquals(records.getNumRecords(), 1001);
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

}
