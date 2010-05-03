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


package com.nokia.mj.test.utils;

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

        aSuite.addTest(new LocalisationTest("formatterTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).formatterTest();
            }
        }));

        aSuite.addTest(new LocalisationTest("resourceLoaderTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LocalisationTest) tc).resourceLoaderTest();
            }
        }));

        return aSuite;
    }

    void formatterTest()
    {
        // Zero argument
        assertEquals(
            "Test",
            new Formatter("Test").toString());

        // One argument tests
        assertEquals(
            "Test 100",
            new Formatter("Test %0N").arg(100).toString());

        assertEquals(
            "Test 1",
            new Formatter("Test %C").arg('1').toString());

        assertEquals(
            "Test A",
            new Formatter("Test %0C").arg('A').toString());

        // Two argument tests
        assertEquals(
            "Test test 100",
            new Formatter("Test %1U %0N").arg(100).arg("test").toString());

        assertEquals(
            "100 test test",
            new Formatter("%0 test %1").arg(100).arg("test").toString());

        assertEquals(
            "Test A B C",
            new Formatter("Test %0C %1 %2C").arg('A').arg('B').arg('C').toString());

        // Two argument tests with array
        assertEquals(
            "Test test 100",
            new Formatter("Test %1U %0N").format(new Object[]
                                                 {
                                                     new Integer(100), "test"
                                                 }));

        assertEquals(
            "100 test test",
            new Formatter("%0 test %1").format(new Object[]
                                               {
                                                   new Integer(100), "test"
                                               }));

        // Missing argument, no failure, just unreplaced parameter
        assertEquals(
            "100 test %1",
            new Formatter("%0 test %1").arg(100).toString());

        // Extra argument, no failure
        assertEquals(
            "100 test",
            new Formatter("%0 test").arg(100).arg("test").toString());

        Date testDate = new Date(1234567890); // 15/01/1970
        // Test case assumes emulator locale is default as EDateEuropean.
        assertEquals("15/01/1970", new Formatter("%0").arg(testDate).toString());

        Object obj = testDate;
        assertEquals("15/01/1970", new Formatter("%0").arg(obj).toString());

        obj = new Integer(1);
        assertEquals("1", new Formatter("%0").arg(obj).toString());

        obj = new String("me");
        assertEquals("Test me", new Formatter("Test %0").arg(obj).toString());

        // Test skip not supported type
        obj = new Boolean(false);
        assertEquals("Test %0", new Formatter("Test %0").arg(obj).toString());

        // Test MAX %1U[08]
        assertEquals(
            "Test testMyTe 100",
            new Formatter("Test %1U[08] %0N").arg(100).arg("testMyTest").toString());

        // Test %U[]
        assertEquals(
            "Test testMyTest",
            new Formatter("Test %U[]").arg("testMyTest").toString());

        // Test %U[08]
        assertEquals(
            "Test testMyTe",
            new Formatter("Test %U[08]").arg("testMyTest").toString());

        // Test %0[]
        assertEquals(
            "Test testMyTest",
            new Formatter("Test %0[]").arg("testMyTest").toString());

        // Test %0[08]
        assertEquals(
            "Test testMyTe",
            new Formatter("Test %0[08]").arg("testMyTest").toString());

        // Test %U[20  Skip
        assertEquals(
            "Test %U[20",
            new Formatter("Test %U[20").arg("testMyTest").toString());

        // Test %U[300] Skip
        assertEquals(
            "Test %U[300]",
            new Formatter("Test %U[300]").arg("testMyTest").toString());

        // Test %U[8] Skip
        assertEquals(
            "Test %U[8]",
            new Formatter("Test %U[8]").arg("testMyTest").toString());

        // Test Max is higher than actual string. Expected output: whole string no padding.
        assertEquals(
            "Test testMyTest",
            new Formatter("Test %U[80]").arg("testMyTest").toString());

        // Test Max is higher than pattern and string is shorter than pattern.
        assertEquals(
            "t to",
            new Formatter("t %U[09]").arg("to").toString());

        // Test 0 len Max
        assertEquals(
            "t ",
            new Formatter("t %U[00]").arg("to").toString());

        // Test 1 len Max
        assertEquals(
            "t 1",
            new Formatter("t %U[01]").arg("12").toString());

        // Test 0 len Max with 0 len replacement
        assertEquals(
            "t ",
            new Formatter("t %U[00]").arg("").toString());

        // Test 1 len Max with 1 len replacement
        assertEquals(
            "t 1",
            new Formatter("t %U[01]").arg("1").toString());
    }

    void resourceLoaderTest()
    {

        ResourceLoader res = new ResourceLoader("testresource", "test_");

        // Test existing
        assertEquals(
            "id1 test",
            res.format("id_1").arg("test").toString());
        assertEquals(
            "id2 test2",
            res.format("id_2").arg("test2").toString());
        assertEquals(
            "id3 test3",
            res.format("id_3").arg("test3").toString());

// Test full name
        assertEquals(
            "id1 test",
            res.format("test_id_1").arg("test").toString());
// Test non-existing, with prefix
        assertEquals(
            "test_id_notfound",
            res.format("test_id_notfound").arg("test3").toString());

        // Test non-existing, without prefix
        assertEquals(
            "test_id_notfound",
            res.format("id_notfound").arg("test3").toString());

        // Test array
        assertEquals(
            "id3 test3",
            res.format("id_3", new Object[] {"test3"}));

        // Test enter decoding.
        assertEquals(
            "id4 \n test4 \n",
            res.format("id_4").arg("test4").toString());

        // Test backlash decoding.
        assertEquals(
            "\\id5 \\ test5",
            res.format("id_5").arg("test5").toString());

        // Test double-quote decoding.
        assertEquals(
            "id6 \"\" test6",
            res.format("id_6").arg("test6").toString());

        // Test single-quote decoding.
        assertEquals(
            "id7 \' test7",
            res.format("id_7").arg("test7").toString());

        // Test tabulator decoding.
        assertEquals(
            "id8\ttest8",
            res.format("id_8").arg("test8").toString());

        // Test character code <0x20AC>
        assertEquals(
            "id9 It takes 800 \u20ac to win!",
            res.format("id_9").arg(800).toString());

        // Test two character codes.
        assertEquals(
            "id10 It takes \u20ac 800 \u20ac to win!",
            res.format("id_10").arg(800).toString());

        // Test two character codes in a row. Test <0x20ac><0x20ac>
        assertEquals(
            "id11 It takes \u20ac\u20ac to win!",
            res.format("id_11").toString());

        // Test only character code.
        assertEquals(
            "\u20ac",
            res.format("id_12").toString());

        // Test invalid character code.
        assertEquals(
            "id13 It <0x20A>to win!",
            res.format("id_13").toString());


        // Test character code as first character.
        assertEquals(
            "\u20ac id14",
            res.format("id_14").toString());

        // Test character code as last character.
        assertEquals(
            "id15 \u20ac",
            res.format("id_15").toString());

        // Test invalid character.
        assertEquals(
            "id16 <0xG0AC>",
            res.format("id_16").toString());
    }
}
