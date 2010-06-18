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
 * This class is to test all TextField constraints
 *
 */

public class TextFieldTests extends List implements CommandListener
{

    //parent MIDlet
    private static Midp_TextField_01 parent = null;
    //TextField to set TextBox title for test3
    private TextField tf = null;
    private Form testForm = null;
    //elements in the listConstraints
    private final String listElements[] =
    {
        "ANY", "URL", "PHONENUMBER", "NUMERIC",
        "EMAILADDR", "PASSWORD", "PWD|EMAILADDR",
        "PWD|NUMERIC", "PWD|PNO", "PWD|URL",
        "DECIMAL", "INITAL_CAPS_SENTENCE",
        "INITIAL_CAPS_WORD", "UNEDITABLE", "NON_PREDICTIVE",
        "SENSITIVE",
        "DECIMAL|PWD", "NUMERIC|UNEDITABLE",
        "DECIMAL|UNEDITABLE", "URL|UNEDITABLE", "EMAILADDR|UNEDITABLE",
        "PHONENUMBER|UNEDITABLE", "ANY|PWD|UNEDITABLE", "NUMERIC|PWD|UNEDITABLE",
        "DECIMAL|PWD|UNEDITABLE", "URL|PWD|UNEDITABLE", "EMAILADDR|PWD|UNDEDITABLE",
        "PNO|PWD|UNDEDITABLE"
    };
    //Command to go Back to the previous Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //Ok command which is added to this List
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    //maximum size of the TextField
    private final int MAX_SIZE = 50;

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public TextFieldTests(Midp_TextField_01 parent)
    {
        //set the title of the list as the name of the MIDlet
        super(parent.getClass().getName(), List.IMPLICIT);
        this.parent = parent;
        for (int i =0; i<listElements.length; i++)
            append(listElements[i], null);
        testForm = new Form("Test Form");
        testForm.addCommand(cmdBack);
        testForm.setCommandListener(this);

        addCommand(cmdOk);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    /**
     *
     * @param title The TextField title
     * @param constraint The TextField constraint
     */
    private void addTextField(String title, int constraint)
    {
        tf = new TextField(title, null, MAX_SIZE, constraint);
        if (testForm.size() != 0)
            testForm.deleteAll();
        testForm.append(tf);
        Display.getDisplay(parent).setCurrent(testForm);
    }

    private void addNumericTextField(String title, int constraint)
    {
        tf = new TextField(title, null, MAX_SIZE, constraint);
        if (testForm.size() != 0)
            testForm.deleteAll();
        testForm.append(tf);
        Display.getDisplay(parent).setCurrent(testForm);
    }

    private void addUneditableTextField(String title, int constraint)
    {
        tf = new TextField(title, "Uneditable", MAX_SIZE, constraint);
        if (testForm.size() != 0)
            testForm.deleteAll();
        testForm.append(tf);
        Display.getDisplay(parent).setCurrent(testForm);
    }

    private void addNumericUneditableTextField(String title, int constraint)
    {
        tf = new TextField(title, "123", MAX_SIZE, constraint);
        if (testForm.size() != 0)
            testForm.deleteAll();
        testForm.append(tf);
        Display.getDisplay(parent).setCurrent(testForm);
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
            Display.getDisplay(parent).setCurrent(this);
        else if (c == cmdOk || c == List.SELECT_COMMAND)
        {
            int index = getSelectedIndex();
            String element = getString(index);
            if (element.equals(listElements[0]))
                addTextField("ANY", TextField.ANY);
            else if (element.equals(listElements[1]))
                addTextField("URL", TextField.URL);
            else if (element.equals(listElements[2]))
                addTextField("PHONENUMBER", TextField.PHONENUMBER);
            else if (element.equals(listElements[3]))
                addNumericTextField("NUMERIC", TextField.NUMERIC);
            else if (element.equals(listElements[4]))
                addTextField("EMAILADDR", TextField.EMAILADDR);
            else if (element.equals(listElements[5]))
                addTextField("PASSWORD", TextField.PASSWORD);
            else if (element.equals(listElements[6]))
                addTextField("PWD|EMAILADDR", TextField.PASSWORD|TextField.EMAILADDR);
            else if (element.equals(listElements[7]))
                addNumericTextField("PWD|NUMERIC", TextField.PASSWORD|TextField.NUMERIC);
            else if (element.equals(listElements[8]))
                addTextField("PWD|PNO", TextField.PASSWORD|TextField.PHONENUMBER);
            else if (element.equals(listElements[9]))
                addTextField("PWD|URL", TextField.PASSWORD|TextField.URL);
            else if (element.equals(listElements[10]))
                addTextField("DECIMAL", TextField.DECIMAL);
            else if (element.equals(listElements[11]))
                addTextField("INITIAL_CAPS_SENTENCE", TextField.INITIAL_CAPS_SENTENCE);
            else if (element.equals(listElements[12]))
                addTextField("INITIAL_CAPS_WORD", TextField.INITIAL_CAPS_WORD);
            else if (element.equals(listElements[13]))
                addUneditableTextField("UNEDITABLE", TextField.UNEDITABLE);
            else if (element.equals(listElements[14]))
                addTextField("NON_PREDICTIVE", TextField.NON_PREDICTIVE);
            else if (element.equals(listElements[15]))
                addTextField("SENSITIVE", TextField.SENSITIVE);
            else if (element.equals(listElements[16]))
                addNumericTextField("DECIMAL|PWD", TextField.DECIMAL|TextField.PASSWORD);
            else if (element.equals(listElements[17]))
                addNumericUneditableTextField("NUMERIC|UNEDITABLE", TextField.NUMERIC|TextField.UNEDITABLE);
            else if (element.equals(listElements[18]))
                addNumericUneditableTextField("DECIMAL|UNEDITABLE", TextField.DECIMAL|TextField.UNEDITABLE);
            else if (element.equals(listElements[19]))
                addUneditableTextField("URL|UNEDITABLE", TextField.URL|TextField.UNEDITABLE);
            else if (element.equals(listElements[20]))
                addUneditableTextField("EMAILADDR|UNEDITABLE", TextField.EMAILADDR|TextField.UNEDITABLE);
            else if (element.equals(listElements[21]))
                addNumericUneditableTextField("PHONENUMBER|UNEDITABLE", TextField.PHONENUMBER|TextField.UNEDITABLE);
            else if (element.equals(listElements[22]))
                addUneditableTextField("ANY|PWD|UNEDITABLE", TextField.ANY|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[23]))
                addNumericUneditableTextField("NUMERIC|PWD|UNEDITABLE", TextField.NUMERIC|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[24]))
                addNumericUneditableTextField("DECIMAL|PWD|UNEDITABLE", TextField.DECIMAL|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[25]))
                addUneditableTextField("URL|PWD|UNEDITABLE", TextField.URL|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[26]))
                addUneditableTextField("EMAILADDR|PWD|UNDEDITABLE", TextField.EMAILADDR|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[27]))
                addNumericUneditableTextField("PNO|PWD|UNDEDITABLE", TextField.PHONENUMBER|TextField.PASSWORD|TextField.UNEDITABLE);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
