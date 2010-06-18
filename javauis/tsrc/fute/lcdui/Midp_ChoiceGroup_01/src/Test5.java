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

import java.io.*;
/**
 * This class is to test the below ChoiceGroup test cases:
 * 1.When it has all elements with an associated image.
 * 2.When the element images are removed from the list.
 *
 */

public class Test5 extends Form implements CommandListener
{

    //the ChoiceGroup to test
    private ChoiceGroup cg = null;

    //the command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the command to remove all the associated images of the elements
    Command cmdRemove = new Command("Remove All Images", Command.SCREEN, 1);

    //the elements in the ChoiceGroup "cg"
    String str[] =
    {
        "1stElement", "2ndElement", "3rdElement", "4thElement",
        "5thElement", "6thElement", "7thElement", "8thElement",
        "9thElement", "10thElement"
    };

    /**
    * This constructor creates ChoiceGroup with all elements(with images)inside and
    * adds a command to remove all the images.The ChoiceGroup is appended to this Form.
    * Displays the current screen to be this Form.
    *
    * @param cgType The ChoiceGroup type
    */
    public Test5(int cgType)
    {
        super("Test5");
        String cgLabel = null;
        //set the ChoiceGroup label according to the type of ChoiceGroup "cg"
        if (cgType == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.POPUP)
            cgLabel = "POPUP";
        setCommandListener(this);
        addCommand(cmdBack);
        addCommand(cmdRemove);

        cg = new ChoiceGroup(cgLabel, cgType);
        append(cg);

        //append all elements with associated image
        addElements();
    }

    /**
     * Adds all elements with associated image
     * to the ChoiceGroup "cg"
     */
    private void addElements()
    {
        for (int i= 0; i< 10; i++)
            cg.append(str[i], Images.getInstance().smallImage);
    } //end of method

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
        if (c == cmdRemove)
        {
            int size = cg.size();
            for (int i=0; i<size; i++)
            {
                cg.set(i, cg.getString(i), null);   //sets the image to be null
            }
        }
    }
}


