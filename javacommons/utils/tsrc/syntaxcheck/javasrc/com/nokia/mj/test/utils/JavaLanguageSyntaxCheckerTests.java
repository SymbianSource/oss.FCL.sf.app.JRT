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

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.JavaLanguageSyntaxChecker;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SecurityDescriptorAttributes unit tests.
 */
public class JavaLanguageSyntaxCheckerTests extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new JavaLanguageSyntaxCheckerTests("testClassNameValidation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JavaLanguageSyntaxCheckerTests)tc).testClassNameValidation();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public JavaLanguageSyntaxCheckerTests()
    {
    }

    public JavaLanguageSyntaxCheckerTests(String aTestName, TestMethod aTestMethod)
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

    public void testClassNameValidation()
    {
        String[] JAVA_KEYWORDS =
        {
            "abstract","continue","for","new","switch",
            "assert","default","if","package","synchronized",
            "boolean","do","goto","private","this","break",
            "double","implements","protected","throw","byte",
            "else","import","public","throws","case","enum",
            "instanceof","return","transient","catch","extends",
            "int","short","try","char","final","interface","static",
            "void","class","finally","long","strictfp","volatile",
            "const","float","native","super","while",
            /* the following are not keywords but literals. still added
               here for simplicity */
            "true", "false", "null"
        };
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName(""));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("."));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("..package"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("*className"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("class*Name"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("0className"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("9className"));
        assertTrue(JavaLanguageSyntaxChecker.checkClassName("class_N$ame"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("int.class_N$ame"));
        for (int i=0; i<JAVA_KEYWORDS.length; i++)
        {
            switch (i%3)
            {
            case 0:
                assertTrue(!JavaLanguageSyntaxChecker.checkClassName(JAVA_KEYWORDS[i] + ".mypackage.classame"));
                break;
            case 1:
                assertTrue(!JavaLanguageSyntaxChecker.checkClassName("mypackage." + JAVA_KEYWORDS[i] + ".classame"));
                break;
            case 2:
                assertTrue(!JavaLanguageSyntaxChecker.checkClassName("mypackage.classame." + JAVA_KEYWORDS[i]));
                break;
            }
        }
        assertTrue(JavaLanguageSyntaxChecker.checkClassName("mypackage1.mypackage2.myclass"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("mypackage1..myclass"));
        assertTrue(!JavaLanguageSyntaxChecker.checkClassName("mypackage1.."));
    }

}
