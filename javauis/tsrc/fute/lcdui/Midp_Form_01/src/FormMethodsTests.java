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

public class FormMethodsTests extends List implements CommandListener, ItemStateListener
{

    private static Midp_Form_01 parent = null;

    private static FormMethodsTests instance = null;

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

    private final String spacerStr = "Spacer (100x100)";

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

    private Command cmdItemState = new Command("Item State Test", Command.SCREEN, 1);

    private Command cmdChangeStringItem = new Command("Change StringItem", Command.SCREEN, 1);

    private Command cmdChangeImageItem = new Command("Change ImageItem", Command.SCREEN, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public FormMethodsTests(Midp_Form_01 parent)
    {
        super(parent.getClass().getName(), List.EXCLUSIVE);
        this.parent = parent;

        itemForm = new Form("Items");
        addCommand(cmdManyItems);
        addCommand(cmdAppend);
        addCommand(cmdItemState);
        addCommand(cmdExit);

        itemForm.addCommand(cmdBack);
        itemForm.setCommandListener(this);
        itemForm.setItemStateListener(this);

        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
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
        instance.append(spacerStr, null);
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
        cg.append("4thElement", null);
        cg.append("5thElement", null);
        cg.append("6thElement", null);
        cg.append("7thElement", null);
        cg.append("8thElement", null);
        cg.append("9thElement", null);
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

            itemForm.append(new ImageItem("Layout Center",
                                          img,
                                          ImageItem.LAYOUT_CENTER,
                                          "Image Cannot be shown"));

            itemForm.append(new ImageItem("Layout Left",
                                          img,
                                          ImageItem.LAYOUT_LEFT,
                                          "Image Cannot be shown"));

            itemForm.append(new ImageItem("Layout Center After",
                                          img,
                                          ImageItem.LAYOUT_CENTER |
                                          ImageItem.LAYOUT_NEWLINE_AFTER,
                                          "Image Cannot be shown"));

            itemForm.append(new ImageItem("Layout Right",
                                          img,
                                          ImageItem.LAYOUT_RIGHT,
                                          "Image Cannot be shown"));

            itemForm.append(new ImageItem("Layout Center Before",
                                          img,
                                          ImageItem.LAYOUT_CENTER |
                                          ImageItem.LAYOUT_NEWLINE_BEFORE,
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
    public void itemStateChanged(Item item)
    {
        itemForm.setTitle(item.getLabel() + " modified!");
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
        if (c == cmdItemState)
        {
            addChoiceGroup(exCgStr, ChoiceGroup.EXCLUSIVE);
            addChoiceGroup(mulCgStr, ChoiceGroup.MULTIPLE);
            addChoiceGroup(popCgStr, ChoiceGroup.POPUP);
            addGauge(iGStr, true);
            addDateField("DATE_TIME", DateField.DATE_TIME);
            addTextField("ANY", TextField.ANY);
            addStringItem("StringItemLabel", "StringItemText");
            addImageItem();
            itemForm.addCommand(cmdChangeStringItem);
            itemForm.addCommand(cmdChangeImageItem);
        }
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
            else if (item.equals(spacerStr))
                addSpacer(100,100);
            else if (item.equals(tfStr))
            {
                addTextField("ANY", TextField.ANY);
                addTextField("EMAILADDR", TextField.EMAILADDR);
                addTextField("NUMERIC", TextField.NUMERIC);
                addTextField("PHONENUMBER", TextField.PHONENUMBER);
                addTextField("URL", TextField.URL);
                addTextField("PASSWORD", TextField.PASSWORD);
                addTextField("PASSWORD|NUMERIC", TextField.PASSWORD|TextField.NUMERIC);
                addTextField("PASSWORD|PHONENUMBER", TextField.PASSWORD|TextField.PHONENUMBER);
                addTextField("PASSWORD|URL", TextField.PASSWORD|TextField.URL);
                addTextField("PASSWORD|EMAILADDR", TextField.PASSWORD|TextField.EMAILADDR);
                addTextField("DECIMAL", TextField.DECIMAL);
                addTextField("INITIAL_CAPS_SENTENCE", TextField.INITIAL_CAPS_SENTENCE);
                addTextField("PINITIAL_CAPS_WORD", TextField.INITIAL_CAPS_WORD);
                addTextField("UNEDITABLE", TextField.UNEDITABLE);
                addTextField("NON_PREDICTIVE", TextField.NON_PREDICTIVE);
                addTextField("SENSITIVE", TextField.SENSITIVE);
            }
            else if (item.equals(bigImgStr))
                addBigImage();
        }
        if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
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
        if (c == cmdChangeStringItem)
        {
            (itemForm.get(6)).setLabel("StringItem Changed!");
            (itemForm.get(6)).notifyStateChanged();
        }
        if (c == cmdChangeImageItem)
        {
            (itemForm.get(7)).setLabel("ImageItem Changed!");
            (itemForm.get(7)).notifyStateChanged();
        }
    }
}
