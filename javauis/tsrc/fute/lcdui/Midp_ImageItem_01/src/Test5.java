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
import java.util.*;

/**
 * This class is to test all the ImageForm tests
 *
 */

public class Test5 extends Form implements CommandListener, ItemCommandListener
{

    //referemce to the parent MIDlet
    private MIDlet m = null;

    //the form to add the ImageItems
    private Form imageItemForm = null;

    //the label to create the ImageItem
    private TextField label = null;
    //the text to create the ImageItem
    private TextField width = null;
    //the text to create the ImageItem
    private TextField height = null;
    //the ImageItem to test
    private ImageItem imgItem = null;

    private Image jigsaw_small = null;
    private Image jigsaw_micro = null;
    private Image jigsaw = null;

    private ChoiceGroup cg = null;
    private ChoiceGroup cgImages = null;
    private String[] cgTypes = {"LAYOUT_DEFAULT","LAYOUT_LEFT","LAYOUT_RIGHT","LAYOUT_CENTER","LAYOUT_TOP","LAYOUT_BOTTOM",
                                "LAYOUT_VCENTER","LAYOUT_NEWLINE_BEFORE","LAYOUT_NEWLINE_AFTER","LAYOUT_SHRINK","LAYOUT_VSHRINK",
                                "LAYOUT_EXPAND","LAYOUT_VEXPAND"
                               };

    private String[] cgImageTypes = {"Normal","Small","Micro","Null"};

    //the command to go back to the main screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the command to create the ImageItem with the entered label and text
    private Command cmdCreate = new Command("Create", Command.SCREEN, 1);

    //the command to create the ImageItem with the entered label and text and with focus
    private Command cmdCreateHL = new Command("Create as Hyperlink", Command.SCREEN, 1);

    //the command to create the ImageItem with the entered label and text and with focus
    private Command cmdCreateButton = new Command("Create as Button", Command.SCREEN, 1);

    //the command to unlock the Item
    private Command cmdUnlock = new Command("Unlock", Command.SCREEN, 1);

