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
package com.nokia.openlcdui.mt_uirobot.choicegroup;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test POPUP styled ChoiceGroup. <br>
 * <br>
 * Created: 2008-11-04
 */
public class ChoiceGroupPopupTest extends ItemUITestBase {

    /**
     * Constructor.
     */
    public ChoiceGroupPopupTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ChoiceGroupPopupTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = ChoiceGroupPopupTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ChoiceGroupPopupTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testOpenPopup");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testOpenPopup")) testOpenPopup();
        else super.runTest();
    }
    
    /**
     * Test command on Items.
     */
    public void testOpenPopup() {
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
