/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.utils;

import java.util.Date;

import com.nokia.mj.impl.utils.ResourceLoader;
import com.nokia.mj.impl.utils.Formatter;

import com.nokia.mj.impl.utils.Logger;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

public class LocalisationTest extends TestCase implements InstallerMain
{
    // Must have no-argument constructor for Class.forname()
    public LocalisationTest()
    {
    }

    public LocalisationTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        com.nokia.mj.impl.utils.OmjTestRunner.run(suite());
    }

    public Test suite()
    {
        // TestSuite aSuite = new TestSuite();
        TestSuite aSuite = new TestSuite(this.getClass().getName());

        aSuite.addTest(new LocalisationTest("testFormatDigits", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).testFormatDigits();
            }
        }));

        aSuite.addTest(new LocalisationTest("testFormatterAvkon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).testFormatterAvkon();
            }
        }));

        aSuite.addTest(new LocalisationTest("testResourceLoaderAvkon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).testResourceLoaderAvkon();
            }
        }));

        if (ResourceLoader.getLocaleIdQt() != null)
        {
            aSuite.addTest(new LocalisationTest("testFormatterQt", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((LocalisationTest) tc).testFormatterQt();
                }
            }));

            aSuite.addTest(new LocalisationTest("testResourceLoaderQt", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((LocalisationTest) tc).testResourceLoaderQt();
                }
            }));

            aSuite.addTest(new LocalisationTest("testNumerusQt", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((LocalisationTest) tc).testNumerusQt();
                }
            }));
        }

        aSuite.addTest(new LocalisationTest("testResourceLoaderCreate", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).testResourceLoaderCreate();
            }
        }));

        aSuite.addTest(new LocalisationTest("testResourceLoaderDestroy", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).testResourceLoaderDestroy();
            }
        }));

        return aSuite;
    }

    void testFormatDigits()
    {
        String str = "Message from number +358101234567 received.";
        // Test are executed with english locale,
        // numbers should not be changed.
        assertEquals(str, FormatterAvkon.formatDigits(str));
        assertEquals(str, FormatterQt.formatDigits(str));
    }

    void testFormatterAvkon()
    {
        // Zero argument
        assertEquals(
            "Test",
            new FormatterAvkon("Test").toString());

        // One argument tests
        assertEquals(
            "Test 100",
            new FormatterAvkon("Test %0N").arg(100).toString());

        assertEquals(
            "Test 1",
            new FormatterAvkon("Test %C").arg('1').toString());

        assertEquals(
            "Test A",
            new FormatterAvkon("Test %0C").arg('A').toString());

        // Two argument tests
        assertEquals(
            "Test test 100",
            new FormatterAvkon("Test %1U %0N").arg(100).arg("test").toString());

        assertEquals(
            "100 test test",
            new FormatterAvkon("%0 test %1").arg(100).arg("test").toString());

        assertEquals(
            "Test A B C",
            new FormatterAvkon("Test %0C %1 %2C").arg('A').arg('B').arg('C').toString());

        // Two argument tests with array
        assertEquals(
            "Test test 100",
            new FormatterAvkon("Test %1U %0N").format(
                new Object[]
                {
                    new Integer(100), "test"
                }));

        assertEquals(
            "100 test test",
            new FormatterAvkon("%0 test %1").format(
                new Object[]
                {
                    new Integer(100), "test"
                }));

        // Missing argument, no failure, just unreplaced parameter
        assertEquals(
            "100 test %1",
            new FormatterAvkon("%0 test %1").arg(100).toString());

        // Extra argument, no failure
        assertEquals(
            "100 test",
            new FormatterAvkon("%0 test").arg(100).arg("test").toString());

        Date testDate = new Date(1234567890); // 15/01/1970
        // Test case assumes emulator locale is default as EDateEuropean.
        assertEquals("15/01/1970", new FormatterAvkon("%0").arg(testDate).toString());

        Object obj = testDate;
        assertEquals("15/01/1970", new FormatterAvkon("%0").arg(obj).toString());

        obj = new Integer(1);
        assertEquals("1", new FormatterAvkon("%0").arg(obj).toString());

        obj = new String("me");
        assertEquals("Test me", new FormatterAvkon("Test %0").arg(obj).toString());

        // Test skip not supported type
        obj = new Boolean(false);
        assertEquals("Test %0", new FormatterAvkon("Test %0").arg(obj).toString());

        // Test MAX %1U[08]
        assertEquals(
            "Test testMyTe 100",
            new FormatterAvkon("Test %1U[08] %0N").arg(100).arg("testMyTest").toString());

        // Test %U[]
        assertEquals(
            "Test testMyTest",
            new FormatterAvkon("Test %U[]").arg("testMyTest").toString());

        // Test %U[08]
        assertEquals(
            "Test testMyTe",
            new FormatterAvkon("Test %U[08]").arg("testMyTest").toString());

        // Test %0[]
        assertEquals(
            "Test testMyTest",
            new FormatterAvkon("Test %0[]").arg("testMyTest").toString());

        // Test %0[08]
        assertEquals(
            "Test testMyTe",
            new FormatterAvkon("Test %0[08]").arg("testMyTest").toString());

        // Test %U[20  Skip
        assertEquals(
            "Test %U[20",
            new FormatterAvkon("Test %U[20").arg("testMyTest").toString());

        // Test %U[300]
        assertEquals(
            "Test testMyTest",
            new FormatterAvkon("Test %U[300]").arg("testMyTest").toString());

        // Test %U[8]
        assertEquals(
            "Test testMyTe",
            new FormatterAvkon("Test %U[8]").arg("testMyTest").toString());

        // Test Max is higher than actual string. Expected output: whole string no padding.
        assertEquals(
            "Test testMyTest",
            new FormatterAvkon("Test %U[80]").arg("testMyTest").toString());

        // Test Max is higher than pattern and string is shorter than pattern.
        assertEquals(
            "t to",
            new FormatterAvkon("t %U[09]").arg("to").toString());

        // Test 0 len Max
        assertEquals(
            "t ",
            new FormatterAvkon("t %U[00]").arg("to").toString());

        // Test 1 len Max
        assertEquals(
            "t 1",
            new FormatterAvkon("t %U[01]").arg("12").toString());

        // Test 0 len Max with 0 len replacement
        assertEquals(
            "t ",
            new FormatterAvkon("t %U[00]").arg("").toString());

        // Test 1 len Max with 1 len replacement
        assertEquals(
            "t 1",
            new FormatterAvkon("t %U[01]").arg("1").toString());
    }

    void testFormatterQt()
    {
        try
        {
            ResourceLoaderQt res =
                (ResourceLoaderQt)ResourceLoaderQt.getInstance(
                    "common", "txt_common_");

            // Zero argument
            assertEquals(
                "Test",
                new FormatterQt(res, "Test").toString());

            // One argument tests
            assertEquals(
                "Test 100",
                new FormatterQt(res, "Test %1").arg(100).toString());

            assertEquals(
                "Test 100",
                new FormatterQt(res, "Test %L1").arg(100).toString());

            assertEquals(
                "Test AAA",
                new FormatterQt(res, "Test %1").arg("AAA").toString());

            assertEquals(
                "Test A",
                new FormatterQt(res, "Test %1").arg('A').toString());

            // Two argument tests
            assertEquals(
                "Test test 100",
                new FormatterQt(res, "Test %2 %L1").arg(100).arg("test").toString());

            assertEquals(
                "100 test test",
                new FormatterQt(res, "%1 test %2").arg(100).arg("test").toString());

            assertEquals(
                "Test A B C",
                new FormatterQt(res, "Test %1 %2 %3").arg('A').arg('B').arg('C').toString());

            // Two argument tests with array
            assertEquals(
                "Test test 100",
                new FormatterQt(res, "Test %2 %L1").format(new Object[]
                {
                    new Integer(100), "test"
                }));

            assertEquals(
                "100 test test",
                new FormatterQt(res, "%1 test %2").format(new Object[]
                {
                    new Integer(100), "test"
                }));

            // Missing argument, no failure, just unreplaced parameter
            assertEquals(
                "100 test %2",
                new FormatterQt(res, "%1 test %2").arg(100).toString());

            // Extra argument, no failure
            assertEquals(
                "100 test",
                new FormatterQt(res, "%1 test").arg(100).arg("test").toString());

            Date testDate = new Date(1234567890); // 15/01/1970
            assertEquals("date1 15/01/1970",
                         new FormatterQt(res, "date1 %1").arg(testDate).toString());

            Object obj = testDate;
            assertEquals("date2 15/01/1970",
                         new FormatterQt(res, "date2 %1").arg(obj).toString());

            assertEquals("date3 15/01/1970",
                         new FormatterQt(res, "date3 %1").format(
                             new Object[] { obj }));

            obj = new Integer(1);
            assertEquals("1", new FormatterQt(res, "%1").arg(obj).toString());

            obj = new String("me");
            assertEquals(
                "Test me",
                new FormatterQt(res, "Test %1").arg(obj).toString());

            // Test skip not supported type
            obj = new Boolean(false);
            assertEquals(
                "Test %1",
                new FormatterQt(res, "Test %1").arg(obj).toString());

            // Test MAX %[8]2
            //assertEquals(
            //    "Test testMyTe 100",
            //    new FormatterQt(res, "Test %[8]2 %1").arg(100).arg("testMyTest").toString());

            // Test %[] Skip
            assertEquals(
                "Test %[]",
                new FormatterQt(res, "Test %[]").arg("testMyTest").toString());

            // Test %[]1 Skip
            assertEquals(
                "Test %[]1",
                new FormatterQt(res, "Test %[]1").arg("testMyTest").toString());

            // Test %[20  Skip
            assertEquals(
                "Test %[20",
                new FormatterQt(res, "Test %[20").arg("testMyTest").toString());

            // Test %[08]1
            assertEquals(
                "Test testMyTe\u2026",
                new FormatterQt(res, "Test %[08]1").arg("testMyTest").toString());

            // Test %[300]1
            assertEquals(
                "Test testMyTest",
                new FormatterQt(res, "Test %[300]1").arg("testMyTest").toString());

            // Test Max is higher than pattern and result is shorter than pattern.
            assertEquals(
                "t to",
                new FormatterQt(res, "t %[09]1").arg("to").toString());

            // Test 0 len Max
            assertEquals(
                "t \u2026",
                new FormatterQt(res, "t %[00]1").arg("to").toString());

            // Test 1 len Max
            assertEquals(
                "t 1\u2026",
                new FormatterQt(res, "t %[01]1").arg("12").toString());

            // Test 0 len Max with 0 len replacement
            assertEquals(
                "t ",
                new FormatterQt(res, "t %[00]1").arg("").toString());

            // Test 1 len Max with 1 len replacement
            assertEquals(
                "t 1",
                new FormatterQt(res, "t %[01]1").arg("1").toString());

            // Plurality form tests
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    void testResourceLoaderAvkon()
    {
        doResourceLoaderAvkonTest("testresource", "test_", "", "");
        doResourceLoaderAvkonTest("testresource,testresource2", "test_,test2_", "", "");
        doResourceLoaderAvkonTest("testresource2,testresource", "unknown_prefix_,test_", "", "");
        doResourceLoaderAvkonTest("testresource2,testresource", "unknown_prefix_,test_", "dummy,", "dummy,");
        doResourceLoaderAvkonTest("testresource", "test_,unknown_prefix_", "", "");
    }

    private void doResourceLoaderAvkonTest(
        String resourceFilename, String resourcePrefix,
        String inPrefix, String outPrefix)
    {
        try
        {
            ResourceLoader res =
                ResourceLoaderAvkon.getInstance(resourceFilename, resourcePrefix);

            // Test existing
            assertEquals(
                "id1 test",
                res.format(inPrefix + "id_1").arg("test").toString());
            assertEquals(
                "id2 test2",
                res.format(inPrefix + "id_2").arg("test2").toString());
            assertEquals(
                "id3 test3",
                res.format(inPrefix + "id_3").arg("test3").toString());

            // Test fetching the same text again
            assertEquals(
                "id1 test",
                res.format(inPrefix + "id_1").arg("test").toString());

            // Test full name
            assertEquals(
                "id1 test",
                res.format(inPrefix + "id_1").arg("test").toString());

            // Test non-existing, with prefix
            assertTrue(
                inPrefix + "id_notfound does not result " + outPrefix + "id_notfound",
                res.format(inPrefix + "id_notfound").arg("test3").toString().endsWith(outPrefix + "id_notfound"));

            // Test non-existing, without prefix
            assertTrue(
                inPrefix + "id_notfound does not result " + outPrefix + "id_notfound",
                res.format(inPrefix + "id_notfound").arg("test3").toString().endsWith(outPrefix + "id_notfound"));

            // Test array
            assertEquals(
                "id3 test3",
                res.format(inPrefix + "id_3", new Object[] {"test3"}));

            // Test enter decoding.
            assertEquals(
                "id4 \n test4 \n",
                res.format(inPrefix + "id_4").arg("test4").toString());

            // Test backlash decoding.
            assertEquals(
                "\\id5 \\ test5",
                res.format(inPrefix + "id_5").arg("test5").toString());

            // Test double-quote decoding.
            assertEquals(
                "id6 \"\" test6",
                res.format(inPrefix + "id_6").arg("test6").toString());

            // Test single-quote decoding.
            assertEquals(
                "id7 \' test7",
                res.format(inPrefix + "id_7").arg("test7").toString());

            // Test tabulator decoding.
            assertEquals(
                "id8\ttest8",
                res.format(inPrefix + "id_8").arg("test8").toString());

            // Test character code <0x20AC>
            assertEquals(
                "id9 It takes 800 \u20ac to win!",
                res.format(inPrefix + "id_9").arg(800).toString());

            // Test two character codes.
            assertEquals(
                "id10 It takes \u20ac 800 \u20ac to win!",
                res.format(inPrefix + "id_10").arg(800).toString());

            // Test two character codes in a row. Test <0x20ac><0x20ac>
            assertEquals(
                "id11 It takes \u20ac\u20ac to win!",
                res.format(inPrefix + "id_11").toString());

            // Test only character code.
            assertEquals(
                "\u20ac",
                res.format(inPrefix + "id_12").toString());

            // Test invalid character code.
            assertEquals(
                "id13 It <0x20A>to win!",
                res.format(inPrefix + "id_13").toString());


            // Test character code as first character.
            assertEquals(
                "\u20ac id14",
                res.format(inPrefix + "id_14").toString());

            // Test character code as last character.
            assertEquals(
                "id15 \u20ac",
                res.format(inPrefix + "id_15").toString());

            // Test invalid character.
            assertEquals(
                "id16 <0xG0AC>",
                res.format(inPrefix + "id_16").toString());
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue("'" + resourceFilename + "', '" + resourcePrefix + "', '" +
                       inPrefix + "', '" + outPrefix + "': " + t, false);
        }
    }

    void testResourceLoaderQt()
    {
        try
        {
            ResourceLoader res =
                ResourceLoaderQt.getInstance("common", "txt_common_");
            assertEquals("OK", res.format("button_ok").toString());
            // Test fetching the same text again
            assertEquals("OK", res.format("button_ok").toString());

            res = ResourceLoaderQt.getInstance("common_errors", "txt_");
            assertEquals("Invalid installation package", res.format("error_info_installation_package_is_invalid").toString());
            assertEquals("Unable to access server", res.format("error_info_no_server_access").toString());

            res = ResourceLoaderQt.getInstance("common,javaapplicationinstaller", "txt_common_,txt_java_inst_");
            assertEquals("Manufacturer", res.format("setlabel_cert_domain_val_manufacturer").toString());
            // Test fetching the same text again
            assertEquals("Manufacturer", res.format("setlabel_cert_domain_val_manufacturer").toString());

            assertEquals(
                "foo (bar)",
                res.format("info_app_suite_name_version").arg("foo").arg("bar").toString());
            // Test fetching the same text again
            assertEquals(
                "foo (bar)",
                res.format("info_app_suite_name_version").arg("foo").arg("bar").toString());


            assertEquals(
                "text_not_found",
                res.format("text_not_found").arg("foo").toString());
            // Test fetching the same text again
            assertEquals(
                "text_not_found",
                res.format("text_not_found").arg("foo").toString());
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    void testNumerusQt()
    {
        try
        {
            ResourceLoader res =
                ResourceLoaderQt.getInstance("clock", "txt_clock_");
            assertEquals("1 hr", res.format("dblist_daily_val_ln_hr").argn(1).toString());
            assertEquals("5 hrs", res.format("dblist_daily_val_ln_hr").argn(5).toString());
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    void testResourceLoaderCreate()
    {
        try
        {
            // Create Avkon ResourceLoader with unknown resource.
            ResourceLoader rl = ResourceLoader.getInstance(
                "unknown_resource_1", "unknown_prefix", null, null);
            ResourceLoader rl2 = ResourceLoader.getInstance(
                "unknown_resource_1", "unknown_prefix", null, null);
            assertTrue("1: null ResourceLoader", rl != null);
            assertTrue("1: not Avkon ResourceLoader", rl instanceof ResourceLoaderAvkon);
            assertTrue("1: not same ResourceLoader instance", rl == rl2);

            // Create Qt ResourceLoader with known resource.
            rl = ResourceLoader.getInstance(
                null, null, "common", "txt_");
            rl2 = ResourceLoader.getInstance(
                null, null, "common", "txt_");
            assertTrue("2: null ResourceLoader", rl != null);
            if (ResourceLoader.getLocaleIdQt() == null)
            {
                assertTrue("2: not Avkon ResourceLoader", rl instanceof ResourceLoaderAvkon);
            }
            else
            {
                assertTrue("2: not Qt ResourceLoader", rl instanceof ResourceLoaderQt);
            }
            assertTrue("2: not same ResourceLoader instance", rl == rl2);

            // When Qt resource is not found, Avkon ResourceLoader should
            // be returned.
            rl = ResourceLoader.getInstance(
                null, null, "unknown_resource_2", "unknown_prefix");
            rl2 = ResourceLoader.getInstance(
                null, null, "unknown_resource_2", "unknown_prefix");
            assertTrue("3: null ResourceLoader", rl != null);
            assertTrue("3: not Avkon ResourceLoader", rl instanceof ResourceLoaderAvkon);
            assertTrue("3: not same ResourceLoader instance", rl == rl2);

            if (ResourceLoader.getLocaleIdQt() != null)
            {
                try
                {
                    // Check that correct exception is thrown for
                    // unknown Qt resource.
                    rl = ResourceLoaderQt.getInstance(
                        "unknown_resource_3", "unknown_prefix");
                    assertTrue("no exception for unknown Qt resource", false);
                }
                catch (IllegalArgumentException iae)
                {
                    // Expected exception
                }
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    void testResourceLoaderDestroy()
    {
        try
        {
            ResourceLoader.destroyAll();
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }
}
