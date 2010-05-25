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
import java.util.*;

/**
 * This class is to test the DateField with ItemStateListener
 *
 */

public class DateFieldTests extends Form implements ItemStateListener, CommandListener
{

    //referemce to the parent MIDlet
    private Midp_DateField_02 m = null;

    private StringItem si = null;

    private DateField df = null;

    private Command cmdCurrDate = new Command("Curr Date", Command.ITEM, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public DateFieldTests(Midp_DateField_02 m)
    {
        //set the name of the form to be the name of MIDlet
        super(m.getClass().getName());
        this.m = m;

        si = new StringItem("Result:", null);
        df = new DateField("Enter Date to test:", DateField.DATE);
        setItemStateListener(this);
        addCommand(cmdExit);
        setCommandListener(this);
        append(df);
        append(si);
        Display.getDisplay(m).setCurrent(this);
    }

    public void itemStateChanged(Item i)
    {
        if (i == df)
        {
            si.setText("itemStateChanged is called");
        }
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdExit)
        {
            m.destroyApp(false);
            m.notifyDestroyed();
        }
    }
}
