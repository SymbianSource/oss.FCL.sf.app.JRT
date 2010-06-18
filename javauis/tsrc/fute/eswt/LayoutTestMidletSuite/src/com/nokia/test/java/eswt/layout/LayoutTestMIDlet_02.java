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
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * The MIDlet is used to examine different aspects of GridLayout.
 *
 */
public class LayoutTestMIDlet_02 extends TestBaseMIDlet
{

    /** GridLayout object */
    private GridLayout layout = null;

    /** Commands */
    /** Add\Remove control commands */
    private Command cmdAddLABEL         = null;
    private Command cmdAddTEXT          = null;
    private Command cmdAddDATE          = null;
    private Command cmdRemoveAll        = null;

    /** Settings commands */
    private Command cmdNumberOfColumns      = null;
    private Command cmdMakeColsEqualTRUE    = null;
    private Command cmdMakeColsEqualFALSE   = null;

    private Command cmdSpacingHORIZONTAL    = null;
    private Command cmdSpacingVERTICAL      = null;

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
        this.shell.setText("Grid Layout Test");
        // create and set the layout
        this.layout = new GridLayout();
        this.shell.setLayout(this.layout);

        // create commands
        // Add control commands
        Command cmdControl = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdControl.setText("Control");

        this.cmdRemoveAll = new Command(cmdControl, Command.GENERAL, 0);
        this.cmdRemoveAll.setText("Remove All");
        this.cmdRemoveAll.addSelectionListener(this);

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

        // Settings command
        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSettings.setText("Settings");

        this.cmdNumberOfColumns = new Command(cmdSettings, Command.GENERAL, 3);
        this.cmdNumberOfColumns.setText("Number of Columns");
        this.cmdNumberOfColumns.addSelectionListener(this);

        Command cmdMakeColsEqual =
            new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdMakeColsEqual.setText("makeColumnsEqualWidth");

        this.cmdMakeColsEqualTRUE =
            new Command(cmdMakeColsEqual, Command.GENERAL, 1);
        this.cmdMakeColsEqualTRUE.setText("true");
        this.cmdMakeColsEqualTRUE.addSelectionListener(this);

        this.cmdMakeColsEqualFALSE =
            new Command(cmdMakeColsEqual, Command.GENERAL, 0);
        this.cmdMakeColsEqualFALSE.setText("false");
        this.cmdMakeColsEqualFALSE.addSelectionListener(this);

        Command cmdSpacing = new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdSpacing.setText("Spacing");

        this.cmdSpacingHORIZONTAL = new Command(cmdSpacing, Command.GENERAL, 1);
        this.cmdSpacingHORIZONTAL.setText("HORIZONTAL");
        this.cmdSpacingHORIZONTAL.addSelectionListener(this);

        this.cmdSpacingVERTICAL = new Command(cmdSpacing, Command.GENERAL, 0);
        this.cmdSpacingVERTICAL.setText("VERTICAL");
        this.cmdSpacingVERTICAL.addSelectionListener(this);

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
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdAddTEXT)
        {
            Text text = new Text(this.shell, SWT.BORDER);
            text.setText("Text");
            this.setLayoutData(text);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdAddDATE)
        {
            DateEditor date = new DateEditor(this.shell, SWT.BORDER);
            date.setDate(new Date());
            this.setLayoutData(date);
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Remove command
        if (event.widget == this.cmdRemoveAll)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
                controls[i].dispose();

            this.shell.update();
            return;
        }

        // Settings commands
        if (event.widget == this.cmdNumberOfColumns)
        {
            int nOfCols = this.layout.numColumns;
            nOfCols = doNumberPrompt("Number of columns:", nOfCols);

            if (nOfCols != this.layout.numColumns)
            {
                this.layout.numColumns = nOfCols;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdMakeColsEqualTRUE)
        {
            this.layout.makeColumnsEqualWidth = true;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdMakeColsEqualFALSE)
        {
            this.layout.makeColumnsEqualWidth = false;
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdSpacingHORIZONTAL)
        {
            int spacing = this.layout.horizontalSpacing;
            spacing = this.doNumberPrompt("Horizontal spacing:", spacing);

            if (spacing != this.layout.horizontalSpacing)
            {
                this.layout.horizontalSpacing = spacing;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (event.widget == this.cmdSpacingVERTICAL)
        {
            int spacing = this.layout.verticalSpacing;
            spacing = this.doNumberPrompt("Vertical spacing:", spacing);

            if (spacing != this.layout.verticalSpacing)
            {
                this.layout.verticalSpacing = spacing;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
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
     * Prompt a user for layout data (GridData).
     * First, the user is asked does he want to set a layout data. If he does,
     * a GridDataConfig dialog is shown. After the user specifies
     * all required values and clicks OK, a GridData object is created and
     * set to the specified control.
     * @param control a Control to which created GridData is set
     */
    private void setLayoutData(Control control)
    {
        // prompt a user does he want to set GridData for a control
        MessageBox msg =
            new MessageBox(this.shell, SWT.ICON_QUESTION | SWT.YES | SWT.NO);
        msg.setMessage("Would you like to set a layout data for the control?");
        if (msg.open() == SWT.NO)
        {
            return;
        }
        // create GridDataConfig dialog
        GridDataConfigDialog dlg = new GridDataConfigDialog(this.shell);
        GridData data = dlg.open();
        if (data != null)
        {
            control.setLayoutData(data);
        }
    }

}