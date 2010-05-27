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
 * This class is to test all the StringItem tests
 *
 */

public class FormStringItemTests extends Form implements CommandListener
{

    //referemce to the parent MIDlet
    private StringOutOfBounds m = null;

    //the form to add the StringItems
    private Form stringItemForm = null;

    //the label to create the StringItem
    private TextField label = null;

    //the text to create the StringItem
    private TextField text = null;

    //the StringItem to test
    private StringItem si = null;
    private StringItem siOrig1 = new StringItem("Label_Original 1", "Text1");
    private StringItem siOrig2 = new StringItem("Label_Original 2", "Text2");
    //the command to go back to the main screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the command to create the StringItem with the entered label and text
    private Command cmdCreate = new Command("Create", Command.SCREEN, 1);
    private Command cmdOtherForm = new Command("Other Form", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public FormStringItemTests(StringOutOfBounds m)
    {
        //set the name of the form to be the name of MIDlet
        super(m.getClass().getName());
        this.m = m;

        this.append(siOrig1);
        this.append(siOrig2);

        addCommand(cmdCreate);
        addCommand(cmdOtherForm);
        addCommand(cmdBack);
        addCommand(cmdExit);
        setCommandListener(this);

        Display.getDisplay(m).setCurrent(this);

        //create StringItemForm
        stringItemForm = new Form("StringItem");
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
        {
            Display.getDisplay(m).setCurrent(this);
        }
        else if (c == cmdCreate)
        {
            try
            {
                si = new StringItem("New Label","New Text");
                this.insert(this.size()+1,si);
                Display.getDisplay(m).setCurrent(this);
            }
            catch (IndexOutOfBoundsException ie)
            {
                Alert ieAlert = new Alert("IndexOOB",ie.toString(),null, null);
                Display.getDisplay(m).setCurrent(ieAlert);
            }
            catch (Exception e)
            {
                Alert eAlert = new Alert("Wrong Exception","Wrong Exception was thrown: "+e.toString(),null, null);
                Display.getDisplay(m).setCurrent(eAlert);
            }
        }
        else if (c == cmdOtherForm)
        {
            try
            {
                stringItemForm.append(siOrig1);
                stringItemForm.append(siOrig2);
                Display.getDisplay(m).setCurrent(stringItemForm);
            }
            catch (IllegalStateException ise)
            {
                Alert iseAlert = new Alert("Illegal State",ise.toString(),null, null);
                Display.getDisplay(m).setCurrent(iseAlert);
            }
        }
        else if (c == cmdExit)
        {
            m.destroyApp(false);
            m.notifyDestroyed();
        }
    }
}
