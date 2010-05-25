/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * This class is called when the Midp_ChoiceGroup_01 MIDlet is launched. It displays
 * the ChoiceGroup item on this Form and lets the tester to select the type of
 * ChoiceGroup before invoking the tests.
 *
 */

public class ChoiceGroupTests extends Form implements CommandListener
{

    //reference to parent MIDlet to go to the different screens
    private static Midp_ChoiceGroup_01 parent=null;

    //the ChoiceGroup, to select the cgType
    private ChoiceGroup cg = null;

    //the instance of this class
    private static ChoiceGroupTests instance = null;

    //the cgTypes added to the ChoiceGroup cg
    private String[] cgTypes = {"Multiple ChoiceGroup", "Exclusive ChoiceGroup", "POPUP ChoiceGroup"};

    //commands to invoke the respective tests
    private Command cmdTest1 = new Command("Test1", Command.SCREEN, 1);
    private Command cmdTest2 = new Command("Test2", Command.SCREEN, 1);
    private Command cmdTest3 = new Command("Test3", Command.SCREEN, 1);
    private Command cmdTest4 = new Command("Test4", Command.SCREEN, 1);
    private Command cmdTest5 = new Command("Test5", Command.SCREEN, 1);
    private Command cmdTest6 = new Command("Test6", Command.SCREEN, 1);
    private Command cmdTest7 = new Command("Test7", Command.SCREEN, 1);
    private Command cmdTest8 = new Command("Test8", Command.SCREEN, 1);
    private Command cmdTest9 = new Command("Test9", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * This constructor creates and appends the ChoiceGroup "cg"to this Form,
     * adds all the test commands and sets the current screen as this Form.
     *
     * @param parent the parent MIDlet of this class.
     */
    public ChoiceGroupTests(Midp_ChoiceGroup_01 parent)
    {

        //label as the MIDlet class name
        super(parent.getClass().getName());
        this.parent = parent;
        cg = new ChoiceGroup("Select CG type:", Choice.EXCLUSIVE, cgTypes, null);
        append(cg);
        //add commands
        addCommand(cmdTest1);
        addCommand(cmdTest2);
        addCommand(cmdTest3);
        addCommand(cmdTest4);
        addCommand(cmdTest5);
        addCommand(cmdTest6);
        addCommand(cmdTest7);
        addCommand(cmdTest8);
        addCommand(cmdTest9);
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
        int type;
        if (t.equals(cgTypes[0]))
        {
            type = ChoiceGroup.MULTIPLE;
        }
        else if (t.equals(cgTypes[1]))
        {
            type = ChoiceGroup.EXCLUSIVE;
        }
        else
        {
            type = ChoiceGroup.POPUP;
        }
        if (c == cmdTest1)
            Display.getDisplay(parent).setCurrent(new Test1(type));
        else if (c== cmdTest2)
            Display.getDisplay(parent).setCurrent(new Test2(type));
        else if (c == cmdTest3)
            Display.getDisplay(parent).setCurrent(new Test3(type));
        else if (c == cmdTest4)
            Display.getDisplay(parent).setCurrent(new Test4(type));
        else if (c == cmdTest5)
            Display.getDisplay(parent).setCurrent(new Test5(type));
        else if (c == cmdTest6)
            Display.getDisplay(parent).setCurrent(new Test6(type));
        else if (c == cmdTest7)
            Display.getDisplay(parent).setCurrent(new Test7(type));
        else if (c == cmdTest8)
            Display.getDisplay(parent).setCurrent(new Test8(type));
        else if (c == cmdTest9)
            Display.getDisplay(parent).setCurrent(new Test9(type));
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
