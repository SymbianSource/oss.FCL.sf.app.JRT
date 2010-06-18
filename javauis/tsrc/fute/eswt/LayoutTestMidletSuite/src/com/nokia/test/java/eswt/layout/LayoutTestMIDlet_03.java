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
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * The MIDlet is used to examine different aspects of RowLayout.
 *
 */
public class LayoutTestMIDlet_03 extends TestBaseMIDlet
{
    /** RowLayout object */
    private RowLayout layout = null;

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
    private Command cmdPackTRUE         = null;
    private Command cmdPackFALSE        = null;
    private Command cmdFillTRUE         = null;
    private Command cmdFillFALSE        = null;
    private Command cmdJustifyTRUE      = null;
    private Command cmdJustifyFALSE     = null;
    private Command cmdWrapTRUE         = null;
    private Command cmdWrapFALSE        = null;
    private Command cmdSpacing          = null;

    private Command cmdMarginTOP            = null;
    private Command cmdMarginBOTTOM         = null;
    private Command cmdMarginLEFT           = null;
    private Command cmdMarginRIGHT          = null;
    private Command cmdMarginWIDTH          = null;
    private Command cmdMarginHEIGHT         = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        this.shell.setText("Row Layout Test");
        // create and set the layout
        this.layout = new RowLayout();
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
        Command cmdType = new Command(cmdSettings, Command.COMMANDGROUP, 6);
        cmdType.setText("Type");

        this.cmdTypeHORIZONTAL = new Command(cmdType, Command.GENERAL, 1);
        this.cmdTypeHORIZONTAL.setText("HORIZONTAL");
        this.cmdTypeHORIZONTAL.addSelectionListener(this);

        this.cmdTypeVERTICAL = new Command(cmdType, Command.GENERAL, 0);
        this.cmdTypeVERTICAL.setText("VERTICAL");
        this.cmdTypeVERTICAL.addSelectionListener(this);

        // pack
        Command cmdPack = new Command(cmdSettings, Command.COMMANDGROUP, 5);
        cmdPack.setText("Pack");

        this.cmdPackTRUE = new Command(cmdPack, Command.GENERAL, 1);
        this.cmdPackTRUE.setText("true");
        this.cmdPackTRUE.addSelectionListener(this);

        this.cmdPackFALSE = new Command(cmdPack, Command.GENERAL, 0);
        this.cmdPackFALSE.setText("false");
        this.cmdPackFALSE.addSelectionListener(this);

        // fill
        Command cmdFill = new Command(cmdSettings, Command.COMMANDGROUP, 4);
        cmdFill.setText("Fill");

        this.cmdFillTRUE = new Command(cmdFill, Command.GENERAL, 1);
        this.cmdFillTRUE.setText("true");
        this.cmdFillTRUE.addSelectionListener(this);

        this.cmdFillFALSE = new Command(cmdFill, Command.GENERAL, 0);
        this.cmdFillFALSE.setText("false");
        this.cmdFillFALSE.addSelectionListener(this);

        // justify
        Command cmdJustify = new Command(cmdSettings, Command.COMMANDGROUP, 3);
        cmdJustify.setText("Justify");

        this.cmdJustifyTRUE = new Command(cmdJustify, Command.GENERAL, 1);
        this.cmdJustifyTRUE.setText("true");
        this.cmdJustifyTRUE.addSelectionListener(this);

        this.cmdJustifyFALSE = new Command(cmdJustify, Command.GENERAL, 0);
        this.cmdJustifyFALSE.setText("false");
        this.cmdJustifyFALSE.addSelectionListener(this);

        // wrap
        Command cmdWrap = new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdWrap.setText("Wrap");

        this.cmdWrapTRUE = new Command(cmdWrap, Command.GENERAL, 1);
        this.cmdWrapTRUE.setText("true");
        this.cmdWrapTRUE.addSelectionListener(this);

        this.cmdWrapFALSE = new Command(cmdWrap, Command.GENERAL, 0);
        this.cmdWrapFALSE.setText("false");
        this.cmdWrapFALSE.addSelectionListener(this);

        // spacing
        this.cmdSpacing = new Command(cmdSettings, Command.GENERAL, 1);
        this.cmdSpacing.setText("Spacing");
        this.cmdSpacing.addSelectionListener(this);

