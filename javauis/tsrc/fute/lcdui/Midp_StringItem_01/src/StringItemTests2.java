/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * This is the main class for StringItemTests1 tests.
 */

public class StringItemTests2 extends MIDlet implements CommandListener, ItemCommandListener
{

    //the form to add the StringItems
    private Form stringItemForm = null;
    //the main form
    private Form mainForm = null;
    //the StringItem to test
    private StringItem si = null;
    //list for StringItem types
    private ChoiceGroup cgType = null;
    //list for labels
    private ChoiceGroup cgLabel = null;
    //list for texts
    private ChoiceGroup cgText = null;
    //the StringItem types
    private String[] siTypes = {"PLAIN", "HYPERLINK", "BUTTON"};
    private String[] siLabels = {"empty", "Short label", "This is a very long label that might not fit on the screen."};
    private String[] siStrings =
    {
        "1 This fits on one line.",
        "2 This is one short paragraph that should fit on 2 lines.",
        "3 This is one longer paragraph that might take about 3 lines in the device, depends on screen size.",
        "4 This paragraph\nhas one linefeed.",
        "5 This is a somewhat longer paragraph that might take about three lines.\nHere was a linefeed.",
        "6TheWrappingBasedOnCharacterBoundariesCanBeTestedWithThisLineOfText."
    };

    private Command cmdAdd = new Command("Add item", Command.SCREEN, 1);
    private Command cmdForward = new Command("Switch form", Command.SCREEN, 1);
    // Command to go back to main form
    private Command cmdBack = new Command("Back", Command.BACK, 1);
    // Command to clear the test form
    private Command cmdClear = new Command("Clear form", Command.SCREEN, 1);

    // Commands for the StringItems
    private Command cmdItem1 = new Command("Itemcommand1", Command.ITEM, 1);
    private Command cmdItem2 = new Command("Itemcommand2", Command.ITEM, 1);
    // Command to exit the MIDlet
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        stringItemForm = new Form("StringItem");
        mainForm = new Form("Main");
        cgType = new ChoiceGroup("Type:", ChoiceGroup.POPUP, siTypes, null);
        cgLabel = new ChoiceGroup("Label:", ChoiceGroup.POPUP, siLabels, null);
        cgText = new ChoiceGroup("Text:", ChoiceGroup.POPUP, siStrings, null);

        mainForm.append(cgType);
        mainForm.append(cgLabel);
        mainForm.append(cgText);

        mainForm.addCommand(cmdAdd);
        mainForm.addCommand(cmdForward);
        mainForm.addCommand(cmdExit);
        mainForm.setCommandListener(this);
        stringItemForm.addCommand(cmdBack);
        stringItemForm.addCommand(cmdClear);
        stringItemForm.setCommandListener(this);
        Display.getDisplay(this).setCurrent(mainForm);
    }

    /**
     *  Signals the MIDlet to terminate and enter the Destroyed state.
     *
     */
    protected void destroyApp(boolean unconditional)
    {
        System.out.println("destroyApp is called");
    }

    /**
     *  Signals the MIDlet to stop and enter the Paused state.
     */
    protected void pauseApp()
    {
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if(c == cmdAdd)
        {
            // Create correct type of stringitem
            int index = cgType.getSelectedIndex();
            if(index == 0)
                si = new StringItem("", "", Item.PLAIN);
            else
            {
                if(index == 1)
                    si = new StringItem("", "", Item.HYPERLINK);
                else if(index == 2)
                    si = new StringItem("", "", Item.BUTTON);
                si.addCommand(cmdItem1);
                si.addCommand(cmdItem2);
                si.setItemCommandListener(this);
            }

            // Set the label
            index = cgLabel.getSelectedIndex();
            if(index == 0)
                si.setLabel("");
            else
                si.setLabel(cgLabel.getString(index));
            // Set the text
            index = cgText.getSelectedIndex();
            si.setText(cgText.getString(index));
            stringItemForm.append(si);
            Display.getDisplay(this).setCurrent(stringItemForm);
        }

        else if(c == cmdBack)
        {
            Display.getDisplay(this).setCurrent(mainForm);
        }
        else if(c == cmdClear)
        {
            stringItemForm.deleteAll();
        }
        else if(c == cmdForward)
        {
            Display.getDisplay(this).setCurrent(stringItemForm);
        }
        else if(c == cmdExit)
        {
            this.notifyDestroyed();
        }
    }

    public void commandAction(Command c, Item i)
    {
        Alert alert = new Alert(c.getLabel(),"Command executed.",null, AlertType.INFO);
        Display.getDisplay(this).setCurrent(alert);
    }
}

