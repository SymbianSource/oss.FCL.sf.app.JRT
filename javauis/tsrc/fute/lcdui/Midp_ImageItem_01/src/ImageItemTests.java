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
 *
 */

public class ImageItemTests extends List implements CommandListener
{

    //This is the reference to parent MIDlet so that various test screen can use
    // use it
    private static Midp_ImageItem_01 parent = null;

    //the this instance
    private static ImageItemTests instance = null;

    //The elements in "this" list
    private String[] tests = {"Test1", "Test2", "Test3", "Test4", "Test5"};

    //img to be used for Test1
    private Image bigImageW = null;

    //img to be used for Test2
    private Image bigImageH = null;

    //img to be used for Test3
    private Image bigImage = null;

    //The command to invoke the tests
    private Command cmdOk = new Command("Ok", Command.OK, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * The constructor appends the this list elements and sets
     * the label of the list to be the MIDlet class name and sets
     * the current displayable this list.
     *
     * @param  parent  is the parent midlet for this class used so that the
     * displayable can control its own behavior.
     */
    public ImageItemTests(Midp_ImageItem_01 parent)
    {
        super(parent.getClass().getName(), List.IMPLICIT);
        this.parent = parent;

        for (int i = 0; i< tests.length; i++)
            append(tests[i], null);
        //add Ok command to this list
        addCommand(cmdOk);
        addCommand(cmdExit);
        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
        //set this list as the current display
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
        if (c == cmdOk)
        {
            int index = instance.getSelectedIndex();
            String t = instance.getString(index);
            if (t.equals(tests[0]))
            {
                bigImageW = Images.getInstance().bigImageW;
                Display.getDisplay(parent).setCurrent(new Test1(bigImageW));
            }
            else if (t.equals(tests[1]))
            {
                bigImageH = Images.getInstance().bigImageH;
                Display.getDisplay(parent).setCurrent(new Test1(bigImageH));
            }
            else if (t.equals(tests[2]))
            {
                bigImage = Images.getInstance().bigImage;
                Display.getDisplay(parent).setCurrent(new Test1(bigImage));
            }
            else if (t.equals(tests[3]))
                Display.getDisplay(parent).setCurrent(new Test2());
            else if (t.equals(tests[4]))
                Display.getDisplay(parent).setCurrent(new Test5(parent));
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method
}
