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

/* This test case sets up private and shared record stores that other tests
 * from different MIDlet suites can try to access
 */
public class CreateSharedStore extends TestCase
{
    // keep these in sync with rmssharedtests.jad / build.xml file
    public static final String VENDORNAME = "Nokia";
    public static final String SUITENAME = "SharedStore";

    // record store names that are shared between suites
    public static final String[] PRIVATESTORES = { "privateStore1", "privateStore2"};
    public static final String[] SHAREDSTORES = { "writable", "non-writable"};

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new CreateSharedStore("testCreatedSharedStores", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((CreateSharedStore) tc).testCreatedSharedStores();
            }
        }));

        return suite;
    }

    public CreateSharedStore() {}
    public CreateSharedStore(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    private void populateRecordStore(RecordStore aStore) throws RecordStoreException
    {
        String[] data = { "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Golf",
                          "Hotel", "India", "Juliet", "Kilo", "Lima", "Mike", "November",
                          "Oscar", "Papa", "Quebec", "Romeo", "Sierra", "Tango", "Uniform",
                          "Victor", "Whiskey", "Xray", "Yankee", "Zulu"
                        };

        for (int i=0; i<data.length; i++)
        {
            aStore.addRecord(data[i].getBytes(), 0, data[i].getBytes().length);
        }
    }

    public void testCreatedSharedStores()
    {
        System.out.println("CreateSharedStore.testCreatedSharedStores()");

        String[] privateStores = { "privateStore1",
                                   "privateStore2"
                                 };

        String[] sharedStores = { "writable",
                                  "non-writable"
                                };

        RecordStore store = null;

        try
        {
            // 0: Init
            System.out.println("0: Init");
            String[] list = RecordStore.listRecordStores();
            for (int i = 0; list != null && i<list.length; i++)
            {
                RecordStore.deleteRecordStore(list[i]);
            }

            // 1: Creating private stores
            System.out.println("1: Creating private stores");
            store = RecordStore.openRecordStore(PRIVATESTORES[0], true);
            populateRecordStore(store);
            store.closeRecordStore();

            store = RecordStore.openRecordStore(PRIVATESTORES[1], true, RecordStore.AUTHMODE_PRIVATE, true);
            populateRecordStore(store);
            store.closeRecordStore();

            // 2: Creating shared stores
            System.out.println("2: Creating shared stores");
            store = RecordStore.openRecordStore(SHAREDSTORES[0], true, RecordStore.AUTHMODE_ANY, true);
            populateRecordStore(store);
            store.closeRecordStore();

            store = RecordStore.openRecordStore(SHAREDSTORES[1], true, RecordStore.AUTHMODE_ANY, false);
            populateRecordStore(store);
            store.closeRecordStore();

            // 3: Check that stores were created ok
            System.out.println("3: Check that stores were created ok");
            list = RecordStore.listRecordStores();
            assertEquals(list.length, PRIVATESTORES.length + SHAREDSTORES.length);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
    }


}