    //the command for layout test
    private Command cmdLayout = new Command("Layout Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdLayoutButton = new Command("Layout Button Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdLayoutHL = new Command("Layout Hyperlink Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdVLayout = new Command("Vertical Layout Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdVLayoutHL = new Command("Vertical Layout Hyperlink Test", Command.SCREEN, 1);
    //the command for layout test
    private Command cmdVLayoutButton = new Command("Vertical Layout Button Test", Command.SCREEN, 1);

    public Test5(MIDlet m)
    {
        //set the name of the form to be the name of MIDlet
        super("Test 5");
        this.m = m;

        //create TextFields
        label = new TextField("Label:", null, 200, TextField.ANY);
        width = new TextField("Set preferred width:", null, 5, TextField.NUMERIC);
        height = new TextField("Set preferred height:", null, 5, TextField.NUMERIC);
        cgImages = new ChoiceGroup("Image:",ChoiceGroup.EXCLUSIVE,cgImageTypes,null);
        cg = new ChoiceGroup("Layout:",ChoiceGroup.EXCLUSIVE,cgTypes,null);

        jigsaw = Images.getInstance().jigsaw;
        jigsaw_small = Images.getInstance().smallJigsaw;
        jigsaw_micro = Images.getInstance().microJigsaw;

        append(label);
        append(width);
        append(height);
        append(cgImages);
        append(cg);
        addCommand(cmdCreate);
        addCommand(cmdCreateHL);
        addCommand(cmdCreateButton);
        addCommand(cmdLayout);
        addCommand(cmdLayoutHL);
        addCommand(cmdLayoutButton);
        addCommand(cmdVLayout);
        addCommand(cmdVLayoutHL);
        addCommand(cmdVLayoutButton);
        setCommandListener(this);

        Display.getDisplay(m).setCurrent(this);

        //create ImageItemForm
        imageItemForm = new Form("ImageItem");
        imageItemForm.addCommand(cmdBack);
        imageItemForm.setCommandListener(this);
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
        {
            label.setString(null);
            Display.getDisplay(m).setCurrent(this);
        }
        else if (c == cmdLayout)
        {

            layoutTest(Item.PLAIN);

        }
        else if (c == cmdLayoutHL)
        {

            layoutTest(Item.HYPERLINK);

        }
        else if (c == cmdLayoutButton)
        {

            layoutTest(Item.BUTTON);

        }
        else if (c == cmdVLayout)
        {

            verticalLayoutTest(Item.PLAIN);

        }
        else if (c == cmdVLayoutHL)
        {

            verticalLayoutTest(Item.HYPERLINK);

        }
        else if (c == cmdVLayoutButton)
        {

            verticalLayoutTest(Item.BUTTON);

        }
        else
        {
            String l = label.getString();
            String alt = "AltText";
            int imgindex = cgImages.getSelectedIndex();
            String img = cgImages.getString(imgindex);
            int index = cg.getSelectedIndex();
            String lo = cg.getString(index);
            int layout = 0;
            int mode = 0;

            if (c == cmdCreate)
                mode = Item.PLAIN;
            else if (c == cmdCreateHL)
            {
                mode = Item.HYPERLINK;
            }
            else if (c == cmdCreateButton)
            {
                mode = Item.BUTTON;
            }

            if (lo.equals(cgTypes[0]))
            {
                layout = Item.LAYOUT_DEFAULT;
            }
            else if (lo.equals(cgTypes[1]))
            {
                layout = Item.LAYOUT_LEFT;
            }
            else if (lo.equals(cgTypes[2]))
            {
                layout = Item.LAYOUT_RIGHT;
            }
            else if (lo.equals(cgTypes[3]))
            {
                layout = Item.LAYOUT_CENTER;
            }
            else if (lo.equals(cgTypes[4]))
            {
                layout = Item.LAYOUT_TOP;
            }
            else if (lo.equals(cgTypes[5]))
            {
                layout = Item.LAYOUT_BOTTOM;
            }
            else if (lo.equals(cgTypes[6]))
            {
                layout = Item.LAYOUT_VCENTER;
            }
            else if (lo.equals(cgTypes[7]))
            {
                layout = Item.LAYOUT_NEWLINE_BEFORE;
            }
            else if (lo.equals(cgTypes[8]))
            {
                layout = Item.LAYOUT_NEWLINE_AFTER;
            }
            else if (lo.equals(cgTypes[9]))
            {
                layout = Item.LAYOUT_SHRINK;
            }
            else if (lo.equals(cgTypes[10]))
            {
                layout = Item.LAYOUT_VSHRINK;
            }
            else if (lo.equals(cgTypes[11]))
            {
                layout = Item.LAYOUT_EXPAND;
            }
            else if (lo.equals(cgTypes[12]))
            {
                layout = Item.LAYOUT_VEXPAND;
            }

            if (img.equals(cgImageTypes[0]))
            {
                imgItem = new ImageItem(l, jigsaw, layout, alt, mode);
            }
            else if (img.equals(cgImageTypes[1]))
            {
                imgItem = new ImageItem(l, jigsaw_small, layout, alt, mode);
            }
            else if (img.equals(cgImageTypes[2]))
            {
                imgItem = new ImageItem(l, jigsaw_micro, layout, alt, mode);
            }
            else if (img.equals(cgImageTypes[3]))
            {
                imgItem = new ImageItem(l, null, layout, alt, mode);
            }

            if (mode == Item.HYPERLINK || mode == Item.BUTTON)
            {
                imgItem.setDefaultCommand(cmdBack);
                imgItem.setDefaultCommand(cmdUnlock);
            }

            imgItem.setItemCommandListener(this);
            setSize(width.getString(),height.getString());
            imgItem.setLayout(layout);
            imageItemForm.append(imgItem);
            Display.getDisplay(m).setCurrent(imageItemForm);
        }
    }
    private void setSize(String width, String height)
    {
        if (width.equals("") && height.equals(""))
        {
            imgItem.setPreferredSize(-1,-1);
        }
        else if (height.equals(""))
        {
            imgItem.setPreferredSize(Integer.parseInt(width),-1);
        }
        else if (width.equals(""))
        {
            imgItem.setPreferredSize(-1,Integer.parseInt(height));
        }
        else
        {
            imgItem.setPreferredSize(Integer.parseInt(width),Integer.parseInt(height));
        }
    }
    private void layoutTest(int mode)
    {
        imageItemForm.deleteAll();
        imgItem = new ImageItem("Default Layout",jigsaw_small,ImageItem.LAYOUT_DEFAULT,"ImageItem",mode);
        ImageItem si2 = new ImageItem("Layout Right",jigsaw_small,ImageItem.LAYOUT_RIGHT,"ImageItem",mode);
        ImageItem si3 = new ImageItem("Layout Left",jigsaw_small,ImageItem.LAYOUT_LEFT,"ImageItem",mode);
        ImageItem si4 = new ImageItem("Layout Center",jigsaw_small,ImageItem.LAYOUT_CENTER,"ImageItem",mode);
        ImageItem si5 = new ImageItem("Layout Expand",jigsaw_small,ImageItem.LAYOUT_EXPAND | ImageItem.LAYOUT_NEWLINE_BEFORE,"ImageItem",mode);
        si5.setDefaultCommand(cmdBack);

        if (mode != Item.PLAIN)
        {
            imgItem.setDefaultCommand(cmdBack);
            si2.setDefaultCommand(cmdBack);
            si3.setDefaultCommand(cmdBack);
            si4.setDefaultCommand(cmdBack);
        }
        imageItemForm.append(imgItem);
        imageItemForm.append(si2);
        imageItemForm.append(si3);
        imageItemForm.append(si4);
        imageItemForm.append(si5);

        Display.getDisplay(m).setCurrent(imageItemForm);
    }

    private void verticalLayoutTest(int mode)
    {
        imageItemForm.deleteAll();
        imgItem = new ImageItem("Default",jigsaw_small,ImageItem.LAYOUT_DEFAULT,"ImageItem",mode);
        imgItem.setPreferredSize(-1,150);

        ImageItem si7 = new ImageItem("Top",jigsaw_small,ImageItem.LAYOUT_TOP,"ImageItem",mode);
        ImageItem si8 = new ImageItem("Vcenter",jigsaw_small,ImageItem.LAYOUT_VCENTER,"ImageItem",mode);
        ImageItem si9 = new ImageItem("Vexpand",jigsaw_small,ImageItem.LAYOUT_VEXPAND,"ImageItem",mode);

        if (mode != Item.PLAIN)
        {
            imgItem.setDefaultCommand(cmdBack);
            si7.setDefaultCommand(cmdBack);
            si8.setDefaultCommand(cmdBack);
            si9.setDefaultCommand(cmdBack);
        }
        imageItemForm.append(imgItem);
        imageItemForm.append(si7);
        imageItemForm.append(si8);
        imageItemForm.append(si9);

        Display.getDisplay(m).setCurrent(imageItemForm);
    }
    public void commandAction(Command c, Item i)
    {
        if (c == cmdBack)
        {
            imgItem = null;
            label.setString(null);
            Display.getDisplay(m).setCurrent(this);
        }
        else if (c == cmdUnlock)
        {
            i.setPreferredSize(-1,-1);
        }
    }
}






