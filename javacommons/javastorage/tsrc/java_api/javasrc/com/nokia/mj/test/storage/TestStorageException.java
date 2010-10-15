/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.test.storage;

import com.nokia.mj.test.storage.utils.StorageSessionTestUtils;
import com.nokia.mj.impl.storage.*;

import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageSession OTA table specific test cases. See test methods for test
 * case details.
 */
public class TestStorageException extends TestCase implements UnitTestSuiteCreator, StorageNames
{
    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestStorageException("testStorageException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageException)tc).testStorageException();
            }
        }));

        return suite;
    }

    public TestStorageException()
    {
    }

    public TestStorageException(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected void setUp()
    {
        iSession = StorageFactory.createSession();
        iJtu = new StorageSessionTestUtils();
    }

    protected void tearDown()
    {
        if (iSession != null)
        {
            try
            {
                iSession.destroySession();
            }
            catch (StorageException se)
            {
                // No can do
                System.out.println("TearDown failed: " + se.toString());
            }
        }
    }

    /**
     * Test StorageException.
     *
     * 1. Test StorageException(aMsg) constructor.
     * 2. Test StorageException(aMsg, rootExp) constructor.
     * 3. Test getRootException return root exception.
     */
    public void testStorageException()
    {
        // 1. Test StorageException(aMsg) constructor.
        String expMessage = "Test Message!";
        StorageException exp = new StorageException(expMessage);
        assertTrue("Invalid message", expMessage.equals(exp.getMessage()));

        // 2. Test StorageException(aMsg, rootExp) constructor.
        String rootMessage = "Root Message!";
        Exception rootExp = new Exception(rootMessage);

        // 3. Test getRootException return root exception.
        exp = new StorageException(expMessage, rootExp);
        assertTrue("Invalid message", expMessage.equals(exp.getMessage()));

        Exception rootThrowable = (Exception) exp.getRootException();
        assertTrue("Invalid root message",
                   rootMessage.equals(rootThrowable.getMessage()));
    }
}
