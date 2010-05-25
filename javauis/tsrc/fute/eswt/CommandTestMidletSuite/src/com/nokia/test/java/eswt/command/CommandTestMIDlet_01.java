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

package com.nokia.test.java.eswt.command;

import java.util.Vector;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Button;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 */
public class CommandTestMIDlet_01 extends TestBaseMIDlet
{

    /** Currently created custom commands */
    private Vector commands = new Vector();

    /** Settings fields */
    private Label nameLabel               = null;
    private Text nameField                = null;
    private Label priorityLabel           = null;
    private ConstrainedText priorityField = null;
    private Button button                   = null;
    /** Currently selected Command and an info label for it */
    private Command current = null;
    private Label currentValue = null;

    /** Add commands */
    private Command cmdAddCOMMANDGROUP = null;

    private Command cmdAddOK        = null;
    private Command cmdAddSELECT    = null;
    private Command cmdAddGENERAL   = null;
    private Command cmdAddDELETE    = null;
    private Command cmdAddHELP      = null;

    private Command cmdAddSTOP      = null;
    private Command cmdAddCANCEL    = null;
    private Command cmdAddBACK      = null;
    private Command cmdAddEXIT      = null;

    /** Remove commands */
    private Command cmdRemove        = null;
    private Command cmdRemoveCurrent = null;
    private Command cmdRemoveAll     = null;

    /** Params commands */
    private Command cmdParamName        = null;
    private Command cmdParamPriority    = null;
    private Command cmdParamButton      = null;
    private Command cmdParamsClear      = null;

    /** Edit commands */
    private Command cmdEdit         = null;
    private Command cmdSetDefault   = null;
    private Command cmdDisable      = null;
    private Command cmdEnable       = null;

    private Command cmdEnableText   = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        this.shell.setText("Command Test");
        this.shell.setLayout(new GridLayout(2, true));

        // create current fields
        Label currentLabel = new Label(this.shell, SWT.NONE);
        currentLabel.setText("Current: ");
        currentLabel.setLayoutData(
            new GridData(SWT.RIGHT, SWT.CENTER, false, false));

        this.currentValue = new Label(this.shell, SWT.NONE);
        this.currentValue.setText("none");
        this.currentValue.setLayoutData(
            new GridData(SWT.LEFT, SWT.CENTER, true, false));

        // create commands
        // Add commands
        Command cmdAdd = new Command(this.shell, Command.COMMANDGROUP, 5);
        cmdAdd.setText("Add");

        this.cmdAddOK = new Command(cmdAdd, Command.GENERAL, 9);
        this.cmdAddOK.setText("OK");
        this.cmdAddOK.addSelectionListener(this);

        this.cmdAddSELECT = new Command(cmdAdd, Command.GENERAL, 8);
        this.cmdAddSELECT.setText("SELECT");
        this.cmdAddSELECT.addSelectionListener(this);

        this.cmdAddGENERAL = new Command(cmdAdd, Command.GENERAL, 7);
        this.cmdAddGENERAL.setText("GENERAL");
        this.cmdAddGENERAL.addSelectionListener(this);

        this.cmdAddDELETE = new Command(cmdAdd, Command.GENERAL, 6);
        this.cmdAddDELETE.setText("DELETE");
        this.cmdAddDELETE.addSelectionListener(this);

        this.cmdAddHELP = new Command(cmdAdd, Command.GENERAL, 5);
        this.cmdAddHELP.setText("HELP");
        this.cmdAddHELP.addSelectionListener(this);

        this.cmdAddSTOP = new Command(cmdAdd, Command.GENERAL, 4);
        this.cmdAddSTOP.setText("STOP");
        this.cmdAddSTOP.addSelectionListener(this);

        this.cmdAddCANCEL = new Command(cmdAdd, Command.GENERAL, 3);
        this.cmdAddCANCEL.setText("CANCEL");
        this.cmdAddCANCEL.addSelectionListener(this);

        this.cmdAddBACK = new Command(cmdAdd, Command.GENERAL, 2);
        this.cmdAddBACK.setText("BACK");
        this.cmdAddBACK.addSelectionListener(this);

        this.cmdAddEXIT = new Command(cmdAdd, Command.GENERAL, 1);
        this.cmdAddEXIT.setText("EXIT");
        this.cmdAddEXIT.addSelectionListener(this);

        this.cmdAddCOMMANDGROUP = new Command(cmdAdd, Command.GENERAL, 0);
        this.cmdAddCOMMANDGROUP.setText("COMMANDGROUP");
        this.cmdAddCOMMANDGROUP.addSelectionListener(this);

        // Remove commands
        this.cmdRemove = new Command(this.shell, Command.COMMANDGROUP, 4);
        this.cmdRemove.setText("Remove");
        this.cmdRemove.setEnabled(false);

