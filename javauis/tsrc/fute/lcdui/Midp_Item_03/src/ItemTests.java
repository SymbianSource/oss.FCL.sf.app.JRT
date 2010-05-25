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
    private static Midp_Item_03 parent = null;

    private String appendItems[] = {"StringItem1", "DateField",
                                    "Gauge", "TextField"
                                   };

    private String insertItems[] = {"StringItem1", "ImageItem"};
    private List list1, list2;

    private Command cmdTest1 =new Command("Test1", Command.SCREEN, 1);

    private Command cmdTest2 =new Command("Test2", Command.SCREEN, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private StringItem str1, str2, str3, str4;

    private Gauge g1;

    private DateField df1;

    private TextField tf1;

    private ImageItem imgItem1;

    private Image image = null;

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public ItemTests(Midp_Item_03 parent)
    {
        super(parent.getClass().getName());
        this.parent = parent;
        str1 = new StringItem("StringItem", "Text");
        str2 = new StringItem("\nNewStringItem1", "Text");
        str3 = new StringItem("StringItem", "Text");
        str4 = new StringItem("NewStringItem1", "Text\n");
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
        str2.setLayout(Item.LAYOUT_NEWLINE_BEFORE);
        str4.setLayout(Item.LAYOUT_NEWLINE_AFTER);
        imgItem1.setLayout(Item.LAYOUT_NEWLINE_AFTER);

        df1 = new DateField("\nDateField", DateField.DATE_TIME);
        tf1 = new TextField("\nTextField", null, 100, TextField.ANY);
        g1= new Gauge("\nInteractive Gauge", true, 50, 10);

        df1.setLayout(Item.LAYOUT_NEWLINE_BEFORE);
        tf1.setLayout(Item.LAYOUT_NEWLINE_BEFORE);
        g1.setLayout(Item.LAYOUT_NEWLINE_BEFORE);

        list1 = new List("Append Item", Choice.IMPLICIT);
        list2 = new List("Insert Item", Choice.IMPLICIT);
        for (int i= 0; i<appendItems.length; i++)
            list1.append(appendItems[i], null);
        for (int i= 0; i<insertItems.length; i++)
            list2.append(insertItems[i], null);

        list1.setCommandListener(this);
        list2.setCommandListener(this);
        append(str1);
        addCommand(cmdTest1);
        addCommand(cmdTest2);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    private void deleteItems()
    {
        int size = size();
        for (int i= 1; i<size; i++)
            delete(i);
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
                    append(str2);
                if (item.equals(appendItems[1]))
                    append(df1);
                if (item.equals(appendItems[2]))
                    append(g1);
                if (item.equals(appendItems[3]))
                    append(tf1);
                Display.getDisplay(parent).setCurrent(this);
            }
            if (s == list2)
            {
                int index = list2.getSelectedIndex();
                String elem = list2.getString(index);
                if (elem.equals(insertItems[0]))
                    insert(0, str4);
                else if (elem.equals(insertItems[1]))
                    insert(0, imgItem1);
                Display.getDisplay(parent).setCurrent(this);
            }
        }
        else if (c == cmdTest1)
        {
            Display.getDisplay(parent).setCurrent(list1);
        }
        else if (c == cmdTest2)
        {
            deleteAll();
            append(str3);
            Display.getDisplay(parent).setCurrent(list2);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}




