/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.test.java.eswt.sortedlist;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

import java.util.Random;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.SortedList;

/**
 * The midlet is used to examine creation and browsing the content
 * of SortedList component. It is also used to check a scroll bar
 * appearance in case if the List's content doesn't fit the control bounds.
 *
 */
public class SortedListTestMIDlet_01 extends TestBaseMIDlet
{

    /** currently used list style */
    protected int order_style   = SWT.NONE;
    protected int type_style    = SWT.NONE;
    protected int mode_style    = SWT.NONE;

    /** sorted list */
    protected SortedList list   = null;

    private Shell tooltip = null;
    /** commands */
    protected Command cmdSetOrderASC        = null;
    protected Command cmdSetOrderDESC   = null;
    protected Command cmdSetTypeSINGLE  = null;
    protected Command cmdSetTypeMULTI   = null;
    protected Command cmdSetFilter      = null;

    protected Command cmdEnableText     = null;
    protected Command cmdDisableText    = null;

    protected Command cmdCreate = null;

    private MenuItem itemCreate = null;
    private MenuItem itemSet = null;

    /** a Text field used to define the number of items in the list */
    protected ConstrainedText itemCount = null;

    /** an array of chars used to generate random strings */
    protected char[] charArray =
    {
        'A','a','B','b','C','c','D','d','E','e',
        'F','f','G','g','H','h','I','i','J','j',
        'K','k','L','l','M','m','N','n','O','o',
        'P','p','Q','q','R','r','S','s','T','t',
        'U','u','V','v','W','w','X','x','Y','y',
        'Z','z'
    };


    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // set the shell title
        this.shell.setText("SortedList Test");

        // create and set a layout
        this.shell.setLayout(new GridLayout(2, false));

        this.createToolTip("");
        // create the CREATE command
        this.cmdCreate = new Command(this.shell, Command.SELECT, 0);
        this.cmdCreate.setText("Create");
        this.cmdCreate.addSelectionListener(this);

        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdSettings.setText("Settings");

        Command cmdSetType = new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdSetType.setText("Type");

        this.cmdSetTypeSINGLE = new Command(cmdSetType, Command.GENERAL, 1);
        this.cmdSetTypeSINGLE.setText("SINGLE");
        this.cmdSetTypeSINGLE.addSelectionListener(this);

        this.cmdSetTypeMULTI = new Command(cmdSetType, Command.GENERAL, 0);
        this.cmdSetTypeMULTI.setText("MULTI");
        this.cmdSetTypeMULTI.addSelectionListener(this);

        Command cmdSetOrder = new Command(cmdSettings, Command.COMMANDGROUP, 0);
        cmdSetOrder.setText("Order");

        this.cmdSetOrderASC = new Command(cmdSetOrder, Command.GENERAL, 1);
        this.cmdSetOrderASC.setText("ASC");
        this.cmdSetOrderASC.addSelectionListener(this);

        this.cmdSetOrderDESC = new Command(cmdSetOrder, Command.GENERAL, 0);
        this.cmdSetOrderDESC.setText("DESC");
        this.cmdSetOrderDESC.addSelectionListener(this);

        this.cmdSetFilter = new Command(this.shell, Command.SELECT, 0);
        this.cmdSetFilter.setText("Set Filter");
        this.cmdSetFilter.addSelectionListener(this);

        this.cmdEnableText = new Command(this.shell, Command.SELECT, 0);
        this.cmdEnableText.setText("Enable Text");
        this.cmdEnableText.addSelectionListener(this);

        this.cmdDisableText = new Command(this.shell, Command.SELECT, 0);
        this.cmdDisableText.setText("Disable Text");
        this.cmdDisableText.addSelectionListener(this);

        // create text field for defining the number of items in the list
        Label nlabel = new Label(this.shell, SWT.NONE);
        nlabel.setText("Items amount: ");
        GridData nldata = new GridData();
        nldata.horizontalAlignment = SWT.RIGHT;
        nlabel.setLayoutData(nldata);

        this.itemCount = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.itemCount.setTextLimit(2);
        this.itemCount.setText(String.valueOf(0));
    }

    /**
     * Create new SortedList with user defined settings.
     * Before creating a new list, previously created is disposed.
     */
    protected void createList()
    {
        // dispose previously created list
        if (this.list != null && !this.list.isDisposed())
        {
            this.list.dispose();
        }

        // create new list
        int count = Integer.parseInt(this.itemCount.getText());
        int length = 4;//length of random string

        String[] items = new String[count];
        for (int i=0; i<count; i++)
        {
            items[i] = String.valueOf(getRandomString(i, length));
        }

        int style =
            SWT.BORDER | SWT.V_SCROLL | this.type_style | this.order_style;

        if (this.mode_style == SortedList.FILTER)
            this.list = new SortedList(this.shell, style, mode_style);
        else
            this.list = new SortedList(this.shell, style);

        GridData data = new GridData();
        data.horizontalSpan = 2;
        data.grabExcessHorizontalSpace  = true;
        data.grabExcessVerticalSpace    = true;
        data.horizontalAlignment        = SWT.CENTER;
        data.horizontalAlignment        = SWT.CENTER;
        this.list.setLayoutData(data);

        this.list.setItems(items);
        this.list.pack();
        this.list.addSelectionListener(this);

        Menu mainmenu = new Menu(this.list);
        this.itemCreate = new MenuItem(mainmenu, SWT.PUSH);
        itemCreate.setText("Create");
        itemCreate.addSelectionListener(this);
        this.itemSet = new MenuItem(mainmenu, SWT.PUSH);
        itemSet.setText("Set");
        itemSet.addSelectionListener(this);
        this.list.setMenu(mainmenu);

        this.shell.pack();
        this.shell.layout();
        this.shell.update();
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        // create command
        if (event.widget == this.cmdCreate || event.widget == this.itemCreate)
        {
            createList();
        }

        // config commands
        if (event.widget == this.cmdSetTypeSINGLE)
        {
            this.type_style = SWT.SINGLE;
            return;
        }
        if (event.widget == this.cmdSetTypeMULTI)
        {
            this.type_style = SWT.MULTI;
            return;
        }
        if (event.widget == this.cmdSetOrderASC)
        {
            this.order_style = SWT.UP;
            return;
        }
        if (event.widget == this.cmdSetOrderDESC)
        {
            this.order_style = SWT.DOWN;
            return;
        }
        if (event.widget == this.cmdSetFilter)
        {
            this.mode_style = SortedList.FILTER;
            return;
        }
        if (event.widget == this.cmdEnableText)
        {
            this.itemCount.setEnabled(true);
            return;
        }
        if (event.widget == this.cmdDisableText)
        {
            this.itemCount.setEnabled(false);
            return;
        }
        else if (event.widget instanceof SortedList)
        {
            updateToolTip("Sel: "+ this.list.getFocus());
        }
        super.widgetSelected(event);

    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {

        updateToolTip("Def: "+this.list.getFocus());
        super.widgetDefaultSelected(event);

    }




    /**
     * Create a rundom string from char array.
     * @param length The length of result string
     * @return generated random string
     */
    protected String getRandomString(int seed, int length)
    {
        String result = null;
        char[] chars = new char[length];

        for (int i=0; i<length; i++)
        {
            Random rand = new Random(System.currentTimeMillis() + seed + i);
            int index = (rand.nextInt() >>> 1) % (this.charArray.length);
            chars[i] = this.charArray[index];
        }
        result = new String(chars);

        return result;
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
        this.tooltip.setLocation(5,5);

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