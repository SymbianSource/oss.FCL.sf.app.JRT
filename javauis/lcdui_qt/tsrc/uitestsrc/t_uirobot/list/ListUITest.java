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
package t_uirobot.list;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import spede.SpedeRunner;
import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based tests to test List's functionality. <br>
 * <br>
 * @created 7.8.2008
 */
public class ListUITest extends UITestBase {

    private static final int SETCURRENT_TIMEOUT = 6000;
    private static final int WAIT_CYCLE_TIME = 50;

    private boolean syncFlag;

    private Command lastCommand;
    private Displayable lastDisplayable;

    /**
     * Constructor.
     */
    public ListUITest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public ListUITest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return New TestSuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new ListUITest("testDefaultSelectCmd", new TestMethod() {
            public void run(TestCase tc) {
                ((ListUITest) tc).testDefaultSelectCmd();
            }
        }));

        return aSuite;
    }

    /**
     * Test method.
     */
    public void testDefaultSelectCmd() {
        List list = new List("Implicit List", Choice.IMPLICIT);
        list.setCommandListener(this);
        assertSetCurrent(list);
        list.append("item 1", null);
        list.append("item 2", null);
        list.append("item 3", null);
        list.append("item 4", null);

        clearCallbackValues();
        key(Key.UpArrow);
        key(Key.UpArrow);
        key(Key.UpArrow);
        key(Key.Select);
        assertCommandListener(List.SELECT_COMMAND, list);
        // TODO: assumes that eSWT table selection moves with latest added item
        assertEquals("Wrong item selected", 0, list.getSelectedIndex());

        clearCallbackValues();
        Command command = new Command("Cmd1", null, Command.SCREEN, 0);
        list.setSelectCommand(command);
        key(Key.DownArrow);
        key(Key.Select);
        assertCommandListener(command, list);
        assertEquals("Wrong item selected", 1, list.getSelectedIndex());
    }

    /**
     * Catches commands and saves Command as well as Displayable to local member
     * variables.
     *
     * @param aCommand Command.
     * @param aDisplayable Displayable.
     */
    public void commandAction(Command aCommand, Displayable aDisplayable) {
        lastCommand = aCommand;
        lastDisplayable = aDisplayable;
        doNotify();
    }

    /**
     * Assert that changing displayable went fine.
     *
     * @param disp a displayable
     */
    protected void assertSetCurrent(Displayable disp) {
        display.setCurrent(disp);
        Displayable curr = null;
        int waitSum = 0;
        while (curr != disp && waitSum <= SETCURRENT_TIMEOUT) {
            block(CHANGE_DISPLAYABLE_DELAY);
            waitSum += CHANGE_DISPLAYABLE_DELAY;
            curr = display.getCurrent();
        }
        assertEquals(disp + " should be current", disp, curr);
    }

    /**
     * Assert the command listener values.
     *
     * @param expCmd expected Command
     * @param expDisp expected Displayable
     */
    protected void assertCommandListener(Command expCmd, Displayable expDisp) {
        assertTrue("Command listener wasn't called with " + expCmd.getLabel(),
                syncFlag);
        assertEquals("Last displayable mismatch", expDisp, lastDisplayable);
        assertEquals("Last command should be " + expCmd.getLabel(), expCmd,
                lastCommand);
    }

    /**
     * Wait until someone calls doNotify or the timeout expires.
     *
     * @param timeout timeout
     * @return true if the doNotify was called during this, else otherwise
     */
    protected boolean doWait(int timeout) {
        clearCallbackValues();
        int waitSum = 0;
        // wait here until syncFlag gets true or timeout expires
        while (!syncFlag && waitSum <= timeout) {
            block(WAIT_CYCLE_TIME);
            waitSum += WAIT_CYCLE_TIME;
        }
        return syncFlag;
    }

    /**
     * Notify the doWait method.
     */
    protected void doNotify() {
        syncFlag = true;
    }

    /**
     * Clear command listener values.
     */
    protected void clearCallbackValues() {
        syncFlag = false;
        lastCommand = null;
        lastDisplayable = null;
    }

}
