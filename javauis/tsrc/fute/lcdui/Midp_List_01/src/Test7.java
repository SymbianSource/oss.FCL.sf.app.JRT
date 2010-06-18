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
 * This class is to test methods of the List
 *
 */

public class Test7 extends List implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet m = null;

    //list which contains list method names as elements
    private List listMethods = null;

    //elements of listMethods
    private String listMethodsStr[] = {"Append", "Delete", "Insert", "Set",
                                       "SetSelectedIndex"
                                      };
    //list append to test append
    private List listAppend = null;

    //list to test insert
    private List listInsert = null;

    //list to test set
    private List listSet = null;

    //list to test selected index
    private List listSel = null;

    //array of strings added to listAppend
    private String listAppendStr[] = {"String", "Image"};

    //array of strings added to listInsert
    private String listInsertStr[] = {"without Image", "with Image"};

    //array of strings added to listSet
    private String listSetStr[] = {"without Image", "with Image"};

    //array of strings added to listSel
    private String listSelStr[] = {"Select", "UnSelect"};

    //the initial elements in this list
    private String first = "1stelement";
    private String second = "2ndelement";
    private String third = "3rdelement";

    //an image which fits in the layout
    private Image smallImage = null;

    //Command back to go to the previous screens
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //Command to popup the list methods
    Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    //Ok command
    Command cmdOk = new Command("Ok", Command.OK, 1);

    //append command
    Command cmdAppend = new Command("Append", Command.SCREEN, 1);

    //insert command
    Command cmdInsert = new Command("Insert", Command.SCREEN, 1);

    //set Command
    Command cmdSet = new Command("Set", Command.SCREEN, 1);

    //Select Command
    Command cmdSel = new Command("Select", Command.SCREEN, 1);

    /**
     *
     * @param listType The List type
     */
    public Test7(int listType)
    {

        super("List", listType);
        //append 3 elements to this list
        append(first, null);
        append(second, null);
        append(third, null);

        //add Commands
        addCommand(cmdBack);
        addCommand(cmdMethods);

        setCommandListener(this);

        m = ListTests.getParent();
    }
    /**
     * This method will create the listMethods with all the methods to test
     *

     */
    private void appendListMethods()
    {
        listMethods = new List("ChoiceGroup Methods", List.EXCLUSIVE, listMethodsStr, null);
        listMethods.addCommand(cmdBack);
        listMethods.addCommand(cmdOk);
        listMethods.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listMethods);
    }

    private void appendListMethod()
    {
        listAppend = new List("Append", List.EXCLUSIVE, listAppendStr, null);
        listAppend.addCommand(cmdBack);
        listAppend.addCommand(cmdAppend);
        listAppend.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listAppend);
    }

    private void insertListMethod()
    {
        listInsert = new List("Insert", List.EXCLUSIVE, listInsertStr, null);
        listInsert.addCommand(cmdBack);
        listInsert.addCommand(cmdInsert);
        listInsert.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listInsert);
    }

    private void setListMethod()
    {
        listSet = new List("Set", List.EXCLUSIVE, listInsertStr, null);
        listSet.addCommand(cmdBack);
        listSet.addCommand(cmdSet);
        listSet.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSet);
    }

    private void setSelIndexListMethod()
    {
        listSel = new List("SelectedIndex", List.EXCLUSIVE, listSelStr, null);
        listSel.addCommand(cmdBack);
        listSel.addCommand(cmdSel);
        listSel.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSel);
    }

    //create image
    private Image createImage()
    {
        try
        {
            smallImage = Image.createImage("/Icon.png");
        }
        catch (java.io.IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
        return smallImage;
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
            if (s == listAppend || s == listInsert || s == listSet || s == listSel)
                Display.getDisplay(m).setCurrent(listMethods);
            else if (s == this)
                ListTests.show();
            else if (s == listMethods)
                Display.getDisplay(m).setCurrent(this);
            else
                ListTests.show();
        }
        else if (c == cmdMethods)
            appendListMethods();
        else if (c == cmdOk)
        {
            int index = listMethods.getSelectedIndex();
            String str = listMethods.getString(index);
            if (str.equals(listMethodsStr[0]))
            {
                appendListMethod();
            }
            else if (str.equals(listMethodsStr[1]))
            {
                if (this.size() != 0)
                {
                    if (ListTests.type == List.MULTIPLE)
                        //For Multiple List delete the 1st element
                        this.delete(0);
                    else
                        this.delete(this.getSelectedIndex());
                }
                Display.getDisplay(m).setCurrent(this);
            }
            else if (str.equals(listMethodsStr[2]))
            {
                insertListMethod();
            }
            else if (str.equals(listMethodsStr[3]))
            {
                setListMethod();
            }
            else if (str.equals(listMethodsStr[4]))
            {
                setSelIndexListMethod();
            }
        }
        else if (c == cmdAppend)
        {
            int i = listAppend.getSelectedIndex();
            String element = listAppend.getString(i);
            if (element.equals(listAppendStr[0]))
            {
                this.append("Appended", null);
                Display.getDisplay(m).setCurrent(this);
            }
            if (element.equals(listAppendStr[1]))
            {
                Image img = createImage();
                this.append("ImageAppended", img);
                Display.getDisplay(m).setCurrent(this);
            }
        }
        else if (c == cmdInsert)
        {
            int i = listInsert.getSelectedIndex();
            String e = listInsert.getString(i);
            if (e.equals(listInsertStr[0]))
            {
                this.insert(0, "inserted", null);
                Display.getDisplay(m).setCurrent(this);
            }
            else
            {
                Image img = createImage();
                this.insert(0, "ImageInserted", img);
                Display.getDisplay(m).setCurrent(this);
            }
        }
        else if (c == cmdSet)
        {
            int i = listSet.getSelectedIndex();
            String e = listSet.getString(i);
            if (e.equals(listSetStr[0]))
            {
                this.set(0, "set", null);
                Display.getDisplay(m).setCurrent(this);
            }
            else
            {
                Image img = createImage();
                this.set(0, "ImageSet", img);
                Display.getDisplay(m).setCurrent(this);
            }
        }
        else if (c == cmdSel)
        {
            int i = listSel.getSelectedIndex();
            String e = listSel.getString(i);
            if (e.equals(listSelStr[0]))
            {
                this.setSelectedIndex(0, true);
                Display.getDisplay(m).setCurrent(this);
            }
            else
            {
                this.setSelectedIndex(0, false);
                Display.getDisplay(m).setCurrent(this);
            }
        }
    }
}


