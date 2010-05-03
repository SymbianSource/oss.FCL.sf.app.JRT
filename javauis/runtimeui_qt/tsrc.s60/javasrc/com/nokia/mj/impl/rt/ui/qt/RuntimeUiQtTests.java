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


package com.nokia.mj.impl.rt.ui.qt;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * RuntimeUiQt unit tests.
 */
public class RuntimeUiQtTests extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new RuntimeUiQtTests("testQt", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RuntimeUiQtTests)tc).testQt();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public RuntimeUiQtTests()
    {
    }

    public RuntimeUiQtTests(String aTestName, TestMethod aTestMethod)
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

    private void testQt()
    {
        doTest(new RuntimeUiQt());
    }

    private void doTest(RuntimeUi runtimeUi)
    {
        System.out.println("+doTest()");
        // error message with details -> make sure both are available to the user
        ExceptionBase exc = new ExceptionBase(
            new ShortErrorMessage(),
            ShortErrorMessage.MSG_WITH_DETAILS_ID,
            null, /* no params for short msg */
            new DetailedErrorMessage(),
            DetailedErrorMessage.MSG_ID,
            null /* no params for detailed msg */);


        runtimeUi.error("MyApplication", exc);

        // error message with short description only
        exc = new ExceptionBase(
            new ShortErrorMessage(),
            ShortErrorMessage.MSG_WITHOUT_DETAILS_ID,
            null, /* no params for short msg */
            null,
            0,
            null /* no params for detailed msg */);
        System.out.println("3");

        runtimeUi.error("MyApplication", exc);

        // Test no crash happen.
        runtimeUi.error("NullException", null);


        boolean answerAvailable = false;

        // Test confirm
        ConfirmData confirmData = new ConfirmData("Confirm dialog. Select Allow", new String[] {"answerOpt1", "answerOpt2", "answerOption3"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 1);

        confirmData = new ConfirmData("Confirm dialog. Select Deny", new String[] {"answerOpt1", "answerOpt2", "answerOption3"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);

        confirmData = new ConfirmData("Null Answer options", null /*Not Supported*/, 1);
        answerAvailable = runtimeUi.confirm("Null answer options", confirmData);
        assertTrue(answerAvailable);

        ConfirmData nullConf = null;
        runtimeUi.confirm("Null Application", nullConf);

        runtimeUi.destroy();
        System.out.println("-doTest()");
    }
}
