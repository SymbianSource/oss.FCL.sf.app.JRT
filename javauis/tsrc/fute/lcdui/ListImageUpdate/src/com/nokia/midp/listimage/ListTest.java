/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.midp.listimage;

/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

import java.io.*;
/**
 * This class is to test the List:
 * With small images and one line element text and image update.
 */

public class ListTest extends List implements CommandListener
{

    private static ListImageUpdate mid = null;
    private static ListTest instance = null;

    private Command cmdUpdate = new Command("Update images", Command.SCREEN,1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);
    private String element;
    private Image aImage;
    //the elements of this list
    String str[] =
    {
        "1stElement should be shown in two lines and text should be truncated",
        "2ndElement", "3rdElement", "4thElement",
        "5thElement", "6thElement", "7thElement", "8thElement",
        "9thElement", "10thElement"
    };

    public ListTest(ListImageUpdate m)
    {
        super("ListImageUpdate", List.IMPLICIT);
        mid = m;
        instance = this;
        addElements();
        addCommand(cmdUpdate);
        addCommand(cmdExit);
        setSelectCommand(SELECT_COMMAND);
        setCommandListener(this);
        Display.getDisplay(mid).setCurrent(instance);

    }
    private void addElements()
    {
        for (int i= 0; i< 10; i++)
            append(str[i], Images.getInstance().smallImage);
    }

    public void commandAction(Command c, Displayable s)
    {

        if (c == SELECT_COMMAND)
        {
            int eIndex = getSelectedIndex();

            element = getString(eIndex);
            aImage = getImage(eIndex);
            if (aImage == Images.getInstance().smallImage)
            {
                set(eIndex, element, Images.getInstance().starImage);
            }
            else if (aImage == Images.getInstance().starImage)
            {
                set(eIndex, element, Images.getInstance().bigImage);
            }
            else if (aImage == Images.getInstance().bigImage)
            {
                set(eIndex, element, Images.getInstance().smallImage);
            }
        }
        if (c == cmdUpdate)
        {
            for (int i= 0; i< 10; i++)
            {
                element = getString(i);
                set(i,element, Images.getInstance().smallImage);
            }
        }
        else if (c == cmdExit)
        {
            mid.destroyApp(false);
            mid.notifyDestroyed();
        }
    }

}//end
