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

package com.nokia.test.java.eswt.querydialog;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/* **
 * this class is intended to test the querydialog component
 *
 */
public class QueryDialogTestMIDlet_01 extends TestBaseMIDlet
{

    /** Setting fields */
    private Text titleField             = null;
    private Text promptField            = null;
    private Text defaultField           = null;
    private ConstrainedText minField    = null;
    private ConstrainedText maxField    = null;

    /** Create commands */
    private Command cmdCreateSTANDARD   = null;
    private Command cmdCreateNUMBER     = null;
    private Command cmdCreateTIME       = null;
    private Command cmdCreateDATE       = null;
    private Command cmdCreatePASSWORD   = null;

    /** Set Title commands */
    private Command cmdSetTitleNULL     = null;
    private Command cmdSetTitleDEFINE   = null;

    /** Set Prompt commands */
    private Command cmdSetPromptNULL    = null;
    private Command cmdSetPromptDEFINE  = null;

    /** Set Default value commands */
    private Command cmdSetDefaultNULL   = null;
    private Command cmdSetDefaultDEFINE = null;

    /** Set Limit commands */
    private Command cmdSetLimitMIN      = null;
    private Command cmdSetLimitMAX      = null;

    /** Clear command */
    private Command cmdClear            = null;


    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        this.shell.setText("Query Dialog Test");

        // create and set a layout
        this.shell.setLayout(new GridLayout(2, false));

        // create Create commands
        Command cmdCreate = new Command(this.shell, Command.COMMANDGROUP, 2);
        cmdCreate.setText("Create");

        this.cmdCreateSTANDARD = new Command(cmdCreate, Command.GENERAL, 4);
        this.cmdCreateSTANDARD.setText("STANDARD");
        this.cmdCreateSTANDARD.addSelectionListener(this);

        this.cmdCreateNUMBER = new Command(cmdCreate, Command.GENERAL, 3);
        this.cmdCreateNUMBER.setText("NUMBER");
        this.cmdCreateNUMBER.addSelectionListener(this);

        this.cmdCreateTIME = new Command(cmdCreate, Command.GENERAL, 2);
        this.cmdCreateTIME.setText("TIME");
        this.cmdCreateTIME.addSelectionListener(this);

        this.cmdCreateDATE = new Command(cmdCreate, Command.GENERAL, 1);
        this.cmdCreateDATE.setText("DATE");
        this.cmdCreateDATE.addSelectionListener(this);

        this.cmdCreatePASSWORD = new Command(cmdCreate, Command.GENERAL, 0);
        this.cmdCreatePASSWORD.setText("PASSWORD");
        this.cmdCreatePASSWORD.addSelectionListener(this);

        // create Set commands
        Command cmdSet = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdSet.setText("Set");

        Command cmdTitle = new Command(cmdSet, Command.COMMANDGROUP, 3);
        cmdTitle.setText("Title");

        this.cmdSetTitleDEFINE = new Command(cmdTitle, Command.GENERAL, 1);
        this.cmdSetTitleDEFINE.setText("define");
        this.cmdSetTitleDEFINE.addSelectionListener(this);

        this.cmdSetTitleNULL = new Command(cmdTitle, Command.GENERAL, 0);
        this.cmdSetTitleNULL.setText("null");
        this.cmdSetTitleNULL.addSelectionListener(this);

        Command cmdPrompt = new Command(cmdSet, Command.COMMANDGROUP, 2);
        cmdPrompt.setText("Prompt");

        this.cmdSetPromptDEFINE = new Command(cmdPrompt, Command.GENERAL, 1);
        this.cmdSetPromptDEFINE.setText("define");
        this.cmdSetPromptDEFINE.addSelectionListener(this);

        this.cmdSetPromptNULL = new Command(cmdPrompt, Command.GENERAL, 0);
        this.cmdSetPromptNULL.setText("null");
        this.cmdSetPromptNULL.addSelectionListener(this);

        Command cmdDefault = new Command(cmdSet, Command.COMMANDGROUP, 1);
        cmdDefault.setText("Default");

        this.cmdSetDefaultDEFINE = new Command(cmdDefault, Command.GENERAL, 1);
        this.cmdSetDefaultDEFINE.setText("define");
        this.cmdSetDefaultDEFINE.addSelectionListener(this);

        this.cmdSetDefaultNULL = new Command(cmdDefault, Command.GENERAL, 0);
        this.cmdSetDefaultNULL.setText("null");
        this.cmdSetDefaultNULL.addSelectionListener(this);

        Command cmdLimit = new Command(cmdSet, Command.COMMANDGROUP, 0);
        cmdLimit.setText("Limit");

        this.cmdSetLimitMIN = new Command(cmdLimit, Command.GENERAL, 1);
        this.cmdSetLimitMIN.setText("min");
        this.cmdSetLimitMIN.addSelectionListener(this);

        this.cmdSetLimitMAX = new Command(cmdLimit, Command.GENERAL, 0);
        this.cmdSetLimitMAX.setText("max");
        this.cmdSetLimitMAX.addSelectionListener(this);

