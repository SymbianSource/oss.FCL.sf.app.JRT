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
 * This class is to test the various methods of TextField
 *
 */

public class TextFieldMethods extends Form implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet m = null;

    //the TextField methods as elements in the listMethodsStr
    private String listMethodsStr[] =
    {
        "setChars", "setConstraints", "setMaxSize", "setString",
        "size", "setLabel", "insert(char)", "insert(string)", "remove", "create",
        "create with 3 lines", "create with 6 lines"
    };

    private char c1[] =  { 'N', 'O', 'K', 'I', 'A'};

    private char c2[] = { 'M', 'I', 'D', 'P'};

    //the char to be inserted
    private char insertedChar[] = { 't', 'h', 'e', 'i', 'n', 's', 'e', 'r', 't', 'e', 'd', 'c', 'h',
                                    'a', 'r'
                                  };

    private String listSetCharsStr[] = { "NOKIA", "MIDP" };

    //the string to test insert( String )
    private String insertedStr = "inserted String";

    //the TextField constraints as elements to listSetConstraints list
    private String listSetConstraintsStr[]=
    {
        "ANY", "URL", "PHONENUMBER", "NUMERIC",
        "EMAILADDR", "PASSWORD", "PWD|EMAILADDR",
        "PWD|NUMERIC", "PWD|PNO", "PWD|URL",
        "DECIMAL", "INITAL_CAPS_SENTENCE",
        "INITIAL_CAPS_WORD", "UNEDITABLE", "NON_PREDICTIVE",
        "SENSITIVE"
    };

    //the TextField max size as elements to listSetMaxSize lsit
    private String listSetMaxSizeStr[] = { "increase", "decrease" };

    //the TextField text as Strings to listSetString list
    private String listSetStringStr[] = { "String", "NULL" };

    //the TextField labels as elements to listSetLabel list
    private String listSetLabelStr[] = { "Label", "NULL" };

    //the list to add listMethodsStr
    private List listMethods = null;

    //the list to add listSetLabelStr
    private List listSetLabel = null;

    //the list to add listSetStringStr
    private List listSetString = null;

    //the list to add lsitSetConstraintsStr
    private List listSetConstraints = null;

    //the list to add listSetMaxSizeStr
    private List listSetMaxSize = null;

    //the list to add listSetCharsStr1 and listSetCharsStr2
    private List listSetChars = null;

    //the list to add listInsertStringStr
    private List listInsertString = null;

    //the TextField to test
    private TextField tf;

    //the command to go back to the Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the commmand to invoke the StringItem method for testing
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    //the command to display the listMethods Screen
    private Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    //the command to set the TextField "tf"
    private Command cmdSet = new Command("Set", Command.SCREEN, 1);

    public TextFieldMethods(MIDlet m)
    {

        super(m.getClass().getName());
        this.m = m;

        tf = new TextField("TextField", "This is the text.", 30, TextField.ANY);
        append(tf);

        addCommand(cmdBack);
        addCommand(cmdMethods);
        setCommandListener(this);
        m = FormUpdateTests.getParent();
        Display.getDisplay(m).setCurrent(this);
    }

    /**
     * This method creates the listMethods List and sets the
     * current screen as listMethods.
     *
     */
    private void addListMethods()
    {
        listMethods = new List("Methods", List.IMPLICIT, listMethodsStr, null);

        listMethods.addCommand(cmdBack);
        listMethods.addCommand(cmdOk);
        listMethods.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listMethods);
    }

    /**
     * This method creates the listSetLabel List and sets the
     * current screen as listSetLabel .
     *
     */
    private void addListSetLabel()
    {
        listSetLabel = new List("SetLabel", List.IMPLICIT, listSetLabelStr, null);

        listSetLabel.addCommand(cmdBack);
        listSetLabel.addCommand(cmdSet);
        listSetLabel.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetLabel);
    }

    /**
     * This method creates the listSetString List and sets the
     * current screen as listSetString.
     *
     */
    private void addListSetString()
    {
        listSetString = new List("SetText", List.IMPLICIT, listSetStringStr, null);

        listSetString.addCommand(cmdBack);
        listSetString.addCommand(cmdSet);
        listSetString.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetString);
    }

    /**
     * This method creates the listSetChars List and sets the
     * current screen as listSetChars.
     *
     */
    private void addListSetChars()
    {
        listSetChars = new List("SetChars", List.IMPLICIT, listSetCharsStr, null);

        listSetChars.addCommand(cmdBack);
        listSetChars.addCommand(cmdSet);
        listSetChars.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetChars);
    }

    /**
     * This method creates the listSetConstraints List and sets the
     * current screen as listSetConstraints.
     *
     *@param cScreen the current screen
     */
    private void addListSetConstraints()
    {
        listSetConstraints = new List("SetConstraints", List.IMPLICIT, listSetConstraintsStr, null);

        listSetConstraints.addCommand(cmdBack);
        listSetConstraints.addCommand(cmdSet);
        listSetConstraints.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetConstraints);
    }

    /**
     * This method creates the listSetMaxSize List and sets the
     * current screen as listSetMaxSize.
     *
     *@param cScreen the current screen
     */
    private void addListSetMaxSize()
    {
        listSetMaxSize = new List("SetMaxSize", List.IMPLICIT, listSetMaxSizeStr, null);

        listSetMaxSize.addCommand(cmdBack);
        listSetMaxSize.addCommand(cmdSet);
        listSetMaxSize.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetMaxSize);
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
            if (s == listMethods)
                Display.getDisplay(m).setCurrent(this);
            else if (s == listSetLabel || s == listSetString || s == listSetMaxSize || s == listSetConstraints
                     || s == listSetChars)
                Display.getDisplay(m).setCurrent(listMethods);
            else
                FormUpdateTests.show();
        }
        else if (c == cmdMethods)
            addListMethods();
        else if (c == cmdOk)
        {
            int index = listMethods.getSelectedIndex();
            String element = listMethods.getString(index);
            if (element.equals(listMethodsStr[0]))
                addListSetChars();
            else if (element.equals(listMethodsStr[1]))
                addListSetConstraints();
            else if (element.equals(listMethodsStr[2]))
                addListSetMaxSize();
            else if (element.equals(listMethodsStr[3]))
                addListSetString();
            else if (element.equals(listMethodsStr[4]))
            {
                int size = tf.size();
                Alert sz = new Alert("SIZE", new Integer(size).toString(), null, null);
                sz.setTimeout(7000);
                Display.getDisplay(m).setCurrent(sz);
            }
            else if (element.equals(listMethodsStr[5]))
                addListSetLabel();
            else if (element.equals(listMethodsStr[6]))
            {
                try
                {
                    tf.insert(insertedChar, 3, 12, 1);
                    Display.getDisplay(m).setCurrent(this);
                }
                catch (IllegalArgumentException  e)
                {
                    Alert a = new Alert("INFO", "TextField max size is less than the inserted chars", null, AlertType.INFO);
                    a.setTimeout(7000);
                    Display.getDisplay(m).setCurrent(a);
                }
            }
            else if (element.equals(listMethodsStr[8]))
            {
                this.delete(0);
                Display.getDisplay(m).setCurrent(this);
            }
            else if (element.equals(listMethodsStr[9]))
            {
                tf = new TextField("TextField", "This is the text.", 30, TextField.ANY);
                append(tf);
                Display.getDisplay(m).setCurrent(this);

            }
            else if (element.equals(listMethodsStr[10]))
            {
                tf = new TextField("TextField", "TextField With a three(3)lines of text, TextField With a three(3) lines of text.", 100, TextField.ANY);
                append(tf);
                Display.getDisplay(m).setCurrent(this);

            }
            else if (element.equals(listMethodsStr[11]))
            {
                tf = new TextField("TextField", "TextField \nWith\na six(6)\nlines\nof text.\nVoila!", 200, TextField.ANY);
                append(tf);
                Display.getDisplay(m).setCurrent(this);

            }
            else if (element.equals(listMethodsStr[7]))
                try
                {
                    tf.insert(insertedStr, 1);
                    Display.getDisplay(m).setCurrent(this);
                }
                catch (IllegalArgumentException  e)
                {
                    Alert a = new Alert("INFO", "TextField max size is less than the inserted string", null, AlertType.INFO);
                    a.setTimeout(7000);
                    Display.getDisplay(m).setCurrent(a);
                }

        }
        else if (c == cmdSet)
        {
            if (s == listSetString)
            {
                int index = listSetString.getSelectedIndex();
                String element = listSetString.getString(index);
                if (element.equals(listSetStringStr[0]))
                {
                    tf.setString("Nokia String set");
                }
                else if (element.equals(listSetStringStr[1]))
                {
                    tf.setString(null);
                }
            }
            if (s == listSetLabel)
            {
                int index = listSetLabel.getSelectedIndex();
                String element = listSetLabel.getString(index);
                if (element.equals(listSetLabelStr[0]))
                    tf.setLabel("TextField label which should be too long to fit on one line");
                if (element.equals(listSetLabelStr[1]))
                    tf.setLabel(null);
            }
            if (s == listSetChars)
            {
                int index = listSetChars.getSelectedIndex();
                String element = listSetChars.getString(index);
                if (element.equals(listSetCharsStr[0]))
                    tf.setChars(c1, 0, 5); //I should catch IllegalArgumentException here
                if (element.equals(listSetCharsStr[1]))
                    tf.setChars(c2, 0, 4); //I should catch IllegalArgumentException here
            }
            if (s == listSetMaxSize)
            {
                int index = listSetMaxSize.getSelectedIndex();
                String element = listSetMaxSize.getString(index);
                //get the current size of textField and increase/decrease it by 15 everytime
                int mSize = tf.getMaxSize();
                if (element.equals(listSetMaxSizeStr[0]))
                {
                    tf.setMaxSize(mSize+15);
                }
                if (element.equals(listSetMaxSizeStr[1]))
                {
                    tf.setMaxSize(mSize-15);
                }
            }
            if (s == listSetConstraints)
            {
                int index = listSetConstraints.getSelectedIndex();
                String element = listSetConstraints.getString(index);
                if (element.equals(listSetConstraintsStr[0]))
                    tf.setConstraints(TextField.ANY);
                if (element.equals(listSetConstraintsStr[1]))
                    tf.setConstraints(TextField.URL);
                if (element.equals(listSetConstraintsStr[2]))
                    tf.setConstraints(TextField.PHONENUMBER);
                if (element.equals(listSetConstraintsStr[3]))
                    tf.setConstraints(TextField.NUMERIC);
                if (element.equals(listSetConstraintsStr[4]))
                    tf.setConstraints(TextField.EMAILADDR);
                if (element.equals(listSetConstraintsStr[5]))
                    tf.setConstraints(TextField.PASSWORD);
                if (element.equals(listSetConstraintsStr[6]))
                    tf.setConstraints(TextField.PASSWORD|TextField.EMAILADDR);
                if (element.equals(listSetConstraintsStr[7]))
                    tf.setConstraints(TextField.PASSWORD|TextField.NUMERIC);
                if (element.equals(listSetConstraintsStr[8]))
                    tf.setConstraints(TextField.PASSWORD|TextField.PHONENUMBER);
                if (element.equals(listSetConstraintsStr[9]))
                    tf.setConstraints(TextField.PASSWORD|TextField.URL);
                if (element.equals(listSetConstraintsStr[10]))
                    tf.setConstraints(TextField.DECIMAL);
                if (element.equals(listSetConstraintsStr[11]))
                    tf.setConstraints(TextField.INITIAL_CAPS_SENTENCE);
                if (element.equals(listSetConstraintsStr[12]))
                    tf.setConstraints(TextField.INITIAL_CAPS_WORD);
                if (element.equals(listSetConstraintsStr[13]))
                    tf.setConstraints(TextField.UNEDITABLE);
                if (element.equals(listSetConstraintsStr[14]))
                    tf.setConstraints(TextField.NON_PREDICTIVE);
                if (element.equals(listSetConstraintsStr[15]))
                    tf.setConstraints(TextField.SENSITIVE);
            }
            Display.getDisplay(m).setCurrent(this);
        }
    }
}
