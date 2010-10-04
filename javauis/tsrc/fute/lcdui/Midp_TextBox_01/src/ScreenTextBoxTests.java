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
 * This class is to test the below TextBox test cases:
 * 1.Layout
 * 2.TextBox constraints and T9 support
 *
 */

public class ScreenTextBoxTests extends Form implements CommandListener
{

    //parent MIDlet
    private static Midp_TextBox_01 parent = null;

    //the instance of this class
    private static ScreenTextBoxTests instance = null;

    //TextBox1 for testing test1
    private TextBox tb1 = null;

    //TextBox2 for testing test2
    private TextBox tb2 = null;

    //TextBox3 for testing test3
    private TextBox tb3 = null;

    //Form for testing test3 which contains textFields title and maxSize
    private Form formTest3 = null;

    //TextField to set TextBox title for test3
    private TextField titleT = null;

    //TextField to set TextBox maxSize for test3
    private TextField maxSizeT = null;

    //List which contains TextField constraints name as elements to test Test2
    private List listConstraints = null;

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

    //command for test3
    private Command cmdGetMaxSize = new Command("getMaxSize", Command.SCREEN, 1);

    //command to show tb3
    private Command cmdShowTextBox3 = new Command("ShowTextBox3", Command.SCREEN, 1);

    //Command for test1
    private Command cmdTest1 = new Command("Test1", Command.SCREEN, 1);

    //Command for test2
    private Command cmdTest2 = new Command("Test2", Command.SCREEN, 1);

    //Command for test3
    private Command cmdTest3 = new Command("Test3", Command.SCREEN, 1);

    //Command to set title to TextBox1
    private Command cmdNullT = new Command("Null Title", Command.OK, 1);

    //Command to set title to TextBox1
    private Command cmdEmptyT = new Command("Empty Title", Command.SCREEN, 1);

    //Command to set title to TextBox1
    private Command cmdBigT = new Command("Big Title", Command.SCREEN, 1);

    //Command to go Back to the previous Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //Command to Ok whic is added to the listConstraints
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    //maximum size which is specified while creating TextBox
    private final int MAX_SIZE = 100;

    //Commands for the item specific options menu.
    private Command cmdScreen1 = new Command("1 item cmd", Command.SCREEN, 1);
    private Command cmdScreen2 = new Command("1 ok cmd", Command.SCREEN, 1);
    private Command cmdScreen3 = new Command("1 item 1 ok cmd", Command.SCREEN, 1);
    private Command cmdScreen4 = new Command("0 item 0 ok cmd", Command.SCREEN, 1);
    private Command cmdItemC = new Command("Item command", Command.ITEM, 1);
    private Command cmdOkC = new Command("Ok command", Command.OK, 1);

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public ScreenTextBoxTests(Midp_TextBox_01 parent)
    {

        //set the title of the form as the name of the MIDlet
        super(parent.getClass().getName());
        this.parent = parent;

        if (instance == null)
        {
            instance = this;
        }
        //add test commands
        instance.addCommand(cmdTest1);
        instance.addCommand(cmdTest2);
        instance.addCommand(cmdTest3);
        instance.addCommand(cmdExit);

        //create TextBox with constraints ANY
        tb3 = new TextBox("ANY", null, MAX_SIZE, TextField.ANY);

        //add cmdBack to tb2
        tb3.addCommand(cmdBack);
        tb3.setCommandListener(this);
        setCommandListener(this);
        show();
    }

    /**
     * creates and sets the current screen to be tb1
     */
    private void addTextBoxTest1()
    {
        //create TextBox
        tb1 = new TextBox("ANY", null, MAX_SIZE, TextField.ANY);
        //add commands to TextBox1
        Ticker t = new Ticker("Test1");
        tb1.setTicker(t);
        tb1.addCommand(cmdNullT);
        tb1.addCommand(cmdEmptyT);
        tb1.addCommand(cmdBigT);
        tb1.addCommand(cmdBack);
        tb1.setCommandListener(this);
        Display.getDisplay(parent).setCurrent(tb1);
    }

    /**
     * creates and sets the current screen to be tb2
     */
    private void addTextBoxTest2(String title, int constraints)
    {
        //create TextBox with constraints ANY
        tb2 = new TextBox(title, null, MAX_SIZE, constraints);
        //add cmdBack to tb2
        tb2.addCommand(cmdBack);
        tb2.setCommandListener(this);
        Display.getDisplay(parent).setCurrent(tb2);
    }
    private void addUneditableTextBoxTest2(String title, int constraints)
    {
        //create TextBox with constraints ANY
        tb2 = new TextBox(title, "Uneditable", MAX_SIZE, constraints);
        //add cmdBack to tb2
        tb2.addCommand(cmdBack);
        tb2.setCommandListener(this);
        Display.getDisplay(parent).setCurrent(tb2);
    }

