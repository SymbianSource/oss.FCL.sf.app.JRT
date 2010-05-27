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
 * This class is the starting point to select the Alert tests.
 * It lets the user to launch the selected type of test.
 *
 */

public class AlertTests extends List implements CommandListener
{

    //reference to parent MIDlet to go to the different screens
    private static Midp_Alert_02 parent=null;

    private static AlertTests instance = null;
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * This constructor displays the IMPLICIT List with MIDlet name as its's title.
     * It then adds "Test1" and "Test2" elements to this List.
     *
     * @param the parent of this class
     */
    public AlertTests(Midp_Alert_02 parent)
    {
        super(parent.getClass().getName(), Choice.IMPLICIT);
        this.parent = parent;
        this.append("Test1", null);
        this.append("Test2", null);
        this.append("Test3", null);
        this.append("Test4", null);
        this.addCommand(cmdExit);
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
        Alert newAlert = new Alert(parent.getClass().getName(),  parent.getClass().getName(), null, AlertType.INFO);
        Display.getDisplay(parent).setCurrent(newAlert,instance);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        int index = instance.getSelectedIndex();
        String element = instance.getString(index);
        if (c == List.SELECT_COMMAND)
        {
            if (element.equals("Test1"))
                Display.getDisplay(parent).setCurrent(new Test1());
            else if (element.equals("Test2"))
                Display.getDisplay(parent).setCurrent(new Test2());
            else if (element.equals("Test3"))
                Display.getDisplay(parent).setCurrent(new Test3());
            else if (element.equals("Test4"))
                Display.getDisplay(parent).setCurrent(new Test4());
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
