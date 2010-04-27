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
* Description: TestConcurrentAccess
*
*/


package com.nokia.mj.test.rms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;

public class TestConcurrentAccess extends TestCase
{
    public static final String VENDORNAME = "Nokia";
    public static final String SUITENAME = "InterSuite";
    public static final String MAIN_CLASS = "j2meunitomj.MIDletTestRunner";

    public static final String STORENAME = "InterSuiteAccess";
    String[] data = { "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Golf",
                      "Hotel", "India", "Juliet", "Kilo", "Lima", "Mike", "November",
                      "Oscar", "Papa", "Quebec", "Romeo", "Sierra", "Tango", "Uniform",
                      "Victor", "Whiskey", "Xray", "Yankee", "Zulu"
                    };

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestConcurrentAccess("testNotifications", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestConcurrentAccess) tc).testNotifications();
            }
        }));

        return suite;
    }

    public TestConcurrentAccess() {}
    public TestConcurrentAccess(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    class Listener implements RecordListener
    {
        public int added = 0;
        public int changed = 0;
        public int deleted = 0;

        public void recordAdded(RecordStore recordStore, int recordId)
        {
            added++;
            if (added >= data.length)
            {
                synchronized (this)
                {
                    this.notifyAll();
                }
            }
        }
        public void recordChanged(RecordStore recordStore, int recordId)
        {
            changed++;
        }
        public void recordDeleted(RecordStore recordStore, int recordId)
        {
            deleted++;
            if (deleted >= data.length)
            {
                synchronized (this)
                {
                    this.notifyAll();
                }
            }
        }

        public void doWait()
        {
            synchronized (this)
            {
                try
                {
                    this.wait(40000); // ensure that test exits at some point
                }
                catch (InterruptedException ie) {}
            }
        }
    }

    public void testNotifications()
    {
        System.out.println("TestConcurrentAccess.testNotifications()");
        try
        {
            try
            {
                RmsFileManager.deleteRecordStore(new RmsName(STORENAME, VENDORNAME, SUITENAME));
            }
            catch (RecordStoreException e) {}

            // app creates empty shared store
            AppLauncher.runApp(MAIN_CLASS, VENDORNAME, SUITENAME);

            RecordStore store = RecordStore.openRecordStore(STORENAME, VENDORNAME, SUITENAME);
            Listener listener = new Listener();
            store.addRecordListener(listener);
            RecordEnumeration enumeration = store.enumerateRecords(null, null, true);

            // launched MIDlet populates store with data
            AppLauncher.launchApp(MAIN_CLASS, VENDORNAME, SUITENAME);

            listener.doWait(); // wait for unit test midlet to add records
            assertEquals(enumeration.numRecords(), data.length);

            for (int i=0, j=data.length-1; i<data.length; i++, j--)
            {
                store.setRecord(i+1, data[j].getBytes(), 0, data[j].getBytes().length);
            }
            listener.doWait(); // wait for unit test midlet to delete all records
            assertEquals(enumeration.numRecords(), 0);

            store.closeRecordStore();

            assertEquals(listener.added, data.length);
            assertEquals(listener.changed, data.length);
            assertEquals(listener.deleted, data.length);
        }
        catch (RecordStoreException e)
        {
            fail("testNotifications() failed:" + e);
        }
    }

}
