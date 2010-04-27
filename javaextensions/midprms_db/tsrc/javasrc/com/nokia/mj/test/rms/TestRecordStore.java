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

/* Test cases for verifying that functionality is according to MIDP2.1 spec
 * These tests will verify that argument validation is done correctly,
 * correct exceptions are thrown and behavior follows the spec
 */
public class TestRecordStore extends TestCase
{
    // keep these in sync with rmstests.jad / build.xml file
    String iVendorName = "Nokia";
    String iSuiteName = "RmsUnitTests";

    public TestRecordStore() {}

    public TestRecordStore(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestRecordStore("testAddRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testAddRecord();
            }
        }));

        aSuite.addTest(new TestRecordStore("testAddRecordListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testAddRecordListener();
            }
        }));

        aSuite.addTest(new TestRecordStore("testCloseRecordStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testCloseRecordStore();
            }
        }));

        aSuite.addTest(new TestRecordStore("testDeleteRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testDeleteRecord();
            }
        }));

        aSuite.addTest(new TestRecordStore("testDeleteRecordStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testDeleteRecordStore();
            }
        }));

        aSuite.addTest(new TestRecordStore("testEnumerateRecords", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testEnumerateRecords();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetLastModified", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetLastModified();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetName", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetName();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetNextRecordID", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetNextRecordID();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetNumRecords", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetNumRecords();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetRecord();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetRecord2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetRecord2();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetRecordSize", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetRecordSize();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetSize", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetSize();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetSizeAvailable", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetSizeAvailable();
            }
        }));

        aSuite.addTest(new TestRecordStore("testGetVersion", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testGetVersion();
            }
        }));

        aSuite.addTest(new TestRecordStore("testListRecordStores", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testListRecordStores();
            }
        }));

        aSuite.addTest(new TestRecordStore("testOpenRecordStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testOpenRecordStore();
            }
        }));

        aSuite.addTest(new TestRecordStore("testOpenRecordStore2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testOpenRecordStore2();
            }
        }));

        aSuite.addTest(new TestRecordStore("testOpenRecordStore3", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testOpenRecordStore3();
            }
        }));

        aSuite.addTest(new TestRecordStore("testRemoveRecordListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testRemoveRecordListener();
            }
        }));

        aSuite.addTest(new TestRecordStore("testSetMode", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testSetMode();
            }
        }));

        aSuite.addTest(new TestRecordStore("testSetRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testSetRecord();
            }
        }));

        aSuite.addTest(new TestRecordStore("testRecordStoreExceptions", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordStore) tc).testRecordStoreExceptions();
            }
        }));

        return aSuite;
    }

    public void assertEquals(byte[] a, byte[] b)
    {
        assertEquals(a.length, b.length);
        for (int i = 0; i < a.length; i++)
        {
            assertEquals(a[i], b[i]);
        }
    }

    public void testAddRecord()
    {
        System.out.println("TestRecordStore.testAddRecord()");
        RecordStore store = null;
        String rec_store_name = "addRecord";

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

            // 1: Add 1st record
            System.out.println("1: Add 1st record");
            byte data[] = {1, 2, 3, 15};
            int recordId = store.addRecord(data, 0, data.length);
            assertEquals(recordId, 1);

            // 2: Add zero-length record (null)
            System.out.println("2: Add zero-length record (null)");
            recordId = store.addRecord(null, 0, 0);
            assertEquals(recordId, 2);
            assertTrue(store.getRecord(recordId) == null);

            // 3: Add zero-length record (len=0)
            System.out.println("3: Add zero-length record (len=0)");
            recordId = store.addRecord(data, 0, 0);
            assertEquals(recordId, 3);
            assertTrue(store.getRecord(recordId) == null);

            // 4: record store is not open
            System.out.println("4: record store is not open");
            store.closeRecordStore();
            try
            {
                store.addRecord(data, 0, data.length);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re)
            {
            }
            store = RecordStore.openRecordStore(rec_store_name, true);

            // 5: record store full
            System.out.println("5: record store full");
            try
            {
                byte[] oneMB = new byte[1000000];
                // assumed max limit < 50MB
                for (int i = 0; i < 50; i++)
                {
                    store.addRecord(oneMB, 0, oneMB.length);
                }
                fail("RecordStoreFullException not thrown");
            }
            catch (RecordStoreFullException re)
            {
            }

            // Add case for RecordStoreException
            // Add case for SecurityException
        }
        catch (Exception e)
        {
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

    public void testAddRecordListener()
    {
        System.out.println("TestRecordStore.testAddRecordListener()");
        RecordStore store = null;
        String rec_store_name = "addRecordListener";

        class Listener implements RecordListener
        {
            public int added = 0;
            public int changed = 0;
            public int deleted = 0;

            public void recordAdded(RecordStore recordStore, int recordId)
            {
                added++;
            }
            public void recordChanged(RecordStore recordStore, int recordId)
            {
                changed++;
            }
            public void recordDeleted(RecordStore recordStore, int recordId)
            {
                deleted++;
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

            // 1: Add listener
            System.out.println("1: Add listener");
            Listener listener = new Listener();
            store.addRecordListener(listener);
            byte data[] = {1, 2, 3, 15};
            store.addRecord(data, 0, data.length);
            assertEquals(listener.added, 1);

            // 2: Add 2nd listener
            System.out.println("2: Add 2nd listener");
            Listener listener2 = new Listener();
            store.addRecordListener(listener2);
            store.addRecord(data, 0, data.length);
            assertEquals(listener.added, 2);
            assertEquals(listener2.added, 1);

            // 3: Add same listener
            System.out.println("3: Add same listener");
            store.addRecordListener(listener2);
            store.addRecord(data, 0, data.length);
            assertEquals(listener.added, 3);
            assertEquals(listener2.added, 2);

            // 4: Listeners removed when store is closed
            System.out.println("4: Listeners removed when store is closed");
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, true);
            store.addRecord(data, 0, data.length);
            assertEquals(listener.added, 3);
            assertEquals(listener2.added, 2);

            // 5: Listener added while closed
            System.out.println("5: Listener added while closed");
            RecordStore store2 = RecordStore.openRecordStore(rec_store_name, true);
            store2.closeRecordStore();
            Listener listener3 = new Listener();
            store2.addRecordListener(listener3);
            store2 = RecordStore.openRecordStore(rec_store_name, true);
            store2.addRecord(data, 0, data.length);
            assertEquals(listener3.added, 1);
            store2.closeRecordStore();

        }
        catch (Exception e)
        {
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

    public void testCloseRecordStore()
    {
        System.out.println("TestRecordStore.testCloseRecordStore()");
        RecordStore store = null;
        String rec_store_name = "closeRecordStore";

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

            // 1: Close record store
            System.out.println("1: Close record store");
            store.closeRecordStore();

            // 2: Close record store when not open
            System.out.println("2: Close record store when not open");
            try
            {
                store.closeRecordStore();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            // 3: Calling methods when not open
            System.out.println("3: Calling methods when not open");
            try
            {
                store.getName();
                fail("RecordStoreNotOpenException not thrown getName()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getVersion();
                fail("RecordStoreNotOpenException not thrown getVersion()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getNumRecords();
                fail("RecordStoreNotOpenException not thrown getNumRecords()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getSize();
                fail("RecordStoreNotOpenException not thrown getSize()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getSizeAvailable();
                fail("RecordStoreNotOpenException not thrown getSizeAvailable()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getLastModified();
                fail("RecordStoreNotOpenException not thrown getLastModified()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getNextRecordID();
                fail("RecordStoreNotOpenException not thrown getNextRecordID()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.addRecord(new byte[1], 0, 1);
                fail("RecordStoreNotOpenException not thrown addRecord()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.deleteRecord(1);
                fail("RecordStoreNotOpenException not thrown deleteRecord()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getRecordSize(1);
                fail("RecordStoreNotOpenException not thrown getRecordSize()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                byte[] data = null;
                store.getRecord(1, data, 0);
                fail("RecordStoreNotOpenException not thrown getRecord(buf)");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.getRecord(1);
                fail("RecordStoreNotOpenException not thrown getRecord()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.setRecord(1, new byte[1], 0, 1);
                fail("RecordStoreNotOpenException not thrown setRecord()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                store.enumerateRecords(null, null, false);
                fail("RecordStoreNotOpenException not thrown enumerateRecords()");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
            // 4: Open and Close counts must match
            System.out.println("4: Open and Close counts must match");
            RecordStore store2 = RecordStore.openRecordStore(rec_store_name, true);
            store2.closeRecordStore();
            // open (2) - close (1)
            store2.getName();
            store2.closeRecordStore();
            // open (2) - close (2)
            try
            {
                store2.getName();
                fail("RecordStoreNotOpenException not thrown getName()");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);

            // Add case for RecordStoreException
        }
        catch (Exception e)
        {
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

    public void testDeleteRecord()
    {
        System.out.println("TestRecordStore.testDeleteRecord()");
        RecordStore store = null;
        String rec_store_name = "deleteRecord";

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

            // 1: Delete record
            System.out.println("1: Delete record");
            int recordId = store.addRecord(null, 0, 0);
            assertEquals(recordId, 1);
            store.deleteRecord(recordId);

            // 2: Check recordId is not reused
            System.out.println("2: Check recordId is not reused");
            recordId = store.addRecord(null, 0, 0);
            assertEquals(recordId, 2);
            store.deleteRecord(recordId);

            // 3: The recordId is invalid
            System.out.println("3: The recordId is invalid");
            try
            {
                store.deleteRecord(2);
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                store.deleteRecord(0);
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                store.deleteRecord(-1);
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                store.deleteRecord(3);
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re) {}

            // 4: deleteRecord when not open
            System.out.println("4: deleteRecord when not open");
            try
            {
                recordId = store.addRecord(null, 0, 0);
                store.closeRecordStore();
                store.deleteRecord(recordId);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            // Add case for RecordStoreException
            // Add case for SecurityException
            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testDeleteRecordStore()
    {
        System.out.println("TestRecordStore.testDeleteRecordStore()");
        RecordStore store = null;
        String rec_store_name = "deleteRecordStore";

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

            // 1: Delete record store
            System.out.println("1: Delete record store");
            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
            try
            {
                store = RecordStore.openRecordStore(rec_store_name, false);
                fail("openRecordStore succeeded (do not create flag)");
            }
            catch (RecordStoreNotFoundException re) {}

            // 2: Delete record store while open
            System.out.println("2: Delete record store while open");
            store = RecordStore.openRecordStore(rec_store_name, true);
            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
                fail("RecordStoreException not thrown");
            }
            catch (RecordStoreException re) {}

            // 3: Deleting not existing record store
            System.out.println("3: Deleting not existing record store");
            try
            {
                RecordStore.deleteRecordStore("doesNotExist");
                fail("RecordStoreNotFoundException not thrown");
            }
            catch (RecordStoreNotFoundException re) {}

            // Add case for shared record store
        }
        catch (Exception e)
        {
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

    public void testEnumerateRecords()
    {
        System.out.println("TestRecordStore.testEnumerateRecords()");
        RecordStore store = null;
        String rec_store_name = "enumerateRecords";

        class filter implements RecordFilter
        {
            public boolean matches(byte[] candidate)
            {
                if (candidate[0] > 25)
                    return true;
                else
                    return false;
            }
        }

        class comparator implements RecordComparator
        {
            public int compare(byte[] rec1, byte[] rec2)
            {
                if (rec1[0] < rec2[0])
                    return RecordComparator.PRECEDES;
                else if (rec1[0] > rec2[0])
                    return RecordComparator.FOLLOWS;
                else
                    return RecordComparator.EQUIVALENT;
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

            // 1: Filter and Comparator null
            System.out.println("1: Filter and Comparator null");
            RecordEnumeration rec_enum = store.enumerateRecords(null, null, true);
            assertNotNull(rec_enum);
            assertEquals(rec_enum.numRecords(), 0);

            // 2: Filter null
            System.out.println("2: Filter null");
            rec_enum = store.enumerateRecords(null, new comparator(), true);
            assertNotNull(rec_enum);
            assertEquals(rec_enum.numRecords(), 0);

            // 3: Comparator null
            System.out.println("3: Comparator null");
            rec_enum = store.enumerateRecords(new filter(), null, true);
            assertNotNull(rec_enum);
            assertEquals(rec_enum.numRecords(), 0);

            // 4: Enum record store while not open
            System.out.println("4: Enum record store while not open");
            store.closeRecordStore();
            try
            {
                rec_enum = store.enumerateRecords(new filter(), null, true);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetLastModified()
    {
        System.out.println("TestRecordStore.testGetLastModified()");
        RecordStore store = null;
        String rec_store_name = "getLastModified";

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

            // 1: 1st time store was modified
            System.out.println("1: 1st time store was modified");
            long modTime = store.getLastModified();
            long time = System.currentTimeMillis();
            assertTrue(modTime <= time);

            // 2: After store was modified
            System.out.println("2: 2nd time store was modified");
            store.addRecord(null, 0, 0);
            long modTime2 = store.getLastModified();
            assertTrue(modTime <= modTime2);
            assertTrue(modTime2 <= System.currentTimeMillis());

            store.setRecord(1, null, 0, 0);
            long setTime = store.getLastModified();
            assertTrue(modTime2 <= setTime);
            assertTrue(setTime <= System.currentTimeMillis());

            store.deleteRecord(1);
            long delTime = store.getLastModified();
            assertTrue(setTime <= delTime);
            assertTrue(delTime <= System.currentTimeMillis());

            // 3: After non-modify operation
            System.out.println("3: After non-modify operation");
            int id = store.addRecord(null, 0, 0);
            long addTime = store.getLastModified();
            store.getRecord(id);
            long opTime = store.getLastModified();
            assertEquals(addTime, opTime);

            // 4: get while not open
            System.out.println("4: get while not open");
            store.closeRecordStore();
            try
            {
                long t = store.getLastModified();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetName()
    {
        System.out.println("TestRecordStore.testGetName()");
        RecordStore store = null;
        String rec_store_name = "getName";

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

            // 1: get
            System.out.println("1: 1st time store was modified");
            String name = store.getName();
            assertEquals(name, rec_store_name);

            // 2: get while not open
            System.out.println("2: get while not open");
            store.closeRecordStore();
            try
            {
                String s = store.getName();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetNextRecordID()
    {
        System.out.println("TestRecordStore.testGetNextRecordID()");
        RecordStore store = null;
        String rec_store_name = "getNextRecordID";

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

            // 1: getNextRecordID before any records
            System.out.println("1: getNextRecordID before any records");
            int nextId = store.getNextRecordID();
            int recordId = store.addRecord(null, 0, 0);
            assertEquals(nextId, recordId);

            // 2: getNextRecordID when there are some records
            System.out.println("2: getNextRecordID when there are some records");
            for (int i=0; i<100; i++)
            {
                store.addRecord(null, 0, 0);
            }
            int expectedId = store.getNumRecords() + 1;
            nextId = store.getNextRecordID();
            recordId = store.addRecord(null, 0, 0);
            assertEquals(expectedId, nextId);
            assertEquals(nextId, recordId);

            // 3: get while not open
            System.out.println("3: get while not open");
            store.closeRecordStore();
            try
            {
                int i = store.getNextRecordID();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetNumRecords()
    {
        System.out.println("TestRecordStore.testGetNumRecords()");
        RecordStore store = null;
        String rec_store_name = "getNumRecords";

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

            // 1: testGetNumRecords before any records
            System.out.println("1: getNextRecordID before any records");
            int count = store.getNumRecords();
            assertEquals(count, 0);

            // 2: getNextRecordID when there are some records
            System.out.println("2: getNextRecordID when there are some records");
            for (int i=0; i<100; i++)
            {
                store.addRecord(null, 0, 0);
            }
            count = store.getNumRecords();
            assertEquals(count, 100);

            // 3: getNextRecordID after some record removals
            System.out.println("3: getNextRecordID after some record removals");
            for (int i=0; i<50; i++)
            {
                store.deleteRecord(i+1);
            }
            count = store.getNumRecords();
            assertEquals(count, 50);

            // 4: get while not open
            System.out.println("4: get while not open");
            store.closeRecordStore();
            try
            {
                int i = store.getNumRecords();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetRecord()
    {
        System.out.println("TestRecordStore.testGetRecord()");
        RecordStore store = null;
        String rec_store_name = "getRecord";

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

            byte[] arr = new byte[0xff];
            for (int i=0; i<arr.length; i++)
            {
                arr[i] = (byte)i;
            }

            // 1: get null record
            System.out.println("1: get null record");
            store.addRecord(null, 0, 0);
            byte[] data = store.getRecord(1);
            assertTrue(data == null);

            // 2: get zero len record
            System.out.println("2: get zero len record");
            store.setRecord(1, arr, 0, 0);
            data = store.getRecord(1);
            assertTrue(data == null);

            // 3: get record
            System.out.println("3: get record");
            store.setRecord(1, arr, 0, arr.length);
            data = store.getRecord(1);
            assertEquals(data, arr);

            // 4: invalid record id
            System.out.println("4: invalid record id");
            try
            {
                data = store.getRecord(-1);
                fail("InvalidRecordIDException not thrown (-1)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                data = store.getRecord(0);
                fail("InvalidRecordIDException not thrown (0)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                data = store.getRecord(store.getNextRecordID());
                fail("InvalidRecordIDException not thrown (getNextRecordID)");
            }
            catch (InvalidRecordIDException re) {}

            // 5: get while not open
            System.out.println("5: get while not open");
            store.closeRecordStore();
            try
            {
                data = store.getRecord(1);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetRecord2()
    {
        System.out.println("TestRecordStore.testGetRecord2()");
        RecordStore store = null;
        String rec_store_name = "getRecord2";

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

            byte[] arr = new byte[0xff];
            for (int i=0; i<arr.length; i++)
            {
                arr[i] = (byte)i;
            }
            byte[] buffer = new byte[arr.length];

            // 1: get null record
            System.out.println("1: get null record");
            store.addRecord(null, 0, 0);
            int copiedBytes = store.getRecord(1, buffer, 0);
            assertEquals(copiedBytes, 0);
            assertTrue(buffer != null);

            // 2: get zero len record
            System.out.println("2: get zero len record");
            store.setRecord(1, arr, 0, 0);
            copiedBytes = store.getRecord(1, buffer, 0);
            assertEquals(copiedBytes, 0);
            assertTrue(buffer != null);

            // 3: get record
            System.out.println("3: get record");
            store.setRecord(1, arr, 0, arr.length);
            copiedBytes = store.getRecord(1, buffer, 0);
            assertEquals(copiedBytes, arr.length);
            assertEquals(buffer, arr);

            // 4: offset tests
            System.out.println("4: offset tests");
            buffer = new byte[arr.length*2];
            store.setRecord(1, arr, 0, arr.length);
            for (int i=0; i<arr.length; i++)
            {
                copiedBytes = store.getRecord(1, buffer, i);
                assertEquals(copiedBytes, arr.length);
                assertEquals(buffer[i], arr[0]);
            }

            // 5: invalid record id
            System.out.println("5: invalid record id");
            try
            {
                copiedBytes = store.getRecord(-1, buffer, 0);
                fail("InvalidRecordIDException not thrown (-1)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                copiedBytes = store.getRecord(0, buffer, 0);
                fail("InvalidRecordIDException not thrown (0)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                copiedBytes = store.getRecord(store.getNextRecordID(), buffer, 0);
                fail("InvalidRecordIDException not thrown (getNextRecordID)");
            }
            catch (InvalidRecordIDException re) {}

            // 6: the record is larger than the buffer supplied
            System.out.println("6: the record is larger than the buffer supplied");
            store.setRecord(1, arr, 0, arr.length);
            try
            {
                byte[] smallBuf = new byte[0];
                copiedBytes = store.getRecord(1, smallBuf, 0);
                fail("ArrayIndexOutOfBoundsException not thrown (0)");
            }
            catch (ArrayIndexOutOfBoundsException re) {}
            try
            {
                byte[] smallBuf = new byte[90];
                copiedBytes = store.getRecord(1, smallBuf, 5);
                fail("ArrayIndexOutOfBoundsException not thrown (offset)");
            }
            catch (ArrayIndexOutOfBoundsException re) {}
            try
            {
                byte[] smallBuf = new byte[arr.length-1];
                copiedBytes = store.getRecord(1, smallBuf, 0);
                fail("ArrayIndexOutOfBoundsException not thrown (len-1)");
            }
            catch (ArrayIndexOutOfBoundsException re) {}

            // 7: get while not open
            System.out.println("7: get while not open");
            store.closeRecordStore();
            try
            {
                copiedBytes = store.getRecord(0, buffer, 0);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetRecordSize()
    {
        System.out.println("TestRecordStore.testGetRecordSize()");
        RecordStore store = null;
        String rec_store_name = "getRecordSize";

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

            // 1: get null record
            System.out.println("1: get null record");
            store.addRecord(null, 0, 0);
            int size = store.getRecordSize(1);
            assertEquals(size, 0);

            // 2: get zero len record
            System.out.println("2: get zero len record");
            store.setRecord(1, new byte[1], 0, 0);
            size = store.getRecordSize(1);
            assertEquals(size, 0);

            // 3: get record
            System.out.println("3: get record");
            for (int i=0; i<100; i++)
            {
                int id = store.addRecord(new byte[i], 0, i);
                size = store.getRecordSize(id);
                assertEquals(size, i);
            }

            // 4: invalid record id
            System.out.println("4: invalid record id");
            try
            {
                size = store.getRecordSize(-1);
                fail("InvalidRecordIDException not thrown (-1)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                size = store.getRecordSize(0);
                fail("InvalidRecordIDException not thrown (0)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                size = store.getRecordSize(store.getNextRecordID());
                fail("InvalidRecordIDException not thrown (getNextRecordID)");
            }
            catch (InvalidRecordIDException re) {}

            // 5: get while not open
            System.out.println("5: get while not open");
            store.closeRecordStore();
            try
            {
                size = store.getRecordSize(1);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetSize()
    {
        System.out.println("TestRecordStore.testGetSize()");
        RecordStore store = null;
        String rec_store_name = "getSize";

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

            // 1: get size (empty store)
            System.out.println("1: get size (empty store)");
            int size = store.getSize();
            assertTrue(size > 0);

            // 2: get size
            System.out.println("2: get size");
            byte[] data = new byte[100000];
            store.addRecord(data, 0, data.length);
            size = store.getSize();
            assertTrue(size > data.length);

            // 3: get while not open
            System.out.println("3: get while not open");
            store.closeRecordStore();
            try
            {
                size = store.getSize();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetSizeAvailable()
    {
        System.out.println("TestRecordStore.testGetSizeAvailable()");
        RecordStore store = null;
        String rec_store_name = "getSizeAvailable";

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

            // 1: get size (empty store)
            System.out.println("1: get size (empty store)");
            int size = store.getSizeAvailable();
            assertTrue(size > 0);

            // 2: get size
            System.out.println("2: get size");
            byte[] data = new byte[100000];
            store.addRecord(data, 0, data.length);
            int sizeLeft = store.getSizeAvailable();
            assertTrue(sizeLeft <= size);

            // 3: get while not open
            System.out.println("3: get while not open");
            store.closeRecordStore();
            try
            {
                size = store.getSizeAvailable();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testGetVersion()
    {
        System.out.println("TestRecordStore.testGetVersion()");
        RecordStore store = null;
        String rec_store_name = "getVersion";

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

            // 1: 1st time store was modified
            System.out.println("1: 1st time store was modified");
            long origVersion = store.getVersion();
            store.addRecord(null, 0, 0);
            long newVersion = store.getVersion();
            assertTrue(origVersion < newVersion);

            // 2: After store was modified
            System.out.println("2: After store was modified");
            origVersion = store.getVersion();
            store.addRecord(null, 0, 0);
            newVersion = store.getVersion();
            assertTrue(origVersion < newVersion);

            origVersion = store.getVersion();
            store.setRecord(1, null, 0, 0);
            newVersion = store.getVersion();
            assertTrue(origVersion < newVersion);

            origVersion = store.getVersion();
            store.deleteRecord(1);
            newVersion = store.getVersion();
            assertTrue(origVersion < newVersion);

            // 3: After non-modify operation
            System.out.println("3: After non-modify operation");
            int id = store.addRecord(null, 0, 0);
            origVersion = store.getVersion();
            store.getRecord(id);
            newVersion = store.getVersion();
            assertEquals(origVersion, newVersion);

            // 4: get while not open
            System.out.println("4: get while not open");
            store.closeRecordStore();
            try
            {
                int i = store.getVersion();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testListRecordStores()
    {
        System.out.println("TestRecordStore.testListRecordStores()");
        RecordStore store = null;
        String[] rec_store_name =
        {
            "abcdefghijklmnopqrstywxyz",
            "ABCDEFGHIJKLMNOPQRSTYWXYZ",
            " 0 1 2 3 4 5 6 7 8 9 0,.-'+`",
            "!#%&/()=?.rs",
            ".rs",
            "openRecordStore.rs",
            "maximum length recordStoreName A",
            // latin small letter a with diaeresis,
            // latin small letter a with ring above, latin small letter o with diaeresis
            "\u00E4 \u00E5 \u00F6 \u20AC",
            // Japanese Yen symbol, Roman AE with acute accent, Greek Capital Alpha, Greek Capital Omega,
            // Euro symbol, Rupee symbol, Cyrillic capital letter DZHE, Arabic letter TEH
            "\u00a5\u01FC\u0391\u03A9\u20ac\u20a8\u040F\u062A",
            // devanagari letter a, tibetan digit zero, katakana letter ya
            "\u0905 \u0F20 \u30E4"
        };

        try
        {
            // 1: list when no record stores
            System.out.println("1: list when no record stores");
            String[] list = RecordStore.listRecordStores();
            assertTrue(list == null);

            // 2: list when multiple record stores
            System.out.println("2: list when multiple record stores");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                store = RecordStore.openRecordStore(rec_store_name[i], true);
                store.closeRecordStore();
            }
            list = RecordStore.listRecordStores();
            assertEquals(list.length, rec_store_name.length);
            int matchCount = 0;
            for (int i=0; i<list.length; i++)
            {
                for (int j=0; j<list.length; j++)
                {
                    if (list[i].equals(rec_store_name[j]))
                    {
                        matchCount++;
                        break;
                    }
                }
            }
            assertEquals(list.length, matchCount);

            // cleanup
            for (int i = 0; i<rec_store_name.length; i++)
            {
                RecordStore.deleteRecordStore(rec_store_name[i]);
            }
        }
        catch (Exception e)
        {
            fail("Unexpected exception " + e);
        }
    }

    public void testOpenRecordStore()
    {
        System.out.println("TestRecordStore.testOpenRecordStore()");
        RecordStore store = null;
        String[] rec_store_name =
        {
            "abcdefghijklmnopqrstywxyz",
            "ABCDEFGHIJKLMNOPQRSTYWXYZ",
            " 0 1 2 3 4 5 6 7 8 9 0,.-'+`",
            "!#%&/()=?.rs",
            ".rs",
            "openRecordStore.rs",
            "maximum length recordStoreName A",
            // latin small letter a with diaeresis,
            // latin small letter a with ring above, latin small letter o with diaeresis
            "\u00E4 \u00E5 \u00F6 \u20AC",
            // Japanese Yen symbol, Roman AE with acute accent, Greek Capital Alpha, Greek Capital Omega,
            // Euro symbol, Rupee symbol, Cyrillic capital letter DZHE, Arabic letter TEH
            "\u00a5\u01FC\u0391\u03A9\u20ac\u20a8\u040F\u062A",
            // devanagari letter a, tibetan digit zero, katakana letter ya
            "\u0905 \u0F20 \u30E4"
        };

        try
        {
            // 1: openRecordStore (createIfNecessary is true)
            System.out.println("1: openRecordStore (createIfNecessary is true)");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                store = RecordStore.openRecordStore(rec_store_name[i], true);
                assertNotNull(store);
                store.closeRecordStore();
            }

            // 2: openRecordStore (createIfNecessary is false + store exists)
            System.out.println("2: openRecordStore (createIfNecessary is false + store exists)");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                store = RecordStore.openRecordStore(rec_store_name[i], false);
                assertNotNull(store);
                store.closeRecordStore();
            }

            // 3: openRecordStore (createIfNecessary is false + store does not exist)
            System.out.println("3: openRecordStore (createIfNecessary is false + store does not exist)");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                RecordStore.deleteRecordStore(rec_store_name[i]);
                try
                {
                    store = RecordStore.openRecordStore(rec_store_name[i], false);
                    fail("RecordStoreNotFoundException not thrown");
                }
                catch (RecordStoreNotFoundException re) {}
            }

            // 4: record store name is invalid
            System.out.println("4: record store name is invalid");
            try
            {
                store = RecordStore.openRecordStore("", true);
                fail("IllegalArgumentException not thrown (too short)");
            }
            catch (IllegalArgumentException re) {}
            try
            {
                store = RecordStore.openRecordStore("recordStoreName too long by 1  Aa", true);
                fail("IllegalArgumentException not thrown (too long)");
            }
            catch (IllegalArgumentException re) {}
            try
            {
                store = RecordStore.openRecordStore("recordStoreName", true, 55, false);
                fail("IllegalArgumentException not thrown (invalid authMode)");
            }
            catch (IllegalArgumentException re) {}

        }
        catch (Exception e)
        {
            fail("Unexpected exception " + e);
        }
    }

    public void testOpenRecordStore2()
    {
        System.out.println("TestRecordStore.testOpenRecordStore2()");
        RecordStore store = null;
        String[] rec_store_name =
        {
            "abcdefghijklmnopqrstywxyz",
            "ABCDEFGHIJKLMNOPQRSTYWXYZ",
            " 0 1 2 3 4 5 6 7 8 9 0,.-'+`",
            "!#%&/()=?.rs",
            ".rs",
            "openRecordStore.rs",
            "maximum length recordStoreName A",
            // latin small letter a with diaeresis,
            // latin small letter a with ring above, latin small letter o with diaeresis
            "\u00E4 \u00E5 \u00F6 \u20AC",
            // Japanese Yen symbol, Roman AE with acute accent, Greek Capital Alpha, Greek Capital Omega,
            // Euro symbol, Rupee symbol, Cyrillic capital letter DZHE, Arabic letter TEH
            "\u00a5\u01FC\u0391\u03A9\u20ac\u20a8\u040F\u062A",
            // devanagari letter a, tibetan digit zero, katakana letter ya
            "\u0905 \u0F20 \u30E4"
        };

        try
        {
            // 1: openRecordStore (createIfNecessary is true)
            System.out.println("1: openRecordStore (createIfNecessary is true)");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                store = RecordStore.openRecordStore(rec_store_name[i], true, RecordStore.AUTHMODE_PRIVATE, false);
                assertNotNull(store);
                store.closeRecordStore();
            }

            // 2: openRecordStore (createIfNecessary is false + store exists)
            System.out.println("2: openRecordStore (createIfNecessary is false + store exists)");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                store = RecordStore.openRecordStore(rec_store_name[i], false, RecordStore.AUTHMODE_PRIVATE, true);
                assertNotNull(store);
                store.closeRecordStore();
            }

            // 3: openRecordStore (createIfNecessary is false + store does not exist)
            System.out.println("3: openRecordStore (createIfNecessary is false + store does not exist)");
            for (int i = 0; i<rec_store_name.length; i++)
            {
                RecordStore.deleteRecordStore(rec_store_name[i]);
                try
                {
                    store = RecordStore.openRecordStore(rec_store_name[i], false, RecordStore.AUTHMODE_ANY, false);
                    fail("RecordStoreNotFoundException not thrown");
                }
                catch (RecordStoreNotFoundException re) {}
            }

            // 4: record store name is invalid
            System.out.println("4: record store name is invalid");
            try
            {
                store = RecordStore.openRecordStore("", true, RecordStore.AUTHMODE_ANY, false);
                fail("IllegalArgumentException not thrown (too short)");
            }
            catch (IllegalArgumentException re) {}
            try
            {
                store = RecordStore.openRecordStore("recordStoreName too long by 1  Aa", true, RecordStore.AUTHMODE_ANY, false);
                fail("IllegalArgumentException not thrown (too long)");
            }
            catch (IllegalArgumentException re) {}

            // 5: invalid authmode
            System.out.println("5: invalid authmode");
            try
            {
                store = RecordStore.openRecordStore("", true, 666, false);
                fail("IllegalArgumentException not thrown (invalid authmode)");
            }
            catch (IllegalArgumentException re) {}

        }
        catch (Exception e)
        {
            fail("Unexpected exception " + e);
        }
    }

    public void testOpenRecordStore3()
    {
        System.out.println("TestRecordStore.testOpenRecordStore3()");
        RecordStore store = null;
        String rec_store_name = "openRecordStore3";

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
            store.closeRecordStore();

            // 1: open
            System.out.println("1: open");
            store = RecordStore.openRecordStore(rec_store_name, iVendorName, iSuiteName);
            store.closeRecordStore();

            // 2: store does not exist
            System.out.println("2: store does not exist");
            RecordStore.deleteRecordStore(rec_store_name);
            try
            {
                store = RecordStore.openRecordStore(rec_store_name, iVendorName, iSuiteName);
                fail("RecordStoreNotFoundException not thrown");
            }
            catch (RecordStoreNotFoundException re) {}

            // 3: vendor/suite does not exist
            System.out.println("3: vendor/suite does not exist");
            try
            {
                store = RecordStore.openRecordStore(rec_store_name, "dummy vendor", "dummy suite");
                fail("RecordStoreNotFoundException not thrown");
            }
            catch (RecordStoreNotFoundException re) {}

            // 3: vendor/suite does not exist
            System.out.println("3: vendor/suite does not exist");
            try
            {
                store = RecordStore.openRecordStore(rec_store_name, "dummy vendor", "dummy suite");
                fail("RecordStoreNotFoundException not thrown");
            }
            catch (RecordStoreNotFoundException re) {}

            // 4: record store name is invalid
            System.out.println("4: record store name is invalid");
            try
            {
                store = RecordStore.openRecordStore("", iVendorName, iSuiteName);
                fail("IllegalArgumentException not thrown (too short)");
            }
            catch (IllegalArgumentException re) {}
            try
            {
                store = RecordStore.openRecordStore("recordStoreName too long by 1  Aa", iVendorName, iSuiteName);
                fail("IllegalArgumentException not thrown (too long)");
            }
            catch (IllegalArgumentException re) {}

            // Add case for SecurityException (use from multiple suites)
        }
        catch (Exception e)
        {
            e.printStackTrace();
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
    }

    public void testRemoveRecordListener()
    {
        System.out.println("TestRecordStore.testRemoveRecordListener()");
        RecordStore store = null;
        String rec_store_name = "removeRecordListener";

        class Listener implements RecordListener
        {
            public int added = 0;
            public int changed = 0;
            public int deleted = 0;

            public void recordAdded(RecordStore recordStore, int recordId)
            {
                added++;
            }
            public void recordChanged(RecordStore recordStore, int recordId)
            {
                changed++;
            }
            public void recordDeleted(RecordStore recordStore, int recordId)
            {
                deleted++;
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

            // 1: Remove listener
            System.out.println("1: Remove listener");
            Listener listener = new Listener();
            store.addRecordListener(listener);
            store.removeRecordListener(listener);
            store.addRecord(null, 0, 0);
            assertEquals(listener.added, 0);

            // 2: Remove non-existing listener
            System.out.println("2: Remove non-existing listener");
            store.removeRecordListener(listener);
            store.addRecord(null, 0, 0);
            assertEquals(listener.added, 0);

            // 3: Listener removed when store is closed
            System.out.println("3: Listener removed when store is closed");
            store.addRecordListener(listener);
            store.closeRecordStore();
            store.removeRecordListener(listener);

            store = RecordStore.openRecordStore(rec_store_name, true);
        }
        catch (Exception e)
        {
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

    public void testSetMode()
    {
        System.out.println("TestRecordStore.testSetMode()");
        RecordStore store = null;
        String rec_store_name = "setMode";

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

            // 1: Set mode
            System.out.println("1: Set mode");
            store.setMode(RecordStore.AUTHMODE_PRIVATE, true);
            store.setMode(RecordStore.AUTHMODE_PRIVATE, false);
            store.setMode(RecordStore.AUTHMODE_ANY, true);
            store.setMode(RecordStore.AUTHMODE_ANY, false);

            // 2: Invalid auth mode
            System.out.println("2: Invalid auth mode");
            try
            {
                store.setMode(-1, true);
                fail("IllegalArgumentException not thrown");
            }
            catch (IllegalArgumentException se) {}
            try
            {
                store.setMode(2, false);
                fail("IllegalArgumentException not thrown");
            }
            catch (IllegalArgumentException se) {}

            // 3: set mode while not open
            System.out.println("3: set mode while not open");
            store.closeRecordStore();
            try
            {
                store.setMode(RecordStore.AUTHMODE_ANY, true);
                fail("RecordStoreException not thrown");
            }
            catch (RecordStoreException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
            // Add cases for SecurityException
        }
        catch (Exception e)
        {
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

    public void testSetRecord()
    {
        System.out.println("TestRecordStore.testSetRecord()");
        RecordStore store = null;
        String rec_store_name = "setRecord";

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

            byte[] arr = new byte[0xff];
            for (int i=0; i<arr.length; i++)
            {
                arr[i] = (byte)i;
            }

            // 1: Set when no records exist
            System.out.println("1: Set when no records exist");
            try
            {
                store.setRecord(1, new byte[1], 0, 1);
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException se) {}

            // 2: set null record
            System.out.println("2: set null record");
            store.addRecord(arr, 0, arr.length);
            store.setRecord(1, null, 0, 0);
            byte[] buffer = store.getRecord(1);
            assertTrue(buffer == null);

            // 3: set zero len record
            System.out.println("3: set zero len record");
            store.setRecord(1, new byte[1], 0, 0);
            buffer = store.getRecord(1);
            assertTrue(buffer == null);

            // 4: set record
            System.out.println("4: set record");
            store.setRecord(1, arr, 0, arr.length);
            buffer = store.getRecord(1);
            assertEquals(buffer, arr);

            // 5: offset tests
            System.out.println("5: offset tests");
            for (int i=0; i<arr.length; i++)
            {
                store.setRecord(1, arr, i, arr.length-i);
                byte[] data = store.getRecord(1);
                assertEquals(data.length, arr.length-i);
                assertEquals(data[0], arr[i]);
            }

            // 6: invalid record id
            System.out.println("6: invalid record id");
            try
            {
                store.setRecord(-1, null, 0, 0);
                fail("InvalidRecordIDException not thrown (-1)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                store.setRecord(0, null, 0, 0);
                fail("InvalidRecordIDException not thrown (0)");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                store.setRecord(store.getNextRecordID(), null, 0, 0);
                fail("InvalidRecordIDException not thrown (getNextRecordID)");
            }
            catch (InvalidRecordIDException re) {}

            // 7: the record is smaller than the numBytes indicated
            System.out.println("7: the record is smaller than the numBytes indicated");
            try
            {
                byte[] smallBuf = new byte[0];
                store.setRecord(1, smallBuf, 0, smallBuf.length+1);
                fail("ArrayIndexOutOfBoundsException not thrown (0)");
            }
            catch (ArrayIndexOutOfBoundsException re) {}
            try
            {
                byte[] smallBuf = new byte[90];
                store.setRecord(1, smallBuf, 5, smallBuf.length);
                fail("ArrayIndexOutOfBoundsException not thrown (offset)");
            }
            catch (ArrayIndexOutOfBoundsException re) {}

            // 8: record store full
            System.out.println("8: record store full");
            int recordId = 0;
            try
            {
                byte[] oneMB = new byte[1000000];
                // assumed max limit < 50MB
                for (int i = 0; i < 50; i++)
                {
                    recordId = store.addRecord(oneMB, 0, oneMB.length);
                }
                fail("RecordStoreFullException not thrown");
            }
            catch (RecordStoreFullException re) {}
            try
            {
                // modifying last record should go over max size
                byte[] twoMB = new byte[2000000];
                store.setRecord(recordId, twoMB, 0, twoMB.length);
                fail("RecordStoreFullException not thrown");
            }
            catch (RecordStoreFullException re) {}

            // 9. set last record so that available space is zero
            System.out.println("9. set last record so that all available space is used");
            int spaceLeft = store.getSizeAvailable() + store.getRecordSize(recordId);
            store.setRecord(recordId, new byte[spaceLeft], 0, spaceLeft);
            assertEquals(store.getSizeAvailable(), 0);
            try
            {
                // go over max size by one byte
                int size = store.getRecordSize(recordId) + 1;
                store.setRecord(recordId, new byte[size], 0, size);
                fail("RecordStoreFullException not thrown");
            }
            catch (RecordStoreFullException re) {}

            // 10: set while not open
            System.out.println("10: set while not open");
            store.closeRecordStore();
            try
            {
                store.setRecord(1, null, 0, 0);
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            store = RecordStore.openRecordStore(rec_store_name, true);
            // Add cases for SecurityException
        }
        catch (Exception e)
        {
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

    public void testRecordStoreExceptions()
    {
        System.out.println("TestRecordStore.testRecordStoreExceptions()");

        Exception e = null;
        String message = "error message";

        // 1: InvalidRecordIDException
        System.out.println("1: InvalidRecordIDException");
        e = new InvalidRecordIDException();
        assertTrue(e.getMessage() == null);
        e = new InvalidRecordIDException(message);
        assertEquals(message, e.getMessage());

        // 2: RecordStoreException
        System.out.println("2: RecordStoreException");
        e = new RecordStoreException();
        assertTrue(e.getMessage() == null);
        e = new RecordStoreException(message);
        assertEquals(message, e.getMessage());

        // 3: RecordStoreFullException
        System.out.println("3: RecordStoreFullException");
        e = new RecordStoreFullException();
        assertTrue(e.getMessage() == null);
        e = new RecordStoreFullException(message);
        assertEquals(message, e.getMessage());

        // 4: RecordStoreNotFoundException
        System.out.println("4: RecordStoreNotFoundException");
        e = new RecordStoreNotFoundException();
        assertTrue(e.getMessage() == null);
        e = new RecordStoreNotFoundException(message);
        assertEquals(message, e.getMessage());

        // 5: RecordStoreNotOpenException
        System.out.println("5: RecordStoreNotOpenException");
        e = new RecordStoreNotOpenException();
        assertTrue(e.getMessage() == null);
        e = new RecordStoreNotOpenException(message);
        assertEquals(message, e.getMessage());
    }

}

