/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.test.java.eswt.listbox;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.FontDialog;
import org.eclipse.ercp.swt.mobile.ListBox;
import org.eclipse.ercp.swt.mobile.ListBoxItem;


import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class ListBoxTestMidlet_01 extends TestBaseMIDlet
{

    /** constrainedtext instance - number of items */
    private ConstrainedText itemNum = null;
    /** list instance */
    private ListBox listBox = null;

    private ListBoxItem[] items = null;

    private int[] single_styles =
    {
        ListBox.LB_STYLE_NO_HEADING_TEXT,
        ListBox.LB_STYLE_NO_HEADING_TEXT|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_NO_HEADING_TEXT|ListBox.LB_MOD_SHOW_SELECTION_NUMBER,
        ListBox.LB_STYLE_NO_HEADING_TEXT|ListBox.LB_MOD_SHOW_SELECTION_NUMBER|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_NO_HEADING_TEXT|ListBox.LB_MOD_SHOW_HEADING_ICONS,
        ListBox.LB_STYLE_NO_HEADING_TEXT|ListBox.LB_MOD_SHOW_HEADING_ICONS|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_1LINE_ITEM,
        ListBox.LB_STYLE_1LINE_ITEM|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_1LINE_ITEM|ListBox.LB_MOD_SHOW_SELECTION_NUMBER,
        ListBox.LB_STYLE_1LINE_ITEM|ListBox.LB_MOD_SHOW_SELECTION_NUMBER|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_1LINE_ITEM|ListBox.LB_MOD_SHOW_HEADING_ICONS,
        ListBox.LB_STYLE_1LINE_ITEM|ListBox.LB_MOD_SHOW_HEADING_ICONS|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_SELECTION_NUMBER,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_SELECTION_NUMBER|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_HEADING_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_HEADING_ICONS|ListBox.LB_MOD_SHOW_DETAIL_ICONS
    };
    private int[] multi_styles =
    {
        ListBox.LB_STYLE_NO_HEADING_TEXT,
        ListBox.LB_STYLE_NO_HEADING_TEXT|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_1LINE_ITEM,
        ListBox.LB_STYLE_1LINE_ITEM|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_DETAIL_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_HEADING_ICONS,
        ListBox.LB_STYLE_2LINE_ITEM|ListBox.LB_MOD_SHOW_HEADING_ICONS|ListBox.LB_MOD_SHOW_DETAIL_ICONS
    };

    private Command CMDMULTI = null;
    private Command CMDSINGLE = null;

    private Command CMDENABLETEXT = null;
    private Command CMDDISABLETEXT = null;

    private Command CMDNEXTSTYLE = null;
    private Command CMDPREVSTYLE = null;

    private Command CMDEMPTY = null;
    private Command CMDREMOVE = null;
    private Command CMDREMOVEALL = null;
    private Command CMDSELECT = null;
    private Command CMDSELECTALL = null;
    private Command CMDDESELECT = null;
    private Command CMDDESELECTALL = null;

    private Command CMDFONT = null;
    private Command CMDHEADFONT = null;

    private Command CMDSET_IMAGE = null;
    private Command CMDSET_EMPTY = null;
    private Command CMDSET_LONG = null;
    private Command CMDSET_SPECIAL = null;

    /** custom image for the ListBox */
    private Image smallImage = null;
    /** custom large image for the ListBox */
    private Image largeImage = null;
    /** custom large image for the ListBox */
    private Image sphereImage = null;
    /** custom large image for the ListBox */
    private Image labelImage = null;
    /** custom transparent image for the ListBox */
    private Image transImage = null;
    /** custom image for the ListBox */
    private Image[] headImages = null;
    /** custom image for the ListBox */
    private Image[] detailImages = null;


    private Shell tooltip = null;
    private int listType = 0;

    private int singlestyleCounter = 0;
    private int multistyleCounter = 0;
    /**
     * initialization
     */
    protected void init()
    {

        // customize the shell
        this.shell.setText("ListBox 1");
        this.shell.setLayout(new RowLayout());
        this.headImages = new Image[4];
        this.detailImages = new Image[4];
        this.singlestyleCounter = 0;
        this.multistyleCounter = 0;
        this.listType = SWT.SINGLE;
        this.createToolTip("ListBox");
        // try to initialize the image
        try
        {
            this.smallImage = new Image(this.display, this.getClass().getResourceAsStream("/res/small.png"));
            this.detailImages[0] = this.smallImage;
            this.headImages[1] = this.smallImage;
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.smallImage = null;
            this.detailImages[0] = null;
            this.headImages[1] = null;
        }

        try
        {
            this.largeImage = new Image(this.display, this.getClass().getResourceAsStream("/res/medium.jpg"));
            this.headImages[3] = this.largeImage;
            this.detailImages[1] = this.largeImage;
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.largeImage = null;
            this.headImages[3] = null;
            this.detailImages[1] = null;
        }
        try
        {
            this.sphereImage = new Image(this.display, this.getClass().getResourceAsStream("/res/sphere.jpg"));
            this.headImages[0] = this.sphereImage;
            this.detailImages[2] = this.sphereImage;
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.sphereImage = null;
            this.headImages[0] = null;
            this.detailImages[2] = null;
        }
        try
        {
            this.labelImage = new Image(this.display, this.getClass().getResourceAsStream("/res/SmallLabel.jpg"));
            this.headImages[2] = this.labelImage;
            this.detailImages[3] = this.labelImage;
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.labelImage = null;
            this.headImages[2] = null;
            this.detailImages[3] = null;
        }
        try
        {
            this.transImage = new Image(this.display, this.getClass().getResourceAsStream("/res/alphatest_small.png"));

        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.transImage = null;

        }
        // "no. of items:" label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("No. of items:");
        label.setLayoutData(new RowData());
        // input field for "no. of items:"
        this.itemNum = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.itemNum.setTextLimit(3);
        this.itemNum.setText(String.valueOf(0));
        this.itemNum.setLayoutData(new RowData());

        // "single listview" left softkey command
        this.CMDSINGLE = new Command(this.shell, Command.SELECT, 1);
        this.CMDSINGLE.setText("Single ListBox");
        this.CMDSINGLE.addSelectionListener(this);
        // "multi listview" left softkey command
        this.CMDMULTI = new Command(this.shell, Command.SELECT, 0);
        this.CMDMULTI.setText("Multi ListBox");
        this.CMDMULTI.addSelectionListener(this);

        this.CMDENABLETEXT = new Command(this.shell, Command.GENERAL, 0);
        this.CMDENABLETEXT.setText("Enable Text");
        this.CMDENABLETEXT.addSelectionListener(this);

        this.CMDDISABLETEXT = new Command(this.shell, Command.GENERAL, 0);
        this.CMDDISABLETEXT.setText("Disable Text");
        this.CMDDISABLETEXT.addSelectionListener(this);

        this.CMDNEXTSTYLE = new Command(this.shell, Command.OK, 0);
        this.CMDNEXTSTYLE.setText("Next style");
        this.CMDNEXTSTYLE.addSelectionListener(this);

        this.CMDPREVSTYLE = new Command(this.shell, Command.OK, 0);
        this.CMDPREVSTYLE.setText("Prev style");
        this.CMDPREVSTYLE.addSelectionListener(this);
    }

    /**
     * create a listbox on the shell
     * @param type the typ of the listbox
     */
    private void createListBox(int type, int layoutstyle)
    {
        // dispose previous listbox
        if (this.listBox != null && !this.listBox.isDisposed())
            this.listBox.dispose();

        int item_count = Integer.parseInt(this.itemNum.getText());
        if (item_count < 0)
            return;

        // create the list
        if (type == SWT.SINGLE)
            this.listBox = new ListBox(this.shell, type  | SWT.BORDER | SWT.V_SCROLL, this.single_styles[layoutstyle]);
        else
            this.listBox = new ListBox(this.shell, type  | SWT.BORDER | SWT.V_SCROLL, this.multi_styles[layoutstyle]);

        this.listType = type;
        // set the items for the listbox

        items = new ListBoxItem[item_count];
        if (type == SWT.SINGLE)
        {
            if (layoutstyle == 0)
                this.createListBoxItems(false,false,false);
            else if (layoutstyle == 1)
                this.createListBoxItems(false,true,false);
            else if (layoutstyle == 2)
                this.createListBoxItems(false,false,false);
            else if (layoutstyle == 3)
                this.createListBoxItems(false,true,false);
            else if (layoutstyle == 4)
                this.createListBoxItems(false,false,true);
            else if (layoutstyle == 5)
                this.createListBoxItems(false,true,true);
            else if (layoutstyle == 6)
                this.createListBoxItems(true,false,false);
            else if (layoutstyle == 7)
                this.createListBoxItems(true,true,false);
            else if (layoutstyle == 8)
                this.createListBoxItems(true,false,false);
            else if (layoutstyle == 9)
                this.createListBoxItems(true,true,false);
            else if (layoutstyle == 10)
                this.createListBoxItems(true,false,true);
            else if (layoutstyle == 11)
                this.createListBoxItems(true,true,true);
            else if (layoutstyle == 12)
                this.createListBoxItems(true,false,false);
            else if (layoutstyle == 13)
                this.createListBoxItems(true,true,false);
            else if (layoutstyle == 14)
                this.createListBoxItems(true,false,false);
            else if (layoutstyle == 15)
                this.createListBoxItems(true,true,false);
            else if (layoutstyle == 16)
                this.createListBoxItems(true,false,true);
            else if (layoutstyle == 17)
                this.createListBoxItems(true,true,true);
        }
        else if (type == SWT.MULTI)
        {
            if (layoutstyle == 0)
                this.createListBoxItems(false,false,false);
            else if (layoutstyle == 1)
                this.createListBoxItems(false,true,false);
            else if (layoutstyle == 2)
                this.createListBoxItems(true,false,false);
            else if (layoutstyle == 3)
                this.createListBoxItems(true,true,false);
            else if (layoutstyle == 4)
                this.createListBoxItems(true,false,false);
            else if (layoutstyle == 5)
                this.createListBoxItems(true,true,false);
            else if (layoutstyle == 6)
                this.createListBoxItems(true,false,true);
            else if (layoutstyle == 7)
                this.createListBoxItems(true,true,true);
        }
        this.listBox.setDataModel(items);
        this.listBox.addSelectionListener(this);

        this.listBox.setLayoutData(new RowData(this.shell.getBounds().width,(this.shell.getBounds().height)-(this.itemNum.getBounds().height*3)-30));
        this.setMenuToList();
        this.shell.layout();
    }

    private void createListBoxItems(boolean head, boolean detailIcon, boolean headIcon)
    {
        for (int i = 0; i < items.length; i++)
        {
            items[i] = new ListBoxItem("detail "+(i+1),null,null,null);
            if (head)
                items[i].setHeadingText("head "+(i+1));
            if (detailIcon)
                items[i].setDetailIcons(this.detailImages);
            if (headIcon)
                items[i].setHeadingIcons(this.headImages);
        }
    }
    private void createEmptyListBox()
    {
        this.items = new ListBoxItem[0];
    }

    private void clearListBoxItems()
    {

        for (int i = 0; i < items.length; i++)
        {
            items[i] = null;
        }

    }

    private void setMenuToList()
    {
        Command remove = new Command(this.listBox, Command.COMMANDGROUP,0);
        remove.setText("Clear");

        this.CMDREMOVE = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVE.setText("Selected");
        this.CMDREMOVE.addSelectionListener(this);

        this.CMDREMOVEALL = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVEALL.setText("All");
        this.CMDREMOVEALL.addSelectionListener(this);

        this.CMDEMPTY = new Command(remove, Command.GENERAL, 0);
        this.CMDEMPTY.setText("Empty ListBox");
        this.CMDEMPTY.addSelectionListener(this);

        Command select = new Command(this.listBox, Command.COMMANDGROUP,0);
        select.setText("Select");

        this.CMDSELECT = new Command(select, Command.GENERAL, 0);
        this.CMDSELECT.setText("Focused");
        this.CMDSELECT.addSelectionListener(this);

        this.CMDSELECTALL = new Command(select, Command.GENERAL, 0);
        this.CMDSELECTALL.setText("All");
        this.CMDSELECTALL.addSelectionListener(this);

        this.CMDDESELECT = new Command(select, Command.GENERAL, 0);
        this.CMDDESELECT.setText("Deselect");
        this.CMDDESELECT.addSelectionListener(this);

        this.CMDDESELECTALL = new Command(select, Command.GENERAL, 0);
        this.CMDDESELECTALL.setText("Deselect all");
        this.CMDDESELECTALL.addSelectionListener(this);

        Command font = new Command(this.listBox, Command.COMMANDGROUP,0);
        font.setText("Font");

        this.CMDFONT = new Command(font, Command.GENERAL, 0);
        this.CMDFONT.setText("Detail");
        this.CMDFONT.addSelectionListener(this);

        this.CMDHEADFONT = new Command(font, Command.GENERAL, 0);
        this.CMDHEADFONT.setText("Head");
        this.CMDHEADFONT.addSelectionListener(this);

        Command set = new Command(this.listBox, Command.COMMANDGROUP,0);
        set.setText("Set");

        this.CMDSET_IMAGE = new Command(set, Command.GENERAL,0);
        this.CMDSET_IMAGE.setText("Image");
        this.CMDSET_IMAGE.addSelectionListener(this);

        this.CMDSET_EMPTY = new Command(set, Command.GENERAL,0);
        this.CMDSET_EMPTY.setText("Empty");
        this.CMDSET_EMPTY.addSelectionListener(this);

        this.CMDSET_LONG = new Command(set, Command.GENERAL,0);
        this.CMDSET_LONG.setText("Long text");
        this.CMDSET_LONG.addSelectionListener(this);

        this.CMDSET_SPECIAL = new Command(set, Command.GENERAL,0);
        this.CMDSET_SPECIAL.setText("Special chars");
        this.CMDSET_SPECIAL.addSelectionListener(this);
    }
    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command)
        {
            this.tooltip.setVisible(false);
            if (event.widget == this.CMDSINGLE)
            {
                // create a single list
                this.createListBox(SWT.SINGLE, this.singlestyleCounter);
                return;
            }
            if (event.widget == this.CMDMULTI)
            {
                // create a multi list
                this.createListBox(SWT.MULTI, this.multistyleCounter);
                return;
            }
            if (event.widget == this.CMDENABLETEXT)
            {
                // enable text control
                this.itemNum.setEnabled(true);
                return;
            }
            if (event.widget == this.CMDDISABLETEXT)
            {
                // disable text control
                this.itemNum.setEnabled(false);
                return;
            }
            if (event.widget == this.CMDNEXTSTYLE)
            {
                if (this.listType == SWT.SINGLE)
                {
                    if (this.singlestyleCounter == this.single_styles.length-1)
                    {
                        this.singlestyleCounter = 0;
                    }
                    else
                    {
                        this.singlestyleCounter++;
                    }
                    this.createListBox(SWT.SINGLE,singlestyleCounter);
                }
                else
                {
                    if (this.multistyleCounter == this.multi_styles.length-1)
                    {
                        this.multistyleCounter = 0;
                    }
                    else
                    {
                        this.multistyleCounter++;
                    }
                    this.createListBox(SWT.MULTI,this.multistyleCounter);
                }
                return;

            }
            if (event.widget == this.CMDPREVSTYLE)
            {
                if (this.listType == SWT.SINGLE)
                {
                    if (this.singlestyleCounter == 0)
                    {
                        this.singlestyleCounter = 0;
                    }
                    else
                    {
                        this.singlestyleCounter--;
                    }
                    this.createListBox(SWT.SINGLE,singlestyleCounter);
                }
                else
                {
                    if (this.multistyleCounter == 0)
                    {
                        this.multistyleCounter = 0;
                    }
                    else
                    {
                        this.multistyleCounter--;
                    }
                    this.createListBox(SWT.MULTI,this.multistyleCounter);
                }
                return;
            }
            if (listBox != null)
            {
                int index = this.listBox.getFocusIndex();
                int indeces[] = this.listBox.getSelectionIndices();

                if (event.widget == this.CMDREMOVE)
                {
                    // remove items
                    if (this.listType == SWT.SINGLE)
                    {
                        this.items[index] = null;
                    }
                    else
                    {
                        for (int i = 0; i < indeces.length; i++)
                        {
                            this.items[indeces[i]] = null;
                        }
                    }
                    this.listBox.refreshList();
                }
                else if (event.widget == this.CMDREMOVEALL)
                {
//                   remove all items
                    try
                    {
                        // set items array full of null
                        this.clearListBoxItems();
                        this.listBox.setDataModel(items);
                    }
                    catch (Exception ex)
                    {
                        // show a messagebox about the exception
                        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                        mb.setText("Exception");
                        mb.setMessage(ex.getMessage());
                        mb.open();
                    }
                    this.listBox.refreshList();
                }
                else if (event.widget == this.CMDEMPTY)
                {
                    //create empty list
                    try
                    {
                        // size of ListBoxItem[] is 0.
                        this.createEmptyListBox();
                        this.listBox.setDataModel(items);
                    }
                    catch (Exception ex)
                    {
                        // show a messagebox about the exception
                        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                        mb.setText("Exception");
                        mb.setMessage(ex.getMessage());
                        mb.open();
                    }
                    this.listBox.refreshList();
                }
                else if (event.widget == this.CMDSELECT)
                {
                    // select items
                    this.listBox.select(index);

                }
                else if (event.widget == this.CMDSELECTALL)
                {
                    // select all items
                    this.listBox.selectAll();

                }
                else if (event.widget == this.CMDDESELECT)
                {
                    // deselect items
                    this.listBox.deselect(index);

                }
                else if (event.widget == this.CMDDESELECTALL)
                {
                    // deselect all items
                    this.listBox.deselectAll();

                }
                else if (event.widget == this.CMDFONT)
                {
                    // set font to items
                    FontDialog dlg = new FontDialog(this.shell);
                    Font font = new Font(this.display,dlg.open());
                    this.listBox.setFont(font);
                }
                else if (event.widget == this.CMDHEADFONT)
                {
                    // set heading font to items
                    FontDialog dlg = new FontDialog(this.shell);
                    Font font = new Font(this.display,dlg.open());
                    this.listBox.setHeadingFont(font);
                }
                else if (event.widget == this.CMDSET_LONG)
                {
                    //
                    if (items.length > 3)
                    {
                        this.items[0].setDetailText("Item's text is changed to long so wrapping can be tested");
                        this.items[1].setHeadingText("Item's text is changed to long so wrapping can be tested");
                        this.items[2].setDetailText("Item's text is changed to long so wrapping can be tested");
                        this.items[2].setHeadingText("Item's text is changed to long so wrapping can be tested");
                    }
                    this.listBox.refreshList();
                }
                else if (event.widget == this.CMDSET_EMPTY)
                {
                    //
                    if (items.length > 3)
                    {
                        this.items[0].setDetailText("");
                        this.items[1].setHeadingText("");
                        this.items[2].setDetailText("");
                        this.items[2].setHeadingText("");
                    }
                    this.listBox.refreshList();
                }
                else if (event.widget == this.CMDSET_SPECIAL)
                {
                    //
                    //
                    if (items.length > 3)
                    {
                        this.items[0].setDetailText("? \n \t %&@#£$ !!!");
                        this.items[1].setHeadingText("? \n \t %&@#£$ !!!");
                        this.items[2].setDetailText("? \n \t %&@#£$ !!!");
                        this.items[2].setHeadingText("? \n \t %&@#£$ !!!");
                    }
                    this.listBox.refreshList();
                }
                else if (event.widget == this.CMDSET_IMAGE)
                {
                    if (items.length > 3)
                    {

                        Image[] setImages = new Image[1];
                        setImages[0] = this.transImage;
                        this.items[0].setDetailIcons(setImages);

                        Image[] setImages2 = new Image[1];
                        setImages2[0] = this.labelImage;
                        this.items[1].setDetailIcons(setImages2);

                        Image[] setImages3 = new Image[1];
                        setImages3[0] = this.largeImage;
                        this.items[2].setDetailIcons(setImages3);

                        Image[] setImages4 = new Image[1];
                        setImages4[0] = this.transImage;
                        this.items[0].setHeadingIcons(setImages4);

                        Image[] setImages5 = new Image[1];
                        setImages5[0] = this.largeImage;
                        this.items[1].setHeadingIcons(setImages5);

                        Image[] setImages6 = new Image[1];
                        setImages6[0] = this.smallImage;
                        this.items[2].setHeadingIcons(setImages6);

                    }
                    this.listBox.refreshList();
                }

            }

        }
        if (event.widget instanceof ListBox)
        {
            ListBoxItem item = this.items[this.listBox.getFocusIndex()];
            if (item != null)
                this.updateToolTip("Selection"+item.getDetailText());
        }
        super.widgetSelected(event);
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {
        if (event.widget instanceof ListBox)
        {
            ListBoxItem item = this.items[this.listBox.getFocusIndex()];
            if (item != null)
                updateToolTip("Default"+this.items[this.listBox.getFocusIndex()].getDetailText());
        }
    }
    /**
     *
     */
    private void createToolTip(String msg)
    {

        // create a modeless shell with border
        this.tooltip = new Shell(this.shell, SWT.MODELESS | SWT.NO_FOCUS | SWT.BORDER);
        this.tooltip.setLayout(new RowLayout(SWT.HORIZONTAL));

        // create a label to be used as a tooltip text
        Label label = new Label(this.tooltip, SWT.NONE);
        label.setText(msg);

        // set the location so the tooltip will be shown in
        // middle of the screen
        this.tooltip.setLocation(Display.getCurrent().getBounds().width/2, Display.getCurrent().getBounds().height/2);

        // finilize
        this.tooltip.pack();
        this.tooltip.layout();
        this.tooltip.setVisible(false);
    }
    /**
     *
     */
    private void updateToolTip(String msg)
    {
        Control[] controls = this.tooltip.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            controls[i].dispose();
        }
        // create a label to be used as a tooltip text
        Label label = new Label(this.tooltip, SWT.NONE);
        label.setText(msg);

        // finilize
        this.tooltip.pack();
        this.tooltip.layout();
        this.tooltip.setVisible(true);
    }
}
