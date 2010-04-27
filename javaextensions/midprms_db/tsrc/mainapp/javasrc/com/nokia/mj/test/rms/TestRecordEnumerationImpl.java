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
* Description: TestRecordEnumerationImpl
*
*/


package com.nokia.mj.impl.rms;

import j2meunit.framework.*;
import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;

public class TestRecordEnumerationImpl extends TestCase
{
    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestRecordEnumerationImpl("getRecordId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRecordEnumerationImpl) tc).getRecordId();
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

    public TestRecordEnumerationImpl() {}
    public TestRecordEnumerationImpl(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void getRecordId()
    {
        System.out.println("TestRecordEnumerationImpl.getRecordId()");
        String storeName = "getRecordId";
        RecordStore store = null;

        try
        {
            try
            {
                RecordStore.deleteRecordStore(storeName);
            }
            catch (Exception e) {}
            store = RecordStore.openRecordStore(storeName, true);

            RecordEnumerationImpl enumeration = (RecordEnumerationImpl)store.enumerateRecords(null, null, true);

            try
            {
                enumeration.getRecordId(0);
                fail("IllegalArgumentException not thrown, index = 0 ");
            }
            catch (IllegalArgumentException e) {}

            store.addRecord(null, 0, 0);
            assertEquals(enumeration.getRecordId(0), 1);

            try
            {
                enumeration.getRecordId(-1);
                fail("IllegalArgumentException not thrown, index = -1 ");
            }
            catch (IllegalArgumentException e) {}
            try
            {
                enumeration.getRecordId(1);
                fail("IllegalArgumentException not thrown, index = 1 ");
            }
            catch (IllegalArgumentException e) {}

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
        System.out.println("TestRecordEnumerationImpl.testCodeCoverage()");

        RmsStrings s = new RmsStrings();
        RmsFileManager fm = new RmsFileManager();

        SQLite sql = new SQLite();
        try
        {
            SQLite.checkHandle(0);
            fail("SQLite.checkHandle() did not throw NullPointerException");
        }
        catch (NullPointerException e) {}

        RecordStoreOpenArgs args = new RecordStoreOpenArgs("name", "vendor", "suite", "password");
        assertEquals(args.getPassword(), "password");

        RmsInfo info = new RmsInfo("name", 0, 0, 0, 0, 0, 0, "algorithm");
        assertEquals(info.getAlgorithm(), "algorithm");
    }

}
