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
/**
 * This class is to test the List:
 * 1.when it has elements with no associated image.
 * 2.when it has too long element label.
 * 3.when it has many elements with image
 * 4.when the list is empty.
 *
 */

public class Test1 extends List implements CommandListener
{

    //the instance of this
    private Test1 instance = null;

    //the command to go back to the ListTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);
    Command cmdAdd = new Command("Add image", Command.SCREEN, 1);
    Command cmdRemove = new Command("Remove All", Command.SCREEN, 1);
    Command cmdSize = new Command("Show size", Command.SCREEN, 1);

    //elements to this list
    private final String element1 = "This is a very long element label. It should be truncated after two lines" +
                                    " with an ellipsis. Check the output that the ellipsis exists there." +
                                    " Let's see if this text is so long that it will be longer than" +
                                    " four lines in the possible list info popup window. Then we can see" +
                                    " if the text was scrollable.";
    private final String element2 ="2nd Element";
    private final String element3 ="3rd Element";
    private final String element4 ="4th Element";
    private final String element5 ="5th Element";

    /**
     *
     * @param listType The List type
     */
    public Test1(int listType)
    {

        super("Test1", listType);

        //append elements to this list
        append(element1, null);
        append(element2, null);
        append(element3, null);
        append(element4, null);
        append(element5, null);

        //add commands to this list
        addCommand(cmdBack);
        addCommand(cmdAdd);
        addCommand(cmdRemove);
        addCommand(cmdSize);
        setCommandListener(this);

        if (instance == null)
            instance = this;
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
            ListTests.show();
        else if (c == cmdAdd)
            instance.insert(0, "Inserted1", Images.getInstance().smallImage);
        else if ((c == cmdRemove) && (instance.size() != 0))
        {
            instance.deleteAll();
        }
        else if (c == cmdSize)
        {
            //insert element at the 0 position
            String size = new String("");
            System.out.println("Font Size: ");
            System.out.println(this.getFont(instance.getSelectedIndex()).getSize());
            System.out.println(this.getFont(instance.getSelectedIndex()).getStyle());
            System.out.println(this.getFont(instance.getSelectedIndex()).getFace());
            System.out.println(this.getFont(instance.getSelectedIndex()).getHeight());
            size = "Font Size: "+instance.getFont(instance.getSelectedIndex()).getHeight()+" "+
                   ListTests.whatSize(instance.getFont(instance.getSelectedIndex()).getSize())+" "+
                   ListTests.whatStyle(instance.getFont(instance.getSelectedIndex()).getStyle())+" "+
                   ListTests.whatFace(instance.getFont(instance.getSelectedIndex()).getFace());
            instance.insert(0, size, null);
        }
    }
}
