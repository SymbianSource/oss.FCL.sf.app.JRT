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
 * This class lets the user to select mode of the DateField to be created.
 *
 */

public class DateFieldTests extends Form implements CommandListener
{

    private static Midp_DateField_01 parent = null;

    //ChoiceGroup to select the DateField mode
    private ChoiceGroup cg = null;

    //this instance
    private static DateFieldTests instance = null;

    //elements of cg
    private String[] dfModes = {"DATE", "TIME", "DATE_TIME"};

    //command to create the DateField for selected mode
    private Command cmdOk = new Command("Ok", Command.OK, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * This constructor creates the cg and appends to this Form
     *
     * @param parent The parent MIDlet of this class
     */
    public DateFieldTests(Midp_DateField_01 parent)
    {
        //set the name of the Form to be the name of the MIDlet
        super(parent.getClass().getName());
        this.parent = parent;
        cg = new ChoiceGroup("Select DF Mode:", Choice.EXCLUSIVE, dfModes, null);
        append(cg);
        addCommand(cmdOk);
        addCommand(cmdExit);
        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
        //show this screen
        show();
    }

    /**
     *  Sets this to be the current screen
     *
     *@return    The Parent value
     */
    public static MIDlet getParent()
    {
        return parent;
    }

    /**
     *  Show this screen
     */
    public static void show()
    {
        Display.getDisplay(parent).setCurrent(instance);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        int index = cg.getSelectedIndex();
        String t = cg.getString(index);
        int mode = -1;
        if (t.equals(dfModes[0]))
        {
            mode = DateField.DATE;
        }
        else if (t.equals(dfModes[1]))
        {
            mode = DateField.TIME;
        }
        else if (t.equals(dfModes[2]))
        {
            mode = DateField.DATE_TIME;
        }
        if (c == cmdOk)
            Display.getDisplay(parent).setCurrent(new DateFieldModes(mode));
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method

}




