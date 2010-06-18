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
package com.nokia.test.java.eswt.layout;

import java.util.Date;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.DateEditor;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class LayoutTestMIDlet_01 extends TestBaseMIDlet
{

    /** Commands */
    /** Add/Remove control commands */
    private Command cmdAddLABEL         = null;
    private Command cmdAddTEXT          = null;
    private Command cmdAddDATE          = null;

    private Command cmdRemove           = null;
    private Command cmdRemoveCURRENT    = null;
    private Command cmdRemoveFIRST      = null;
    private Command cmdRemoveLAST       = null;
    private Command cmdRemoveALL        = null;

    /** Layout commands */
    private Command cmdFillLayout   = null;
    private Command cmdRowLayout    = null;
    private Command cmdGridLayout   = null;

    protected void init()
    {
        // init the shell
        this.shell.setText("Layout Test");
        this.shell.setLayout(new RowLayout());

        // create commands
        // Control commands
        Command cmdControl = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdControl.setText("Control");

        Command cmdAdd = new Command(cmdControl, Command.COMMANDGROUP, 1);
        cmdAdd.setText("Add");

        this.cmdAddLABEL = new Command(cmdAdd, Command.GENERAL, 2);
        this.cmdAddLABEL.setText("Label");
        this.cmdAddLABEL.addSelectionListener(this);

        this.cmdAddTEXT = new Command(cmdAdd, Command.GENERAL, 1);
        this.cmdAddTEXT.setText("Text");
        this.cmdAddTEXT.addSelectionListener(this);

        this.cmdAddDATE = new Command(cmdAdd, Command.GENERAL, 0);
        this.cmdAddDATE.setText("Date Editor");
        this.cmdAddDATE.addSelectionListener(this);

        this.cmdRemove = new Command(cmdControl, Command.COMMANDGROUP, 0);
        this.cmdRemove.setText("Remove");
        this.cmdRemove.setEnabled(false);

        this.cmdRemoveCURRENT = new Command(cmdRemove, Command.GENERAL, 3);
        this.cmdRemoveCURRENT.setText("current");
        this.cmdRemoveCURRENT.addSelectionListener(this);

        this.cmdRemoveFIRST = new Command(cmdRemove, Command.GENERAL, 2);
        this.cmdRemoveFIRST.setText("first");
        this.cmdRemoveFIRST.addSelectionListener(this);

        this.cmdRemoveLAST = new Command(cmdRemove, Command.GENERAL, 1);
        this.cmdRemoveLAST.setText("last");
        this.cmdRemoveLAST.addSelectionListener(this);

        this.cmdRemoveALL = new Command(cmdRemove, Command.GENERAL, 0);
        this.cmdRemoveALL.setText("all");
        this.cmdRemoveALL.addSelectionListener(this);

        // Layout commands
        Command cmdLayout = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdLayout.setText("Layout");

        this.cmdFillLayout = new Command(cmdLayout, Command.GENERAL, 2);
        this.cmdFillLayout.setText("Fill Layout");
        this.cmdFillLayout.addSelectionListener(this);

        this.cmdRowLayout = new Command(cmdLayout, Command.GENERAL, 1);
        this.cmdRowLayout.setText("Row Layout");
        this.cmdRowLayout.addSelectionListener(this);

        this.cmdGridLayout = new Command(cmdLayout, Command.GENERAL, 0);
        this.cmdGridLayout.setText("Grid Layout");
        this.cmdGridLayout.addSelectionListener(this);
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {

        // Control commands
        // Add commands
        if (event.widget == this.cmdAddLABEL)
        {
            Label label = new Label(this.shell, SWT.NONE);
            label.setText("Label");
            this.cmdRemove.setEnabled(true);

            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdAddTEXT)
        {
            Text text = new Text(this.shell, SWT.BORDER);
            text.setText("Text");
            this.cmdRemove.setEnabled(true);

            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdAddDATE)
        {
            DateEditor date = new DateEditor(this.shell, SWT.BORDER);
            date.setDate(new Date());
            this.cmdRemove.setEnabled(true);

            this.shell.layout();
            this.shell.update();
            return;
        }
        // Remove commands
        if (event.widget == this.cmdRemoveCURRENT)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                if (controls[i].isFocusControl())
                {
                    controls[i].dispose();
                    this.shell.layout();
                    this.shell.update();
                    if (this.shell.getChildren().length == 0)
                    {
                        this.cmdRemove.setEnabled(false);
                    }
                    break;
                }
            }
            return;
        }
        if (event.widget == this.cmdRemoveFIRST)
        {
            Control[] controls = this.shell.getChildren();
            if (controls.length > 0)
            {
                controls[0].dispose();
                this.shell.layout();
                this.shell.update();
                if (this.shell.getChildren().length == 0)
                {
                    this.cmdRemove.setEnabled(false);
                }
            }
            return;
        }
        if (event.widget == this.cmdRemoveLAST)
        {
            Control[] controls = this.shell.getChildren();
            int length = controls.length;
            if (length > 0)
            {
                controls[length-1].dispose();
                this.shell.layout();
                this.shell.update();
                if (this.shell.getChildren().length == 0)
                {
                    this.cmdRemove.setEnabled(false);
                }
            }
            return;
        }
        if (event.widget == this.cmdRemoveALL)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].dispose();
            }
            this.shell.layout();
            this.shell.update();
            if (this.shell.getChildren().length == 0)
            {
                this.cmdRemove.setEnabled(false);
            }
            return;
        }

        // Layout commands
        if (event.widget == this.cmdFillLayout)
        {
            this.shell.setLayout(new FillLayout());
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdRowLayout)
        {
            this.shell.setLayout(new RowLayout());
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdGridLayout)
        {
            this.shell.setLayout(new GridLayout());
            this.shell.layout();
            this.shell.update();
            return;
        }

        super.widgetSelected(event);
    }
}
