/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package t_uirobot.choicegroup;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import t_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test POPUP styled ChoiceGroup. <br>
 * <br>
 * Created: 2008-11-04
 */
public class ChoiceGroupPopupTest extends ItemUITestBase
{

    /**
     * Constructor.
     */
    public ChoiceGroupPopupTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ChoiceGroupPopupTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();
        aSuite.addTest(new ChoiceGroupPopupTest("testOpenPopup",
                                                new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ChoiceGroupPopupTest) tc).testOpenPopup();
            }
        }));

        return aSuite;
    }

    /**
     * Test command on Items.
     */
    public void testOpenPopup()
    {
        Command cmd = new Command("Cmd", Command.ITEM, 0);
        ChoiceGroup choiceGroup = new ChoiceGroup("Popup", Choice.POPUP);
        choiceGroup.append("item1", null);
        choiceGroup.append("item2", null);
        choiceGroup.append("item3", null);
        choiceGroup.append("item4", null);
        choiceGroup.addCommand(cmd);

        form.append(choiceGroup);

        int sel = choiceGroup.getSelectedIndex();
        assertTrue("Invalid initial selected index", sel != -1);

        // Set form current
        setCurrent(form);
        block(CHANGE_DISPLAYABLE_DELAY);

        // Click MSK
        key(Key.Select);
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.Select);

        assertTrue("No item selected", choiceGroup.getSelectedIndex() != -1);
        assertTrue("Could not change selection",
                   choiceGroup.getSelectedIndex() != sel);

        assertItemStateChanged("case1", choiceGroup);
    }


}
