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


package com.nokia.mj.impl.security.common;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.common.PermissionBase;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SecurityCommon unit tests.
 */
public class SecurityCommonTests extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SecurityCommonTests("testPermissionBase", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityCommonTests)tc).testPermissionBase();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SecurityCommonTests()
    {
    }

    public SecurityCommonTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    // tests for PermissionBase class
    private void testPermissionBase()
    {
        PermissionImpl p = new PermissionImpl("name");
        assertTrue(
            // null values
            p.matchActions(null, null)
            && !p.matchActions("", null)
            && !p.matchActions(null, "")
            // empty values
            && p.matchActions("", "")
            // matches found
            && p.matchActions("read", "read")
            && p.matchActions("read,write","read")
            && p.matchActions("read,open,write","write,read")
            // matches not found
            && !p.matchActions("read","write")
            && !p.matchActions("read,write","write,open"));
    }
}
