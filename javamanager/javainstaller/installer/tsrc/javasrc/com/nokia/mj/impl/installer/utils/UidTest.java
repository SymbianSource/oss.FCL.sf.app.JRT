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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Unit tests for S60 Uid.
 */
public class UidTest extends TestCase implements InstallerMain
{
    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new UidTest("testUidInsideRange", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((UidTest)tc).testUidInsideRange();
            }
        }));

        suite.addTest(new UidTest("testUidOutOfRange", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((UidTest)tc).testUidOutOfRange();
            }
        }));

        suite.addTest(new UidTest("testProtUidInUntrusted", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((UidTest)tc).testProtUidInUntrusted();
            }
        }));

        suite.addTest(new UidTest("testUidInJarForTrusted", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((UidTest)tc).testUidInJarForTrusted();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public UidTest()
    {
    }

    public UidTest(String aTestName, TestMethod aTestMethod)
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

    public void testUidInsideRange()
    {
        // Trusted uids
        checkValidJadJarCombinations(PlatformUid.createUid("0x20000000"), true);
        checkValidJadJarCombinations(PlatformUid.createUid("0x2fffffff"), true);
        checkValidJadJarCombinations(PlatformUid.createUid("0xa0000000"), true);
        checkValidJadJarCombinations(PlatformUid.createUid("0xafffffff"), true);
        checkValidJadJarCombinations(PlatformUid.createUid("0xe0000000"), true);
        checkValidJadJarCombinations(PlatformUid.createUid("0xefffffff"), true);

        // Untrusted uids
        checkValidJadJarCombinations(PlatformUid.createUid("0xa0000000"), false);
        checkValidJadJarCombinations(PlatformUid.createUid("0xafffffff"), false);
        checkValidJadJarCombinations(PlatformUid.createUid("0xe0000000"), false);
        checkValidJadJarCombinations(PlatformUid.createUid("0xefffffff"), false);
    }

    public void testUidOutOfRange()
    {
        // Trusted uids
        checkInvalidJadJarCombinations(PlatformUid.createUid("0x1fffffff"), true);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0x30000000"), true);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0x9fffffff"), true);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0xb0000000"), true);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0xdfffffff"), true);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0xf0000000"), true);

        // Untrusted uids
        checkInvalidJadJarCombinations(PlatformUid.createUid("0x1fffffff"), false);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0x30000000"), false);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0x9fffffff"), false);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0xb0000000"), false);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0xdfffffff"), false);
        checkInvalidJadJarCombinations(PlatformUid.createUid("0xf0000000"), false);
    }

    public void testProtUidInUntrusted()
    {
        checkInvalidUid(PlatformUid.createUid("0x20000001"), false, true, true, 906);
        checkInvalidUid(PlatformUid.createUid("0x20000001"), false, false, true, 907);
        checkInvalidUid(PlatformUid.createUid("0x20000001"), false, true, false, 906);
    }

    public void testUidInJarForTrusted()
    {
        checkValidUid(PlatformUid.createUid("0x20000001"), true, true, true);
        checkValidUid(PlatformUid.createUid("0x20000001"), true, false, true);
        checkInvalidUid(PlatformUid.createUid("0x20000001"), true, true, false, 907);

        checkValidUid(PlatformUid.createUid("0xa0000001"), true, true, true);
        checkValidUid(PlatformUid.createUid("0xa0000001"), true, false, true);
        checkInvalidUid(PlatformUid.createUid("0xa0000001"), true, true, false, 907);

        checkValidUid(PlatformUid.createUid("0xe0000001"), true, true, true);
        checkValidUid(PlatformUid.createUid("0xe0000001"), true, false, true);
        checkInvalidUid(PlatformUid.createUid("0xe0000001"), true, true, false, 907);
    }

    private void checkValidJadJarCombinations(Uid aUid, boolean aTrusted)
    {
        checkValidUid(aUid, aTrusted, true, true);
        checkValidUid(aUid, aTrusted, false, true);
        if (!aTrusted)
        {
            checkValidUid(aUid, aTrusted, true, false);
        }
    }

    private void checkInvalidJadJarCombinations(Uid aUid, boolean aTrusted)
    {
        checkInvalidUid(aUid, aTrusted, true, true, (aTrusted? 907: 906));
        checkInvalidUid(aUid, aTrusted, true, false, 906);
        checkInvalidUid(aUid, aTrusted, false, true, 907);
    }

    private void checkValidUid(
        Uid aUid, boolean aTrusted,
        boolean aExistsInJad, boolean aExistsInJar)
    {
        try
        {
            PlatformUid.checkValidity("TestAttr", aUid, aTrusted, aExistsInJad, aExistsInJar);
        }
        catch (InvalidAttributeException iae)
        {
            iae.printStackTrace();
            assertTrue("Expected Uid to be valid, " +
                       getUidString(aUid, aTrusted, aExistsInJad, aExistsInJar),
                       false);
        }
    }

    private void checkInvalidUid(
        Uid aUid, boolean aTrusted,
        boolean aExistsInJad, boolean aExistsInJar,
        int aOtaCode)
    {
        try
        {
            PlatformUid.checkValidity("TestAttr", aUid, aTrusted, aExistsInJad, aExistsInJar);
            assertTrue("Uid should not be valid: " +
                       getUidString(aUid, aTrusted, aExistsInJad, aExistsInJar),
                       false);
        }
        catch (InvalidAttributeException iae)
        {
            assertTrue("Wrong OTA code for " +
                       getUidString(aUid, aTrusted, aExistsInJad, aExistsInJar) +
                       ", got " + iae.getOtaStatusCode() +
                       ", expected " + aOtaCode, iae.getOtaStatusCode() == aOtaCode);
        }
    }

    private String getUidString(Uid aUid, boolean aTrusted,
                                boolean aExistsInJad, boolean aExistsInJar)
    {
        return "Uid:0x" + Long.toString(PlatformUid.getIntValue(aUid), 16) +
               ",T:" + aTrusted + ",Jad:" + aExistsInJad + ",Jar:" + aExistsInJar;
    }
}
