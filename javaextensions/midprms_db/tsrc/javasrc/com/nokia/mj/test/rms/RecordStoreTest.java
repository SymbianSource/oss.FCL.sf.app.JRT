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

/* These tests are made with knowledge about record store internal implementation.
 */
public class RecordStoreTest extends TestCase
{

    public RecordStoreTest() {}

    public RecordStoreTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new RecordStoreTest("testOpenRecordStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testOpenRecordStore();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testUsingSharedRecordStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testUsingSharedRecordStore();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testGetCommonProperties", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testGetCommonProperties();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testCompareProperties", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testCompareProperties();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testAddRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testAddRecord();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testAddDeleteRecords", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testAddDeleteRecords();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testAddDeleteRecords2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testAddDeleteRecords2();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testGetSize", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testGetSize();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testGetNextRecordId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testGetNextRecordId();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testNullRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testNullRecord();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testListRecordstore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testListRecordstore();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testEnumeration", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testEnumeration();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testListeners", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testListeners();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testFiltering", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testFiltering();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testSplitAndMerge", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testSplitAndMerge();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testRecordStoreSpeed", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testRecordStoreSpeed();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testEnumerationWithEmptyStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testEnumerationWithEmptyStore();
            }
        }));

        aSuite.addTest(new RecordStoreTest("testFilterComparator", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RecordStoreTest) tc).testFilterComparator();
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

    public void testOpenRecordStore()
    {
        System.out.println("RecordStoreTest.testOpenRecordStore()");
        RecordStore store = null;

        // 1: IllegalArgumentException
        System.out.println("1: IllegalArgumentException - zero length name");
        try
        {
            store = RecordStore.openRecordStore("", true);
            fail("IllegalArgumentException for zero lenght rs name was not thrown");
        }
        catch (IllegalArgumentException iae)
        {
            //test OK
        }
        catch (Exception e)
        {
            fail("Wrong exception " + e);
        }

        System.out.println("1: IllegalArgumentException - too long name");
        try
        {
            store = RecordStore.openRecordStore("aaaaabbbbbcccccdddddeeeeefffffggg", true);
            fail("IllegalArgumentException for 33 chars lenght rs name expected");
        }
        catch (IllegalArgumentException iae)
        {
            //test OK
        }
        catch (Exception e)
        {
            fail("Wrong exception " + e);
        }

        // 2: RecordStoreNotFoundException
        System.out.println("2: RecordStoreNotFoundException");
        try
        {
            store = RecordStore.openRecordStore("no_such_rs", false);
            fail("RecordStoreNotFoundException expected");
        }
        catch (RecordStoreNotFoundException nfe)
        {
            //test OK
        }
        catch (Exception e)
        {
            fail("Wrong exception " + e);
        }

        //3: create new record store
        System.out.println("3: create new record store");
        try
        {
            store = RecordStore.openRecordStore("new_rs", true);
            assertNotNull(store);
        }
        catch (Exception e)
        {
            fail("Exception not expected " + e);
        }

        //4: open record store one more time
        System.out.println("4: open record store one more time");
        try
        {
            RecordStore store2 = RecordStore.openRecordStore("new_rs", false);
            if (!store2.equals(store))
            {
                fail("reference was not to the same RecordStore object ");
            }
            store2.closeRecordStore();
        }
        catch (Exception e)
        {
            fail("Exception not expected " + e);
        }

        //5: open existing record store
        System.out.println("5: open existing record store");
        try
        {
            store.closeRecordStore(); //was open in 1.3
            store = RecordStore.openRecordStore("new_rs", false);
            assertNotNull(store);
        }
        catch (Exception e)
        {
            fail("Exception not expected " + e);
        }

        //6: clean up
        System.out.println("6: clean up");
        try
        {
            store.closeRecordStore();
            RecordStore.deleteRecordStore("new_rs");

        }
        catch (Exception e)
        {
            fail("Exception not expected " + e);
        }

        //7: close record store second time
        System.out.println("7: close record store second time");
        try
        {
            store.closeRecordStore();
            fail("recordStoreNotOpenException expected ");
        }
        catch (RecordStoreNotOpenException noe)
        {
            //test OK
        }
        catch (Exception e)
        {
            fail("Wrong exception " + e);
        }

        //8. deleteRecordStore(null)
        System.out.println("8. deleteRecordStore(null)");
        try
        {
            RecordStore.deleteRecordStore(null);
            fail("RecordStoreException expected");
        }
        catch (RecordStoreException rse) {}
    }

    public void testUsingSharedRecordStore()
    {
        System.out.println("RecordStoreTest.testUsingSharedRecordStore()");
        RecordStore store = null;

        // keep these in sync with CreateSharedStore
        String vendorName = CreateSharedStore.VENDORNAME;
        String suiteName  = CreateSharedStore.SUITENAME;

        try
        {
            // 1: Trying to open private store
            System.out.println("1: Trying to open private store");
            for (int i = 0; i<CreateSharedStore.PRIVATESTORES.length; i++)
            {
                try
                {
                    store = RecordStore.openRecordStore(CreateSharedStore.PRIVATESTORES[i], vendorName, suiteName);
                    store.closeRecordStore();
                    fail("SecurityException was not thrown");
                }
                catch (SecurityException re) {}
            }

            // 2: Trying to open non-existing store
            System.out.println("2: Trying to open non-existing store");
            try
            {
                store = RecordStore.openRecordStore("non-existing store", vendorName, suiteName);
                store.closeRecordStore();
                fail("RecordStoreNotFoundException was not thrown");
            }
            catch (RecordStoreNotFoundException re) {}


            // 3: Open shared store
            System.out.println("3: Open shared store");
            for (int i = 0; i<CreateSharedStore.SHAREDSTORES.length; i++)
            {
                try
                {
                    store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[i], vendorName, suiteName);
                    store.closeRecordStore();
                }
                catch (Exception re)
                {
                    fail("Exception not expected " + re);
                }
            }

            // 4: Using writable shared store
            System.out.println("4: Using writable shared store");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[0], vendorName, suiteName);
            int recordId = store.addRecord(null, 0, 0);
            store.setRecord(recordId, new byte[1000], 0, 1000);
            store.deleteRecord(recordId);
            store.closeRecordStore();

            // 5: Using writable shared store - enumeration
            System.out.println("5: Using writable shared store - enumeration");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[0], vendorName, suiteName);
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            try
            {
                while (true)
                {
                    int id  = enumeration.nextRecordId();
                    // go back to get the record
                    if (enumeration.hasPreviousElement())
                    {
                        enumeration.previousRecordId();
                    }
                    else
                    {
                        enumeration.reset();
                    }
                    byte[] orig = store.getRecord(id);
                    byte[] data = enumeration.nextRecord();
                    assertEquals(orig, data);
                }
            }
            catch (InvalidRecordIDException re) {}
            enumeration.destroy();
            store.closeRecordStore();

            // 6: Using non-writable shared store
            System.out.println("6: Using non-writable shared store");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[1], vendorName, suiteName);
            try
            {
                store.addRecord(null, 0, 0);
                fail("SecurityException was not thrown (add)");
            }
            catch (SecurityException re) {}
            try
            {
                store.setRecord(1, new byte[1000], 0, 1000);
                fail("SecurityException was not thrown (set)");
            }
            catch (SecurityException re) {}
            try
            {
                store.deleteRecord(recordId);
                fail("SecurityException was not thrown (delete)");
            }
            catch (SecurityException re) {}
            store.closeRecordStore();

            // 7: Using non-writable shared store - enumeration
            System.out.println("7: Using non-writable shared store - enumeration");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[1], vendorName, suiteName);
            enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            try
            {
                while (true)
                {
                    int id  = enumeration.nextRecordId();
                    // go back to get the record
                    if (enumeration.hasPreviousElement())
                    {
                        enumeration.previousRecordId();
                    }
                    else
                    {
                        enumeration.reset();
                    }
                    byte[] orig = store.getRecord(id);
                    byte[] data = enumeration.nextRecord();
                    assertEquals(orig, data);
                }
            }
            catch (InvalidRecordIDException re) {}
            enumeration.destroy();
            store.closeRecordStore();

            // 8: Writable shared store - setMode
            System.out.println("8: Writable shared store - setMode");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[0], vendorName, suiteName);
            try
            {
                store.setMode(RecordStore.AUTHMODE_ANY, true);
                fail("SecurityException was not thrown");
            }
            catch (SecurityException re) {}
            store.closeRecordStore();

            // 9: Non-writable shared store - setMode
            System.out.println("9: Non-writable shared store - setMode");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[1], vendorName, suiteName);
            try
            {
                store.setMode(RecordStore.AUTHMODE_ANY, true);
                fail("SecurityException was not thrown");
            }
            catch (SecurityException re) {}
            store.closeRecordStore();


        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Exception not expected " + e);
        }
    }

    public void testGetCommonProperties()
    {
        // test common record store properties (size, version, last modified)
        System.out.println("RecordStoreTest.testGetCommonProperties()");

        RecordStore store = null;
        String rec_store_name = "testGetCommonProperties";

        //1: getVersion / getLastModified check
        System.out.println("1: getVersion / getLastModified check");
        try
        {
            store = RecordStore.openRecordStore(rec_store_name, true);

            int ver1 = store.getVersion();
            long time1 = store.getLastModified();
            try
            {
                Thread.sleep(50);
            }
            catch (InterruptedException ie)
            {
            }

            byte data[] = {1, 2, 3};
            store.addRecord(data, 0, data.length);

            int ver2 = store.getVersion();
            long time2 = store.getLastModified();

            assertTrue(ver1 < ver2);
            assertTrue(time1 < time2);

            store.closeRecordStore();

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }

        // 2: getVersion / getLastModified while not open
        System.out.println("2: getVersion / getLastModified while not open");
        try
        {
            int ver3 = store.getVersion();
            fail("recordStoreNotOpenException expected");
        }
        catch (RecordStoreNotOpenException noe)
        {
        }

        try
        {
            long time3 = store.getLastModified();
            fail("recordStoreNotOpenException expected");
        }
        catch (RecordStoreNotOpenException noe)
        {
        }

        try
        {
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }

        try
        {
            store = RecordStore.openRecordStore(rec_store_name, false);
            fail("recordStoreNotFoundException expected ");
        }
        catch (RecordStoreNotFoundException nfe)
        {
        }
        catch (Exception ex)
        {
            fail("another type of Exception is expected " + ex);
        }
    }

    public void testCompareProperties()
    {
        // test of keeping rs property info after closing rs
        System.out.println("RecordStoreTest.testGetCommonProperties()");

        RecordStore store = null;
        String rec_store_name = "testCompareProperties";

        int ver1 = -1, ver2 = -1;
        long time1 = 0, time2 = 0;
        byte[] data1, data2;

        //1: open empty record store and write some data
        System.out.println("1: open empty record store and write some data");
        try
        {
            store = RecordStore.openRecordStore(rec_store_name, true);

            int size = store.getNumRecords();
            assertEquals(size, 0);

            byte data[] = {1, 2, 3, 15};
            store.addRecord(data, 0, data.length);

            size = store.getNumRecords();
            assertEquals(size, 1);

            ver1 = store.getVersion();
            time1 = store.getLastModified();

            store.closeRecordStore();

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }

        //2: open record store and compare properties
        System.out.println("2: open record store and compare properties");
        try
        {
            store = RecordStore.openRecordStore(rec_store_name, false);

            ver2 = store.getVersion();
            assertTrue(ver1 == ver2);

            time2 = store.getLastModified();
            assertTrue(time1 == time2);

            int size = store.getNumRecords();
            assertEquals(size, 1);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testAddRecord()
    {
        System.out.println("RecordStoreTest.testAddRecord()");

        RecordStore store = null;
        String rec_store_name = "testAddRecord";

        byte data1[] = {1, 2, 3, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};

        int rec1, rec2 = 0;

        //1. add and get two records from record store
        System.out.println("1. add and get two records from record store");
        try
        {
            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);

            assertEquals(rec1, 1);
            assertEquals(rec2, 2);

            int size = store.getNumRecords();
            assertEquals(size, 2);

            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            size = store.getNumRecords();
            assertEquals(size, 2);

            byte[] data1copy = store.getRecord(rec1);
            assertEquals(data1, data1copy);

            byte[] data2copy = store.getRecord(rec2);
            assertEquals(data2, data2copy);

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }

        //2. invalid record id
        System.out.println("2. invalid record id");
        try
        {
            byte[] data = store.getRecord(rec2+1);
            fail("InvalidRecordIDException expected");
        }
        catch (InvalidRecordIDException irid)
        {
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }

        //3. invalid record
        System.out.println("3. invalid record");
        try
        {
            store.addRecord(null, 0, 1);
            fail("NullPointerException expected");
        }
        catch (NullPointerException e)
        {
        }
        catch (RecordStoreException e)
        {
        }

        try
        {
            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testAddDeleteRecords()
    {
        //special cases for set record:
        // new record longer than old one
        // new record shorter than old one
        System.out.println("RecordStoreTest.testAddDeleteRecords()");
        RecordStore store = null;
        String rec_store_name = "testAddDeleteRecords";

        byte data1[] = {2, 3, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};

        int rec1, rec2, rec3 = 0;

        try
        {
            // 1: add records
            System.out.println("1: add records");
            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);
            rec3 = store.addRecord(data3, 0, data3.length);

            int size = store.getNumRecords();
            assertEquals(size, 3);

            store.deleteRecord(rec2);
            size = store.getNumRecords();
            assertEquals(size, 2);

            // 2: delete invalid record
            System.out.println("2: delete invalid record");
            try
            {
                store.deleteRecord(10801);
                fail("invalid rec id, exception expected");
            }
            catch (InvalidRecordIDException irid)
            {
            }
            catch (Exception ex)
            {
                fail("Exception not expected " + ex);
            }

            assertEquals(size, 2);

            // 3: delete non-existing record
            System.out.println("3: delete non-existing record");
            try
            {
                byte[] data = store.getRecord(rec2);
            }
            catch (InvalidRecordIDException irid)
            {
            }
            catch (Exception ex)
            {
                fail("FAIL 5.3: Exception not expected " + ex);
            }

            store.setRecord(rec1, data2, 0, data2.length); //original record is shorter than new one
            store.setRecord(rec3, data2, 0, data2.length); //original record is longer than new one

            byte[] data1copy = store.getRecord(rec1);
            assertEquals(data2, data1copy);

            byte[] data3copy = store.getRecord(rec3);
            assertEquals(data2, data3copy);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testAddDeleteRecords2()
    {
        // special cases for delete:
        // - delete the only record in record store
        // - delete the last record
        // - delete the first record
        System.out.println("RecordStoreTest.testAddDeleteRecords2()");
        RecordStore store = null;
        String rec_store_name = "testAddDeleteRecords2";

        byte data1[] = {12, 23, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};
        byte data4[] = {21, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};
        byte data5[] = {21, 20, 19, 18, 17, 33, 45, 11, 22, 33, 44, 33, 22, 11};

        int rec1, rec2, rec3, rec4, rec5 = 0;

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e)
            {
            }

            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);
            rec3 = store.addRecord(data3, 0, data3.length);
            rec4 = store.addRecord(data4, 0, data4.length);
            rec5 = store.addRecord(data5, 0, data5.length);

            int size = store.getNumRecords();
            assertEquals(size, 5);

            //delete first record
            store.deleteRecord(rec1);
            size = store.getNumRecords();
            assertEquals(size, 4);

            // reset
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            size = store.getNumRecords();
            assertEquals(size, 4);

            byte[] data = store.getRecord(rec2);
            assertEquals(data2, data);

            //delete last record
            store.deleteRecord(rec5);
            size = store.getNumRecords();
            assertEquals(size, 3);

            // reset
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            size = store.getNumRecords();
            assertEquals(size, 3);

            data = store.getRecord(rec4);
            assertEquals(data4, data);

            //delete record in the middle
            store.deleteRecord(rec3);
            size = store.getNumRecords();
            assertEquals(size, 2);

            // reset
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            size = store.getNumRecords();
            assertEquals(size, 2);

            data = store.getRecord(rec2);
            assertEquals(data2, data);

            data = store.getRecord(rec4);
            assertEquals(data4, data);

            //delete all records
            store.deleteRecord(rec2);
            store.deleteRecord(rec4);

            size = store.getNumRecords();
            assertEquals(size, 0);

            // reset
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            size = store.getNumRecords();
            assertEquals(size, 0);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }


    public void testGetSize()
    {
        System.out.println("RecordStoreTest.testGetSize()");
        RecordStore store = null;
        String rec_store_name = "testGetSize";

        byte data1[] = {12, 23, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};
        byte data4[] = {21, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};

        int rec1, rec2, rec3, rec4 = 0;

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e) {}

            store = RecordStore.openRecordStore(rec_store_name, true);

            int total_size1 = store.getSize();
            int avail_size1 = store.getSizeAvailable();

            rec2 = store.addRecord(data2, 0, data2.length);
            rec3 = store.addRecord(data3, 0, data3.length);
            rec4 = store.addRecord(data4, 0, data4.length);

            int total_size2 = store.getSize();
            int avail_size2 = store.getSizeAvailable();

            assertTrue(total_size2 >= total_size1);
            // available size may not change on all platforms
            assertTrue(avail_size2 <= avail_size1);

            //delete long record and add short one
            store.deleteRecord(rec4);
            rec1 = store.addRecord(data1, 0, data1.length);

            // reset
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            int total_size3 = store.getSize();
            int avail_size3 = store.getSizeAvailable();

            assertTrue(total_size3 <= total_size2);
            assertTrue(avail_size3 >= avail_size2);

            int size = store.getNumRecords();
            assertEquals(size, 3);

            //read record that was added to the place of previously deleted
            byte[] data = store.getRecord(rec1);
            assertEquals(data, data1);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testGetNextRecordId()
    {
        System.out.println("RecordStoreTest.testGetNextRecordId()");
        //getRecordSize, getRecord(), getNextRecordId, getName
        RecordStore store = null;
        String rec_store_name = "testGetNextRecordId";

        byte data1[] = {12, 23, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};
        byte data4[] = {21, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};

        int rec1, rec2, rec3, rec4 = 0;

        try
        {
            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e) {}

            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);

            int nextId = store.getNextRecordID();
            rec3 = store.addRecord(data3, 0, data3.length);
            assertEquals(nextId, rec3);

            nextId = store.getNextRecordID();

            // new data doesn't fit to old place, new record will be created
            // but it shouldn't use nextId value
            store.setRecord(rec2, data4, 0, data4.length);

            rec4 = store.addRecord(data4, 0, data4.length);
            assertEquals(nextId, rec4);

            int len = store.getRecordSize(rec2);
            assertEquals(len, data4.length);

            byte[] arr1 = store.getRecord(rec3);
            int recLen = store.getRecordSize(rec3);
            assertEquals(recLen, arr1.length);

            byte[] arr2 = new byte[recLen +2];
            int recLen2 = store.getRecord(rec3, arr2, 2);
            assertEquals(recLen2, recLen);
            for (int i = 0; i < arr1.length; i++)
            {
                assertEquals(arr1[i], arr2[i+2]);
            }

            String name = store.getName();
            assertEquals(name.compareTo(rec_store_name), 0);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testNullRecord()
    {
        System.out.println("RecordStoreTest.testNullRecord()");
        RecordStore store = null;
        String rec_store_name = "testNullRecord";

        int rec1, rec2, rec3, rec4 = 0;

        try
        {
            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e) {}

            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(null, 0, 0);
            rec2 = store.addRecord(null, 0, 0);
            rec3 = store.addRecord(null, 0, 0);

            //check num records
            assertEquals(store.getNumRecords(), 3);

            //get record size
            assertEquals(store.getRecordSize(rec2), 0);

            //get record
            assertTrue(store.getRecord(rec2) == null);

            // delete one record
            store.deleteRecord(rec2);

            // close and open
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            // get num record
            assertEquals(store.getNumRecords(), 2);

            //get record size & get record
            assertEquals(store.getRecordSize(rec3), 0);
            assertTrue(store.getRecord(rec3) == null);

            //set record
            byte data1[] = {21, 20, 19, 18, 17, 33, 45};
            store.setRecord(rec1, data1, 0, data1.length);

            //close and open store
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            //check record
            byte[] data = store.getRecord(rec1);
            assertEquals(data, data1);

            // delete records
            store.deleteRecord(rec1);
            store.deleteRecord(rec3);

            store.closeRecordStore();
            // delete record store
            RecordStore.deleteRecordStore(rec_store_name);

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testListRecordstore()
    {
        System.out.println("RecordStoreTest.testListRecordstore()");

        RecordStore store1 = null;
        RecordStore store2 = null;
        RecordStore store3 = null;

        String rec_store_name1 = "testListRecordstore-1";
        String rec_store_name2 = "testListRecordstore-2$%^&";
        String rec_store_name3 = "testListRecordstore-3ASF??**";

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name1);
            }
            catch (Exception e) { }

            try
            {
                RecordStore.deleteRecordStore(rec_store_name2);
            }
            catch (Exception e) { }

            try
            {
                RecordStore.deleteRecordStore(rec_store_name3);
            }
            catch (Exception e) { }

            String[] list1 = RecordStore.listRecordStores();
            assertTrue(list1 == null);

            store1 = RecordStore.openRecordStore(rec_store_name1, true);
            store2 = RecordStore.openRecordStore(rec_store_name2, true);
            store3 = RecordStore.openRecordStore(rec_store_name3, true);

            //try to delete open record store
            try
            {
                RecordStore.deleteRecordStore(rec_store_name1);
                fail("delete open record store succeeded");
            }
            catch (RecordStoreException rse)
            {
            }

            store1.closeRecordStore();
            store2.closeRecordStore();
            store3.closeRecordStore();

            String[] list2 = RecordStore.listRecordStores();
            assertNotNull(list2);
            assertEquals(list2.length, 3);
            boolean found1 = false;
            for (int i = 0; i < list2.length; i++)
            {
                if (rec_store_name1.compareTo(list2[i]) == 0)
                {
                    found1 = true;
                    break;
                }
            }
            assertTrue(found1);

            boolean found2 = false;
            for (int i = 0; i < list2.length; i++)
            {
                if (rec_store_name2.compareTo(list2[i]) == 0)
                {
                    found2 = true;
                    break;
                }
            }
            assertTrue(found2);

            boolean found3 = false;
            for (int i = 0; i < list2.length; i++)
            {
                if (rec_store_name3.compareTo(list2[i]) == 0)
                {
                    found3 = true;
                    break;
                }
            }
            assertTrue(found3);

            RecordStore.deleteRecordStore(rec_store_name1);
            RecordStore.deleteRecordStore(rec_store_name2);
            RecordStore.deleteRecordStore(rec_store_name3);

        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testEnumeration()
    {
        System.out.println("RecordStoreTest.testEnumeration()");
        RecordStore store = null;
        String rec_store_name = "testEnumeration";

        byte data1[] = {12, 23, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};
        byte data4[] = {21, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};

        int rec1, rec2, rec3, rec4 = 0;

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e) { }

            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);
            rec3 = store.addRecord(data3, 0, data3.length);
            rec4 = store.addRecord(data4, 0, data4.length);

            RecordEnumeration rec_enum = store.enumerateRecords(null, null, false);
            assertNotNull(rec_enum);
            assertEquals(rec_enum.numRecords(), 4);

            int skip = rec_enum.nextRecordId(); // skip first record to simplify the test

            while (rec_enum.hasNextElement())
            {

                int id = rec_enum.nextRecordId();
                int idd = rec_enum.previousRecordId(); //return back

                byte[] rec_arr = store.getRecord(id);
                byte[] rec_arr_enum = rec_enum.nextRecord();
                assertEquals(rec_arr, rec_arr_enum);
            }

            while (rec_enum.hasPreviousElement())
            {

                int id = rec_enum.previousRecordId();
                int idd = rec_enum.nextRecordId(); //return back

                byte[] rec_arr = store.getRecord(id);
                byte[] rec_arr_enum = rec_enum.previousRecord();
                assertEquals(rec_arr, rec_arr_enum);
            }

            rec_enum.rebuild();
            assertEquals(rec_enum.numRecords(), 4);
            assertTrue(rec_enum.isKeptUpdated() == false);
            rec_enum.destroy();

            try
            {
                int temp = rec_enum.numRecords();
                fail("enum.numRecords() while destroyed");
            }
            catch (IllegalStateException ise)
            {
            }

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }


    public void testListeners()
    {
        System.out.println("RecordStoreTest.testListeners()");

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

        RecordStore store = null;
        String rec_store_name = "testListeners";

        byte data1[] = {12, 23, 15, 77};
        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};
        byte data4[] = {21, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};

        int rec1, rec2, rec3, rec4 = 0;

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e)
            {
            }

            store = RecordStore.openRecordStore(rec_store_name, true);

            Listener listener = new Listener();

            store.addRecordListener(listener);
            store.addRecordListener(listener);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);
            rec3 = store.addRecord(data3, 0, data3.length);

            store.setRecord(rec1, data4, 0, data4.length);
            store.setRecord(rec2, data4, 0, data4.length);

            store.deleteRecord(rec1);

            store.removeRecordListener(listener);
            rec1 = store.addRecord(data1, 0, data1.length);
            store.setRecord(rec1, data4, 0, data4.length);
            store.deleteRecord(rec1);

            assertEquals(listener.added, 3);
            assertEquals(listener.changed, 2);
            assertEquals(listener.deleted, 1);

            store.addRecordListener(listener);
            store.closeRecordStore();

            assertEquals(listener.deleted, 1);
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testFiltering()
    {
        System.out.println("RecordStoreTest.testFiltering()");
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

        RecordStore store = null;
        String rec_store_name = "testFiltering";

        byte data1[] = {12, 23, 15, 77};
        byte data2[] = {14, 22, 32, 15, 99};
        byte data3[] = {60, 20, 19, 18, 17, 33, 45};
        byte data4[] = {70, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};
        byte data5[] = {40, 20, 19, 18, 17, 33, 45, 33, 21};
        byte data15[] = {71, 20, 19, 18, 17, 33, 45, 33, 21};
        byte data25[] = {72, 20, 19, 18, 17, 33, 45, 33, 21};
        byte data35[] = {73, 20, 19, 18, 17, 33, 45, 33, 21};

        int rec1, rec2, rec3, rec4, rec5, rec6, rec7, rec8 = 0;

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e)
            {
            }

            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec2 = store.addRecord(data2, 0, data2.length);
            rec3 = store.addRecord(data3, 0, data3.length);
            rec4 = store.addRecord(data4, 0, data4.length);
            rec5 = store.addRecord(data5, 0, data5.length);

            rec6 = store.addRecord(data15, 0, data15.length);
            rec7 = store.addRecord(data25, 0, data25.length);
            rec8 = store.addRecord(data35, 0, data35.length);

            //rec1 and rec2 are excluded by filter
            RecordEnumeration rec_enum = store.enumerateRecords(new filter(), new comparator(), true);

            assertNotNull(rec_enum);
            assertEquals(rec_enum.numRecords(), 6);

            int id = rec_enum.nextRecordId();
            assertEquals(id, rec5);

            id = rec_enum.nextRecordId();
            assertEquals(id, rec3);

            id = rec_enum.nextRecordId();
            assertEquals(id, rec4);

            // delete next record
            store.deleteRecord(rec6);

            //now enumeration should be updated
            id = rec_enum.nextRecordId();
            assertEquals(id, rec7);

            //delete previous record
            store.deleteRecord(rec4);

            // check enumeration is updated again
            id = rec_enum.previousRecordId();
            assertEquals(id, rec3);

            //add new record
            int recNew = store.addRecord(data4, 0, data4.length);
            id = rec_enum.nextRecordId();
            assertEquals(id, recNew);

            //modify rec
            byte new_data[] = {71, 23, 15, 77};
            store.setRecord(rec5, new_data, 0, new_data.length);
            id = rec_enum.nextRecordId();
            assertEquals(id, rec5);
            assertEquals(rec_enum.numRecords(), 5);

            // add record that will be sorted out
            store.addRecord(data2, 0, data2.length);
            assertEquals(rec_enum.numRecords(), 5);
            assertTrue(rec_enum.isKeptUpdated() == true);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }

    public void testSplitAndMerge()
    {
        System.out.println("RecordStoreTest.testSplitAndMerge()");
        RecordStore store = null;
        String rec_store_name = "testSplitAndMerge";

        byte data1[] = new byte[1024];
        for (int i = 0; i < data1.length; i++)
            data1[i] = 33;

        byte data2[] = {21, 22, 32, 15, 99};
        byte data3[] = {21, 20, 19, 18, 17, 33, 45};
        byte data4[] = {21, 20, 19, 18, 17, 33, 45, 33, 21, 54, 76, 76};

        int rec1, rec2, rec3, rec4, rec5, rec11 = 0;

        try
        {

            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
            }
            catch (Exception e)
            {
            }

            store = RecordStore.openRecordStore(rec_store_name, true);

            rec1 = store.addRecord(data1, 0, data1.length);
            rec11 = store.addRecord(data1, 0, data1.length);

            int size = store.getSize();
            store.deleteRecord(rec1);

            rec2 = store.addRecord(data2, 0, data2.length);
            int size1 = store.getSize();
            assertTrue(size1 <= size);

            rec3 = store.addRecord(data3, 0, data3.length);
            size1 = store.getSize();
            assertTrue(size1 <= size);

            rec4 = store.addRecord(data4, 0, data4.length);
            size1 = store.getSize();
            assertTrue(size1 <= size);

            //re-read data form file
            store.closeRecordStore();
            store = RecordStore.openRecordStore(rec_store_name, false);

            // add one more
            rec5 = store.addRecord(data4, 0, data4.length);
            size1 = store.getSize();
            assertTrue(size1 <= size);

            byte[] data = store.getRecord(rec2);
            assertEquals(data2, data);

            data = store.getRecord(rec3);
            assertEquals(data3, data);

            data = store.getRecord(rec4);
            assertEquals(data4, data);

            // rec5 == rec4
            data = store.getRecord(rec5);
            assertEquals(data4, data);

            // delete last record in record store, now record store size should be shrinked
            store.deleteRecord(rec11);
            size1 = store.getSize();
            assertTrue(size1 <= size);

            store.deleteRecord(rec2);
            store.deleteRecord(rec3);
            store.deleteRecord(rec5);
            store.deleteRecord(rec4);
            rec1 = store.addRecord(data1, 0, data1.length);
            size1 = store.getSize();
            assertTrue(size1 <= size);

            store.closeRecordStore();
            RecordStore.deleteRecordStore(rec_store_name);
        }
        catch (Exception ex)
        {
            fail("Exception not expected " + ex);
        }
    }


    /**
     * Get the estimated speed of the RecordStore.
     * We test the speed of RecodStore creation, closing and deletion, and record creation, updating, reading, deletetion.
     * For the records we both test speed for 20 records of 100 bytes and 1 record of 2KB.
     * @return The data to be displayed about the estimated speed of the RecordStore.
     */
    public void testRecordStoreSpeed()
    {
        System.out.println("RecordStoreTest.testRecordStoreSpeed()");

        long t1, t2;
        RecordStore store= null;
        int freeRMS= 0;
        final String recordStoreName= "Test";

        String error= "";
        int timeCreateRMS= -1;
        int timeAddRMSa= -1;
        int timeAddRMSb= -1;
        int timeSetRMSa= -1;
        int timeSetRMSb= -1;
        int timeGetRMSa= -1;
        int timeGetRMSb= -1;
        int timeDeleteRMS= -1;
        int timeCloseRMS= -1;
        int timeDestroyRMS= -1;
        int recordStoreSize= -1;

        int rec_num = -1;
        int timeListRMSd = -1;

        final int DATA_LENGTH= 100;
        final int BIG_DATA_LENGTH= 1024*16;
        final int REC_NUM = 8;

        try
        {
            t1= System.currentTimeMillis();
            store= RecordStore.openRecordStore(recordStoreName, true);
            t2= System.currentTimeMillis();
            timeCreateRMS= (int)(t2 - t1);
            freeRMS= store.getSizeAvailable();
            byte[] data= new byte[DATA_LENGTH];
            byte[] bigData= new byte[BIG_DATA_LENGTH];
            int[] recordId= new int[21];
            int i;
            t1= System.currentTimeMillis();
            for (i= REC_NUM-1; i >= 0; i--)
            {
                recordId[i]= store.addRecord(data, 0, DATA_LENGTH);
            }
            t2= System.currentTimeMillis();
            timeAddRMSa= (int)(t2 - t1);
            t1= t2;
            recordId[REC_NUM]= store.addRecord(bigData, 0, BIG_DATA_LENGTH);
            t2= System.currentTimeMillis();
            timeAddRMSb= (int)(t2 - t1);
            for (i= DATA_LENGTH - 1; i >= 0; i--)
            {
                data[i]= (byte)i;
            }
            for (i= BIG_DATA_LENGTH - 1; i >= 0; i--)
            {
                bigData[i]= (byte)i;
            }
            t1= System.currentTimeMillis();
            for (i= REC_NUM-1; i >= 0; i--)
            {
                store.setRecord(recordId[i], data, 0, DATA_LENGTH);
            }
            t2= System.currentTimeMillis();
            timeSetRMSa= (int)(t2 - t1);
            t1= t2;

            RecordStore store1= RecordStore.openRecordStore("#$total_fake%^", true);
            RecordStore store2= RecordStore.openRecordStore("fake2", true);
            String list[] = RecordStore.listRecordStores();
            rec_num = list.length;
            t2= System.currentTimeMillis();
            timeListRMSd = (int)(t2 - t1);
            t1 = t2;

            for (i = 0; i < rec_num; i++)
                System.out.println("store name from list = " +list[i]);

            store1.closeRecordStore();
            store2.closeRecordStore();
            RecordStore.deleteRecordStore("#$total_fake%^");
            RecordStore.deleteRecordStore("fake2");

            store.setRecord(recordId[REC_NUM], bigData, 0, BIG_DATA_LENGTH);
            t2= System.currentTimeMillis();
            timeSetRMSb= (int)(t2 - t1);
            t1= t2;
            for (i= REC_NUM-1; i >= 0; i--)
            {
                data= store.getRecord(recordId[i]);
            }
            t2= System.currentTimeMillis();
            timeGetRMSa= (int)(t2 - t1);
            t1= t2;
            bigData= store.getRecord(recordId[REC_NUM]);
            t2= System.currentTimeMillis();
            timeGetRMSb= (int)(t2 - t1);
            recordStoreSize= store.getSize();
            t1= System.currentTimeMillis();
            for (i= REC_NUM; i >= 0; i--)
            {
                store.deleteRecord(recordId[i]);
            }
            t2= System.currentTimeMillis();
            timeDeleteRMS= (int)(t2 - t1);
        }
        catch (Exception e)
        {
            error= e.toString();
        }
        finally
        {
            if (store != null)
            {
                try
                {
                    t1= System.currentTimeMillis();
                    store.closeRecordStore();
                    t2= System.currentTimeMillis();
                    timeCloseRMS= (int)(t2 - t1);
                    t1= t2;
                    RecordStore.deleteRecordStore(recordStoreName);
                    t2= System.currentTimeMillis();
                    timeDestroyRMS= (int)(t2 - t1);
                }
                catch (Exception e1)
                {
                    // ignore the exception.
                }
            }
            store= null; // free memory in all the cases.
        }

        String result =
            "Create:"
            + timeCreateRMS
            + "ms\nAdd: "
            + timeAddRMSa
            + "ms, "
            + timeAddRMSb
            + "ms\nSet: "
            + timeSetRMSa
            + "ms, "
            + timeSetRMSb
            + "ms\nGet: "
            + timeGetRMSa
            + "ms, "
            + timeGetRMSb
            + "ms\nDelete: "
            + timeDeleteRMS
            + "ms\nClose: "
            + timeCloseRMS
            + "ms\nDestroy: "
            + timeDestroyRMS
            + "ms\n"

            + "List: "
            + timeListRMSd
            + " ms "
            + rec_num
            + " recs\n"
            + "Total size: "
            + recordStoreSize
            + "Bytes\n"
            + error;
        System.out.println(result);
    }

    public void testEnumerationWithEmptyStore()
    {
        System.out.println("RecordStoreTest.testEnumerationWithEmptyStore()");
        RecordStore store = null;
        String rec_store_name = "testEnumerationWithEmptyStore";

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

            // 1: check has methods
            System.out.println("1: check has methods");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertEquals(enumeration.numRecords(), 0);
            assertTrue(enumeration.hasNextElement() == false);
            assertTrue(enumeration.hasPreviousElement() == false);

            // 2: check iteration methods
            System.out.println("2: check iteration methods");

            try
            {
                enumeration.nextRecord();
                fail("InvalidRecordIDException not thrown - nextRecord");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                enumeration.nextRecordId();
                fail("InvalidRecordIDException not thrown - nextRecordId");
            }
            catch (InvalidRecordIDException re) {}

            try
            {
                enumeration.previousRecord();
                fail("InvalidRecordIDException not thrown - previousRecord");
            }
            catch (InvalidRecordIDException re) {}
            try
            {
                enumeration.previousRecordId();
                fail("InvalidRecordIDException not thrown - previousRecordId");
            }
            catch (InvalidRecordIDException re) {}

            enumeration.destroy();

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

    public void testFilterComparator()
    {
        System.out.println("RecordStoreTest.testFilterComparator()");
        RecordStore store = null;
        String rec_store_name = "testFilterComparator";

        class FilterComparator implements RecordFilter, RecordComparator
        {
            private String iFilterStr = "filter";

            public boolean matches(byte[] aCandidate)
            {
                String str = new String(aCandidate, 0, aCandidate.length);
                return str.startsWith(iFilterStr);
            }

            public int compare(byte[] aRec1, byte[] aRec2)
            {
                String str1 = new String(aRec1, 0, aRec1.length);
                String str2 = new String(aRec2, 0, aRec2.length);

                if (str1.compareTo(str2) < 0)
                {
                    return RecordComparator.PRECEDES;
                }
                else if (str1.compareTo(str2) > 0)
                {
                    return RecordComparator.FOLLOWS;
                }
                else
                    return RecordComparator.EQUIVALENT;
            }

            public void setFilterString(String aFilter)
            {
                iFilterStr = aFilter;
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

            String initArr[] = {"Mercury", "Venus", "Earth", "Mars"};
            for (int i = 0; i < initArr.length; i++)
            {
                byte[] b = initArr[i].getBytes();
                store.addRecord(b, 0, b.length);
            }

            // 1: add record so that it goes to the end of sorted collection
            System.out.println("1: add record so that it goes to the end of sorted collection");
            FilterComparator fc = new FilterComparator();
            fc.setFilterString("M");
            RecordEnumeration enumeration = store.enumerateRecords(fc, fc, true);
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated());

            // set enum to point to the last element
            enumeration.previousRecord();
            assertTrue(!enumeration.hasNextElement());

            String newStr = "Morse";
            byte[] b = newStr.getBytes();
            int recordId = store.addRecord(b, 0, b.length);

            // check that we have new last element
            assertTrue(enumeration.hasNextElement());
            enumeration.reset();

            int numRec = enumeration.numRecords();
            String expectedResult[] = {"Mars", "Mercury", "Morse"};
            assertEquals(numRec, expectedResult.length);
            for (int i = 0; i < numRec; i++)
            {
                byte[] record = enumeration.nextRecord();
                String str = new String(record, 0, record.length);
                assertEquals(expectedResult[i], str);
            }
            store.deleteRecord(recordId); // restore init state
            enumeration.destroy();

            // 2: add record so that it goes to the beginning of sorted collection
            System.out.println("2: add record so that it goes to the beginning of sorted collection");
            enumeration = store.enumerateRecords(fc, fc, true);
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated());

            // set enum to point to the first element
            enumeration.nextRecord();
            assertTrue(!enumeration.hasPreviousElement());

            newStr = "Mama";
            b = newStr.getBytes();
            recordId = store.addRecord(b, 0, b.length);

            // check that we have new first element
            assertTrue(enumeration.hasPreviousElement());
            enumeration.reset();

            numRec = enumeration.numRecords();
            String expectedResult2[] = {"Mama", "Mars", "Mercury"};
            assertEquals(numRec, expectedResult2.length);
            for (int i = 0; i < numRec; i++)
            {
                byte[] record = enumeration.nextRecord();
                String str = new String(record, 0, record.length);
                assertEquals(expectedResult2[i], str);
            }
            store.deleteRecord(recordId); // restore init state
            enumeration.destroy();

            // 3: add record so that it goes to the middle of sorted collection
            System.out.println("3: add record so that it goes to the middle of sorted collection");
            enumeration = store.enumerateRecords(fc, fc, true);
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated());

            // set enum to point to the last element
            enumeration.previousRecord();

            newStr = "Marsman";
            b = newStr.getBytes();
            recordId = store.addRecord(b, 0, b.length);

            // check that we have correct prev element
            b = enumeration.previousRecord();
            String middle = new String(b, 0, b.length);
            assertEquals(middle, newStr);
            enumeration.reset();

            numRec = enumeration.numRecords();
            String expectedResult3[] = {"Mars", "Marsman", "Mercury"};
            assertEquals(numRec, expectedResult3.length);
            for (int i = 0; i < numRec; i++)
            {
                byte[] record = enumeration.nextRecord();
                String str = new String(record, 0, record.length);
                assertEquals(expectedResult3[i], str);
            }
            store.deleteRecord(recordId); // restore init state
            enumeration.destroy();

            // 4: add record so that it goes to the middle of sorted collection (equivalent)
            System.out.println("4: add record so that it goes to the middle of sorted collection (equivalent)");
            enumeration = store.enumerateRecords(fc, fc, true);
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated());

            newStr = "Mars";
            b = newStr.getBytes();
            recordId = store.addRecord(b, 0, b.length);

            numRec = enumeration.numRecords();
            String expectedResult4[] = {"Mars", "Mars", "Mercury"};
            assertEquals(numRec, expectedResult4.length);
            for (int i = 0; i < numRec; i++)
            {
                byte[] record = enumeration.nextRecord();
                String str = new String(record, 0, record.length);
                assertEquals(expectedResult4[i], str);
            }
            store.deleteRecord(recordId); // restore init state
            enumeration.destroy();

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

