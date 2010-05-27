/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

/**
 * This class is to test the TextField test case:
 * -TextField remembers the cursor position when the control loses focus and
 * regains later
 *
 */

public class TextFieldTests extends Form implements CommandListener
{

    //reference to the parent MIDlet
    private Midp_TextField_03 parent = null;

    private final int MAX_SIZE = 50;

    private TextField testTextField = null;

    private Gauge dummyGauge = null;

    private ChoiceGroup cg = null;

    private TextField dummyTextField = null;

    private ChoiceGroup dummyChoiceGroup = null;

    private String[] cgStrs = {"1st element", "2nd element", "3rd element", "4th element"};

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     *
     * @param m The parent MIDlet of this class
     */
    public TextFieldTests(Midp_TextField_03 m)
    {
        super(m.getClass().getName());
        this.parent =m;
        testTextField = new TextField("Test TextField", null, MAX_SIZE, TextField.ANY);
        dummyGauge = new Gauge("Interactive Gauge", true, 20, 5);
        dummyTextField = new TextField("Dummy TextField", null, MAX_SIZE, TextField.ANY);
        dummyChoiceGroup = new ChoiceGroup("Dummy ChoiceGroup", ChoiceGroup.EXCLUSIVE, cgStrs, null);
        append(testTextField);
        append(dummyGauge);
        append(dummyTextField);
        append(dummyChoiceGroup);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
