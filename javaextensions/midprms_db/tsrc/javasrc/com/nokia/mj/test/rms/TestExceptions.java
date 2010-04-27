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
import com.nokia.mj.impl.rms.RmsErrorStrings;

/* This test set checks that exception messages are correct
 */
public class TestExceptions extends TestCase
{
    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestExceptions("testRecordStoreException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testRecordStoreException();
            }
        }));

        suite.addTest(new TestExceptions("testRecordStoreFullException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testRecordStoreFullException();
            }
        }));

        suite.addTest(new TestExceptions("testRecordStoreNotFoundException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testRecordStoreNotFoundException();
            }
        }));

        suite.addTest(new TestExceptions("testRecordStoreNotOpenException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testRecordStoreNotOpenException();
            }
        }));

        suite.addTest(new TestExceptions("testInvalidRecordIDException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testInvalidRecordIDException();
            }
        }));

        suite.addTest(new TestExceptions("testIllegalStateException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testIllegalStateException();
            }
        }));

        suite.addTest(new TestExceptions("testIllegalArgumentException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testIllegalArgumentException();
            }
        }));

        suite.addTest(new TestExceptions("testNullPointerException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testNullPointerException();
            }
        }));

        suite.addTest(new TestExceptions("testSecurityException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExceptions) tc).testSecurityException();
            }
        }));

        return suite;
    }

    public TestExceptions() {}
    public TestExceptions(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void testRecordStoreException()
    {
        System.out.println("TestExceptions.testRecordStoreException()");
        RecordStore store = null;
        String rec_store_name = "RecordStoreException";

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

            // 1: delete while open
            System.out.println("1: delete while open");
            try
            {
                RecordStore.deleteRecordStore(rec_store_name);
                fail("RecordStoreException not thrown");
            }
            catch (RecordStoreException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.RS_IS_OPEN + rec_store_name);
            }

            // How to test RmsStrings.READ_FAILED
            // How to test RmsStrings.RESIZE_FAILED
            // How to test RmsStrings.WRITE_FAILED
            // How to test RmsStrings.OPEN_FAILED + rs name

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

    public void testRecordStoreFullException()
    {
        System.out.println("TestExceptions.testRecordStoreFullException()");
        RecordStore store = null;
        String rec_store_name = "RecordStoreFullException";

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

            // 1: record store full
            System.out.println("1: record store full");
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
            catch (RecordStoreFullException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.FULL);
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

    public void testRecordStoreNotFoundException()
    {
        System.out.println("TestExceptions.testRecordStoreNotFoundException()");
        RecordStore store = null;
        String rec_store_name = "RecordStoreNotFoundException";

        try
        {
            // 1: Trying to open non-existing store
            System.out.println("1: Trying to open non-existing store");
            try
            {
                store = RecordStore.openRecordStore(rec_store_name, false);
                fail("RecordStoreNotFoundException expected");
            }
            catch (RecordStoreNotFoundException e)
            {
                assertTrue(e.getMessage().startsWith(RmsErrorStrings.NOT_FOUND + rec_store_name));
            }

            // 2: Trying to open non-existing store (shared)
            System.out.println("2: Trying to open non-existing store (shared)");
            String vendorName = "dummyVendor";
            String suiteName  = "dummysuite";

            try
            {
                store = RecordStore.openRecordStore(rec_store_name, vendorName, suiteName);
                fail("RecordStoreNotFoundException was not thrown");
            }
            catch (RecordStoreNotFoundException e)
            {
                String s = RmsErrorStrings.NOT_FOUND + rec_store_name + " " + vendorName + " " + suiteName;
                assertEquals(e.getMessage(), s);
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

    public void testRecordStoreNotOpenException()
    {
        System.out.println("TestExceptions.testRecordStoreNotOpenException()");
        RecordStore store = null;
        String rec_store_name = "RecordStoreNotOpenException";

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

            // 1: not open
            System.out.println("1: not open");
            store.closeRecordStore();
            try
            {
                store.getSize();
                fail("RecordStoreNotOpenException not thrown (getSize)");
            }
            catch (RecordStoreNotOpenException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.NOT_OPEN);
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

    public void testInvalidRecordIDException()
    {
        System.out.println("TestExceptions.testInvalidRecordIDException()");
        RecordStore store = null;
        String rec_store_name = "InvalidRecordIDException";

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

            // 1: using invalid id
            System.out.println("1: using invalid id");
            try
            {
                store.getRecord(-1);
                fail("InvalidRecordIDException not thrown (getRecord)");
            }
            catch (InvalidRecordIDException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.INVALID_ID + "-1");
            }

            // 2: no nextRecord
            System.out.println("2: no nextRecord");
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            try
            {
                enumeration.nextRecord();
                fail("InvalidRecordIDException not thrown (nextRecord)");
            }
            catch (InvalidRecordIDException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.NO_NEXT_RECORD);
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

    public void testIllegalStateException()
    {
        System.out.println("TestExceptions.testIllegalStateException()");
        RecordStore store = null;
        String rec_store_name = "IllegalStateException";

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

            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);
            enumeration.destroy();

            // 1: calling methods after destroy
            System.out.println("1: calling methods after destroy");
            try
            {
                enumeration.destroy();
                fail("IllegalStateException not thrown");
            }
            catch (IllegalStateException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.NOT_USABLE);
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

    public void testIllegalArgumentException()
    {
        System.out.println("TestExceptions.testIllegalArgumentException()");
        RecordStore store = null;
        String rec_store_name = "IllegalArgumentException";

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

            // 1: Invalid auth mode
            System.out.println("1: Invalid auth mode");
            try
            {
                store.setMode(-1, true);
                fail("IllegalArgumentException not thrown");
            }
            catch (IllegalArgumentException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.INVALID_MODE);
            }

            // 2: record store name is invalid
            System.out.println("2: record store name is invalid");
            try
            {
                store = RecordStore.openRecordStore("", true);
                fail("IllegalArgumentException not thrown");
            }
            catch (IllegalArgumentException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.INVALID_NAME);
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

    public void testNullPointerException()
    {
        System.out.println("TestExceptions.testNullPointerException()");
        RecordStore store = null;
        String rec_store_name = "NullPointerException";

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

            // 1: null data with non-zero length
            System.out.println("1: null data with non-zero length");
            try
            {
                store.addRecord(null, 0, 1);
                fail("NullPointerException not thrown");
            }
            catch (NullPointerException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.DATA_NULL);
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

    public void testSecurityException()
    {
        System.out.println("TestExceptions.testSecurityException()");
        RecordStore store = null;
        String rec_store_name = "SecurityException";

        try
        {
            // 1: Trying to open private store
            System.out.println("1: Trying to open private store");
            String vendorName = CreateSharedStore.VENDORNAME;
            String suiteName  = CreateSharedStore.SUITENAME;
            String storeName  = CreateSharedStore.PRIVATESTORES[0];
            try
            {
                store = RecordStore.openRecordStore(storeName, vendorName, suiteName);
                fail("SecurityException was not thrown (open)");
            }
            catch (SecurityException e)
            {
                String s = RmsErrorStrings.OPEN_FAILED + storeName + " " + vendorName + " " + suiteName;
                assertEquals(e.getMessage(), s);
            }

            // 2: Using non-writable shared store
            System.out.println("2: Using non-writable shared store");
            store = RecordStore.openRecordStore(CreateSharedStore.SHAREDSTORES[1], vendorName, suiteName);
            try
            {
                store.addRecord(null, 0, 0);
                fail("SecurityException was not thrown (add)");
            }
            catch (SecurityException e)
            {
                assertEquals(e.getMessage(), RmsErrorStrings.NOT_ALLOWED);
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