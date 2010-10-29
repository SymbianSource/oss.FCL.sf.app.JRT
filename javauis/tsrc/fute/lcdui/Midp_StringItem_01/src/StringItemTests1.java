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

public class StringItemTests1 extends MIDlet implements CommandListener, ItemCommandListener
{

    //the form to add the StringItems
    private Form stringItemForm = null;
    //the StringItem to test
    private StringItem si = null;
    //the StringItem types
    private String[] siTypes = {"PLAIN", "HYPERLINK", "BUTTON"};

    // commands for StringItem type
    private Command cmdSI1 = new Command(siTypes[0], Command.SCREEN, 1);
    private Command cmdSI2 = new Command(siTypes[1], Command.SCREEN, 1);
    private Command cmdSI3 = new Command(siTypes[2], Command.SCREEN, 1);

    // Commands for label and text
    private Command cmd01 = new Command("Label and text", Command.SCREEN, 1);
    private Command cmd02 = new Command("Text, empty label", Command.SCREEN, 1);
    private Command cmd03 = new Command("Whitespace in label, normal text", Command.SCREEN, 1);
    private Command cmd04 = new Command("Very long text, normal label", Command.SCREEN, 1);
    private Command cmd09 = new Command("Very long label, normal text", Command.SCREEN, 1);
    // Commands for new line char tests
    private Command cmd05 = new Command("Label with 1 newline, normal text", Command.SCREEN, 1);
    private Command cmd06 = new Command("Label with 2 newline, normal text", Command.SCREEN, 1);
    private Command cmd07 = new Command("Text with 1 newline, normal label", Command.SCREEN, 1);
    private Command cmd08 = new Command("Text with 2 newline, normal label", Command.SCREEN, 1);
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
        si = new StringItem("Label", "Text");
        stringItemForm.append(si);
        stringItemForm.addCommand(cmdSI1);
        stringItemForm.addCommand(cmdSI2);
        stringItemForm.addCommand(cmdSI3);
        stringItemForm.addCommand(cmd01);
        stringItemForm.addCommand(cmd02);
        stringItemForm.addCommand(cmd03);
        stringItemForm.addCommand(cmd04);
        stringItemForm.addCommand(cmd05);
        stringItemForm.addCommand(cmd06);
        stringItemForm.addCommand(cmd07);
        stringItemForm.addCommand(cmd08);
        stringItemForm.addCommand(cmd09);
        stringItemForm.addCommand(cmdExit);
        stringItemForm.setCommandListener(this);
        Display.getDisplay(this).setCurrent(stringItemForm);
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
        if(c == cmdSI1)
        {
            // Plain type
            stringItemForm.deleteAll();
            si = new StringItem("Label", "Text");
            stringItemForm.append(si);
        }
        else if(c == cmdSI2)
        {
            // Hyperlink type
            stringItemForm.deleteAll();
            si = new StringItem("Label", "Text", Item.HYPERLINK);
            stringItemForm.append(si);
            si.addCommand(cmdItem1);
            si.addCommand(cmdItem2);
            si.setItemCommandListener(this);
        }
        else if(c == cmdSI3)
        {
            // Button type
            stringItemForm.deleteAll();
            si = new StringItem("Label", "Text", Item.BUTTON);
            stringItemForm.append(si);
            si.addCommand(cmdItem1);
            si.addCommand(cmdItem2);
            si.setItemCommandListener(this);
        }
        else if(c == cmd01)
        {
            si.setLabel("This is the label");
            si.setText("This is the text.");
        }
        else if(c == cmd02)
        {
            si.setLabel("");
            si.setText("This is the text.");
        }
        else if(c == cmd03)
        {
            si.setLabel("          ");
            si.setText("This is the text.");
        }
        else if(c == cmd04)
        {
            si.setLabel("This is the label");
            si.setText("This is a very long text. " +
                       "This is a very long text. " +
                       "This is a very long text. " +
                       "This is a very long text. " +
                       "This is a very long text. " +
                       "This is a very long text. " +
                       "This is a very long text.");
        }
        else if(c == cmd05)
        {
            si.setLabel("Label with 1\nnewline char");
            si.setText("This is the text.");
        }
        else if(c == cmd06)
        {
            si.setLabel("Label with 2\n\nnewline chars");
            si.setText("This is the text.");
        }
        else if(c == cmd07)
        {
            si.setLabel("This is the label");
            si.setText("Text with 1\nnewline char.");
        }
        else if(c == cmd08)
        {
            si.setLabel("This is the label");
            si.setText("Text with 2\n\nnewline chars.");
        }
        else if(c == cmd09)
        {
            si.setText("This is the text.");
            si.setLabel("This is a very long label. " +
                        "This is a very long label. " +
                        "This is a very long label. " +
                        "This is a very long label. " +
                        "This is a very long label. " +
                        "This is a very long label. " +
                        "This is a very long label.");
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

