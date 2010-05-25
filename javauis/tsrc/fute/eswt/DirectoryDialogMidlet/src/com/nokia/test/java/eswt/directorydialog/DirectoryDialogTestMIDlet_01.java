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

package com.nokia.test.java.eswt.directorydialog;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 */
public class DirectoryDialogTestMIDlet_01 extends TestBaseMIDlet
{

    /** setting fields */
    protected Text fieldTitle           = null;
    protected Text fieldMessage     = null;
    protected Text fieldFilterPath  = null;

    /** commands */
    protected Command cmdSetTitleDEFINE     = null;
    protected Command cmdSetTitleNULL           = null;

    protected Command cmdSetMessageDEFINE       = null;
    protected Command cmdSetMessageNULL     = null;

    protected Command cmdSetFilterPathDEFINE    = null;
    protected Command cmdSetFilterPathNULL  = null;

    protected Command cmdSetClean               = null;

    protected Command cmdOpen                   = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        this.shell.setText("Directory Dialog Test");
        this.shell.setLayout(new GridLayout(2, false));

        // create commands
        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdSettings.setText("Settings");

        // title commands
        Command cmdSetTitle = new Command(cmdSettings, Command.COMMANDGROUP, 3);
        cmdSetTitle.setText("Title");

        this.cmdSetTitleDEFINE = new Command(cmdSetTitle, Command.GENERAL, 1);
        this.cmdSetTitleDEFINE.setText("define");
        this.cmdSetTitleDEFINE.addSelectionListener(this);

        this.cmdSetTitleNULL = new Command(cmdSetTitle, Command.GENERAL, 0);
        this.cmdSetTitleNULL.setText("null");
        this.cmdSetTitleNULL.addSelectionListener(this);

        // message commands
        Command cmdSetMessage =
            new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdSetMessage.setText("Message");

        this.cmdSetMessageDEFINE =
            new Command(cmdSetMessage, Command.GENERAL, 1);
        this.cmdSetMessageDEFINE.setText("define");
        this.cmdSetMessageDEFINE.addSelectionListener(this);

        this.cmdSetMessageNULL = new Command(cmdSetMessage, Command.GENERAL, 0);
        this.cmdSetMessageNULL.setText("null");
        this.cmdSetMessageNULL.addSelectionListener(this);

        // filter PATH command
        Command cmdSetFilter =
            new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdSetFilter.setText("Filter");

        Command cmdSetFilterPATH =
            new Command(cmdSetFilter, Command.COMMANDGROUP, 0);
        cmdSetFilterPATH.setText("PATH");

        this.cmdSetFilterPathDEFINE =
            new Command(cmdSetFilterPATH, Command.GENERAL, 1);
        this.cmdSetFilterPathDEFINE.setText("define");
        this.cmdSetFilterPathDEFINE.addSelectionListener(this);

        this.cmdSetFilterPathNULL =
            new Command(cmdSetFilterPATH, Command.GENERAL, 0);
        this.cmdSetFilterPathNULL.setText("null");
        this.cmdSetFilterPathNULL.addSelectionListener(this);

        // clean command
        this.cmdSetClean = new Command(cmdSettings, Command.GENERAL, 0);
        this.cmdSetClean.setText("Clean");
        this.cmdSetClean.addSelectionListener(this);

