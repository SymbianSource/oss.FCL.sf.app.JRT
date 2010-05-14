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
package t_uirobot.stringitem;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import t_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test hyperlink. <br>
 * <br>
 * Created: 2008-10-20
 */
public class HyperLinkTest extends ItemUITestBase
{

    /**
     * Constructor.
     */
    public HyperLinkTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public HyperLinkTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new HyperLinkTest("testCommands",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).testCommands();
            }
        }));

        aSuite.addTest(new HyperLinkTest("testAppearanceModeChangePlain",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).
                testAppearanceModeChange(StringItem.PLAIN);
            }
        }));

        aSuite.addTest(new HyperLinkTest("testAppearanceModeChangeButton",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).
                testAppearanceModeChange(StringItem.BUTTON);
            }
        }));

        aSuite.addTest(new HyperLinkTest("testAppearanceModeChangeHyperlink",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).
                testAppearanceModeChange(StringItem.HYPERLINK);
            }
        }));

        aSuite.addTest(new HyperLinkTest("testFocusInAppearanceModeChangePlain",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).
                testFocusInAppearanceModeChange(StringItem.PLAIN);
            }
        }));

        aSuite.addTest(new HyperLinkTest(
                           "testFocusInAppearanceModeChangeButton",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).
                testFocusInAppearanceModeChange(StringItem.BUTTON);
            }
        }));

        aSuite.addTest(new HyperLinkTest(
                           "testFocusInAppearanceModeChangeHyperlink",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).
                testFocusInAppearanceModeChange(StringItem.HYPERLINK);
            }
        }));

        aSuite.addTest(new HyperLinkTest("testLongHyperlinkWithLSK",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).testLongHyperlink(Key.CBA1);
            }
        }));

        aSuite.addTest(new HyperLinkTest("testLongHyperlinkWithMSK",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((HyperLinkTest) tc).testLongHyperlink(Key.Select);
            }
        }));

        return aSuite;
    }

    /**
     * Tests the basic functionality of command added to Hyperlink.
     */
    public void testCommands()
    {
        StringItem hyperlink = new StringItem("label", "url",
                                              StringItem.HYPERLINK);

        testCommand(hyperlink);

        // Because focus transfer in case of hyperlink is different than
        // in normal cases it is good to test that commands works when
        // moving focus from previous item to hyperlink as well as when
        // moving focus from next item:
        form.deleteAll();
        StringItem button1 = new StringItem(null, "button1", StringItem.BUTTON);
        StringItem button2 = new StringItem(null, "button2", StringItem.BUTTON);
        Command dummyCommand = new Command("dummy", "", Command.ITEM, 0);
        button1.addCommand(dummyCommand);
        button2.addCommand(dummyCommand);
        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);

        // Some typical url-characters are replaced with 'x'-chars to avoid
        // some problems in eSWT/AVKON implementation. Those problems should
        // go away when using QT-based eSWT.

        Command ok = new Command("Ok", "", Command.ITEM, 0);

        StringItem hyperlink3 = new StringItem(null,
                                               "xxxx://this.link.should.be.wrapped.to.many"
                                               + ".lines.info/index.php?wrapped=truexi=hope",
                                               StringItem.HYPERLINK);
        hyperlink3.addCommand(ok);
        hyperlink3.setItemCommandListener(this);

        form.append(button1);
        form.append(hyperlink3);
        form.append(button2);

        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.RightArrow);
        key(Key.CBA1);
        assertItemCmdListener("hyperlink focus transfer test, case 1", ok,
                              hyperlink3);

        key(Key.RightArrow);
        key(Key.LeftArrow);
        key(Key.CBA1);
        assertItemCmdListener("hyperlink focus transfer test, case 2", ok,
                              hyperlink3);
    }

    /**
     * Tests that commands are working as expected when commands
     * are added or removed dynamically.
     *
     * @param mode Appearancemode of StringItem.
     */
    public void testAppearanceModeChange(int mode)
    {
        switch(mode)
        {
        case StringItem.PLAIN:
            print("Running test with appearance mode PLAIN.");
            break;
        case StringItem.BUTTON:
            print("Running test with appearance mode BUTTON.");
            break;
        case StringItem.HYPERLINK:
            print("Running test with appearance mode HYPERLINK.");
            break;
        default:
            fail("Invalid test (incorrect appearance mode).");
            break;
        }

        StringItem stringItem = new StringItem("label", "url", mode);
        Command ok = new Command("Ok", "", Command.ITEM, 0);

        form.append(stringItem);
        stringItem.addCommand(ok);
        stringItem.setItemCommandListener(this);

        setCurrent(form);

        // Remove command and make sure the command is not working anymore:
        stringItem.removeCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("cmd activated after removing", null, null);

        // Add command again and verify it works:
        stringItem.addCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("", ok, stringItem);
    }

    /**
     * Tests that focus transfer works when commands are added/removed
     * dynamically.
     *
     * @param mode Appearancemode of StringItem.
     */
    public void testFocusInAppearanceModeChange(int mode)
    {
        switch(mode)
        {
        case StringItem.PLAIN:
            print("Running test with appearance mode PLAIN.");
            break;
        case StringItem.BUTTON:
            print("Running test with appearance mode BUTTON.");
            break;
        case StringItem.HYPERLINK:
            print("Running test with appearance mode HYPERLINK.");
            break;
        default:
            fail("Invalid test (incorrect appearance mode).");
            break;
        }

        StringItem stringItem = new StringItem(null, "url", mode);

        StringItem button1 = new StringItem(null, "button1", Item.BUTTON);
        StringItem button2 = new StringItem(null, "button2", Item.BUTTON);

        Command ok = new Command("giamcc1", "", Command.ITEM, 0);
        Command buttonCommand1 = new Command("giamcc12", "", Command.ITEM, 0);
        Command buttonCommand2 = new Command("giamcc13", "", Command.ITEM, 0);

        form.append(button1);
        form.append(stringItem);
        form.append(button2);

        // Set listener but do not add commands yet:
        stringItem.setItemCommandListener(this);
        button1.addCommand(buttonCommand1);
        button2.addCommand(buttonCommand2);
        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);

        setCurrent(form);

        // At this point there are two buttons and plain stringItem
        // in middle of them. Focus is in first button.
        // Move to second button, add command to StringItem, move
        // to StringItem and verify command works:
        key(Key.RightArrow);

        stringItem.addCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.LeftArrow);

        assertItemCmdListener("case1", ok, stringItem);

        // Now remove command from StringItem. Test assumes that in this
        // case focus is moved to next item which is the second button.
        // Verify that happens and also make sure it is possible to move
        // to first button also:
        stringItem.removeCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("case2", buttonCommand2, button2);

        key(Key.LeftArrow);

        key(Key.CBA1);
        assertItemCmdListener("case3", buttonCommand1, button1);

        // Now add command back to StringItem again and make sure it
        // works:
        stringItem.addCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.RightArrow);

        key(Key.CBA1);
        assertItemCmdListener("case4", ok, stringItem);

        // And verify also the functionality of second button after
        // these changes:
        key(Key.RightArrow);

        key(Key.CBA1);
        assertItemCmdListener("case5", buttonCommand2, button2);
    }

    /**
     * Tests that Hyperlink works as expected when its size exceeds
     * screen height.
     *
     * @param key Keycode which is used to verify the hyperlink command
     *      works.
     */
    public void testLongHyperlink(int key)
    {
        StringItem stringItem = new StringItem(
            "label",
            "The quick brown fox jumps over the lazy dog and realizes"
            + " that this hyperlink text is repeated several times to make"
            + " sure the size of the hyperlink exceeds screen height. "
            + "The quick brown fox jumps over the lazy dog and realizes"
            + " that this hyperlink text is repeated several times to make"
            + " sure the size of the hyperlink exceeds screen height. "
            + "The quick brown fox jumps over the lazy dog and realizes"
            + " that this hyperlink text is repeated several times to make"
            + " sure the size of the hyperlink exceeds screen height. ",
            StringItem.HYPERLINK);

        Command ok = new Command("lhlc1", "", Command.ITEM, 0);

        StringItem button1 = new StringItem(null, "button1", Item.BUTTON);
        Command buttonCommand1 = new Command("lhlc2", "", Command.ITEM, 0);

        form.append(stringItem);
        stringItem.addCommand(ok);
        stringItem.setItemCommandListener(this);

        form.append(button1);
        button1.addCommand(buttonCommand1);
        button1.setItemCommandListener(this);

        setCurrent(form);

        //TODO: When running all tests and the key is LSK and in following loop
        //the i is 0, latestCommand is buttonCommand1 and latestItem is null.
        //If button is not added to the form then the latestCommand also
        //will be null.
        //Problem has something to do with timing, because
        //adding debugprints to this test will sometimes make it pass.
        //For example following delay will make the test pass:
        block(CHANGE_DISPLAYABLE_DELAY);

        // Scroll down and after each keydown-press make sure the hyperlink
        // is still focused:
        final int rounds = 10;
        final int roundsInHyperlink = 3;
        for(int i = 0; i < rounds; i++)
        {
            key(key);

            if(i < roundsInHyperlink)
            {
                // Code assumes that it takes at least three
                // keypresses to reach button which is located below hyperlink:
                assertItemCmdListener("clicking hyperlink. i=" + i, ok,
                                      stringItem);
            }
            else if(i >= (rounds - 1))
            {
                // After 10 keypresses button should be focused:
                assertItemCmdListener("clicking button. i=" + i,
                                      buttonCommand1, button1);
            }

            key(Key.RightArrow);
            block(CHANGE_DISPLAYABLE_DELAY);
        }
    }

}