        // margin
        Command cmdMargin = new Command(cmdSettings, Command.COMMANDGROUP, 0);
        cmdMargin.setText("Margin");

        this.cmdMarginTOP = new Command(cmdMargin, Command.GENERAL, 5);
        this.cmdMarginTOP.setText("TOP");
        this.cmdMarginTOP.addSelectionListener(this);

        this.cmdMarginBOTTOM = new Command(cmdMargin, Command.GENERAL, 4);
        this.cmdMarginBOTTOM.setText("BOTTOM");
        this.cmdMarginBOTTOM.addSelectionListener(this);

        this.cmdMarginLEFT = new Command(cmdMargin, Command.GENERAL, 3);
        this.cmdMarginLEFT.setText("LEFT");
        this.cmdMarginLEFT.addSelectionListener(this);

        this.cmdMarginRIGHT = new Command(cmdMargin, Command.GENERAL, 2);
        this.cmdMarginRIGHT.setText("RIGHT");
        this.cmdMarginRIGHT.addSelectionListener(this);

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
            this.setLayoutData(label);
            this.cmdRemove.setEnabled(true);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdAddTEXT)
        {
            Text text = new Text(this.shell, SWT.BORDER);
            text.setText("Text");
            this.setLayoutData(text);
            this.cmdRemove.setEnabled(true);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdAddDATE)
        {
            DateEditor date = new DateEditor(this.shell, SWT.BORDER);
            date.setDate(new Date());
            this.setLayoutData(date);
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
        // pack
        if (event.widget == this.cmdPackTRUE)
        {
            this.layout.pack = true;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdPackFALSE)
        {
            this.layout.pack = false;
            this.shell.layout();
            this.shell.update();
            return;
        }
        // fill
        if (event.widget == this.cmdFillTRUE)
        {
            this.layout.fill = true;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdFillFALSE)
        {
            this.layout.fill = false;
            this.shell.layout();
            this.shell.update();
            return;
        }
        // justify
        if (event.widget == this.cmdJustifyTRUE)
        {
            this.layout.justify = true;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdJustifyFALSE)
        {
            this.layout.justify = false;
            this.shell.layout();
            this.shell.update();
            return;
        }
        // wrap
        if (event.widget == this.cmdWrapTRUE)
        {
            this.layout.wrap = true;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdWrapFALSE)
        {
            this.layout.wrap = false;
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
        if (event.widget == this.cmdMarginTOP)
        {
            int margin = this.layout.marginTop;
            margin = this.doNumberPrompt("Top margin:", margin);

            if (margin != this.layout.marginTop)
            {
                this.layout.marginTop = margin;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdMarginBOTTOM)
        {
            int margin = this.layout.marginBottom;
            margin = this.doNumberPrompt("Bottom margin:", margin);

            if (margin != this.layout.marginBottom)
            {
                this.layout.marginBottom = margin;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdMarginLEFT)
        {
            int margin = this.layout.marginLeft;
            margin = this.doNumberPrompt("Left margin:", margin);

            if (margin != this.layout.marginLeft)
            {
                this.layout.marginLeft = margin;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdMarginRIGHT)
        {
            int margin = this.layout.marginRight;
            margin = this.doNumberPrompt("Right margin:", margin);

            if (margin != this.layout.marginRight)
            {
                this.layout.marginRight = margin;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
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

    /**
     * Prompt a user for layout data (RowData).
     * First, the user is asked does he want to set a layout data. If he does,
     * he is promted for width and height values. A RowData object is created
     * then and set to the specified control.
     * @param control a Control to which created RowData is set
     */
    private void setLayoutData(Control control)
    {
        // prompt a user does he want to set GridData for a control
        MessageBox msg =
            new MessageBox(this.shell, SWT.ICON_QUESTION | SWT.YES | SWT.NO);
        msg.setMessage("Would you like to set layout data for the control?");

        if (msg.open() == SWT.NO)
        {
            return;
        }

        // create layout data
        RowData data = new RowData();

        // prompt user for width and height
        int width = this.doNumberPrompt("Width:", data.width);
        data.width = width;
        int height = this.doNumberPrompt("Height:", data.height);
        data.height = height;

        // set layout data
        control.setLayoutData(data);
    }
}
