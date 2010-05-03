/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.test.comms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestSuite;
import j2meunit.framework.TestMethod;

import com.nokia.mj.impl.comms.exception.CommsException;

public class TestCommsException extends TestCase
{

    public TestCommsException()
    {
    }

    public TestCommsException(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestCommsException("testCommsException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsException) tc).testCommsException();
            }
        }));

        return aSuite;
    }

    public void testCommsException()
    {
        System.out.println("TestCommsException.testCommsException()");
        CommsException ce = null;

        // contructor with error message
        ce = new CommsException("this is error msg");
        assertNotNull(ce);
        assertEquals(ce.getRootException(), null);

        // contructor with root exception
        ce = new CommsException("this is error msg", new Exception("this is root exception"));
        assertNotNull(ce);
        assertNotNull(ce.getRootException());
    }

}
