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
package t_uirobot.textfield;

import j2meunit.framework.*;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.TextField;

import t_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test TextField's commands. <br>
 * <br>
 * Created: 2008-09-15
 */
public class TextFieldCommandTest extends ItemUITestBase
{

    private static int maxsize = 2000;

    /**
     * Constructor.
     */
    public TextFieldCommandTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public TextFieldCommandTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TextFieldCommandTest("testCommands",
                                                new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextFieldCommandTest) tc).testCommands();
            }
        }));
        aSuite.addTest(new TextFieldCommandTest("testTextFieldChange",
                                                new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextFieldCommandTest) tc).testTextFieldChange();
            }
        }));
        aSuite.addTest(new TextFieldCommandTest("testItemState",
                                                new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextFieldCommandTest) tc).testItemState();
            }
        }));
        return aSuite;
    }

    /**
     * Tests the basic functionality of command added to TextField.
     */
    public void testCommands()
    {
        TextField tf = new TextField("name", "text", maxsize, TextField.ANY);

        Command ok = new Command("Ok", "", Command.ITEM, 0);
        tf.addCommand(ok);
        tf.setItemCommandListener(this);

        form.append(tf);

        setCurrent(form);
        block(500); //it seems that we need more time to activate
        //all listenerers
        // Click command and verify it works:
        key(Key.CBA1);
        assertItemCmdListener("", ok, tf);
    }

    /**
     * Tests TextField's value changing.
     */
    public void testTextFieldChange()
    {
        TextField tf1 = new TextField("TextField", "text1", maxsize,
                                      TextField.ANY);
        TextField tf2 = new TextField("TextField", "text2", maxsize,
                                      TextField.ANY);
        boolean case1 = true;
        boolean case2 = true;
        boolean case3 = true;
        boolean case4 = true;
        boolean case5 = true;
        String reason = "";
        form.append(tf1);
        form.append(tf2);
        setCurrent(form);

        // Change TextField CaretPosition
        key(Key.RightArrow);
        if(tf1.getCaretPosition() != 0)
        {
            case1 = false;
            reason = "Wrong CaretPosition(case1). " + "expected " + 0
                     + "but got" + tf1.getCaretPosition();
            reason += "<<<<<>>>>>>";
        }
        //assertEquals("Wrong CaretPosition(case1).", 0, tf1.getCaretPosition());

        // Change TextField CaretPosition back to original:
        key(Key.LeftArrow);
        if(tf1.size() != tf1.getCaretPosition())
        {
            case2 = false;
            reason +=  "Wrong CaretPosition(case 2)." + "expected " + tf1.size()
                       + "but got" + tf1.getCaretPosition();
            reason += "<<<<<>>>>>>";
        }
        /*assertEquals("Wrong CaretPosition(case 2).", tf1.size(),
                tf1.getCaretPosition());
        */
        // Change focus to nextTextField
        key(Key.DownArrow);
        if(tf2.getCaretPosition() != 0)
        {
            case3 = false;
            reason +=  "Wrong CaretPosition(case 3)." + "expected " + 0
                       + "but got" + tf2.getCaretPosition();
            reason += "<<<<<>>>>>>";
        }
        /*assertEquals("Wrong CaretPosition(case 3).", 0,
                tf2.getCaretPosition());*/

        // Now Caret have to be in the end
        key(Key.LeftArrow);
        if(tf2.size() != tf2.getCaretPosition())
        {
            case4 = false;
            reason +=  "Wrong CaretPosition(case 4)." + "expected " + tf2.size()
                       + "but got" + tf2.getCaretPosition();
            reason += "<<<<<>>>>>>";
        }
        /*
        assertEquals("Wrong CaretPosition(case 4).", tf2.size(),
                tf2.getCaretPosition()); */

        //go to first TextField
        key(Key.RightArrow);
        key(Key.UpArrow);
        if(tf1.size() != tf1.getCaretPosition())
        {
            case5 = false;
            reason +=  "Wrong CaretPosition(case 5)." + "expected " + tf1.size()
                       + "but got" + tf1.getCaretPosition();
        }
        assertTrue(reason, case1 & case2 & case3 & case4 & case5);
    }

    /**
     * Tests that Form receives ItemStateChanged-events when TextField's
     * value is changed.
     */
    public void testItemState()
    {
        TextField tf = new TextField("header", "text", maxsize,
                                     TextField.ANY);

        form.append(tf);

        setCurrent(form);

        // change caret position of TextField and check that listener called.
        key(Key.LeftArrow);
        assertItemStateChanged("", tf);
    }

}
