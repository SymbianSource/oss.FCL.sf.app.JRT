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
package com.nokia.test.java.eswt.table;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class TableTestMidlet_01 extends TestBaseMIDlet
{

    private Table table = null;

    /** small image */
    private Image smallimg = null;
    /** big image */
    private Image bigimg = null;
    /** transparent image */
    private Image transimg = null;
    /** gif image */

    private Shell tooltip = null;
    /** commands */
    protected Command cmdSingleTable    = null;
    protected Command cmdMultiTable     = null;
    protected Command cmdAlignSingleTable   = null;
    protected Command cmdAlignMultiTable    = null;
    protected Command cmdImageSingleTable   = null;
    protected Command cmdImageMultiTable    = null;
    protected Command cmdLargeMultiTable        = null;
    protected Command cmdHideTable      = null;
    protected Command cmdLargeSingleTable       = null;
    protected Command cmdHeader         = null;
    protected Command cmdLines          = null;
    protected Command cmdSetCheck       = null;
    protected Command cmdClear              = null;
    private Command cmdRemove               = null;
    private Command cmdRemoveAll            = null;
    private Command cmdRemoveAllButFirst    = null;

    private Command cmdSetColor         = null;

    /** Aligment menu commands */
    private Command cmdAligmentLEFT   = null;
    private Command cmdAligmentRIGHT  = null;
    private Command cmdAligmentCENTER = null;

    private MenuItem cmdSingle;

    private MenuItem cmdMulti;

    /**
     * show a messagebox with the thrown exception
     * @param the text to be displayed in the messagebox
     */
    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }

    protected void init()
    {
        // customize the shell
        this.shell.setText("Table Tests");
        // create and set a layout
        this.shell.setLayout(new GridLayout(2, false));
        this.createToolTip("");
        // try to initialize the images
        try
        {
            this.smallimg = new Image(this.display, this.getClass().getResourceAsStream("/res/sphere.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Small image could not be loaded");
            this.smallimg = null;
        }
        try
        {
            this.bigimg = new Image(this.display, this.getClass().getResourceAsStream("/res/small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Big image could not be loaded");
            this.bigimg = null;
        }
        // try to initialize the images
        try
        {
            this.transimg = new Image(this.display, this.getClass().getResourceAsStream("/res/alphatest_small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Transparent image could not be loaded");
            this.transimg = null;
        }

        // "singletable" left softkey command
        this.cmdSingleTable = new Command(this.shell, Command.SELECT, 2);
        cmdSingleTable.setText("Single Table");
        cmdSingleTable.addSelectionListener(this);
        // "multitable" creation command
        this.cmdMultiTable = new Command(this.shell, Command.SELECT, 1);
        cmdMultiTable.setText("Multi Table");
        cmdMultiTable.addSelectionListener(this);
        // table with hide creation command
        this.cmdHideTable = new Command(this.shell, Command.SELECT, 0);
        cmdHideTable.setText("Table with Hide");
        cmdHideTable.addSelectionListener(this);
        // large "singletable" left softkey command
        this.cmdLargeSingleTable = new Command(this.shell, Command.SELECT, 2);
        cmdLargeSingleTable.setText("Large Single Table");
        cmdLargeSingleTable.addSelectionListener(this);
        // large "multitable" left softkey command
        this.cmdLargeMultiTable = new Command(this.shell, Command.SELECT, 1);
        cmdLargeMultiTable.setText("Large Multi Table");
        cmdLargeMultiTable.addSelectionListener(this);
        // large "singletable" left softkey command
        this.cmdAlignSingleTable = new Command(this.shell, Command.SELECT, 2);
        cmdAlignSingleTable.setText("Align Single Table");
        cmdAlignSingleTable.addSelectionListener(this);
        // large "multitable" left softkey command
        this.cmdAlignMultiTable = new Command(this.shell, Command.SELECT, 1);
        cmdAlignMultiTable.setText("Align Multi Table");
        cmdAlignMultiTable.addSelectionListener(this);
        // image "singletable" left softkey command
        this.cmdImageSingleTable = new Command(this.shell, Command.SELECT, 2);
        cmdImageSingleTable.setText("Image Single Table");
        cmdImageSingleTable.addSelectionListener(this);
        // image "multitable" left softkey command
        this.cmdImageMultiTable = new Command(this.shell, Command.SELECT, 1);
        cmdImageMultiTable.setText("Image Multi Table");
        cmdImageMultiTable.addSelectionListener(this);


        // command
        this.cmdHeader = new Command(this.shell, Command.SELECT, 0);
        cmdHeader.setText("Change Header Visibility");
        cmdHeader.addSelectionListener(this);
        //  command
        this.cmdLines = new Command(this.shell, Command.SELECT, 0);
        cmdLines.setText("Change Lines Visibility");
        cmdLines.addSelectionListener(this);

        //  command
        this.cmdSetColor = new Command(this.shell, Command.SELECT, 0);
        cmdSetColor.setText("Change color");
        cmdSetColor.addSelectionListener(this);

        //  command
        this.cmdSetCheck = new Command(this.shell, Command.SELECT, 0);
        cmdSetCheck.setText("Change Check");
        cmdSetCheck.addSelectionListener(this);


        // Aligment submenu
        Command cmdAligment = new Command(this.shell, Command.COMMANDGROUP, 4);
        cmdAligment.setText("Aligment");

        this.cmdAligmentLEFT = new Command(cmdAligment, Command.GENERAL, 2);
        this.cmdAligmentLEFT.setText("LEFT");
        this.cmdAligmentLEFT.addSelectionListener(this);

        this.cmdAligmentCENTER = new Command(cmdAligment, Command.GENERAL, 1);
        this.cmdAligmentCENTER.setText("CENTER");
        this.cmdAligmentCENTER.addSelectionListener(this);

        this.cmdAligmentRIGHT = new Command(cmdAligment, Command.GENERAL, 0);
        this.cmdAligmentRIGHT.setText("RIGHT");
        this.cmdAligmentRIGHT.addSelectionListener(this);

        // Aligment submenu
        Command cmdRemoveMenu = new Command(this.shell, Command.COMMANDGROUP, 4);
        cmdRemoveMenu.setText("Remove");

        this.cmdRemove = new Command(cmdRemoveMenu, Command.GENERAL, 2);
        this.cmdRemove.setText("Selected");
        this.cmdRemove.addSelectionListener(this);

        this.cmdRemoveAll = new Command(cmdRemoveMenu, Command.GENERAL, 1);
        this.cmdRemoveAll.setText("Remove All");
        this.cmdRemoveAll.addSelectionListener(this);

        this.cmdRemoveAllButFirst = new Command(cmdRemoveMenu, Command.GENERAL, 0);
        this.cmdRemoveAllButFirst.setText("Remove All but first");
        this.cmdRemoveAllButFirst.addSelectionListener(this);

        //  command
        this.cmdClear = new Command(this.shell, Command.SELECT, 1);
        cmdClear.setText("Clear Shell");
        cmdClear.addSelectionListener(this);

    }

    /**
     * Cleanup the screen.
     * Removes all widget objects which are currently presented
     * on the shell.
     */
    private void shellCleanup()
    {

        if (this.shell == null || this.shell.isDisposed()) return;

        Control[] controls = this.shell.getChildren();

        for (int i=0; i<controls.length; i++)
        {
            controls[i].dispose();
        }

        this.shell.update();
    }

    private void createTable(int style, int rows, int cols, boolean pack)
    {

        if (pack && style == SWT.MULTI)
        {
            table = new Table(this.shell,style|SWT.CHECK);
        }
        else
        {
            table = new Table(this.shell,style);
        }
        table.setSize(new Point(this.shell.getClientArea().width, this.shell.getClientArea().height/2));

        final int ROWS = rows;
        final int COLS = cols;
        for (int i = 0; i < ROWS; ++i)
        {
            new TableItem(table, 0);
        }
        for (int c = 0; c < COLS; ++c)
        {
            TableColumn column = new TableColumn(table, 0);
            column.setText("Column " + c);
            for (int i = 0; i < ROWS; ++i)
            {
                table.getItem(i).setText(c, "Item " + c + ", " + i);
            }
            if (pack)
                column.pack();
            else
            {
                column.setWidth((int)(this.shell.getClientArea().width/2));
                column.setAlignment(SWT.CENTER);
            }
        }
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        if (style == SWT.SINGLE)
        {
            table.addSelectionListener(this);
        }
        Menu mainmenu = new Menu(this.table);
        this.cmdSingle = new MenuItem(mainmenu, SWT.PUSH);
        cmdSingle.setText("Single");
        cmdSingle.addSelectionListener(this);
        this.cmdMulti = new MenuItem(mainmenu, SWT.PUSH);
        cmdMulti.setText("Multi");
        cmdMulti.addSelectionListener(this);
        this.table.setMenu(mainmenu);

        this.shell.layout();
        this.shell.update();
    }
    private void createImageTable(int style, int rows, int cols, boolean pack)
    {
        table = new Table(this.shell,style);
        table.setSize(new Point(this.shell.getClientArea().width, this.shell.getClientArea().height));

        final int ROWS = rows;
        final int COLS = cols;
        for (int i = 0; i < ROWS; ++i)
        {
            new TableItem(table, 0);
        }
        for (int c = 0; c < COLS; ++c)
        {
            TableColumn column = new TableColumn(table, 0);
            column.setText("Column " + c);
            for (int i = 0; i < ROWS; ++i)
            {
                table.getItem(i).setText(c, "Item " + c + ", " + i);
                if (pack)
                    table.getItem(i).setImage(this.smallimg);
                else
                {
                    table.getItem(i).setImage(this.bigimg);
                }
            }
            if (pack)
                column.pack();
            else
            {
                column.setWidth((int)(this.shell.getClientArea().width/2));
                column.setAlignment(SWT.CENTER);
            }
        }
        table.getItem(0).setImage(this.transimg);
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        table.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));

        this.shell.layout();
        this.shell.update();
    }


    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command || event.widget instanceof MenuItem)
        {
            this.tooltip.setVisible(false);
            try
            {
                if (event.widget == this.cmdSingleTable || event.widget == this.cmdSingle)
                {
                    createTable(SWT.SINGLE,5,2,true);
                }
                else if (event.widget == this.cmdMultiTable || event.widget == this.cmdMulti)
                {
                    createTable(SWT.MULTI,5,2,true);
                }
                else if (event.widget == this.cmdLargeMultiTable)
                {
                    createTable(SWT.MULTI,50,20,true);
                }
                else if (event.widget == this.cmdLargeSingleTable)
                {
                    createTable(SWT.SINGLE,50,20,true);
                }
                else if (event.widget == this.cmdHideTable)
                {
                    createTable(SWT.HIDE_SELECTION,5,2,true);
                }
                else if (event.widget == this.cmdAlignMultiTable)
                {
                    createTable(SWT.MULTI,10,2,false);
                }
                else if (event.widget == this.cmdAlignSingleTable)
                {
                    createTable(SWT.SINGLE,10,2,false);
                }
                else if (event.widget == this.cmdImageSingleTable)
                {
                    createImageTable(SWT.SINGLE,5,2,false);
                }
                else if (event.widget == this.cmdImageMultiTable)
                {
                    createImageTable(SWT.MULTI,5,2,true);
                }
                else if (event.widget == this.cmdHeader)
                {
                    if (table.getHeaderVisible())
                        table.setHeaderVisible(false);
                    else
                        table.setHeaderVisible(true);
                }
                else if (event.widget == this.cmdLines)
                {
                    if (table.getLinesVisible())
                        table.setLinesVisible(false);
                    else
                        table.setLinesVisible(true);
                }
                else if (event.widget == this.cmdAligmentLEFT)
                {
                    table.getColumn(0).setAlignment(SWT.LEFT);
                    table.getColumn(1).setAlignment(SWT.LEFT);
                }
                else if (event.widget == this.cmdAligmentCENTER)
                {
                    table.getColumn(0).setAlignment(SWT.CENTER);
                    table.getColumn(1).setAlignment(SWT.CENTER);
                }
                else if (event.widget == this.cmdAligmentRIGHT)
                {
                    table.getColumn(0).setAlignment(SWT.RIGHT);
                    table.getColumn(1).setAlignment(SWT.RIGHT);
                }
                else if (event.widget == this.cmdClear)
                {
                    this.shellCleanup();
                }
                else if (event.widget == this.cmdRemove)
                {
                    if (table.getSelectionCount() <= 1)
                    {
                        if (table.getSelectionIndex() != -1)
                            table.remove(table.getSelectionIndex());
                    }
                    else
                    {
                        table.remove(table.getSelectionIndices());
                    }
                }
                else if (event.widget == this.cmdRemoveAll)
                {
                    table.removeAll();
                }
                else if (event.widget == this.cmdRemoveAllButFirst)
                {
                    table.remove(1,table.getItemCount()-1);
                }
                else if (event.widget == this.cmdSetCheck)
                {

                    for (int i = 0; i < table.getItems().length; i++)
                    {
                        if (!table.getItem(0).getChecked())
                            table.getItems()[i].setChecked(true);
                        else
                            table.getItems()[i].setChecked(false);
                    }
                }
                else if (event.widget == this.cmdSetColor)
                {
                    if (table.getSelectionCount() <= 1)
                    {
                        if (table.getSelectionIndex() != -1)
                            table.getItem((table.getSelectionIndex())).setBackground(Display.getDefault().getSystemColor(SWT.COLOR_YELLOW));
                        table.getItem((table.getSelectionIndex())).setForeground(Display.getDefault().getSystemColor(SWT.COLOR_RED));
                    }
                    else
                    {
                        for (int i= 0; i < table.getSelectionIndices().length; i++)
                        {
                            table.getItem(table.getSelectionIndices()[i]).setBackground(Display.getDefault().getSystemColor(SWT.COLOR_GREEN));
                            table.getItem(table.getSelectionIndices()[i]).setForeground(Display.getDefault().getSystemColor(SWT.COLOR_WHITE));
                        }
                    }
                }

            }
            catch (Exception ex)
            {
                // display the exception
                MessageBox msg = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                msg.setMessage(ex.getMessage());
                msg.open();
                return;
            }
        }
        else if (event.widget instanceof Table)
        {
            try
            {
                Table table = (Table)event.widget;
                updateToolTip("Sel: "+ table.getItem(table.getSelectionIndex()).getText());
            }
            catch (Exception ex)
            {
                // display the exception
                MessageBox msg = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                msg.setMessage(ex.getMessage());
                msg.open();
                return;
            }
        }
        super.widgetSelected(event);

    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {
        try
        {
            if (event.widget instanceof Table)
            {
                Table table = (Table)event.widget;
                updateToolTip("Def: "+table.getItem(table.getSelectionIndex()).getText());
            }
        }
        catch (Exception ex)
        {
            // display the exception
            MessageBox msg = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            msg.setMessage(ex.getMessage());
            msg.open();
            return;
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




}
