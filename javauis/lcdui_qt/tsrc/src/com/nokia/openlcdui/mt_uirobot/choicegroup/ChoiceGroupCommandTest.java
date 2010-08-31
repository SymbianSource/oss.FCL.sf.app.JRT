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

import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test ChoiceGroup commands. <br>
 * <br>
 * Created: 2008-11-04
 */
public class ChoiceGroupCommandTest extends ItemUITestBase
{

    /**
     * Constructor.
     */
    public ChoiceGroupCommandTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ChoiceGroupCommandTest(String sTestName)
    {
        super(sTestName);
    }

    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = ChoiceGroupCommandTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new ChoiceGroupCommandTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testCommandsExclusive");
        methodNames.addElement("testCommandsMultiple");
        return methodNames;
    }

    public void runTest() throws Throwable
    {
        if(getName().equals("testCommandsExclusive")) testChoiceGroupExclusiveCmds();
        else if(getName().equals("testCommandsMultiple")) testChoiceGroupMultipleCmds();
        else super.runTest();
    }

    public void testChoiceGroupExclusiveCmds()
    {
        ChoiceGroup choiceGroup = new ChoiceGroup("Exclusive", Choice.EXCLUSIVE);
        choiceGroup.append("item1", null);
        choiceGroup.append("item2", null);
        choiceGroup.append("item3", null);
        choiceGroup.append("item4", null);
        testCommand(choiceGroup);
    }

    public void testChoiceGroupMultipleCmds()
    {
        ChoiceGroup choiceGroup = new ChoiceGroup("Multiple", Choice.MULTIPLE);
        choiceGroup.append("item1", null);
        choiceGroup.append("item2", null);
        choiceGroup.append("item3", null);
        choiceGroup.append("item4", null);
        testCommand(choiceGroup);
    }


}