    private void addNumericUneditableTextBoxTest2(String title, int constraints)
    {
        //create TextBox with constraints ANY
        tb2 = new TextBox(title, "123", MAX_SIZE, constraints);
        //add cmdBack to tb2
        tb2.addCommand(cmdBack);
        tb2.setCommandListener(this);
        Display.getDisplay(parent).setCurrent(tb2);
    }
    /**
     * Method to create and set the current screen
     * to be formTest3
     */
    private void addFormTest3()
    {
        //create form with title
        formTest3 = new Form("Test3");
        titleT = new TextField("Title", "ANY", 50, TextField.ANY);
        maxSizeT = new TextField("MaxSize", null, 6, TextField.NUMERIC);
        formTest3.append(titleT);
        formTest3.append(maxSizeT);
        formTest3.addCommand(cmdBack);
        formTest3.addCommand(cmdGetMaxSize);
        formTest3.addCommand(cmdShowTextBox3);
        formTest3.setCommandListener(this);
        Display.getDisplay(parent).setCurrent(formTest3);
    }

    /**
     * create the listConstraints with elements.
     * This is called when Test2 command is pressed
     */
    private void addListConstraints()
    {
        listConstraints = new List("ListConstraints", List.IMPLICIT, listElements, null);
        listConstraints.setCommandListener(this);
        listConstraints.addCommand(cmdBack);
        listConstraints.addCommand(cmdOk);
        Display.getDisplay(parent).setCurrent(listConstraints);
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
        if (c == cmdTest1)
        {
            addTextBoxTest1();
            Display.getDisplay(parent).setCurrent(tb1);
        }
        else if (c == cmdNullT)
        {
            tb1.setTitle(null);
            Display.getDisplay(parent).setCurrent(tb1);
        }
        else if (c == cmdEmptyT)
        {
            tb1.setTitle(" ");
            Display.getDisplay(parent).setCurrent(tb1);
        }
        else if (c == cmdBigT)
        {
            tb1.setTitle("The title of this TextBox is too long to fit in to the" +
                         " reserved space");
            Display.getDisplay(parent).setCurrent(tb1);
        }
        else if (c == cmdBack)
        {
            if (s == tb1 || s == listConstraints || s == formTest3)
                Display.getDisplay(parent).setCurrent(instance);
            else if (s == tb2)
                Display.getDisplay(parent).setCurrent(listConstraints);
            else if (s == tb3)
                Display.getDisplay(parent).setCurrent(formTest3);
        }
        else if (c == cmdTest2)
            addListConstraints();
        else if (c == cmdOk)
        {
            int index = listConstraints.getSelectedIndex();
            String element = listConstraints.getString(index);
            if (element.equals(listElements[0]))
                addTextBoxTest2("ANY", TextField.ANY);
            else if (element.equals(listElements[1]))
                addTextBoxTest2("URL", TextField.URL);
            else if (element.equals(listElements[2]))
                addTextBoxTest2("PHONENUMBER", TextField.PHONENUMBER);
            else if (element.equals(listElements[3]))
                addTextBoxTest2("NUMERIC", TextField.NUMERIC);
            else if (element.equals(listElements[4]))
                addTextBoxTest2("EMAILADDR", TextField.EMAILADDR);
            else if (element.equals(listElements[5]))
                addTextBoxTest2("PASSWORD", TextField.PASSWORD);
            else if (element.equals(listElements[6]))
                addTextBoxTest2("PWD|EMAILADDR", TextField.PASSWORD|TextField.EMAILADDR);
            else if (element.equals(listElements[7]))
                addTextBoxTest2("PWD|NUMERIC", TextField.PASSWORD|TextField.NUMERIC);
            else if (element.equals(listElements[8]))
                addTextBoxTest2("PWD|PNO", TextField.PASSWORD|TextField.PHONENUMBER);
            else if (element.equals(listElements[9]))
                addTextBoxTest2("PWD|URL", TextField.PASSWORD|TextField.URL);
            else if (element.equals(listElements[10]))
                addTextBoxTest2("DECIMAL", TextField.DECIMAL);
            else if (element.equals(listElements[11]))
                addTextBoxTest2("INITIAL_CAPS_SENTENCE", TextField.INITIAL_CAPS_SENTENCE);
            else if (element.equals(listElements[12]))
                addTextBoxTest2("INITIAL_CAPS_WORD", TextField.INITIAL_CAPS_WORD);
            else if (element.equals(listElements[13]))
                addUneditableTextBoxTest2("UNEDITABLE", TextField.UNEDITABLE);
            else if (element.equals(listElements[14]))
                addTextBoxTest2("NON_PREDICTIVE", TextField.NON_PREDICTIVE);
            else if (element.equals(listElements[15]))
                addTextBoxTest2("SENSITIVE", TextField.SENSITIVE);

            else if (element.equals(listElements[16]))
                addTextBoxTest2("DECIMAL|PWD", TextField.DECIMAL|TextField.PASSWORD);
            else if (element.equals(listElements[17]))
                addNumericUneditableTextBoxTest2("NUMERIC|UNEDITABLE", TextField.NUMERIC|TextField.UNEDITABLE);
            else if (element.equals(listElements[18]))
                addNumericUneditableTextBoxTest2("DECIMAL|UNEDITABLE", TextField.DECIMAL|TextField.UNEDITABLE);
            else if (element.equals(listElements[19]))
                addUneditableTextBoxTest2("URL|UNEDITABLE", TextField.URL|TextField.UNEDITABLE);
            else if (element.equals(listElements[20]))
                addUneditableTextBoxTest2("EMAILADDR|UNEDITABLE", TextField.EMAILADDR|TextField.UNEDITABLE);
            else if (element.equals(listElements[21]))
                addNumericUneditableTextBoxTest2("PHONENUMBER|UNEDITABLE", TextField.PHONENUMBER|TextField.UNEDITABLE);

            else if (element.equals(listElements[22]))
                addUneditableTextBoxTest2("ANY|PWD|UNEDITABLE", TextField.ANY|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[23]))
                addNumericUneditableTextBoxTest2("NUMERIC|PWD|UNEDITABLE", TextField.NUMERIC|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[24]))
                addNumericUneditableTextBoxTest2("DECIMAL|PWD|UNEDITABLE", TextField.DECIMAL|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[25]))
                addUneditableTextBoxTest2("URL|PWD|UNEDITABLE", TextField.URL|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[26]))
                addUneditableTextBoxTest2("EMAILADDR|PWD|UNDEDITABLE", TextField.EMAILADDR|TextField.PASSWORD|TextField.UNEDITABLE);
            else if (element.equals(listElements[27]))
                addNumericUneditableTextBoxTest2("PNO|PWD|UNDEDITABLE", TextField.PHONENUMBER|TextField.PASSWORD|TextField.UNEDITABLE);

            tb2.addCommand(cmdScreen1);
            tb2.addCommand(cmdScreen2);
            tb2.addCommand(cmdScreen3);
            tb2.addCommand(cmdScreen4);
            tb2.setCommandListener(this);
        }
        else if (c == cmdTest3)
            addFormTest3();
        else if (c == cmdGetMaxSize)
        {
            String sz = maxSizeT.getString();
            if (sz.length() != 0)
                tb3.setMaxSize(Integer.parseInt(sz));
            int mSize = tb3.getMaxSize();
            Alert a = new Alert("Max Size", new Integer(mSize).toString(),
                                null, AlertType.INFO);
            a.setTimeout(Alert.FOREVER);
            Display.getDisplay(parent).setCurrent(a);
        }
        else if (c == cmdShowTextBox3)
        {
            tb3.setTitle(titleT.getString());
            Display.getDisplay(parent).setCurrent(tb3);
        }
        else if (c == cmdScreen1)
        {
            tb2.removeCommand(cmdOkC);
            tb2.addCommand(cmdItemC);
            tb2.setTitle("TextBox");
        }
        else if (c == cmdScreen2)
        {
            tb2.removeCommand(cmdItemC);
            tb2.addCommand(cmdOkC);
            tb2.setTitle("TextBox");
        }
        else if (c == cmdScreen3)
        {
            tb2.addCommand(cmdItemC);
            tb2.addCommand(cmdOkC);
            tb2.setTitle("TextBox");
        }
        else if (c == cmdScreen4)
        {
            tb2.removeCommand(cmdItemC);
            tb2.removeCommand(cmdOkC);
            tb2.setTitle("TextBox");
        }
        else if (c == cmdItemC)
        {
            tb2.setTitle("Item command executed.");
        }
        else if (c == cmdOkC)
        {
            tb2.setTitle("Ok command executed.");
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method

}
