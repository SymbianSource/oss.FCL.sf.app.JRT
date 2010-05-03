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
package com.nokia.openlcdui.mt_uirobot.gauge;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test gauge commands. <br>
 * <br>
 * Created: 2008-09-15
 */
public class GaugeCommandsTest extends ItemUITestBase {

    private static final int GAUGE_MAX_VALUE = 10;
    private static final int GAUGE_MAX_VALUE_2 = 5;
    private static final int GAUGE_TEST_VALUE = 2;

    /**
     * Constructor.
     */
    public GaugeCommandsTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public GaugeCommandsTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = GaugeCommandsTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new GaugeCommandsTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testShowTimedAlert");
        methodNames.addElement("testCustomListener");
        methodNames.addElement("testCustomListenerCustomCommand");
        methodNames.addElement("testKeypressOnCustomCommands");
        methodNames.addElement("testLongTextScrolling");
        methodNames.addElement("testImageAndTextCombinations");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testCommands")) testCommands();
        // TODO: currently failing because ProgressBar is not focusable.
        // It is not clear should it be focusable in future?
        else if (getName().equals("testNoninteractiveCommands")) testNoninteractiveCommands();
        else if (getName().equals("testValueChange")) testValueChange();
        else if (getName().equals("testItemState")) testItemState();
        else super.runTest();
    }    
   

    /**
	 * Tests the basic functionality of command added to Gauge.
	 */
	public void testCommands() {
		Gauge gauge = new Gauge("interactive, max=10, initial=0", true,
				GAUGE_MAX_VALUE, 0);

		Command ok = new Command("Ok", "", Command.ITEM, 0);
		gauge.addCommand(ok);
		gauge.setItemCommandListener(this);

		form.append(gauge);

		Display.getDisplay(getMIDlet()).setCurrent(form);
		block(CHANGE_DISPLAYABLE_DELAY);

		// Click command and verify it works:
		key(Key.CBA1);
		assertItemCmdListener("", ok, gauge);
	}

    /**
	 * It should be possible to add commands to non-interactive gauges too and
	 * in that case gauges should be selectable.
	 */
    public void testNoninteractiveCommands() {
        Gauge gauge = new Gauge("non-interactive, max=10, initial=0",
                false, GAUGE_MAX_VALUE, 0);
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        button1.addCommand(new Command("Ok1", "", Command.ITEM, 0));

        gauge.addCommand(ok);
        gauge.setItemCommandListener(this);

        form.append(gauge);

        Display.getDisplay(getMIDlet()).setCurrent(form);
        block(CHANGE_DISPLAYABLE_DELAY);

        // Click LSK and verify it activates command:
        key(Key.CBA1);
		assertItemCmdListener("case1", ok, gauge);

        // Add button, change focus to it and make sure it is possible
        // to move focus back to gauge:
        form.append(button1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.DownArrow);
        key(Key.UpArrow);
        key(Key.CBA1);
        assertItemCmdListener("case2", ok, gauge);
    }

    /**
	 * Tests Gauge's value changing.
	 */
	public void testValueChange() {
		Gauge gauge = new Gauge("interactive, max=10, initial=0", true,
				GAUGE_MAX_VALUE, 0);
		StringItem button1 = new StringItem("", "click!", Item.BUTTON);
		button1.addCommand(new Command("Ok1", "", Command.ITEM, 0));

		form.append(gauge);

		// Add dummy button after gauge. This makes sure that the focus
		// is not moving when pressing right-arrow when gauge is selected.
		form.append(button1);

		setCurrent(form);

		// Change gauge value:
		key(Key.RightArrow);
		assertEquals("Wrong gauge value (case 1).", 1, gauge.getValue());

		// Change gauge value back to original:
		key(Key.LeftArrow);
		assertEquals("Wrong gauge value (case 2).", 0, gauge.getValue());

		// Change value dynamically:
		gauge.setValue(GAUGE_MAX_VALUE);

		// Change maximum value to be lower than current value:
		gauge.setMaxValue(GAUGE_MAX_VALUE_2);

		// Make sure the current value is changed to maximum:
		assertEquals("Current value not changed to maximum.",
				GAUGE_MAX_VALUE_2, gauge.getValue());

		// Set value to minimum and then move value by using keypresses.
		// That makes sure the visual appearance of gauge is changed dynamically
		// when value is changed programmatically:
		gauge.setValue(0);
		key(Key.RightArrow);
		key(Key.RightArrow);

		assertEquals("Value not expected after setValue()-call.",
				GAUGE_TEST_VALUE, gauge.getValue());

		// Remove gauge from form and try to set value again:
		form.deleteAll();
		gauge.setValue(GAUGE_TEST_VALUE);
		assertEquals("Value not expected after gauge removed from form.",
				GAUGE_TEST_VALUE, gauge.getValue());
	}

    /**
	 * Tests that Form receives ItemStateChanged-events when gauge's value is
	 * changed.
	 */
    public void testItemState() {
        Gauge gauge = new Gauge("interactive, max=10, initial=0",
                true, GAUGE_MAX_VALUE, 0);

        form.append(gauge);

        setCurrent(form);

        // Adjust gauge's value and make sure the listener is called:
        key(Key.RightArrow);
        assertItemStateChanged("", gauge);
    }

}
