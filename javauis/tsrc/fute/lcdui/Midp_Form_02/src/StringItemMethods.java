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
 * This class is to test the various methods of StringItem
 *
 */

public class StringItemMethods extends Form implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet m = null;

    //the StringItem methods as elements in the listMethodsStr
    private String listMethodsStr[] = {"SetLabel", "SetText"};

    //the StringItem labels as Strings to setLabel in StringItem
    private String listSetLabelStr[] = {"Label", "NULL"};

    //the StringItem text as Strings to setText in StringItem
    private String listSetTextStr[] = {"Text", "NULL"};

    //the list to add listMethodsStr
    private List listMethods = null;

    //the list to add listSetLabelStr
    private List listSetLabel = null;

    //the list to add listSetTextStr
    private List listSetText = null;

    //the StringItem to test
    private StringItem si = null;

    //the command to go back to the Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the commmand to invoke the StringItem method for testing
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    //the command to display the listMethods Screen
    private Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    //the command to set the StringItem
    private Command cmdSet = new Command("Set", Command.OK, 1);


    public StringItemMethods(MIDlet m)
    {

        super(m.getClass().getName());
        this.m = m;
        si = new StringItem("StringItem Label", "StringItem text");
        append(si);
        addCommand(cmdBack);
        addCommand(cmdMethods);
        setCommandListener(this);
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
     * This method creates the listSetText List and sets the
     * current screen as listSetText.
     *
      */
    private void addListSetText()
    {
        listSetText = new List("SetText", List.IMPLICIT, listSetTextStr, null);
        listSetText.addCommand(cmdBack);
        listSetText.addCommand(cmdSet);
        listSetText.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetText);
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
            else if (s == listSetLabel || s == listSetText)
                Display.getDisplay(m).setCurrent(listMethods);
            else
                FormUpdateTests.show();
        }
        else if (c == cmdMethods)
            addListMethods();
        else if (c == cmdOk || c == List.SELECT_COMMAND)
        {
            int index = listMethods.getSelectedIndex();
            String element = listMethods.getString(index);
            if (element.equals(listMethodsStr[0]))
                addListSetLabel();
            else if (element.equals(listMethodsStr[1]))
                addListSetText();
        }
        else if (c == cmdSet)
        {
            if (s == listSetText)
            {
                int index = listSetText.getSelectedIndex();
                String element = listSetText.getString(index);
                if (element.equals(listSetTextStr[0]))
                    si.setText("StringItem text is set");
                else if (element.equals(listSetTextStr[1]))
                    si.setText(null);
            }
            if (s == listSetLabel)
            {
                int index = listSetLabel.getSelectedIndex();
                String element = listSetLabel.getString(index);
                if (element.equals(listSetLabelStr[0]))
                    si.setLabel("StringItem label which should be too long to fit on one line");
                if (element.equals(listSetLabelStr[1]))
                    si.setLabel(null);
            }
            Display.getDisplay(m).setCurrent(this);
        }
    }
}
