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
 *
 */

public class ItemTests extends Form implements CommandListener
{

    //reference to the MIDlet to go to different screens
    private static Midp_Item_02 parent = null;

    private String appendItems[] = {"StringItem1", "ImageItem", "ChoiceGroup", "DateField",
                                    "Gauge", "TextField", "StringItem2"
                                   };

    //cg elements
    private String cgElements[] = {"element1", "element2", "element3", "element4", "element5", "element6"};

    private List list1;

    private Command cmdAppend1 =new Command("Append1", Command.SCREEN, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private StringItem str1, str2, str3;

    private ChoiceGroup cg1;

    private Gauge g1;

    private DateField df1;

    private TextField tf1;

    private ImageItem imgItem1;

    private Image image = null;

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public ItemTests(Midp_Item_02 parent)
    {
        super(parent.getClass().getName());
        this.parent = parent;
        str1 = new StringItem("StringItem", "Text");
        str2 = new StringItem("NewStringItem1", "Text");
        str3 = new StringItem("NewStringItem2", "Text\n");
        try
        {
            image = Image.createImage("/Icon.png");
        }
        catch (IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
        imgItem1 = new ImageItem("ImageItem", image,
                                 Item.LAYOUT_NEWLINE_AFTER, null);
        str2.setLayout(Item.LAYOUT_NEWLINE_AFTER);

        cg1 = new ChoiceGroup("ChoiceGroup", Choice.EXCLUSIVE, cgElements, null);
        df1 = new DateField("DateField", DateField.DATE_TIME);
        tf1 = new TextField("TextField", null, 100, TextField.ANY);
        g1= new Gauge("Interactive Gauge", true, 50, 10);

        list1 = new List("Insert Item", Choice.IMPLICIT);
        for (int i= 0; i<appendItems.length; i++)
            list1.append(appendItems[i], null);
        list1.setCommandListener(this);
        append(str1);
        addCommand(cmdAppend1);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    private void deleteItems()
    {
        int size = size();
        if (size>1)
            delete(0);
    }

    /**
    *  This method handles command invocations.
    *
    *@param  c  This is the command responsible for the event.
    *@param  s  Should be equal to this.
    */
    public void commandAction(Command c, Displayable s)
    {
        if (c == List.SELECT_COMMAND)
        {
            if (s == list1)
            {
                int i = list1.getSelectedIndex();
                String item = list1.getString(i);
                deleteItems();
                if (item.equals(appendItems[0]))
                    insert(0, str2);
                if (item.equals(appendItems[1]))
                    insert(0, imgItem1);
                if (item.equals(appendItems[2]))
                    insert(0, cg1);
                if (item.equals(appendItems[3]))
                    insert(0, df1);
                if (item.equals(appendItems[4]))
                    insert(0, g1);
                if (item.equals(appendItems[5]))
                    insert(0, tf1);
                if (item.equals(appendItems[6]))
                    insert(0, str3);
                Display.getDisplay(parent).setCurrent(this);
            }
        }
        else if (c == cmdAppend1)
        {
            Display.getDisplay(parent).setCurrent(list1);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }

}
