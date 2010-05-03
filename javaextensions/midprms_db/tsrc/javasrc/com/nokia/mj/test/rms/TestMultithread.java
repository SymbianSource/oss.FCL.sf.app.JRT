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
* Description:
*
*/


package com.nokia.mj.test.rms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import javax.microedition.rms.*;

/* This test set ensures that there are no deadlocks
 */
public class TestMultithread extends TestCase
{
    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestMultithread("testMultithread", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMultithread) tc).testMultithread();
            }
        }));

        return suite;
    }

    public TestMultithread() {}
    public TestMultithread(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void testMultithread()
    {
        System.out.println("TestMultithread.testMultithread()");
        RecordStore store = null;
        String rec_store_name = "testMultithread";

        class Listener implements RecordListener
        {
            public void recordAdded(RecordStore recordStore, int recordId)
            {
                try
                {
                    int num = recordStore.getNumRecords();
                    if (num > 50)
                    {
                        RecordEnumeration enumeration = recordStore.enumerateRecords(null, null, true);
                        while (enumeration.hasNextElement())
                        {
                            int id = enumeration.nextRecordId();
                            recordStore.deleteRecord(id);
                        }
                        recordStore.closeRecordStore();
                    }
                }
                catch (RecordStoreException e) {}
            }
            public void recordChanged(RecordStore recordStore, int recordId)
            {
                try
                {
                    recordStore.getNumRecords();
                }
                catch (RecordStoreException e) {}
            }
            public void recordDeleted(RecordStore recordStore, int recordId)
            {
                try
                {
                    recordStore.getNumRecords();
                }
                catch (RecordStoreException e) {}
            }
        }

        class StoreWorker extends Thread
        {
            private RecordStore iStore= null;

            public StoreWorker(RecordStore aStore)
            {
                this.iStore = aStore;
            }

            public void run()
            {
                try
                {
                    while (true)
                    {
                        int recordId = iStore.addRecord(null, 0, 0);
                        iStore.setRecord(recordId, new byte[100], 0, 100);
                    }
                }
                catch (RecordStoreException e) {}
            }
        }

        class EnumWorker extends Thread
        {
            private RecordStore iStore= null;

            public EnumWorker(RecordStore aStore)
            {
                this.iStore = aStore;
            }

            public void run()
            {
                try
                {
                    while (true)
                    {
                        RecordEnumeration enumeration = iStore.enumerateRecords(null, null, true);
                        int id = enumeration.nextRecordId();
                        iStore.setRecord(id, new byte[100], 0, 100);
                    }
                }
                catch (RecordStoreException e) {}
            }
        }

        try
        {
            // 0: Init
            System.out.println("0: Init");
            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e) {}
            store = RecordStore.openRecordStore(rec_store_name, true);

            store.addRecordListener(new Listener());
            store.addRecord(null, 0, 0);

            // 1: create worker threads
            System.out.println("1: create worker threads");

            StoreWorker s1 = new StoreWorker(store);
            StoreWorker s2 = new StoreWorker(store);
            StoreWorker s3 = new StoreWorker(store);
            StoreWorker s4 = new StoreWorker(store);
            StoreWorker s5 = new StoreWorker(store);

            EnumWorker e1 = new EnumWorker(store);
            EnumWorker e2 = new EnumWorker(store);
            EnumWorker e3 = new EnumWorker(store);
            EnumWorker e4 = new EnumWorker(store);
            EnumWorker e5 = new EnumWorker(store);

            e1.start();
            e2.start();
            e3.start();
            e4.start();
            e5.start();
            s1.start();
            s2.start();
            s3.start();
            s4.start();
            s5.start();

            e1.join();
            e2.join();
            e3.join();
            e4.join();
            e5.join();
            s1.join();
            s2.join();
            s3.join();
            s4.join();
            s5.join();

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
        finally
        {
            try
            {
                store.closeRecordStore();
            }
            catch (Exception e) {}
            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e) {}
        }
    }


}
