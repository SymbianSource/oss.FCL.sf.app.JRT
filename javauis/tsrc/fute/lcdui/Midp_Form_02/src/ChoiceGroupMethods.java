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
 * This class is to test the various methods of ChoiceGroup
 *
 */

public class ChoiceGroupMethods extends Form implements CommandListener
{

    private MIDlet m = null;

    private ChoiceGroup cg = null;

    private List listMethods = null;

    private String listMethodsStr[] = {"Append", "Delete", "Insert", "Set",
                                       "SetSelectedIndex", "DeleteAll"
                                      };

    private List listAppend = null;

    private List listInsert = null;

    private List listSet = null;

    private List listSel = null;

    private String listAppendStr[] = {"String", "Image"};

    private String listInsertStr[] = {"with out Image", "with Image"};

    private String listSetStr[] = {"with out Image", "with Image"};

    private String listSelStr[] = {"Select", "UnSelect"};

    private String elements[] = {"1st element", "2nd element", "3rd element"};

    private String currScreen = null;

    private Image smallImage = null;

    private Command cmdBack = new Command("Back", Command.BACK, 1);

    private Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    private Command cmdOk = new Command("Ok", Command.OK, 1);

    private Command cmdAppend = new Command("Append", Command.OK, 1);

    private Command cmdSet = new Command("Set", Command.OK, 1);

    private Command cmdSel = new Command("Select", Command.OK, 1);

    //insert command
    Command cmdInsert = new Command("Insert", Command.SCREEN, 1);

    private int type = 0;

    /**
     *
     * @param cgType The ChoiceGroup type
     */
    public ChoiceGroupMethods(int cgType)
    {

        super("ChoiceGroup");
        this.type = cgType;
        String cgLabel = null;
        if (type == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (type == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (type == ChoiceGroup.POPUP)
            cgLabel = "POPUP";
        cg = new ChoiceGroup(cgLabel, type, elements, null);
        append(cg);
        addCommand(cmdBack);
        addCommand(cmdMethods);
        setCommandListener(this);
        m = FormUpdateTests.getParent();
        currScreen = "choicegroup";
    }

    private void appendListMethods(String scrn)
    {
        listMethods = new List("ChoiceGroup Methods", List.EXCLUSIVE, listMethodsStr, null);
        listMethods.addCommand(cmdBack);
        listMethods.addCommand(cmdOk);
        listMethods.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listMethods);
        currScreen = scrn;
    }

    private void appendListMethod(String scrn)
    {
        listAppend = new List("Append", List.EXCLUSIVE, listAppendStr, null);
        listAppend.addCommand(cmdBack);
        listAppend.addCommand(cmdAppend);
        listAppend.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listAppend);
        currScreen = scrn;
    }

    private void insertListMethod(String scrn)
    {
        listInsert = new List("Insert", List.EXCLUSIVE, listInsertStr, null);
        listInsert.addCommand(cmdBack);
        listInsert.addCommand(cmdInsert);
        listInsert.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listInsert);
        currScreen = scrn;
    }

    private void setListMethod(String scrn)
    {
        listSet = new List("Set", List.EXCLUSIVE, listInsertStr, null);
        listSet.addCommand(cmdBack);
        listSet.addCommand(cmdSet);
        listSet.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSet);
        currScreen = scrn;
    }

    private void setSelIndexListMethod(String scrn)
    {
        listSel = new List("SelectedIndex", List.EXCLUSIVE, listSelStr, null);
        listSel.addCommand(cmdBack);
        listSel.addCommand(cmdSel);
        listSel.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSel);
        currScreen = scrn;
    }

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
            if (currScreen.equals("append"))
            {
                Display.getDisplay(m).setCurrent(listMethods);
                currScreen = "main";
            }
            else if (currScreen.equals("choicegroup"))
            {
                FormUpdateTests.show();
            }
            else if (currScreen.equals("main"))
            {
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
            else if (currScreen.equals("insert"))
            {
                Display.getDisplay(m).setCurrent(listMethods);
                currScreen = "main";
            }
            else if (currScreen.equals("set"))
            {
                Display.getDisplay(m).setCurrent(listMethods);
                currScreen = "main";
            }
            else if (currScreen.equals("select"))
            {
                Display.getDisplay(m).setCurrent(listMethods);
                currScreen = "main";
            }
            else
            {
                FormUpdateTests.show();
            }
        }
        else if (c == cmdMethods)
            appendListMethods("main");
        else if (c == cmdOk)
        {
            int index = listMethods.getSelectedIndex();
            String str = listMethods.getString(index);
            if (str.equals(listMethodsStr[0]))
            {
                appendListMethod("append");
            }
            else if (str.equals(listMethodsStr[1]))
            {
                if (cg.size() != 0)
                {
                    if (type == List.MULTIPLE)
                        //For Multiple List delete the 1st element
                        cg.delete(0);
                    else
                        cg.delete(cg.getSelectedIndex());
                }
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
            else if (str.equals(listMethodsStr[2]))
            {
                insertListMethod("insert");
            }
            else if (str.equals(listMethodsStr[3]))
            {
                setListMethod("set");
            }
            else if (str.equals(listMethodsStr[4]))
            {
                setSelIndexListMethod("select");
            }
            else if (str.equals(listMethodsStr[5]))
            {
                if (cg.size() != 0)
                    cg.deleteAll();
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
        }
        else if (c == cmdAppend)
        {
            if (currScreen.equals("append"))
            {
                int i = listAppend.getSelectedIndex();
                String element = listAppend.getString(i);
                if (element.equals(listAppendStr[0]))
                {
                    cg.append("Appended", null);
                    Display.getDisplay(m).setCurrent(this);
                    currScreen = "choicegroup";
                }
                if (element.equals(listAppendStr[1]))
                {
                    Image img = createImage();
                    cg.append("ImageAppended", img);
                    Display.getDisplay(m).setCurrent(this);
                    currScreen = "choicegroup";
                }
            }
        }
        else if (c == cmdInsert)
        {
            if (currScreen.equals("insert"))
            {
                int i = listInsert.getSelectedIndex();
                String e = listInsert.getString(i);
                if (e.equals(listInsertStr[0]))
                {
                    cg.insert(0, "inserted", null);
                    Display.getDisplay(m).setCurrent(this);
                    currScreen = "choicegroup";
                }
                else if (e.equals(listInsertStr[1]))
                {
                    Image img = createImage();
                    cg.insert(0, "ImageInserted", img);
                    Display.getDisplay(m).setCurrent(this);
                    currScreen = "choicegroup";
                }
            }
        }
        else if (c == cmdSet)
        {
            int i = listSet.getSelectedIndex();
            String e = listSet.getString(i);
            if (e.equals(listSetStr[0]))
            {
                cg.set(0, "set", null);
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
            else
            {
                Image img = createImage();
                cg.set(0, "ImageSet", img);
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
        }
        else if (c == cmdSel)
        {
            int i = listSel.getSelectedIndex();
            String e = listSel.getString(i);
            if (e.equals(listSelStr[0]))
            {
                cg.setSelectedIndex(0, true);
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
            else
            {
                cg.setSelectedIndex(0, false);
                Display.getDisplay(m).setCurrent(this);
                currScreen = "choicegroup";
            }
        }
    }
}