        this.cmdRemoveCurrent = new Command(cmdRemove, Command.GENERAL, 1);
        this.cmdRemoveCurrent.setText("current");
        this.cmdRemoveCurrent.addSelectionListener(this);
        this.cmdRemoveCurrent.setEnabled(false);

        this.cmdRemoveAll = new Command(cmdRemove, Command.GENERAL, 0);
        this.cmdRemoveAll.setText("all");
        this.cmdRemoveAll.addSelectionListener(this);

        // Edit commands
        this.cmdEdit = new Command(this.shell, Command.COMMANDGROUP, 1);
        this.cmdEdit.setText("Edit");
        this.cmdEdit.setEnabled(false);

        this.cmdSetDefault = new Command(this.cmdEdit, Command.GENERAL, 2);
        this.cmdSetDefault.setText("Set Default");
        this.cmdSetDefault.addSelectionListener(this);

        this.cmdDisable = new Command(this.cmdEdit, Command.GENERAL, 1);
        this.cmdDisable.setText("Disable");
        this.cmdDisable.addSelectionListener(this);

        this.cmdEnable = new Command(this.cmdEdit, Command.GENERAL, 0);
        this.cmdEnable.setText("Enable All");
        this.cmdEnable.addSelectionListener(this);
        this.cmdEnable.setEnabled(false);

        this.cmdEnableText = new Command(this.cmdEdit, Command.GENERAL, 0);
        this.cmdEnableText.setText("Switch Text State");
        this.cmdEnableText.addSelectionListener(this);


        // Params commands
        Command cmdParams = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdParams.setText("Params");

        this.cmdParamName = new Command(cmdParams, Command.GENERAL, 2);
        this.cmdParamName.setText("Name");
        this.cmdParamName.addSelectionListener(this);

        this.cmdParamPriority = new Command(cmdParams, Command.GENERAL, 1);
        this.cmdParamPriority.setText("Priority");
        this.cmdParamPriority.addSelectionListener(this);

        this.cmdParamButton = new Command(cmdParams, Command.GENERAL, 1);
        this.cmdParamButton.setText("Set Button");
        this.cmdParamButton.addSelectionListener(this);

        this.cmdParamsClear = new Command(cmdParams, Command.GENERAL, 0);
        this.cmdParamsClear.setText("Clear");
        this.cmdParamsClear.addSelectionListener(this);
        this.cmdParamsClear.setEnabled(false);