        // open command
        this.cmdOpen = new Command(this.shell, Command.GENERAL, 0);
        this.cmdOpen.setText("Open");
        this.cmdOpen.addSelectionListener(this);
    }

    /**
     * Create and open a DirectoryDialog
     */
    protected void open()
    {
        // create a dialog
        DirectoryDialog dlg = new DirectoryDialog(this.shell);

        // configure the dialog
        if (this.fieldTitle != null && !this.fieldTitle.isDisposed())
        {
            if (this.fieldTitle.isEnabled())
            {
                dlg.setText(this.fieldTitle.getText());
            }
            else
            {
                dlg.setText(null);
            }
        }

        if (this.fieldMessage != null && !this.fieldMessage.isDisposed())
        {
            if (this.fieldMessage.isEnabled())
            {
                dlg.setMessage(this.fieldMessage.getText());
            }
            else
            {
                dlg.setMessage(null);
            }
        }

        if (this.fieldFilterPath != null &&
                !this.fieldFilterPath.isDisposed())
        {
            if (this.fieldFilterPath.isEnabled())
            {
                dlg.setFilterPath(this.fieldFilterPath.getText());
            }
            else
            {
                dlg.setFilterPath(null);
            }
        }

        // open the dialog
        dlg.open();
    }

    /**
     * Clear all previously defined settings
     */
    protected void clean()
    {
        Control[] controls = this.shell.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            if (controls[i] != null && !controls[i].isDisposed())
            {
                controls[i].dispose();
            }
        }

        // recreate a title label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("Settings");
        GridData data = new GridData();
        data.horizontalSpan             = 2;
        data.horizontalAlignment        = SWT.CENTER;
        data.grabExcessHorizontalSpace  = true;
        label.setLayoutData(data);

        // update the shell
        this.shell.layout();
        this.shell.update();
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        try
        {
            // title commands
            if (event.widget == this.cmdSetTitleDEFINE)
            {
                if (this.fieldTitle == null || this.fieldTitle.isDisposed())
                {
                    this.fieldTitle = this.createField("Title: ");
                }
                else if (!this.fieldTitle.isEnabled())
                {
                    this.fieldTitle.setEnabled(true);
                    this.fieldTitle.setText("");
                }
                return;
            }
            if (event.widget == this.cmdSetTitleNULL)
            {
                if (this.fieldTitle == null || this.fieldTitle.isDisposed())
                {
                    this.fieldTitle = this.createField("Title: ");
                }
                this.fieldTitle.setEnabled(false);
                this.fieldTitle.setText("null");
                return;
            }

            // message commands
            if (event.widget == this.cmdSetMessageDEFINE)
            {
                if (this.fieldMessage == null ||
                        this.fieldMessage.isDisposed())
                {
                    this.fieldMessage = this.createField("Message: ");
                }
                else if (!this.fieldMessage.isEnabled())
                {
                    this.fieldMessage.setEnabled(true);
                    this.fieldMessage.setText("");
                }
                return;
            }
            if (event.widget == this.cmdSetMessageNULL)
            {
                if (this.fieldMessage == null ||
                        this.fieldMessage.isDisposed())
                {
                    this.fieldMessage = this.createField("Message: ");
                }
                this.fieldMessage.setEnabled(false);
                this.fieldMessage.setText("null");
                return;
            }

            // filter PATH commands
            if (event.widget == this.cmdSetFilterPathDEFINE)
            {
                if (this.fieldFilterPath == null ||
                        this.fieldFilterPath.isDisposed())
                {
                    this.fieldFilterPath = this.createField("Filter PATH: ");
                }
                else if (!this.fieldFilterPath.isEnabled())
                {
                    this.fieldFilterPath.setEnabled(true);
                    this.fieldFilterPath.setText("");
                }
                return;
            }
            if (event.widget == this.cmdSetFilterPathNULL)
            {
                if (this.fieldFilterPath == null ||
                        this.fieldFilterPath.isDisposed())
                {
                    this.fieldFilterPath = this.createField("Filter PATH: ");
                }
                this.fieldFilterPath.setEnabled(false);
                this.fieldFilterPath.setText("null");
                return;
            }

            // clean command
            if (event.widget == this.cmdSetClean)
            {
                this.clean();
                return;
            }

            // open command
            if (event.widget == this.cmdOpen)
            {
                this.open();
                return;
            }
        }
        catch (Exception ex)
        {
            MessageBox msg = new MessageBox(this.shell);
            msg.setMessage(ex.getMessage());
            msg.open();
        }
        super.widgetSelected(event);
    }

    /**
     * Create and return a Text field with preceding info label
     */
    protected Text createField(String title)
    {
        // create info label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText(title);

        // create a Text field
        Text field = new Text(this.shell, SWT.BORDER);

        // update the shell
        this.shell.layout();
        this.shell.update();

        return field;
    }
}