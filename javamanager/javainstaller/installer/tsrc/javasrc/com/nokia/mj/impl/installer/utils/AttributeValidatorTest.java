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

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.Attribute;

import java.util.Hashtable;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * AttributeValidator unit tests.
 */
public class AttributeValidatorTest extends TestCase implements InstallerMain
{
    private MidpAttributeValidator iMav = null;

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new AttributeValidatorTest("testMidletN", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testMidletN();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testNokiaMidletUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testNokiaMidletUid();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testMidletJarSize", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testMidletJarSize();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testMidletDataSize", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testMidletDataSize();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testMidletVersion", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testMidletVersion();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testRuntimeExecutionEnvironment", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testRuntimeExecutionEnvironment();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testMicroEditionProfile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testMicroEditionProfile();
            }
        }));

        suite.addTest(new AttributeValidatorTest("testMicroEditionConfiguration", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeValidatorTest)tc).testMicroEditionConfiguration();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public AttributeValidatorTest()
    {
    }

    public AttributeValidatorTest(String aTestName, TestMethod aTestMethod)
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
        iMav = new MidpAttributeValidator();
    }

    protected void tearDown()
    {
        iMav = null;
    }

    public void testMidletN()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "MIDlet-1";
        assertValidValue(tbl, attrName, " M1 , \t  ,  p.M1 ");
        assertValidValue(tbl, attrName, " M1 ,  /m1icon.png  ,  \t p.M1 ");
        assertInvalidValue(tbl, attrName, ", , p.M1"); // missing name
        assertInvalidValue(tbl, attrName, "M1, , "); // missing class
        assertInvalidValue(tbl, attrName, "M1, , p.M1, extra"); // extra part
    }

    public void testNokiaMidletUid()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "Nokia-MIDlet-UID-1";
        assertValidValue(tbl, attrName, "0xeeff0000");
        assertValidValue(tbl, attrName, "[eeff0000]");
        assertValidValue(tbl, attrName, "12345678");
    }

    public void testMidletJarSize()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "MIDlet-Jar-Size";
        assertValidValue(tbl, attrName, "0");
        assertValidValue(tbl, attrName, "100");
        assertInvalidValue(tbl, attrName, ""); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, " "); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, "100a");
        assertInvalidValue(tbl, attrName, "0x100");
    }

    public void testMidletDataSize()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "MIDlet-Data-Size";
        assertValidValue(tbl, attrName, "0");
        assertValidValue(tbl, attrName, "100");
        assertInvalidValue(tbl, attrName, ""); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, " "); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, "100a");
        assertInvalidValue(tbl, attrName, "0x100");
    }

    public void testMidletVersion()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "MIDlet-Version";
        assertValidValue(tbl, attrName, "1");
        assertValidValue(tbl, attrName, "1."); // OPEN: Is this valid? If we are lenient it is.
        assertValidValue(tbl, attrName, "1.1");
        assertValidValue(tbl, attrName, "1.1."); // OPEN: Is this valid? If we are lenient it is.
        assertValidValue(tbl, attrName, "1.1.1");
        assertInvalidValue(tbl, attrName, ""); // invalid, attr is mandatory
        assertInvalidValue(tbl, attrName, " "); // invalid, attr is mandatory
        assertInvalidValue(tbl, attrName, "1.1:1"); // invalid char
        assertInvalidValue(tbl, attrName, "1.1.1."); // invalid, only three parts allowed
        assertInvalidValue(tbl, attrName, "1.1.1.1"); // invalid, only three parts allowed
        assertInvalidValue(tbl, attrName, "abc"); // invalid chars
    }

    public void testRuntimeExecutionEnvironment()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "Runtime-Execution-Environment";
        assertValidValue(tbl, attrName, "MIDP.CLDC");
        assertValidValue(tbl, attrName, ""); // valid, empty value is accepted
        assertValidValue(tbl, attrName, " "); // valid, empty value is accepted
        assertInvalidValue(tbl, attrName, "MIDP.CDC");
    }

    public void testMicroEditionProfile()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "MicroEdition-Profile";
        assertValidValue(tbl, attrName, "MIDP-1.0");
        assertValidValue(tbl, attrName, "MIDP-2.0");
        assertValidValue(tbl, attrName, "MIDP-2.1");
        assertValidValue(tbl, attrName, "MIDP-1.0 MIDP-1.1 MIDP-2.1");
        assertValidValue(tbl, attrName, "  MIDP-1.0  \t MIDP-1.1   MIDP-2.1 ");
        assertInvalidValue(tbl, attrName, ""); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, " "); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, "J2SE-1.3");
        assertInvalidValue(tbl, attrName, "MIDP-1.0 MIDP-3.0");
        assertInvalidValue(tbl, attrName, "MIDP-2.2");
        assertInvalidValue(tbl, attrName, "MIDP-2.1.1");
        assertInvalidValue(tbl, attrName, "MIDP-2.1.0.1");
    }

    public void testMicroEditionConfiguration()
    {
        Hashtable tbl = getTestTbl();
        String attrName = "MicroEdition-Configuration";
        assertValidValue(tbl, attrName, "CLDC-1.0");
        assertValidValue(tbl, attrName, "CLDC-1.1");
        assertInvalidValue(tbl, attrName, ""); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, " "); // invalid, empty value is not accepted
        assertInvalidValue(tbl, attrName, "CLDC-1.1.0.1");
        assertInvalidValue(tbl, attrName, "CLDC-1.2");
        assertInvalidValue(tbl, attrName, "CLDC-1.1 CLDC-1.2");
        assertInvalidValue(tbl, attrName, "CDC-1.1");
    }

    /**
     * Asserts that given attribute value is valid.
     */
    private void assertValidValue(Hashtable aTbl, String aName, String aValue)
    {
        aTbl.put(aName, new Attribute(aName, aValue, false));
        assertTrue(aName + ": " + aValue + " ==> expected to succeed",
                   isValid(aTbl, MidpAttributeValidator.JAR));
    }

    /**
     * Asserts that given attribute value is invalid.
     */
    private void assertInvalidValue(Hashtable aTbl, String aName, String aValue)
    {
        aTbl.put(aName, new Attribute(aName, aValue, false));
        assertTrue(aName + ": " + aValue + " ==> expected to fail",
                   !isValid(aTbl, MidpAttributeValidator.JAR));
    }

    /**
     * Returns true if given attribute table is validated succesfully,
     * false otherwise.
     */
    private boolean isValid(Hashtable aTbl, int aFileType)
    {
        boolean result = false;
        try
        {
            iMav.validate(aTbl, aFileType);
            result = true; // Validation succeeded.
        }
        catch (Exception ex)
        {
            // Validation failed.
            //System.out.println(ex);
        }
        return result;
    }

    /**
     * Constructs a default attribute table for test cases.
     */
    private static Hashtable getTestTbl()
    {
        Hashtable tbl = new Hashtable();
        tbl.put("MIDlet-Name", new Attribute("MIDlet-Name", "M1", false));
        tbl.put("MIDlet-Vendor", new Attribute("MIDlet-Vendor", "Unknown", false));
        tbl.put("MIDlet-Version", new Attribute("MIDlet-Version", "1.0", false));
        tbl.put("MIDlet-1", new Attribute("MIDlet-1", "M1, , p.M1", false));
        return tbl;
    }
}
