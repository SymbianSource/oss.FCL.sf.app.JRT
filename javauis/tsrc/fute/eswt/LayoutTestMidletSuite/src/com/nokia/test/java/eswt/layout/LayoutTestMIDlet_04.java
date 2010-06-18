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

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * The MIDlet is used to examine different aspects of FillLayout.
 */
public class LayoutTestMIDlet_04 extends TestBaseMIDlet
{
    /** FillLayout object */
    private FillLayout layout = null;

    /** Commands */
    /** Add\Remove control commands */
    private Command cmdAddLABEL         = null;
    private Command cmdAddTEXT          = null;
    private Command cmdAddDATE          = null;

    private Command cmdRemove           = null;
    private Command cmdRemoveCURRENT    = null;
    private Command cmdRemoveFIRST      = null;
    private Command cmdRemoveLAST       = null;
    private Command cmdRemoveALL        = null;

    /** Settings commands */
    private Command cmdTypeVERTICAL     = null;
    private Command cmdTypeHORIZONTAL   = null;
    private Command cmdSpacing          = null;

    private Command cmdMarginWIDTH          = null;
    private Command cmdMarginHEIGHT         = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        this.shell.setText("Fill Layout Test");
        // create and set the layout
        this.layout = new FillLayout();
        this.shell.setLayout(this.layout);

        // create commands
        // Add control commands
        Command cmdControl = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdControl.setText("Control");

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

        // Settings commands
        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSettings.setText("Settings");

        // type
        Command cmdType = new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdType.setText("Type");

        this.cmdTypeHORIZONTAL = new Command(cmdType, Command.GENERAL, 1);
        this.cmdTypeHORIZONTAL.setText("HORIZONTAL");
        this.cmdTypeHORIZONTAL.addSelectionListener(this);

        this.cmdTypeVERTICAL = new Command(cmdType, Command.GENERAL, 0);
        this.cmdTypeVERTICAL.setText("VERTICAL");
        this.cmdTypeVERTICAL.addSelectionListener(this);

        // spacing
        this.cmdSpacing = new Command(cmdSettings, Command.GENERAL, 1);
        this.cmdSpacing.setText("Spacing");
        this.cmdSpacing.addSelectionListener(this);

        // margin
        Command cmdMargin = new Command(cmdSettings, Command.COMMANDGROUP, 0);
        cmdMargin.setText("Margin");

        this.cmdMarginWIDTH = new Command(cmdMargin, Command.GENERAL, 1);
        this.cmdMarginWIDTH.setText("WIDTH");
        this.cmdMarginWIDTH.addSelectionListener(this);

        this.cmdMarginHEIGHT = new Command(cmdMargin, Command.GENERAL, 0);
        this.cmdMarginHEIGHT.setText("HEIGHT");
        this.cmdMarginHEIGHT.addSelectionListener(this);
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {

        // Add controls
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

        // Settings commands
        // type
        if (event.widget == this.cmdTypeHORIZONTAL)
        {
            this.layout.type = SWT.HORIZONTAL;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdTypeVERTICAL)
        {
            this.layout.type = SWT.VERTICAL;
            this.shell.layout();
            this.shell.update();
            return;
        }
        // spacing
        if (event.widget == this.cmdSpacing)
        {
            int spacing = this.layout.spacing;
            spacing = this.doNumberPrompt("Spacing:", spacing);

            if (spacing != this.layout.spacing)
            {
                this.layout.spacing = spacing;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        // margin
        if (event.widget == this.cmdMarginWIDTH)
        {
            int margin = this.layout.marginWidth;
            margin = this.doNumberPrompt("Margin width:", margin);

            if (margin != this.layout.marginWidth)
            {
                this.layout.marginWidth = margin;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdMarginHEIGHT)
        {
            int margin = this.layout.marginHeight;
            margin = this.doNumberPrompt("Margin height:", margin);

            if (margin != this.layout.marginHeight)
            {
                this.layout.marginHeight = margin;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }

        super.widgetSelected(event);
    }

    /**
     * Create and open a number QueryDialog with specified prompt and default
     * value. The result of dialog open is returned as int.
     * @param prompt a prompt message
     * @param value default value
     * @return result of dialog open or default value if input was canceled
     */
    private int doNumberPrompt(String prompt, int value)
    {
        int result = value;

        if (value < 0) value = 0;

        QueryDialog dlg =
            new QueryDialog(this.shell, SWT.NONE, QueryDialog.NUMERIC);

        dlg.setPromptText(prompt, String.valueOf(value));
        String svalue = dlg.open();
        if (svalue != null)
        {
            result = Integer.parseInt(svalue);
        }
        return result;
    }
}
