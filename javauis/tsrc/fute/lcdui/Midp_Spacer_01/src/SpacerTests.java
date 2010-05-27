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
 * This class is to test the methods of the Form like
 * appending many items, appending one Item, deleting an Item and
 * changing/replacing an Item etc.
 *
 * changed arrange of image items to get layout shown correctly
 */

public class SpacerTests extends List implements CommandListener
{

    private static Midp_Spacer_01 parent = null;

    private static SpacerTests instance = null;

    private final String exCgStr = "Exclusive ChoiceGroup";

    private final String mulCgStr = "Multiple ChoiceGroup";

    private final String popCgStr = "Popup ChoiceGroup";

    private final String dfStr = "DateField";

    private final String iGStr = "Interactive Gauge";

    private final String nGStr = "Non interactive Gauge";

    private final String siStr = "StringItem";

    private final String tfStr = "TextField";

    private final String bigImgStr = "Big Image";

    private final String imgStr = "ImageItem";

    private final String smallSpacerStr = "Small Spacer";

    private final String bigSpacerStr = "Big Spacer";

    private final String wideSpacerStr = "Wide Spacer";

    private final String tallSpacerStr = "Tall Spacer";

    private ChoiceGroup cg = null;

    private DateField df = null;

    private Gauge g = null;

    private StringItem si = null;

    private TextField tf = null;

    private Spacer sp = null;

    private List tests;

    private Form itemForm = null;

    private Command cmdBack = new Command("Back", Command.BACK, 1);

    private Command cmdAppend = new Command("Append Item", Command.SCREEN, 1);

    private Command cmdReplace = new Command("Replace Item", Command.SCREEN, 1);

    private Command cmdDelete = new Command("Delete Item", Command.SCREEN, 1);