        // create Clear commands
        this.cmdClear = new Command(this.shell, Command.GENERAL, 0);
        this.cmdClear.setText("Clear");
        this.cmdClear.addSelectionListener(this);
    }

    /**
     * Create a QueryDialog with defined parameters and open it.
     * @param type a type of a QueryDialog dialog to open
     */
    private void openDialog(int type)
    {
        try
        {
            QueryDialog dlg = new QueryDialog(this.shell, SWT.NONE, type);

            // set title
            if (this.titleField != null && !this.titleField.isDisposed())
            {
                if (!this.titleField.isEnabled() &&
                        "null".equals(this.titleField.getText()))
                {
                    dlg.setText(null);
                }
                else
                {
                    dlg.setText(this.titleField.getText());
                }
            }

            // set prompt and default
            String defaultValue = "";
            if (this.defaultField != null && !this.defaultField.isDisposed())
            {
                if (!this.defaultField.isEnabled() &&
                        "null".equals(this.defaultField.getText()))
                {
                    defaultValue = null;
                }
                else
                {
                    defaultValue = this.defaultField.getText();
                }
            }

            if (this.promptField != null && !this.promptField.isDisposed())
            {
                if (!this.promptField.isEnabled() &&
                        "null".equals(this.promptField.getText()))
                {
                    dlg.setPromptText(null, defaultValue);
                }
                else
                {
                    dlg.setPromptText(this.promptField.getText(), defaultValue);
                }
            }

            // set limit
            if (this.minField != null && !this.minField.isDisposed())
            {
                String value = this.minField.getText();
                if (value != null && !"".equals(value))
                {
                    dlg.setMinimum(Integer.parseInt(this.minField.getText()));
                }
            }
            if (this.maxField != null && !this.maxField.isDisposed())
            {
                String value = this.maxField.getText();
                if (value != null && !"".equals(value))
                {
                    dlg.setMaximum(Integer.parseInt(this.maxField.getText()));
                }
            }

            // open the dialog
            dlg.open();

        }
        catch (IllegalArgumentException ex1)
        {
            MessageBox msg = new MessageBox(this.shell, SWT.OK);
            msg.setMessage("Exception: " + ex1.getMessage());
            msg.open();
        }
        catch (SWTException ex2)
        {
            MessageBox msg = new MessageBox(this.shell, SWT.OK);
            msg.setMessage("Exception: " + ex2.getMessage());
            msg.open();
        }
        catch (Exception ex)
        {
            MessageBox msg = new MessageBox(this.shell, SWT.OK);
            msg.setMessage("Exception: " + ex.getMessage());
            msg.open();
        }
    }


    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {

        // create commands
        if (e.widget == this.cmdCreateSTANDARD)
        {
            this.openDialog(QueryDialog.STANDARD);
            return;
        }
        if (e.widget == this.cmdCreateNUMBER)
        {
            this.openDialog(QueryDialog.NUMERIC);
            return;
        }
        if (e.widget == this.cmdCreateTIME)
        {
            this.openDialog(QueryDialog.TIME);
            return;
        }
        if (e.widget == this.cmdCreateDATE)
        {
            this.openDialog(QueryDialog.DATE);
            return;
        }
        if (e.widget == this.cmdCreatePASSWORD)
        {
            this.openDialog(QueryDialog.PASSWORD);
            return;
        }

        // set commands
        if (e.widget == this.cmdSetTitleDEFINE)
        {
            // check does the field already exist
            if (this.titleField == null || this.titleField.isDisposed())
            {
                // create title field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Title: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.titleField = new Text(this.shell, SWT.BORDER);
                this.titleField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));

            }
            // clear and enable the field
            this.titleField.setText("");
            this.titleField.setEnabled(true);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetTitleNULL)
        {
            if (this.titleField == null || this.titleField.isDisposed())
            {
                // create title field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Title: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.titleField = new Text(this.shell, SWT.BORDER);
                this.titleField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            this.titleField.setText("null");
            // disable the field
            this.titleField.setEnabled(false);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetPromptDEFINE)
        {
            // check does the field already exist
            if (this.promptField == null || this.promptField.isDisposed())
            {
                // create prompt field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Prompt: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.promptField = new Text(this.shell, SWT.BORDER);
                this.promptField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            // clear and enable the field
            this.promptField.setText("");
            this.promptField.setEnabled(true);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetPromptNULL)
        {
            if (this.promptField == null || this.promptField.isDisposed())
            {
                // create prompt field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Prompt: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.promptField = new Text(this.shell, SWT.BORDER);
                this.promptField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            this.promptField.setText("null");
            // disable the field
            this.promptField.setEnabled(false);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetDefaultDEFINE)
        {
            // check does the field already exist
            if (this.defaultField == null || this.defaultField.isDisposed())
            {
                // create default field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Default value: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.defaultField = new Text(this.shell, SWT.BORDER);
                this.defaultField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            // clear and enable the field
            this.defaultField.setText("");
            this.defaultField.setEnabled(true);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetDefaultNULL)
        {
            if (this.defaultField == null || this.defaultField.isDisposed())
            {
                // create default field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Default value: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.defaultField = new Text(this.shell, SWT.BORDER);
                this.defaultField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            this.defaultField.setText("null");
            // disable the field
            this.defaultField.setEnabled(false);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetLimitMIN)
        {
            if (this.minField == null || this.minField.isDisposed())
            {
                // create min field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Min length: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.minField = new ConstrainedText(this.shell,
                                                    SWT.BORDER,
                                                    ConstrainedText.NUMERIC);
                this.minField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetLimitMAX)
        {
            if (this.maxField == null || this.maxField.isDisposed())
            {
                // create max field
                Label label = new Label(this.shell, SWT.NONE);
                label.setText("Max length: ");
                label.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));

                this.maxField = new ConstrainedText(this.shell,
                                                    SWT.BORDER,
                                                    ConstrainedText.NUMERIC);
                this.maxField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }

        // clear command
        if (e.widget == this.cmdClear)
        {
            // dispose all config field (clear the screen)
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                if (controls[i] != null && !controls[i].isDisposed())
                    controls[i].dispose();
            }
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }

        // Exit command handling
        super.widgetSelected(e);
    }
}
