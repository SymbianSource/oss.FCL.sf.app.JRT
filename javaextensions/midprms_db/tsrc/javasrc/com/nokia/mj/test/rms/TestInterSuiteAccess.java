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
* Description: TestInterSuiteAccess
*
*/


package com.nokia.mj.test.rms;

import j2meunit.framework.*;
import javax.microedition.rms.*;

/* Purpose of this test is to ensure that record store modifications are visible
*  to different suites that have the record store open.
*  This test sets up shared record store, which is then modified by
*  this MIDlet and mainapp unit test.
*  This MIDlet should not be run as stand-alone MIDlet.
 */
public class TestInterSuiteAccess extends TestCase
{
    public static final String STORENAME = "InterSuiteAccess";

    String[] data = { "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Golf",
                      "Hotel", "India", "Juliet", "Kilo", "Lima", "Mike", "November",
                      "Oscar", "Papa", "Quebec", "Romeo", "Sierra", "Tango", "Uniform",
                      "Victor", "Whiskey", "Xray", "Yankee", "Zulu"
                    };

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestInterSuiteAccess("testInterSuiteAccess", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInterSuiteAccess) tc).testInterSuiteAccess();
            }
        }));

        return suite;
    }

    public TestInterSuiteAccess() {}
    public TestInterSuiteAccess(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void testInterSuiteAccess()
    {
        System.out.println("TestInterSuiteAccess.testInterSuiteAccess()");
        try
        {
            String[] list = RecordStore.listRecordStores();
            if (list == null)
            {
                createRecordStore();
            }
            else
            {
                doTest();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
    }

    public void createRecordStore()
    {
        System.out.println("TestInterSuiteAccess.createRecordStore()");
        try
        {
            RecordStore store = RecordStore.openRecordStore(STORENAME, true, RecordStore.AUTHMODE_ANY, true);
            store.closeRecordStore();
        }
        catch (RecordStoreException e)
        {
            fail("createRecordStore failed:" + e);
        }
    }

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
            if (changed >= data.length)
            {
                synchronized (this)
                {
                    this.notifyAll();
                }
            }
        }
        public void recordDeleted(RecordStore recordStore, int recordId)
        {
            deleted++;
        }

        public void doWait()
        {
            synchronized (this)
            {
                try
                {
                    // just to ensure that MIDlet exits at some point if ran as stand-alone
                    this.wait(1000*20); // 20s
                }
                catch (InterruptedException ie) {}
            }
        }
    }


    public void doTest()
    {
        System.out.println("TestInterSuiteAccess.doTest()");
        try
        {
            RecordStore store = RecordStore.openRecordStore(STORENAME, true);
            Listener listener = new Listener();
            store.addRecordListener(listener);

            for (int i=0; i<data.length; i++)
            {
                store.addRecord(data[i].getBytes(), 0, data[i].getBytes().length);
            }

            listener.doWait(); // wait for mainapp unit test to modify all records

            // try to change mode while other suite has store open
            try
            {
                store.setMode(RecordStore.AUTHMODE_PRIVATE, false);
                fail("setMode() did not throw IllegalStateException");
            }
            catch (IllegalStateException e) {}

            for (int i=0; i<data.length; i++)
            {
                store.deleteRecord(i+1);
            }

            store.closeRecordStore();
            assertEquals(listener.added, data.length);
            assertEquals(listener.changed, data.length);
            assertEquals(listener.deleted, data.length);
        }
        catch (RecordStoreException e)
        {
            fail("doTest failed:" + e);
        }
    }

}
