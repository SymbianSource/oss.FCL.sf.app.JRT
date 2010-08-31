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
package t_screenshots.form;

import j2meunit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import t_uirobot.UITestBase;


/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>This moduletest does screenshot based testing for Form. <br>
 *
 * @created 2008-07-23
 */
public class FormSTest extends UITestBase
{

    private Command exitCmd;

    private Image smallImage;
    private Image mediumImage;

    private Form form;

    /**
     * Constructor.
     */
    public FormSTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public FormSTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception
    {
        super.setUp();
        exitCmd = new Command("exit", "exit", Command.EXIT, 0);
        try
        {
            smallImage = Image.createImage("32x32.jpeg");
        }
        catch(IOException e)
        {
            e.printStackTrace();
        }

        try
        {
            mediumImage = Image.createImage("100x100.png");
        }
        catch(IOException e)
        {
            e.printStackTrace();
        }
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return New testsuite.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new FormSTest("plainStringItemTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((FormSTest) tc).plainStringItemTest();
            }
        }));
        aSuite.addTest(new FormSTest("plainImageItemTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((FormSTest) tc).plainImageItemTest();
            }
        }));
        aSuite.addTest(new FormSTest("stringImageButtonTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((FormSTest) tc).stringImageButtonTest();
            }
        }));

        return aSuite;
    }

    /**
     * Makes screenshot which shows different aspects of PLAIN StringItem.
     */
    void plainStringItemTest()
    {
        form = new Form("Form test");

        String img = "FormPlainStringItem.gif";
        String fewLines = "Loremipsumdolorsitamet"
                          + "consectetuer adipiscing elit.";
        StringItem st1 = new StringItem("header", "Text 1");
        StringItem st2 = new StringItem(null, "Text 2");
        st2.setLayout(Item.LAYOUT_TOP);
        StringItem st3 = new StringItem(null, "Text 3");
        st3.setLayout(Item.LAYOUT_VCENTER);
        StringItem st4 = new StringItem(null, "Text 4");
        st4.setLayout(Item.LAYOUT_CENTER);
        StringItem st5 = new StringItem("header 5", "Text 5");
        StringItem st6 = new StringItem(null, "Text 6");
        st6.setLayout(Item.LAYOUT_TOP);
        StringItem st7 = new StringItem(null, "Text 7");
        st7.setLayout(Item.LAYOUT_VCENTER);
        StringItem st8 = new StringItem(null, "Text 8");
        st8.setLayout(Item.LAYOUT_RIGHT);
        StringItem st9 = new StringItem("header 9", "Text 9");
        StringItem st10 = new StringItem(null, "Text 10");
        st10.setLayout(Item.LAYOUT_TOP);
        StringItem st11 = new StringItem(null, "Text 11");
        st11.setLayout(Item.LAYOUT_VCENTER);
        StringItem st12 = new StringItem(null, fewLines);
        st12.setLayout(Item.LAYOUT_LEFT);
        form.append(st1);
        form.append(st2);
        form.append(st3);
        form.append(st4);
        form.append(st5);
        form.append(st6);
        form.append(st7);
        form.append(st8);
        form.append(st9);
        form.append(st10);
        form.append(st11);
        form.append(st12);
        setCurrent(form);
        boolean passed = true;
        passed = takeScreenshot(img);
        assertTrue("plainStringItemTest", passed);
        form = null;
    }

    /**
     * Makes screenshot which shows different aspects of PLAIN ImageItem.
     */
    void plainImageItemTest()
    {
        String img = "FormImageItem.gif";
        form = new Form("Form test");

        ImageItem im1 = new ImageItem("header 1", smallImage,
                                      ImageItem.LAYOUT_LEFT, "justImage");
        ImageItem im2 = new ImageItem(null, smallImage,
                                      ImageItem.LAYOUT_VCENTER, "justImage");
        ImageItem im3 = new ImageItem(null, smallImage, ImageItem.LAYOUT_TOP,
                                      "justImage");
        ImageItem im4 = new ImageItem("header 2", smallImage,
                                      ImageItem.LAYOUT_CENTER, "justImage");
        ImageItem im5 = new ImageItem(null, smallImage,
                                      ImageItem.LAYOUT_VCENTER, "justImage");
        ImageItem im6 = new ImageItem(null, smallImage, ImageItem.LAYOUT_TOP,
                                      "justImage");
        ImageItem im7 = new ImageItem("header 2", smallImage,
                                      ImageItem.LAYOUT_RIGHT, "justImage");
        ImageItem im8 = new ImageItem(null, mediumImage,
                                      ImageItem.LAYOUT_VCENTER, "justImage");
        ImageItem im9 = new ImageItem(null, smallImage, ImageItem.LAYOUT_TOP,
                                      "justImage");
        form.append(im1);
        form.append(im2);
        form.append(im3);
        form.append(im4);
        form.append(im5);
        form.append(im6);
        form.append(im7);
        form.append(im8);
        form.append(im9);
        setCurrent(form);
        block(CHANGE_DISPLAYABLE_DELAY);
        boolean passed = true;
        passed = takeScreenshot(img);
        assertTrue("plainImageItemTest", passed);
        form = null;
    }

    /**
     * Screenshot based test cases which demonstrates combination of PLAIN and
     * BUTTON StrinItems, and PLAIN ImageItems.
     */
    void stringImageButtonTest()
    {
        form = new Form("Form test");
        String img = "FormImageStringButtonItem.gif";

        StringItem st1 = new StringItem("header 1", "Text 1");
        StringItem st2 = new StringItem(null, "Text 2");
        StringItem st3 = new StringItem(null, "Text 3");
        ImageItem im1 = new ImageItem("header 2", smallImage,
                                      ImageItem.LAYOUT_CENTER, "justImage");
        im1.setLayout(ImageItem.LAYOUT_CENTER);
        ImageItem im2 = new ImageItem("header 3", mediumImage,
                                      ImageItem.LAYOUT_CENTER, "justImage");
        ImageItem im3 = new ImageItem("header 4", smallImage,
                                      ImageItem.LAYOUT_CENTER, "justImage");
        StringItem st4 = new StringItem("header 5", "Click 1", Item.BUTTON);
        StringItem st5 = new StringItem(null, "Click 2", Item.BUTTON);
        StringItem st6 = new StringItem(null, "Click 3", Item.BUTTON);
        st4.setLayout(Item.LAYOUT_RIGHT);
        st4.addCommand(exitCmd);
        st5.addCommand(exitCmd);
        st6.addCommand(exitCmd);
        form.append(st1);
        form.append(st2);
        form.append(st3);

        form.append(im1);
        form.append(im2);
        form.append(im3);

        form.append(st4);
        form.append(st5);
        form.append(st6);
        setCurrent(form);
        // block(SpedeRunner.longTapDelay);

        boolean passed = true;
        passed = takeScreenshot(img);
        assertTrue("stringImageButtonTest", passed);
        form = null;
    }

}