        // update the shell
        this.shell.layout();
        this.shell.update();
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {

        // Settings commands
        if (event.widget == this.cmdParamName)
        {
            if (this.nameLabel == null || this.nameLabel.isDisposed())
            {
                this.nameLabel = new Label(this.shell, SWT.None);
                this.nameLabel.setText("Name: ");
                this.nameLabel.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, false, false));
            }
            if (this.nameField == null || this.nameField.isDisposed())
            {
                this.nameField = new Text(this.shell, SWT.BORDER);
                this.nameField.setText("Command");
                this.nameField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            this.cmdParamsClear.setEnabled(true);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdParamPriority)
        {
            if (this.priorityLabel == null || this.priorityLabel.isDisposed())
            {
                this.priorityLabel = new Label(this.shell, SWT.None);
                this.priorityLabel.setText("Priority: ");
                this.priorityLabel.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, false, false));
            }
            if (this.priorityField == null || this.priorityField.isDisposed())
            {
                this.priorityField = new ConstrainedText(this.shell,
                        SWT.BORDER, ConstrainedText.NUMERIC);

                this.priorityField.setText("0");
                this.priorityField.setLayoutData(
                    new GridData(SWT.LEFT, SWT.CENTER, true, false));
            }
            this.cmdParamsClear.setEnabled(true);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdParamButton)
        {

            if (this.button == null || this.button.isDisposed())
            {
                this.button = new Button(this.shell,SWT.BORDER);
                this.button.setText("Command");

                this.button.setLayoutData(
                    new GridData(SWT.RIGHT, SWT.CENTER, true, false));
            }
            this.cmdParamsClear.setEnabled(true);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdParamsClear)
        {
            this.clearSettings();

            this.shell.layout();
            this.shell.update();
            return;
        }

        // Add commands
        if (event.widget == this.cmdAddCOMMANDGROUP)
        {
            this.addCommand(Command.COMMANDGROUP);
            return;
        }
        if (event.widget == this.cmdAddOK)
        {
            this.addCommand(Command.OK);
            return;
        }
        if (event.widget == this.cmdAddSELECT)
        {
            this.addCommand(Command.SELECT);
            return;
        }
        if (event.widget == this.cmdAddGENERAL)
        {
            this.addCommand(Command.GENERAL);
            return;
        }
        if (event.widget == this.cmdAddDELETE)
        {
            this.addCommand(Command.DELETE);
            return;
        }
        if (event.widget == this.cmdAddHELP)
        {
            this.addCommand(Command.HELP);
            return;
        }
        if (event.widget == this.cmdAddSTOP)
        {
            this.setExit(false);
            this.addCommand(Command.STOP);
            return;
        }
        if (event.widget == this.cmdAddCANCEL)
        {
            this.setExit(false);
            this.addCommand(Command.CANCEL);
            return;
        }
        if (event.widget == this.cmdAddBACK)
        {
            this.setExit(false);
            this.addCommand(Command.BACK);
            return;
        }
        if (event.widget == this.cmdAddEXIT)
        {
            this.setExit(false);
            this.addCommand(Command.EXIT);
            return;
        }

        // Remove commands
        if (event.widget == this.cmdRemoveCurrent)
        {
            this.removeCommand(this.current);
            this.current = null;
            this.currentValue.setText("none");

            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdRemoveAll)
        {
            while (this.commands.size()>0)
            {
                Command cmd = (Command) this.commands.elementAt(0);
                this.removeCommand(cmd);
            }
            this.current = null;
            this.currentValue.setText("none");

            this.shell.layout();
            this.shell.update();
            return;
        }

        // Edit commands
        if (event.widget == this.cmdSetDefault)
        {
            if (this.current != null && !this.current.isDisposed())
            {
                this.current.setDefaultCommand();
            }
            return;
        }
        if (event.widget == this.cmdDisable)
        {
            if (this.current != null && !this.current.isDisposed())
            {
                this.current.setEnabled(false);
                this.current = null;
                this.currentValue.setText("none");

                this.cmdEnable.setEnabled(true);
                this.cmdSetDefault.setEnabled(false);
                this.cmdDisable.setEnabled(false);
                this.cmdRemoveCurrent.setEnabled(false);

                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdEnable)
        {
            for (int i=0; i<this.commands.size(); i++)
            {
                ((Command)this.commands.elementAt(i)).setEnabled(true);
            }
            this.cmdEnable.setEnabled(false);
            if (this.current == null)
            {
                this.cmdEdit.setEnabled(false);
            }
            return;
        }
        if (event.widget == this.cmdEnableText)
        {
            Control[] controls = this.shell.getChildren();

            for (int i=0; i<controls.length; i++)
            {
                if (controls[i] instanceof Text)
                {
                    controls[i].setEnabled(!controls[i].getEnabled());
                }
            }

            return;
        }

        // Custom commands (set to current)
        if (event.widget instanceof Command && event.widget != this.cmdExit)
        {
            this.setCurrent((Command) event.widget);
            return;
        }

        super.widgetSelected(event);
    }

    /**
     * Create new command of specified type and add it to the Options menu.
     * @param type a type of a command to be added
     * @return created and added command
     */
    private Command addCommand(int type)
    {
        Command cmd = null;
        // take priority
        int priority = 0;
        if (this.priorityField != null && !this.priorityField.isDisposed())
        {
            priority = Integer.parseInt(this.priorityField.getText());
        }
        // take name
        String name = null;
        if (this.nameField != null && !this.nameField.isDisposed())
        {
            name = this.nameField.getText();
        }

        cmd = new Command(this.shell, type, priority);
        if (name != null)
        {
            cmd.setText(name);
        }

        cmd.addSelectionListener(this);
        this.commands.addElement(cmd);
        this.cmdRemove.setEnabled(true);

        return cmd;
    }

    /**
     * Set a custom command to currently selected.
     * @param cmd currently selected custom command
     */
    private void setCurrent(Command cmd)
    {
        this.current = cmd;
        this.currentValue.setText(this.current.getText());
        this.currentValue.pack();

        this.cmdEdit.setEnabled(true);
        this.cmdSetDefault.setEnabled(true);
        this.cmdDisable.setEnabled(true);
        this.cmdRemoveCurrent.setEnabled(true);

        this.shell.layout();
        this.shell.update();
    }

    /**
     * Remove specified custom command.
     * @param cmd command to be removed
     */
    private void removeCommand(Command cmd)
    {
        if (this.commands.removeElement(cmd))
        {
            cmd.dispose();
            this.cmdEdit.setEnabled(false);
            this.cmdRemoveCurrent.setEnabled(false);
        }
        if (this.commands.size() == 0)
        {
            this.cmdRemove.setEnabled(false);
        }
    }

    /**
     * Dispose the settings field currently presented on the screen.
     */
    private void clearSettings()
    {
        if (this.nameLabel != null && !this.nameLabel.isDisposed())
            this.nameLabel.dispose();
        if (this.nameField != null && !this.nameField.isDisposed())
            this.nameField.dispose();

        if (this.priorityLabel != null && !this.priorityLabel.isDisposed())
            this.priorityLabel.dispose();
        if (this.priorityField != null && !this.priorityField.isDisposed())
            this.priorityField.dispose();

        this.cmdParamsClear.setEnabled(false);
    }
}
