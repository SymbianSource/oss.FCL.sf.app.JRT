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
package com.nokia.test.java.eswt.listview;

import javax.microedition.lcdui.Font;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.MobileDevice;
import org.eclipse.ercp.swt.mobile.Screen;
import org.eclipse.ercp.swt.mobile.ScreenEvent;
import org.eclipse.ercp.swt.mobile.ScreenListener;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ercp.swt.mobile.ListView;


import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class ListViewTestMIDlet_01 extends TestBaseMIDlet implements  ScreenListener
{

    /** constrainedtext instance - number of items */
    private ConstrainedText itemNum = null;
    /** list instance */
    private ListView listView = null;

    private Command CMDMULTI = null;
    private Command CMDSINGLE = null;
    private Command CMDSINGLE_IMAGE = null;
    private Command CMDMULTI_IMAGE = null;

    private Command CMDENABLETEXT = null;
    private Command CMDDISABLETEXT = null;

    private Command CMDREMOVE = null;
    private Command CMDREMOVELIST = null;
    private Command CMDREMOVEALL = null;
    private Command CMDSELECT = null;
    private Command CMDSELECTALL = null;
    private Command CMDDESELECT = null;
    private Command CMDDESELECTALL = null;
    private Command CMDADD = null;

    private Command CMDSET_IMAGE = null;
    private Command CMDSET_TRANSIMAGE = null;
    private Command CMDSET_EMPTY = null;
    private Command CMDSET_LONG = null;
    private Command CMDSET_SPECIAL = null;

    private Command CMDSETSELECT = null;
    private Command CMDSETSELECTALL = null;
    private Command CMDSETSELECTINDECS = null;
    private Command CMDSETSELECTSTRING = null;

    private Command CMDDENSITY_LOW = null;
    private Command CMDDENSITY_MEDIUM = null;
    private Command CMDDENSITY_HIGH = null;

    private MenuItem cmdSingle = null;
    private MenuItem cmdMulti = null;
    /** custom image for the Listview */
    private Image smallImage = null;
    /** custom large image for the Listview */
    private Image largeImage = null;
    /** custom transparent image for the Listview */
    private Image transImage = null;

    private Shell tooltip = null;

    private MobileDevice iDevice;
    private Screen iScreen;
    /**
     * initialization
     */
    protected void init()
    {

        // customize the shell
        this.shell.setText("ListView 1");
        this.shell.setLayout(new RowLayout());
        iDevice = MobileDevice.getMobileDevice();
        this.iScreen = iDevice.getScreens()[0];
        iScreen.addEventListener(this);
        // try to initialize the image
        try
        {
            this.smallImage = new Image(this.display, this.getClass().getResourceAsStream("/small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.smallImage = null;
        }

        try
        {
            this.largeImage = new Image(this.display, this.getClass().getResourceAsStream("/medium.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.largeImage = null;
        }
        try
        {
            this.transImage = new Image(this.display, this.getClass().getResourceAsStream("/alphatest_small.png"));
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

        this.createToolTip("");
        // "no. of items:" label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("No. of items:");
        label.setLayoutData(new RowData());
        // input field for "no. of items:"
        this.itemNum = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.itemNum.setTextLimit(3);
        this.itemNum.setText(String.valueOf(0));
        this.itemNum.setLayoutData(new RowData(70,SWT.DEFAULT));

        // "single listview" left softkey command
        this.CMDSINGLE = new Command(this.shell, Command.SELECT, 1);
        this.CMDSINGLE.setText("Single ListView");
        this.CMDSINGLE.addSelectionListener(this);
        // "multi listview" left softkey command
        this.CMDMULTI = new Command(this.shell, Command.OK, 0);
        this.CMDMULTI.setText("Multi ListView");
        this.CMDMULTI.addSelectionListener(this);
        // "single listview" left softkey command
        this.CMDSINGLE_IMAGE = new Command(this.shell, Command.OK, 0);
        this.CMDSINGLE_IMAGE.setText("Single ListView with image");
        this.CMDSINGLE_IMAGE.addSelectionListener(this);
        // "multi listview" left softkey command
        this.CMDMULTI_IMAGE = new Command(this.shell, Command.SELECT, 0);
        this.CMDMULTI_IMAGE.setText("Multi ListView with image");
        this.CMDMULTI_IMAGE.addSelectionListener(this);

        this.CMDENABLETEXT = new Command(this.shell, Command.GENERAL, 0);
        this.CMDENABLETEXT.setText("Enable Text");
        this.CMDENABLETEXT.addSelectionListener(this);

        this.CMDDISABLETEXT = new Command(this.shell, Command.GENERAL, 0);
        this.CMDDISABLETEXT.setText("Disable Text");
        this.CMDDISABLETEXT.addSelectionListener(this);

        Command density = new Command(this.shell, Command.COMMANDGROUP,0);
        density.setText("Density");

        this.CMDDENSITY_LOW = new Command(density, Command.GENERAL, 0);
        this.CMDDENSITY_LOW.setText("Low");
        this.CMDDENSITY_LOW.addSelectionListener(this);

        this.CMDDENSITY_MEDIUM = new Command(density, Command.GENERAL, 0);
        this.CMDDENSITY_MEDIUM.setText("Medium");
        this.CMDDENSITY_MEDIUM.addSelectionListener(this);

        this.CMDDENSITY_HIGH = new Command(density, Command.GENERAL, 0);
        this.CMDDENSITY_HIGH.setText("High");
        this.CMDDENSITY_HIGH.addSelectionListener(this);

        Command remove = new Command(this.shell, Command.COMMANDGROUP,0);
        remove.setText("Remove");

        this.CMDREMOVE = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVE.setText("Selected");
        this.CMDREMOVE.addSelectionListener(this);

        this.CMDREMOVELIST = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVELIST.setText("Start");
        this.CMDREMOVELIST.addSelectionListener(this);

        this.CMDREMOVEALL = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVEALL.setText("All");
        this.CMDREMOVEALL.addSelectionListener(this);

        Command select = new Command(this.shell, Command.COMMANDGROUP,0);
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

        this.CMDADD = new Command(this.shell, Command.GENERAL, 0);
        this.CMDADD.setText("Add");
        this.CMDADD.addSelectionListener(this);

        Command set = new Command(this.shell, Command.COMMANDGROUP,0);
        set.setText("Set");

        this.CMDSET_IMAGE = new Command(set, Command.GENERAL,0);
        this.CMDSET_IMAGE.setText("Image");
        this.CMDSET_IMAGE.addSelectionListener(this);

        this.CMDSET_TRANSIMAGE = new Command(set, Command.GENERAL,0);
        this.CMDSET_TRANSIMAGE.setText("Transparent Image");
        this.CMDSET_TRANSIMAGE.addSelectionListener(this);

        this.CMDSET_EMPTY = new Command(set, Command.GENERAL,0);
        this.CMDSET_EMPTY.setText("Empty");
        this.CMDSET_EMPTY.addSelectionListener(this);

        this.CMDSET_LONG = new Command(set, Command.GENERAL,0);
        this.CMDSET_LONG.setText("Long text");
        this.CMDSET_LONG.addSelectionListener(this);

        this.CMDSET_SPECIAL = new Command(set, Command.GENERAL,0);
        this.CMDSET_SPECIAL.setText("Special chars");
        this.CMDSET_SPECIAL.addSelectionListener(this);

        Command setSelection = new Command(this.shell, Command.COMMANDGROUP,0);
        setSelection.setText("Set Selection");
        this.CMDSETSELECT = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECT.setText("Focused");
        this.CMDSETSELECT.addSelectionListener(this);

        this.CMDSETSELECTALL = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECTALL.setText("All");
        this.CMDSETSELECTALL.addSelectionListener(this);

        this.CMDSETSELECTINDECS = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECTINDECS.setText("Deselect");
        this.CMDSETSELECTINDECS.addSelectionListener(this);

        this.CMDSETSELECTSTRING = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECTSTRING.setText("Deselect all");
        this.CMDSETSELECTSTRING.addSelectionListener(this);
    }

    /**
     * create a listview on the shell
     * @param type the typ of the listbox
     */
    private void createListView(int type, int layoutstyle, boolean image)
    {
        // dispose previous listview
        if (this.listView != null && !this.listView.isDisposed())
            this.listView.dispose();
        // create the listview
        this.listView = new ListView(this.shell, type | SWT.BORDER | SWT.V_SCROLL,layoutstyle);
        if (image)
        {
            for (int i = 0; i < Integer.parseInt(this.itemNum.getText()); i++)
                this.listView.add("Item "+(i+1), this.smallImage);
        }
        else
        {
            for (int i = 0; i < Integer.parseInt(this.itemNum.getText()); i++)
                this.listView.add("Item "+(i+1), null);
        }
        this.listView.addSelectionListener(this);
        this.listView.setLayoutData(new RowData(this.shell.getBounds().width,this.shell.getBounds().height-(this.itemNum.getBounds().height*2)-30));
        this.listView.pack();
        Menu mainmenu = new Menu(this.listView);
        this.cmdSingle = new MenuItem(mainmenu, SWT.PUSH);
        cmdSingle.setText("Single");
        cmdSingle.addSelectionListener(this);
        this.cmdMulti = new MenuItem(mainmenu, SWT.PUSH);
        cmdMulti.setText("Multi");
        cmdMulti.addSelectionListener(this);
        this.listView.setMenu(mainmenu);

        this.shell.layout();
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command || event.widget instanceof MenuItem)
        {
            if (event.widget == this.CMDSINGLE || event.widget == this.cmdSingle)
            {
                // create a single list
                this.createListView(SWT.SINGLE, ListView.LOW,false);
                return;
            }
            if (event.widget == this.CMDMULTI || event.widget == this.cmdMulti)
            {
                // create a multi list
                this.createListView(SWT.MULTI, ListView.LOW,false);
                return;
            }
            if (event.widget == this.CMDSINGLE_IMAGE)
            {
                // create a single list
                this.createListView(SWT.SINGLE, ListView.LOW,true);
                return;
            }
            if (event.widget == this.CMDMULTI_IMAGE)
            {
                // create a multi list
                this.createListView(SWT.MULTI, ListView.LOW,true);
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
            if (listView != null)
            {
                int index = this.listView.getFocusIndex();
                int indeces[] = this.listView.getSelectionIndices();

                if (event.widget == this.CMDDENSITY_LOW)
                {
                    // enable text control
                    this.listView.setLayoutDensity(ListView.LOW);
                    return;
                }
                if (event.widget == this.CMDDENSITY_MEDIUM)
                {
                    // enable text control
                    this.listView.setLayoutDensity(ListView.MEDIUM);
                    return;
                }
                if (event.widget == this.CMDDENSITY_HIGH)
                {
                    // enable text control
                    this.listView.setLayoutDensity(ListView.HIGH);
                    return;
                }
                if (event.widget == this.CMDREMOVE)
                {
                    // remove items
                    if (this.listView.getStyle() == SWT.SINGLE)
                        this.listView.remove(index);
                    else
                        this.listView.remove(indeces);

                    return;
                }
                if (event.widget == this.CMDREMOVELIST)
                {
                    // remove items
                    if (this.listView.getItemCount() > 5)
                        this.listView.remove(0, 4);

                    return;
                }
                if (event.widget == this.CMDREMOVEALL)
                {
                    // remove all items
                    this.listView.removeAll();
                    return;
                }
                if (event.widget == this.CMDSELECT)
                {
                    // select items
                    this.listView.select(index);
                    return;
                }
                if (event.widget == this.CMDSELECTALL)
                {
                    // select all items
                    this.listView.selectAll();
                    return;
                }
                if (event.widget == this.CMDDESELECT)
                {
                    // deselect items
                    this.listView.deselect(index);
                    return;
                }
                if (event.widget == this.CMDDESELECTALL)
                {
                    // deselect all items
                    this.listView.deselectAll();
                    return;
                }
                if (event.widget == this.CMDADD)
                {
                    // add items
                    this.listView.add("Added",null,index);

                    return;
                }
                if (event.widget == this.CMDSET_LONG)
                {
                    // set long text items
                    if (this.listView.getStyle() == SWT.SINGLE)
                        this.listView.setItem(index,"Item's text is changed to long so wrapping can be tested",listView.getIcon(index));
                    else
                    {
                        for (int i = 0; i < indeces.length; i++)
                            this.listView.setItem(indeces[i], "Item's text is changed to long so wrapping can be tested", listView.getIcon(indeces[i]));
                    }
                    return;
                }
                if (event.widget == this.CMDSET_EMPTY)
                {
                    // set empty text items
                    if (this.listView.getStyle() == SWT.SINGLE)
                        this.listView.setItem(index,"",listView.getIcon(index));
                    else
                    {
                        for (int i = 0; i < indeces.length; i++)
                            this.listView.setItem(indeces[i], "", listView.getIcon(indeces[i]));
                    }
                    return;
                }
                if (event.widget == this.CMDSET_SPECIAL)
                {
                    // set special text items
                    if (this.listView.getStyle() == SWT.SINGLE)
                        this.listView.setItem(index,"? \n \t %&@#£$ !!!",listView.getIcon(index));
                    else
                    {
                        for (int i = 0; i < indeces.length; i++)
                            this.listView.setItem(indeces[i], "?%&@#£$ \n \t !!!", listView.getIcon(indeces[i]));
                    }
                    return;
                }

                if (event.widget == this.CMDSET_IMAGE)
                {
                    // set images items
                    if (this.listView.getStyle() == SWT.SINGLE)
                        this.listView.setItem(index,listView.getItem(index),this.largeImage);
                    else
                    {
                        for (int i = 0; i < indeces.length; i++)
                            this.listView.setItem(indeces[i], listView.getItem(indeces[i]), this.largeImage);
                    }
                    return;
                }
                if (event.widget == this.CMDSET_TRANSIMAGE)
                {
                    // set images items
                    if (this.listView.getStyle() == SWT.SINGLE)
                        this.listView.setItem(index,listView.getItem(index),this.transImage);
                    else
                    {
                        for (int i = 0; i < indeces.length; i++)
                            this.listView.setItem(indeces[i], listView.getItem(indeces[i]), this.transImage);
                    }
                    return;
                }
            }
        }
        else if (event.widget instanceof ListView)
        {
            updateToolTip("Sel: "+ this.listView.getItem(this.listView.getFocusIndex()));
        }
        super.widgetSelected(event);

    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {

        updateToolTip("Def: "+this.listView.getItem(this.listView.getFocusIndex()));

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
        this.tooltip.setLocation(5, 5);

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

    public void screenActivated(ScreenEvent event)
    {
    }
    public void screenDeactivated(ScreenEvent event)
    {
    }
    public void screenOrientationChanged(ScreenEvent event)
    {
        if (this.listView != null)
        {
            this.listView.setLayoutData(new RowData(this.shell.getBounds().width,this.shell.getBounds().height-(this.itemNum.getBounds().height*2)-30));
        }
        if (this.tooltip != null)
        {
            this.tooltip.setLocation(5, 5);
        }
        this.shell.layout();
    }
}
