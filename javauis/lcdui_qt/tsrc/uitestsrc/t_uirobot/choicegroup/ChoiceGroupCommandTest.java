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

import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;

import t_uirobot.ItemUITestBase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test ChoiceGroup commands. <br>
 * <br>
 * Created: 2008-11-04
 */
public class ChoiceGroupCommandTest extends ItemUITestBase {

	/**
     * Constructor.
     */
    public ChoiceGroupCommandTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ChoiceGroupCommandTest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }


    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new ChoiceGroupCommandTest("testCommands - EXCLUSIVE",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ChoiceGroup item = new ChoiceGroup("Exclusive",
                                Choice.EXCLUSIVE);
                        ((ChoiceGroupCommandTest) tc).testCommand(item);
                    }
                }));

        aSuite.addTest(new ChoiceGroupCommandTest("testCommands - MULTIPLE",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ChoiceGroup item = new ChoiceGroup("Multiple",
                                Choice.MULTIPLE);
                        ((ChoiceGroupCommandTest) tc).testCommand(item);
                    }
                }));

        return aSuite;
    }

    public void testChoiceGroupExclusiveCmds() {
        ChoiceGroup choiceGroup = new ChoiceGroup("Exclusive", Choice.EXCLUSIVE);
        choiceGroup.append("item1", null);
        choiceGroup.append("item2", null);
        choiceGroup.append("item3", null);
        choiceGroup.append("item4", null);
        testCommand(choiceGroup);
    }

    public void testChoiceGroupMultipleCmds() {
        ChoiceGroup choiceGroup = new ChoiceGroup("Multiple", Choice.MULTIPLE);
        choiceGroup.append("item1", null);
        choiceGroup.append("item2", null);
        choiceGroup.append("item3", null);
        choiceGroup.append("item4", null);
        testCommand(choiceGroup);
    }


}
