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
 * This class is to test the DateField (initial value and changing input mode)
 *
 */

public class DateFieldTests extends Form implements CommandListener
{

    // reference to the parent MIDlet
    private DateField_Test parent = null;

    // DateField item
    private DateField dateField = null;

    // Commands for changing InputMode
    private Command cmdDate = new Command("Date", Command.ITEM, 1);
    private Command cmdTime = new Command("Time", Command.ITEM, 1);
    private Command cmdDateTime = new Command("Date&Time", Command.ITEM, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public DateFieldTests(DateField_Test m)
    {

        //set the name of the form to be the name of MIDlet
        super(m.getClass().getName());
        this.parent = m;

        try
        {
            // create a new Datefield
            dateField = new DateField("DATEFIELD: ", DateField.DATE_TIME);

            // set an initial date
            Date newDate = new Date(0);
            dateField.setDate(newDate);

            // add DateField to Form
            append(dateField);

            // set input mode of the DateField
            dateField.setInputMode(DateField.DATE_TIME);
            dateField.setInputMode(DateField.TIME);
            dateField.setInputMode(DateField.DATE);

            // add commands to Form
            addCommand(cmdDate);
            addCommand(cmdTime);
            addCommand(cmdDateTime);
            addCommand(cmdExit);

            setCommandListener(this);

            // show DateField on display
            Display.getDisplay(parent).setCurrent(this);

        }
        catch (Exception ex)
        {
            Alert eAlert = new Alert("Exception","Exception was thrown: "+ex.toString(),null, AlertType.ERROR);
            Display.getDisplay(parent).setCurrent(eAlert);
        }
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdTime)
        {
            dateField.setInputMode(DateField.TIME);
            Display.getDisplay(parent).setCurrent(this);
        }
        else if (c == cmdDate)
        {
            dateField.setInputMode(DateField.DATE);
            Display.getDisplay(parent).setCurrent(this);
        }
        else if (c == cmdDateTime)
        {
            dateField.setInputMode(DateField.DATE_TIME);
            Display.getDisplay(parent).setCurrent(this);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }

}
