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

/* Test cases for verifying that functionality is according to MIDP2.1 spec.
 * These tests will verify that argument validation is done correctly,
 * correct exceptions are thrown and behavior follows the spec
 */
public class TestRecordEnumeration extends TestCase
{

    public TestRecordEnumeration() {}

    public TestRecordEnumeration(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestRecordEnumeration("testDestroy", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testDestroy();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testHasNextElement", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testHasNextElement();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testHasPreviousElement", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testHasPreviousElement();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testIsKeptUpdated", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testIsKeptUpdated();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testKeepUpdated", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testKeepUpdated();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testNextRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testNextRecord();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testNextRecordId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testNextRecordId();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testNumRecords", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testNumRecords();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testPreviousRecord", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testPreviousRecord();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testPreviousRecordId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testPreviousRecordId();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testRebuild", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testRebuild();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testReset", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testReset();
            }
        }));

        aSuite.addTest(new TestRecordEnumeration("testEnumerationOrder", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumeration) tc).testEnumerationOrder();
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

    String[] iData = { "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Golf",
                       "Hotel", "India", "Juliet", "Kilo", "Lima", "Mike", "November",
                       "Oscar", "Papa", "Quebec", "Romeo", "Sierra", "Tango", "Uniform",
                       "Victor", "Whiskey", "Xray", "Yankee", "Zulu"
                     };

    private void populateRecordStore(RecordStore aStore) throws RecordStoreException
    {


        for (int i=0; i<iData.length; i++)
        {
            aStore.addRecord(iData[i].getBytes(), 0, iData[i].getBytes().length);
        }
    }

    class DefaultFilter implements RecordFilter
    {
        public boolean matches(byte[] candidate)
        {
            if (candidate[0] > 25)
                return true;
            else
                return false;
        }
    }

    class DefaultComparator implements RecordComparator
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

    public void testDestroy()
    {
        System.out.println("TestRecordEnumeration.testDestroy()");
        RecordStore store = null;
        String rec_store_name = "destroy";

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
            populateRecordStore(store);

            // 1: destroy
            System.out.println("1: destroy");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            enumeration.destroy();

            // 2: calling methods after destroy
            System.out.println("2: calling methods after destroy");
            try
            {
                enumeration.destroy();
                fail("IllegalStateException not thrown - destroy");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.hasNextElement();
                fail("IllegalStateException not thrown - hasNextElement");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.hasPreviousElement();
                fail("IllegalStateException not thrown - hasPreviousElement");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.isKeptUpdated();
                fail("IllegalStateException not thrown - isKeptUpdated");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.keepUpdated(true);
                fail("IllegalStateException not thrown - keepUpdated");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.nextRecord();
                fail("IllegalStateException not thrown - nextRecord");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.nextRecordId();
                fail("IllegalStateException not thrown - nextRecordId");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.numRecords();
                fail("IllegalStateException not thrown - numRecords");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.previousRecord();
                fail("IllegalStateException not thrown - previousRecord");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.rebuild();
                fail("IllegalStateException not thrown - rebuild");
            }
            catch (IllegalStateException re) {}
            try
            {
                enumeration.reset();
                fail("IllegalStateException not thrown - reset");
            }
            catch (IllegalStateException re) {}

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

    public void testHasNextElement()
    {
        System.out.println("TestRecordEnumeration.testHasNextElement()");
        RecordStore store = null;
        String rec_store_name = "hasNextElement";

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

            // 1: empty store
            System.out.println("1: empty store");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertTrue(enumeration.hasNextElement() == false);
            enumeration.destroy();

            // 2: traverse all records in the record store
            System.out.println("2: traverse all records in the record store");
            populateRecordStore(store);
            enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            int count = 0;
            while (enumeration.hasNextElement())
            {
                enumeration.nextRecordId();
                count++;
            }
            assertEquals(enumeration.numRecords(), count);
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

    public void testHasPreviousElement()
    {
        System.out.println("TestRecordEnumeration.testHasPreviousElement()");
        RecordStore store = null;
        String rec_store_name = "hasPreviousElement";

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

            // 1: empty store
            System.out.println("1: empty store");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertTrue(enumeration.hasPreviousElement() == false);
            enumeration.destroy();

            // 2: traverse all records in the record store
            System.out.println("2: traverse all records in the record store");
            populateRecordStore(store);
            enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            try
            {
                while (true)
                {
                    enumeration.nextRecordId();
                }
            }
            catch (InvalidRecordIDException e) {}
            int count = 0;
            while (enumeration.hasPreviousElement())
            {
                enumeration.previousRecordId();
                count++;
            }
            assertEquals(enumeration.numRecords(), count);
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

    public void testIsKeptUpdated()
    {
        System.out.println("TestRecordEnumeration.testIsKeptUpdated()");
        RecordStore store = null;
        String rec_store_name = "isKeptUpdated";

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
            populateRecordStore(store);

            // 1: isKeptUpdated after open
            System.out.println("1: isKeptUpdated after open");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertTrue(enumeration.isKeptUpdated() == true);
            enumeration = store.enumerateRecords(null, null, false);
            assertTrue(enumeration.isKeptUpdated() == false);
            enumeration.destroy();

            // 2: isKeptUpdated after keepUpdated
            System.out.println("2: isKeptUpdated after keepUpdated");
            enumeration = store.enumerateRecords(null, null, true);
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated() == true);
            enumeration.keepUpdated(false);
            assertTrue(enumeration.isKeptUpdated() == false);
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

    public void testKeepUpdated()
    {
        System.out.println("TestRecordEnumeration.testKeepUpdated()");
        RecordStore store = null;
        String rec_store_name = "keepUpdated";

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
            populateRecordStore(store);

            // 1: keepUpdated false (open true)
            System.out.println("1: keepUpdated false  (open true)");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertTrue(enumeration.isKeptUpdated() == true);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            enumeration.keepUpdated(false);
            store.addRecord(null, 0, 0);
            assertTrue(enumeration.isKeptUpdated() == false);
            assertEquals(enumeration.numRecords(), store.getNumRecords()-1);
            enumeration.destroy();

            // 2: keepUpdated false (open false)
            System.out.println("2: keepUpdated false  (open false)");
            enumeration = store.enumerateRecords(null, null, false);
            assertTrue(enumeration.isKeptUpdated() == false);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords()-1);
            enumeration.keepUpdated(false);
            assertTrue(enumeration.isKeptUpdated() == false);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords()-2);
            enumeration.destroy();

            // 3: keepUpdated true (open true)
            System.out.println("3: keepUpdated true  (open true)");
            enumeration = store.enumerateRecords(null, null, true);
            assertTrue(enumeration.isKeptUpdated() == true);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated() == true);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            enumeration.destroy();

            // 4: keepUpdated true (open false)
            System.out.println("4: keepUpdated true  (open false)");
            enumeration = store.enumerateRecords(null, null, false);
            assertTrue(enumeration.isKeptUpdated() == false);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords()-1);
            enumeration.keepUpdated(true);
            assertTrue(enumeration.isKeptUpdated() == true);
            store.addRecord(null, 0, 0);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
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

    public void testNextRecord()
    {
        System.out.println("TestRecordEnumeration.testNextRecord()");
        RecordStore store = null;
        String rec_store_name = "nextRecord";

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
            populateRecordStore(store);

            // 1: nextRecord
            System.out.println("1: nextRecord");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            int index = 0;
            try
            {
                while (true)
                {
                    byte[] data = enumeration.nextRecord();
                    // go back to get the record
                    if (enumeration.hasPreviousElement())
                    {
                        enumeration.previousRecordId();
                    }
                    else
                    {
                        enumeration.reset();
                    }
                    int id  = enumeration.nextRecordId();
                    byte[] orig = store.getRecord(id);
                    assertEquals(orig, data);
                    index++;
                }
            }
            catch (InvalidRecordIDException re) {}
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertEquals(index, store.getNumRecords());

            // 2: InvalidRecordIDException
            System.out.println("2: InvalidRecordIDException");
            try
            {
                enumeration.nextRecord();
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re)
            {
                try
                {
                    enumeration.nextRecord();
                    fail("InvalidRecordIDException not thrown - (2nd)");
                }
                catch (InvalidRecordIDException ie) {}
            }
            enumeration.reset();
            enumeration.nextRecord();

            // 3: RecordStoreNotOpenException
            System.out.println("3: RecordStoreNotOpenException");
            store.closeRecordStore();
            try
            {
                enumeration.nextRecord();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            enumeration.destroy();
            store = RecordStore.openRecordStore(rec_store_name, true);
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

    public void testNextRecordId()
    {
        System.out.println("TestRecordEnumeration.testNextRecordId()");
        RecordStore store = null;
        String rec_store_name = "nextRecordId";

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
            populateRecordStore(store);

            // 1: nextRecordId
            System.out.println("1: nextRecordId");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            int index = 0;
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
                    index++;
                }
            }
            catch (InvalidRecordIDException re) {}
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertEquals(index, store.getNumRecords());

            // 2: InvalidRecordIDException
            System.out.println("2: InvalidRecordIDException");
            try
            {
                enumeration.nextRecordId();
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re)
            {
                try
                {
                    enumeration.nextRecordId();
                    fail("InvalidRecordIDException not thrown - (2nd)");
                }
                catch (InvalidRecordIDException ie) {}
            }
            enumeration.reset();
            enumeration.nextRecordId();
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

    public void testNumRecords()
    {
        System.out.println("TestRecordEnumeration.testNumRecords()");
        RecordStore store = null;
        String rec_store_name = "numRecords";

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
            populateRecordStore(store);

            // 1: testNumRecords
            System.out.println("1: testNumRecords");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            try
            {
                while (true)
                {
                    int id  = enumeration.nextRecordId();
                    store.deleteRecord(id);
                    assertEquals(enumeration.numRecords(), store.getNumRecords());
                }
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

    public void testPreviousRecord()
    {
        System.out.println("TestRecordEnumeration.testPreviousRecord()");
        RecordStore store = null;
        String rec_store_name = "previousRecord";

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
            populateRecordStore(store);

            // 1: previousRecord
            System.out.println("1: previousRecord");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            int index = 0;
            try
            {
                while (true)
                {
                    enumeration.nextRecord();
                    // go back to get the record
                    if (enumeration.hasPreviousElement())
                    {
                        byte[] data = enumeration.previousRecord();
                        enumeration.nextRecord();
                        int id  = enumeration.previousRecordId();
                        byte[] orig = store.getRecord(id);
                        assertEquals(orig, data);
                        enumeration.nextRecord();
                    }
                    index++;
                }
            }
            catch (InvalidRecordIDException re) {}
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertEquals(index, store.getNumRecords());

            // 2: InvalidRecordIDException
            System.out.println("2: InvalidRecordIDException");
            while (enumeration.hasPreviousElement())
            {
                enumeration.previousRecord();
            }
            try
            {
                enumeration.previousRecord();
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re) {}

            // 3: InvalidRecordIDException
            System.out.println("3: InvalidRecordIDException - subsequent calls will throw exception");
            enumeration.destroy();
            enumeration = store.enumerateRecords(null, null, true);
            try
            {
                enumeration.nextRecord();
                enumeration.previousRecord();
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re)
            {
                try
                {
                    enumeration.previousRecord();
                    fail("InvalidRecordIDException not thrown - (2nd)");
                }
                catch (InvalidRecordIDException ie) {}
            }

            // 4: RecordStoreNotOpenException
            System.out.println("4: RecordStoreNotOpenException");
            enumeration.nextRecord();
            store.closeRecordStore();
            try
            {
                enumeration.previousRecord();
                fail("RecordStoreNotOpenException not thrown");
            }
            catch (RecordStoreNotOpenException re) {}

            enumeration.destroy();
            store = RecordStore.openRecordStore(rec_store_name, true);
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

    public void testPreviousRecordId()
    {
        System.out.println("TestRecordEnumeration.testPreviousRecordId()");
        RecordStore store = null;
        String rec_store_name = "previousRecordId";

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
            populateRecordStore(store);

            // 1: previousRecordId
            System.out.println("1: previousRecordId");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            int index = 0;
            try
            {
                while (true)
                {
                    enumeration.nextRecord();
                    // go back to get the record
                    if (enumeration.hasPreviousElement())
                    {
                        byte[] data = enumeration.previousRecord();
                        enumeration.nextRecord();
                        int id  = enumeration.previousRecordId();
                        byte[] orig = store.getRecord(id);
                        assertEquals(orig, data);
                        enumeration.nextRecord();
                    }
                    index++;
                }
            }
            catch (InvalidRecordIDException re) {}
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            assertEquals(index, store.getNumRecords());

            // 2: InvalidRecordIDException
            System.out.println("2: InvalidRecordIDException");
            while (enumeration.hasPreviousElement())
            {
                enumeration.previousRecordId();
            }
            try
            {
                enumeration.previousRecordId();
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re) {}
            enumeration.destroy();

            // 3: InvalidRecordIDException
            System.out.println("3: InvalidRecordIDException - subsequent calls will throw exception");
            enumeration = store.enumerateRecords(null, null, true);
            try
            {
                enumeration.nextRecordId();
                enumeration.previousRecordId();
                fail("InvalidRecordIDException not thrown");
            }
            catch (InvalidRecordIDException re)
            {
                try
                {
                    enumeration.previousRecordId();
                    fail("InvalidRecordIDException not thrown - (2nd)");
                }
                catch (InvalidRecordIDException ie) {}
            }
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

    public void testRebuild()
    {
        System.out.println("TestRecordEnumeration.testRebuild()");
        RecordStore store = null;
        String rec_store_name = "rebuild";

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

            // 1: rebuild (empty store)
            System.out.println("1: rebuild (empty store)");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, false);
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            enumeration.rebuild();
            assertEquals(enumeration.numRecords(), store.getNumRecords());

            // 2: rebuild
            System.out.println("2: rebuild");
            populateRecordStore(store);
            assertEquals(enumeration.numRecords(), 0);
            enumeration.rebuild();
            assertEquals(enumeration.numRecords(), store.getNumRecords());
            RecordEnumeration enumeration2 = store.enumerateRecords(null, null, false);
            int i = 0;
            try
            {
                while (true)
                {
                    assertEquals(enumeration.nextRecord(), enumeration2.nextRecord());
                    i++;
                }
            }
            catch (InvalidRecordIDException re) {}
            assertEquals(store.getNumRecords(), i);

            enumeration.destroy();
            enumeration2.destroy();
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

    public void testReset()
    {
        System.out.println("TestRecordEnumeration.testReset()");
        RecordStore store = null;
        String rec_store_name = "reset";

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

            // 1: reset (empty store)
            System.out.println("1: reset (empty store)");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            RecordEnumeration enumeration2 = store.enumerateRecords(null, null, true);
            assertTrue(enumeration.hasPreviousElement() == enumeration2.hasPreviousElement());
            assertTrue(enumeration.hasNextElement() == enumeration2.hasNextElement());
            enumeration.reset();
            assertTrue(enumeration.hasPreviousElement() == enumeration2.hasPreviousElement());
            assertTrue(enumeration.hasNextElement() == enumeration2.hasNextElement());

            // 2: reset
            System.out.println("2: reset");
            populateRecordStore(store);
            assertTrue(enumeration.hasPreviousElement() == enumeration2.hasPreviousElement());
            assertTrue(enumeration.hasNextElement() == enumeration2.hasNextElement());
            enumeration.nextRecord();
            enumeration.reset();
            assertTrue(enumeration.hasPreviousElement() == enumeration2.hasPreviousElement());
            assertTrue(enumeration.hasNextElement() == enumeration2.hasNextElement());

            enumeration.destroy();
            enumeration2.destroy();
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

    public void testEnumerationOrder()
    {
        // This test checks that record enumeration returns records in FIFO order
        // if null RecordComparator is provided
        // MIDP spec says that order is undefined in this case but many acceptance tests
        // assume this order
        System.out.println("TestRecordEnumeration.testEnumerationOrder()");
        RecordStore store = null;
        String rec_store_name = "testEnumerationOrder";

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
            populateRecordStore(store);

            // 1: check order
            System.out.println("1: check order");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, false);
            for(int i = 0; enumeration.hasNextElement(); i++)
            {
                byte[] r = enumeration.nextRecord();
                assertEquals(r, iData[i].getBytes());
            }
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

