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
 * This class is to test the below ChoiceGroup test case:
 * 1.When the element label is longer than the image.
 *
 */

public class Test3 extends Form implements CommandListener
{

    //the ChoiceGroup to test
    private ChoiceGroup cg = null;

    //the command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the longLabel which is added to the ChoiceGroup "cg"
    private final String longLabel = "This is a very long element label with image." +
                                     " This is a very long element label with image.";

    /**
     * This constructor is to create and append the ChoiceGroup  "cg".
     * The ChoiceGroup contains biglabel element label longer than the bigImage.
     *
     * @param cgType The ChoiceGroup type
     */

    public Test3(int cgType)
    {

        super("Test3");
        String cgLabel = null;
        //set the ChoiceGroup label according to the type of ChoiceGroup "cg"
        if (cgType == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.POPUP)
            cgLabel = "POPUP";
        cg = new ChoiceGroup(cgLabel, cgType);
        cg.append(longLabel, Images.getInstance().smallImage);
        append(cg);

        addCommand(cmdBack);
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

    }
}


