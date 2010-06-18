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
 * This class is to test the below ChoiceGroup test cases:
 * 1.ChoiceGroup has element with no associated image.
 * 2.ChoiceGroup has too long element label.
 * 3.Adding as many elements with same image
 * 4.ChoiceGroup is empty.
 */

public class Test1 extends Form implements CommandListener
{

    //The ChoiceGroup "cg" to test
    private ChoiceGroup cg = null;

    //The Command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //invoke to add image element to the ChoiceGroup "cg"
    Command cmdAdd = new Command("Add image", Command.SCREEN, 1);

    //invokes to remeove all elements in the ChoiceGroup "cg"
    Command cmdRemove = new Command("Remove All", Command.SCREEN, 1);

    //invokes to remeove all elements in the ChoiceGroup "cg"
    Command cmdSize = new Command("Shown size", Command.SCREEN, 1);

    //the longLabel which is added to ChoiceGroup "cg"
    private final String longLabel = "This is a very long element label, it should be truncated" +
                                     " with an ellipsis. This is a very long element label, it" +
                                     " should be truncated with an ellipsis.";


    //the ChoiceGroup "cg" elements
    private String elements[] = {longLabel, "2nd Element", "3rd Element", "4th Element", "5 Element"};

    /**
     * This constructor creates ChoiceGroup "cg" with the elements inside and adds Commands to
     * add and remove elements from the cg.The cg is appended to this form.
     * Displays the current screen to be this form
     *
     * @param cgType The ChoiceGroup type
     */
    public Test1(int cgType)
    {
        super("Test1");
        String cgLabel = null;
        //set the ChoiceGroup label according to the type of ChoiceGroup "cg"
        if (cgType == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.POPUP)
            cgLabel = "POPUP";

        cg = new ChoiceGroup(cgLabel, cgType, elements, null);
        append(cg);

        //add commands
        addCommand(cmdBack);
        addCommand(cmdAdd);
        addCommand(cmdRemove);
        addCommand(cmdSize);

        setCommandListener(this);
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
            ChoiceGroupTests.show();
        else if (c == cmdAdd)
            //insert element at the 0 position
            cg.insert(0, "Inserted1", Images.getInstance().smallImage);
        else if ((c == cmdRemove) && (cg.size() != 0))
            cg.deleteAll();
        else if (c == cmdSize)
        {
            //insert element at the 0 position
            String size = new String("");
            size = "Font Size: "+ cg.getFont(0).getSize();
            cg.insert(0, size, null);
        }
    }
}


