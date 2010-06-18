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
 * This class is called when the Midp_List_02 MIDlet is launched. It displays
 * the ChoiceGroup item on this Form and lets the tester to select the type of
 * List before invoking the tests
 *
 */

public class ListTests extends Form implements CommandListener
{

    //reference to parent MIDlet to go to the different screens
    private static Midp_List_02 parent=null;

    //The ChoiceGroup, to select the List type
    private ChoiceGroup cg = null;

    //the instance of this class
    private static ListTests instance = null;

    //The cgTypes added to the ChoiceGroup cg
    private String[] listTypes = {"Exclusive ChoiceGroup", "Implicit ChoiceGroup"};

    //commands to invoke the respective tests
    private Command cmdTest1 = new Command("Test1", Command.SCREEN, 1);
    //the command to exit midlet
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public ListTests(Midp_List_02 parent)
    {

        //label as the MIDlet class name
        super(parent.getClass().getName());
        this.parent = parent;
        cg = new ChoiceGroup("Select List type:", Choice.EXCLUSIVE, listTypes, null);
        append(cg);
        //add command
        addCommand(cmdTest1);
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
        int type = 0;
        if (t.equals(listTypes[0]))
            type = ChoiceGroup.EXCLUSIVE;
        else if (t.equals(listTypes[1]))
            type = ChoiceGroup.IMPLICIT;
        if (c == cmdTest1)
            Display.getDisplay(parent).setCurrent(new Test1(type));
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method

}
