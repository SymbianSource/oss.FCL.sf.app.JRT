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
* Description: TestPackageProtectedMethods
*
*/


package javax.microedition.rms;

import j2meunit.framework.*;
import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;

public class TestPackageProtectedMethods extends TestCase
{
    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestPackageProtectedMethods("testAddSetTag", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPackageProtectedMethods) tc).testAddSetTag();
            }
        }));

        suite.addTest(new TestPackageProtectedMethods("testGetRecordStoreInfo", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPackageProtectedMethods) tc).testGetRecordStoreInfo();
            }
        }));

        suite.addTest(new TestPackageProtectedMethods("testImportExport", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPackageProtectedMethods) tc).testImportExport();
            }
        }));

        suite.addTest(new TestPackageProtectedMethods("testOpenRecordStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPackageProtectedMethods) tc).testOpenRecordStore();
            }
        }));

        suite.addTest(new TestPackageProtectedMethods("testEnumerationWithTags", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPackageProtectedMethods) tc).testEnumerationWithTags();
            }
        }));

        suite.addTest(new TestPackageProtectedMethods("testCodeCoverage", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPackageProtectedMethods) tc).testCodeCoverage();
            }
        }));

        return suite;
    }

    public TestPackageProtectedMethods() {}
    public TestPackageProtectedMethods(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void testAddSetTag()
    {
        System.out.println("TestPackageProtectedMethods.testAddSetTag()");
        String storeName = "testAddSetTag";
        RecordStore store = null;

        try
        {
            // 0: Init
            System.out.println("0: Init");
            try
            {
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
            store = RecordStore.openRecordStore(storeName, true);

            // 1: Add record without tag
            System.out.println("1: Add record without tag");
            byte data[] = storeName.getBytes();
            int recordId = store.addRecord(data, 0, data.length);
            int tag = store.getTag(recordId);
            assertEquals(tag, 0);

            // 2: Set record without tag
            System.out.println("2: Set record without tag");
            store.setRecord(recordId, data, 0, data.length);
            tag = store.getTag(recordId);
            assertEquals(tag, 0);

            // 3: Set record with tag
            System.out.println("3: Set record with tag");
            store.setRecord(recordId, data, 0, data.length, 1234);
            tag = store.getTag(recordId);
            assertEquals(tag, 1234);

            // 4: Add record with tag
            System.out.println("4: Add record with tag");
            recordId = store.addRecord(data, 0, data.length, 4321);
            tag = store.getTag(recordId);
            assertEquals(tag, 4321);

            // 5: Set record without tag (keep existing)
            System.out.println("5: Set record without tag (keep existing)");
            store.setRecord(recordId, data, 0, data.length);
            tag = store.getTag(recordId);
            assertEquals(tag, 4321);

            // 6: Set record with tag (new tag)
            System.out.println("6: Set record with tag (new tag)");
            store.setRecord(recordId, data, 0, data.length, 5555);
            tag = store.getTag(recordId);
            assertEquals(tag, 5555);
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
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
        }
    }

    public void testGetRecordStoreInfo()
    {
        System.out.println("TestPackageProtectedMethods.testGetRecordStoreInfo()");
        String storeName = "testGetRecordStoreInfo";
        RecordStore store = null;

        try
        {
            try
            {
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
            store = RecordStore.openRecordStore(storeName, true);

            // 1: RecordStoreInfo
            System.out.println("1: RecordStoreInfo");
            RecordStoreInfo info = store.getRecordStoreInfo();
            assertEquals(info.getAuthMode(), RecordStore.AUTHMODE_PRIVATE);
            assertEquals((int)info.getSize(), store.getSize());
            assertEquals((int)info.getSizeAvailable(), store.getSizeAvailable());
            assertTrue(!info.isEncrypted());
            assertTrue(info.isWriteable());

            // 2: RecordStoreInfo when closed
            System.out.println("2: RecordStoreInfo when closed");
            store.closeRecordStore();
            try
            {
                store.getRecordStoreInfo();
                fail("RecordStoreNotOpenException not thrown getRecordStoreInfo()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                info.getAuthMode();
                fail("RecordStoreNotOpenException not thrown getAuthMode()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                info.getSize();
                fail("RecordStoreNotOpenException not thrown getSize()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                info.getSizeAvailable();
                fail("RecordStoreNotOpenException not thrown getSizeAvailable()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                info.isEncrypted();
                fail("RecordStoreNotOpenException not thrown isEncrypted()");
            }
            catch (RecordStoreNotOpenException re) {}
            try
            {
                info.isWriteable();
                fail("RecordStoreNotOpenException not thrown isWriteable()");
            }
            catch (RecordStoreNotOpenException re) {}
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
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
        }
    }

    public void testImportExport()
    {
        System.out.println("TestPackageProtectedMethods.testImportExport()");
        String storeName = "testImportExport";
        RecordStore store = null;

        try
        {
            try
            {
                RecordStore.exportRecordStore(null, "", "", "");
                fail("RecordStoreException not thrown exportRecordStore()");
            }
            catch (RecordStoreException re) {}
            try
            {
                RecordStore.importRecordStore(null, "", "");
                fail("RecordStoreException not thrown importRecordStore()");
            }
            catch (RecordStoreException re) {}
        }
        catch (Exception e)
        {
            fail("Unexpected exception " + e);
        }
    }

    public void testOpenRecordStore()
    {
        System.out.println("TestPackageProtectedMethods.testOpenRecordStore()");
        String storeName = "testOpenRecordStore";
        RecordStore store = null;

        try
        {
            store = RecordStore.openRecordStore(storeName, true, RecordStore.AUTHMODE_PRIVATE,
                                                true, "dummyPassword");
            store.closeRecordStore();

            RmsName name = new RmsName(storeName);
            store = RecordStore.openRecordStore(name.getName(), name.getVendorName(),
                                                name.getSuiteName(), "dummyPassword");
            store.closeRecordStore();
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
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
        }
    }

    public void testEnumerationWithTags()
    {
        System.out.println("TestPackageProtectedMethods.testEnumerationWithTags()");
        String storeName = "testEnumerationWithTags";
        RecordStore store = null;

        class Filter implements RecordFilter
        {
            public boolean matches(byte[] candidate)
            {
                if (candidate == null || candidate.length % 2 == 0)
                    return true;
                else
                    return false;
            }
        }

        class Comparator implements RecordComparator
        {
            public int compare(byte[] rec1, byte[] rec2)
            {
                if (rec1 == null) rec1 = new byte[0];
                if (rec2 == null) rec2 = new byte[0];

                if (rec1.length < rec2.length)
                    return RecordComparator.PRECEDES;
                else if (rec1.length > rec2.length)
                    return RecordComparator.FOLLOWS;
                else
                    return RecordComparator.EQUIVALENT;
            }
        }

        try
        {
            try
            {
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
            store = RecordStore.openRecordStore(storeName, true);

            int[] tags = new int[10];
            int[] tags2 = new int[10];
            for (int i = 0; i < tags.length; i++)
            {
                tags[i] = i;
                tags2[i] = 50;
            }

            RecordEnumeration enum1 = store.enumerateRecords(null, null, true, null);
            RecordEnumeration enum2 = store.enumerateRecords(new Filter(), null, true, new int[0]);
            RecordEnumeration enum3 = store.enumerateRecords(null, new Comparator(), true, tags);
            RecordEnumeration enum4 = store.enumerateRecords(new Filter(), new Comparator(), true, tags2);

            int[] tags3 = new int[100];
            for (int i = 0; i < tags3.length; i++)
            {
                tags3[i] = i;
                byte[] data = new byte[i];
                store.addRecord(data, 0, data.length, i);
            }
            RecordEnumeration enum5 = store.enumerateRecords(null, null, true, tags3);
            RecordEnumeration enum6 = store.enumerateRecords(new Filter(), new Comparator(), true, tags3);

            assertEquals(enum1.numRecords(), 100);
            assertEquals(enum2.numRecords(), 0);
            assertEquals(enum3.numRecords(), 10);
            assertEquals(enum4.numRecords(), 1);
            assertEquals(enum5.numRecords(), 100);
            assertEquals(enum6.numRecords(), 50);

            store.closeRecordStore();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            fail("Exception not expected " + ex);
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
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
        }
    }

    public void testCodeCoverage()
    {
        System.out.println("TestPackageProtectedMethods.testCodeCoverage()");

        SecureRecordStoreException e1 = new SecureRecordStoreException();
        SecureRecordStoreException e2 = new SecureRecordStoreException("dummy");
    }

}
