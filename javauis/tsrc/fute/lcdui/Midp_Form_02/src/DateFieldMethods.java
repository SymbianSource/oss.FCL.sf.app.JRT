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
import javax.microedition.lcdui.List;

/**
 * This class is to test the various methods of DateField
 *
 */

public class DateFieldMethods extends Form implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet m = null;

    //the DateField methods as elements in the listMethodsStr
    private String listMethodsStr[] = {"SetDate", "SetInputMode", "SetLabel"};

    //the Strings to set date as Current or NULL
    private String listSetDateStr[] = {"Current", "NULL"};

    //the Date modes as Strings to setMode in the DateField
    private String listSetModeStr[] = {"DATE", "Date_TIME", "TIME"};

    //the DateField labels as Strings to setLabel in DateField
    private String listSetLabelStr[] = {"Label", "NULL" };

    //instead I should use Displayable object to know about the
    //current Displayable
    private String methodsScrn = "methods";

    private String mainScrn = "main";

    private String setDateScrn = "date";

    private String setModeScrn = "mode";

    private String setLabelScrn = "label";

    private String currScreen = null;

    //the list to add listMethodsStr
    private List listMethods = null;

    //the list to add listSetDateStr
    private List listSetDate = null;

    //the list to add listSetModeStr
    private List listSetMode = null;

    //the list to add listSetLabelStr
    private List listSetLabel = null;

    //the DateField to test
    private DateField df = null;

    //the command to go back to the Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the commmand to invoke the DateField method for testing
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    //the command to display the listMethods Screen
    private Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    //the command to set the DateField
    private Command cmdSet = new Command("Set", Command.OK, 1);


    public DateFieldMethods(MIDlet m)
    {
        super(m.getClass().getName());
        this.m = m;
        df = new DateField("DateField", DateField.DATE);
        append(df);
        setCommandListener(this);
        addCommand(cmdBack);
        addCommand(cmdMethods);
        currScreen = mainScrn;
    }

    /**
     * This method creates the listMethods List and sets the
     * current screen as listMethods.
     *
     *@param cScreen the current screen
     */
    private void addListMethods(String cScreen)
    {
        listMethods = new List("Methods", List.IMPLICIT, listMethodsStr, null);

        listMethods.addCommand(cmdBack);
        listMethods.addCommand(cmdOk);
        listMethods.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listMethods);
        currScreen = cScreen;
    }

    /**
     * This method creates the listSetDate List and sets the
     * current screen as listSetDate.
     *
     *@param cScreen the current screen
     */
    private void addListSetDate(String cScreen)
    {
        listSetDate = new List("SetDate", List.IMPLICIT, listSetDateStr, null);

        listSetDate.addCommand(cmdBack);
        listSetDate.addCommand(cmdSet);
        listSetDate.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetDate);
        currScreen = cScreen;
    }

    /**
     * This method creates the listSetMode List and sets the
     * current screen as listSetMode .
     *
     *@param cScreen the current screen
     */
    private void addListSetMode(String cScreen)
    {
        listSetMode = new List("SetInputMode", List.IMPLICIT, listSetModeStr, null);

        listSetMode.addCommand(cmdBack);
        listSetMode.addCommand(cmdSet);
        listSetMode.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetMode);
        currScreen = cScreen;
    }

    /**
     * This method creates the listSetLabel List and sets the
     * current screen as listSetLabel .
     *
     *@param cScreen the current screen
     */
    private void addListSetLabel(String cScreen)
    {
        listSetLabel = new List("SetLabel", List.IMPLICIT, listSetLabelStr, null);

        listSetLabel.addCommand(cmdBack);
        listSetLabel.addCommand(cmdSet);
        listSetLabel.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetLabel);
        currScreen = cScreen;
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
            if (currScreen.equals(methodsScrn))
            {
                System.out.println("currScreen" + currScreen);
                Display.getDisplay(m).setCurrent(this);
                currScreen = mainScrn;
            }
            else if (currScreen.equals(setDateScrn) || currScreen.equals(setModeScrn)
                     || currScreen.equals(setLabelScrn))
            {
                System.out.println("currScreen" + currScreen);
                Display.getDisplay(m).setCurrent(listMethods);
                currScreen = methodsScrn;
            }
            else
            {
                FormUpdateTests.show();
            }
        }

        else if (c == cmdMethods)
        {
            addListMethods(methodsScrn);
        }
        else if (c == cmdOk || c == List.SELECT_COMMAND)
        {
            int index = listMethods.getSelectedIndex();
            String element = listMethods.getString(index);
            if (element.equals(listMethodsStr[0]))
                addListSetDate(setDateScrn);
            else if (element.equals(listMethodsStr[1]))
                addListSetMode(setModeScrn);
            else if (element.equals(listMethodsStr[2]))
                addListSetLabel(setLabelScrn);
        }
        else if (c == cmdSet)
        {
            if (s == listSetDate)
            {
                int index = listSetDate.getSelectedIndex();
                String element = listSetDate.getString(index);
                if (element.equals(listSetDateStr[0]))
                {
                    df.setDate(Calendar.getInstance(TimeZone.getDefault())
                               .getTime());
                }
                else if (element.equals(listSetDateStr[1]))
                {
                    df.setDate(null);
                }
            }
            if (s == listSetMode)
            {
                int index = listSetMode.getSelectedIndex();
                String element = listSetMode.getString(index);
                if (element.equals(listSetModeStr[0]))
                    df.setInputMode(df.DATE);
                if (element.equals(listSetModeStr[1]))
                    df.setInputMode(df.DATE_TIME);
                if (element.equals(listSetModeStr[2]))
                    df.setInputMode(df.TIME);
            }
            if (s == listSetLabel)
            {
                int index = listSetLabel.getSelectedIndex();
                String element = listSetLabel.getString(index);
                if (element.equals(listSetLabelStr[0]))
                    df.setLabel("DateField label which should be too long to fit on one line");
                if (element.equals(listSetLabelStr[1]))
                    df.setLabel(null);
            }
            Display.getDisplay(m).setCurrent(this);
            currScreen = mainScrn;
        }
    }
}
