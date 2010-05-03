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


package com.nokia.mj.impl.storage;

import com.nokia.mj.test.storage.utils.StorageSessionTestUtils;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageAttirubte test cases. See test methods for test case details.
 */
public class TestStorageAttribute extends TestCase
        implements InstallerMain, StorageNames
{
    /**
     * Directory for JavaStorage tests.
     */
    private static final String iTestRoot = "./jstest";

    /**
     * Directory for JavaStorage journal and temp files.
     */
    private static final String iIsRoot = iTestRoot + "/js";

    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestStorageAttribute("testNameValueConstructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageAttribute)tc).testNameValueConstructor();
            }
        }));

        suite.addTest(new TestStorageAttribute("testNameValueTypeConstructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageAttribute)tc).testNameValueTypeConstructor();
            }
        }));

        suite.addTest(new TestStorageAttribute("testAttribute", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageAttribute)tc).testAttribute();
            }
        }));

        suite.addTest(new TestStorageAttribute("testtoString", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageAttribute)tc).testtoString();
            }
        }));

        suite.addTest(new TestStorageAttribute("testEscape", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageAttribute)tc).testEscape();
            }
        }));

        suite.addTest(new TestStorageAttribute("testEscapeToStorage", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageAttribute)tc).testEscapeToStorage();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestStorageAttribute()
    {
    }

    public TestStorageAttribute(String aTestName, TestMethod aTestMethod)
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
     * Test constructor with name and value.
     * 2. Test with Name and Value.
     * 3. Test Name null throws StorageException
     * 4. Test Name "" throws StorageException
     * 5. Test value null. Type is not set SE thrown.
     * 6. Test value "".
     * 7. Test one len name and value.
     * 8. Test value null, type STRING throws StorageException.
     */
    public void testNameValueConstructor()
    {
        StorageAttribute sa = new StorageAttribute("N/A", "N/A");
        // 2. Test with Name and Value
        try
        {
            String name = "AttrName";
            String value = "AttrValue";
            sa = new StorageAttribute(name, value);

            assertTrue(name.equals(sa.getName()));
            assertTrue(value.equals(sa.getValue()));
            // Default value is set if not defined.
            assertTrue(sa.getType() == StorageAttribute.STRING_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 3. Test null name
        try
        {
            String name = null;
            String value = "AttrValue";
            sa = new StorageAttribute(name, value);

            assertTrue("Null name does not throw exception", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 4. Test empty name
        try
        {
            String name = "";
            String value = "AttrValue";
            sa = new StorageAttribute(name, value);

            assertTrue("Empty name does not throw exception", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 5. Test null value.
        try
        {
            String name = "AttrName";
            String value = null;
            sa = new StorageAttribute(name, value);
            assertTrue("No exp when value Null", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("Wrong exp thrown: " + t.toString(), false);

        }

        // 6. Test empty value
        try
        {
            String name = "AttrName";
            String value = "";
            sa = new StorageAttribute(name, value);

            assertTrue(name.equals(sa.getName()));
            assertTrue(sa.getValue() == "");
            assertTrue(sa.getType() == StorageAttribute.STRING_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 7. Test one len arguments
        try
        {
            String name = "A";
            String value = "V";
            sa = new StorageAttribute(name, value);

            assertTrue(name.equals(sa.getName()));
            assertTrue(value.equals(sa.getValue()));
            // Default value is set if not defined.
            assertTrue(sa.getType() == StorageAttribute.STRING_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 8. Test value null, type STRING throws StorageException.
        try
        {
            String name = "A";
            String value = null;
            sa = new StorageAttribute(name, value);
            assertTrue("No exp thrown when null value, string type", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("Wrong exception thrown: " + t.toString(), false);
        }

    }

    /**
     * Test constructor with name, value and type.
     * 1. Test with Name, Value and Type.
     * 2. Test Name null throws StorageException
     * 3. Test Name "" throws StorageException
     * 4. Test value null. If value is not set null is returned.
     * 5. Test value "".
     * 6. Test TYPE is set.
     * 7. Test one len arguments.
     * 8. Test not supported type throws StorageException.
     */
    public void testNameValueTypeConstructor()
    {
        StorageAttribute sa = null;

        // 1. Test with Name, Value and Type.
        try
        {
            String name = "AttrName";
            String value = "AttrValue";
            sa = new StorageAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(value.equals(sa.getValue()));
            assertTrue(sa.getType() == StorageAttribute.INT_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 2. Test null name
        try
        {
            String name = null;
            String value = "AttrValue";
            sa = new StorageAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue("Null name does not throw exception", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 3. Test empty name
        try
        {
            String name = "";
            String value = "AttrValue";
            sa = new StorageAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue("Empty name does not throw exception", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 4. Test null value.
        try
        {
            String name = "AttrName";
            String value = null;
            sa = new StorageAttribute(name, value, StorageAttribute.NULL_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(sa.getValue() == null);
            assertTrue(sa.getType() == StorageAttribute.NULL_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 5. Test empty value
        try
        {
            String name = "AttrName";
            String value = "";
            sa = new StorageAttribute(name, value, StorageAttribute.STRING_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(sa.getValue() == "");
            assertTrue(sa.getType() == StorageAttribute.STRING_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 6. and 7. Test one len arguments.
        try
        {
            String name = "A";
            String value = "V";
            sa = new StorageAttribute(
                name, value, StorageAttribute.STRING_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(value.equals(sa.getValue()));
            // Default value is set if not defined.
            assertTrue(sa.getType() == StorageAttribute.STRING_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute creation failed: " + se.getMessage(), false);
        }

        // 8. Test not supported type throws StorageException.
        try
        {
            String name = NAME;
            String value = "AttrValue";
            sa = new StorageAttribute(name, value, 8);

            assertTrue("Not defined type do not throw exp", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
    }

    /**
     * Test constructor with name, value and type.
     * Getters are used to verify values.
     *
     * 1. Test with Name, Value and Type.
     * 2. Test Name null throws StorageException
     * 3. Test Name "" throws StorageException
     * 4. Test value null. If value is not set null is returned.
     * 5. Test value "".
     * 6. Test one len arguments.
     */
    public void testAttribute()
    {
        StorageAttribute sa = new StorageAttribute("N/A", "N/A");

        // 1. Test with Name, Value and Type.
        try
        {
            String name = "AttrName";
            String value = "AttrValue";
            sa.setAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(value.equals(sa.getValue()));
            assertTrue(sa.getType() == StorageAttribute.INT_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute set failed: " + se.getMessage(), false);
        }

        // 2. Test null name
        try
        {
            String name = null;
            String value = "AttrValue";
            sa.setAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue("Null name does not throw exception", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 3. Test empty name
        try
        {
            String name = "";
            String value = "AttrValue";
            sa.setAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue("Empty name does not throw exception", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 4. Test null value.
        try
        {
            String name = "AttrName";
            String value = null;
            sa.setAttribute(name, value, StorageAttribute.NULL_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(sa.getValue() == null);
            assertTrue(sa.getType() == StorageAttribute.NULL_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute set failed: " + se.getMessage(), false);
        }

        // 5. Test empty value.
        try
        {
            String name = "AttrName";
            String value = "";
            sa.setAttribute(name, value, StorageAttribute.INT_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(sa.getValue() == "");
            assertTrue(sa.getType() == StorageAttribute.INT_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute set failed: " + se.getMessage(), false);
        }

        // 6. Test one len arguments
        try
        {
            String name = "N";
            String value = "V";
            sa.setAttribute(name, value, StorageAttribute.STRING_TYPE);

            assertTrue(name.equals(sa.getName()));
            assertTrue(value.equals(sa.getValue()));
            assertTrue(sa.getType() == StorageAttribute.STRING_TYPE);
        }
        catch (StorageException se)
        {
            assertTrue("Attribute set failed: " + se.getMessage(), false);
        }
    }

    /**
     * Test toString method.
     * 1. Test with Name, Value and Type.
     * 2. Test value null.
     * 3. Test value "".
     */
    public void testtoString()
    {
        try
        {
            // 1. Test with Name, Value and Type.
            String name = "AttrName";
            String value = "AttrValue";
            StorageAttribute sa =
                new StorageAttribute(name, value, StorageAttribute.INT_TYPE);

            String refString = "AttrName='AttrValue' Type: 1";
            assertTrue(sa.toString().equals(refString));

            // 2. Test value null
            value = null;
            sa.setAttribute(name, value, StorageAttribute.NULL_TYPE);
            refString = "AttrName='NULL' Type: 3";
            assertTrue(sa.toString().equals(refString));

            // 3. Test value empty
            value = "";
            sa.setAttribute(name, value, StorageAttribute.INT_TYPE);

            refString = "AttrName='' Type: 1";
            assertTrue(sa.toString().equals(refString));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp: " + t.toString(), false);
        }
    }

    /**
     * Test character escape.
     *
     * 1. Test ' char is escaped.
     * 2. Test '' chars are escaped.
     * 3. Test 'a' chars are escaped.
     * 4. Test ' only escape char.
     * 5. Test 'a escape char first.
     * 6. Test a' escape char last.
     */
    public void testEscape()
    {
        // 1. Test ' char is escaped.
        executeEscapeTest("Tes't", "Tes''t");

        // 2. Test '' chars are escaped.
        executeEscapeTest("Tes''t", "Tes''''t");

        // 3. Test 'a' chars are escaped.
        executeEscapeTest("'a'", "''a''");

        // 4. ' char only.
        executeEscapeTest("'", "''");

        // 5. ' char start.
        executeEscapeTest("'Hello", "''Hello");

        // 6. ' char end.
        executeEscapeTest("Hello'", "Hello''");
    }

    /**
     * Test character escape. Attributes are written, searched and removed
     * using storage.
     *
     * 1. Test ' char is escaped.
     * 2. Test '' chars are escaped.
     * 3. Test 'a' chars are escaped.
     * 4. Test ' only escape char.
     * 5. Test 'a escape char first.
     * 6. Test a' escape char last.
     */
    public void testEscapeToStorage()
    {
        // 1. Test escape charcter is escaped.
        executeToStorage("escT1", "Tes't");

        // 2. Test '' chars are escaped.
        executeToStorage("escT2", "Tes''t");

        // 3. Test 'a' chars are escaped.
        executeToStorage("escT3", "'a'");

        // 4. Only ' char.
        executeToStorage("escT4", "'");

        // 5. Escape char first.
        executeToStorage("escT5", "'a'");

        // 6. Escape char last.
        executeToStorage("escT6", "a'");
    }

    private void executeEscapeTest(String aValue, String aExpectedVal)
    {
        String name = "AttrName";
        StorageAttribute sa =
            new StorageAttribute(name, aValue, StorageAttribute.INT_TYPE);

        try
        {
            assertTrue(name.equals(sa.getName()));
            assertTrue(sa.getEscapedValue().equals(aExpectedVal));
        }
        catch (StorageException se)
        {
            assertTrue("Escape fail: " + se.getMessage() + "Val: '"
                       + aValue + "'", false);
        }
    }

    private void executeToStorage(String aUid, String aValue)
    {
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName =  APPLICATION_PACKAGE_TABLE;
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(VENDOR, aValue));

            assertTrue("Populate failed",
                       iJtu.populate(iSession, tableName, aUid, entry));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            // Utils add ID to entry
            entry.removeAttribute(ID);

            assertTrue("Remove failed",
                       iJtu.remove(iSession, tableName, aUid, entry));

            iSession.rollbackTransaction();
            iSession.close();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            iSession.close();
            assertTrue("UnExp exp1: " + t.toString(), false);
        }
    }
}
