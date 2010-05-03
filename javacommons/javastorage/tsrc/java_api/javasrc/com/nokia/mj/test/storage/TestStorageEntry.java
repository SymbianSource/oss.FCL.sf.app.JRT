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

import com.nokia.mj.impl.installer.utils.InstallerMain;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;
import java.util.Enumeration;

/**
 * StorageSession connection test cases. See test methods for test case details.
 */
public class TestStorageEntry extends TestCase implements InstallerMain
{
    /**
     * Directory for JavaStorage tests.
     */
    private static final String iTestRoot = "./jstest";

    /**
     * Directory for JavaStorage journal and temp files.
     */
    private static final String iIsRoot = iTestRoot + "/js";


    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestStorageEntry("testAddAttribute", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageEntry)tc).testAddAttribute();
            }
        }));

        suite.addTest(new TestStorageEntry("testElements", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageEntry)tc).testElements();
            }
        }));

        suite.addTest(new TestStorageEntry("testGetAttribute", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageEntry)tc).testGetAttribute();
            }
        }));

        suite.addTest(new TestStorageEntry("testRemoveByObject", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageEntry)tc).testRemoveByObject();
            }
        }));

        suite.addTest(new TestStorageEntry("testRemoveByName", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageEntry)tc).testRemoveByName();
            }
        }));

        suite.addTest(new TestStorageEntry("testToString", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestStorageEntry)tc).testToString();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestStorageEntry()
    {
    }

    public TestStorageEntry(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    /**
     * Test connection open
     * 1. Test adding null throws StorageException.
     * 2. Test add attribute.
     * 3. Test add second attribute.
     * 4. Test add same attribute again does not override previous.
     * 5. Test remove all.
     */
    public void testAddAttribute()
    {
        StorageAttribute sa = null;
        StorageEntry entry = new StorageEntry();

        // 1. Test add null.
        try
        {
            entry.addAttribute(sa);
            assertTrue("Adding null attribute does not throw exp.", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 2. Add attribute
        try
        {
            StorageAttribute attr = new StorageAttribute(
                "Name", "Value", StorageAttribute.STRING_TYPE);
            entry.addAttribute(attr);

            assertTrue("Invalid size after adding one attr",
                       entry.size() == 1);

            StorageAttribute added = entry.getAttribute(attr.getName());
            assertTrue("Attribute was not succesfully added",
                       attr.equals(added));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp1: " + t.toString(), false);
        }

        // 3. Add second attribute
        try
        {
            StorageAttribute attr = new StorageAttribute(
                "Name2", "Value2", StorageAttribute.INT_TYPE);
            entry.addAttribute(attr);

            assertTrue("Invalid size after adding second attr",
                       entry.size() == 2);

            StorageAttribute added = entry.getAttribute(attr.getName());
            assertTrue("Attribute was not succesfully added",
                       attr.equals(added));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp2: " + t.toString(), false);
        }

        // 4. Add second attribute again
        try
        {
            StorageAttribute attr = new StorageAttribute(
                "Name3", "Value3", StorageAttribute.INT_TYPE);
            entry.addAttribute(attr);

            assertTrue("Invalid size after adding third attr",
                       entry.size() == 3);

            StorageAttribute added = entry.getAttribute(attr.getName());

            assertTrue("No match", attr.equals(added));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp3: " + t.toString(), false);
        }

        // 5. Clean i.e. test remove all.
        entry.removeAll();
        assertTrue(entry.size() == 0);
    }

    /**
     * Test connection open
     * 1. Test elements with empty entry.
     * 2. Test elements when one attribute.
     * 3. Test elements when two attributes
     * 4. Test elements after remove all.
     */
    public void testElements()
    {
        StorageEntry entry = new StorageEntry();

        // 1. Elements when empty entry.
        try
        {
            Enumeration attrs = entry.elements();
            assertTrue(attrs.hasMoreElements() == false);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp1: " + t.toString(), false);
        }

        // 2. Test elements when one entry.
        try
        {
            StorageAttribute attr = new StorageAttribute(
                "Name", "Value", StorageAttribute.STRING_TYPE);
            entry.addAttribute(attr);

            assertTrue("Invalid size after adding one attr",
                       entry.size() == 1);

            Enumeration attrs = entry.elements();
            assertTrue(attrs.hasMoreElements() == true);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp2: " + t.toString(), false);
        }

        // 3. Test elements when two attrs.
        try
        {
            entry.removeAll();
            StorageAttribute attr = new StorageAttribute(
                "Name", "Value", StorageAttribute.STRING_TYPE);
            entry.addAttribute(attr);

            StorageAttribute attr2 = new StorageAttribute(
                "Name2", "Value2", StorageAttribute.STRING_TYPE);
            entry.addAttribute(attr2);

            assertTrue("Invalid size after adding one attr",
                       entry.size() == 2);

            Enumeration attrs = entry.elements();
            assertTrue(attrs.hasMoreElements() == true);
            StorageAttribute sa = (StorageAttribute) attrs.nextElement();
            assertTrue(sa.equals(attr));

            assertTrue(attrs.hasMoreElements() == true);
            sa = (StorageAttribute) attrs.nextElement();
            assertTrue(sa.equals(attr2));

            assertTrue(attrs.hasMoreElements() == false);
        }
        catch (Throwable t)
        {
            assertTrue("Unexp exp3: " + t.toString(), false);
        }

        // 4. Test after remove all
        try
        {
            entry.removeAll();
            Enumeration attrs = entry.elements();
            assertTrue(attrs.hasMoreElements() == false);
        }
        catch (Throwable t)
        {
            assertTrue("Unexp exp4: " + t.toString(), false);
        }
    }

    /**
     * Test getAttribute.
     * 1. Get attribute empty entry returns null.
     * 2. Get attribute null name throws StorageException.
     * 3. Get attribute empty name throws StorageException.
     * 4. Get attribute no match returns null.
     * 5. Get attribute.
     * 6. Two attributes. Get attributes.
     * 7. Remove attribute. Get attribute. Other one returns null.
     * 8. Remove all. Get attribute returns null.
     */
    public void testGetAttribute()
    {
        // 1. Get attribute empty entry returns null.
        StorageEntry entry = new StorageEntry();
        assertTrue(entry.getAttribute("Name") == null);

        // 2. Get attribute null name throws StorageException.
        try
        {
            entry.getAttribute(null);
            assertTrue("getAttribute null name does not throw exp.",
                       false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp2: " + t.toString(), false);
        }

        // 3. Get attribute empty name throws StorageException.
        try
        {
            entry.getAttribute("");
            assertTrue("getAttribute empty name does not throw exp.",
                       false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp3: " + t.toString(), false);
        }

        // 4. Get attribute no match returns null.
        entry = new StorageEntry();
        StorageAttribute attribute = new StorageAttribute(
            "Name",
            "Value",
            StorageAttribute.INT_TYPE);

        entry.addAttribute(attribute);
        assertTrue(entry.size() == 1);

        try
        {
            assertTrue(entry.getAttribute("I Do not exists!") == null);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp4: " + t.toString(), false);
        }

        // 5. Get attribute.
        try
        {
            StorageAttribute resAttr = entry.getAttribute("Name");
            assertTrue(attribute.equals(resAttr));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp5: " + t.toString(), false);
        }

        // 6. Two attributes. Get attributes.
        StorageAttribute attribute2 = new StorageAttribute(
            "Name2",
            "Value2",
            StorageAttribute.INT_TYPE);

        entry.addAttribute(attribute2);
        assertTrue(entry.size() == 2);

        try
        {
            StorageAttribute resAttr = entry.getAttribute("Name");
            assertTrue(attribute.equals(resAttr));

            resAttr = entry.getAttribute("Name2");
            assertTrue(attribute2.equals(resAttr));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp6: " + t.toString(), false);
        }

        // 7. Remove attribute. Get attribute. Other one returns null.
        try
        {
            assertTrue(entry.removeAttribute(attribute));
            assertTrue(entry.size() == 1);

            StorageAttribute resAttr = entry.getAttribute("Name");
            assertTrue(resAttr == null);

            resAttr = entry.getAttribute("Name2");
            assertTrue(attribute2.equals(resAttr));
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp7: " + t.toString(), false);
        }

        // 8. Remove all. Get attribute returns null.
        try
        {
            entry.removeAll();
            assertTrue(entry.getAttribute("Name") == null);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp8: " + t.toString(), false);
        }
    }

    /**
     * Test atturibute removal.
     * 1. Test when empty entry returns false.
     * 2. Test remove null object returns false.
     * 3. Test remove.
     * 4. Test remove between.
     * 5. Test remove when attribute has null value.
     * 6. Test remove when attribute has empty value.
     */
    public void testRemoveByObject()
    {
        // 1. Test when empty entry returns false.
        StorageEntry entry = new StorageEntry();
        StorageAttribute attribute = new StorageAttribute(
            "Name",
            "Value",
            StorageAttribute.INT_TYPE);
        assertTrue(entry.removeAttribute(attribute) == false);

        // 2. Test remove null object returns false.
        StorageAttribute nullAttr = null;
        assertTrue(entry.removeAttribute(nullAttr) == false);


        // 3. Test remove.
        try
        {
            entry.addAttribute(attribute);
            assertTrue(entry.size() == 1);
            assertTrue(entry.removeAttribute(attribute));
            assertTrue(entry.size() == 0);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp3: " + t.toString(), false);
        }

        // 4. Test remove between.
        try
        {
            entry.addAttribute(attribute);
            assertTrue(entry.size() == 1);

            StorageAttribute attribute2 = new StorageAttribute(
                "Name2",
                "Value2",
                StorageAttribute.INT_TYPE);

            entry.addAttribute(attribute2);
            assertTrue(entry.size() == 2);

            StorageAttribute attribute3 = new StorageAttribute(
                "Name3",
                "Value3",
                StorageAttribute.INT_TYPE);

            entry.addAttribute(attribute3);
            assertTrue(entry.size() == 3);

            assertTrue(entry.removeAttribute(attribute2));
            assertTrue(entry.size() == 2);
            // Check both remaining attrs.
            assertTrue(entry.getAttribute("Name").equals(attribute));
            assertTrue(entry.getAttribute("Name3").equals(attribute3));
        }
        catch (Throwable t)
        {
            assertTrue("Unexp exp4: " + t.toString(), false);
        }

        // 5. Test remove when attribute has null value.
        try
        {
            entry.removeAll();
            String name = null;
            StorageAttribute attrNullVal = new StorageAttribute(
                "Name2",
                name,
                StorageAttribute.NULL_TYPE);
            entry.addAttribute(attrNullVal);
            assertTrue(entry.size() == 1);

            // Null is converted to empty string by StorageAttribute.
            StorageAttribute refAttr = new StorageAttribute(
                "Name2",
                null,
                StorageAttribute.NULL_TYPE);

            assertTrue(entry.removeAttribute(refAttr));
            assertTrue(entry.size() == 0);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp5: " + t.toString(), false);
        }

        // 6. Test remove when attribute has empty value.
        try
        {
            StorageAttribute attrEmptyVal = new StorageAttribute(
                "Name2",
                "",
                StorageAttribute.INT_TYPE);
            entry.addAttribute(attrEmptyVal);
            assertTrue(entry.size() == 1);

            assertTrue(entry.removeAttribute(attrEmptyVal));
            assertTrue(entry.size() == 0);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp6: " + t.toString(), false);
        }
    }

    /**
     * Test attribute removal by name.
     * 1. Test when empty entry returns false.
     * 2. Test remove by null name returns false.
     * 3. Test remove by empty name returns false.
     * 4. Test remove.
     */
    public void testRemoveByName()
    {
        // 1. Test when empty entry returns false.
        StorageEntry entry = new StorageEntry();
        StorageAttribute attribute = new StorageAttribute(
            "Name",
            "Value",
            StorageAttribute.INT_TYPE);

        assertTrue(entry.removeAttribute(attribute) == false);

        // 2. Test remove by null name returns false.
        String name = null;
        assertTrue(entry.removeAttribute(name) == false);

        // 3. Test remove by empty name returns false.
        name = "";
        assertTrue(entry.removeAttribute(name) == false);

        // 4. Test remove.
        try
        {
            name = "Name";
            entry.addAttribute(attribute);
            assertTrue(entry.size() == 1);
            assertTrue(entry.removeAttribute(name));
            assertTrue(entry.size() == 0);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exp4: " + t.toString(), false);
        }

        entry.removeAll();
    }

    /**
     * Test toString method.
     * 1. Test empty.
     * 2. Test one attribute.
     * 3. Test two attributes.
     */
    public void testToString()
    {
        // 1. Test empty.
        StorageEntry entry = new StorageEntry();
        String refString = "{}";
        assertTrue(entry.toString().equals(refString));

        // 2. Test one attribute.
        StorageAttribute attr = new StorageAttribute(
            "Na", "Va", StorageAttribute.INT_TYPE);
        entry.addAttribute(attr);

        refString = "{Na=Na='Va' Type: 1}";
        assertTrue(entry.toString().equals(refString));

        // 3. Test two attributes.
        attr = new StorageAttribute(
            "Na2", "Va2", StorageAttribute.STRING_TYPE);
        entry.addAttribute(attr);

        refString = "{Na=Na='Va' Type: 1, Na2=Na2='Va2' Type: 2}";
        assertTrue(entry.toString().equals(refString));
    }
}
