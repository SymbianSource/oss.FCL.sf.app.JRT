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
 * This class is to test the List element selection when the element is
 * deleted, inserted and replaced.
 *
 */

public class Test1 extends List implements CommandListener
{

    private MIDlet m = null;
    //The Command to go back to the ListTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    Command cmdAdd = new Command("Add", Command.SCREEN, 1);
    Command cmdInsert = new Command("Insert", Command.SCREEN, 1);
    Command cmdSet = new Command("Set", Command.SCREEN, 1);
    Command cmdDelete = new Command("Delete", Command.SCREEN, 1);
    Command cmdDeleteAll = new Command("Delete All", Command.SCREEN, 1);

    //the List elements
    private String elements[] = {"1st Element", "2nd Element", "3rd Element", "4th Element",
                                 "5 Element", "6th Element", "7th Element", "8th Element"
                                };

    /**
    * This constructor adds elements to this List and adds Commands to
    * add, insert, set and delte elements from the List.
    * Displays the current screen to be this List
    *
    * @param cgType The ChoiceGroup type
    */
    public Test1(int cgType)
    {

        super(null, cgType);
        String cgLabel = null;
        //set the List title according to the type of this List
        if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.IMPLICIT)
            cgLabel = "IMPLICIT";
        this.setTitle(cgLabel);
        for (int i=0; i<elements.length; i++)
            this.append(elements[i], null);

        //add commands
        addCommand(cmdInsert);
        addCommand(cmdSet);
        addCommand(cmdDelete);
        addCommand(cmdAdd);
        addCommand(cmdDeleteAll);
        addCommand(cmdBack);
        setCommandListener(this);
        m = ListTests.getParent();
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        int index = this.getSelectedIndex();
        if (c == cmdBack)
            ListTests.show();
        if (c == cmdAdd)
            this.append("Added", Images.getInstance().smallImage);
        else if (c==cmdDeleteAll)
            this.deleteAll();
        try
        {
            if (c == cmdInsert)
                this.insert(index, "Inserted", Images.getInstance().smallImage);
            else if (c==cmdSet)
                this.set(index, "Replaced", Images.getInstance().smallImage);
            else if (c==cmdDelete)
                this.delete(index);
        }
        catch (IndexOutOfBoundsException iobe)
        {
            Alert a = new Alert("IOB Exception", "List is empty", null, AlertType.INFO);
            Display.getDisplay(m).setCurrent(a);
        }
    }
}