    private Command cmdManyItems = new Command("Many Items", Command.SCREEN, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public SpacerTests(Midp_Spacer_01 parent)
    {
        super(parent.getClass().getName(), List.EXCLUSIVE);
        this.parent = parent;

        itemForm = new Form("Items");
        addCommand(cmdManyItems);
        addCommand(cmdAppend);
        addCommand(cmdExit);

        itemForm.addCommand(cmdBack);
        itemForm.setCommandListener(this);

        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
        instance.append(smallSpacerStr, null);
        instance.append(bigSpacerStr, null);
        instance.append(wideSpacerStr, null);
        instance.append(tallSpacerStr, null);
        instance.append(exCgStr, null);
        instance.append(mulCgStr, null);
        instance.append(popCgStr, null);
        instance.append(dfStr, null);
        instance.append(iGStr, null);
        instance.append(nGStr, null);
        instance.append(siStr, null);
        instance.append(tfStr, null);
        instance.append(imgStr, null);
        instance.append(bigImgStr, null);

        show();
    }

    /**
     *  Sets this to be the current screen
     *
     *@return    The Parent value
     */
    public static MIDlet getParent()
    {
        return parent;
    }

    /**
     *  Show this screen
     */
    public static void show()
    {
        Display.getDisplay(parent).setCurrent(instance);
    }

    private void addChoiceGroup(String title, int type)
    {
        cg = new ChoiceGroup(title, type);
        cg.append("1stElement", null);
        cg.append("2ndElement", null);
        cg.append("3ndElement", null);
        itemForm.append(cg);
    }

    private void addGauge(String title, boolean mode)
    {
        g = new Gauge(title, mode, 10, 5);
        itemForm.append(g);
    }

    private void addDateField(String title, int mode)
    {
        df = new DateField(title, mode);
        itemForm.append(df);
    }

    private void addSpacer(int minWidth, int minHeight)
    {
        sp = new Spacer(minWidth, minHeight);
        itemForm.append(sp);
    }

    private void addStringItem(String title, String text)
    {
        si = new StringItem(title, text);
        itemForm.append(si);
    }

    private void addBigImage()
    {
        try
        {
            Image bigImage = Image.createImage("/bigImage.png");
            itemForm.append(bigImage);
        }
        catch (java.io.IOException err)
        {
            System.out.println("Error in creating the image!");
        }
    }

    private void addImageItem()
    {
        try
        {
            Image img = Image.createImage("/plasma.png");

            itemForm.append(new ImageItem("Default Layout",
                                          img,
                                          ImageItem.LAYOUT_DEFAULT,
                                          "Image Cannot be shown"));
        }
        catch (java.io.IOException err)
        {
            System.out.println("Error in creating the image!");
        }
    }

    private void addTextField(String label, int constraints)
    {
        if (label.equals("UNEDITABLE"))
        {
            tf = new TextField(label, "uneditable", 30, constraints);
        }
        else
            tf = new TextField(label, "", 30, constraints);
        itemForm.append(tf);
    }

    private void deleteFormItems()
    {
        int size = itemForm.size();
        if (size != 0)
        {
            for (int i=0; i<size; i++)
                itemForm.delete(0);
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
        int index = instance.getSelectedIndex();
        String item = instance.getString(index);
        if (c == cmdAppend || c == cmdManyItems)
        {
            if (c == cmdAppend)
            {
                deleteFormItems();
                itemForm.removeCommand(cmdReplace);
                itemForm.removeCommand(cmdDelete);
            }
            if (c == cmdManyItems)
            {
                itemForm.addCommand(cmdReplace);
                itemForm.addCommand(cmdDelete);
            }
            if (item.equals(exCgStr))
                addChoiceGroup(exCgStr, ChoiceGroup.EXCLUSIVE);
            else if (item.equals(mulCgStr))
                addChoiceGroup(mulCgStr, ChoiceGroup.MULTIPLE);
            else if (item.equals(popCgStr))
                addChoiceGroup(popCgStr, ChoiceGroup.POPUP);
            else if (item.equals(iGStr))
                addGauge(iGStr, true);
            else if (item.equals(nGStr))
                addGauge(nGStr, false);
            else if (item.equals(dfStr))
            {
                addDateField("DATE", DateField.DATE);
                addDateField("TIME", DateField.TIME);
                addDateField("DATE_TIME", DateField.DATE_TIME);
            }
            else if (item.equals(siStr))
                addStringItem("StringItemLabel", "StringItemText");
            else if (item.equals(imgStr))
                addImageItem();
            else if (item.equals(smallSpacerStr))
                addSpacer(20,20);
            else if (item.equals(bigSpacerStr))
                addSpacer(100,100);
            else if (item.equals(wideSpacerStr))
                addSpacer(1000,10);
            else if (item.equals(tallSpacerStr))
                addSpacer(10,1000);
            else if (item.equals(tfStr))
            {
                addTextField("ANY", TextField.ANY);
                addTextField("UNEDITABLE", TextField.UNEDITABLE);
            }
            else if (item.equals(bigImgStr))
                addBigImage();
        }
        Display.getDisplay(parent).setCurrent(itemForm);
        if (c == cmdBack)
            Display.getDisplay(parent).setCurrent(instance);
        if (c == cmdReplace)
        {
            //Item to replace 1st Item
            Item si = new StringItem("Replaced StringItem", "StringItem Text");
            if (itemForm.size() != 0)
                itemForm.set(0, si);
            Display.getDisplay(parent).setCurrent(itemForm);
        }
        if (c == cmdDelete)
        {
            if (itemForm.size() == 0)
            {
                //to tell the user Form is empty
                Alert a = new Alert("Info", "Form is empty,add Items before testing delete", null, AlertType.INFO);
                a.setTimeout(5000);
                Display.getDisplay(parent).setCurrent(a);
            }
            else
            {
                itemForm.delete(0);
            }
            Display.getDisplay(parent).setCurrent(itemForm);
        }
        if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method
}
