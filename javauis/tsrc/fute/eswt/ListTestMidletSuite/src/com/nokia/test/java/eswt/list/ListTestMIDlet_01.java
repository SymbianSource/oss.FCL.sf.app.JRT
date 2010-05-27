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

package com.nokia.test.java.eswt.list;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the list component
 * - creating single list
 * - creating multi list
 * - navigating and scrolling in lists
 */
public class ListTestMIDlet_01 extends TestBaseMIDlet
{
    private MenuItem cmdSingle = null;
    private MenuItem cmdMulti = null;
    /** constrainedtext instance - number of items */
    private ConstrainedText itemNum = null;
    /** list instance */
    private List list = null;
    /** */
    private Shell tooltip = null;
    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("List 1");
        this.shell.setLayout(new GridLayout(2, true));

        // "no. of items:" label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("No. of items:");
        label.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
        // input field for "no. of items:"
        this.itemNum = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.itemNum.setTextLimit(2);
        this.itemNum.setText(String.valueOf(0));
        this.itemNum.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
        this.list = new List(this.shell, SWT.SINGLE | SWT.BORDER | SWT.V_SCROLL);
        this.list.setItems(new String[] {"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"});
        this.list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        this.list.addSelectionListener(this);
        this.shell.layout();
        this.createToolTip("");

        // "single list" left softkey command
        Command cmd = new Command(this.shell, Command.SELECT, 1);
        cmd.setText("Single List");
        cmd.addSelectionListener(this);
        // "multi list" left softkey command
        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Multi List");
        cmd.addSelectionListener(this);

        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Enable Text");
        cmd.addSelectionListener(this);

        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Disable Text");
        cmd.addSelectionListener(this);
    }

    /**
     * create a list on the shell
     * @param type the typ of the list
     */
    private void createList(int type)
    {
        try
        {
            // dispose previous list
            if (this.list != null && !this.list.isDisposed())
                this.list.dispose();
            // create the list
            this.list = new List(this.shell, type | SWT.V_SCROLL);
            for (int i = 0; i < Integer.parseInt(this.itemNum.getText()); i++)
                this.list.add("Item "+(i+1));
            this.list.addSelectionListener(this);

            this.list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

            Menu mainmenu = new Menu(this.list);
            this.cmdSingle = new MenuItem(mainmenu, SWT.PUSH);
            cmdSingle.setText("Single List");
            cmdSingle.addSelectionListener(this);
            this.cmdMulti = new MenuItem(mainmenu, SWT.PUSH);
            cmdMulti.setText("Multi List");
            cmdMulti.addSelectionListener(this);
            this.list.setMenu(mainmenu);
            this.shell.layout();
            this.shell.update();
        }
        catch (Exception ex)
        {
            this.updateToolTip(ex.getMessage());
        }
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            this.tooltip.setVisible(false);
            String scmd = ((MenuItem) event.widget).getText();
            if ("Single List".equals(scmd))
            {
                // create a single list
                this.createList(SWT.SINGLE);
                return;
            }
            if ("Multi List".equals(scmd))
            {
                // create a multi list
                this.createList(SWT.MULTI);
                return;
            }
        }
        if (event.widget instanceof Command)
        {
            this.tooltip.setVisible(false);
            String scmd = ((Command) event.widget).getText();
            if ("Single List".equals(scmd))
            {
                // create a single list
                this.createList(SWT.SINGLE);
                return;
            }
            if ("Multi List".equals(scmd))
            {
                // create a multi list
                this.createList(SWT.MULTI);
                return;
            }
            if ("Enable Text".equals(scmd))
            {
                // enable text control
                this.itemNum.setEnabled(true);
                return;
            }
            if ("Disable Text".equals(scmd))
            {
                // enable text control
                this.itemNum.setEnabled(false);
                return;
            }
        }
        else if (event.widget instanceof List)
        {
            updateToolTip("Sel: "+ this.list.getItem(this.list.getFocusIndex()));
        }
        super.widgetSelected(event);

    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {

        updateToolTip("Def: "+this.list.getItem(this.list.getFocusIndex()));

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
