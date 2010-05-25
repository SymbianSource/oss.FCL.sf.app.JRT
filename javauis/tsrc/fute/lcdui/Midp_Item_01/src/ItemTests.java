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
 *  changed Append2 ImageItem so that StringItem has different layout than ImageItem.
 */

public class ItemTests extends Form implements CommandListener
{

    //reference to the MIDlet to go to different screens
    private static Midp_Item_01 parent = null;

    private String appendItems[] = {"StringItem1", "ImageItem", "ChoiceGroup", "DateField",
                                    "Gauge", "TextField", "StringItem2"
                                   };

    //cg elements
    private String cgElements[] = {"element1", "element2", "element3", "element4", "element5", "element6"};

    private List list1, list2, list3, list4;

    private Command cmdAppend1 =new Command("Append1", Command.SCREEN, 1);

    private Command cmdAppend2 =new Command("Append2", Command.SCREEN, 1);

    private Command cmdAppend3 =new Command("Append3", Command.SCREEN, 1);

    private Command cmdAppend4 =new Command("Append4", Command.SCREEN, 1);

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
    public ItemTests(Midp_Item_01 parent)
    {
        super(parent.getClass().getName());
        this.parent = parent;
        str1 = new StringItem("StringItem", "Text");
        str2 = new StringItem("NewStringItem1", "Text");
        str3 = new StringItem("\nNewStringItem2", "Text");
        try
        {
            image = Image.createImage("/Icon.png");
        }
        catch (IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
        imgItem1 = new ImageItem("ImageItem", image,
                                 Item.LAYOUT_NEWLINE_BEFORE, null);

        cg1 = new ChoiceGroup("ChoiceGroup", Choice.EXCLUSIVE, cgElements, null);
        df1 = new DateField("DateField", DateField.DATE_TIME);
        tf1 = new TextField("TextField", null, 100, TextField.ANY);
        g1= new Gauge("Interactive Gauge", true, 50, 10);

        list1 = new List("Insert Item", Choice.IMPLICIT);
        list2 = new List("Insert Item", Choice.IMPLICIT);
        list3 = new List("Insert Item", Choice.IMPLICIT);
        list4 = new List("Insert Item", Choice.IMPLICIT);
        for (int i= 0; i<appendItems.length; i++)
            list1.append(appendItems[i], null);
        for (int i= 0; i<appendItems.length-1; i++)
        {
            list2.append(appendItems[i], null);
            list3.append(appendItems[i], null);
            list4.append(appendItems[i], null);
        }
        list1.setCommandListener(this);
        list2.setCommandListener(this);
        list3.setCommandListener(this);
        list4.setCommandListener(this);
        append(str1);
        addCommand(cmdAppend1);
        addCommand(cmdAppend2);
        addCommand(cmdAppend3);
        addCommand(cmdAppend4);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    private void deleteItems()
    {
        int size = size();
        if (size>0)
        {
            for (int i=1; i<size; i++)
                delete(i);
        }
    }

    private void appendCg(int layout)
    {
        cg1.setLayout(layout);
        append(cg1);
    }

    private void appendDf(int layout)
    {
        df1.setLayout(layout);
        append(df1);
    }
    private void appendG(int layout)
    {
        g1.setLayout(layout);
        append(g1);
    }
    private void appendImgItem(int layout)
    {
        imgItem1.setLayout(layout);
        System.out.println(imgItem1.getLayout());
        append(imgItem1);
    }

    private void appendSi(int layout)
    {
        str2.setLayout(layout);
        append(str2);
    }

    private void appendTf(int layout)
    {
        tf1.setLayout(layout);
        append(tf1);
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
            imgItem1.setLayout(Item.LAYOUT_NEWLINE_BEFORE);
            str1.setLayout(Item.LAYOUT_DEFAULT);
            str2.setLayout(Item.LAYOUT_NEWLINE_BEFORE);
            if (s == list1)
            {
                int i = list1.getSelectedIndex();
                String item = list1.getString(i);
                deleteItems();
                if (item.equals(appendItems[0]))
                    append(str2);
                if (item.equals(appendItems[1]))
                    append(imgItem1);
                if (item.equals(appendItems[2]))
                    append(cg1);
                if (item.equals(appendItems[3]))
                    append(df1);
                if (item.equals(appendItems[4]))
                    append(g1);
                if (item.equals(appendItems[5]))
                    append(tf1);
                if (item.equals(appendItems[6]))
                    append(str3);
                Display.getDisplay(parent).setCurrent(this);
            }
            if (s == list2)
            {
                int id = list2.getSelectedIndex();
                String text = list2.getString(id);
                deleteItems();
                if (text.equals(appendItems[0]))
                    appendSi(Item.LAYOUT_LEFT);
                if (text.equals(appendItems[1]))
                {//different layout for StringItem that IageItem has
                    str1.setLayout(Item.LAYOUT_CENTER);
                    appendImgItem(Item.LAYOUT_LEFT);
                }
                if (text.equals(appendItems[2]))
                    appendCg(Item.LAYOUT_LEFT);
                if (text.equals(appendItems[3]))
                    appendDf(Item.LAYOUT_LEFT);
                if (text.equals(appendItems[4]))
                    appendG(Item.LAYOUT_LEFT);
                if (text.equals(appendItems[5]))
                    appendTf(Item.LAYOUT_LEFT);
                Display.getDisplay(parent).setCurrent(this);
            }
            if (s == list3)
            {
                int id = list3.getSelectedIndex();
                String text = list3.getString(id);
                deleteItems();
                if (text.equals(appendItems[0]))
                    appendSi(Item.LAYOUT_CENTER);
                if (text.equals(appendItems[1]))
                    appendImgItem(Item.LAYOUT_CENTER);
                if (text.equals(appendItems[2]))
                    appendCg(Item.LAYOUT_CENTER);
                if (text.equals(appendItems[3]))
                    appendDf(Item.LAYOUT_CENTER);
                if (text.equals(appendItems[4]))
                    appendG(Item.LAYOUT_CENTER);
                if (text.equals(appendItems[5]))
                    appendTf(Item.LAYOUT_CENTER);
                Display.getDisplay(parent).setCurrent(this);
            }
            if (s == list4)
            {
                int id = list4.getSelectedIndex();
                String text = list4.getString(id);
                deleteItems();
                if (text.equals(appendItems[0]))
                    appendSi(Item.LAYOUT_RIGHT);
                if (text.equals(appendItems[1]))
                    appendImgItem(Item.LAYOUT_RIGHT);
                if (text.equals(appendItems[2]))
                    appendCg(Item.LAYOUT_RIGHT);
                if (text.equals(appendItems[3]))
                    appendDf(Item.LAYOUT_RIGHT);
                if (text.equals(appendItems[4]))
                    appendG(Item.LAYOUT_RIGHT);
                if (text.equals(appendItems[5]))
                    appendTf(Item.LAYOUT_RIGHT);
                Display.getDisplay(parent).setCurrent(this);
            }

        }
        else if (c == cmdAppend1)
        {
            Display.getDisplay(parent).setCurrent(list1);
        }
        else if (c == cmdAppend2)
        {
            Display.getDisplay(parent).setCurrent(list2);
        }
        else if (c == cmdAppend3)
        {
            Display.getDisplay(parent).setCurrent(list3);
        }
        else if (c == cmdAppend4)
        {
            Display.getDisplay(parent).setCurrent(list4);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }

}
