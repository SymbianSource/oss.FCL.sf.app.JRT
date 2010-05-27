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
 * This class is called when Midp_List_01 MIDlet is launched.It displays
 * a ChoiceGroup item on this Form and lets the user to select the type of
 * List before running the tests
 *
 */

public class ListTests extends Form implements CommandListener
{

    //reference to the MIDlet to go to different screens
    private static Midp_List_01 parent = null;

    //ChoiceGroup which contains the list types to be selected
    private ChoiceGroup cg = null;

    //this instance
    private static ListTests instance = null;

    private String[] listTypes = { "Exclusive", "Implicit", "Multiple" };

    //the command to test Test1
    private Command cmdTest1 = new Command("Test1", Command.SCREEN, 1);
    //the command to test Test2
    private Command cmdTest2 = new Command("Test2", Command.SCREEN, 1);
    //the command to test Test3
    private Command cmdTest3 = new Command("Test3", Command.SCREEN, 1);
    //the command to test Test4
    private Command cmdTest4 = new Command("Test4", Command.SCREEN, 1);
    //the command to test Test5
    private Command cmdTest5 = new Command("Test5", Command.SCREEN, 1);
    //the command to test Test6
    private Command cmdTest6 = new Command("Test6", Command.SCREEN, 1);
    //the command to test Test7
    private Command cmdTest7 = new Command("Test7", Command.SCREEN, 1);
    //the command to test Test8
    private Command cmdTest8 = new Command("Test8", Command.SCREEN, 1);
    //the command to exit midlet
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);


    public static int type;

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public ListTests(Midp_List_01 parent)
    {
        super(parent.getClass().getName());
        this.parent = parent;
        //create ChoiceGroup
        cg = new ChoiceGroup("Select List type:", Choice.EXCLUSIVE, listTypes, null);
        append(cg);
        //addCommands
        addCommand(cmdTest1);
        addCommand(cmdTest2);
        addCommand(cmdTest3);
        addCommand(cmdTest4);
        addCommand(cmdTest5);
        addCommand(cmdTest6);
        addCommand(cmdTest7);
        addCommand(cmdTest8);
        addCommand(cmdExit);
        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
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
        if (t.equals(listTypes[0]))
            type = List.EXCLUSIVE;
        else if (t.equals(listTypes[1]))
            type = List.IMPLICIT;
        else if (t.equals(listTypes[2]))
            type = List.MULTIPLE;
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
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method
    public static String whatFace(int face)
    {
        if (face == Font.FACE_SYSTEM)
            return "SYSTEM";
        else if (face == Font.FACE_MONOSPACE)
            return "MONOSPACE";
        else if (face == Font.FACE_PROPORTIONAL)
            return "PROPORTIONAL";
        else return null;
    }

    public static String whatStyle(int style)
    {
        if (style== Font.STYLE_PLAIN)
            return "PLAIN";
        else if (style== Font.STYLE_ITALIC)
            return "ITALIC";
        else if (style== Font.STYLE_BOLD)
            return "BOLD";
        else if (style== Font.STYLE_UNDERLINED)
            return "UNDERLINED";
        else return "MIXED";
    }

    public static String whatSize(int size)
    {
        if (size== Font.SIZE_SMALL)
            return "SMALL";
        else if (size== Font.SIZE_MEDIUM)
            return "MEDIUM";
        else if (size== Font.SIZE_LARGE)
            return "LARGE";
        else return null;
    }


}




