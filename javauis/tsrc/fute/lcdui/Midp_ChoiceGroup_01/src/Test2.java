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
 * 1.When the ChoiceGroup element image is larger than the layout it fits
 *
 */

public class Test2 extends Form implements CommandListener
{

    //the ChoiceGroup to test
    private ChoiceGroup cg = null;

    //the command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    /**
     * This constructor gets the bigImage from the Images class and appends to this
     * form.
     *
     * @param cgType The ChoiceGroup type
     */
    public Test2(int cgType)
    {
        super("Test2");
        String cgLabel = null;
        //set the ChoiceGroup label according to the type of ChoiceGroup "cg"
        if (cgType == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.POPUP)
            cgLabel = "POPUP";
        cg = new ChoiceGroup(cgLabel, cgType);
        cg.append("Big Image", Images.getInstance().bigImage);
        addCommand(cmdBack);
        setCommandListener(this);
        //append cg to this form
        append(cg);
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


