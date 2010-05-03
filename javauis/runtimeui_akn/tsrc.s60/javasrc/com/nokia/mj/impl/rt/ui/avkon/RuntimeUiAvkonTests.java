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


package com.nokia.mj.impl.rt.ui.avkon;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.rt.ui.avkon.RuntimeUiAvkon;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * RuntimeUiAvkon unit tests.
 */
public class RuntimeUiAvkonTests extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new RuntimeUiAvkonTests("testAvkon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RuntimeUiAvkonTests)tc).testAvkon();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public RuntimeUiAvkonTests()
    {
    }

    public RuntimeUiAvkonTests(String aTestName, TestMethod aTestMethod)
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

    private void testAvkon()
    {
        doTest(new RuntimeUiAvkon());
    }

    private void doTest(RuntimeUi runtimeUi)
    {
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
        runtimeUi.error("MyApplication", exc);
        // confirm with first option highlighted and chosen
        ConfirmData confirmData = new ConfirmData("Please make sure the first answer option is highlighted and select that.", new String[] {"answerOpt1", "answerOpt2", "answerOption3"}, 0);
        boolean answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);
        // confirm with second option chosen
        confirmData = new ConfirmData("Please make sure the second answer option is highlighted and select that.", new String[] {"answerOpt1", "answerOpt2", "answerOption3"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 1);
        /* Uncomment when/if the Cancel option is available
        // confirm with no answer available
        confirmData = new ConfirmData("Please cancel this dialog.", new String[]{"answerOpt1", "answerOpt2", "answerOption3"}, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(!answerAvailable && confirmData.getAnswer() == ConfirmData.NO_ANSWER);
        // confirm without options (null options)
        confirmData = new ConfirmData("Please make sure there are no answer options available and cancel this dialog.", null, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(!answerAvailable && confirmData.getAnswer() == ConfirmData.NO_ANSWER);*/
        // confirm with null option -> an empty string is displayed instead
        confirmData = new ConfirmData("Please make sure the third answer option is empty and select that.", new String[] {"answerOption1", "answerOption2", null}, 2);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 2);
        // confirm with first option chosen
        confirmData = new ConfirmData("Please make sure there are two buttons available and select the left hand one.", new String[] {"answerOpt1", "answerOpt2"}, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);
        // confirm with second option chosen
        confirmData = new ConfirmData("Please make sure there are two buttons available and select the right hand one.", new String[] {"answerOpt1", "answerOpt2"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 1);
        /* Uncomment when/if the Cancel option is available
        // confirm with no answer available
        confirmData = new ConfirmData("Please cancel this dialog.", new String[]{"answerOpt1", "answerOpt2"}, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(!answerAvailable && confirmData.getAnswer() == ConfirmData.NO_ANSWER);
        // confirm without options (null options)
        confirmData = new ConfirmData("Please make sure there are no answer options available and cancel this dialog.", null, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(!answerAvailable && confirmData.getAnswer() == ConfirmData.NO_ANSWER);*/
        // confirm with one option
        confirmData = new ConfirmData("Please make sure there is only one button available and select that.", new String[] {"answerOption1"}, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);
        /* Uncomment when the caption of the buttone can be set freely
        // confirm with null option -> an empty string is displayed instead
        confirmData = new ConfirmData("Please make sure the caption of the right hand button is empty and select that.", new String[]{"answerOption1", null}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 1);*/
        runtimeUi.destroy();
    }
}
