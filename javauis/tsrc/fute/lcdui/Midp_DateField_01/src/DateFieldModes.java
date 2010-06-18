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
 * This class creates DateField according to the selected mode.
 *
 */

public class DateFieldModes extends Form implements CommandListener
{

    //title to the DateField
    private String title = null;
    //DateField to test
    private DateField df = null;
    //command to go back to the DateFieldTests screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    /**
     * This constructor creates the DateFields according to the mode.
     * It also sets the title to be the name of the mode.
     *
     * @param mode The DateField mode
     */
    public DateFieldModes(int mode)
    {

        super("");
        switch (mode)
        {
        case DateField.DATE:
            title = "DATE";
            df = new DateField(title, mode);
            break;
        case DateField.TIME:
            title = "TIME";
            df = new DateField(title, mode);
            break;
        case DateField.DATE_TIME:
            title = "DATE_TIME";
            df = new DateField(title, mode);
            break;
        }
        append(df);
        addCommand(cmdBack);
        setCommandListener(this);
    }
    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
            DateFieldTests.show();
    }
}
