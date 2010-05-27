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

import org.eclipse.swt.SWT;

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.SortedList;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class SortedListTestMIDlet_02 extends TestBaseMIDlet
{

    /** */
    protected SortedList list = null;
    /** */
    protected String[] itemStrings =
    {
        "ABC", "dsgwew", "abdfsd", "zadsvd",
        "CBA", "sasvds", "bmorbp", "vdbdfs",
        "KSA", "wbfdps", "xdpvsv", "jsavfd",
        "007", "123954", "937205", "295462"
    };
    /** */
    protected Text field = null;

    /** Commands */
    protected Command cmdSingleTest     = null;
    protected Command cmdMultiTest      = null;

    protected Command cmdAdd            = null;
    protected Command cmdRemoveSelected = null;
    protected Command cmdRemoveAll      = null;
    protected Command cmdSetDefaults    = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // set the shell title
        this.shell.setText("SortedList Test");

        // create test case selection commands
        this.cmdSingleTest = new Command(this.shell, Command.GENERAL, 1);
        this.cmdSingleTest.setText("Single");
        this.cmdSingleTest.addSelectionListener(this);

        this.cmdMultiTest = new Command(this.shell, Command.GENERAL, 0);
        this.cmdMultiTest.setText("Multi");
        this.cmdMultiTest.addSelectionListener(this);
    }

    /**
     * Testcase setup.
     * Cleanup the screen and initialize test case for
     * specified list type (Sinlge\Multi).
     */
    protected void setup(int type)
    {
        // cleanup the shell
        this.cleanup();
        this.shell.update();

        // create and set layout
        GridLayout layout = new GridLayout(1, true);

        this.shell.setLayout(layout);

        // create commands
        this.cmdAdd = new Command(this.shell, Command.GENERAL, 3);
        this.cmdAdd.setText("Add");
        this.cmdAdd.addSelectionListener(this);

        Command cmdRemove = new Command(this.shell, Command.COMMANDGROUP, 2);
        cmdRemove.setText("Remove");
        this.cmdRemoveSelected = new Command(cmdRemove, Command.SELECT, 0);
        this.cmdRemoveSelected.setText("Selected");
        this.cmdRemoveSelected.addSelectionListener(this);
        this.cmdRemoveAll = new Command(cmdRemove, Command.SELECT, 0);
        this.cmdRemoveAll.setText("All");
        this.cmdRemoveAll.addSelectionListener(this);

        Command cmdSet = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdSet.setText("Set");
        this.cmdSetDefaults = new Command(cmdSet, Command.SELECT, 0);
        this.cmdSetDefaults.setText("Defaults");
        this.cmdSetDefaults.addSelectionListener(this);

        this.list = new SortedList(this.shell, type | SWT.BORDER | SWT.V_SCROLL, SortedList.FILTER);
        GridData listData = new GridData(SWT.FILL, SWT.FILL, true, true);
        this.list.setLayoutData(listData);
        // create Text field and SortedList (default values)
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("List Item: ");
        GridData ldata = new GridData(SWT.LEFT, SWT.CENTER, false, false);
        label.setLayoutData(ldata);

        this.field = new Text(this.shell, SWT.BORDER);
        GridData fdata = new GridData(SWT.FILL, SWT.CENTER, true, false);
        this.field.setLayoutData(fdata);
        this.list.setItems(this.itemStrings);

        // layout and update the shell
        this.shell.layout();
        this.shell.update();
    }

    /**
     * Cleanup.
     */
    private void cleanup()
    {
        // screen cleanup
        Control[] controls = this.shell.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            if (controls[i] != null && !controls[i].isDisposed())
                controls[i].dispose();
        }

        // Options menu cleanup
        if (this.cmdSingleTest != null &&
                !this.cmdSingleTest.isDisposed())
        {
            this.cmdSingleTest.dispose();
        }

        if (this.cmdMultiTest != null &&
                !this.cmdMultiTest.isDisposed())
        {
            this.cmdMultiTest.dispose();
        }
    }

    /*
     *  (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {

        // test case selection
        if (event.widget == this.cmdSingleTest)
        {
            this.setup(SWT.SINGLE);
            return;
        }
        if (event.widget == this.cmdMultiTest)
        {
            this.setup(SWT.MULTI);
            return;
        }

        // test case relevant actions
        if (event.widget == this.cmdAdd)
        {
            if (this.list == null ||
                    this.list.isDisposed() ||
                    this.field == null)
            {
                return;
            }

            this.list.add(this.field.getText());
            GridData listData = new GridData(SWT.FILL, SWT.FILL, true, true);
            this.list.setLayoutData(listData);
            return;
        }
        if (event.widget == this.cmdRemoveSelected)
        {
            if (this.list == null || this.list.isDisposed())
            {
                return;
            }

            String[] selected = this.list.getSelection();
            for (int i=0; i<selected.length; i++)
            {
                this.list.remove(selected[i]);
            }
            return;
        }
        if (event.widget == this.cmdRemoveAll)
        {
            if (this.list == null || this.list.isDisposed())
            {
                return;
            }

            this.list.removeAll();
            return;
        }
        if (event.widget == this.cmdSetDefaults)
        {
            if (this.list == null || this.list.isDisposed())
            {
                return;
            }

            this.list.setItems(this.itemStrings);
            return;
        }

        // exit or ignore
        super.widgetSelected(event);
    }
}
