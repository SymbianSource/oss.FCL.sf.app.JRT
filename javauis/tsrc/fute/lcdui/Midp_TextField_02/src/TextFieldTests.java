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
 * This class is to test the TextFields editing space indicators.
 */

public class TextFieldTests extends Form implements CommandListener
{

    //reference to the parent MIDlet
    private Midp_TextField_02 parent = null;

    //Command to get the size of the TextField
    private Command cmdSize = new Command("Size", Command.SCREEN, 1);

    //Command to get the maxSize of the TextField
    private Command cmdMaxSize = new Command("MaxSize", Command.SCREEN, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    //The maximum size of the TextField
    private final int MAX_SIZE = 50;
    private TextField tf = null;

    /**
     *
     * @param m The parent MIDlet of this class
     */
    public TextFieldTests(Midp_TextField_02 m)
    {

        super(m.getClass().getName());
        this.parent =m;
        tf = new TextField("ANY", "Editing space indicators are presented.",
                           MAX_SIZE, TextField.ANY);
        append(tf);
        addCommand(cmdSize);
        addCommand(cmdMaxSize);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  d  Should be equal to this.
     */

    public void commandAction(Command c, Displayable d)
    {
        if (c == cmdSize)
        {
            Alert a = new Alert(null, "TextField size: " + tf.size(), null, null);
            Display.getDisplay(parent).setCurrent(a);
        }
        else if (c == cmdMaxSize)
        {
            Alert a = new Alert(null, "TextField maximum size: " + tf.getMaxSize(),
                                null, null);
            Display.getDisplay(parent).setCurrent(a);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
