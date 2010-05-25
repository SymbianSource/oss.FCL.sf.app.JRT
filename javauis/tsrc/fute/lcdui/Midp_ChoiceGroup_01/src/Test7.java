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
 * This is to test the below ChoiceGroup test case:
 *1.elements with different fonts
 *
 */

public class Test7 extends Form implements CommandListener
{

    //the ChoiceGroup to test
    private ChoiceGroup cg = null;

    //the command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the elements which are added to the cg
    private String str[] =
    {
        "2ndElement", "3rdElement", "4thElement",
        "5thElement", "6thElement", "7thElement", "8thElement",
        "9thElement", "10thElement"
    };

    /**
    *
    * @param cgType The ChoiceGroup type
    */
    public Test7(int cgType)
    {

        super("Test7");
        String cgLabel = null;
        //set the ChoiceGroup label according to the type of ChoiceGroup "cg"
        if (cgType == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.POPUP)
            cgLabel = "POPUP";
        addCommand(cmdBack);
        setCommandListener(this);
        cg = new ChoiceGroup(cgLabel, cgType);
        append(cg);
        cg.append("1stElement", Images.getInstance().smallImage);
        //add elements to the ChoiceGroup "cg"
        addElements();

        cg.setFont(0, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM));
        cg.setFont(1, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM));
        cg.setFont(2, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_ITALIC, Font.SIZE_MEDIUM));
        cg.setFont(3, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD|Font.STYLE_ITALIC, Font.SIZE_MEDIUM));

        cg.setFont(4, Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_UNDERLINED|Font.STYLE_ITALIC, Font.SIZE_SMALL));

        cg.setFont(5, Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_LARGE));
    }

    /**
     * to append elements to the ChoiceGroup "cg"
     */
    private void addElements()
    {
        for (int i= 0; i< 9; i++)
        {
            cg.append(str[i], null);
        }
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


